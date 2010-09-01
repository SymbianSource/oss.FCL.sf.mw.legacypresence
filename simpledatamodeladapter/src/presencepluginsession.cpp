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


#include <e32std.h>
#include <ximpserviceinfo.h> //For MXIMPServiceInfo
#include <ximpprotocolconnectionhost.h> //Handle command
#include <msimpleconnectionobserver.h>//Connection callback methods
#include <simplefactory.h> //create instances to abstract class
#include <msimpleconnection.h> //Part of Registration API from SIMPLE engine
#include <msimpleconnectionobserver.h>
#include <pressettingsapi.h> //for TPresSettingsSet
#include <XdmSettingsApi.h>
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <spdefinitions.h>

#include "presencepluginxdmutils.h"
#include "presencepluginsession.h"
#include "mpresencepluginconnectionobs.h" 
#include "presencepluginauthorization.h"
#include "presenceplugindata.h"

// ======== MEMBER FUNCTIONS ========
     
// ---------------------------------------------------------------------------
// CPresencePluginSession::CPresencePluginSession()
// ---------------------------------------------------------------------------
//
CPresencePluginSession::CPresencePluginSession( 
    MPresencePluginConnectionObs& aObs )
    : CActive( CActive::EPriorityStandard ),iObs(aObs),
    iType( MPresencePluginConnectionObs::ENoReq ),iSipConnected( EFalse ),
    iXdmConnected( EFalse ), iXdmLocalMode( EFalse ), iOperation( ENoOperation ),
    iServiceId( KErrNotFound )
    {
    CActiveScheduler::Add(this);
    }
  
