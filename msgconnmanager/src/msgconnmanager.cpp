/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Connection manager
*
*/




// INCLUDE FILES
#include <in_sock.h>
#include <es_enum.h>
#include <f32file.h>
#include <flogger.h>
#include "msgconntimer.h"
#include "msgconnmanager.h"

// ================= MEMBER FUNCTIONS =======================
//


// ----------------------------------------------------------
// CMsgConnManager::CMsgConnManager
// 
// ----------------------------------------------------------
//
CMsgConnManager::CMsgConnManager() : CActive( EPriorityUserInput + 2 ),
                                     iProgressStarted( EFalse ),
                                     iConnectionClosed( ETrue )
                                                
    {
    
    }

// ----------------------------------------------------------
// CMsgConnManager::NewL
// 
// ----------------------------------------------------------
//
CMsgConnManager* CMsgConnManager::NewL( const TInt aAccessPoint )
    {
    CMsgConnManager* self = new ( ELeave ) CMsgConnManager();
    CleanupStack::PushL( self );
    self->ConstructL( aAccessPoint );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CMsgConnManager::ConstructL
// 
// ----------------------------------------------------------
//
void CMsgConnManager::ConstructL( const TInt aAccessPoint )
    {
    DeleteLogFileL();
    iAccessPointID = aAccessPoint;
    iConnectionTimer = CMsgConnTimer::NewL();
    iCommsDatabase = CCommsDatabase::NewL( EDatabaseTypeUnspecified );
    User::LeaveIfError( iSocketSession.Connect() );
    User::LeaveIfError( iConnection.Open( iSocketSession ) );
    iConnectionClosed = EFalse;
    iAccessPointSettings.SetIapId( aAccessPoint );
    iAccessPointSettings.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
    #ifdef _DEBUG
        WriteToLog( _L8( "CONNMAN: LOGGING ENABLED" ) );
    #endif
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------
// CMsgConnManager::~CMsgConnManager
// 
// ----------------------------------------------------
//
CMsgConnManager::~CMsgConnManager()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::~CMsgConnManager(), Destructor." ) );
    #endif
    Cancel();
    delete iCommsDatabase;
    delete iConnectionTimer;
    iConnection.Close();
    iSocketSession.Close();
    iEventSubscribers.Close();
    }

// ----------------------------------------------------------
// CMsgConnManager::DeleteLogFileL
// 
// ----------------------------------------------------------
//
void CMsgConnManager::DeleteLogFileL()
    {
    RFs session;
    User::LeaveIfError( session.Connect() );
    CFileMan* manager = CFileMan::NewL( session );
    manager->Delete( _L("C:\\logs\\MsgConnMan\\*.*") );
    session.Close();
    delete manager;
    manager = NULL;
    }

// ---------------------------------------------------------
// CMsgConnManager::StartBearerObserver
// 
// ---------------------------------------------------------
//
void CMsgConnManager::StartNetworkObserver()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::StartNetworkObserver()  Closed: %d" ), iConnectionClosed );
    #endif
    #ifdef __WINS__
        if( !iConnectionClosed )
            {
            iStatus = KRequestPending;
            SetActive();
            }
    #else
        if( !iConnectionClosed )
            {
            if( !IsActive() )
                {
                iStatus = KRequestPending;
                iConnection.ProgressNotification( iProgressBuffer, iStatus );
                SetActive();
                }
            }
    #endif
    }

