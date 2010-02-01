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
#include <presenceinfofieldcollection.h>
#include <presenceinfofield.h>
#include <presenceinfofieldvaluetext.h>
#include <presenceinfofieldvalueenum.h>
#include <presenceobjectfactory.h>
#include <presenceerrors.hrh>

#include <presentitygroupmemberinfo.h>

#include <protocolpresencedatahost.h>
#include <protocolpresentitygroupsdatahost.h>
#include <protocolpresencewatchingdatahost.h>

#include <ximpdatasubscriptionstate.h>
#include <ximperrors.hrh>
#include <ximpidentity.h>
#include <ximpobjectcollection.h>
#include <ximpobjectfactory.h>
#include <ximpprotocolconnectionhost.h>
#include <ximpstatus.h>

#include <simplefactory.h>
#include <simpleerrors.h>
#include <msimplewinfo.h>
#include <msimplewatcher.h>
#include <msimpledocument.h>
#include <msimpleelement.h>

#include "simpleplugincommon.h"
#include "simplepluginwatcher.h"
#include "simplepluginentitywatcher.h"
#include "simpleplugindebugutils.h"
#include "simpleutils.h"
#include "simplepluginxdmutils.h"
#include "simpleplugindata.h"
#include "simplepluginconnection.h"



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::CSimplePluginWatcher
// ---------------------------------------------------------------------------
//
CSimplePluginWatcher::CSimplePluginWatcher(
    MSimplePluginSettings& aConnSets,
    MSimplePluginConnectionObs& aObs,
    MSimpleConnection& aConn )