// ---------------------------------------------------------------------------
// CPresencePluginSession::ConstructL()
// ---------------------------------------------------------------------------
//  
void CPresencePluginSession::ConstructL( const MXIMPServiceInfo& aService )
    {
    DP_SDA2("CPresencePluginSession::ConstructL: service id: %d", 
        aService.IapId() );

    CSPSettings* spSettings = CSPSettings::NewL();
    CleanupStack::PushL( spSettings );
    
    CSPProperty* property = CSPProperty::NewLC();
    spSettings->FindPropertyL( aService.IapId(),
                                ESubPropertyPresenceSettingsId,
                                *property );    
    if ( property )
        {
        property->GetValue( iPresSettingId );
        }
        
    CleanupStack::PopAndDestroy( property );        
    CleanupStack::PopAndDestroy( spSettings );

    DP_SDA2("PluginSession::ConstructL iPresSettingId %d",iPresSettingId );
    __ASSERT_ALWAYS( iPresSettingId > 0, User::Leave( KErrArgument ) );
    iServiceId = aService.IapId();
    
    //Create connection
    iConnection = TSimpleFactory::NewConnectionL( *this, aService.IapId() );
    
    //Check here is xdm settings ok
    CheckXDMSettingsL( aService.IapId() );
    
    DP_SDA("CPresencePluginSession::ConstructL end");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginSession::NewL()
// ---------------------------------------------------------------------------
//  
CPresencePluginSession* CPresencePluginSession::NewL( 
    const MXIMPServiceInfo& aService,
    MPresencePluginConnectionObs& aObs )
    {
    DP_SDA("CPresencePluginSession::NewL");
    CPresencePluginSession* self =
        new( ELeave ) CPresencePluginSession( aObs );
    CleanupStack::PushL( self );
    self->ConstructL( aService );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginSession::~CPresencePluginSession()
// ---------------------------------------------------------------------------
//  
CPresencePluginSession::~CPresencePluginSession()
    {
    DP_SDA("CPresencePluginSession::~CPresencePluginSession");

    delete iUserId8;
    delete iDomain;
    if ( iConnection )
        {
        iConnection->Close();        
        }    
    
    DP_SDA("~CPresencePluginSession iXdmUtils");
    delete iXdmUtils;
    iXdmUtils = NULL;
    DP_SDA("CPresencePluginSession::~CPresencePluginSession end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginSession::SimpleConnection()
// ---------------------------------------------------------------------------
//
TInt32 CPresencePluginSession::GetPresenceSetId( ) const
    {
    return iPresSettingId;            
    }
        
// ---------------------------------------------------------------------------
// CPresencePluginSession::SimpleConnection()
// ---------------------------------------------------------------------------
//
MSimpleConnection* CPresencePluginSession::SimpleConnection( )
    {
    return iConnection;            
    }

// ---------------------------------------------------------------------------
// CPresencePluginSession::XdmSettingsId()
// ---------------------------------------------------------------------------
//
TInt CPresencePluginSession::XdmSettingsId( )
    {
    DP_SDA("CPresencePluginSession::XdmSettingsId");
    return iXdmSetting;            
    } 

// ---------------------------------------------------------------------------
// CPresencePluginSession::CurrentDomain()
// ---------------------------------------------------------------------------
//
TPtrC16 CPresencePluginSession::CurrentDomain( )
    {
    return iDomain ? iDomain->Des() : TPtrC16();
    }    

// ---------------------------------------------------------------------------
// CPresencePluginSession::IsXdmLocalmode()
// ---------------------------------------------------------------------------
//
TBool CPresencePluginSession::IsXdmLocalmode()
	{
	DP_SDA("CPresencePluginSession::IsXdmLocalmode");
	return iXdmLocalMode;  
	}
// ---------------------------------------------------------------------------
// CPresencePluginSession::OpenSessionL()
// ---------------------------------------------------------------------------
//
void CPresencePluginSession::OpenSessionL()
    {
    DP_SDA("CPresencePluginSession::OpenSessionL");
    //Check connection status
    MSimpleConnection::TSimpleState connectionStatus =
    	iConnection->ConnectionStatus();
    DP_SDA2("OpenSessionL Connection Status %d", connectionStatus);
    
    if(  connectionStatus == MSimpleConnection::EInactive )
        {
        DP_SDA("CPresencePluginSession::OpenSessionL Not connected yet"); 
        iSipConnected = EFalse;
        // SIP register using by give presence setting id
        iOpId = iConnection->LoginL( iPresSettingId );
        iType = MPresencePluginConnectionObs::EOpenSess;
        }
    else if ( connectionStatus == MSimpleConnection::EActive )
        {
        DP_SDA(":OpenSessionL already connected complete");
        iSipConnected = ETrue;
        iType = MPresencePluginConnectionObs::EOpenSess;
        RequestCompleteL( iOpId, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginSession::ConnectionStatusL()
// ---------------------------------------------------------------------------
//
void CPresencePluginSession::ConnectionStatusL( 
    MSimpleConnection::TSimpleState aState )
    {
    DP_SDA2("CPresencePluginSession::ConnectionStatusL aState %d", aState );
        
    if ( (aState == MSimpleConnection::EInactive ) 
        && iSipConnected )
        {
        DP_SDA("CPresencePluginSession::ConnectionStatusL EInactive");
        
        DP_SDA("CPresencePluginSession::ConnectionStatusL clear pres cache");
        iObs.InternalPresenceAuthorization().PluginData().RemoveCacheL();
        DP_SDA("CPresencePluginSession::ConnectionStatusL clear cache ok");
        
        // cancel all XDM request
  		iObs.XdmUtilsL()->Cancel();
  		//Set connection Terminated, terminate will be remove all client binds
  		iObs.TerminateConnectionL();
        iSipConnected = EFalse;
        iXdmConnected = EFalse;
        }
    else if( ( aState == MSimpleConnection::EUnavailable && iSipConnected ) )
		{
		//Its seems that SIP connection is unavaible 
		DP_SDA("CPresencePluginSession::ConnectionStatusL EUnavailable");
		}

    }
    
// ---------------------------------------------------------------------------
// CPresencePluginSession::CloseConnection()
// ---------------------------------------------------------------------------
//
void CPresencePluginSession::CloseConnection()
    {
    DP_SDA("CPresencePluginSession::CloseConnection");
    if( iConnection )
        {
        DP_SDA("CPresencePluginSession::CloseConnection close called");
        iConnection->Close();
        iConnection = NULL;
        }
    DP_SDA("CPresencePluginSession::CloseConnection end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginSession::ConnectionStatus()
// ---------------------------------------------------------------------------
//
TBool CPresencePluginSession::ConnectionStatus()
    {
    DP_SDA("CPresencePluginSession::ConnectionStatus");
    TBool connection = EFalse;
    if( iXdmConnected && iSipConnected )
    	{
    	connection = ETrue;
    	}
    return connection;
    }
        
// ---------------------------------------------------------------------------
// CPresencePluginSession::RequestCompleteL()
// ---------------------------------------------------------------------------
//
void CPresencePluginSession::RequestCompleteL( TInt /*aOpId*/, TInt aStatus )
    {
    DP_SDA("CPresencePluginSession::RequestCompleteL");
    MPresencePluginConnectionObs::TReqType current = iType;
     
    if ( !aStatus  )
       {
       DP_SDA("CPresencePluginSession::RequestCompleteL SIP session is open");
       iSipConnected = ETrue;
       if( iXdmConnected )
    	   {
    	   DP_SDA("CPresencePluginSession::RequestCompleteL xdm IS OK");
    	   iType = MPresencePluginConnectionObs::ENoReq;
    	   iObs.CompleteReq( current, aStatus );
    	   }
       else
    	   {
    	   DP_SDA("CPresencePluginSession::RequestCompleteL Initialize XDM");
    	   __ASSERT_DEBUG( !IsActive(), User::Leave( KErrNotReady ) );
	       //Lets test XDM connection before complete
		   iXdmUtils = CPresencePluginXdmUtils::NewL( 
					iObs, iXdmSetting, iXdmLocalMode );
		   
		   iOperation =  EInitializeXdm;
		   iXdmUtils->InitializeXdmL( iStatus );
		   SetActive();
    	   }
	   
       }
    DP_SDA("CPresencePluginSession::RequestCompleteL END");
    } 
        
// ---------------------------------------------------------------------------
// CPresencePluginSession::CurrentSipPresentity()
// ---------------------------------------------------------------------------
//
TPtrC8 CPresencePluginSession::CurrentSipPresentity()
    {
    DP_SDA("CPresencePluginSession::CurrentSipPresentity");
    // Get from Simple engine
    delete iUserId8;
    iUserId8 = NULL;
    TRAPD(err, iUserId8 = iConnection->CurrentSIPIdentityL().AllocL() )
    if ( err == KErrNone )
        {
        return iUserId8 ? iUserId8->Des() : TPtrC8();
        }
    else
        {
        return iUserId8 ? KNullDesC8() : TPtrC8();
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginSession::CheckXDMSettings()
// ---------------------------------------------------------------------------
//
void CPresencePluginSession::CheckXDMSettingsL( TUint aServiceId )
	{
	//Check if xdm settings not valid or there is text http://localhost
	//in settings. If not valid or text found client will be save resourcelist
	//to in phonememory
    
	TPresSettingsSet mySet;    
    User::LeaveIfError( 
            PresSettingsApi::SettingsSetL( iPresSettingId, mySet ));
    iXdmSetting = mySet.iXDMSetting;
    if( iDomain )
        {
        delete iDomain;
        iDomain = NULL;
        }
    iDomain = mySet.iDomainSyntax.AllocL(); 
    
    DP_SDA(" -> CheckXDMSettings - store domain to service table");
    // Service table 
    CSPSettings* spsettings = CSPSettings::NewLC();
    CSPProperty* property = CSPProperty::NewLC();
    
    DP_SDA(" -> CheckXDMSettings - spsettings and property created");
    User::LeaveIfError( property->SetName( ESubPropertyPresenceAddrScheme ) );
    DP_SDA(" -> CheckXDMSettings - property name set");
    
    TInt atPosInDomain = iDomain->Locate( '@' );
    DP_SDA2(" -> CheckXDMSettings - @ position in domain: %d", atPosInDomain );
    if ( KErrNotFound == atPosInDomain )
        {
        DP_SDA(" -> CheckXDMSettings - no @ in domain, set");
        property->SetValue( iDomain->Des() );
        }
    else
        {
        DP_SDA(" -> CheckXDMSettings - @ found in domain, remove");
        HBufC* formattedDomain = HBufC::NewLC( iDomain->Length() );    
        TPtr formattedDomainPtr( formattedDomain->Des() );
        formattedDomainPtr.Copy( iDomain->Mid( ( atPosInDomain + 1 ) ) );
        property->SetValue( formattedDomainPtr );
        CleanupStack::PopAndDestroy( formattedDomain );
        }
    DP_SDA(" -> CheckXDMSettings - property value set");
    User::LeaveIfError( 
        spsettings->AddOrUpdatePropertyL( aServiceId, *property ) );
    DP_SDA(" -> CheckXDMSettings - property added or updated");
    
    CleanupStack::PopAndDestroy( property );
    CleanupStack::PopAndDestroy( spsettings );    

#ifdef _DEBUG    
    DP_SDA2("XdmSettingsId xdmSetting id %d",iXdmSetting );    
    DP_SDA2("CheckXDMSettings::XdmSettingsId domain %S",iDomain );   
#endif
    
    //Continue check, add localhost to buf2
    HBufC* buf2 = KLocalhost().AllocLC();
    
	//Get Uri from XDM settings 
	HBufC* xcaproot = NULL;
	
	TRAPD( error, xcaproot =
		TXdmSettingsApi::PropertyL( iXdmSetting, EXdmPropUri ) );
	DP_SDA2("CheckXDMSettings Set localmode, err %d", error);

	if( KErrNotFound != error && xcaproot )
		{
		//Compare buf2 to xcaproot
		if ( !xcaproot->Compare( buf2->Des() ) )
        	{
        	DP_SDA("CheckXDMSettings Set localmode");
        	iXdmLocalMode = ETrue;
        	}
        else
        	{
        	DP_SDA("CheckXDMSettings Set Normal Mode");
        	iXdmLocalMode = EFalse;
        	}
		
		DP_SDA("CheckXDMSettings PopAndDestroy buf2");
		}
	else
		{
		//It seems there is no valid XCAP settings, so we try localmode
		//Shuld we create automaticaly XCAP localhost settings?
		iXdmLocalMode = ETrue;
		}
	
	DP_SDA("CheckXDMSettings PopAndDestroy xcaproot");
	CleanupStack::PopAndDestroy( buf2 );
	delete xcaproot;
	}

// ---------------------------------------------------------------------------
// CPresencePluginSession::XdmUtilsL()
// ---------------------------------------------------------------------------
//
CPresencePluginXdmUtils* CPresencePluginSession::XdmUtilsL()
	{
	if ( !iXdmUtils )
		{
		DP_SDA("CPresencePluginSession::XdmUtilsL !iXdmUtils");                
		iXdmUtils = CPresencePluginXdmUtils::NewL( 
				iObs, iXdmSetting, iXdmLocalMode );
		}
	
	return iXdmUtils;
	}

// ---------------------------------------------------------------------------
// CPresencePluginSession::ServiceId()
// ---------------------------------------------------------------------------
//
TInt& CPresencePluginSession::ServiceId()
    {
    return iServiceId;
    }

// ---------------------------------------------------------------------------
// CPresencePluginSession::DoCancel()
// ---------------------------------------------------------------------------
//
void CPresencePluginSession::DoCancel(  )
    {
    DP_SDA("CPresencePluginSession::DoCancel CANCEL");
    iXdmUtils->Cancel();
    }

// ---------------------------------------------------------------------------
// CPresencePluginSession::RunL()
// ---------------------------------------------------------------------------
//
void CPresencePluginSession::RunL(  )
    {
    DP_SDA("CPresencePluginSession::RunL");
    TInt status = iStatus.Int();
    DP_SDA2("CPresencePluginSession::RunL status %d", status );
    
    if(  ( iOperation == EInitializeXdm ) && !status )
    	{
    	DP_SDA("CPresencePluginSession::RequestCompleteL connection ready");
    	iXdmConnected = ETrue;
        MPresencePluginConnectionObs::TReqType current = iType;
        iType = MPresencePluginConnectionObs::ENoReq;
        iObs.CompleteReq( current, status );
    	}
    else
    	{
    	DP_SDA("Connection FAIL terminated");
    	iXdmConnected = EFalse;
    	iSipConnected = EFalse;
    	iType = MPresencePluginConnectionObs::ECloseSess;
    	iObs.CompleteReq( iType, status );
    	}

    }

// ---------------------------------------------------------------------------
// CPresencePluginSession::RunError
// ---------------------------------------------------------------------------
//
TInt CPresencePluginSession::RunError( TInt /*aError*/ )
    {
    DP_SDA("CPresencePluginSession::RunError"); 
    return KErrNone;
    }

 // End of file
 