// ---------------------------------------------------------
// CMsgConnManager::RunL()
// 
// ---------------------------------------------------------
//
void CMsgConnManager::RunL()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::RunL(), iConnManStatus: %d  iStatus: %d  iStage: %d  iError: %d" ),
                    iConnManStatus, iStatus.Int(), iProgressBuffer().iStage, iProgressBuffer().iError );
    #endif
    switch( iConnManStatus ) 
        {
        case EMsgConnManWaitingForStartComplete:
            #ifdef _DEBUG
                WriteToLog( _L8( " Current status is EMsgConnManWaitingForStartComplete." ) );
            #endif
            WatchConnectionOpening();
            break;
        case EMsgConnManWaitingForNotifications:
            #ifdef _DEBUG
                WriteToLog( _L8( " Current status is EMsgConnManWaitingForNotifications." ) );
            #endif
            WatchConnectionOpening();
            break;
        case EMsgConnManObservingAccessPoint:
            #ifdef _DEBUG
                WriteToLog( _L8( " Current status is EMsgConnManObservingAccessPoint." ) );
            #endif
            DetermineConnectionStatusL();
            break;
        case EMsgConnManBearerSuspended:
            #ifdef _DEBUG
                WriteToLog( _L8( " Current status is EMsgConnManBearerSuspended." ) );
            #endif
            DetermineConnectionStatusL();
            break;
        case EMsgConnManDisconnectingPrevious:
            #ifdef _DEBUG
                WriteToLog( _L8( " Timer expired, complete client's request." ) );
            #endif
            User::RequestComplete( iTimedCompleteStatus, KErrNone );
            iConnManStatus = EMsgConnManIdle;
            break;
        case EMsgConnManIdle:
            #ifdef _DEBUG
                WriteToLog( _L8( " Idle, iStatus is %d." ), iStatus.Int() );
            #endif
            break;
        default:
            #ifdef _DEBUG
                WriteToLog( _L8( " Default case, should not be here." ) );
            #endif
            break;
        }
    #ifdef _DEBUG
        WriteToLog( _L8( " RunL() ends" ) );
    #endif
    }

// ----------------------------------------------------
// CMsgConnManager::WatchConnectionOpening()
// 
// ----------------------------------------------------
//
void CMsgConnManager::WatchConnectionOpening()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "  CMsgConnManager::WatchConnectionOpening() - iProgressStarted: %d" ), iProgressStarted );
    #endif
    //Lets quit right away if it is an error
    if( iStatus != KErrNone )
        {
        #ifdef _DEBUG
            WriteToLog( _L8( "   Something wrong, complete with error: %d." ), iStatus.Int() );
        #endif
        iConnectionTimer->Cancel();
        if( iConnManStatus == EMsgConnManWaitingForNotifications )
            iConnection.CancelProgressNotification();
        iConnection.Close();
        iConnectionClosed = ETrue;
        User::RequestComplete( iClientStatus, iStatus.Int() );
        iConnManStatus = EMsgConnManIdle;
        }
    else
        {
        if( iProgressStarted )
            {
            iLastConnectionStage = iProgressBuffer().iStage;
            if( iLastConnectionStage == KLinkLayerOpen )  //we are done
                {
                #ifdef _DEBUG
                    WriteToLog( _L8( "   Connected. Completing with KErrNone." ) );
                #endif
                iConnectionTimer->Cancel();
                iConnManStatus = EMsgConnManObservingAccessPoint;
                StartNetworkObserver();
                User::RequestComplete( iClientStatus, KErrNone );
                iProgressStarted = EFalse;
                }
            else
                {
                #ifdef _DEBUG
                    WriteToLog( _L8( "   Keep on waiting for progress." ) );
                #endif
                IssueRequest();
                }
            }
        else
            {
            #ifdef _DEBUG
                WriteToLog( _L8( "   Start complete, watch progress." ) );
            #endif
            if( iConnManStatus == EMsgConnManWaitingForStartComplete )
                {
                iProgressStarted = ETrue;
                iConnManStatus = EMsgConnManWaitingForNotifications;
                }
            IssueRequest();
            }
        }
    }

