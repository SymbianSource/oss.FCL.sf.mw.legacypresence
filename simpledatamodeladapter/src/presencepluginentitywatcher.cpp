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
#include <e32svr.h>
#include <utf.h>
#include <ximpdatasubscriptionstate.h>
#include <protocolpresencewatchingdatahost.h>
#include <ximpobjectfactory.h>
#include <presenceinfo.h>
#include <ximpprotocolconnectionhost.h>
#include <protocolpresencedatahost.h>
#include <presenceobjectfactory.h>
#include <ximpstatus.h>
#include <ximpidentity.h>
#include <simplefactory.h>
#include <msimplewatcher.h>
#include <msimpledocument.h>
#include <msimpleconnection.h>
#include <simpleerrors.h>
#include <avabilitytext.h>

#include "presencepluginentitywatcher.h"
#include "presenceplugindata.h"
#include "presencepluginwatcher.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::CPresencePluginEntityWatcher()
// ---------------------------------------------------------------------------
//
CPresencePluginEntityWatcher::CPresencePluginEntityWatcher(
    MPresencePluginConnectionObs& aObs,
    MSimpleConnection& aConn,
    CPresencePluginWatcher& aWatcher )
    :iConnObs(aObs), iConnection(aConn),
    iPluginWatcher(aWatcher), iOperation( EPluginIdle )
    {
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::ConstructL()
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::ConstructL( 
    CPresencePluginData* aPresenceData )
    { 
    DP_SDA("CPresencePluginEntityWatcher::ConstructL - begin");
    iWatcher = TSimpleFactory::NewWatcherL( iConnection, *this );
    iPresenceData = aPresenceData; 
    DP_SDA("CPresencePluginEntityWatcher::ConstructL - end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::NewL()
// ---------------------------------------------------------------------------
//
CPresencePluginEntityWatcher* CPresencePluginEntityWatcher::NewL(
    MPresencePluginConnectionObs& aObs,
    MSimpleConnection& aConn,
    CPresencePluginWatcher& aWatcher,
    CPresencePluginData* aPresenceData )
    {
    DP_SDA("CPresencePluginEntityWatcher::NewL");
    CPresencePluginEntityWatcher* self =
        new( ELeave ) CPresencePluginEntityWatcher( aObs, aConn, aWatcher );
    CleanupStack::PushL( self );
    self->ConstructL( aPresenceData );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::~CPresencePluginEntityWatcher()
// ---------------------------------------------------------------------------
//
CPresencePluginEntityWatcher::~CPresencePluginEntityWatcher()
    {
    DP_SDA("CPresencePluginEntityWatcher::~CPresencePluginEntityWatcher");
    if ( iWatcher )
        {
        iWatcher->Close();
        }
    delete iEntityId;
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::StartSubscribeL()
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::StartSubscribeL(
    const TDesC8& aPresentityId )
    {
    DP_SDA("CPresencePluginEntityWatcher::StartSubscribeL");
    delete iEntityId; 
    iEntityId = NULL; 
    DP_SDA(" StartSubscribeL - entity id deleted");
    
    TRAPD( error, iSimpleId = iWatcher->SubscribeL( 
        aPresentityId, NULL, ETrue, EFalse ););
    DP_SDA2("StartSubscribeL subscribe error %d",error);
    
    if( KErrNone != error )
        {
        DP_SDA("CPresencePluginEntityWatcher::StartSubscribeL ERROR");
        if( KErrInUse == error )
            {
            DP_SDA(" iWatcher is in use try to close and restart");
            iWatcher->Close();
            iWatcher = TSimpleFactory::NewWatcherL( iConnection, *this );
            DP_SDA("StartSubscribeL Second try after creating iWatcher again");
            error = KErrNone;
            TRAP( error, iSimpleId = iWatcher->GetPresenceL( 
                aPresentityId, NULL, EFalse ););
            DP_SDA2("iWatcher->GetPresenceL error2 = %d",error );
            }
        }
    
    // Save entity id after successful call 
    DP_SDA("StartSubscribeL - allocate entity id");
    iEntityId = aPresentityId.AllocL();
    DP_SDA("StartSubscribeL - new entity id stored");
    
    iOperation = EPluginStart;
    
    DP_SDA("CPresencePluginEntityWatcher::StartSubscribeL End");
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::StartSubscribeL()
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::StartSubscribeL(
    const TDesC8& aPresentityId, TRequestStatus& aClientRequst )
    {
    DP_SDA("CPresencePluginEntityWatcher::StartSubscribeL 2");
    delete iEntityId; 
    iEntityId = NULL; 
    DP_SDA("CPresencePluginEntityWatcher::StartSubscribeL - entity id deleted");
    
    iClientStatus = &aClientRequst;
    *iClientStatus = KRequestPending; // wait for watcher complete
    iOperation = EPluginStartNotifyClient; 
    
    TRAPD( error, iSimpleId = iWatcher->SubscribeL( 
        aPresentityId, NULL, ETrue, EFalse ););
    DP_SDA2("StartSubscribeL subscribe error %d",error);
    
    if( KErrInUse == error )
        {
        DP_SDA("CPresencePluginEntityWatcher::StartSubscribeL ERROR");	
        DP_SDA("StartSubscribeL iWatcher is in use try to close and restart");
        iWatcher->Close();
        iWatcher = TSimpleFactory::NewWatcherL( iConnection, *this );
        DP_SDA("StartSubscribeL Second try after creating iWatcher again");
        error = KErrNone;
        TRAP( error, iSimpleId = iWatcher->GetPresenceL( 
            aPresentityId, NULL, EFalse ););
        DP_SDA2("iWatcher->GetPresenceL error2 = %d",error );
        }
    User::LeaveIfError( error );
    
    // Save entity id after successful call 
    DP_SDA("StartSubscribeL - allocate entity id");
    iEntityId = aPresentityId.AllocL(); 
    DP_SDA("StartSubscribeL - new entity id stored");
    
    DP_SDA("CPresencePluginEntityWatcher::StartSubscribeL End"); 
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::StopSubscribeL()
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::StopSubscribeL( 
    TRequestStatus& aClientRequst )
    {
    DP_SDA("CPresencePluginEntityWatcher::StopSubscribeL 2 (list)");
    iClientStatus = &aClientRequst;
    *iClientStatus = KRequestPending; // wait for watcher complete    
    
    iSimpleId = iWatcher->UnsubscribeL();
    iOperation = EPluginStop;
    DP_SDA("CPresencePluginEntityWatcher::StopSubscribeL 2 end"); 
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::StopSubscribeL()
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::StopSubscribeL( )
    {
    DP_SDA("CPresencePluginEntityWatcher::StopSubscribeL");
    iSimpleId = iWatcher->UnsubscribeL();
    iOperation = EPluginStop;
    DP_SDA("CPresencePluginEntityWatcher::StopSubscribeL end"); 
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::WatcherReqCompleteL
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::WatcherReqCompleteL( 
    TInt /*aOpId*/, TInt aStatus )
    {
    DP_SDA("CPresencePluginEntityWatcher::WatcherReqCompleteL");
    DP_SDA2("WatcherReqCompleteL status %d",aStatus );
    TPluginEntityWatcherOperation orig = iOperation;
    DP_SDA2("WatcherReqCompleteL orig %d",orig );
    iOperation = EPluginIdle;
    
    if( !aStatus )
        {
        iPluginWatcher.AcceptL( this );
        }
    
    if ( EPluginStartNotifyClient == orig )
        {
        orig = EPluginStart;
        DP_SDA(" WatcherReqCompleteL complete client");
        CompleteClientReq( aStatus );
        }
    else if ( iClientStatus )
        {
        DP_SDA(" WatcherReqCompleteL complete client 2");
        CompleteClientReq( aStatus );
        }
    
    MSimpleWatcher::TSimpleSipSubscriptionState subscribeStatus =
        iWatcher->SipSubscriptionState();
    DP_SDA2("WatcherReqCompleteL subscribe status %d",subscribeStatus );
     	
    if ( aStatus && ( orig == EPluginStop || orig == EPluginStart &&
        MSimpleWatcher::ESimpleStateTerminated != subscribeStatus ) )
        {
        DP_SDA("CPresencePluginEntityWatcher::WatcherReqCompleteL if");
        // Delete this entity as useless
        DP_SDA("WatcherReqCompleteL DELETE WATCHER");
        iPluginWatcher.DeleteWatcher( iEntityId->Des() ); 
        //Do not call anything, 
        //since the method call above deletes this instance.
        }
    else if( !aStatus && ( MSimpleWatcher::ESimpleStateActive ==
        subscribeStatus  &&
        orig == EPluginStop ) )
        {
        DP_SDA(" WatcherReqCompleteL state STOP");	
        // Delete this entity as useless
        DP_SDA("WatcherReqCompleteL DELETE WATCHER 2");
        iPluginWatcher.DeleteWatcher( iEntityId->Des() );
        }
    else if ( MSimpleWatcher::ESimpleStatePending == subscribeStatus )
        {
        SetPendingToXIMPL();
        }
    DP_SDA("CPresencePluginEntityWatcher::WatcherReqCompleteL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::SetPendingToXIMPL
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::SetPendingToXIMPL()
    {
    DP_SDA("CPresencePluginEntityWatcher::SetPendingToXIMPL");
    //check what is grand request list subscribe state
    if( iConnObs.GrandListState() )
        {
        DP_SDA("CPresencePluginEntityWatcher::SetPendingToXIMPL True");
        MProtocolPresenceWatchingDataHost& watcherHost =    
        iConnObs.ProtocolPresenceHost().WatchingDataHost();
        MPresenceInfo* prInfo = 
            iConnObs.PresenceObjectFactoryOwn().NewPresenceInfoLC();
        
        //Call set to pending
        iPresenceData->NotifyToPendingToXIMPL( 
            iConnObs.PresenceObjectFactoryOwn(),
            *prInfo );
            
        MXIMPIdentity* identity2 = iConnObs.ObjectFactory().NewIdentityLC();    
        HBufC* uniBuffer = 
            CnvUtfConverter::ConvertToUnicodeFromUtf8L( iEntityId->Des() );
        CleanupStack::PushL( uniBuffer );
        
        HBufC* withoutPrefix = iPresenceData->RemovePrefixLC( *uniBuffer );
        identity2->SetIdentityL( *withoutPrefix );    
        
        //Call presence cache writer too
        TBuf<20> buf;
        buf.Copy( KPendingRequestExtensionValue );
        
        iPresenceData->WriteStatusToCacheL( *withoutPrefix, 
            MPresenceBuddyInfo2::ENotAvailable,
            buf,
            KNullDesC() ); 
        
        // XIMP Host API callback 
        watcherHost.HandleSubscribedPresentityPresenceL( identity2, prInfo );
        
        CleanupStack::PopAndDestroy( withoutPrefix );
        CleanupStack::PopAndDestroy( uniBuffer );
        CleanupStack::Pop();        // >> identity2
        CleanupStack::Pop();        // >> prInfo
        }
    DP_SDA("CPresencePluginEntityWatcher::SipSubscriptionState end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::SetActiveToXIMPL
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::SetActiveToXIMPL( const TDesC& aIdentity )
    {
    DP_SDA("CPresencePluginEntityWatcher::SetActiveToXIMPL, identity");
    //check what is grand request list subscribe state
    if( iConnObs.GrandListState() )
        {
        DP_SDA("CPresencePluginEntityWatcher::SetPendingToXIMPL True");
        MProtocolPresenceWatchingDataHost& watcherHost =    
        iConnObs.ProtocolPresenceHost().WatchingDataHost();
        MPresenceInfo* prInfo = 
            iConnObs.PresenceObjectFactoryOwn().NewPresenceInfoLC();
        
        //Call set to pending
        DP_SDA(" SetPendingToXIMPL, notify active");
        iPresenceData->NotifyToActiveToXIMPL( 
            iConnObs.PresenceObjectFactoryOwn(),
            *prInfo );

        DP_SDA(" SetActiveToXIMPL, create new identity");
        MXIMPIdentity* identity2 = iConnObs.ObjectFactory().NewIdentityLC();    

        DP_SDA(" SetActiveToXIMPL, strip prefix");
        HBufC* withoutPrefix = iPresenceData->RemovePrefixLC( aIdentity );
        identity2->SetIdentityL( *withoutPrefix );    

        DP_SDA(" SetActiveToXIMPL, write to cache");
        //Call presence cache writer too
        iPresenceData->WriteStatusToCacheL( *withoutPrefix, 
            MPresenceBuddyInfo2::ENotAvailable,
            KDefaultAvailableStatus(),
            KNullDesC() );    

        DP_SDA(" SetActiveToXIMPL, ximp api callback");
        // XIMP Host API callback 
        watcherHost.HandleSubscribedPresentityPresenceL( identity2, prInfo );
        
        DP_SDA("CPresencePluginEntityWatcher::SetActiveToXIMPL, cleanup");
        CleanupStack::PopAndDestroy( withoutPrefix );
        CleanupStack::Pop();        // >> identity2
        CleanupStack::Pop();        // >> prInfo
        }   
    DP_SDA("CPresencePluginEntityWatcher::SetActiveToXIMPL end");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::SetTerminatedToXIMPL
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::SetTerminatedToXIMPL( 
	const TDesC& aIdentity )
    {
    DP_SDA("CPresencePluginEntityWatcher::SetTerminatedToXIMPL");
    DP_SDA2("CPresencePluginEntityWatcher::SetTerminatedToXIMPL, uri: %S", 
        &aIdentity );    
    //check what is grand request list subscribe state
    if( iConnObs.GrandListState() )
        {
        DP_SDA("CPresencePluginEntityWatcher::SetTerminatedToXIMPL True");
        MProtocolPresenceWatchingDataHost& watcherHost =    
        iConnObs.ProtocolPresenceHost().WatchingDataHost();
        MPresenceInfo* prInfo = 
            iConnObs.PresenceObjectFactoryOwn().NewPresenceInfoLC();
        
        //Call set to pending
        iPresenceData->NotifyTerminatedToXIMPL( 
            iConnObs.PresenceObjectFactoryOwn(),
            *prInfo );

        MXIMPIdentity* identity2 = iConnObs.ObjectFactory().NewIdentityLC();    
        
        HBufC* withoutPrefix = iPresenceData->RemovePrefixLC( aIdentity );
        identity2->SetIdentityL( *withoutPrefix );   
        
        // XIMP Host API callback 
        watcherHost.HandleSubscribedPresentityPresenceL( identity2, prInfo );

        CleanupStack::PopAndDestroy( withoutPrefix );
        CleanupStack::Pop();        // >> identity2
        CleanupStack::Pop( );        // >> prInfo
        }   
    DP_SDA("CPresencePluginEntityWatcher::SetTerminatedToXIMPL end");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::SetActiveToXIMPL
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::SetActiveToXIMPL( 
	MSimpleDocument& aDocument )
    {
    DP_SDA("CPresencePluginEntityWatcher::SetActiveToXIMPL, aDocument");
    // No need to check the expiration here since WatcherTerminatedL
    // is called then too.
    MProtocolPresenceWatchingDataHost& watcherHost =    
        iConnObs.ProtocolPresenceHost().WatchingDataHost();
    MPresenceInfo* prInfo = 
        iConnObs.PresenceObjectFactoryOwn().NewPresenceInfoLC();

    DP_SDA(" SetActiveToXIMPL, call NotifyToPrInfoL");
    
    iPresenceData->NotifyToPrInfoL( 
        iConnObs.ObjectFactory(),
        iConnObs.PresenceObjectFactoryOwn(),
        aDocument, *prInfo );
    DP_SDA(" SetActiveToXIMPL, NotifyToPrInfoL ok");
    
    // Get variables from presence info object for cache entry
    MPresenceBuddyInfo2::TAvailabilityValues availability = 
        MPresenceBuddyInfo2::ENotAvailable;
    
    HBufC* extendedAvailability = HBufC::NewLC( KBufSize255 );
    TPtr extendedAvailabilityPtr( extendedAvailability->Des() );
    
    HBufC* statusMessage = HBufC::NewLC( KBufSize255 );
    TPtr statusMessagePtr( statusMessage->Des() );
    
    DP_SDA(" SetActiveToXIMPL, NotifyToPrInfoL ok, get cache entries");
    iPresenceData->CacheEntriesFromPrInfo( *prInfo,
        availability, extendedAvailabilityPtr, statusMessagePtr );
    
    DP_SDA(" SetActiveToXIMPL, create identity");
    MXIMPIdentity* identity2 = iConnObs.ObjectFactory().NewIdentityLC();    
    HBufC* uniBuffer = 
        CnvUtfConverter::ConvertToUnicodeFromUtf8L( 
            *aDocument.EntityURI() );
    CleanupStack::PushL( uniBuffer );
    DP_SDA("CPresencePluginEntityWatcher::SetActiveToXIMPL, identity ok");
    
    DP_SDA("CPresencePluginEntityWatcher::SetActiveToXIMPL, strip prefix");
    HBufC* withoutPrefix = iPresenceData->RemovePrefixLC( *uniBuffer );
    identity2->SetIdentityL( *withoutPrefix );   

    //Call presence cache writer too
    iPresenceData->WriteStatusToCacheL( *withoutPrefix, availability,
         extendedAvailabilityPtr, statusMessagePtr );
    
    // XIMP Host API callback 
    watcherHost.HandleSubscribedPresentityPresenceL( identity2, prInfo );

    CleanupStack::PopAndDestroy( withoutPrefix );
    CleanupStack::PopAndDestroy( uniBuffer );
    CleanupStack::Pop();        // >> identity2
    
    CleanupStack::PopAndDestroy( statusMessage );
    CleanupStack::PopAndDestroy( extendedAvailability );
    
    CleanupStack::Pop();        // >> prInfo
    DP_SDA("CPresencePluginEntityWatcher::SetActiveToXIMPL end");   
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::SetTerminatedToXIMPL
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::SetTerminatedToXIMPL( )
    {
    DP_SDA("CPresencePluginEntityWatcher::SetTerminatedToXIMPL");
    DP_SDA("CPresencePluginEntityWatcher::SetTerminatedToXIMPL True");
    
    MProtocolPresenceWatchingDataHost& watcherHost = 
        iConnObs.ProtocolPresenceHost().WatchingDataHost();
    MPresenceInfo* prInfo = 
        iConnObs.PresenceObjectFactoryOwn().NewPresenceInfoLC();
    
    //Call set to pending
    iPresenceData->NotifyTerminatedToXIMPL( 
        iConnObs.PresenceObjectFactoryOwn(),
        *prInfo );
    
    MXIMPIdentity* identity2 = iConnObs.ObjectFactory().NewIdentityLC();
    HBufC* uniBuffer = 
       CnvUtfConverter::ConvertToUnicodeFromUtf8L( iEntityId->Des() );
    
    CleanupStack::PushL( uniBuffer );
    HBufC* withoutPrefix = iPresenceData->RemovePrefixLC( *uniBuffer );
    identity2->SetIdentityL( *withoutPrefix ); 
    
    //Call presence cache writer too
    iPresenceData->WriteStatusToCacheL( *withoutPrefix, 
        MPresenceBuddyInfo2::ENotAvailable,
        KInvisibleState(),
        KNullDesC() );
    
    // XIMP Host API callback 
    watcherHost.HandleSubscribedPresentityPresenceL( identity2, prInfo );
    
    DP_SDA2("SetTerminatedToXIMPL2: ident %S", &identity2->Identity() );
    CleanupStack::PopAndDestroy( withoutPrefix );
    CleanupStack::PopAndDestroy( uniBuffer );
    CleanupStack::Pop(); // >> identity21
    CleanupStack::Pop(); // >> prInfo
    
    DP_SDA("CPresencePluginEntityWatcher::SetTerminatedToXIMPL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::WatcherNotificationL()
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::WatcherNotificationL( 
    MSimpleDocument& aDocument )
    {
    DP_SDA("CPresencePluginEntityWatcher::WatcherNotificationL");
    
    MSimpleWatcher::TSimpleSipSubscriptionState subscribeStatus =
        iWatcher->SipSubscriptionState();
    DP_SDA2("WatcherNotificationL subscribe status %d",subscribeStatus );
    
    //If subscribeStatus = pending
    if( MSimpleWatcher::ESimpleStatePending == subscribeStatus )
        {
        // Pending
        DP_SDA("CPresencePluginEntityWatcher::WatcherNotificationL PENDING");
        SetPendingToXIMPL();
        }
    else if( MSimpleWatcher::ESimpleStateTerminated == subscribeStatus )
        {
        DP_SDA("CPresencePluginEntityWatcher::WatcherNotificationL TERMINATED");
        SetTerminatedToXIMPL( );
        }
    else
        {
        //Active
        DP_SDA("WatcherNotificationL normal Case");
        SetActiveToXIMPL( aDocument );
        }
   	DP_SDA("CPresencePluginEntityWatcher::WatcherNotificationL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::WatcherListNotificationL()
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::WatcherListNotificationL(
    MSimplePresenceList& aList )
    {
    DP_SDA("CPresencePluginEntityWatcher::WatcherListNotificationL");
    // call all the necessary callbacks, for new data + teminated ones.
    
    MProtocolPresenceWatchingDataHost& watcherHost = 
         iConnObs.Host()->ProtocolPresenceDataHost().WatchingDataHost();
    
    RPointerArray<MPresenceInfo> entities;
    RPointerArray<MXIMPIdentity> terminated;
    RPointerArray<MXIMPIdentity> actives;
    
    TRAPD( err, iPresenceData->NotifyListToPrInfoL(
         iConnObs.ObjectFactory(),iConnObs.PresenceObjectFactoryOwn(),
          aList, entities, actives, terminated ));
    if ( err )
        {
        entities.ResetAndDestroy();
        terminated.ResetAndDestroy();
        actives.ResetAndDestroy();
        entities.Close();
        terminated.Close();
        actives.Close();
        return;
        }
    
    // Start to collect data for HandleSubscribedPresentityPresenceL
    TInt counter = entities.Count();
    for ( TInt i = counter-1 ; i>=0; i-- )
        {
        HBufC* withoutPrefix = iPresenceData->RemovePrefixLC( 
            actives[i]->Identity() );
        actives[i]->SetIdentityL( *withoutPrefix );    
        
        watcherHost.HandleSubscribedPresentityPresenceL(
            actives[i], entities[i] );
        // Owenership is transferred
        actives.Remove(i);
        entities.Remove(i);
        CleanupStack::PopAndDestroy( withoutPrefix );
        }
    
    entities.ResetAndDestroy();
    terminated.ResetAndDestroy();
    actives.ResetAndDestroy();
    
    entities.Close();
    terminated.Close();
    actives.Close();
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::WatcherTerminatedL()
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::WatcherTerminatedL(
    TInt /*aOpId*/, TInt aReason )
    {
    DP_SDA2("CPresencePluginEntityWatcher::WatcherTerminatedL -reason: %d", aReason );
    
    MProtocolPresenceWatchingDataHost& watcherHost = 
        iConnObs.Host()->ProtocolPresenceDataHost().WatchingDataHost();
    
    MXIMPDataSubscriptionState *state =
        iConnObs.ObjectFactory().NewDataSubscriptionStateLC();
    MXIMPStatus* status = iConnObs.ObjectFactory().NewStatusLC();
    state->SetSubscriptionStateL( 
        MXIMPDataSubscriptionState::ESubscriptionInactive );
    status->SetResultCode( aReason );
    MXIMPIdentity* identity = iConnObs.ObjectFactory().NewIdentityLC();
    
    HBufC16* buf16 = HBufC16::NewLC( PresentityId().Length());
    buf16->Des().Copy( PresentityId() );
    identity->SetIdentityL( buf16->Des() );
    
    //Visualize closed state OpenSer
    SetTerminatedToXIMPL( buf16->Des() );
    
    CleanupStack::PopAndDestroy( buf16 );
    watcherHost.SetPresentityPresenceDataSubscriptionStateL( 
        identity, state, status );
    DP_SDA("CPresencePluginEntityWatcher::WatcherTerminatedL pop");
    CleanupStack::Pop( 3 );
    
    iPluginWatcher.DeleteWatcher( iEntityId->Des() ); 
    
    DP_SDA("CPresencePluginEntityWatcher::WatcherTerminatedL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::CompleteClientReq()
// ---------------------------------------------------------------------------
//
void CPresencePluginEntityWatcher::CompleteClientReq( TInt aStatus )
    {
    DP_SDA("CPresencePluginEntityWatcher::CompleteClientReq"); 
    iOperation = EPluginIdle;    
    TRequestStatus* s = iClientStatus;
    User::RequestComplete( s, aStatus );
    iClientStatus = NULL;
    DP_SDA("CPresencePluginEntityWatcher::CompleteClientReq out"); 
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::PresentityId
// ---------------------------------------------------------------------------
//
TPtrC8 CPresencePluginEntityWatcher::PresentityId( )
    {
    DP_SDA("CPresencePluginEntityWatcher::PresentityId");
    return iEntityId ? iEntityId->Des() : TPtrC8();
    }

// ---------------------------------------------------------------------------
// CPresencePluginEntityWatcher::XIMPRequestId
// ---------------------------------------------------------------------------
//
TXIMPRequestId CPresencePluginEntityWatcher::XIMPRequestId( )
    {
    return iXIMPId;  
    }

// End of file
