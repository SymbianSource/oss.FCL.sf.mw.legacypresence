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




#include <e32std.h>

#include <ximpdatasubscriptionstate.h>
#include <ximpobjectfactory.h>

#include <simplefactory.h>
#include <msimplewinfowatcher.h>

#include "simpleplugincommon.h"
#include "simplepluginwinfo.h"
#include "simpleplugindebugutils.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSimplePluginWinfo::CSimplePluginWinfo()
// ---------------------------------------------------------------------------
//
CSimplePluginWinfo::CSimplePluginWinfo(
    MSimplePluginConnectionObs& aObs,
    MSimpleConnection& aConn )
  : iConnObs(aObs), iConnection(aConn), iSubscribed(0), iWinfoCompleted(EFalse)
    {
    }

// ---------------------------------------------------------------------------
// CSimplePluginWinfo::ConstructL()
// ---------------------------------------------------------------------------
//
void CSimplePluginWinfo::ConstructL( )
    {
    iWinfoWatcher = TSimpleFactory::NewWinfoWatcherL( iConnection, *this );
    }

// ---------------------------------------------------------------------------
// CSimplePluginWinfo::NewL()
// ---------------------------------------------------------------------------
//
CSimplePluginWinfo* CSimplePluginWinfo::NewL(
    MSimplePluginConnectionObs& aObs,
    MSimpleConnection& aConn )
    {
    CSimplePluginWinfo* self =
        new( ELeave ) CSimplePluginWinfo( aObs, aConn );
    CleanupStack::PushL( self );
    self->ConstructL(  );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSimplePluginWinfo::~CSimplePluginWinfo()
// ---------------------------------------------------------------------------
//
CSimplePluginWinfo::~CSimplePluginWinfo()
    {
    if ( iWinfoWatcher )
        {
        iWinfoWatcher->Close();
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWinfo::SetHost()
// ---------------------------------------------------------------------------
//
void CSimplePluginWinfo::SetHost(
    MXIMPProtocolConnectionHost* aHost )
    {
    iHost = aHost;
    }

// ---------------------------------------------------------------------------
// CSimplePluginWinfo::SubscribeWinfoListL()
// ---------------------------------------------------------------------------
//
void CSimplePluginWinfo::SubscribeWinfoListL( TXIMPRequestId aReqId )
    {
    iWinfoCompleted = EFalse;
    iSubscribed++;
    if ( iSubscribed == 1 )
        {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginWinfo: -> SubscribeWatcherListL") );
#endif
        iSimpleId = iWinfoWatcher->SubscribeWatcherListL( NULL );
        iSubscribed = ETrue;
        iPrFwId = aReqId;
        }
    else
        {
        iSimpleId = 0;
        iPrFwId = aReqId;
        WinfoReqCompleteL( iSimpleId, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWinfo::UnsubscribeWinfoListL()
// ---------------------------------------------------------------------------
//
void CSimplePluginWinfo::UnsubscribeWinfoListL( TXIMPRequestId aReqId )
    {
    iWinfoCompleted = EFalse;
    TInt orig = iSubscribed;
    iSubscribed--;
    if ( iSubscribed < 0 )
        {
        iSubscribed = 0;
        }

    if ( !iSubscribed && orig )
        {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginWinfo: -> UnsubscribeL") );
#endif
        iSubscribed++;
        iSimpleId = iWinfoWatcher->UnsubscribeL();
        iSubscribed--;
        iPrFwId = aReqId;
        }
    else
        {
        iSimpleId = 0;
        iPrFwId = aReqId;
        WinfoReqCompleteL( iSimpleId, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWinfo::WinfoReqCompleteL()
// ---------------------------------------------------------------------------
//
void CSimplePluginWinfo::WinfoReqCompleteL( TInt /*aOpId*/, TInt aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginWinfo: WinfoReqCompleteL status=%d"), aStatus );
#endif

    if ( !iWinfoCompleted  )
        {
        iWinfoCompleted = ETrue;
        iSimpleId = 0;
        iConnObs.CompleteWinfoReq( iPrFwId, aStatus );
        // Do not wait MXIMPProtocolConnectionHostObserver callback, this
        // class is ready to serve the next request now.
        iPrFwId = TXIMPRequestId();
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWinfo::WinfoTerminatedL()
// ---------------------------------------------------------------------------
//
void CSimplePluginWinfo::WinfoTerminatedL(
    TInt aOpId, TInt aReason )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginWinfo: WinfoTerminatedL opid=%d"), aOpId );
#endif

    if ( iSimpleId == aOpId )
        {
        iSimpleId = 0;
        iConnObs.WinfoTerminatedL( aReason );
        }
 }

// ---------------------------------------------------------------------------
// CSimplePluginWinfo::WinfoNotificationL()
// ---------------------------------------------------------------------------
//
void CSimplePluginWinfo::WinfoNotificationL(
    MSimpleWinfo& aWinfo )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginWinfo: WinfoNotificationL starts"));
#endif

    WinfoReqCompleteL( iSimpleId, KErrNone );
    iConnObs.WinfoNotification( aWinfo );

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginWinfo: WinfoNotificationL ends"));
#endif
    }


// End of file

