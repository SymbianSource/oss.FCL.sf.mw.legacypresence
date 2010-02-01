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

#include <personpresenceinfo.h>
#include <presenceinfo.h>
#include <presenceinfofield.h>
#include <presenceinfofieldcollection.h>
#include <presenceinfofieldvaluetext.h>
#include <presenceinfofieldvalueenum.h>
#include <presenceinfofieldvaluebinary.h>
#include <presenceobjectfactory.h>
#include <presencewatcherinfo.h>
#include <presenceerrors.hrh>

#include <servicepresenceinfo.h>
#include <devicepresenceinfo.h>

#include <protocolpresencedatahost.h>
#include <protocolpresencepublishingdatahost.h>

#include <ximpdatasubscriptionstate.h>
#include <ximperrors.hrh>
#include <ximpidentity.h>
#include <ximpobjectcollection.h>
#include <ximpobjectfactory.h>
#include <ximpprotocolconnectionhost.h>
#include <ximpstatus.h>

#include <simplefactory.h>

#include <msimplewinfo.h>
#include <msimplepublisher.h>
#include <msimplewatcher.h>
#include <msimpledocument.h>
#include <msimpleelement.h>
#include <msimplecontent.h>

#include "simpleplugincommon.h"
#include "simplepluginpublisher.h"
#include "simpleplugindebugutils.h"
#include "simpleutils.h"
#include "simplepluginwinfo.h"
#include "simpleplugindata.h"
#include "simplepluginxdmutils.h"
#include "simplepluginconnection.h"


// ======== MEMBER FUNCTIONS ========


// -----------------------------------------------------------------------------
// CSimplePluginWatcherInfo::CSimplePluginWatcherInfo
// -----------------------------------------------------------------------------
CSimplePluginWatcherInfo::CSimplePluginWatcherInfo( )
        {}

// -----------------------------------------------------------------------------
// CSimplePluginWatcherInfo::~CSimplePluginWatcherInfo
// -----------------------------------------------------------------------------
CSimplePluginWatcherInfo::~CSimplePluginWatcherInfo()
    {
    delete iId;
    delete iSipId;
    }

