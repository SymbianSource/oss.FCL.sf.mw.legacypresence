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

#include <e32math.h> 
#include <ximpprotocolconnection.h>

#include "presencepluginconnection.h"
#include "presenceconnectioninfo.h"
#include "presenceplugincommon.h"
#include "presencelogger.h"

 
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresenceConnectionInfo::CPresenceConnectionInfo()
// ---------------------------------------------------------------------------
//
CPresenceConnectionInfo::CPresenceConnectionInfo( 
        MXIMPProtocolConnection* aConnection ):
	iConnection( aConnection ),	
    iConnectStatus( CPresencePluginConnection::ENotActive )
    {
    iClientCount = 0;
    iClientId = GenerateClientId();
    }

// ---------------------------------------------------------------------------
// CPresenceConnectionInfo::NewL()
// ---------------------------------------------------------------------------
//
CPresenceConnectionInfo* CPresenceConnectionInfo::NewL( 
		MXIMPProtocolConnection* aConnection )
    {
    DP_SDA("CPresenceConnectionInfo::NewL");
    CPresenceConnectionInfo* self =
    	new( ELeave ) CPresenceConnectionInfo( aConnection );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresenceConnectionInfo::~CPresenceConnectionInfo()
// ---------------------------------------------------------------------------
//
CPresenceConnectionInfo::~CPresenceConnectionInfo()
    {
    DP_SDA("CPresenceConnectionInfo::~CPresenceConnectionInfo");
    DP_SDA2("CPresenceConnectionInfo:: clientID %d", iClientId);
    
    delete iETag;
    iETag = NULL;
    
    DP_SDA("CPresenceConnectionInfo::~CPresenceConnectionInfo done");
    }

// ---------------------------------------------------------------------------
// CPresenceConnectionInfo::SetConnectionStatus()
// ---------------------------------------------------------------------------
//
void CPresenceConnectionInfo::SetConnectionStatus( 
		CPresencePluginConnection::TConnectionStatus aStatus )
    {
    DP_SDA2("CPresenceConnectionInfo::SetConnectionStatus %d", aStatus);
    DP_SDA2("CPresenceConnectionInfo:: clientID %d", iClientId);
    iConnectStatus = aStatus;
    }

// ---------------------------------------------------------------------------
// CPresenceConnectionInfo::SetConnectionETag()
// ---------------------------------------------------------------------------
//
void CPresenceConnectionInfo::SetConnectionETag( const TDesC8& aETag )
    {
    DP_SDA("CPresenceConnectionInfo::SetConnectionETag");
    DP_SDA2("CPresenceConnectionInfo:: clientID %d", iClientId);
    delete iETag;
    iETag = NULL;
    TRAP_IGNORE( ( iETag = aETag.AllocL() ) );
    }

// ---------------------------------------------------------------------------
// CPresenceConnectionInfo::GetConnectionStatus()
// ---------------------------------------------------------------------------
//
CPresencePluginConnection::TConnectionStatus 
	CPresenceConnectionInfo::GetConnectionStatus()
    {
    DP_SDA("CPresenceConnectionInfo::GetConnectionStatus");
    DP_SDA2("CPresenceConnectionInfo:: clientID %d", iClientId);
    DP_SDA2("CPresenceConnectionInfo:: connection status %d", iConnectStatus);
    return iConnectStatus;
    }

// ---------------------------------------------------------------------------
// CPresenceConnectionInfo::GetConnectionEtag()
// ---------------------------------------------------------------------------
//
TDesC8* CPresenceConnectionInfo::GetConnectionEtag()
    {
    DP_SDA("CPresenceConnectionInfo::GetConnectionEtag");
    DP_SDA2("CPresenceConnectionInfo:: clientID %d", iClientId);
    return iETag;
    }

// ---------------------------------------------------------------------------
// CPresenceConnectionInfo::GetConnection()
// ---------------------------------------------------------------------------
//
MXIMPProtocolConnection* CPresenceConnectionInfo::GetConnection()
    {
    DP_SDA("CPresenceConnectionInfo::GetConnection");
    DP_SDA2("CPresenceConnectionInfo:: clientID %d", iClientId);
    return iConnection;
    }

// ---------------------------------------------------------------------------
// CPresenceConnectionInfo::IncreaseClientCount()
// ---------------------------------------------------------------------------
//
void CPresenceConnectionInfo::IncreaseClientCount()
    {
    iClientCount++;
    DP_SDA2("CPresenceConnectionInfo::IncreaseClientCount %d", iClientCount );
    DP_SDA2("CPresenceConnectionInfo:: clientID %d", iClientId);
    }

// ---------------------------------------------------------------------------
// CPresenceConnectionInfo::DecreaseClientCount()
// ---------------------------------------------------------------------------
//
void CPresenceConnectionInfo::DecreaseClientCount()
    {
    iClientCount--;
    DP_SDA2("CPresenceConnectionArray::DecreaseClientCount %d", iClientCount );
    DP_SDA2("CPresenceConnectionArray:: clientID %d", iClientId);
    }

// ---------------------------------------------------------------------------
// CPresenceConnectionInfo::ClientCount()
// ---------------------------------------------------------------------------
//
TInt CPresenceConnectionInfo::ClientCount()
    {
    DP_SDA2("CPresenceConnectionInfo::ClientCount %d", iClientCount );
    DP_SDA2("CPresenceConnectionInfo:: clientID %d", iClientId);
    return iClientCount;
    }

// ---------------------------------------------------------------------------
// CPresenceConnectionInfo::GetClientId()
// ---------------------------------------------------------------------------
//
TInt CPresenceConnectionInfo::GetClientId()
    {
    DP_SDA2("CPresenceConnectionInfo::ClientCount %d", iClientCount );
    DP_SDA2("CPresenceConnectionInfo:: clientID %d", iClientId);
    return iClientId;
    }

// ---------------------------------------------------------------------------
// CPresenceConnectionInfo::GenerateClientId()
// ---------------------------------------------------------------------------
//
TInt CPresenceConnectionInfo::GenerateClientId()
    {
    // Generate unique session client id
    DP_SDA("CPresenceConnectionInfo::GenerateClientIdstart");
    const TInt KMaxRand = 9999;
    TInt64 seed;
    TTime time;
    time.HomeTime();
    seed = time.Int64();
    TInt random = Math::Rand( seed ) % KMaxRand;
    DP_SDA("CPresenceConnectionInfo::GenerateClientId 1");
    return random; 
    }

// End of file
