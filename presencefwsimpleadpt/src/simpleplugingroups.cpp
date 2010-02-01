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

#include <presentitygroupinfo.h>
#include <presenceobjectfactory.h>
#include <presentitygroupmemberinfo.h>
#include <protocolpresencedatahost.h>
#include <protocolpresentitygroupsdatahost.h>
#include <ximpdatasubscriptionstate.h>
#include <ximpidentity.h>
#include <ximpobjectfactory.h>
#include <ximpobjectcollection.h>
#include <ximpprotocolconnectionhost.h>
#include <ximpstatus.h>

#include <simplefactory.h>
#include <msimpleconnection.h>

#include <pressettingsapi.h>
#include <cpresencexdm.h>
#include <rlspresxdmconsts.h>
#include <XdmErrors.h>
#include <presenceerrors.hrh>

#include "simpleplugingroups.h"
#include "simpleplugindebugutils.h"
#include "simplepluginxdmutils.h"
#include "simpleplugincommon.h"
#include "simplepluginconnection.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSimplePluginGroups::CSimplePluginGroups
// ---------------------------------------------------------------------------
//
CSimplePluginGroups::CSimplePluginGroups(
    MSimplePluginConnectionObs& aObs  )
: CActive( CActive::EPriorityStandard ),
  iConnObs(aObs), iSubscribed(KSimplePluginSubsNone),
  iOperation( ENoOperation),
  iState( EPluginIdle ), iCompleted( ETrue )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::ConstructL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::ConstructL( )
    {
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::NewL
// ---------------------------------------------------------------------------
//
CSimplePluginGroups* CSimplePluginGroups::NewL(
    MSimplePluginConnectionObs& aObs )
    {
    CSimplePluginGroups* self =
        new( ELeave ) CSimplePluginGroups( aObs );
    CleanupStack::PushL( self );
    self->ConstructL(  );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::~CSimplePluginGroups
// ---------------------------------------------------------------------------
//
CSimplePluginGroups::~CSimplePluginGroups()
    {
    delete iPresIdentity;
    delete iPresIdentity2;
    delete iDisplayName;
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::DoSubscribePresentityGroupListL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::DoSubscribePresentityGroupListL(
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: DoSubscribePresentityGroupListL"));
#endif

    iCompleted = EFalse;

    // List of lists under OMa buddylist
    StartXdmOperationL( aReqId, EGetListOfLists );
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::DoUnsubscribePresentityGroupListL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::DoUnsubscribePresentityGroupListL(
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: DoUnsubscribePresentityGroupListL"));
#endif
    iPrFwId = aReqId;
    iCompleted = EFalse;
    CompletePrFwReq( KErrNone );
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::DoCreatePresentityGroupL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::DoCreatePresentityGroupL(
    const MXIMPIdentity& aGroupId,
    const TDesC16& aDisplayName,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: DoCreatePresentityGroupL"));
#endif
    iCompleted = EFalse;
    StartXdmOperationL(
        aGroupId, aDisplayName, aReqId, ECreatePresentityGroup );
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::DoDeletePresentityGroupL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::DoDeletePresentityGroupL(
    const MXIMPIdentity& aGroupId,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: DoDeletePresentityGroupL"));
#endif
    iCompleted = EFalse;
    StartXdmOperationL( aGroupId, aReqId, EDeletePresentityGroup );
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::DoUpdatePresentityGroupDisplayNameL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::DoUpdatePresentityGroupDisplayNameL(
    const MXIMPIdentity& /*aGroupId*/,
    const TDesC16& /*aDisplayName*/,
    TXIMPRequestId /*aReqId*/ )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: DoUpdatePresentityGroupDisplayNameL"));
#endif
    // Notice: not supported.
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::DoSubscribePresentityGroupContentL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::DoSubscribePresentityGroupContentL(
    const MXIMPIdentity& aGroupId,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: DoSubscribePresentityGroupContentL"));
#endif
    iCompleted = EFalse;

    // List of lists under OMa buddylist
    StartXdmOperationL( aGroupId, aReqId, EGetListContent );
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::DoUnsubscribePresentityGroupContentL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::DoUnsubscribePresentityGroupContentL(
    const MXIMPIdentity& /*aGroupId*/,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: DoUnsubscribePresentityGroupContentL"));
#endif
    iCompleted = EFalse;
    iPrFwId = aReqId;
    CompletePrFwReq( KErrNone );
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::DoAddPresentityGroupMemberL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::DoAddPresentityGroupMemberL(
    const MXIMPIdentity& aGroupId,
    const MXIMPIdentity& aMemberId,
    const TDesC16& aMemberDisplayName,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: DoAddPresentityGroupMemberL"));
#endif
    iCompleted = EFalse;
    StartXdmOperationL(
        aGroupId, aMemberId, aMemberDisplayName,
        aReqId, EAddPresentityGroupMember );
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::DoRemovePresentityGroupMemberL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::DoRemovePresentityGroupMemberL(
    const MXIMPIdentity& aGroupId,
    const MXIMPIdentity& aMemberId,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: DoRemovePresentityGroupMemberL"));
#endif
    iCompleted = EFalse;
    StartXdmOperationL(
        aGroupId, aMemberId,
        aReqId, ERemovePresentityGroupMember );
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::DoUpdatePresentityGroupMemberDisplayNameL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::DoUpdatePresentityGroupMemberDisplayNameL(
    const MXIMPIdentity& /*aGroupId*/,
    const MXIMPIdentity& /*aMemberId*/,
    const TDesC16& /*aMemberDisplayName*/,
    TXIMPRequestId /*aReqId*/ )
    {
#ifdef _DEBUG
    PluginLogger::Log(
        _L("PluginGroups: DoUpdatePresentityGroupMemberDisplayNameL"));
#endif
    // Notice: Not supported.
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::DoCancel()
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::DoCancel(  )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: DoCancel"));
#endif
    if ( iOperation != ENoOperation )
        {
        iXdmUtils->Cancel();
        }
    else
        {
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::RunL()
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::RunL(  )
    {
    TInt myStatus = iStatus.Int();

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: RunL status=%d active=%d"), myStatus, IsActive() );
#endif

    if ( !myStatus )
        {
        // OK resposne
        CallActualXdmOperationL();
        }
    else
        {
        if ( iOperation == EDeletePresentityGroup && iState == EPluginCommitRls )
            {
            // Igonere RLS deletion failure and contine Shared XDM deletion
            CallActualXdmOperationL();
            }
        else
            {
            // Other errors terminated the show
            CompletePrFwReq( myStatus );
            }
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::CallActualXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::CallActualXdmOperationL()
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: CallActualXdmOperationL operation=%d"), iOperation );
#endif

    switch ( iOperation )
        {
        case ECreatePresentityGroup:
            HandleCreatePresentityGroupL( iPresIdentity->Des(), iDisplayName->Des() );
            break;
        case EDeletePresentityGroup:
            HandleDeletePresentityGroupL( iPresIdentity->Des() );
            break;
        case EAddPresentityGroupMember:
            HandleAddPresentityGroupMemberL();
            break;
        case ERemovePresentityGroupMember:
            HandleRemovePresentityGroupMemberL();
            break;
        case EGetListOfLists:
            GetListOfListsL();
            break;
        case EGetListContent:
            GetListContentL();
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::RunError
// ---------------------------------------------------------------------------
//
TInt CSimplePluginGroups::RunError( TInt aError )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: RunError %d active=%d"), aError, IsActive());
#endif
    // complete the open request
    CompletePrFwReq( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CSimplePluginGroups::GetInterface(
        TInt32 aInterfaceId,
        TIfGetOps aOptions )
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        MProtocolPresentityGroups* myIf = this;
        return myIf;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( _L("CSimplePlugin"), KErrExtensionNotSupported );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CSimplePluginGroups::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions ) const
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        const MProtocolPresentityGroups* myIf = this;
        return myIf;
        }
    else if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( _L("CSimplePlugin"), KErrExtensionNotSupported );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::GetInterfaceId()
// ---------------------------------------------------------------------------
//
TInt32 CSimplePluginGroups::GetInterfaceId() const
    {
    return MProtocolPresentityGroups::KInterfaceId;
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::SetPresIdentityL()
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::SetPresIdentityL(
    const MXIMPIdentity& aPresentityId )
    {
    delete iPresIdentity;
    iPresIdentity = NULL;
    iPresIdentity = aPresentityId.Identity().AllocL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::SetPresIdentity2L()
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::SetPresIdentity2L(
    const MXIMPIdentity& aPresentityId )
    {
    delete iPresIdentity2;
    iPresIdentity2 = NULL;
    iPresIdentity2 = aPresentityId.Identity().AllocL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::StartXdmOperationL(
    TXIMPRequestId aReqId,
    TPluginGroupsOperation aOperation )
    {
    __ASSERT_DEBUG( !IsActive(), User::Leave( KErrCorrupt ) );
    iPrFwId = aReqId;
    iOperation = aOperation;
    StartXdmOperationL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::StartXdmOperationL(
    const MXIMPIdentity& aId,
    TXIMPRequestId aReqId,
    TPluginGroupsOperation aOperation )
    {
    __ASSERT_DEBUG( !IsActive(), User::Leave( KErrCorrupt ) );
    SetPresIdentityL( aId );
    iPrFwId = aReqId;
    iOperation = aOperation;
    StartXdmOperationL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::StartXdmOperationL(
        const MXIMPIdentity& aId,
        const TDesC16& aDisplayName,
        TXIMPRequestId aReqId,
        TPluginGroupsOperation aOperation )
    {
    __ASSERT_DEBUG( !IsActive(), User::Leave( KErrCorrupt ) );
    SetPresIdentityL( aId );

    delete iDisplayName;
    iDisplayName = NULL;
    iDisplayName = aDisplayName.AllocL();

    iPrFwId = aReqId;
    iOperation = aOperation;
    StartXdmOperationL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::StartXdmOperationL(
        const MXIMPIdentity& aId,
        const MXIMPIdentity& aMemberId,
        TXIMPRequestId aReqId,
        TPluginGroupsOperation aOperation )
    {
    __ASSERT_DEBUG( !IsActive(), User::Leave( KErrCorrupt ) );
    SetPresIdentityL( aId );
    SetPresIdentity2L( aMemberId );
    iPrFwId = aReqId;
    iOperation = aOperation;
    StartXdmOperationL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::StartXdmOperationL(
        const MXIMPIdentity& aId,
        const MXIMPIdentity& aMemberId,
        const TDesC16& aDisplayName,
        TXIMPRequestId aReqId,
        TPluginGroupsOperation aOperation )
    {
    __ASSERT_DEBUG( !IsActive(), User::Leave( KErrCorrupt ) );
    SetPresIdentityL( aId );
    SetPresIdentity2L( aMemberId );

    delete iDisplayName;
    iDisplayName = NULL;
    iDisplayName = aDisplayName.AllocL();

    iPrFwId = aReqId;
    iOperation = aOperation;
    StartXdmOperationL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::StartXdmOperationL()
    {
    TRAPD( err, DoStartXdmOperationL() );
    if ( err )
        {
        User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
        }
    else
        {
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::DoStartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::DoStartXdmOperationL()
    {
    if ( !iXdmUtils )
        {
        iXdmUtils = iConnObs.XdmUtilsL();
        }
    else
        {
        }

    // Delete Group is the only method that access XDM rules,
    // others access shared lists only.
    if ( iOperation != EDeletePresentityGroup )
        {
        iXdmUtils->InitializeXdmsOnlyL( iStatus );
        }
    else
        {
        iXdmUtils->InitializeXdmL( iStatus );
        }

    iState = EPluginInitXdm;
    SetActive();
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::GetListOfListsL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::GetListOfListsL()
    {
    MXIMPObjectFactory& myFactory = iConnObs.ObjectFactory();
    MProtocolPresentityGroupsDataHost& dataHost =
        iConnObs.Host()->ProtocolPresenceDataHost().GroupsDataHost();

    // This searches data from the local cache only.
    MXIMPObjectCollection* userLists =
        myFactory.NewObjectCollectionLC();          // << userLists

    const TInt KMyGran = 10;
    CDesCArrayFlat* lists = new (ELeave) CDesCArrayFlat( KMyGran );
    CleanupStack::PushL( lists );      // << lists

    CDesCArrayFlat* dNames = new (ELeave) CDesCArrayFlat( KMyGran );
    CleanupStack::PushL( dNames );      // << dNames

    iXdmUtils->GetUserListsL( *lists, *dNames );

    CopyGroupArraysToCollectionL( *lists, *dNames, *userLists );

    CleanupStack::PopAndDestroy( dNames );  // >> dNames
    CleanupStack::PopAndDestroy( lists );   // >> lists

    CompletePrFwReq( KErrNone );

    // Callback for subscription state (terminated).
    MXIMPDataSubscriptionState* myState =
        myFactory.NewDataSubscriptionStateLC();
    MXIMPStatus* myStatus = myFactory.NewStatusLC();
    // Notice: consider error codes
    myStatus->SetResultCode( KErrNone );
    myState->SetSubscriptionStateL(
        MXIMPDataSubscriptionState::ESubscriptionInactive );
    myState->SetDataStateL( MXIMPDataSubscriptionState::EDataUnavailable );
#ifdef _DEBUG
    PluginLogger::Log(
    _L("PluginGroups: callback SetPresentityGroupListDataSubscriptionStateL"));
#endif
    dataHost.SetPresentityGroupListDataSubscriptionStateL(
        myState, myStatus );


    CleanupStack::Pop( 2 ); // >> myState, myStatus

    // callback for data
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: callback HandlePresentityGroupListL"));
#endif
    dataHost.HandlePresentityGroupListL( userLists );
    CleanupStack::Pop();     // >> userLists
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::GetListContentL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::GetListContentL()
    {
    MXIMPObjectFactory& myFactory = iConnObs.ObjectFactory();
    MProtocolPresentityGroupsDataHost& dataHost =
        iConnObs.Host()->ProtocolPresenceDataHost().GroupsDataHost();

    MXIMPObjectCollection* entities =
        myFactory.NewObjectCollectionLC();          // << entities

    const TInt KMyGran = 10;
    CPtrCArray* ids = new (ELeave) CPtrCArray( KMyGran );
    CleanupStack::PushL( ids );      // << ids

    CPtrCArray* dispNames = new (ELeave) CPtrCArray( KMyGran );
    CleanupStack::PushL( dispNames  );      // << dispNames


    iXdmUtils->GetEntitiesInListL( iPresIdentity->Des(), *ids, *dispNames );

    CopyGroupMembersToCollectionL( *ids, *dispNames, *entities );

    CleanupStack::PopAndDestroy( dispNames );
    CleanupStack::PopAndDestroy( ids );

    // Complete the PrFw request
    CompletePrFwReq( KErrNone );

    // callback for data
    MXIMPIdentity* id = myFactory.NewIdentityLC();  // << id
    id->SetIdentityL( iPresIdentity->Des() );
#ifdef _DEBUG
    PluginLogger::Log(
        _L("PluginGroups: callback HandlePresentityGroupContentL ."));
#endif
    dataHost.HandlePresentityGroupContentL( id, entities );
    CleanupStack::Pop();                  // >> id
    CleanupStack::Pop();                  // >> entities

    // Callback for subscription state (terminated).
    id = myFactory.NewIdentityLC();                     // << id
    id->SetIdentityL( iPresIdentity->Des() );
    MXIMPDataSubscriptionState* myState =
        myFactory.NewDataSubscriptionStateLC();         // << myState
    MXIMPStatus* myStatus = myFactory.NewStatusLC();    // << myStatus
    // Notice: consider error codes
    myStatus->SetResultCode( KErrNone );
    myState->SetSubscriptionStateL(
        MXIMPDataSubscriptionState::ESubscriptionInactive );
#ifdef _DEBUG
    PluginLogger::Log(
        _L("PluginGroups: callback SetPresentityGroupContentDataSubscriptionStateL"));
#endif
    dataHost.SetPresentityGroupContentDataSubscriptionStateL(
        id, myState, myStatus );
    CleanupStack::Pop( 3 ); // >> myStatus, myState, id
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::CompletePrFwReq
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::CompletePrFwReq( TInt aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: CompletePrFwReq status=%d"), aStatus );
#endif

    if ( iCompleted  )
        {
        return;
        }

    iCompleted = ETrue;
    iOperation = ENoOperation;
    iConnObs.CompleteReq( iPrFwId, aStatus );
    iPrFwId = TXIMPRequestId();
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::CompleteMe
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::CompleteMe( TInt aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginGroups: CompleteMe status=%d"), aStatus );
#endif

    iStatus = KRequestPending;
    TRequestStatus* s= &iStatus;
    User::RequestComplete( s, aStatus );
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::CopyGroupArraysToCollectionL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::CopyGroupArraysToCollectionL(
    CDesCArrayFlat& aLists, CDesCArrayFlat& aNames, MXIMPObjectCollection& aCollection )
    {
    TInt count = aLists.Count();
    for ( TInt i=0; i < count; i++ )
        {
        // create here the PrFW group info
        MPresentityGroupInfo* info =
            iConnObs.PresenceObjectFactory().NewPresentityGroupInfoLC(); // << info
        MXIMPIdentity* id = iConnObs.ObjectFactory().NewIdentityLC();  // << id
        id->SetIdentityL( aLists[i] );
        info->SetGroupIdL( id );
        CleanupStack::Pop(); // >> id

        info->SetGroupDisplayNameL( aNames[i] );
        aCollection.AddObjectL( info );
        CleanupStack::Pop(); // >> info
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::CopyGroupMembersToCollectionL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::CopyGroupMembersToCollectionL(
    CPtrCArray& aIds, CPtrCArray& aNames, MXIMPObjectCollection& aCollection)
    {
    TInt count = aIds.Count();
    for ( TInt i=0; i < count; i++ )
        {
        // create here MPresentityGroupMemberInfo
        MPresentityGroupMemberInfo* info =
            iConnObs.PresenceObjectFactory().NewPresentityGroupMemberInfoLC(); // << info
        MXIMPIdentity* id = iConnObs.ObjectFactory().NewIdentityLC();  // << id
        id->SetIdentityL( aIds[i] );
        info->SetGroupMemberIdL( id );
        CleanupStack::Pop(); // >> id

        info->SetGroupMemberDisplayNameL( aNames[i] );
        aCollection.AddObjectL( info );
        CleanupStack::Pop(); // >> info
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::HandleCreatePresentityGroupL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::HandleCreatePresentityGroupL(
    const TDesC& aGroupId, const TDesC& aDispName )
    {
    if ( iState == EPluginInitXdm )
        {
        iState = EPluginCommitXdm;
        iXdmUtils->CreateEntityGroupL( aGroupId, aDispName );
        iXdmUtils->CommitXdmL( iStatus );
        SetActive();
        }
    else
        {
        // We are ready
        CompletePrFwReq( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::HandleDeletePresentityGroupL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::HandleDeletePresentityGroupL( const TDesC& aGroupId )
    {
    if ( iState == EPluginInitXdm )
        {
        iState = EPluginFetchRls;
        iXdmUtils->FetchRlsL( iStatus );
        SetActive();
        }
    else if ( iState == EPluginFetchRls )
        {
        iState = EPluginCommitRls;
        // Remove RLS service
        iXdmUtils->RemoveRlsServiceByResourceListL( aGroupId, iStatus );
        SetActive();
        }
    else if ( iState == EPluginCommitRls )
        {
        iState = EPluginCommitXdm;
        iXdmUtils->DeleteEntityGroupL( aGroupId );
        iXdmUtils->CommitXdmL( iStatus );
        SetActive();
        }
    else if ( iState == EPluginCommitXdm )
        {
        // Delete the group from a granted rule. The following won't leave if
        // it does not locate under the rule.
#ifdef _DEBUG
        PluginLogger::Log(
                _L("PluginWatcher: TEST COVERS 2 **"));
#endif
        iState = ERemoveGroupFromGranted;
        iXdmUtils->RemoveGroupFromGrantedL( aGroupId, iStatus );
        SetActive();
        }
    else
        {
        // We are ready
        CompletePrFwReq( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::HandleAddPresentityGroupMemberL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::HandleAddPresentityGroupMemberL( )
    {
    if ( iState == EPluginInitXdm )
        {
        iXdmUtils->AddPresentityGroupMemberL(
            iPresIdentity->Des(), iPresIdentity2->Des(),
            iDisplayName->Des(), iStatus );
        iState = EPluginAddGroupMember;
        SetActive();
        }
    else
        {
        // We are ready
        CompletePrFwReq( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginGroups::HandleRemovePresentityGroupMemberL
// ---------------------------------------------------------------------------
//
void CSimplePluginGroups::HandleRemovePresentityGroupMemberL()
    {
    if ( iState == EPluginInitXdm )
        {
        iXdmUtils->RemovePresentityGroupMemberL(
            iPresIdentity->Des(), iPresIdentity2->Des(), iStatus );
        iState = EPluginRemoveGroupMember;
        SetActive();
        }
    else
        {
        // We are ready
        CompletePrFwReq( KErrNone );
        }
    }


// End of file