// ----------------------------------------------------
// CMsgConnManager::DetermineConnectionStatus()
// 
// ----------------------------------------------------
//
void CMsgConnManager::DetermineConnectionStatusL()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "  CMsgConnManager::DetermineConnectionStatus()" ) );
    #endif
    #ifdef __WINS__
        Cancel();
        switch( iStatus.Int() )
    #else
        switch( iProgressBuffer().iStage )
    #endif
        {
        case KLinkLayerClosed:
        case KConnectionUninitialised:
            #ifdef _DEBUG
                CMsgConnManager::WriteToLog( _L8( "   KLinkLayerClosed, notify clients of EMsgBearerLost" ) );
            #endif
            Cancel();
            iConnManStatus = EMsgConnManBearerLost;
            BroadcastBearerEventL( EMsgBearerLost );
            iConnection.Close();
            iConnectionClosed = ETrue;
            iEventSubscribers.Reset();
            iConnManStatus = EMsgConnManIdle;
            break;
        case KLinkLayerOpen:
            #ifdef _DEBUG
                WriteToLog( _L8( "   KLinkLayerOpen -> GPRS Context Active." ) );
            #endif
            if( iConnManStatus == EMsgConnManBearerSuspended )
                {
                #ifdef _DEBUG
                    WriteToLog( _L8( "   Moving to active state." ) );
                #endif
                iConnManStatus = EMsgConnManObservingAccessPoint;
                BroadcastBearerEventL( EMsgBearerActive );
                StartNetworkObserver();
                }
            break;
        case KDataTransferTemporarilyBlocked:
            #ifdef _DEBUG
                WriteToLog( _L8( "   KDataTransferTemporarilyBlocked -> GPRS Context Suspended." ) );
            #endif
            if( iConnManStatus != EMsgConnManBearerSuspended )
                {
                #ifdef _DEBUG
                    WriteToLog( _L8( "   Moving to suspended state." ) );
                #endif
                iConnManStatus = EMsgConnManBearerSuspended;
                BroadcastBearerEventL( EMsgBearerSuspended );
                }
            StartNetworkObserver();
            break;
        default:
            #ifdef _DEBUG
                #ifdef __WINS__
                    if( iStatus.Int() == KErrCancel )
                        WriteToLog( _L8( "   Bearer event generation cancelled" ) );
                    else
                        WriteToLog( _L8( "   Some weird status: %d" ), iStatus.Int() );
                #else
                    WriteToLog( _L8( "   Default case! WTF?!?" ) );
                #endif
            #endif
            StartNetworkObserver();
            break;
        }     
    }

// ---------------------------------------------------------
// CMsgConnManager::CancelStartL
// 
// ---------------------------------------------------------
//
void CMsgConnManager::CancelStartL()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::CancelStartL()" ) );
    #endif
    if( iClientStatus != NULL && iClientStatus->Int() == KRequestPending )
        {
        if( iConnManStatus == EMsgConnManWaitingForStartComplete ||
            iConnManStatus == EMsgConnManWaitingForNotifications )
            {
            Cancel();
            iEventSubscribers.Reset();
            User::RequestComplete( iClientStatus, KErrCancel );
            iConnManStatus = EMsgConnManIdle;
            iConnectionClosed = ETrue;
            }
        else User::Leave( KErrAbort );
        }
    else User::Leave( KErrAbort );
    }

// ---------------------------------------------------------
// CMsgConnManager::StopConnection
// 
// ---------------------------------------------------------
//
void CMsgConnManager::StopConnection()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::StopConnection()" ) );
    #endif
    if( iConnManStatus == EMsgConnManWaitingForStartComplete ||
        iConnManStatus == EMsgConnManWaitingForNotifications ||
        iConnManStatus == EMsgConnManIdle )
        {
        #ifdef _DEBUG
            WriteToLog( _L8( "  Status is %d, wrong state for this operation. Abort." ), iConnManStatus );
        #endif
        }
    else
        {
        //If bearer has been lost, we're already closed
        if( iConnManStatus != EMsgConnManBearerLost )
            {
            Cancel();
            iConnection.Close();
            iConnectionClosed = ETrue;
            iEventSubscribers.Reset();
            iConnManStatus = EMsgConnManIdle;
            }
        }
    #ifdef _DEBUG
        WriteToLog( _L8( "  StopConnection() ends." ) );
    #endif
    }

