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
#include <utf.h>

#include <ximpdatasubscriptionstate.h>
#include <personpresenceinfo.h>
#include <protocolpresencewatchingdatahost.h>
#include <presenceinfo.h>
#include <presenceinfofieldcollection.h>
#include <presenceinfofield.h>
#include <presenceinfofieldvaluetext.h>
#include <presenceinfofieldvalueenum.h>
#include <presenceobjectfactory.h>
#include <presentitygroupmemberinfo.h>
#include <protocolpresencedatahost.h>
#include <protocolpresentitygroupsdatahost.h>
#include <ximpobjectcollection.h>
#include <ximpobjectfactory.h>
#include <ximpprotocolconnectionhost.h>
#include <ximpstatus.h>
#include <ximpidentity.h>

#include <simplefactory.h>

#include <msimplewinfo.h>
#include <msimplewatcher.h>
#include <msimpledocument.h>
#include <msimpleelement.h>

#include "simpleplugincommon.h"
#include "simplepluginentitywatcher.h"
#include "simpleplugindebugutils.h"
#include "simpleplugindata.h"
#include "simpleutils.h"
#include "simplepluginwatcher.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::CSimplePluginEntityWatcher
// ---------------------------------------------------------------------------
//
CSimplePluginEntityWatcher::CSimplePluginEntityWatcher(
    MSimplePluginConnectionObs& aObs,
    MSimpleConnection& aConn,
    CSimplePluginWatcher& aWatcher
     )
  : iPluginWatcher(aWatcher), iConnObs(aObs), iConnection(aConn),
  iOperation(EPluginIdle)
    {
    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::ConstructL
// ---------------------------------------------------------------------------
//
void CSimplePluginEntityWatcher::ConstructL( )
    {
    iWatcher = TSimpleFactory::NewWatcherL( iConnection, *this );
    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::NewL
// ---------------------------------------------------------------------------
//
CSimplePluginEntityWatcher* CSimplePluginEntityWatcher::NewL(
    MSimplePluginConnectionObs& aObs,
    MSimpleConnection& aConn,
    CSimplePluginWatcher& aWatcher )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginEntityWatcher: NewL"));
#endif
    CSimplePluginEntityWatcher* self =
        new( ELeave ) CSimplePluginEntityWatcher( aObs, aConn, aWatcher );
    CleanupStack::PushL( self );
    self->ConstructL(  );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::~CSimplePluginEntityWatcher
// ---------------------------------------------------------------------------
//
CSimplePluginEntityWatcher::~CSimplePluginEntityWatcher()
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginEntityWatcher: DESTRUCTOR"));
#endif
    if ( iWatcher )
        {
        iWatcher->Close();
        }
    delete iEntityId;
    delete iSimpleEntityId;
    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::StartSubscribeL
// ---------------------------------------------------------------------------
//
void CSimplePluginEntityWatcher::StartSubscribeL(
    const TDesC8& aPresentityId )
    {
    delete iEntityId;
    iEntityId = NULL;

    iListSubsActive = EFalse;

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginEntityWatcher: -> SubscribeL"));
#endif
    iSimpleId = iWatcher->SubscribeL( aPresentityId, NULL, ETrue, EFalse );

    // Save entity id after successful call
    iEntityId = aPresentityId.AllocL();

    iOperation = EPluginStart;
    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::StartSubscribeListL
// ---------------------------------------------------------------------------
//
void CSimplePluginEntityWatcher::StartSubscribeListL(
    const TDesC8& aPresentityId )
    {
    delete iEntityId;
    iEntityId = NULL;

    iListSubsActive = ETrue;

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginEntityWatcher: -> SubscribeListL"));
#endif
    iSimpleId = iWatcher->SubscribeListL( aPresentityId, NULL, ETrue, EFalse );

    // Save entity id after successful call
    iEntityId = aPresentityId.AllocL();

    iOperation = EPluginStart;
    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::SetSimpleNameL
// ---------------------------------------------------------------------------
//
void CSimplePluginEntityWatcher::SetSimpleNameL(
    const TDesC8& aPresentityId )
    {
    delete iSimpleEntityId;
    iSimpleEntityId = NULL;


    // Save entity id after successful call
    iSimpleEntityId = aPresentityId.AllocL();

    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::StopSubscribeL
// ---------------------------------------------------------------------------
//
void CSimplePluginEntityWatcher::StopSubscribeL(
     )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginEntityWatcher: -> UnsubscribeL"));
#endif
    iSimpleId = iWatcher->UnsubscribeL( );
    iOperation = EPluginStop;
    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::WatcherReqCompleteL
// ---------------------------------------------------------------------------
//
void CSimplePluginEntityWatcher::WatcherReqCompleteL(
    TInt /*aOpId*/, TInt aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginEntityWatcher: WatcherReqCompleteL"));
#endif
    TPluginEntityWatcherOperation orig = iOperation;
    iOperation = EPluginIdle;

    CompleteClientReq( aStatus );

    if ( aStatus && ( orig == EPluginStop || orig == EPluginStart) )
        {
#ifdef _DEBUG
        PluginLogger::Log(_L("PluginEntityWatcher: calls DeleteWatcher **"));
#endif
        // Delete this entity as useless
        iPluginWatcher.DeleteWatcher( iEntityId->Des() );
        // Do not call anything, since the method call above deletes this instance.
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::WatcherNotificationL
// ---------------------------------------------------------------------------
//
void CSimplePluginEntityWatcher::WatcherNotificationL(
    MSimpleDocument& aDocument )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginEntityWatcher: WatcherNotificationL" ));
#endif

    // No need to check the expiration here since WatcherTerminatedL
    // is called then too.
    MProtocolPresenceWatchingDataHost& watcherHost =
        iConnObs.Host()->ProtocolPresenceDataHost().WatchingDataHost();
    MPresenceInfo* prInfo =  iConnObs.PresenceObjectFactory().NewPresenceInfoLC();

    CSimplePluginData::NotifyToPrInfoL( iConnObs.PresenceObjectFactory(), aDocument, *prInfo );

#ifdef _DEBUG
    // ---------------------------------------------------------
    const MPersonPresenceInfo* pers_debug = prInfo->PersonPresence();
    const MPresenceInfoFieldCollection& coll_debug = pers_debug->Fields();
    TInt count_debug = coll_debug.FieldCount();
    PluginLogger::Log(_L("PluginEntityWatcher: nbr of fields received =%d"), count_debug );
    // ---------------------------------------------------------
#endif

    MXIMPIdentity* identity2 = iConnObs.ObjectFactory().NewIdentityLC();
    HBufC* uniBuffer =
        CnvUtfConverter::ConvertToUnicodeFromUtf8L( *aDocument.EntityURI() );
    CleanupStack::PushL( uniBuffer );               // << uniBuffer
    identity2->SetIdentityL( uniBuffer->Des() );
    CleanupStack::PopAndDestroy( uniBuffer );  // >> uniBuffer

    if ( iListSubsActive )
        {
        // Set Group Id
        HBufC* uniBuffer2 =
            CnvUtfConverter::ConvertToUnicodeFromUtf8L( iEntityId->Des() );
        CleanupStack::PushL( uniBuffer2 );
        MXIMPIdentity* gId = iConnObs.ObjectFactory().NewIdentityLC(); // << gId
        gId->SetIdentityL( uniBuffer2->Des() );

        MXIMPObjectCollection* coll =
            iConnObs.ObjectFactory().NewObjectCollectionLC();    // << coll

       iPluginWatcher.GetEntitiesInListL( uniBuffer2->Des(), *coll );

        MProtocolPresentityGroupsDataHost& groupHost =
            iConnObs.Host()->ProtocolPresenceDataHost().GroupsDataHost();
#ifdef _DEBUG
        PluginLogger::Log(
        _L("PluginEntityWatcher: callback HandlePresentityGroupContentL"));
#endif
        groupHost.HandlePresentityGroupContentL( gId, coll );

        CleanupStack::Pop( 2 ); // >> gId, coll
        CleanupStack::PopAndDestroy( uniBuffer2 );

        // This is done only for first notification for group subscription
        iListSubsActive = EFalse;
        }

    // PrFw Host API callback
#ifdef _DEBUG
    PluginLogger::Log(
        _L("PluginEntityWatcher: callback HandleSubscribedPresentityPresenceL"));
#endif
    watcherHost.HandleSubscribedPresentityPresenceL( identity2, prInfo );
    CleanupStack::Pop();        // >> identity2
    CleanupStack::Pop();        // >> prInfo
    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::WatcherListNotificationL
// ---------------------------------------------------------------------------
//
void CSimplePluginEntityWatcher::WatcherListNotificationL(
    MSimplePresenceList& aList )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginEntityWatcher: WatcherListNotificationL" ));
#endif

    // call all the necessary callbacks, for new data + teminated ones.

    // check out whether fullstate list or not
    TBool fullState( EFalse );

    MProtocolPresenceWatchingDataHost& watcherHost =
        iConnObs.Host()->ProtocolPresenceDataHost().WatchingDataHost();

    RPointerArray<MPresenceInfo> entities;
    RPointerArray<MXIMPIdentity> terminated;
    RPointerArray<MXIMPIdentity> allEntities;

    TRAPD( err, CSimplePluginData::NotifyListToPrInfoL(
        iConnObs.ObjectFactory(), iConnObs.PresenceObjectFactory(),
        aList, entities, allEntities, terminated, fullState ));
    if ( err )
        {
#ifdef _DEBUG
        PluginLogger::Log(_L("PluginEntityWatcher: WatcherListNotificationL error *" ));
#endif
        entities.ResetAndDestroy();
        terminated.ResetAndDestroy();
        allEntities.ResetAndDestroy();
        entities.Close();
        terminated.Close();
        allEntities.Close();
        return;
        }

    CompleteClientReq( KErrNone );

    // Call HandleSubscribedPresentityPresenceL for all users
    TInt counter = entities.Count();
    for ( TInt i = counter-1 ; i>=0; i-- )
        {
        // ---------------------------------------------------------
#ifdef _DEBUG
        const MPersonPresenceInfo* info = entities[i]->PersonPresence();
        const MPresenceInfoFieldCollection& coll_debug = info->Fields();
        TInt count_debug = coll_debug.FieldCount();

        PluginLogger::Log(
            _L("PluginWatcher: callback HandleSubscribedPresentityPresenceL nbrFields=%d"), count_debug );
#endif
    // ---------------------------------------------------------

        watcherHost.HandleSubscribedPresentityPresenceL(
            allEntities[i], entities[i] );
        // Owenership is transferred
        allEntities.Remove(i);
        entities.Remove(i);
        }

    // call SetPresentityPresenceDataSubscriptionStateL for terminated users
    counter = terminated.Count();
    for ( TInt i = counter-1 ; i>=0; i-- )
        {
        MXIMPDataSubscriptionState *state = iConnObs.ObjectFactory().NewDataSubscriptionStateLC();
        state->SetSubscriptionStateL(
            MXIMPDataSubscriptionState::ESubscriptionInactive );
#ifdef _DEBUG
        PluginLogger::Log(
                _L("PluginWatcher: TEST COVERS 1"));
        PluginLogger::Log(
        _L("PluginWatcher: callback SetPresentityPresenceDataSubscriptionStateL"));
#endif
        watcherHost.SetPresentityPresenceDataSubscriptionStateL(
            terminated[i], state, NULL );
        CleanupStack::Pop( 1 ); // state
        // Owenership is transferred
        terminated.Remove(i);
        }

    entities.ResetAndDestroy();
    terminated.ResetAndDestroy();
    allEntities.ResetAndDestroy();

    entities.Close();
    terminated.Close();
    allEntities.Close();
    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::WatcherTerminatedL
// ---------------------------------------------------------------------------
//
void CSimplePluginEntityWatcher::WatcherTerminatedL(
    TInt /*aOpId*/, TInt /*aReason*/ )
    {
    // Call PrFw Host and  tell to CSimplePluginWatcher that
    // this entity can be deleted.

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginEntityWatcher: WatcherTerminatedL" ));
#endif

    MProtocolPresenceWatchingDataHost& watcherHost =
        iConnObs.Host()->ProtocolPresenceDataHost().WatchingDataHost();
    MXIMPDataSubscriptionState *state = iConnObs.ObjectFactory().NewDataSubscriptionStateLC();
    MXIMPStatus* status = iConnObs.ObjectFactory().NewStatusLC();
    state->SetSubscriptionStateL(
        MXIMPDataSubscriptionState::ESubscriptionInactive );
    status->SetResultCode( KErrCompletion );
    MXIMPIdentity* identity = iConnObs.ObjectFactory().NewIdentityLC();

    HBufC16* buf16 = HBufC16::NewLC( PresentityId().Length());
    buf16->Des().Copy( PresentityId() );
    identity->SetIdentityL( buf16->Des() );
    CleanupStack::PopAndDestroy( buf16 );

#ifdef _DEBUG
    PluginLogger::Log(
    _L("PluginWatcher: callback SetPresentityPresenceDataSubscriptionStateL"));
#endif
    watcherHost.SetPresentityPresenceDataSubscriptionStateL(
        identity, state, status );
    CleanupStack::Pop( 3 ); // identity, status, state

    // Delete this entity as useless
    iPluginWatcher.DeleteWatcher( iEntityId->Des() );
    // Do not call anything, since the method call above deletes this instance.

    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::CompleteClientReq
// ---------------------------------------------------------------------------
//
void CSimplePluginEntityWatcher::CompleteClientReq( TInt aStatus )
    {
    // complete the open PrFw request immediately here!
    iPluginWatcher.CompleteWatcher( aStatus );
    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::PresentityId
// ---------------------------------------------------------------------------
//
TPtrC8 CSimplePluginEntityWatcher::PresentityId( )
    {
    return iEntityId ? iEntityId->Des() : TPtrC8();
    }

// ---------------------------------------------------------------------------
// CSimplePluginEntityWatcher::PrFwRequestId
// ---------------------------------------------------------------------------
//
TXIMPRequestId CSimplePluginEntityWatcher::PrFwRequestId( )
    {
    return iPrFwId;
    }


// End of file
