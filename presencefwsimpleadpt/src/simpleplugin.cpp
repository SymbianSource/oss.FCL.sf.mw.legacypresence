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
* Description:    SIMPLE Protocol implementation for Presence Framework
*
*/




#include <ecom/implementationproxy.h>

#include <ximpserviceinfo.h>
#include <ximpbase.h>
#include <ximpidentity.h>
#include <presenceinfo.h>
#include <ximpprotocolconnectionhost.h>

#include "simpleplugin.h"
#include "simpleplugindef.h"
#include "simplepluginconnection.h"



// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Key value pair table to identify correct constructor
// function for the requested interface.
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( K_SIMPLEPLUGIN_1_IMPLEMENTATION_UID, CSimplePlugin::NewL )
    };

// ---------------------------------------------------------------------------
// Exported function to return the implementation proxy table
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// ---------------------------------------------------------------------------
// CSimplePlugin::CSimplePlugin()
// ---------------------------------------------------------------------------
//
CSimplePlugin::CSimplePlugin()
    {
    }

// ---------------------------------------------------------------------------
// CSimplePlugin::ConstructL()
// ---------------------------------------------------------------------------
//
void CSimplePlugin::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CSimplePlugin::NewLC()
// ---------------------------------------------------------------------------
//
CSimplePlugin* CSimplePlugin::NewLC()
    {
    CSimplePlugin* self = new( ELeave ) CSimplePlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CSimplePlugin::NewL()
// ---------------------------------------------------------------------------
//
CSimplePlugin* CSimplePlugin::NewL()
    {
    CSimplePlugin* self = CSimplePlugin::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSimplePlugin::~CSimplePlugin()
// ---------------------------------------------------------------------------
//
CSimplePlugin::~CSimplePlugin()
    {
    iConnections.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CSimplePlugin::PrimeHost()
// ---------------------------------------------------------------------------
//
void CSimplePlugin::PrimeHost( MXIMPProtocolPluginHost& aHost )
    {
    iHost = &aHost;
    }

// ---------------------------------------------------------------------------
// CSimplePlugin::AcquireConnectionL()
// ---------------------------------------------------------------------------
//
MXIMPProtocolConnection& CSimplePlugin::AcquireConnectionL(
    const MXIMPServiceInfo& aServiceInfo,
    const MXIMPContextClientInfo& aContextClient )
    {
    
    // TODO: singleton takes place here. Change name to CSimplePluginImp???
    // CSimplePluginConnection reflects to Singleton. This should compare PresenceID
    // and share a single entity with identical ids.HOW DOES PrFW work in this case???

    // support multiple connections
    // always create a new connection - no connection sharing, it takes place in SIP Stack.
    CSimplePluginConnection* connection =
        CSimplePluginConnection::NewL( aServiceInfo, aContextClient );
    CleanupStack::PushL( connection );
    iConnections.AppendL( connection );
    CleanupStack::Pop( connection );
    return *connection;
    }

// ---------------------------------------------------------------------------
// CSimplePlugin::ReleaseConnection()
// ---------------------------------------------------------------------------
//
void CSimplePlugin::ReleaseConnection( MXIMPProtocolConnection& aConnection )
    {
    TInt connectionsCount = iConnections.Count();

    for( TInt i( connectionsCount - 1 ); i >= 0; i-- )
        {
        MXIMPProtocolConnection* tmp = iConnections[i];
        if( tmp == &aConnection )
            {
            delete iConnections[i];
            iConnections.Remove( i );
            }
        }
    }

// ---------------------------------------------------------------------------
// CSimplePlugin::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CSimplePlugin::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions )
    {

    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        MXIMPProtocolPlugin* myIf = this;
        return myIf;
        }
    if( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( _L("CSimplePlugin"), KErrExtensionNotSupported );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSimplePlugin::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CSimplePlugin::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions ) const
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        const MXIMPProtocolPlugin* myIf = this;
        return myIf;
        }
    if( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( _L("CSimplePlugin"), KErrExtensionNotSupported );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSimplePlugin::GetInterfaceId()
// ---------------------------------------------------------------------------
//
TInt32 CSimplePlugin::GetInterfaceId() const
    {
    return MXIMPProtocolPlugin::KInterfaceId;
    }
    
    



// End of file