// ---------------------------------------------------------
// CMsgConnManager::StopConnection
// 
// ---------------------------------------------------------
//
void CMsgConnManager::StopConnection( TRequestStatus& aStatus )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CImpsConnectionManager::StopConnectionL()" ) );
    #endif  
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iTimedCompleteStatus = &aStatus;
    if( iConnManStatus == EMsgConnManWaitingForStartComplete ||
        iConnManStatus == EMsgConnManWaitingForNotifications ||
        iConnManStatus == EMsgConnManDisconnectingPrevious ||
        iConnManStatus == EMsgConnManIdle )
        {
        #ifdef _DEBUG
            WriteToLog( _L8( "  Status is %d, wrong state for this operation. Abort." ),
                                iConnManStatus );
        #endif
        User::RequestComplete( iClientStatus, KErrAbort );
        }
    else
        {
        //If bearer has been lost, we're already closed
        if( iConnManStatus != EMsgConnManBearerLost )
            {
            Cancel();
            iConnection.Close();
            iConnectionClosed = ETrue;
            iEventSubscribers.Reset();
            iConnectionTimer->ActivateTimer( iStatus, 4000000 );
            iConnManStatus = EMsgConnManDisconnectingPrevious;
            SetActive();
            }
        else
            {
            #ifdef _DEBUG
                WriteToLog( _L8( "  Bearer lost, nothing to do. Complete with KErrNone" ) );
            #endif
            User::RequestComplete( iClientStatus, KErrNone );
            }
        }
    #ifdef _DEBUG
        WriteToLog( _L8( " StopConnectionL() ends" ) );
    #endif
    }
    
// ----------------------------------------------------
// CMsgConnManager::BroadcastBearerEvent
// 
// ----------------------------------------------------
//
void CMsgConnManager::BroadcastBearerEventL( const TMsgBearerEvent aBearerEvent )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::BroadcastBearerEvent" ) );
    #endif
    TInt subs = iEventSubscribers.Count();
    #ifdef _DEBUG
        WriteToLog( _L8( " %d clients subscribed." ), subs );
    #endif
    if( subs > 0 )
        {
        TBool empty = EFalse;
        MMsgBearerObsCallback* client = NULL;
        for( TInt i = 0;i < subs && !empty;i++ )
            {
            //Just in case:
            //If a client has managed to remove a listener from the queue
            //in the previous callback, "i" will index a bad handle.
            subs = iEventSubscribers.Count();
            if( subs > 0 && i < subs )
                {
                client = iEventSubscribers[i];
                __ASSERT_ALWAYS( client != NULL, User::Panic( _L( "ObserverNULL!" ), 1 ) );
                TInt error = iProgressBuffer().iError;
                if( aBearerEvent == EMsgBearerLost )
                    {
                    #ifdef _DEBUG
                        TInt stage = iProgressBuffer().iStage;
                        WriteToLog( _L8( " Event is EMsgBearerLost" ) );
                        WriteToLog( _L8( "   Stage: %d" ), stage );
                        WriteToLog( _L8( "   Error: %d" ), error );
                    #endif
                    }
                TBool authClose = error == KErrConnectionTerminated;
                client->HandleBearerEventL( authClose, aBearerEvent );
                #ifdef _DEBUG
                    WriteToLog( _L8("  Client no. %d serviced" ), i );
                #endif
                }
            else
                {
                #ifdef _DEBUG
                    WriteToLog( _L8(
                    "  An event listener was removed inside the callback!" ) );
                #endif
                //Leave the for loop
                empty = ETrue;
                }
            }
        }
    #ifdef _DEBUG
        WriteToLog( _L8( " BroadcastBearerEvent() ends" ) );
    #endif
    }

// ----------------------------------------------------
// CMsgConnManager::IssueRequest
// 
// ----------------------------------------------------
//
void CMsgConnManager::IssueRequest()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::IssueRequest(), status: %d" ), IsActive() );
    #endif
    iStatus = KRequestPending;
    iConnection.ProgressNotification( iProgressBuffer, iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CMsgConnManager::StartConnection
//
// ---------------------------------------------------------
//
void CMsgConnManager::StartConnection( TRequestStatus& aStatus )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::StartConnection()  iConnManStatus: %d" ), iConnManStatus );
    #endif
    TInt error = 0;
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    if( iConnectionClosed )
        {
        error = iConnection.Open( iSocketSession );
        iConnectionClosed = EFalse;
        }
    TBool exists = EFalse;
    TNifProgress progress;
    error = iConnection.Progress( progress );
    #ifdef _DEBUG
        WriteToLog( _L8( " Connection stage: %d  error: %d" ), progress.iStage, error );
    #endif
    TRAPD( err, exists = IsValidApL( iAccessPointID ) );
    if( ( error == KErrNone && err == KErrNone ) && exists )
        {
        if( progress.iStage == KConnectionUninitialised ) 
            HandleNewConnection();
        else
            HandleExistingConnection();
        }
    else
        {
        #ifdef _DEBUG
            WriteToLog( _L8( " Something's wrong, complete with KErrUnknown. IAP exists: %d  Error: %d Err: %d" ), exists, error, err );
        #endif
        User::RequestComplete( iClientStatus, KErrUnknown );
        iConnectionClosed = ETrue;
        }
    }

