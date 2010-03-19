/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  IETF SIMPLE Protocol implementation for XIMP Framework
*
*/


#include <badesca.h>
#include <ecom/implementationproxy.h>
#include <ximpserviceinfo.h>
#include <ximpbase.h>
#include <ximpidentity.h>
#include <presenceinfo.h>
#include <ximpprotocolconnectionhost.h>
#include <ximpprotocolconnection.h>
#include <ximpserviceinfo.h>
#include <ximpcontextclientinfo.h>

#include "presenceplugin.h"
#include "presenceplugindef.h"
#include "presencepluginconnection.h"
#include "presenceplugincommon.h"
#include "presenceplugin_resource.hrh"
#include "presenceconnectioninfo.h"
 
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <spdefinitions.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Key value pair table to identify correct constructor
// function for the requested interface.
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    //Warning in Lint 611
    IMPLEMENTATION_PROXY_ENTRY( PRESENCEPLUGIN_1_IMPLEMENTATION_UID,
        CPresencePlugin::NewL )
    };

// ---------------------------------------------------------------------------
// Exported function to return the implementation proxy table
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) /
        sizeof( TImplementationProxy );
    return ImplementationTable;
    }
    
// ---------------------------------------------------------------------------
// CPresencePlugin::CPresencePlugin()
// ---------------------------------------------------------------------------
//
CPresencePlugin::CPresencePlugin()
    {
    //empty
    }
   
