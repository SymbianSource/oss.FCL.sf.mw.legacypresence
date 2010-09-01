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
#include <utf.h>

#include "presencepluginwatcherinfo.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CPresencePluginWatcherInfo::CPresencePluginWatcherInfo
// ---------------------------------------------------------------------------
CPresencePluginWatcherInfo::CPresencePluginWatcherInfo( )
        {}

// ---------------------------------------------------------------------------
// CPresencePluginWatcherInfo::~CPresencePluginWatcherInfo
// ---------------------------------------------------------------------------
CPresencePluginWatcherInfo::~CPresencePluginWatcherInfo()
    {
    delete iId;
    delete iSipId;
    }

// ----------------------------------------------------------
// CPresencePluginWatcherInfo::NewL
// ----------------------------------------------------------
//
CPresencePluginWatcherInfo* CPresencePluginWatcherInfo::NewL(
   const TDesC8& aId, const TDesC& aSipId )
    {
    CPresencePluginWatcherInfo* self =
    	new (ELeave) CPresencePluginWatcherInfo( );
    CleanupStack::PushL( self );
    self->ConstructL( aId, aSipId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcherInfo::ConstructL
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcherInfo::ConstructL( 
    const TDesC8& aId, const TDesC& aSipId  )
    {
    iId = aId.AllocL();
    iSipId = aSipId.AllocL();
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcherInfo::Destroy
// ---------------------------------------------------------------------------
void CPresencePluginWatcherInfo::Destroy()
    {
    DP_SDA("CPresencePluginWatcherInfo::Destroy");
    iLink.Deque();
    delete this;
    DP_SDA("CPresencePluginWatcherInfo::Destroy end");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginWatcherInfo::Match
// ---------------------------------------------------------------------------
TBool CPresencePluginWatcherInfo::Match( 
	const TDesC8& aId,
	const TDesC& aSipId )
    {
    DP_SDA("CPresencePluginWatcherInfo::Match");
    if ( (!iId->Des().CompareF( aId )) && (!iSipId->Des().CompareF( aSipId )) )    
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }       
    }
       
// ---------------------------------------------------------------------------
// CPresencePluginWatcherInfo::SipId
// ---------------------------------------------------------------------------
//
TPtrC CPresencePluginWatcherInfo::SipId( )
    {
    DP_SDA("CPresencePluginWatcherInfo::SipId");
    return iSipId ? iSipId->Des() : TPtrC();       
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginWatcherInfo::LinkOffset
// ---------------------------------------------------------------------------
//    
TInt CPresencePluginWatcherInfo::LinkOffset()
    {
    return _FOFF(CPresencePluginWatcherInfo, iLink);
    }
    
// End of file