// ---------------------------------------------------------
// CMsgConnManager::HandleNewConnection
// 
// ---------------------------------------------------------
//
void CMsgConnManager::HandleNewConnection()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "  CMsgConnManager::HandleNewConnection()" ) );
    #endif
    switch( iConnManStatus )
        {
        case EMsgConnManIdle:
            #ifdef _DEBUG
                WriteToLog( _L8( "   Current status is EMsgConnManIdle," ) );
                WriteToLog( _L8( "   starting to connect  IAP: %d" ), iAccessPointSettings.IapId() );
            #endif
            iLastConnectionStage = KConnectionUninitialised;
            iConnection.Start( iAccessPointSettings, iStatus );
            iConnManStatus = EMsgConnManWaitingForStartComplete;
            iConnectionTimer->ActivateTimer( this, 20000000 );
            SetActive();
            break;
        case EMsgConnManDisconnectingPrevious:
            #ifdef _DEBUG
                WriteToLog( _L8( "   Current status is EMsgConnManDisconnectingPrevious," ) );
                WriteToLog( _L8( "   new connection cannot be started, ignore." ) );
            #endif
            User::RequestComplete( iClientStatus, KErrNotReady );
            break;
        default:
            #ifdef _DEBUG
                WriteToLog( _L8( "   We're in some weird state, complete with KErrUnknown" ) );
            #endif
            User::RequestComplete( iClientStatus, KErrUnknown );
            iConnManStatus = EMsgConnManIdle;
            break;
        }   
    }

// ---------------------------------------------------------
// CMsgConnManager::HandleExistingConnection
// 
// ---------------------------------------------------------
//
void CMsgConnManager::HandleExistingConnection()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "  CMsgConnManager::HandleOldConnection()" ) );
    #endif
    switch( iConnManStatus )
        {
        case EMsgConnManDisconnectingPrevious:
            #ifdef _DEBUG
                WriteToLog( _L8( "   Current status is EMsgConnManDisconnectingPrevious," ) );
                WriteToLog( _L8( "   attempting to open a connection while previous one is still disconnecting. Complete with KErrNotReady." ) );
            #endif
            User::RequestComplete( iClientStatus, KErrNotReady );
            break;
        case EMsgConnManObservingAccessPoint:
            #ifdef _DEBUG
                WriteToLog( _L8( "   Current status is EMsgConnManObservingAccessPoint," ) );
                WriteToLog( _L8( "   already connected" ) );
            #endif
            User::RequestComplete( iClientStatus, KErrNone );
            break;
        case EMsgConnManWaitingForStartComplete:
        case EMsgConnManWaitingForNotifications:
            #ifdef _DEBUG
                WriteToLog( _L8( "   Already connecting, ignore" ) );
            #endif
            User::RequestComplete( iClientStatus, KErrAlreadyExists );
            break;
        default:
            #ifdef _DEBUG
                WriteToLog( _L8( "   Unknown state, complete with KErrUnknown." ) );
            #endif
            User::RequestComplete( iClientStatus, KErrUnknown );
        }
    }