// ---------------------------------------------------------------------------
// CPresencePlugin::NewLC()
// ---------------------------------------------------------------------------
//
CPresencePlugin* CPresencePlugin::NewLC()
    {
    CPresencePlugin* self = new( ELeave ) CPresencePlugin();
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePlugin::NewL()
// ---------------------------------------------------------------------------
//
CPresencePlugin* CPresencePlugin::NewL()
    {
    CPresencePlugin* self = CPresencePlugin::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePlugin::~CPresencePlugin()
// ---------------------------------------------------------------------------
//
CPresencePlugin::~CPresencePlugin()
    {
    DP_SDA("CPresencePlugin::~CPresencePlugin");
    iConnections.ResetAndDestroy();
    iConnectionArray.ResetAndDestroy();
    }
    
// ---------------------------------------------------------------------------
// CPresencePlugin::~CPresencePlugin()
// ---------------------------------------------------------------------------
//   
void CPresencePlugin::PrimeHost( MXIMPProtocolPluginHost& aHost ) 
    {
    DP_SDA("CPresencePlugin::PrimeHost");
    iHost = &aHost;
    }
    
// ---------------------------------------------------------------------------
// CPresencePlugin::AcquireConnectionL()
// ---------------------------------------------------------------------------
//
MXIMPProtocolConnection& CPresencePlugin::AcquireConnectionL(
    const MXIMPServiceInfo& aServiceInfo,
    const MXIMPContextClientInfo& aContextClient )
    {
    DP_SDA("CPresencePlugin::AcquireConnectionL");
    MXIMPProtocolConnection* tmp = NULL;
    CPresenceConnectionInfo* connectionArray = NULL;
    TBool connected = EFalse;
    
    //Check if presence is allready connected to id
    //!!!! IN this adapter that IapID is service id ID!!!!
    
    DP_SDA2("CPresencePlugin::AcquireConnectionL - service id: %d", 
        aServiceInfo.IapId() );
    
    TInt presenceId = KErrNotFound;
    CSPSettings* spSettings = CSPSettings::NewL();
    CleanupStack::PushL( spSettings );
    
    CSPProperty* property = CSPProperty::NewLC();
    spSettings->FindPropertyL( aServiceInfo.IapId(),
                                ESubPropertyPresenceSettingsId,
                                *property );    
    if ( property )
        {
        property->GetValue( presenceId );
        }
    CleanupStack::PopAndDestroy( property );        
    CleanupStack::PopAndDestroy( spSettings );

    DP_SDA2(" -> presence settings id: %d",presenceId );
    __ASSERT_ALWAYS( presenceId > 0, User::Leave( KErrArgument ) );
    
    TInt connectionsCount = iConnections.Count();
    DP_SDA2("AcquireConnectionL : connectionsCount =%d ", connectionsCount );
    
    for( TInt i( connectionsCount - 1 ); i >= 0; i-- )
    	{
		DP_SDA2("AcquireConnectionL connection count %d", connectionsCount);
        if( iConnections[i]->GetPresenceSetId() == presenceId )
            {
            DP_SDA2("AcquireConnectionL RETURN OLD CONNECTION : i = %d " , i );
            //Check connection status
			tmp = iConnections[i];
			connectionArray = iConnections[i]->GetConnectionArray();
			
			//Check connection status from connectionArray
			CPresencePluginConnection::TConnectionStatus status =
				connectionArray->GetConnectionStatus();
			
			if( CPresencePluginConnection::ENotActive == status )
			    {
			    DP_SDA("AcquireConnectionL ENotActive ");
                DP_SDA("AcquireConnectionL Del old inactive connection");
                //Call session CLOSE here!!!!!
                //Ensure there is no any client to bind this connect
                //Release this failed connection
                DeleteConnection(i);
			    }
			else if ( CPresencePluginConnection::ETerminated == status )
                {
                DP_SDA("AcquireConnectionL ETerminated CREATE NEW CONNECTION ");
                //Roamning try use lates ETag
                //Delete old terminaded connection and create new with old ETag                             
                CPresencePluginConnection* connection =
                	CPresencePluginConnection::NewL(
                        aServiceInfo, aContextClient,
                        *iConnectionArray[i]->GetConnectionEtag() );
                CleanupStack::PushL( connection );
                
                DeleteConnection( i );
                
                //Add this new connection to connectionArray
                CPresenceConnectionInfo* connectionInfo =
                	CPresenceConnectionInfo::NewL( connection );
                iConnectionArray.Append( connectionInfo );
                connection->SetConnectionArray( connectionInfo );
                iConnections.AppendL( connection );
                CleanupStack::Pop( connection ); 
                tmp = connection;
                connected = ETrue;
                }
			else if ( CPresencePluginConnection::EActive == status )
			    {
			    DP_SDA("AcquireConnectionL EActive");
                // NOTE: Incase there is no reply from XIMP before client is
                // shutdown , then closeconnection wouldnt be called, reopening 
                // client will increase clientCount which is wrong. ClientCount 
                // should be checked for validity before increasing? 
                // If already connected
                iConnectionArray[i]->IncreaseClientCount();
                connected = ETrue;
                tmp = iConnections[i];
                DP_SDA("AcquireConnectionL RETURN OLD CONNECTION done");
			    }
			break;
            }
        }   
    if( !connected )
        {
        DP_SDA("CPresencePlugin::AcquireConnectionL CREATE NEW CONNECTION");
        // If not already connected 
        CPresencePluginConnection* connection = CPresencePluginConnection::NewL(
        	aServiceInfo, aContextClient );
        CleanupStack::PushL( connection );
        
        //Add this new connection to connectionArray
        CPresenceConnectionInfo* connectionInfo =
        	CPresenceConnectionInfo::NewL( connection );
        iConnectionArray.Append( connectionInfo );
        connection->SetConnectionArray( connectionInfo );
        //Add on binded client
        connectionInfo->IncreaseClientCount();
        
        iConnections.AppendL( connection );
        CleanupStack::Pop( connection ); 
    	tmp = connection;
    	DP_SDA("AcquireConnectionL CREATE NEW CONNECTION DONE"); 
        }
    DP_SDA("CPresencePlugin::AcquireConnectionL ret");    
	return *tmp;				
    }
    
// ---------------------------------------------------------------------------
// CPresencePlugin::ReleaseConnection()
// ---------------------------------------------------------------------------
//
void CPresencePlugin::ReleaseConnection( MXIMPProtocolConnection& aConnection )
    {
    DP_SDA("CPresencePlugin::ReleaseConnection");   
	TInt connectionsCount = iConnections.Count();
    
    DP_SDA2("ReleaseConnection : connectionsCount =%d ", connectionsCount );    
        
    for ( TInt i=0; i < connectionsCount; i++ )          
        {
        DP_SDA("CPresencePlugin::ReleaseConnection for start");
        if ( iConnectionArray[i]->GetConnection() == &aConnection )
            {
            //Check how many client is using this connection
            TInt clientCount = iConnectionArray[i]->ClientCount();
            
            DP_SDA2("ReleaseConnection :  clientCount = %d ", clientCount );    
            DP_SDA2("CPresencePlugin::ReleaseConnection :  i = %d", i );    
                
            if ( connectionsCount == 1 && clientCount <= 0 )
                {
                DP_SDA("CPresencePlugin::ReleaseConnection Lets start to check");
                DP_SDA2("CPresencePlugin::ReleaseConnection connect status %d,", iConnections[i]->GetSessionStatus());
                //Check this connection status
                if( !(CPresencePluginConnection::ENotActive ==
                	iConnectionArray[i]->GetConnectionStatus() ))
                   {
                   DP_SDA("CPresencePlugin::ReleaseConnection not have clients");
                   DeleteConnection(i);
                   break;
                   }
                DP_SDA("connectionsCount == 1 && i == 0 && clientCount == 0 ");    
                DP_SDA("ReleaseConnection : Don't delete this connection ");    
                break; //don't delete first connection
                } 
            DP_SDA("CPresencePlugin::ReleaseConnection for end");  
            }
        }     
    DP_SDA("CPresencePlugin::ReleaseConnection end");     
    }
    
// ---------------------------------------------------------------------------
// CPresencePlugin::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CPresencePlugin::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions )
    {       
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface 
        return this;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( KPluginName, KErrExtensionNotSupported );
        }  
    return NULL;    
    }

// ---------------------------------------------------------------------------
// CPresencePlugin::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CPresencePlugin::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions ) const
    {
    DP_SDA("CPresencePlugin::GetInterface");
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        return const_cast<CPresencePlugin*>(this);
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( KPluginName, KErrExtensionNotSupported );
        }       
    return NULL; 
    }