// ----------------------------------------------------------
// CSimplePluginWatcherInfo::NewL
// ----------------------------------------------------------
//
CSimplePluginWatcherInfo* CSimplePluginWatcherInfo::NewL(
   const TDesC8& aId, const TDesC& aSipId )
    {
    CSimplePluginWatcherInfo* self = new (ELeave) CSimplePluginWatcherInfo( );
    CleanupStack::PushL( self );
    self->ConstructL( aId, aSipId );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------
// CSimplePluginWatcherInfo::ConstructL
// ----------------------------------------------------------
//
void CSimplePluginWatcherInfo::ConstructL(
    const TDesC8& aId, const TDesC& aSipId  )
    {
    iId = aId.AllocL();
    iSipId = aSipId.AllocL();
    }

// -----------------------------------------------------------------------------
// CSimplePluginWatcherInfo::Destroy
// -----------------------------------------------------------------------------
void CSimplePluginWatcherInfo::Destroy()
    {
    iLink.Deque();
    delete this;
    }

// -----------------------------------------------------------------------------
// CSimplePluginWatcherInfo::Match
// -----------------------------------------------------------------------------
TBool CSimplePluginWatcherInfo::Match( const TDesC8& aId, const TDesC& aSipId )
    {
    if ( (!iId->Des().CompareF( aId )) && (!iSipId->Des().CompareF( aSipId)) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CSimplePluginWatcherInfo::SipId
// -----------------------------------------------------------------------------
TPtrC CSimplePluginWatcherInfo::SipId( )
    {
    return iSipId ? iSipId->Des() : TPtrC();
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::CSimplePluginPublisher
// ---------------------------------------------------------------------------
//
CSimplePluginPublisher::CSimplePluginPublisher(
    MSimplePluginSettings& aConnSets,
    MSimplePluginConnectionObs& aObs,
    MSimpleConnection& aConn )
: CActive( CActive::EPriorityStandard ),
  iConnObs(aObs), iConnSets( aConnSets), iConnection(aConn),
  iSubscribed(EFalse), iSubscribedOwn(EFalse), iPublished(EFalse),
  iWatcherList( CSimplePluginWatcherInfo::LinkOffset())
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::ConstructL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::ConstructL( )
    {
    iPublisher = TSimpleFactory::NewPublisherL( iConnection, *this );
    iWatcher = TSimpleFactory::NewWatcherL( iConnection, *this );
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::NewL
// ---------------------------------------------------------------------------
//
CSimplePluginPublisher* CSimplePluginPublisher::NewL(
    MSimplePluginSettings& aConnSets,
    MSimplePluginConnectionObs& aObs,
    MSimpleConnection& aConn )
    {
    CSimplePluginPublisher* self =
        new( ELeave ) CSimplePluginPublisher( aConnSets, aObs, aConn );
    CleanupStack::PushL( self );
    self->ConstructL(  );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::~CSimplePluginPublisher
// ---------------------------------------------------------------------------
//
CSimplePluginPublisher::~CSimplePluginPublisher()
    {
    if ( iDocument )
        {
        iDocument->Close();
        }

    if ( iPublisher )
        {
        iPublisher->Close();
        }

    if ( iWatcher )
        {
        iWatcher->Close();
        }

    // delete iWatchers;
    DeleteWatchers();

    }

// -----------------------------------------------------------------------------
// CSimplePluginPublisher::DeleteWatchers
// -----------------------------------------------------------------------------
void CSimplePluginPublisher::DeleteWatchers()
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: DeleteWatchers" ) );
#endif
    // Delete all buffered transaction requests
    TDblQueIter<CSimplePluginWatcherInfo> rIter( iWatcherList );
    rIter.SetToFirst();

    while ( rIter )
        {
        CSimplePluginWatcherInfo* w = rIter;
        rIter++;
        // delete wathcer info
        w->Destroy();
        }
    }

// -----------------------------------------------------------------------------
// CSimplePluginPublisher::AddWatcherIfNotExistsL
// -----------------------------------------------------------------------------
void CSimplePluginPublisher::AddWatcherIfNotExistsL( const TDesC8& aId, const TDesC& aSipId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: AddWatcherIfNotExistsL" ) );
#endif
    TDblQueIter<CSimplePluginWatcherInfo> rIter( iWatcherList );
    rIter.SetToFirst();

    TBool found(EFalse);

    while ( rIter )
        {
        CSimplePluginWatcherInfo* w = rIter;
        rIter++;
        found = w->Match( aId, aSipId );
        if ( found )
            {
            break;
            }
        else
            {
            // continue searching
            }
        }
    if ( !found )
        {
#ifdef _DEBUG
        PluginLogger::Log(_L("PluginPublisher: AddWatcherIfNotExistsL adds a watcher" ) );
#endif
        CSimplePluginWatcherInfo* w = CSimplePluginWatcherInfo::NewL( aId, aSipId );
        iWatcherList.AddLast( *w );
        }
    }

// -----------------------------------------------------------------------------
// CSimplePluginPublisher::RemoveWatcherIfExistsL
// -----------------------------------------------------------------------------
void CSimplePluginPublisher::RemoveWatcherIfExistsL( const TDesC8& aId, const TDesC& aSipId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: RemoveWatcherIfExistsL" ) );
#endif
    TDblQueIter<CSimplePluginWatcherInfo> rIter( iWatcherList );
    rIter.SetToFirst();

    TBool found(EFalse);

    while ( rIter )
        {
        CSimplePluginWatcherInfo* w = rIter;
        rIter++;
        // delete wathcer info
        found = w->Match( aId, aSipId );
        if ( found )
            {
#ifdef _DEBUG
            PluginLogger::Log(_L("PluginPublisher: RemoveWatcherIfExistsL removes a watcher" ) );
#endif
            w->Destroy();
            break;
            }
        else
            {
            // continue searching
            }
        }
    }
// -----------------------------------------------------------------------------
// CSimplePluginPublisher::MakeCurrentWatcherListLC
// -----------------------------------------------------------------------------
CDesCArrayFlat* CSimplePluginPublisher::MakeCurrentWatcherListLC()
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: MakeCurrentWatcherListL" ) );
#endif
    // No one should be added more than once
    const TInt KMyGran = 10;
    CDesCArrayFlat* watchers = new (ELeave) CDesCArrayFlat( KMyGran );
    CleanupStack::PushL( watchers );      // << watchers

    // add user only once here.
    TDblQueIter<CSimplePluginWatcherInfo> rIter( iWatcherList );
    rIter.SetToFirst();

    while ( rIter )
        {
        CSimplePluginWatcherInfo* w = rIter;
        rIter++;

        TInt dummy = 0;
        if ( watchers->Find( w->SipId(), dummy ))
            {
            watchers->AppendL( w->SipId() );
            }
        else
            {
            // continue searching
            }
        }

    return watchers;
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::DoPublishOwnPresenceL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::DoPublishOwnPresenceL(
    const MPresenceInfo& aOwnPresence,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: DoPublishOwnPresenceL"));
#endif

    const MPersonPresenceInfo* persInfo = aOwnPresence.PersonPresence();

    // convert data format from PrFW to internal
    if ( !persInfo )
        {
        // Notice: error codes
        CompletePrFwReq( KErrArgument );
        return;
        }
    InitializeSimpleDocumentL( );
    AddSimpleDocumentPersL( persInfo );

    TInt myCount = aOwnPresence.ServicePresenceCount();
    for ( TInt i=0; i < myCount; i++ )
        {
        const MServicePresenceInfo& servInfo = aOwnPresence.ServicePresenceAt(i);
        AddSimpleDocumentServiceL( servInfo );
        }
    myCount = aOwnPresence.DevicePresenceCount();
    for ( TInt i=0; i < myCount; i++ )
        {
        const MDevicePresenceInfo& devInfo = aOwnPresence.DevicePresenceAt(i);
        AddSimpleDocumentDeviceL( devInfo );
        }

    // Ensure that XDM rules exists, the show continues in RunL
    StartXdmOperationL(aReqId );
    iOperation = EPublishOwn;
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::DoSubscribeOwnPresenceL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::DoSubscribeOwnPresenceL(
    const MPresenceInfoFilter& /*aPif*/,   // notice: aPif filter not supported
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: DoSubscribeOwnPresenceL"));
#endif

    StartXdmOperationL( aReqId );

    iOperation = ESubscribeOwn;
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::DoUpdateOwnPresenceSubscriptionPifL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::DoUpdateOwnPresenceSubscriptionPifL(
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: DoUpdateOwnPresenceSubscriptionPifL"));
#endif
    // Notice: aPif filter not supported
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::DoUnsubscribeOwnPresenceL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::DoUnsubscribeOwnPresenceL(
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: DoUnsubscribeOwnPresenceL"));
#endif
    iSubscribedOwn = EFalse;
    TRAPD( err, iSimpleId = iWatcher->UnsubscribeL());
    if ( err )
        {
        User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
        }
    iPrFwId = aReqId;
    iOperation = EUnsubscribeOwn;
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::DoSubscribePresenceWatcherListL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::DoSubscribePresenceWatcherListL(
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: DoSubscribePresenceWatcherListL"));
#endif

    if ( !iSubscribed )
        {
        TRAPD( err, iConnObs.WinfoHandlerL()->SubscribeWinfoListL( aReqId ));
        if ( err )
            {
            User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
            }
        iSubscribed = ETrue;
        iPrFwId = aReqId;
        iOperation = ESubscribeWinfo;
        }
    else
        {
        iPrFwId = aReqId;
        CompletePrFwReq( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::DoUnsubscribePresenceWatcherListL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::DoUnsubscribePresenceWatcherListL(
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: DoUnsubscribePresenceWatcherListL"));
#endif
    if ( iSubscribed )
        {
        TRAPD( err, iConnObs.WinfoHandlerL()->UnsubscribeWinfoListL( aReqId ));
        if ( err )
            {
            User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
            }
        iSubscribed = EFalse;
        iPrFwId = aReqId;
        iOperation = EUnsubscribeWinfo;
        }
    else
        {
        iPrFwId = aReqId;
        CompletePrFwReq( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::PublishReqCompleteL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::PublishReqCompleteL( TInt /*aOpid*/, TInt aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: PublishReqCompleteL("));
#endif
    if ( !aStatus )
        {
        iPublished = ETrue;
        }
    else
        {
        }

    CompletePrFwReq( aStatus );
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::PublishTerminatedL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::PublishTerminatedL( TInt /*aOpid*/ )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: PublishTerminatedL"));
#endif
    // Notice: nothing to do now in Host APi.
    iPublished = EFalse;
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::WatcherReqCompleteL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::WatcherReqCompleteL( TInt /*aOpid*/, TInt aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: WatcherReqCompleteL"));
#endif
    if ( !aStatus )
        {
        iSubscribedOwn = ETrue;
        }
    else
        {
        }
    CompletePrFwReq( aStatus );
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::WatcherNotificationL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::WatcherNotificationL( MSimpleDocument& aDocument )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: WatcherNotificationL"));
#endif

    // Notice: Do not need to check the expiration here since WatcherTerminatedL
    // is called then too.

    MProtocolPresencePublishingDataHost& publishHost =
        iConnObs.Host()->ProtocolPresenceDataHost().PublishingDataHost();
    MPresenceInfo* prInfo = iConnObs.PresenceObjectFactory().NewPresenceInfoLC();  // << prInfo
    CSimplePluginData::NotifyToPrInfoL( iConnObs.PresenceObjectFactory(), aDocument, *prInfo );

#ifdef _DEBUG
    // ---------------------------------------------------------
    const MPersonPresenceInfo* pers_debug = prInfo->PersonPresence();
    const MPresenceInfoFieldCollection& coll_debug = pers_debug->Fields();
    TInt count_debug = coll_debug.FieldCount();
    PluginLogger::Log(_L("PluginPublisher: nbr of fields received =%d"), count_debug );
    // ---------------------------------------------------------
#endif

    // PrFw Host API callbacks

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: callback HandleSubscribedOwnPresenceL"));
#endif
    publishHost.HandleSubscribedOwnPresenceL( prInfo );
    CleanupStack::Pop();  // >> prInfo

    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::WatcherListNotificationL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::WatcherListNotificationL( MSimplePresenceList& /*aList*/ )
    {
    // Notice: not needed.
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::WatcherTerminatedL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::WatcherTerminatedL(
    TInt /*aOpId*/, TInt /*aReason*/ )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: WatcherTerminatedL"));
#endif
    iSubscribedOwn = EFalse;

    MProtocolPresencePublishingDataHost& publishHost =
        iConnObs.Host()->ProtocolPresenceDataHost().PublishingDataHost();
    MXIMPDataSubscriptionState *state = iConnObs.ObjectFactory().NewDataSubscriptionStateLC();
    MXIMPStatus* status = iConnObs.ObjectFactory().NewStatusLC();
    state->SetSubscriptionStateL( MXIMPDataSubscriptionState::ESubscriptionInactive );
    status->SetResultCode( KErrCompletion );
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: callback SetOwnPresenceDataSubscriptionStateL"));
#endif
    publishHost.SetOwnPresenceDataSubscriptionStateL( state, status );
    CleanupStack::Pop( 2 ); // status, state
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::CompletePrFwReq
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::CompletePrFwReq( TInt aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: CompletePrFwReq status=%d"), aStatus );
#endif
    if ( iOperation != ENoOperation )
        {
        iOperation = ENoOperation;
        iConnObs.CompleteReq( iPrFwId, aStatus );
        iPrFwId = TXIMPRequestId();
        }
    else
        {
        }
    return;
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::DoCancel
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::DoCancel(  )
    {
    iXdmUtils->Cancel();
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::RunL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::RunL(  )
    {

    TInt status = iStatus.Int();

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: RunL %d"), status );
#endif

    if ( !status )
        {
        if ( iOperation == ESubscribeOwn )
            {
            iSimpleId = iWatcher->SubscribeL(
                iConnSets.CurrentSipPresentity8(),
                NULL,  // aFilter <-> aPif
                ETrue, EFalse );
            }
        else
            {
            MakePublishReqL();
            }
        }
    else
        {
        CompletePrFwReq( status );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::RunError
// ---------------------------------------------------------------------------
//
TInt CSimplePluginPublisher::RunError( TInt aError )
    {
    CompletePrFwReq( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::GetInterface
// ---------------------------------------------------------------------------
//
TAny* CSimplePluginPublisher::GetInterface(
        TInt32 aInterfaceId,
        TIfGetOps aOptions )
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        MProtocolPresencePublishing* myIf = this;
        return myIf;
        }
    else if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( _L("CSimplePlugin"), KErrExtensionNotSupported );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::GetInterface
// ---------------------------------------------------------------------------
//
const TAny* CSimplePluginPublisher::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions ) const
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        const MProtocolPresencePublishing* myIf = this;
        return myIf;
        }
    else if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( _L("CSimplePlugin"), KErrExtensionNotSupported );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::GetInterfaceId
// ---------------------------------------------------------------------------
//
TInt32 CSimplePluginPublisher::GetInterfaceId() const
    {
    return MProtocolPresencePublishing::KInterfaceId;
    }


// ---------------------------------------------------------------------------
// CSimplePluginPublisher::InitializeSimpleDocumentL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::InitializeSimpleDocumentL( )
    {
    if ( iDocument )
        {
        iDocument->Close();
        iDocument = NULL;
        }
    iDocument = TSimpleFactory::NewDocumentL();
    iDocument->AddNamespaceL( KSimplePDM, KSimpleNsPDM );
    iDocument->AddNamespaceL( KSimpleRPID, KSimpleNsRPID );
    iDocument->AddNamespaceL( KSimpleOP, KSimpleNsOP );
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::AddSimpleDocumentPersL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::AddSimpleDocumentPersL(
    const MPersonPresenceInfo* aInfo )
    {
    CSimplePluginData::AddPrPersToSimpleDocumentL(
        aInfo, *iDocument, iConnSets.CurrentSipPresentity8() );
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::AddSimpleDocumentServiceL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::AddSimpleDocumentServiceL(
    const MServicePresenceInfo& aInfo )
    {
    if ( aInfo.Fields().FieldCount() > 0 || aInfo.ServiceType().Length() )
        {
        // Notice: currently all the fields in the namespace are supported,
        // but this ensures that if namespace is extended later, it is
        // handled right way in the adaptation
        User::Leave( KPresenceErrPresenceInfoFieldTypeNotSupported );
        }
    else
        {
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::AddSimpleDocumentDeviceL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::AddSimpleDocumentDeviceL(
    const MDevicePresenceInfo& aInfo )
    {
    if ( aInfo.Fields().FieldCount() > 0 || aInfo.DeviceName().Length() )
        {
        // Notice: currently all the fields in the namespace are supported,
        // but this ensures that if namespace is extended later, it is
        // handled right way in the adaptation
        User::Leave( KPresenceErrPresenceInfoFieldTypeNotSupported );
        }
    else
        {
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::MakePublishReqL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::MakePublishReqL( )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: -> MakePublishReqL"));
#endif
    // Send the iDocument
    if ( !iPublished )
        {
        iSimpleId = iPublisher->StartPublishL( *iDocument, ETrue );
        }
    else
        {
        iSimpleId = iPublisher->ModifyPublishL( *iDocument);
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::StartXdmOperationL(
    TXIMPRequestId aReqId )
    {
    TRAPD( err, DoStartXdmOperationL( aReqId ));
    if ( err )
        {
        User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::DoStartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::DoStartXdmOperationL(
    TXIMPRequestId aReqId )
    {
    __ASSERT_DEBUG( !IsActive(), User::Leave( KErrCorrupt ) );
    iPrFwId = aReqId;
    if ( !iXdmUtils )
        {
        iXdmUtils = iConnObs.XdmUtilsL();
        }
    iXdmUtils->InitializeXdmL( iStatus );
    SetActive();
    }



// ---------------------------------------------------------------------------
// CSimplePluginPublisher::WinfoNotificationL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::WinfoNotificationL(
    MSimpleWinfo& aWinfo )
    {
    // ignore if not subscribed
    if ( !iSubscribed )
        {
        return;
        }

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: WinfoNotificationL"));
#endif

    // Handle full-state and partial state notifications
    // - If terminated -> remove from iWatcherList
    // - If active -> add into iWatcherList if does not already exist
    // WinfoTerminatedL handles termination of subscription.

    // Notice: CSimplePluginWinfo has completed the open request if needed before this.


    /* example:

    // active/pending/terminated

   <?xml version="1.0"?>
     <watcherinfo xmlns="urn:ietf:params:xml:ns:watcherinfo"
   version="0" state="full">
     <watcher-list resource="sip:presentity@example.com"
                   package="presence">
         <watcher status="active"
            id="sr8fdsj"
            duration-subscribed="509"
            expiration="20"
            event="approved">sip:watcherA@example.com"
          </watcher>
     */

    RPointerArray<MSimpleElement> elems;
    CleanupClosePushL( elems );         // << elems
    TInt err = aWinfo.SimpleElementsL( elems );
    User::LeaveIfError( err );
    TInt count = elems.Count();

    __ASSERT_DEBUG( count == 1, User::Leave( KErrCorrupt ) );

    using namespace NSimplePlugin::NSimpleOma;

    const TDesC8* stateVal = aWinfo.AttrValue( KSimpleState8 );
    if ( stateVal && !stateVal->CompareF( KSimpleFull8 ))
        {
#ifdef _DEBUG
        PluginLogger::Log(_L("PluginPublisher: fullstate = TRUE") );
#endif
        // full winfo-list is received
        DeleteWatchers();
        }
  else
        {
#ifdef _DEBUG
        PluginLogger::Log(_L("PluginPublisher: fullstate = FALSE") );
#endif
    }

    MSimpleElement* elem = elems[0];
    TPtrC8 p8 = elem->LocalName();
    err = p8.CompareF( KSimpleWatcherList8 );
    User::LeaveIfError( err );

    err = elem->SimpleElementsL( elems );
    User::LeaveIfError( err );

    // Collect the active watchers only.
    UpdateActiveWatchersListL( elems );

    CDesCArrayFlat* watchers = MakeCurrentWatcherListLC();  // << watchers
    MXIMPObjectCollection *actives =
        iConnObs.ObjectFactory().NewObjectCollectionLC();                  // << actives

    // Create MPresenceWatcherInfo entities for
    // all active watchers and add to actives.
    TInt wCount = watchers->MdcaCount();
    for ( TInt j = 0; j < wCount; j++ )
        {
        // create MPresenceWatcherInfo object
        MPresenceWatcherInfo* wInfo =
            iConnObs.PresenceObjectFactory().NewPresenceWatcherInfoLC();           // << wInfo

        MXIMPIdentity* identity = iConnObs.ObjectFactory().NewIdentityLC();  // << identity
#ifdef _DEBUG
        TBuf<200> debug_buffer;
        debug_buffer = watchers->MdcaPoint( j );
        PluginLogger::Log(_L("PluginPublisher: add watcher into collection: %S"), &debug_buffer);
#endif
        identity->SetIdentityL( watchers->MdcaPoint( j ) );
        wInfo->SetWatcherIdL( identity );
        CleanupStack::Pop( );                               // >> identity

        wInfo->SetWatcherDisplayNameL( watchers->MdcaPoint( j ) );
        wInfo->SetWatcherTypeL( MPresenceWatcherInfo::EPresenceSubscriber );

        actives->AddObjectL( wInfo );
        CleanupStack::Pop( );                           // >> wInfo
        }

    MProtocolPresencePublishingDataHost& publishHost =
        iConnObs.Host()->ProtocolPresenceDataHost().PublishingDataHost();

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: callback HandlePresenceWatcherListL"));
#endif
    publishHost.HandlePresenceWatcherListL( actives );
    CleanupStack::Pop();                            // >> actives
    CleanupStack::PopAndDestroy( watchers );        // >> watchers
    CleanupStack::PopAndDestroy( &elems );          // >> elems

    }

// ---------------------------------------------------------------------------
// CSimplePluginPublisher::WinfoTerminatedL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::WinfoTerminatedL( TInt /*aReason*/ )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: WinfoTerminatedL") );
#endif

    if ( !iSubscribed )
        {
        return;
        }

    // call SetPresenceWatcherListDataSubscriptionStateL
    iSubscribed = EFalse;

    MProtocolPresencePublishingDataHost& publishHost =
        iConnObs.Host()->ProtocolPresenceDataHost().PublishingDataHost();
    MXIMPDataSubscriptionState *state = iConnObs.ObjectFactory().NewDataSubscriptionStateLC();
    MXIMPStatus* status = iConnObs.ObjectFactory().NewStatusLC();
    state->SetSubscriptionStateL( MXIMPDataSubscriptionState::ESubscriptionInactive );
    state->SetDataStateL( MXIMPDataSubscriptionState::EDataUnavailable );
    status->SetResultCode( KErrCompletion );
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginPublisher: callback SetPresenceWatcherListDataSubscriptionStateL"));
#endif
    publishHost.SetPresenceWatcherListDataSubscriptionStateL( state, status );
    CleanupStack::Pop( 2 ); // status, state
    }


// ---------------------------------------------------------------------------
// CSimplePluginPublisher::UpdateActiveWatchersListL
// ---------------------------------------------------------------------------
//
void CSimplePluginPublisher::UpdateActiveWatchersListL(
    RPointerArray<MSimpleElement>& aElems )
    {
    // Collect active users.
    using namespace NSimplePlugin::NSimpleOma;

    HBufC* nodeContent = NULL;

    TInt count = aElems.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        MSimpleElement* elem = aElems[i];
        TPtrC8 p8( elem->LocalName());
        if (!( p8.CompareF( KSimpleWatcher8 )))
            {
            const TDesC8* pp8 = elem->AttrValue( KSimpleStatus8 );
            // Active wathers here
            if ( pp8 && !pp8->CompareF( KSimpleActive8 ))
                {

                // save id since there may be multiple subscriptions
                // from a single watcher SIP identity.
                const TDesC8* pId8 = elem->AttrValue( KSimpleId8 );

                // Build collection of grant requests
                // Find the child node containing the SIP entity
                nodeContent = elem->ContentUnicodeL();
                CleanupStack::PushL( nodeContent );         // << nodeContent

                AddWatcherIfNotExistsL( pId8 ? *pId8 : KNullDesC8, nodeContent->Des() );

                CleanupStack::PopAndDestroy( nodeContent ); // >> nodeContent
                }
            // Terminated wathers here, remove them from active list
            else if ( pp8 && !pp8->CompareF( KSimpleTerminated8 ))
                {

                const TDesC8* pId8 = elem->AttrValue( KSimpleId8 );

                // Remove terminated from iWatcherList
                nodeContent = elem->ContentUnicodeL();
                CleanupStack::PushL( nodeContent );             // << nodeContent

                RemoveWatcherIfExistsL( pId8 ? *pId8 : KNullDesC8, nodeContent->Des() );

                CleanupStack::PopAndDestroy( nodeContent );     // >> nodeContent
                }
            }
        }

    }



// End of file