// ---------------------------------------------------------
// CMsgConnManager::HandleTimerEventL
// 
// ---------------------------------------------------------
//
void CMsgConnManager::HandleTimerEvent( TInt /*aStatus*/ )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::HandleTimerEventL()" ) );
    #endif
    switch( iConnManStatus )
        {
        case EMsgConnManWaitingForNotifications:
        case EMsgConnManWaitingForStartComplete:
            Cancel();
            #ifdef _DEBUG
                WriteToLog( _L8( "  Connection initiation timed out, complete with error KErrTimedOut" ) );
            #endif
            User::RequestComplete( iClientStatus, KErrTimedOut );
            iConnection.Close();
            iConnectionClosed = ETrue;
            iConnManStatus = EMsgConnManIdle;
            break;
        default:
            #ifdef _DEBUG
                WriteToLog( _L8( "  Default case, problems..." ) );
            #endif
            break;
        }
    }

// ---------------------------------------------------------
// CMsgConnManager::AddEventSubscriberL
// 
// ---------------------------------------------------------
//
void CMsgConnManager::AddEventSubscriberL( MMsgBearerObsCallback* aClient )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::AddEventSubscriberL()" ) );
    #endif
    if( iEventSubscribers.Count() < KMaxNumberOfSubscribers )
        User::LeaveIfError( iEventSubscribers.Append( aClient ) );
    else
        {
        #ifdef _DEBUG
            WriteToLog( _L8( "  Not allowed to have more than %d clients" ), KMaxNumberOfSubscribers );
        #endif
        User::Leave( KErrAlreadyExists );
        }
    }

// ---------------------------------------------------------
// CMsgConnManager::RemoveEventSubscriber
// 
// ---------------------------------------------------------
//
void CMsgConnManager::RemoveEventSubscriber( MMsgBearerObsCallback* aClient )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::RemoveEventSubscriber()" ) );
    #endif
    TInt index = iEventSubscribers.Find( aClient );
    if( index >= 0 )
        {      
        iEventSubscribers.Remove( index );
        }
    }

#ifdef _DEBUG
// ----------------------------------------------------
// CMsgConnManager::WriteToLog
// 
// ----------------------------------------------------
//
void CMsgConnManager::WriteToLog( TRefByValue<const TDesC8> aFmt,... )                                 
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KConnManLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    RDebug::RawPrint( buf );
    RFileLogger::Write( KConnManLogDir, KConnManLogFile, EFileLoggingModeAppend, buf );
    }
#endif

// ----------------------------------------------------
// CMsgConnManager::SetAccessPointIDL
// 
// ----------------------------------------------------
//
void CMsgConnManager::SetAccessPointIDL( const TInt aAccessPointID )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::SetAccessPointIDL: %d" ), aAccessPointID );
    #endif
    if( IsValidApL( aAccessPointID ) )
        {
        iAccessPointID = aAccessPointID;
        iAccessPointSettings.SetIapId( iAccessPointID );
        iAccessPointSettings.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
        }
    else
        {
        #ifdef _DEBUG
            WriteToLog( _L8( "  The specified AP does not exist, leaves with KErrNotFound" ) );
        #endif
        User::Leave( KErrNotFound );
        }
    }

// ----------------------------------------------------
// CMsgConnManager::NumberOfActiveConns
// 
// ----------------------------------------------------
//
TInt CMsgConnManager::NumberOfActiveConns()
    {
    TUint count = 0;
    TInt error = KErrNone;
    if( !iConnectionClosed )
    	{
    	error = iConnection.EnumerateConnections( count );
    	}
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::NumberOfActiveConns()  Conns: %d  Error: %d" ), count, error );
    #endif
    return KErrNone == error ? count : error;
    }

