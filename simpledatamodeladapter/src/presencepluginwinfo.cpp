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
#include <simplefactory.h>
#include <msimplewinfowatcher.h>
#include <msimpleconnection.h>				 

#include "presencepluginwinfo.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginWinfo::CPresencePluginWinfo()
// ---------------------------------------------------------------------------
//
CPresencePluginWinfo::CPresencePluginWinfo(
    MPresencePluginConnectionObs& aObs,
    MSimpleConnection& aConn )
    :iConnObs( aObs ), iConnection( aConn ),
    iSubscribed( EFalse ),
    iWinfoCompleted( EFalse )
    {       
    }

// ---------------------------------------------------------------------------
// CPresencePluginWinfo::ConstructL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWinfo::ConstructL( )
    {        
    iWinfoWatcher = TSimpleFactory::NewWinfoWatcherL( iConnection, *this );
    }

// ---------------------------------------------------------------------------
// CPresencePluginWinfo::NewL()
// ---------------------------------------------------------------------------
//
CPresencePluginWinfo* CPresencePluginWinfo::NewL(
    MPresencePluginConnectionObs& aObs,
    MSimpleConnection& aConn )
    {
    DP_SDA("CPresencePluginWinfo::NewL");
    CPresencePluginWinfo* self =
        new( ELeave ) CPresencePluginWinfo( aObs, aConn );
    CleanupStack::PushL( self );
    self->ConstructL(  );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginWinfo::~CPresencePluginWinfo()
// ---------------------------------------------------------------------------
//
CPresencePluginWinfo::~CPresencePluginWinfo()
    {
    if ( iWinfoWatcher )
        {
        iWinfoWatcher->Close();
        }
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginWinfo::SetHost()
// ---------------------------------------------------------------------------
//    
void CPresencePluginWinfo::SetHost( 
    MXIMPProtocolConnectionHost* aHost )
    {
    iHost = aHost;    
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginWinfo::SubscribeWinfoListL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginWinfo::SubscribeWinfoListL( TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginWinfo::SubscribeWinfoListL");
    iWinfoCompleted = EFalse;
    if ( !iSubscribed )
        {
        DP_SDA("CPresencePluginWinfo::SubscribeWinfoListL !iSubscribed");    
        iSimpleId = iWinfoWatcher->SubscribeWatcherListL( NULL );
        iSubscribed = ETrue;
        iXIMPId = aReqId;       
        }
    else
        {
        DP_SDA("CPresencePluginWinfo::SubscribeWinfoListL else ");
        iSimpleId = 0;
        iXIMPId = aReqId;         
        WinfoReqCompleteL( iSimpleId, KErrNone );
        }
    DP_SDA("CPresencePluginWinfo::SubscribeWinfoListL end");           
    } 
    
// ---------------------------------------------------------------------------
// CPresencePluginWinfo::UnsubscribeWinfoListL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginWinfo::UnsubscribeWinfoListL( TXIMPRequestId aReqId )
    {
    if ( iSubscribed )
        {    
        iSubscribed = EFalse;        
        iSimpleId = iWinfoWatcher->UnsubscribeL();
        iXIMPId = aReqId;       
        }
    else
        {
        iSimpleId = 0;
        iXIMPId = aReqId;         
        WinfoReqCompleteL( iSimpleId, KErrNone );
        }           
    }  
      
// ---------------------------------------------------------------------------
// CPresencePluginWinfo::WinfoReqCompleteL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWinfo::WinfoReqCompleteL( TInt aOpId, TInt aStatus )
    {
    DP_SDA("CPresencePluginWinfo::WinfoReqCompleteL");
    if ( iSimpleId == aOpId )
        {
        iWinfoCompleted = ETrue;
        iSimpleId = 0;    
        iConnObs.CompleteReq( iXIMPId, aStatus ); 
        // Do not wait MXIMPProtocolConnectionHostObserver callback, this
        // class is ready to serve the next request now.
        iXIMPId = TXIMPRequestId();              
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginWinfo::WinfoTerminatedL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWinfo::WinfoTerminatedL(
    TInt aOpId, TInt aReason )
    {

    if ( iSimpleId == aOpId )
        {
        iSimpleId = 0;        
        iConnObs.WinfoTerminatedL( aReason );
        }
 }              

// ---------------------------------------------------------------------------
// CPresencePluginWinfo::WinfoNotificationL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWinfo::WinfoNotificationL(
    MSimpleWinfo& aWinfo )
    {
    if ( !iWinfoCompleted )
        {
        WinfoReqCompleteL( iSimpleId, KErrNone );
        }
    iConnObs.WinfoNotification( aWinfo );    
    }
 
// End of file
