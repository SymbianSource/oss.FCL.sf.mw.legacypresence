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

#include <simplefactory.h>
#include <msimpleconnection.h>

#include <pressettingsapi.h>
#include <cpresencexdm.h>
#include <rlspresxdmconsts.h>
#include <XdmErrors.h>
#include <prescondmisc.h>
#include <prescondidentityone.h>
#include <presenceactionxdm.h>
#include <presencetransformxdm.h>
#include <crlsxdm.h>

#include <XdmEngine.h>
#include <XdmDocument.h>
#include <XdmProtocolInfo.h>
#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include <XdmSettingsApi.h>
#include <XdmErrors.h>

#include "simpleplugindebugutils.h"
#include "simpleplugincommon.h"
#include "simplepluginxdmutils.h"
#include "simpleplugindata.h"
#include "simplepluginvariation.h"
#include "msimplepluginsettings.h"

// Test suite
#include "msimpleplugintestobs.h"

// This is for debugging and local test mode oly
#include <in_sock.h>

// URI prefixies
const TInt KMyLenPrefix = 4;
_LIT16( KMySip, "sip:" );
_LIT16( KMyTel, "tel:" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::CSimplePluginXdmUtils
// ---------------------------------------------------------------------------
//
CSimplePluginXdmUtils::CSimplePluginXdmUtils(
    MSimplePluginSettings& aConnSets, TInt aXmdId )
: CActive( EPriorityStandard ),
  iConnSets(aConnSets), iSettingsId(aXmdId),
  iXdmState(EStateIdle), iOperation( ENoOperation), iTestObs( NULL )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::ConstructL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::ConstructL(  )
    {
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::NewL
// ---------------------------------------------------------------------------
//
CSimplePluginXdmUtils* CSimplePluginXdmUtils::NewL(
    MSimplePluginSettings& aConnSets,
    TInt aXmdId )
    {
    CSimplePluginXdmUtils* self =
        new( ELeave ) CSimplePluginXdmUtils( aConnSets, aXmdId );
    CleanupStack::PushL( self );
    self->ConstructL(   );
    CleanupStack::Pop( self );
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: NewL this=%d"), (TInt)self);
#endif

    return self;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::~CSimplePluginXdmUtils()
// ---------------------------------------------------------------------------
//
CSimplePluginXdmUtils::~CSimplePluginXdmUtils()
    {

    Cancel();

    CancelWrappers();

    if ( iXdmDoc && iXdmEngine )
        {
        TRAP_IGNORE( iXdmEngine->DeleteDocumentModelL( iXdmDoc ));
        }
    delete iRlsXdm;
    delete iPresenceXdm;
    delete iXdmEngine;
    delete iAuxBuffer;

    delete iAuxBuffer2;

    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::InitializeXdmL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::InitializeXdmL( TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: InitializeXdmL") );
#endif

    iOperation = EXdmInit;
    iClientStatus = &aStatus;

    if ( !iXdmOk )
        {
        GetXdmOMAListsL();
        iXdmState = EGetXdmOMALists;
        *iClientStatus = KRequestPending;
        }
    else if ( !iPresXdmOk )
        {
        GetXdmRulesL();
        *iClientStatus = KRequestPending;
        }
    else
        {
        // All is ok
        *iClientStatus = KRequestPending;
        CompleteClientReq( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::InitializeXdmsOnlyL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::InitializeXdmsOnlyL( TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: InitializeXdmsOnlyL") );
#endif

    iOperation = EXdmInitXdmsOnly;
    iClientStatus = &aStatus;

    if ( !iXdmOk )
        {
        GetXdmOMAListsL();
        iXdmState = EGetXdmOMAListsOnly;
        *iClientStatus = KRequestPending;
        }
    else
        {
        // All is ok
        *iClientStatus = KRequestPending;
        CompleteClientReq( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::RunL()
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::RunL(  )
    {
    TPluginXdmState origState = iXdmState;
    iXdmState = EStateIdle;
    TInt myStatus = iStatus.Int();

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: RunL %d"), myStatus );
#endif

    if ( iTestObs )
        {
        // Test suite
        iTestObs->TestCallbackInRunL(
            myStatus, origState, iOperation, KNullDesC );
        }

#ifdef FAKE_XDM_OK
#ifdef _DEBUG
    // -191 = KErrHostUnreach in_sock.h
    if ( myStatus == KErrTimedOut || myStatus == KXcapErrorNetworkNotAvailabe || -191 )
        {
        PluginLogger::Log(_L("PluginXdmUtils : RunL FAKE_XDM_OK resets error code %d"), myStatus );
        myStatus = 0;
        }
#endif
#endif

    DoXdmRunL( origState, myStatus );
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoXdmRunL()
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::DoXdmRunL( TPluginXdmState aOrigState, TInt aStatus )
    {

    switch ( aOrigState )
        {
        case EGetXdmOMALists:
        case EGetXdmOMAListsOnly:
            {
            // no OMA resource-list, let's start to create it.
            if ( CreateXdmOMAListsIfNeededL() )
                {
                iXdmState = aOrigState == EGetXdmOMALists ? ECreateXdmOMALists : ECreateXdmOMAListsOnly;
                }
            else
                {
                iXdmOk = ETrue;
                if ( !iPresXdmOk && aOrigState == EGetXdmOMALists )
                    {
                    GetXdmRulesL();
                    }
                else
                    {
                    CompleteClientReq( KErrNone );
                    }
                }
            }
            break;
        case ECreateXdmOMALists:
            {
            if ( aStatus )
                {
                CompleteClientReq( aStatus );
                }
            else
                {
                iXdmOk = ETrue;
                GetXdmRulesL();
                }
            }
            break;
        case ECreateXdmOMAListsOnly:
            {
            if ( !aStatus )
                {
                iXdmOk = ETrue;
                }
            CompleteClientReq( aStatus );
            }
            break;
        default:
            // complete reqular request
            CompleteClientReq( aStatus );
            break;
        };
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::CancelWrappers()
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::CancelWrappers( )
    {
    iOperation = EXdmCancel;

    // Cancel M-class interface methods that calls callback method.
    if ( iXdmState == EGetRlsServices ||
         iXdmState == EUpdateRlsServices ||
         iXdmState == EReUpdateRlsServices )
        {
        iXdmState = ECancelDocument;
#ifdef _DEBUG
        PluginLogger::Log(_L("PluginXdm: -> iRlsXdm->Cancel"));
#endif
        // Better call Cancel() instead of CancelUpdateL().
        // It works and is easier to implement. Otherwise we should have here
        // SetActive(); iStatus=KRequestPending; User::WaitForRequest(iStatus)
        // and we should have implemented HandleRLSUpdateCancelL that completes
        // own iStatus.
        iRlsXdm->Cancel();
        }
    else if ( iXdmState == EGetXdmRules ||
              iXdmState == EUpdateXdmRules )
        {
        iXdmState = ECancelDocument;
#ifdef _DEBUG
        PluginLogger::Log(_L("PluginXdm: -> iPresenceXdm->Cancel"));
#endif
        iPresenceXdm->Cancel();
        }
    else
        {
        }
    }
// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoCancel()
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::DoCancel(  )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: DoCancel"));
#endif
    iOperation = EXdmCancel;

    if ( iXdmState == EGetXdmOMALists || iXdmState == ECreateXdmOMALists ||
         iXdmState == EGetXdmOMAListsOnly || iXdmState == ECreateXdmOMAListsOnly ||
         iXdmState == EUpdateXdmOMAList )
        {
        // Cancel a request made to XDM Engine.
        iXdmState = ECancelDocument;
#ifdef _DEBUG
      PluginLogger::Log(_L("PluginXdm: iXdmEngine->CancelUpdate"));
#endif
        iXdmEngine->CancelUpdate( iXdmDoc );
        CompleteClientReq( KErrCancel );
        }
    else
        {
        // Cancel a request where we have completed our own iStatus.
        // Very rare posibility that this happens when GetXdmOMALists is not ready.
        // It's very hard to make it happen in test suite.
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::RunError
// ---------------------------------------------------------------------------
//
TInt CSimplePluginXdmUtils::RunError( TInt aError )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: RunError"));
#endif
    // complete the open request
    CompleteClientReq( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::FetchRlsL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::FetchRlsL(
    TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: FetchRlsL"));
#endif
    _LIT( KMyOper, "FetchRlsL");

    iClientStatus = &aStatus;

    iOperation = EXdmRlsFetch;

    if ( !iRlsXdm )
        {
        iRlsXdm = CRLSXDM::NewL( iSettingsId );
        }

    if ( iTestObs )
        {
        // Test suite
        iTestObs->TestCallbackAsynchL(
            KErrNone, iXdmState, iOperation, KMyOper );
        }

    // The following returns in MRLSPresXDMAsyncHandler callback
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: -> iRlsXdm->UpdateAllFromServerL"));
#endif
    User::LeaveIfError( iRlsXdm->UpdateAllFromServerL( this ));

    *iClientStatus = KRequestPending;

    iXdmState = EGetRlsServices;

    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::AddRlsGroupL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::AddRlsGroupL(
    const TDesC&  aName )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: AddRlsGroupL"));
#endif
    iOperation = EXdmRlsAdd;

    HBufC16* buf = CreateRlsDocumentUri16LC(
        aName, iConnSets.CurrentSipPresentity(), iConnSets.CurrentDomain() );

    // KErrAlreadyExists
    TInt errx = iRlsXdm->AddServiceURIL( buf->Des() );
    if ( errx && errx != KErrAlreadyExists )
        {
        User::Leave( errx );
        }
    User::LeaveIfError( iRlsXdm->SwitchToServiceURIL( buf->Des() ));

    HBufC* xcapUri = TXdmSettingsApi::PropertyL( iSettingsId, EXdmPropUri );
    CleanupStack::PushL( xcapUri );             // << xcapUri

    HBufC* listUri = MakeRlsBuddyUriL( xcapUri->Des(), aName );
    CleanupStack::PushL( listUri );             // << listUri

    TInt err = iRlsXdm->AddElementL( KPresResourceList,
        listUri->Des(), aName );
    if ( err && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }

    // Save aName for RLS Service URI negoriation handling (HTTP error 409).
    // Notice: We assume that only one AddRlsGroupL is called between CommitRlsL methods.

    delete iAuxBuffer;
    iAuxBuffer = NULL;
    iAuxBuffer = aName.AllocL();

    CleanupStack::PopAndDestroy( listUri );     // >> listUri
    CleanupStack::PopAndDestroy( xcapUri );     // >> xcapUri
    CleanupStack::PopAndDestroy( buf );
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::RemoveRlsGroupL
// Use RemoveRlsServiceByResourceListL when possible
//
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::RemoveRlsGroupL(
    const TDesC&  aName )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: RemoveRlsGroupL"));
#endif

    iOperation = EXdmRlsRemove;

    HBufC16* buf = CreateRlsDocumentUri16LC(
        aName, iConnSets.CurrentSipPresentity(), iConnSets.CurrentDomain() );

    User::LeaveIfError( iRlsXdm->RemoveServiceURIL( buf->Des()));
    CleanupStack::PopAndDestroy( buf );
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::AddEntityToGrantedL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::AddEntityToGrantedL(
    const TDesC&  aUri, TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: AddEntityToGrantedL"));
#endif
    iClientStatus = &aStatus;

    iOperation = EXdmAddUserToGroup;

    DoAddUserToListL( iConnSets.Variation().GrantListName(), aUri );

    // send to the server and start wait a response
    UpdateXdmsL();
    iXdmState = EUpdateXdmOMAList;

    *iClientStatus = KRequestPending;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::AddEntityToBlockedL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::AddEntityToBlockedL(
    const TDesC&  aUri, TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: AddEntityToBlockedL"));
#endif
    iClientStatus = &aStatus;

    iOperation = EXdmAddUserToGroup;
    DoAddUserToListL( KSimpleOMABlockedList, aUri );

    // send to the server and start wait a response
    UpdateXdmsL();
    iXdmState = EUpdateXdmOMAList;

    *iClientStatus = KRequestPending;
    }

// ----------------------------------------------------
// CSimplePluginXdmUtils::DoAddUserToListL
// ----------------------------------------------------
//
void CSimplePluginXdmUtils::DoAddUserToListL(
    const TDesC& aList, const TDesC&  aUri )
    {

    using namespace NSimplePlugin::NSimpleOma;

    RPointerArray<CXdmDocumentNode> lists;
    CleanupClosePushL( lists );                 // <<  lists

    CXdmDocumentNode* root = iXdmDoc->DocumentRoot();
    CXdmDocumentNode* foundNode = SearchListUnderParentL( root, aList );

    if ( foundNode )
        {
        if ( foundNode->IsEmptyNode() )
            {
            foundNode->SetEmptyNode( EFalse );
            }

        CXdmDocumentNode* newNode = foundNode->CreateChileNodeL( KSimpleEntry );
        CXdmNodeAttribute* attributeEntryUri = newNode->CreateAttributeL( KSimpleUri );
        attributeEntryUri->SetAttributeValueL( aUri );
        iXdmDoc->AppendL( newNode );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( &lists  );     // >> lists
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::RemoveEntityFromGrantedL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::RemoveEntityFromGrantedL(
    const TDesC&  aUri, TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: RemoveEntityFromGrantedL"));
#endif
    iClientStatus = &aStatus;

    iOperation = EXdmRemoveUserFromGroup;
    DoRemoveUserFromListL( iConnSets.Variation().GrantListName(), aUri );

    // send to the server and start wait a response
    UpdateXdmsL();
    iXdmState = EUpdateXdmOMAList;

    *iClientStatus = KRequestPending;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::RemoveEntityFromBlockedL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::RemoveEntityFromBlockedL(
    const TDesC&  aUri, TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: RemoveEntityFromBlockedL"));
#endif
    iClientStatus = &aStatus;

    iOperation = EXdmRemoveUserFromGroup;
    DoRemoveUserFromListL( KSimpleOMABlockedList, aUri );

    // send to the server and start wait a response
    UpdateXdmsL();
    iXdmState = EUpdateXdmOMAList;

    *iClientStatus = KRequestPending;
    }

// ----------------------------------------------------
// CSimplePluginXdmUtils::DoRemoveUserFromListL
// ----------------------------------------------------
//
void CSimplePluginXdmUtils::DoRemoveUserFromListL(
    const TDesC& aList, const TDesC&  aUri )
    {
    CXdmNodeAttribute* attr = NULL;

    using namespace NSimplePlugin::NSimpleOma;

    RPointerArray<CXdmDocumentNode> lists;
    CleanupClosePushL( lists );

    RPointerArray<CXdmDocumentNode> nodes;
    CleanupClosePushL( nodes );

    CXdmDocumentNode* root = iXdmDoc->DocumentRoot();
    CXdmDocumentNode* foundNode = SearchListUnderParentL( root, aList );
    if ( foundNode )
        {
        User::LeaveIfError( foundNode->Find( KSimpleEntry, nodes ));
        TInt nodeCount = nodes.Count();
        for ( TInt i = 0; i < nodeCount; i++ )
            {
            CXdmDocumentNode* currNode = nodes[i];
            attr = ( currNode )->Attribute( KSimpleUri );
            if ( attr && !attr->AttributeValue().CompareF( aUri ))
                {
                // This is the user we are looking for deletion.
                iXdmDoc->DeleteDataL( currNode );
                break;
                }
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( &nodes ); // >>> nodes
    CleanupStack::PopAndDestroy( &lists ); // >>> lists
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::RemoveGroupFromGrantedL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::RemoveGroupFromGrantedL(
    const TDesC&  aName, TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: RemoveGroupFromGrantedL"));
#endif
    iClientStatus = &aStatus;

    HBufC* xcapUri = TXdmSettingsApi::PropertyL( iSettingsId, EXdmPropUri );
    CleanupStack::PushL( xcapUri );             // << xcapUri

    HBufC* listUri = MakeRlsBuddyUriL( xcapUri->Des(), aName );
    CleanupStack::PushL( listUri );             // << listUri

    DoRemoveListFromGrantRuleL( listUri->Des() );

    CleanupStack::PopAndDestroy( listUri );
    CleanupStack::PopAndDestroy( xcapUri );

    iOperation = EXdmWithdrawGroup;
    }

// ----------------------------------------------------
// CSimplePluginXdmUtils::DoRemoveListFromGrantRuleL
// ----------------------------------------------------
//
void CSimplePluginXdmUtils::DoRemoveListFromGrantRuleL(
    const TDesC&  aListUri )
    {
    TPresCondMisc misc;
    misc.Init( iPresenceXdm );

    if ( !misc.DeleteExternalListL( iConnSets.Variation().GrantRuleName(), aListUri ))
        {
        // update document in the server only if there was a modification.
        UpdateXdmRulesL();
        *iClientStatus = KRequestPending;
        }
    else
        {
        // Complete without server operation
        CompleteClientReq( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::AddGroupToGrantedL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::AddGroupToGrantedL(
    const TDesC&  aName, TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: AddGroupToGrantedL"));
#endif

    iClientStatus = &aStatus;

    if ( !DoesUserListExistsL( aName )  )
      {
      User::Leave( KErrNotFound );
      }

    HBufC* xcapUri = TXdmSettingsApi::PropertyL( iSettingsId, EXdmPropUri );
    CleanupStack::PushL( xcapUri );             // << xcapUri

    HBufC* listUri = MakeRlsBuddyUriL( xcapUri->Des(), aName );
    CleanupStack::PushL( listUri );             // << listUri

    iOperation = EXdmGrantGroup;

    DoAddListIntoGrantRuleL( listUri->Des() );

    CleanupStack::PopAndDestroy( listUri );     // >> listUri
    CleanupStack::PopAndDestroy( xcapUri );     // >> xcapUri

    *iClientStatus = KRequestPending;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoAddListIntoGrantRuleL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::DoAddListIntoGrantRuleL(
    const TDesC& aListUri)
    {
    TPresCondMisc misc;
    misc.Init( iPresenceXdm );

    User::LeaveIfError( misc.AddExternListL( iConnSets.Variation().GrantRuleName(), aListUri ));

    UpdateXdmRulesL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::GrantForEveryoneL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::GrantForEveryoneL( TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: GrantForEveryoneL"));
#endif
    iClientStatus = &aStatus;

    iOperation = EXdmGrantForAll;
    DoGrantForAllL();

    *iClientStatus = KRequestPending;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::WithdrawFromEveryoneL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::WithdrawFromEveryoneL( TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: WithdrawFromEveryoneL"));
#endif
    iClientStatus = &aStatus;

    iOperation = EXdmWithdrawFromAll;
    DoWithdrawFromAllL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::SetReactiveAuthL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::SetReactiveAuthL( TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: SetReactiveAuthL"));
#endif
    iClientStatus = &aStatus;

    iOperation = EXdmSetReactiveAuth;
    DoSetReactiveAuthL();

    *iClientStatus = KRequestPending;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::SetProactiveAuthL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::SetProactiveAuthL( TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: SetProactiveAuthL"));
#endif
    iClientStatus = &aStatus;

    iOperation = EXdmSetProactiveAuth;
    DoSetProactiveAuthL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::SubscribeBlockListL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::SubscribeBlockListL( CPtrCArray& aMembers )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: SubscribeBlockListL"));
#endif
    iOperation = EXdmGetBlockedList;
    DoGetListMembersL( KSimpleOMABlockedList, aMembers );
    // The cient will call a method GetEntitiesL that access those arrays
    // Those member arrays will be emptied after the call. + in destrcutor.
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::UnsubscribeBlockListL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::UnsubscribeBlockListL( TRequestStatus& aStatus )
    {
    iClientStatus = &aStatus;
    *iClientStatus = KRequestPending;
    // Subscription state is always terminated
    CompleteClientReq( KErrNone );
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoGetListMembersL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::DoGetListMembersL(
    const TDesC& aList, CPtrCArray& aMembers  )
    {
    using namespace NSimplePlugin::NSimpleOma;

    aMembers.Reset();

    RPointerArray<CXdmDocumentNode> nodes;
    CleanupClosePushL( nodes );

    CXdmNodeAttribute* attr = NULL;

    CXdmDocumentNode* buddylist = DoGetBuddyListL();
    CXdmDocumentNode* foundNode = SearchListUnderParentL( buddylist, aList );
    if ( foundNode )
        {
        nodes.Reset();
        // Do not leave if there is nobody blocked
        TRAP_IGNORE( foundNode->Find( KSimpleEntry, nodes ));
        TInt nodeCount = nodes.Count();
        for ( TInt i = 0; i < nodeCount; i++ )
            {
            attr = (nodes[i])->Attribute( KSimpleUri );
            if ( attr )
                {
                aMembers.AppendL( attr->AttributeValue() );
                }
            }
        }

    CleanupStack::PopAndDestroy( &nodes );
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::HandleRLSUpdateDocumentL
// RLS Callback
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::HandleRLSUpdateDocumentL( TInt aErrorCode )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: HandleRLSUpdateDocumentL %d"), aErrorCode);
#ifdef FAKE_XDM_OK

    if ( aErrorCode == KErrTimedOut || aErrorCode == KXcapErrorNetworkNotAvailabe || aErrorCode == KErrHostUnreach )
        {
        PluginLogger::Log(
            _L("PluginXdm : HandleRLSUpdateDocumentL FAKE_XDM_OK resets error code %d"),
            aErrorCode  );
        aErrorCode  = 0;
        }
#endif
#endif

    TRAPD( err, DoHandleRLSUpdateDocumentL( aErrorCode ));
    if ( err )
        {
        // Complete with ok or error the last initial opreration
        CompleteClientReq( err );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoHandleRLSUpdateDocumentL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::DoHandleRLSUpdateDocumentL( TInt aErrorCode )
    {

    if ( iTestObs )
        {
        // Test suite
        iTestObs->TestCallbackInRunL( aErrorCode, iXdmState, iOperation, KNullDesC );
        }

    // Resend 409 failed request only if the server sent a preposal for the right URI
    // and only once, so that this do not remain in endless loop between server and
    // a client.
    if ( aErrorCode == KXcapErrorHttpConflict &&
         iOperation == EXdmRlsAdd &&
         iRlsXdm->NegotiatedServiceUri().Length() )
        {
        RlsServiceUriNegotiationL();
        return;
        }

    switch ( iXdmState )
        {
        case EGetRlsServices:
            if ( aErrorCode == KXcapErrorHttpNotFound || aErrorCode == KErrNotFound )
                {
                aErrorCode = KErrNone;
                }
            break;
        default:
            break;
        }

    CompleteClientReq( aErrorCode );
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::HandleRLSUpdateCancelL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::HandleRLSUpdateCancelL( TInt /*aErrorCode*/ )
    {
    // notice: not needed
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::HandleRLSDeleteAllEmptyListsL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::HandleRLSDeleteAllEmptyListsL( TInt /*aErrorCode*/ )
    {
    // Notice: Not needed
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::HandlePresUpdateDocumentL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::HandlePresUpdateDocumentL( TInt aErrorCode )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: HandlePresUpdateDocumentL %d"), aErrorCode);
#ifdef FAKE_XDM_OK
        if ( aErrorCode == KErrTimedOut || aErrorCode == KXcapErrorNetworkNotAvailabe || aErrorCode == KErrHostUnreach )
            {
            PluginLogger::Log(_L("PluginXdm : HandlePresUpdateDocumentL FAKE_XDM_OK resets error code %d"), aErrorCode  );
            aErrorCode  = 0;
            }
#endif
#endif

    TRAPD( err, DoHandlePresUpdateDocumentL( aErrorCode ));
    if ( err )
        {
        // Complete with ok or error the last initial opreration
        CompleteClientReq( err );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoHandlePresUpdateDocumentL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::DoHandlePresUpdateDocumentL( TInt aErrorCode )
    {

    if ( iTestObs )
        {
        // Test suite
        iTestObs->TestCallbackInRunL(
            aErrorCode, iXdmState, iOperation, KNullDesC );
        }

    if ( iXdmState == EGetXdmRules )
        {
        iXdmState = EStateIdle;
        if ( aErrorCode && aErrorCode != KXcapErrorHttpNotFound &&
             aErrorCode != KErrNotFound )
            {
            User::Leave( aErrorCode );
            }
        // check and create the rules when needed
        MakeInitialXdmsDocumentL();
        }
    else
        {
        iPresXdmOk = ETrue;
        // Complete with ok or error the last initial opreration
        CompleteClientReq( aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::HandlePresUpdateCancelL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::HandlePresUpdateCancelL( TInt /*aErrorCode*/ )
    {
    // notice: not needed
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::GetXdmOMAListsL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::GetXdmOMAListsL()
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: GetXdmOMAListsL"));
#endif
    if ( !iXdmEngine )
        {
        // get data from a network if not done so yet
        CXdmProtocolInfo* info = CXdmProtocolInfo::NewL( iSettingsId );
        CleanupStack::PushL( info );
        // Notice: we don't use cache for XDM
        info->SetCacheUsage( EFalse );
        iXdmEngine = CXdmEngine::NewL( *info );
        CleanupStack::PopAndDestroy( info );
        }
    if ( !iXdmOk )
        {
        using namespace NSimplePlugin::NSimpleOma;
        if ( iXdmDoc )
            {
            // Clean a document from s previous failed case.
            iXdmEngine->DeleteDocumentModelL( iXdmDoc );
            iXdmDoc = NULL;
            }

        // Get data from network
        iXdmDoc = iXdmEngine->CreateDocumentModelL( KSimpleIndex, EXdmSharedXdm );
        iXdmDoc->FetchDataL();
        UpdateXdmsL();
        iXdmState = EGetXdmOMALists;
        }
    else
        {
        // Document already exists, no need to search from a server
        iStatus = KRequestPending;
        TRequestStatus* s= &iStatus;
        User::RequestComplete( s, KErrNone );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::CreateXdmOMAListsIfNeededL
// Notice: separate CheckXdmOMAListsL method  (validity)
// ---------------------------------------------------------------------------
//
TBool CSimplePluginXdmUtils::CreateXdmOMAListsIfNeededL( )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: CreateXdmOMAListsIfNeededL"));
#endif

    using namespace NSimplePlugin::NSimpleOma;

    CXdmDocumentNode* root = iXdmDoc->DocumentRoot();
    if ( !root )
        {
        root = iXdmDoc->CreateRootL();
        root->SetNameL( KSimpleResourceLists );
        }

    if ( root->IsEmptyNode() )
        {
        root->SetEmptyNode( EFalse );
        }

    TInt isCreated = 0;

    // add all the list nodes

    isCreated += CreateXdmOMAListIfNeededL( root, iConnSets.Variation().SharedXdmTop() );

    isCreated += CreateXdmOMAListIfNeededL( root, KSimpleOMABlockedList );

    // grant-list name is variable issue
    isCreated += CreateXdmOMAListIfNeededL( root, iConnSets.Variation().GrantListName()  );

    if ( !isCreated )
        {
        return EFalse;
        }

    // send to the server and start wait a response
    iXdmDoc->AppendL( root );
    UpdateXdmsL();
    iXdmState = ECreateXdmOMALists;

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::CreateXdmOMAListIfNeededL
// ---------------------------------------------------------------------------
//
TBool CSimplePluginXdmUtils::CreateXdmOMAListIfNeededL(
    CXdmDocumentNode* aRoot, const TDesC& aName )
    {
    TBool isCreated( EFalse );
    CXdmDocumentNode* nodeFound = SearchListUnderParentL( aRoot, aName );
    if ( !nodeFound )
        {
        CreateResourceListL( aRoot, aName );
        isCreated = ETrue;
        }
    return isCreated;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::CreateResourceListL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::CreateResourceListL(
    CXdmDocumentNode* aParent, const TDesC& aName )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: CreateResourceListL"));
#endif
    using namespace NSimplePlugin::NSimpleOma;
    CXdmDocumentNode* child1 = aParent->CreateChileNodeL( KSimpleList );
    CXdmNodeAttribute* name = child1->CreateAttributeL( KSimpleName );
    name->SetAttributeValueL( aName );
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::SearchListUnderParentL
// ---------------------------------------------------------------------------
//
CXdmDocumentNode* CSimplePluginXdmUtils::SearchListUnderParentL(
    CXdmDocumentNode* aParent, const TDesC& aName )
    {
    using namespace NSimplePlugin::NSimpleOma;

    if ( !aParent )
        {
        return NULL;
        }

    RPointerArray<CXdmDocumentNode> resultArray;
    RPointerArray<SXdmAttribute16>  attributeArray;

    CleanupClosePushL( resultArray );           // <<< resultArray
    CleanupClosePushL( attributeArray );        // <<< attributeArray

    SXdmAttribute16 attr;
    attr.iName.Set( KSimpleName );
    attr.iValue.Set( aName );
    attributeArray.AppendL( &attr );

    CXdmDocumentNode* currNode = NULL;

    aParent->Find( KSimpleList, resultArray, attributeArray );

    TInt count = resultArray.Count();
    for ( TInt i=0; i < count; i++ )
        {
        currNode = resultArray[i];
        CXdmDocumentNode* parent = currNode->Parent();
        if ( parent == aParent )
            {
            break;
            }
        currNode = NULL;
        }

    CleanupStack::PopAndDestroy( &attributeArray); // >>> attributeArray
    CleanupStack::PopAndDestroy( &resultArray );   // >>> resultArray

    return currNode;

    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::MakeRlsUriL
// ---------------------------------------------------------------------------
//
HBufC* CSimplePluginXdmUtils::MakeRlsUriL(
    const TDesC& aXcapUri, const TDesC& aListName )
    {

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: MakeRlsUriL"));
#endif

    /* example:
        "http://XCAP.EXAMPLE.COM/resource-lists/users/
        sip:USER@EXAMPLE.COM/index/~~/resource-lists/list%5b@name=%22oma_blockedlist%22%5d"
        */

    using namespace NSimplePlugin::NSimpleRls;

    TInt myLen = totalSize;
    myLen += aXcapUri.Length();
    myLen += aListName.Length();
    myLen += iConnSets.CurrentSipPresentity().Length();
    HBufC* myBuf = HBufC::NewL( myLen );

    myBuf->Des().Append( KOne );
    myBuf->Des().Append( aXcapUri);
    myBuf->Des().Append( KThree );
    myBuf->Des().Append( iConnSets.CurrentSipPresentity() );
    myBuf->Des().Append( KFive );
    myBuf->Des().Append( aListName );
    myBuf->Des().Append( KSeven );

    return myBuf;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::MakeRlsBuddyUriL
// ---------------------------------------------------------------------------
//
HBufC* CSimplePluginXdmUtils::MakeRlsBuddyUriL(
    const TDesC& aXcapUri, const TDesC& aListName )
    {
    /* example:
        "http://XCAP.EXAMPLE.COM/resource-lists/users/
        sip:USER@EXAMPLE.COM/~~/resource-lists/list%5b@name=%22oma_buddylist%22%5d/
        list5d@name=%22my_friends%22%5d"
        */

    using namespace NSimplePlugin::NSimpleRlsBuddy;

    TInt myLen = totalSize + aXcapUri.Length() + aListName.Length() +
                 iConnSets.CurrentSipPresentity().Length();

    HBufC* myBuf = HBufC::NewL( myLen );
    TPtr temp = myBuf->Des();

    temp.Append( KOne );
    temp.Append( aXcapUri);
    temp.Append( KThree );
    temp.Append( iConnSets.CurrentSipPresentity() );
    temp.Append( KFive );
    temp.Append( aListName );
    temp.Append( KSeven );

    return myBuf;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::GetXdmRulesL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::GetXdmRulesL()
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: GetXdmRulesL"));
#endif
    if ( !iPresenceXdm )
        {
        // get data from a network if not done so yet
        iPresenceXdm = CPresenceXDM::NewL( iSettingsId );
        }
    User::LeaveIfError( iPresenceXdm->UpdateAllFromServerL( this ));
    iXdmState = EGetXdmRules;
    // The is completed in a callback method, not in RunL
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::MakeInitialXdmsDocumentL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::MakeInitialXdmsDocumentL()
    {

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: MakeInitialXdmsDocumentL"));
#endif
    TBuf<KSimplePluginMaxRuleLength> myRuleId;

    TBool ruleAdded( EFalse );

    TPresenceActionXDM action;
    action.Init( iPresenceXdm );

    TPresenceTransformXDM transform;
    transform.Init( iPresenceXdm );

    TPresCondMisc misc;
    misc.Init( iPresenceXdm );

    TPresCondIdentityOne one;
    one.Init( iPresenceXdm );

    HBufC* xcapUri = TXdmSettingsApi::PropertyL( iSettingsId, EXdmPropUri );
    CleanupStack::PushL( xcapUri );
    HBufC* listUri = MakeRlsUriL( xcapUri->Des(), KSimpleOMABlockedList );
    CleanupStack::PushL( listUri );

    // Default rule
    // get the rule id, use run-time variation
    myRuleId = iConnSets.Variation().DefaultRuleName(); // KSimpleXdmDefaultRule;
    if ( myRuleId.Length() && !iPresenceXdm->IsRuleExist( myRuleId ))
        {
        // If default rule does not exist in the variation then do not create it.
        MakeDefaultRuleL( myRuleId, action, transform, misc );
        ruleAdded = ETrue;
        }

    // Block rule
    // get the rule id, use run-time variation
    myRuleId = iConnSets.Variation().BlockRuleName();
    // ruleCreate = EFalse;

    if ( !iPresenceXdm->IsRuleExist( myRuleId ))
        {
        iPresenceXdm->CreateNewRuleL( myRuleId );
        ruleAdded = ETrue;

        // use run-time variation for rule action
        CSimplePluginVariation::TSimplePluginAuthRule authRule =
            iConnSets.Variation().BlockRuleAction();

        User::LeaveIfError( misc.AddExternListL( myRuleId, listUri->Des() ));

        switch (authRule)
            {
            case CSimplePluginVariation::ERulePoliteBlock:
                User::LeaveIfError( action.AddOrReplaceActionL(myRuleId, KPresPoliteBlock ));
                break;
            default:
                User::LeaveIfError( action.AddOrReplaceActionL(myRuleId, KPresBlock ));
            };
        }

    // Grant rule
    // get the rule id, use run-time variation
    myRuleId = iConnSets.Variation().GrantRuleName();
    if ( !iPresenceXdm->IsRuleExist( myRuleId ))
        {
        iPresenceXdm->CreateNewRuleL( myRuleId );
        ruleAdded = ETrue;
        CleanupStack::PopAndDestroy( listUri );
        listUri = NULL;
        listUri = MakeRlsUriL( xcapUri->Des(), iConnSets.Variation().GrantListName() );
        CleanupStack::PushL( listUri );
        User::LeaveIfError( misc.AddExternListL( myRuleId, listUri->Des() ));
        User::LeaveIfError( action.AddOrReplaceActionL( myRuleId, KPresAllow ));
        User::LeaveIfError( transform.AddOrRepProvideAllTransComplexL(
            myRuleId, KPresProvideAllAttributes ));
        User::LeaveIfError( transform.AddOrRepProvideAllTransComplexL(
            myRuleId, KPresProvidePersons ));
        User::LeaveIfError( transform.AddOrRepProvideAllTransComplexL(
            myRuleId, KPresProvideDevices ));
        User::LeaveIfError( transform.AddOrRepProvideAllTransComplexL(
            myRuleId, KPresProvideServices ));
        }

    // Grant own subscription rule
    // get the rule id, use run-time variation
    myRuleId = iConnSets.Variation().GrantOwnRuleName();
    if ( !iPresenceXdm->IsRuleExist( myRuleId ))
        {
        iPresenceXdm->CreateNewRuleL( myRuleId );
        ruleAdded = ETrue;
        User::LeaveIfError( action.AddOrReplaceActionL( myRuleId, KPresAllow ));
        User::LeaveIfError( transform.AddOrRepProvideAllTransComplexL(
            myRuleId, KPresProvideAllAttributes ));
        User::LeaveIfError( transform.AddOrRepProvideAllTransComplexL(
            myRuleId, KPresProvidePersons ));
        User::LeaveIfError( transform.AddOrRepProvideAllTransComplexL(
            myRuleId, KPresProvideDevices ));
        User::LeaveIfError( transform.AddOrRepProvideAllTransComplexL(
            myRuleId, KPresProvideServices ));
        // own sip identity
        User::LeaveIfError( one.AddIdentityL(
            myRuleId, iConnSets.CurrentSipPresentity() ));
        }

    // Update into server only when needed.
    if ( ruleAdded )
        {
        UpdateXdmRulesL();
        }
    else
        {
        iXdmState = EUpdateXdmRules;
        iPresXdmOk = ETrue;
        // Complete with ok or error the last initial opreration
        CompleteClientReq( KErrNone );
        }

    CleanupStack::PopAndDestroy( listUri );
    CleanupStack::PopAndDestroy( xcapUri );
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoGrantForAllL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::DoGrantForAllL()
    {
    const TInt KMyPresRuleIDLength(30);
    TBuf<KMyPresRuleIDLength> myRuleId;

    TInt err( KErrNone );

    TPresenceActionXDM action;
    action.Init( iPresenceXdm );

    // Default rule id is searched
    myRuleId = iConnSets.Variation().DefaultRuleName(); // KSimpleXdmDefaultRule;
    TRAP( err, iPresenceXdm->CreateNewRuleL( myRuleId ) );
    if ( err && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }

    // modify the rule in the server.
    User::LeaveIfError( action.AddOrReplaceActionL( myRuleId, KPresAllow ));
    UpdateXdmRulesL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoWithdrawFromAllL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::DoWithdrawFromAllL()
    {
    const TInt KMyPresRuleIDLength(30);
    TBuf<KMyPresRuleIDLength> myRuleId;

    /* pseudo code:
        if current rule is ALLOW then {  BLOCK  }
        otherwise no action.
    */

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginWatcher: DoWithdrawFromAllL TEST COVERS 3"));
#endif


    TInt err( KErrNone );

    TPresenceActionXDM action;
    action.Init( iPresenceXdm );

    // Default rule id is searched
    myRuleId = iConnSets.Variation().DefaultRuleName();
    TRAP( err, iPresenceXdm->CreateNewRuleL( myRuleId ) );
    if ( err && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }

    TBuf<KMyPresRuleIDLength> myActionBuf;
    err = action.GetActionL( myRuleId, myActionBuf);

    if ( !myActionBuf.CompareF( KPresAllow )  )
        {
        User::LeaveIfError( action.AddOrReplaceActionL( myRuleId, KPresPoliteBlock ));
        UpdateXdmRulesL();
        *iClientStatus = KRequestPending;
        }
    else
        {
        CompleteClientReq( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoSetReactiveAuthL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::DoSetReactiveAuthL()
    {
#ifdef _DEBUG
    PluginLogger::Log(
            _L("PluginWatcher: DoSetReactiveAuthL TEST COVERS 5"));
#endif

    // Default rule is modified to CONFIRM

    const TInt KMyPresRuleIDLength(30);
    TBuf<KMyPresRuleIDLength> myRuleId;

    TInt err( KErrNone );

    TPresenceActionXDM action;
    action.Init( iPresenceXdm );

    // Default rule id is searched
    myRuleId = iConnSets.Variation().DefaultRuleName();
    TRAP( err, iPresenceXdm->CreateNewRuleL( myRuleId ) );
    if ( err && err != KErrAlreadyExists )
        {
#ifdef _DEBUG
        PluginLogger::Log( _L("PluginWatcher: TEST COVERS 5b"));
#endif
        User::Leave( err );
        }

    // modify the rule in the server.
    User::LeaveIfError( action.AddOrReplaceActionL( myRuleId, KPresConfirm ));
    UpdateXdmRulesL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoSetProactiveAuthL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::DoSetProactiveAuthL()
    {
#ifdef _DEBUG
    PluginLogger::Log(
            _L("PluginWatcher: DoSetProactiveAuthL TEST COVERS 6"));
#endif

    /* pseudo code:
        if current rule is CONFIRM then {  ALLOW  }
        otherwise no action.
    */

    const TInt KMyPresRuleIDLength(30);
    TBuf<KMyPresRuleIDLength> myRuleId;

    TInt err( KErrNone );

    TPresenceActionXDM action;
    action.Init( iPresenceXdm );

    // Default rule id is searched
    myRuleId = iConnSets.Variation().DefaultRuleName();
    TRAP( err, iPresenceXdm->CreateNewRuleL( myRuleId ) );
    if ( err && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }

    // modify the rule in the server.
    TBuf<KMyPresRuleIDLength> myActionBuf;
    err = action.GetActionL( myRuleId, myActionBuf);

    if ( !myActionBuf.CompareF( KPresConfirm )  )
        {
        User::LeaveIfError( action.AddOrReplaceActionL( myRuleId, KPresAllow ));
        UpdateXdmRulesL();
        *iClientStatus = KRequestPending;
        }
    else
        {
        CompleteClientReq( KErrNone );
        }

    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::CreateEntityGroupL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::CreateEntityGroupL(
    const TDesC&  aName, const TDesC&  aDisplayName )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: CreateEntityGroupL"));
#endif
    iOperation = EXdmCreateGroup;
    DoCreateEntityGroupL( aName, aDisplayName );
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DeleteEntityGroupL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::DeleteEntityGroupL(
    const TDesC&  aName)
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: DeleteEntityGroupL"));
#endif

    iOperation = EXdmDeleteGroup;
    DoDeleteEntityGroupL( aName );
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::AddPresentityGroupMemberL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::AddPresentityGroupMemberL(
    const TDesC&  aGroup,
    const TDesC&  aUser,
    const TDesC&  aDispName,
    TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: AddPresentityGroupMemberL"));
#endif

    iClientStatus = &aStatus;

    iOperation = EXdmAddUserToGroup;
    DoAddUserToUserListL( aGroup, aUser, aDispName );

    // send to the server and start wait a response
    UpdateXdmsL();

    *iClientStatus = KRequestPending;
    iXdmState = EUpdateXdmOMAList;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::RemovePresentityGroupMemberL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::RemovePresentityGroupMemberL(
    const TDesC&  aGroup,
    const TDesC&  aUser,
    TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: RemovePresentityGroupMemberL"));
#endif

    iClientStatus = &aStatus;

    iOperation = EXdmAddUserToGroup;
    DoRemoveUserFromUserListL( aGroup, aUser );

    // send to the server and start wait a response
    UpdateXdmsL();

    *iClientStatus = KRequestPending;
    iXdmState = EUpdateXdmOMAList;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::GetUserListsL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::GetUserListsL(
    CDesCArrayFlat& aIds, CDesCArrayFlat& aDispNames )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: GetUserListsL"));
#endif
    using namespace NSimplePlugin::NSimpleOma;

    aIds.Reset();
    aDispNames.Reset();

    CXdmDocumentNode* buddylist = DoGetBuddyListL();
    CXdmDocumentNode* entry = NULL;

    RPointerArray<CXdmDocumentNode> lists;
    CleanupClosePushL( lists );         // <<< lists

    RPointerArray<CXdmDocumentNode> lists2;
    CleanupClosePushL( lists2 );        // <<< lists2

    buddylist->Find( KSimpleList, lists );
    TInt count = lists.Count();
    for ( TInt i=0; i < count; i++ )
        {
        // Name
        entry = lists[i];
        CXdmNodeAttribute* attr = entry->Attribute( KSimpleName );
        if ( attr )
            {
            aIds.AppendL( attr->AttributeValue() );
#ifdef _DEBUG
            TBuf<200> debug_buffer;
            debug_buffer = attr->AttributeValue();
            PluginLogger::Log(_L("PluginXdm: add group id into aIds : %S"), &debug_buffer );
#endif
            }
        else
            {
            continue;
            }
        // Display name
        TInt err = entry->Find( KSimpleDisplayName, lists2 );
        // Notice: assume there is only one display-name.
        // Always when an item is added into aIds we have to add an itmem also into aDispNames.
        if ( !err )
            {
            CXdmDocumentNode* disName = lists2[0];
            if ( !disName->IsEmptyNode() )
                {
                // Convert Display-Name to Unicode.
                HBufC16* uri16 = NULL;
                uri16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L( disName->LeafNodeContent() );
                CleanupStack::PushL( uri16 );  // << uri16
                aDispNames.AppendL( uri16->Des() );
                CleanupStack::PopAndDestroy( uri16 );  // >> uri16
                }
                else
                    {
                    aDispNames.AppendL( KNullDesC );
                    }
            }
            else
                {
                aDispNames.AppendL( KNullDesC );
                }
        }

    CleanupStack::PopAndDestroy( &lists2 ); // >>> lists2
    CleanupStack::PopAndDestroy( &lists ); // >>> lists
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::GetEntitiesInListL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::GetEntitiesInListL(
    const TDesC&  aList, CPtrCArray& aIds, CPtrCArray& aDispNames )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: GetEntitiesInListL"));
#endif

    using namespace NSimplePlugin::NSimpleOma;

    CXdmDocumentNode* buddylist = DoGetBuddyListL();
    CXdmDocumentNode* target = DoGetUserListL( aList, buddylist );

    // make a collection of MPresentityGroupMemberInfo
    RPointerArray<CXdmDocumentNode> entries;
    CleanupClosePushL(entries);       // <<< entries

    RPointerArray<CXdmDocumentNode> nodes;
    CleanupClosePushL(nodes);         // <<< nodes

    target->Find( KSimpleEntry, entries );
    TInt count = entries.Count();
    for ( TInt i=0; i < count; i++ )
        {
        // ID
        CXdmDocumentNode* entry = entries[i];
        CXdmNodeAttribute* attr = entry->Attribute( KSimpleUri );
        if ( attr )
            {
            aIds.AppendL( attr->AttributeValue() );
#ifdef _DEBUG
            TBuf<200> debug_buffer;
            debug_buffer = attr->AttributeValue();
            PluginLogger::Log(_L("PluginXdm: add member into collection : %S"), &debug_buffer );
#endif
            }
        else
            {
            continue;
            }

        // Display name
        TInt err = entry->Find( KSimpleDisplayName, nodes );
        // Notice: assume there is only one display-name
        if ( !err )
            {
            CXdmDocumentNode* disName = nodes[0];
            HBufC16* uri16 = NULL;
            uri16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L( disName->LeafNodeContent() );
            CleanupStack::PushL( uri16 );  // << uri16
            aDispNames.AppendL( uri16->Des() );
            CleanupStack::PopAndDestroy( uri16 );  // >> uri16
            }
        else
            {
            aDispNames.AppendL( TPtrC() );
            }
        }

    CleanupStack::PopAndDestroy( &nodes ); // >>> nodes
    CleanupStack::PopAndDestroy( &entries ); // >>> entries
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::CommitXdmL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::CommitXdmL( TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: CommitXdmL"));
#endif
    iClientStatus = &aStatus;

    UpdateXdmsL();

    *iClientStatus = KRequestPending;
    iXdmState = EUpdateXdmOMAList;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::CommitRlsL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::CommitRlsL( TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: CommitRlsL"));
#endif
    iClientStatus = &aStatus;
    *iClientStatus = KRequestPending;
    UpdateRlsL();

    iXdmState = EUpdateRlsServices;
    }

// ----------------------------------------------------
// CSimplePluginXdmUtils::DoCreateEntityGroupL
// ----------------------------------------------------
//
void CSimplePluginXdmUtils::DoCreateEntityGroupL(
    const TDesC& aList, const TDesC&  aDisName )
    {
    using namespace NSimplePlugin::NSimpleOma;

    CXdmDocumentNode* buddylist = DoGetBuddyListL();
    if ( buddylist->IsEmptyNode() )
        {
        buddylist->SetEmptyNode( EFalse );
        }

    CXdmDocumentNode* foundNode = SearchListUnderParentL(
        buddylist, aDisName );

    if ( foundNode )
        {
        User::Leave( KErrAlreadyExists );
        }

    CXdmDocumentNode* newNode = buddylist->CreateChileNodeL( KSimpleList );
    CXdmNodeAttribute* attributeName = newNode->CreateAttributeL( KSimpleName );
    attributeName->SetAttributeValueL( aList );

    // Display name
    CXdmDocumentNode* displayName = newNode->CreateChileNodeL( KSimpleDisplayName );
    displayName->SetLeafNode( ETrue );
    displayName->SetLeafNodeContentL( aDisName );

    iXdmDoc->AppendL( newNode );
    }

// ----------------------------------------------------
// CSimplePluginXdmUtils::DoDeleteEntityGroupL
// ----------------------------------------------------
//
void CSimplePluginXdmUtils::DoDeleteEntityGroupL(
    const TDesC& aList )
    {
    CXdmDocumentNode* buddylist = DoGetBuddyListL();
    CXdmDocumentNode* target = DoGetUserListL( aList, buddylist );
    iXdmDoc->DeleteDataL( target );
    // iXdmEngine is asked to be updated to a server by CSimplePlugingGroups later.
    }

// ----------------------------------------------------
// CSimplePluginXdmUtils::DoAddUserToUserListL
// ----------------------------------------------------
//
void CSimplePluginXdmUtils::DoAddUserToUserListL(
    const TDesC& aList, const TDesC&  aUser, const TDesC&  aDisName )
    {
    RPointerArray<CXdmDocumentNode> resultArray;
    RPointerArray<SXdmAttribute16>  attributeArray;

    CleanupClosePushL( resultArray );           // << resultArray
    CleanupClosePushL( attributeArray );        // << attributeArray

    CXdmDocumentNode* buddylist = DoGetBuddyListL();
    CXdmDocumentNode* target = DoGetUserListL( aList, buddylist );

    if ( target->IsEmptyNode() )
        {
        target->SetEmptyNode( EFalse );
        }

    using namespace NSimplePlugin::NSimpleOma;

    SXdmAttribute16 attr;
    attr.iName.Set( KSimpleUri );
    attr.iValue.Set( aUser );
    attributeArray.AppendL( &attr );

    // If member already exists then leave
    target->Find( KSimpleEntry, resultArray, attributeArray );
    if ( resultArray.Count() > 0 )
        {
        User::Leave( KErrAlreadyExists );
        }

    CXdmDocumentNode* newNode = target->CreateChileNodeL( KSimpleEntry );
    CXdmNodeAttribute* attributeName = newNode->CreateAttributeL( KSimpleUri );
    attributeName->SetAttributeValueL( aUser );

    // Display name
    CXdmDocumentNode* displayName = newNode->CreateChileNodeL( KSimpleDisplayName );
    displayName->SetLeafNode( ETrue );
    displayName->SetLeafNodeContentL( aDisName );

    iXdmDoc->AppendL( newNode );

    CleanupStack::PopAndDestroy( &attributeArray );
    CleanupStack::PopAndDestroy( &resultArray );
    }

// ----------------------------------------------------
// CSimplePluginXdmUtils::DoRemoveUserFromUserListL
// ----------------------------------------------------
//
void CSimplePluginXdmUtils::DoRemoveUserFromUserListL(
    const TDesC& aList, const TDesC&  aUri )
    {
    CXdmNodeAttribute* attr = NULL;
    TBool myFound( EFalse );

    using namespace NSimplePlugin::NSimpleOma;

    CXdmDocumentNode* buddylist = DoGetBuddyListL();
    CXdmDocumentNode* target = DoGetUserListL( aList, buddylist );

    RPointerArray<CXdmDocumentNode> entries;
    CleanupClosePushL( entries );           // <<< entries

    User::LeaveIfError( target->Find( KSimpleEntry, entries ));
    TInt nodeCount = entries.Count();
    for ( TInt i = 0; i < nodeCount; i++ )
        {
        CXdmDocumentNode* currNode = entries[i];
        attr = currNode->Attribute( KSimpleUri );
        if ( attr && !attr->AttributeValue().CompareF( aUri ))
            {
            // This is the user we are looking for deletion.
            iXdmDoc->DeleteDataL( currNode );
            myFound = ETrue;
            break;
            }
        }

    CleanupStack::PopAndDestroy( &entries ); // >>> entries

    if ( !myFound )
        {
        // If the member is not found then leave
        User::Leave( KErrNotFound );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoGetBuddyListL
// ---------------------------------------------------------------------------
//
CXdmDocumentNode* CSimplePluginXdmUtils::DoGetBuddyListL()
    {
    CXdmDocumentNode* root = iXdmDoc->DocumentRoot();
    CXdmDocumentNode* ret = SearchListUnderParentL( root, iConnSets.Variation().SharedXdmTop() );
    if ( !ret )
        {
        User::Leave( KErrNotFound );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoGetUserListL
// ---------------------------------------------------------------------------
//
CXdmDocumentNode* CSimplePluginXdmUtils::DoGetUserListL(
    const TDesC& aList, CXdmDocumentNode* aBuddyList )
    {
    CXdmDocumentNode* listNode = NULL;
    CXdmNodeAttribute* attr = NULL;
    TBool found( EFalse );

    using namespace NSimplePlugin::NSimpleOma;

    RPointerArray<CXdmDocumentNode> lists;
    CleanupClosePushL( lists );

    User::LeaveIfError( aBuddyList->Find( KSimpleList, lists ));
    TInt count = lists.Count();
    for ( TInt i=0; i < count; i++ )
        {
        listNode = lists[i];
        attr = listNode->Attribute( KSimpleName );
        if ( attr && !attr->AttributeValue().CompareF( aList ))
            {
            // List is found
            found = ETrue;
            break;
            }
        }
    if ( !found )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( &lists );
    return listNode;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::UpdateXdmRulesL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::UpdateXdmRulesL()
    {
    _LIT( KMyOper, "UpdateXdmRulesL");
    if ( iTestObs )
        {
        // Test suite
        iTestObs->TestCallbackAsynchL(
            KErrNone, iXdmState, iOperation, KMyOper );
        }

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: UpdateXdmRulesL -> iPresenceXdm->UpdateToServerL"));
#endif
    User::LeaveIfError( iPresenceXdm->UpdateToServerL( this ));
    iXdmState = EUpdateXdmRules;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::UpdateXdmsL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::UpdateXdmsL()
    {
    _LIT( KMyOper, "UpdateXdmsL");
    if ( iTestObs )
        {
        // Test suite
        iTestObs->TestCallbackAsynchL(
            KErrNone, iXdmState, iOperation, KMyOper );
        }

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: UpdateXdmsL -> iXdmEngine->UpdateToServerL"));
#endif
    iXdmEngine->UpdateL( iXdmDoc, iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::UpdateRlsL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::UpdateRlsL()
    {
    _LIT( KMyOper, "UpdateRlsL");

    if ( iTestObs )
        {
        // Test suite
        iTestObs->TestCallbackAsynchL(
            KErrNone, iXdmState, iOperation, KMyOper );
        }
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: -> iRlsXdm->UpdateToServerL"));
#endif
    User::LeaveIfError( iRlsXdm->UpdateToServerL( this ));
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::SetTestObserver
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::SetTestObserver( MSimplePluginTestObs* aObs )
    {
    iTestObs = aObs;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::CreateRlsDocumentUri16LC
// ---------------------------------------------------------------------------
//
HBufC16* CSimplePluginXdmUtils::CreateRlsDocumentUri16LC(
    const TDesC16& aGroup,
    const TDesC16& aPresentityUri,
    const TDesC16& aDomain )
    {

    TPtrC uriTemplate( iConnSets.Variation().RlsGroupUriTemplate() );

    // Calculate URI length
    TInt uriSize = uriTemplate.Length() + KMyLenPrefix;
    if ( uriTemplate.Find( KSimpleGroupTag ) != KErrNotFound )
        {
        TPtrC p8( KSimpleGroupTag );
        uriSize = uriSize - p8.Length() + aGroup.Length();
        }
    if ( uriTemplate.Find( KSimpleUserTag ) != KErrNotFound )
        {
        TPtrC p8( KSimpleUserTag );
        uriSize = uriSize - p8.Length() + aPresentityUri.Length();
        }
    if ( uriTemplate.Find( KSimpleDomainTag ) != KErrNotFound )
        {
        TPtrC p8( KSimpleDomainTag );
        uriSize = uriSize - p8.Length() + aDomain.Length();
        }

    // allocate new buffer for URI
    // Temporarily the buffer might need more space then the final URI length
    // (when tags are longer than the real values replacing them).
    if ( uriSize < uriTemplate.Length() + KMyLenPrefix )
        {
        uriSize = uriTemplate.Length() + KMyLenPrefix +
                  aGroup.Length() + aPresentityUri.Length() + aDomain.Length();
        }
    HBufC16* buf = HBufC16::NewLC( uriSize );
    TPtr16 pBuf(buf->Des());

    // Append "sip:" prefix
    pBuf.Append( KMySip );

    // Append template
    pBuf.Append( uriTemplate );

    // Replace tags in URI template with the real values.
    ReplaceGroupTag( buf, aGroup );
    ReplaceDomainTag( buf, aDomain );
    ReplaceUserTag( buf, aPresentityUri );

    return buf;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoesUserListExistsL
// ---------------------------------------------------------------------------
//
TBool CSimplePluginXdmUtils::DoesUserListExistsL( const TDesC& aName )
  {
    CXdmDocumentNode* buddylist = DoGetBuddyListL();
    CXdmDocumentNode* foundNode = SearchListUnderParentL( buddylist, aName );
    return foundNode ? ETrue : EFalse;
  }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::AuxBuffer
// ---------------------------------------------------------------------------
//
TPtrC CSimplePluginXdmUtils::AuxBuffer( )
    {
    return iAuxBuffer ? iAuxBuffer->Des() : TPtrC();
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::RemoveRlsServiceByResourceListL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::RemoveRlsServiceByResourceListL(
    const TDesC& aGroup, TRequestStatus& aStatus )
    {

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: RemoveRlsServiceByResourceListL"));
#endif

    TBool ret( EFalse );

    iOperation = EXdmRlsRemove;
    iClientStatus = &aStatus;

    // convert a Group into resource-list value
    HBufC* xcapUri = TXdmSettingsApi::PropertyL( iSettingsId, EXdmPropUri );
    CleanupStack::PushL( xcapUri );             // << xcapUri

    HBufC* listUri = MakeRlsBuddyUriL( xcapUri->Des(), aGroup );
    CleanupStack::PushL( listUri );             // << listUri

    // remove service-uri in the rls-services document
    ret = DoRemoveRlsServiceByResourceListL( listUri->Des() );

    CleanupStack::PopAndDestroy( listUri );
    CleanupStack::PopAndDestroy( xcapUri );

    *iClientStatus = KRequestPending;

    if ( ret )
        {
        // Update RLS document when needed.
        UpdateRlsL();
        iXdmState = EUpdateRlsServices;
        }
    else
        {
        CompleteClientReq( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoRemoveRlsServiceByResourceListL
// ---------------------------------------------------------------------------
//
TBool CSimplePluginXdmUtils::DoRemoveRlsServiceByResourceListL( const TDesC& aResourceList )
    {
    TBool ret( EFalse );

    CDesC16Array* myArray = new (ELeave) CDesC16ArraySeg(10);
    CleanupStack::PushL( myArray );

    CDesC16Array* myValues = new (ELeave) CDesC16ArraySeg(10);
    CleanupStack::PushL( myValues );

    iRlsXdm->GetServiceUrisL( *myArray );
    TInt myCount = myArray->MdcaCount();

    for ( TInt i=0; i < myCount && !ret; i++ )
        {
        iRlsXdm->SwitchToServiceURIL( myArray->MdcaPoint( i ));
        iRlsXdm->GetElementsL( KPresResourceList, *myValues);
        TInt myValuesCount = myValues->MdcaCount();
        for ( TInt j=0; j < myValuesCount; j++ )
            {
            if ( !myValues->MdcaPoint( i ).Compare( aResourceList ) )
                {
                // delete service with the URI
                iRlsXdm->RemoveServiceURIL( myArray->MdcaPoint( i ) );
                ret = ETrue;
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy( myValues );
    CleanupStack::PopAndDestroy( myArray );

    return ret;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::RlsServiceByResourceListL
// ---------------------------------------------------------------------------
//
HBufC* CSimplePluginXdmUtils::RlsServiceByResourceListLC(
    const TDesC& aGroup )
    {

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: :RlsServiceByResourceListLC"));
#endif

    HBufC* myRet = NULL;

    // convert a Group into resource-list value
    HBufC* xcapUri = TXdmSettingsApi::PropertyL( iSettingsId, EXdmPropUri );
    CleanupStack::PushL( xcapUri );             // << xcapUri

    HBufC* listUri = MakeRlsBuddyUriL( xcapUri->Des(), aGroup );
    CleanupStack::PushL( listUri );             // << listUri

    // remove service-uri in the rls-services document
    myRet = DoGetRlsServiceByResourceListL( listUri->Des() );

    CleanupStack::PopAndDestroy( listUri );
    CleanupStack::PopAndDestroy( xcapUri );

    CleanupStack::PushL( myRet );

    return myRet;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::DoGetRlsServiceByResourceListL
// ---------------------------------------------------------------------------
//
HBufC* CSimplePluginXdmUtils::DoGetRlsServiceByResourceListL( const TDesC& aResourceList )
    {

    HBufC* ret = NULL;

    CDesC16Array* myArray = new (ELeave) CDesC16ArraySeg(10);
    CleanupStack::PushL( myArray );

    CDesC16Array* myValues = new (ELeave) CDesC16ArraySeg(10);
    CleanupStack::PushL( myValues );

    iRlsXdm->GetServiceUrisL( *myArray );
    TInt myCount = myArray->MdcaCount();

    for ( TInt i=0; i < myCount && !ret; i++ )
        {
        iRlsXdm->SwitchToServiceURIL( myArray->MdcaPoint( i ));
        iRlsXdm->GetElementsL( KPresResourceList, *myValues);
        TInt myValuesCount = myValues->MdcaCount();
        for ( TInt j=0; j < myValuesCount; j++ )
            {
            if ( !myValues->MdcaPoint( i ).Compare( aResourceList ) )
                {
                // save the URI
                ret = myArray->MdcaPoint( i ).AllocL();
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy( myValues );
    CleanupStack::PopAndDestroy( myArray );

    if ( !ret )
        {
        User::Leave( KErrNotFound );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::RlsServiceUriNegotiationL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::RlsServiceUriNegotiationL()
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: :RlsServiceUriNegotiationL"));
#endif
    // Handle RLS Service URI negotiation

    HBufC16* buf = CreateRlsDocumentUri16LC(
        AuxBuffer(), iConnSets.CurrentSipPresentity(), iConnSets.CurrentDomain() );

    TRAP_IGNORE( iRlsXdm->RemoveServiceURIL( buf->Des() ));

    TInt errx = iRlsXdm->AddServiceURIL( iRlsXdm->NegotiatedServiceUri() );
    if ( errx && errx != KErrAlreadyExists )
        {
        User::Leave( errx );
        }
    User::LeaveIfError( iRlsXdm->SwitchToServiceURIL( iRlsXdm->NegotiatedServiceUri() ));

    HBufC* xcapUri = TXdmSettingsApi::PropertyL( iSettingsId, EXdmPropUri );
        CleanupStack::PushL( xcapUri );             // << xcapUri

    HBufC* listUri = MakeRlsBuddyUriL( xcapUri->Des(), AuxBuffer() );
    CleanupStack::PushL( listUri );             // << listUri

    TInt err = iRlsXdm->AddElementL( KPresResourceList, listUri->Des(), AuxBuffer() );
    if ( err && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }

    CleanupStack::PopAndDestroy( listUri );     // >> listUri
    CleanupStack::PopAndDestroy( xcapUri );     // >> xcapUri
    CleanupStack::PopAndDestroy( buf );

    delete iAuxBuffer;
    iAuxBuffer = NULL;
    iAuxBuffer = iRlsXdm->NegotiatedServiceUri().AllocL();

    UpdateRlsL();
    iXdmState = EReUpdateRlsServices;
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::MakeDefaultRuleL
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::MakeDefaultRuleL(
    TDes& aRuleId,
    TPresenceActionXDM& aAction,
    TPresenceTransformXDM& aTransform,
    TPresCondMisc& aMisc )
    {

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginXdm: :MakeDefaultRuleL"));
#endif

    iPresenceXdm->CreateNewRuleL( aRuleId );
    User::LeaveIfError( aMisc.AddOtherIdentityL(aRuleId) );

    // use run-time variation for rule action
    CSimplePluginVariation::TSimplePluginAuthRule authRule =
        iConnSets.Variation().DefaultRuleAction();

    switch (authRule)
        {
        case CSimplePluginVariation::ERuleConfirm:
            User::LeaveIfError( aAction.AddOrReplaceActionL(aRuleId, KPresConfirm ));
            break;
        case CSimplePluginVariation::ERulePoliteBlock:
            User::LeaveIfError( aAction.AddOrReplaceActionL(aRuleId, KPresPoliteBlock ));
            break;
        case CSimplePluginVariation::ERuleBlock:
            User::LeaveIfError( aAction.AddOrReplaceActionL(aRuleId, KPresBlock ));
            break;
        default:
            User::LeaveIfError( aAction.AddOrReplaceActionL(aRuleId, KPresAllow ));
        };
    User::LeaveIfError( aTransform.AddOrRepProvideAllTransComplexL(
        aRuleId, KPresProvideAllAttributes ));
    User::LeaveIfError( aTransform.AddOrRepProvideAllTransComplexL(
        aRuleId, KPresProvidePersons ));
    User::LeaveIfError( aTransform.AddOrRepProvideAllTransComplexL(
        aRuleId, KPresProvideDevices ));
    User::LeaveIfError( aTransform.AddOrRepProvideAllTransComplexL(
        aRuleId, KPresProvideServices ));
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::ReplaceGroupTag
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::ReplaceGroupTag( HBufC16* aBuf, const TDesC16& aGroup )
    {
    // Replace group if found
    TPtr pBuf( aBuf->Des() );
    TInt pos = pBuf.Find( KSimpleGroupTag );
    if ( pos != KErrNotFound )
        {
        TPtrC p8( KSimpleGroupTag );
        pBuf.Replace( pos, p8.Length(), aGroup );
        }
    else
        {
        // No tag
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::ReplaceDomainTag
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::ReplaceDomainTag( HBufC16* aBuf, const TDesC16& aDomain )
    {
    // Replace domain if found
    TPtr pBuf( aBuf->Des() );
    TInt pos = pBuf.Find( KSimpleDomainTag );
    if ( pos != KErrNotFound )
        {
        // remove leading "@"
        TPtrC p8( KSimpleDomainTag );
        if ( aDomain[0] == '@')
            {
            pBuf.Replace( pos, p8.Length(), aDomain.Mid( 1 ));
            }
        else
            {
            pBuf.Replace( pos, p8.Length(), aDomain );
            }
        }
    else
        {
        // No tag
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::ReplaceUserTag
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::ReplaceUserTag(
    HBufC16* aBuf, const TDesC16& aPresentityUri )
    {
    TPtr pBuf( aBuf->Des() );
    TInt presentityUriLen = aPresentityUri.Length();
    // Replace user if found
    TInt pos = pBuf.Find( KSimpleUserTag );
    if ( pos != KErrNotFound )
        {
        TPtrC p8( KSimpleUserTag );

        // Remove leading "sip:" or "tel:" prefix and strip off domain part from aPresentityUri.
        TInt domainPos = presentityUriLen;
        TInt userIdPos = 0;

        if ( !aPresentityUri.Left( KMyLenPrefix ).CompareF( KMySip ) ||
             !aPresentityUri.Left( KMyLenPrefix ).CompareF( KMyTel ))
            {
            userIdPos = KMyLenPrefix;
            }

        for ( TInt i = userIdPos; i < presentityUriLen; i++ )
            {
            if ( aPresentityUri[i] == '@' )
                {
                domainPos = i;
                break;
                }
            }

        TPtrC userId( aPresentityUri.Mid( userIdPos, domainPos - userIdPos ));
        pBuf.Replace( pos, p8.Length(), userId );
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginXdmUtils::CompleteClientReq
// ---------------------------------------------------------------------------
//
void CSimplePluginXdmUtils::CompleteClientReq( TInt aStatus )
    {
    iXdmState = EStateIdle;
    iOperation = ENoOperation;
    TRequestStatus* s = iClientStatus;
    User::RequestComplete( s, aStatus );
    }


// End of file