// ---------------------------------------------------------
// CMsgConnManager::ReadFromCommsDbLC
// 
// ---------------------------------------------------------
//
HBufC* CMsgConnManager::ReadFromCommsDbLC( const TSupportedCommDbRecords aParameter )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::ReadFromCommsDbLC()" ) );
    #endif
    TPtrC dbRecord;
    HBufC* value = NULL;
    CCommsDbTableView* view = NULL;
    TInt error = InitCommDbViewLC( dbRecord, view, aParameter );
    if( error == KErrNone )
        {
        TDbColType colType;
        TUint32 attribute = 0;
        view->ReadTypeAttribL( dbRecord, colType, attribute );
        switch( colType )
            {
            case EDbColBit:
                break;
            case EDbColUint32:
                {
                TUint32 intValue = 0;
                _LIT( KProxyPort,"%d" );
                view->ReadUintL( dbRecord, intValue );
                value = HBufC::NewLC( sizeof( KProxyPort ) );
                TPtr portPtr( value->Des() );
                portPtr.Format( KProxyPort, intValue ); 
                value->Des().Copy( portPtr );    
                }
                break;
            case EDbColText8:
                break;
            case EDbColText16:
                break;
            case EDbColLongText16:
                value = view->ReadLongTextLC( dbRecord );
                break;
            default:
                break;
            }
            if( value != NULL )
                {
                #ifdef _DEBUG
                    HBufC8* temp = HBufC8::NewL( value->Des().Length() );
                    TPtr8 ptr( temp->Des() );
                    ptr.Copy( *value );
                    WriteToLog( _L8( "  Value was found: %S" ), &ptr );
                    delete temp;
                    temp = NULL;
                #endif
                CleanupStack::Pop();                //value
                CleanupStack::PopAndDestroy();      //view
                CleanupStack::PushL( value );       //Put it back in
                }
            else
                {
                #ifdef _DEBUG
                    WriteToLog( _L8( "  No proxy address defined in CommDb" ) );
                #endif
                CleanupStack::PopAndDestroy();      //view
                }
        }
    else
        {
        #ifdef _DEBUG
            WriteToLog( _L8( "  Error: %d" ), error );
        #endif
        }
    return value;
    }

// ---------------------------------------------------------
// CMsgConnManager::InitCommDbViewLC
// 
// ---------------------------------------------------------
//
TInt CMsgConnManager::InitCommDbViewLC( TPtrC& aDbRecord,
                                        CCommsDbTableView*& aView,
                                        const TSupportedCommDbRecords aParameter )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "  CMsgConnManager::InitCommDbViewLC" ) );
    #endif
    aView = iCommsDatabase->OpenViewMatchingUintLC
                            ( TPtrC( IAP ), TPtrC( COMMDB_ID ), iAccessPointID );
    TInt error = aView->GotoFirstRecord();
    if( error == KErrNone )
        {
        error = DetermineRecordTypeLC( aDbRecord, aView, aParameter );
        #ifdef _DEBUG
            if( error != KErrNone )
                WriteToLog( _L8( "    DetermineRecordType() failed - Error: %d" ), error );
        #endif
        }
    else
        {
        #ifdef _DEBUG
            WriteToLog( _L8( "    No matching view found - Error: %d" ), error );
        #endif
        CleanupStack::PopAndDestroy();  //aView
        aView = NULL;
        }
    return error;
    }

// ---------------------------------------------------------
// CMsgConnManager::DetermineRecordType
// 
// ---------------------------------------------------------
//
TInt CMsgConnManager::DetermineRecordTypeLC( TPtrC& aDbColumn,
                                             CCommsDbTableView*& aView,
                                             const TSupportedCommDbRecords aParameter )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "      CMsgConnManager::DetermineRecordType()" ) );
    #endif 
    TInt error = KErrNotFound;
    switch( aParameter )
        {
        case EMsgProxyAddress:
            #ifdef _DEBUG
                WriteToLog( _L8( "      Type is EImpsProxyAddress" ) );
            #endif
            if( CheckProxyUsageLC( aView ) )
                {
                error = KErrNone;
                aDbColumn.Set( TPtrC( PROXY_SERVER_NAME ) );
                }
            break;
        case EMsgProxyPort:
            #ifdef _DEBUG
                WriteToLog( _L8( "      Type is EImpsProxyPort" ) );
            #endif
            if( CheckProxyUsageLC( aView ) )
                {
                error = KErrNone;
                aDbColumn.Set( TPtrC( PROXY_PORT_NUMBER ) );
                }
            break;
        default:
            #ifdef _DEBUG
                WriteToLog( _L8( "      Default case, problems..." ) );
            #endif
            error = KErrNotSupported;
            break;
        }
    return error;
    }