// ---------------------------------------------------------------------------
// CPresencePlugin::GetInterfaceId()
// ---------------------------------------------------------------------------
//
TInt32 CPresencePlugin::GetInterfaceId() const
    {
    return MXIMPProtocolPlugin::KInterfaceId;
    }    

// ---------------------------------------------------------------------------
// CPresencePlugin::DeleteConnection()
// ---------------------------------------------------------------------------
//
void CPresencePlugin::DeleteConnection( TInt aIndex )
    {
    //First delete connectionArray where connection is same at connection
    DP_SDA("CPresencePlugin::DeleteConnection"); 
    
    TInt connectionArrayCount = iConnectionArray.Count();
    
    for ( TInt i=0; i < connectionArrayCount; i++ )
        {
        DP_SDA2("CPresencePlugin::DeleteConnection FINDING, con status %d", iConnectionArray[i]->GetConnectionStatus() ); 
        if( (iConnections[aIndex] == iConnectionArray[i]->GetConnection()) &&
            !(CPresencePluginConnection::EActive ==
            iConnectionArray[i]->GetConnectionStatus() )    )
            {
            DP_SDA("CPresencePlugin::DeleteConnection connection Match"); 
            delete iConnectionArray[i];
            iConnectionArray.Remove( i );
            iConnectionArray.Compress();
            break; 
            }
        }
    delete iConnections[aIndex];
    iConnections.Remove( aIndex );
    iConnections.Compress();
    DP_SDA("CPresencePlugin::DeleteConnection end"); 
    }

// End of file