: CActive( CActive::EPriorityStandard ),
  iConnSets( aConnSets), iConnObs(aObs), iConnection(aConn), iCompleted( ETrue ),
  iOperation( EPluginUndef ), iXdmState( EPluginIdle )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::ConstructL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::ConstructL( )
    {
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::NewL
// ---------------------------------------------------------------------------
//
CSimplePluginWatcher* CSimplePluginWatcher::NewL(
    MSimplePluginSettings& aConnSets,
    MSimplePluginConnectionObs& aObs,
    MSimpleConnection& aConn )
    {
    CSimplePluginWatcher* self =
        new( ELeave ) CSimplePluginWatcher( aConnSets, aObs, aConn );
    CleanupStack::PushL( self );
    self->ConstructL(  );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::~CSimplePluginWatcher
// ---------------------------------------------------------------------------
//
CSimplePluginWatcher::~CSimplePluginWatcher()
    {
    iWatchers.ResetAndDestroy();
    iWatchers.Close();
    delete iPresIdentity;
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DoSubscribePresentityPresenceL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DoSubscribePresentityPresenceL(
    const MXIMPIdentity& aPresentityId,
    const MPresenceInfoFilter& /*aPif*/,   // notice: aPif filter not supported
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginWatcher: DoSubscribePresentityPresenceL"));
#endif

    TRAPD( err, DoDoSubscribePresentityPresenceL(aPresentityId, aReqId ));
    if ( err )
        {
        User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DoDoSubscribePresentityPresenceL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DoDoSubscribePresentityPresenceL(
    const MXIMPIdentity& aPresentityId,
    TXIMPRequestId aReqId )
    {
    iOperation = EPluginUndef;
    iXdmState = EPluginIdle;

    SetPresIdentityL( aPresentityId );
    iPrFwId = aReqId;
    iCompleted = EFalse;

  HBufC8* pres8 = NULL;
    pres8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( iPresIdentity->Des() );
    CleanupStack::PushL( pres8 ); // << pres8

    CSimplePluginEntityWatcher* watcher = MatchWatcher2L( pres8->Des(), ETrue );
    watcher->StartSubscribeL( pres8->Des() );
    CleanupStack::PopAndDestroy( pres8 );  // >> pres8
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DoUpdatePresentityPresenceSubscriptionPifL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DoUpdatePresentityPresenceSubscriptionPifL(
    const MXIMPIdentity& /*aPresentityId*/,
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
#ifdef _DEBUG
    PluginLogger::Log(
        _L("PluginWatcher: DoUpdatePresentityPresenceSubscriptionPifL"));
#endif

    iOperation = EPluginUndef;

    // notice: aPif filter not supported
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DoUnsubscribePresentityPresenceL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DoUnsubscribePresentityPresenceL(
    const MXIMPIdentity& aPresentityId,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginWatcher: DoUnsubscribePresentityPresenceL"));
#endif

    TRAPD( err, DoDoUnsubscribePresentityPresenceL( aPresentityId, aReqId ) );
    if ( err )
        {
        User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DoDoUnsubscribePresentityPresenceL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DoDoUnsubscribePresentityPresenceL(
    const MXIMPIdentity& aPresentityId,
    TXIMPRequestId aReqId )
    {

    iOperation = EPluginUndef;
    iXdmState = EPluginIdle;

    SetPresIdentityL( aPresentityId );
    iPrFwId = aReqId;
    iCompleted = EFalse;

  HBufC8* pres8 = NULL;
    pres8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( iPresIdentity->Des() );
    CleanupStack::PushL( pres8 ); // << pres8

    CSimplePluginEntityWatcher* watcher = MatchWatcher2L( pres8->Des(), EFalse );
    if ( !watcher )
        {
        User::Leave( KErrNotFound ); // Notice: error code
        }
    else
        {
#ifdef _DEBUG
        PluginLogger::Log(_L("PluginWatcher: -> StopSubscribeL"));
#endif
        watcher->StopSubscribeL();
        }
    CleanupStack::PopAndDestroy( pres8 );  // >> pres8
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DoSubscribePresentityGroupMembersPresenceL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DoSubscribePresentityGroupMembersPresenceL(
    const MXIMPIdentity& aGroupId,
    const MPresenceInfoFilter& /*aPif*/,   // notice: aPif filter not supported
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(
        _L("PluginWatcher: DoSubscribePresentityGroupMembersPresenceL"));
#endif

    iOperation = EPluginSubscribeGroup;
    iXdmState = EPluginIdle;

    SetPresIdentityL( aGroupId );
    iPrFwId = aReqId;
    iCompleted = EFalse;

    // Get Shared XDM lists first and create RLS service first
    StartXdmOperationL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DoUpdatePresentityGroupMembersPresenceSubscriptionPifL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DoUpdatePresentityGroupMembersPresenceSubscriptionPifL(
    const MXIMPIdentity& /*aGroupId*/,
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
#ifdef _DEBUG
    PluginLogger::Log(
  _L("PluginWatcher: DoUpdatePresentityGroupMembersPresenceSubscriptionPifL"));
#endif
    // notice: aPif filter not supported
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DoUnsubscribePresentityGroupMembersPresenceL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DoUnsubscribePresentityGroupMembersPresenceL(
    const MXIMPIdentity& aGroupId,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(
        _L("PluginWatcher: DoUnsubscribePresentityGroupMembersPresenceL"));
#endif
    TRAPD( err, DoDoUnsubscribePresentityGroupMembersPresenceL( aGroupId, aReqId ));
    if ( err )
        {
        User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DoDoUnsubscribePresentityGroupMembersPresenceL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DoDoUnsubscribePresentityGroupMembersPresenceL(
    const MXIMPIdentity& aGroupId,
    TXIMPRequestId aReqId )
    {
    iOperation = EPluginUnsubscribeGroup;
    iXdmState = EPluginIdle;

    SetPresIdentityL( aGroupId );
    iPrFwId = aReqId;
    iCompleted = EFalse;

    HBufC* buf16 = iXdmUtils->RlsServiceByResourceListLC( iPresIdentity->Des() ); // << buf16

    HBufC8* buf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( buf16->Des() );
    CleanupStack::PushL( buf );  // << buf

    CSimplePluginEntityWatcher* watcher = MatchWatcher2L( buf->Des(), EFalse );

    CleanupStack::PopAndDestroy( buf );  // >> buf
    CleanupStack::PopAndDestroy( buf16 );  // >> buf16

    if ( !watcher )
        {
        User::Leave( KErrNotFound ); // Notice: error code
        }
    else
        {
        watcher->StopSubscribeL();
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::GetInterface
// ---------------------------------------------------------------------------
//
TAny* CSimplePluginWatcher::GetInterface(
        TInt32 aInterfaceId,
        TIfGetOps aOptions )
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        MProtocolPresenceWatching* myIf = this;
        return myIf;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( _L("CSimplePlugin"), KErrExtensionNotSupported );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::GetInterface
// ---------------------------------------------------------------------------
//
const TAny* CSimplePluginWatcher::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions ) const
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        const MProtocolPresenceWatching* myIf = this;
        return myIf;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( _L("CSimplePlugin"), KErrExtensionNotSupported );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::GetInterfaceId
// ---------------------------------------------------------------------------
//
TInt32 CSimplePluginWatcher::GetInterfaceId() const
    {
    return MProtocolPresenceWatching::KInterfaceId;
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::MatchWatcher2L
// ---------------------------------------------------------------------------
//
CSimplePluginEntityWatcher* CSimplePluginWatcher::MatchWatcher2L(
    const TDesC8& aPresentityid, TBool aCreate )
    {
    TInt count = iWatchers.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CSimplePluginEntityWatcher* temp = (iWatchers[i]);
        if ( !temp->PresentityId().CompareF( aPresentityid ))
            {
            return temp;
            }
        }

    // If watcher was not found, crete it when wanted so.
    if ( !aCreate )
        {
        return NULL;
        }
    else
        {
        CSimplePluginEntityWatcher* watcher = CSimplePluginEntityWatcher::NewL(
            iConnObs, iConnection, *this );
        iWatchers.Append( watcher );
        return watcher;
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DeleteWatcher
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DeleteWatcher(
    const TDesC8& aPresentityid )
    {

#ifdef _DEBUG
    PluginLogger::Log( _L("PluginWatcher: DeleteWatcher **"));
#endif

    TInt count = iWatchers.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CSimplePluginEntityWatcher* temp = (iWatchers[i]);
        if ( !temp->PresentityId().CompareF( aPresentityid ) )
            {
            iWatchers.Remove( i );
            iWatchers.GranularCompress();
            delete temp;
            break;
            }
        else
            {
            }
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::GetEntitiesInListL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::GetEntitiesInListL(
    const TDesC& aList, MXIMPObjectCollection& aMembers )
    {
    __ASSERT_DEBUG( iXdmUtils, User::Leave( KErrCorrupt ) );

    const TInt KMyGran = 10;
    CPtrCArray* ids = new (ELeave) CPtrCArray( KMyGran );
    CleanupStack::PushL( ids );      // << ids

    CPtrCArray* dispNames = new (ELeave) CPtrCArray( KMyGran );
    CleanupStack::PushL( dispNames  );      // << dispNames

    iXdmUtils->GetEntitiesInListL( aList, *ids, *dispNames );

    CopyGroupMembersToCollectionL( *ids, *dispNames, aMembers );

    CleanupStack::PopAndDestroy( dispNames );
    CleanupStack::PopAndDestroy( ids );
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::StartWatchingRlsL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::StartWatchingRlsL()
    {
    HBufC* buf = iXdmUtils->RlsServiceByResourceListLC( iPresIdentity->Des() );  // << buf

    HBufC8* buf2 = NULL;
    buf2 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( buf->Des() );
    CleanupStack::PushL( buf2 ); // << buf2

    CSimplePluginEntityWatcher* watcher = MatchWatcher2L( buf2->Des(), ETrue );

    watcher->StartSubscribeListL( buf2->Des() );

    watcher->SetSimpleNameL( buf2->Des() );

    CleanupStack::PopAndDestroy( buf2 );  // >> buf2
    CleanupStack::PopAndDestroy( buf );  // >> buf
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::SetPresIdentityL()
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::SetPresIdentityL(
     const MXIMPIdentity& aPresentityId )
     {
     delete iPresIdentity;
     iPresIdentity = NULL;
     iPresIdentity = aPresentityId.Identity().AllocL();
     }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::CompletePrFwReq
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::CompletePrFwReq( TInt aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginWatcher: CompletePrFwReq status=%d"), aStatus );
#endif
    iXdmState = EPluginIdle;

    if ( !iCompleted )
        {
        iCompleted = ETrue;
        // Convert error code when needed
        if ( aStatus == KSimpleErrNotFound &&
             ( iOperation == EPluginSubscribeGroup ||
               iOperation == EPluginUnsubscribeGroup ))
            {
            aStatus = KPresenceErrUnknownPresentityGroup;
            }
        else if ( aStatus == KSimpleErrNotFound )
            {
            aStatus = KPresenceErrUnknownPresentity;
            }
        else
            {
            }
        iConnObs.CompleteReq( iPrFwId, aStatus );
        iPrFwId = TXIMPRequestId();
        iOperation = EPluginUndef;
        }
    else
        {
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::CompleteWatcher
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::CompleteWatcher( TInt aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginWatcher: CompleteWatcher status=%d"), aStatus );
#endif
    // start to remove RLS service when needed.
    if ( iOperation == EPluginUnsubscribeGroup && iXdmState == EPluginIdle )
        {
        TRAPD( err, iXdmUtils->FetchRlsL( iStatus ));
        if ( err )
            {
            CompletePrFwReq( err );
            }
        else
            {
            iXdmState = EPluginFetchRls;
            SetActive();
            }
        }
    else
        {
        CompletePrFwReq( aStatus );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::StartXdmOperationL()
    {
    TRAPD( err, DoStartXdmOperationL() );
    if ( err )
        {
        User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DoStartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DoStartXdmOperationL()
    {
    __ASSERT_DEBUG( !IsActive(), User::Leave( KErrCorrupt ) );
    if ( !iXdmUtils )
        {
        iXdmUtils = iConnObs.XdmUtilsL();
        }
    iXdmUtils->InitializeXdmsOnlyL( iStatus );

    iXdmState = EPluginInitXdm;

    SetActive();
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DoCancel
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DoCancel(  )
    {
    iXdmUtils->Cancel();
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::RunL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::RunL(  )
    {

    TInt status = iStatus.Int();

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginWatcher: RunL %d"), status );
#endif
    if ( !status )
        {
        // create RLS service first here
        if ( iOperation == EPluginSubscribeGroup )
            {
            DoRunForSubscriptionL();
            }
        // remove RLS service when needed.
        else if ( iOperation == EPluginUnsubscribeGroup )
            {
            DoRunForUnsubscriptionL();
            }
        else
            {
            // Idle state should not be completed. We should not come here
            // because of PrFw core makes one request at a time.
            }
        }
    else
        {
        CompletePrFwReq( status );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::RunError
// ---------------------------------------------------------------------------
//
TInt CSimplePluginWatcher::RunError( TInt aError )
    {
    CompletePrFwReq( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::GetListContentL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::GetListContentL()
    {
    MXIMPObjectFactory& myFactory = iConnObs.ObjectFactory();
    MProtocolPresentityGroupsDataHost& dataHost =
        iConnObs.Host()->ProtocolPresenceDataHost().GroupsDataHost();

    MXIMPObjectCollection* entities =
        myFactory.NewObjectCollectionLC();          // << entities
    GetEntitiesInListL( iPresIdentity->Des(), *entities );

    // callback for data
    MXIMPIdentity* id = myFactory.NewIdentityLC();  // << id
    id->SetIdentityL( iPresIdentity->Des() );
#ifdef _DEBUG
    PluginLogger::Log(
        _L("PluginWatcher: callback HandlePresentityGroupContentL"));
#endif
    dataHost.HandlePresentityGroupContentL( id, entities );
    CleanupStack::Pop();                            // >> id
    CleanupStack::Pop();                            // >> entities

    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::CopyGroupMembersToCollectionL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::CopyGroupMembersToCollectionL(
    CPtrCArray& aIds, CPtrCArray& aNames, MXIMPObjectCollection& aCollection)
    {
    MXIMPObjectFactory& myFactory = iConnObs.ObjectFactory();
    MPresenceObjectFactory& myPresenceFactory = iConnObs.PresenceObjectFactory();
    TInt count = aIds.Count();
    for ( TInt i=0; i < count; i++ )
        {
        // create here MPresentityGroupMemberInfo
        MPresentityGroupMemberInfo* info = myPresenceFactory.NewPresentityGroupMemberInfoLC(); // << info
        MXIMPIdentity* id = myFactory.NewIdentityLC();  // << id
        id->SetIdentityL( aIds[i] );
        info->SetGroupMemberIdL( id );
        CleanupStack::Pop(); // >> id

        info->SetGroupMemberDisplayNameL( aNames[i] );
        aCollection.AddObjectL( info );
        CleanupStack::Pop(); // >> info
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DoRunForSubscriptionL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DoRunForSubscriptionL( )
    {
    if ( iXdmState == EPluginInitXdm )
        {
        // get members of the list first
        GetListContentL();

        // Fetch RLS document first before modifying the data.
        TRAPD( err, iXdmUtils->FetchRlsL( iStatus ));
        if ( err )
            {
            CompletePrFwReq( err );
            }
        else
            {
            iXdmState = EPluginFetchRls;
            SetActive();
            }
        }
    else if ( iXdmState == EPluginFetchRls )
        {
        // Create RLS document service
        iXdmUtils->AddRlsGroupL( iPresIdentity->Des() );
        iXdmState = EPluginAddGroupMember;
        iXdmUtils->CommitRlsL( iStatus );
        SetActive();
        }
    else
        {
        StartWatchingRlsL();
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginWatcher::DoRunForUnsubscriptionL
// ---------------------------------------------------------------------------
//
void CSimplePluginWatcher::DoRunForUnsubscriptionL( )
    {
    TInt status = iStatus.Int();
    if ( iXdmState == EPluginIdle )
        {
        TRAPD( err, iXdmUtils->FetchRlsL( iStatus ));
        if ( err )
            {
            CompletePrFwReq( err );
            }
        else
            {
            iXdmState = EPluginFetchRls;
            SetActive();
            }
        }
    else if ( iXdmState == EPluginFetchRls )
        {
        TRAPD( err, iXdmUtils->RemoveRlsServiceByResourceListL( iPresIdentity->Des(), iStatus ));
        if ( err )
            {
            CompletePrFwReq( err );
            }
        else
            {
            iXdmState = EPluginRemoveGroupMember;
            SetActive();
            }
        }
    else
        {
        CompletePrFwReq( status );
        }
    }



// End of file