// ---------------------------------------------------------
// CMsgConnManager::CheckProxyUsageLC
// 
// ---------------------------------------------------------
//
TBool CMsgConnManager::CheckProxyUsageLC( CCommsDbTableView*& aView )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::CheckProxyUsageL()" ) );
    #endif
    TBool useProxy = EFalse;
    TUint32 serviceIdNumber = 0;
    TBuf<KCommsDbSvrMaxFieldLength> serviceIdText;
    aView->ReadTextL( TPtrC( IAP_SERVICE_TYPE ), serviceIdText );
    aView->ReadUintL( TPtrC( IAP_SERVICE ), serviceIdNumber );
    CleanupStack::PopAndDestroy();  //aView
    aView = NULL;
    aView = iCommsDatabase->OpenViewOnProxyRecordLC( serviceIdNumber, serviceIdText );
    TInt error = aView->GotoFirstRecord();
    if( error == KErrNone )
        {
        aView->ReadBoolL( TPtrC( PROXY_USE_PROXY_SERVER ), useProxy );
        if( !useProxy )
            {
            CleanupStack::PopAndDestroy();  //aView
            aView = NULL;
            }
        }
    else
        {
        #ifdef _DEBUG
            WriteToLog( _L8( "  The AP does not use a proxy server." ) );
        #endif
        CleanupStack::PopAndDestroy();  //aView
        aView = NULL;
        }
    return useProxy;
    }

// ---------------------------------------------------------
// CMsgConnManager::IsValidApL
// 
// ---------------------------------------------------------
//
TBool CMsgConnManager::IsValidApL( TUint32 aUid )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::IsValidApL()" ) );
    #endif
    CCommsDbTableView* table = iCommsDatabase->OpenViewMatchingUintLC( TPtrC( IAP ),
                                                                       TPtrC( COMMDB_ID ),
                                                                       aUid );
    TInt res = table->GotoFirstRecord();
    CleanupStack::PopAndDestroy(); // table
    return res == KErrNone;
    }

// ---------------------------------------------------------
// CMsgConnManager::DoCancel
// 
// ---------------------------------------------------------
//
void CMsgConnManager::DoCancel()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CMsgConnManager::DoCancel()" ) );
    #endif
    switch( iConnManStatus )
        {
        case EMsgConnManWaitingForStartComplete:
            #ifdef _DEBUG
                WriteToLog( _L8( "  Status is EMsgConnManWaitingForStartComplete" ) );
            #endif
            iConnection.Close();
            iConnectionTimer->Cancel();
            break;
        case EMsgConnManWaitingForNotifications:
            #ifdef _DEBUG
                WriteToLog( _L8( "  Status is EMsgConnManWaitingForNotifications" ) );
            #endif
            iConnection.CancelProgressNotification();
            iConnectionTimer->Cancel();
            iConnection.Close();
            break;
        case EMsgConnManDisconnectingPrevious:
            #ifdef _DEBUG
                WriteToLog( _L8( "  Status is EMsgConnManDisconnectingPrevious" ) );
            #endif
            iConnectionTimer->Cancel();
            User::RequestComplete( iClientStatus, KErrCancel );
             #ifdef _DEBUG
                WriteToLog( _L8( "    StopConnection() terminated, client's request completed." ) );
            #endif
            break;
        case EMsgConnManObservingAccessPoint:
        case EMsgConnManBearerSuspended:
            {
            #ifdef _DEBUG
                WriteToLog( _L8( "  Status is %d" ), iConnManStatus );
            #endif
            iConnection.CancelProgressNotification();
            #ifdef __WINS__
                TRequestStatus* status = &iStatus;
                User::RequestComplete( status, KErrCancel );
            #endif
            }
            break;
        case EMsgConnManIdle:
            #ifdef _DEBUG
                WriteToLog( _L8( "  Status is EMsgConnManIdle" ) );
            #endif
            break;
        default:
            #ifdef _DEBUG
                WriteToLog( _L8( "  Default case" ) );
            #endif
            break;
        }
    }

// ----------------------------------------------------
// NewImpsConnManL
// Returns a ConnManager instance
// ----------------------------------------------------
//
EXPORT_C MMsgConnManager* NewMsgConnManagerL( const TInt aDefaultAccessPoint ) 
    { 
    return CMsgConnManager::NewL( aDefaultAccessPoint ); 
    }


