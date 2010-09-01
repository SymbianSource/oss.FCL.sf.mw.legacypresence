/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include <protocolpresenceauthorizationdatahost.h>
#include <ximpobjectfactory.h>
#include <presenceobjectfactory.h>
#include <ximpstatus.h>
#include <ximpidentity.h>
#include <presencegrantrequestinfo.h>
#include <ximpobjectcollection.h>
#include <presenceblockinfo.h>
#include <presenceerrors.hrh>

#include <simplefactory.h>
#include <msimpleconnection.h>
#include <msimplewinfo.h>
#include <msimpleelement.h>

#include "simplepluginauthorization.h"
#include "simpleplugindebugutils.h"
#include "simplepluginwinfo.h"
#include "simplepluginxdmutils.h"
#include "simpleplugincommon.h"
#include "simplepluginconnection.h"

/**
    The design is the following one

    1. DoGrantPresenceForEveryOneL
      modify default rule to ALLOW.

    2. DoWithdrawPresenceFromEveryOneL
      if current rule is ALLOW then
        {  BLOCK  }
        otherwise no action.

    3. DoSubscribePresenceGrantRequestListL
      modify default rule to CONFIRM.

    4. DoUnsubscribePresenceGrantRequestListL
      if current rule is CONFIRM then
        { go to ALLOW  }
        otherwise no action.
*/


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::CSimplePluginAuthorization
// ---------------------------------------------------------------------------
//
CSimplePluginAuthorization::CSimplePluginAuthorization(
    MSimplePluginConnectionObs& aObs )
: CActive( CActive::EPriorityStandard ),
  iConnObs(aObs),
  iOperation(ENoOperation),
  iXdmOk(EFalse), iDefRule(ERuleDef), iWinfoSubs( EFalse )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::ConstructL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::ConstructL( )
    {

    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::NewL
// ---------------------------------------------------------------------------
//
CSimplePluginAuthorization* CSimplePluginAuthorization::NewL(
    MSimplePluginConnectionObs& aObs )
    {
    CSimplePluginAuthorization* self =
        new( ELeave ) CSimplePluginAuthorization( aObs );
    CleanupStack::PushL( self );
    self->ConstructL(  );
    CleanupStack::Pop( self );
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: NewL this=%d"), (TInt)self );
#endif
    return self;
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::~CSimplePluginAuthorization
// ---------------------------------------------------------------------------
//
CSimplePluginAuthorization::~CSimplePluginAuthorization()
    {
    delete iPresIdentity;
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::SetDataHost
// We assume that this method is given by PrFw right after
// constrcution.
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::SetDataHost(
    MProtocolPresenceAuthorizationDataHost& aDataHost )
    {
    iDataHost = &aDataHost;
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::WinfoTerminatedL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::WinfoTerminatedL( TInt /*aReason*/ )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: WinfoTerminatedL") );
#endif

    if ( iWinfoSubs )
        {
        MXIMPDataSubscriptionState* myState = iConnObs.ObjectFactory().NewDataSubscriptionStateLC();
        MXIMPStatus* myStatus = iConnObs.ObjectFactory().NewStatusLC();
        // Notice: consider error codes
        myStatus->SetResultCode( KErrCompletion );
        myState->SetSubscriptionStateL(
            MXIMPDataSubscriptionState::ESubscriptionInactive );

        // parameters' OWNERSHIP is taken
        iDataHost->SetPresenceGrantRequestDataSubscriptionStateL(
            myState,
            myStatus );

        iWinfoSubs = EFalse;

        CleanupStack::Pop( /*myStatus*/ );
        CleanupStack::Pop( /*myState*/ );
        }
 }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoSubscribePresenceGrantRequestListL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoSubscribePresenceGrantRequestListL(
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoSubscribePresenceGrantRequestListL"));
#endif

    /*
    This means Reactive authorization, i.e. modify default rule as CONFIRM.
    Also this starts the WINFO subscription.

    Once the authorization request is received the
    MProtocolPresenceAuthorizationDataHost::HandlePresenceGrantRequestReceivedL shall
    be called in the host API.

    First make the watcher list SIP subscription and after that is completed
    in HandleIfMine then continue and modify XDMS authentication rule.
    HandleIfMine can be called also immediately if there already exists an WINFO subscrption.
    */

    iOperation = ESubscribePresenceGrantRequestList;
    iPrFwId = aReqId;
    TRAPD( err, iConnObs.WinfoHandlerL()->SubscribeWinfoListL( aReqId ));
    if ( err )
        {
        iOperation = ENoOperation;
        User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
        }
    else
        {
        iWinfoSubs = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoUnsubscribePresenceGrantRequestListL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoUnsubscribePresenceGrantRequestListL(
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoUnsubscribePresenceGrantRequestListL TEST COVERS 4"));
#endif

    // First modify XDMS authentication rule and then unsubscribe
    // SIP watcher list subscription.
    TRAPD( err, DoStartXdmOperationL( aReqId, EUnsubscribePresenceGrantRequestList ));
    if ( err )
        {
        User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
        }
    iPrFwId = aReqId;
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoGrantPresenceForPresentityL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoGrantPresenceForPresentityL(
    const MXIMPIdentity& aPresentityId,
    const MPresenceInfoFilter& /*aPif*/,  // notice: pif not supported
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoGrantPresenceForPresentityL"));
#endif
    StartXdmOperationL( aPresentityId, aReqId, EGrantPresenceForPresentity );
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoUpdatePresenceGrantPifForPresentityL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoUpdatePresenceGrantPifForPresentityL(
    const MXIMPIdentity& /*aPresentityId*/,
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoUpdatePresenceGrantPifForPresentityL"));
#endif
    // Notice: aPif filter not supported.
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoWithdrawPresenceGrantFromPresentityL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoWithdrawPresenceGrantFromPresentityL(
    const MXIMPIdentity& aPresentityId,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoWithdrawPresenceGrantFromPresentityL"));
#endif
    StartXdmOperationL( aPresentityId, aReqId, EWithdrawFromPresentity );
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoGrantPresenceForPresentityGroupMembersL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoGrantPresenceForPresentityGroupMembersL(
    const MXIMPIdentity& aGroupId,
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoGrantPresenceForPresentityGroupMembersL"));
#endif
    StartXdmOperationL( aGroupId, aReqId, EGrantPresenceForPresentityGroupMembers );
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoUpdatePresenceGrantPifForPresentityGroupMembersL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoUpdatePresenceGrantPifForPresentityGroupMembersL(
    const MXIMPIdentity& /*aGroupId*/,
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoUpdatePresenceGrantPifForPresentityGroupMembersL"));
#endif
    // Notice: aPif filter not supported
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoWithdrawPresenceGrantFromPresentityGroupMembersL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoWithdrawPresenceGrantFromPresentityGroupMembersL(
    const MXIMPIdentity& aGroupId,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoWithdrawPresenceGrantFromPresentityGroupMembersL"));
#endif
    StartXdmOperationL( aGroupId, aReqId, EWithdrawFromGroupMembers );
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoGrantPresenceForEveryoneL()
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoGrantPresenceForEveryoneL(
    const MPresenceInfoFilter& /*aPif*/,   // // Notice: aPif filter not supported
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoGrantPresenceForEveryoneL"));
#endif
    MXIMPIdentity* nobody = iConnObs.ObjectFactory().NewIdentityLC();  // << nobody
    StartXdmOperationL( *nobody, aReqId, EGrantForEveryone );
    CleanupStack::PopAndDestroy( );                     // >> nobody
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoUpdatePresenceGrantPifForEveryoneL()
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoUpdatePresenceGrantPifForEveryoneL(
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoUpdatePresenceGrantPifForEveryoneL"));
#endif
    // Notice: aPif filter not supported
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoWithdrawPresenceGrantFromEveryoneL()
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoWithdrawPresenceGrantFromEveryoneL(
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoWithdrawPresenceGrantFromEveryoneL"));
#endif
    MXIMPIdentity* nobody = iConnObs.ObjectFactory().NewIdentityLC();  // << nobody
    StartXdmOperationL( *nobody, aReqId, EWithdrawFromEveryone );
    CleanupStack::PopAndDestroy( );                     // >> nobody
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoSubscribePresenceBlockListL()
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoSubscribePresenceBlockListL(
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoSubscribePresenceBlockListL"));
#endif
    MXIMPIdentity* nobody = iConnObs.ObjectFactory().NewIdentityLC();  // << nobody
    StartXdmOperationL( *nobody, aReqId, ESubscribeBlockList );
    CleanupStack::PopAndDestroy( );                     // >> nobody
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoUnsubscribePresenceBlockListL()
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoUnsubscribePresenceBlockListL(
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoUnsubscribePresenceBlockListL"));
#endif
    MXIMPIdentity* nobody = iConnObs.ObjectFactory().NewIdentityLC();  // << nobody
    StartXdmOperationL( *nobody, aReqId, EUnsubscribeBlockList );
    CleanupStack::PopAndDestroy( );                     // >> nobody
    }

// Blocking presence information access from single presentities

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoBlockPresenceForPresentityL()
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoBlockPresenceForPresentityL(
    const MXIMPIdentity& aPresentityId,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoBlockPresenceForPresentityL"));
#endif
    StartXdmOperationL( aPresentityId, aReqId, EBlockPresentity );
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoCancelPresenceBlockFromPresentityL()
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoCancelPresenceBlockFromPresentityL(
    const MXIMPIdentity& aPresentityId,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: DoCancelPresenceBlockFromPresentityL"));
#endif
    StartXdmOperationL( aPresentityId, aReqId, EUnblockPresentity );
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoCancel()
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoCancel(  )
    {
    iXdmUtils->Cancel();
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::RunL()
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::RunL(  )
    {
    TInt myStatus = iStatus.Int();

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: RunL %d"), myStatus );
#endif

    if ( iOperation == EUnsubscribePresenceGrantRequestList && !myStatus )
        {
        // unsubscribe watcher list after successful default rule modification.
        TRAPD( err, iConnObs.WinfoHandlerL()->UnsubscribeWinfoListL( iPrFwId ));
        if ( err )
            {
            User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
            }
        else
            {
            // This will end to HandleIfMine.
            }
        }
    else if ( !iXdmOk && !myStatus )
        {
        iXdmOk = ETrue;
        CallActualXdmOperationL();
        }
    else // iXdmOK or myStatus
        {
        CompletePrFwReq( myStatus );
        }

    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::CallActualXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::CallActualXdmOperationL()
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: CallActualXdmOperationL") );
#endif
    switch ( iOperation )
        {
        case EGrantPresenceForPresentity:
            // iPresIdentity can't be null in this case
            iXdmUtils->AddEntityToGrantedL( iPresIdentity->Des(), iStatus );
            SetActive();
            break;
        case EWithdrawFromPresentity:
            // iPresIdentity can't be null in this case
            iXdmUtils->RemoveEntityFromGrantedL( iPresIdentity->Des(),iStatus );
            SetActive();
            break;
        case EGrantPresenceForPresentityGroupMembers:
            // iPresIdentity can't be null in this case
            iXdmUtils->AddGroupToGrantedL( iPresIdentity->Des(), iStatus );
            SetActive();
            break;
        case EWithdrawFromGroupMembers:
            // iPresIdentity can't be null in this case
            iXdmUtils->RemoveGroupFromGrantedL( iPresIdentity->Des(), iStatus );
            SetActive();
            break;
        case EGrantForEveryone:
            iXdmUtils->GrantForEveryoneL( iStatus );
            SetActive();
            break;
        case EWithdrawFromEveryone:
            iXdmUtils->WithdrawFromEveryoneL( iStatus );
            SetActive();
            break;
        case ESubscribeBlockList:
            HandleSubscribeBlockListL();
            break;
        case EUnsubscribeBlockList:
            iXdmUtils->UnsubscribeBlockListL( iStatus );
            SetActive();
            break;
        case EBlockPresentity:
            // iPresIdentity can't be null in this case
            iXdmUtils->AddEntityToBlockedL( iPresIdentity->Des(), iStatus );
            SetActive();
            break;
        case EUnblockPresentity:
            // iPresIdentity can't be null in this case
            iXdmUtils->RemoveEntityFromBlockedL( iPresIdentity->Des(), iStatus );
            SetActive();
            break;
        case ESubscribePresenceGrantRequestList:
            iXdmUtils->SetReactiveAuthL( iStatus );
            SetActive();
            break;
        case EUnsubscribePresenceGrantRequestList:
        default:
            iXdmUtils->SetProactiveAuthL( iStatus );
            SetActive();
            break;
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::CopyBlockersToArrayL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::CopyBlockersToArrayL(
        CPtrCArray& aBlockers, RPointerArray<MXIMPIdentity>& aBlockedSouls )
    {
    MXIMPObjectFactory& myFactory = iConnObs.ObjectFactory();

    aBlockedSouls.Reset();

    TInt nodeCount = aBlockers.Count();
    for ( TInt i = 0; i < nodeCount; i++ )
        {
        MXIMPIdentity* entity = myFactory.NewIdentityLC(); // << entity
        entity->SetIdentityL( aBlockers[i] );
        aBlockedSouls.AppendL( entity );
        CleanupStack::Pop(); // >> entity
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::RunError
// ---------------------------------------------------------------------------
//
TInt CSimplePluginAuthorization::RunError( TInt aError )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: RunError %d"), aError );
#endif
    // complete the open request
    CompletePrFwReq( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CSimplePluginAuthorization::GetInterface(
        TInt32 aInterfaceId,
        TIfGetOps aOptions )
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        MProtocolPresenceAuthorization* myIf = this;
        return myIf;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( _L("CSimplePlugin"), KErrExtensionNotSupported );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CSimplePluginAuthorization::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions ) const
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        const MProtocolPresenceAuthorization* myIf = this;
        return myIf;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( _L("CSimplePlugin"), KErrExtensionNotSupported );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::GetInterfaceId()
// ---------------------------------------------------------------------------
//
TInt32 CSimplePluginAuthorization::GetInterfaceId() const
    {
    return MProtocolPresenceAuthorization::KInterfaceId;
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::SetPresIdentityL()
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::SetPresIdentityL(
     const MXIMPIdentity& aPresentityId )
     {
     delete iPresIdentity;
     iPresIdentity = NULL;
     iPresIdentity = aPresentityId.Identity().AllocL();
     }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::WinfoNotificationL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::WinfoNotificationL(
    MSimpleWinfo& aWinfo )
    {
    if ( !iWinfoSubs )
        {
        return;
        }
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: WinfoNotificationL starts"));
#endif

    using namespace NSimplePlugin::NSimpleOma;

    RPointerArray<MSimpleElement> elems;
    CleanupClosePushL( elems );
    TInt err = aWinfo.SimpleElementsL( elems );
    User::LeaveIfError( err );
    TInt count = elems.Count();

    if ( !count )
        {
        // stop parsing empty notification
        User::Leave( KErrArgument );
        }

    __ASSERT_DEBUG( count == 1, User::Leave( KErrCorrupt ) );

    const TDesC8* stateVal = aWinfo.AttrValue( KSimpleState8 );
    if ( stateVal && !stateVal->CompareF( KSimpleFull8 ))
        {
        // Call full list method
        HandleFullWinfoNotificationL( aWinfo );
        }
    else
        {
        // Indicate changes in WINFO list one by one.
        HandlePartialWinfoNotificationL( aWinfo );
        }
    CleanupStack::PopAndDestroy( &elems );

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: WinfoNotificationL ends"));
#endif
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::StartXdmOperationL(
    const MXIMPIdentity& aId,
    TXIMPRequestId aReqId,
    TPluginAuthOperation aOperation )
    {
    TRAPD( err, DoStartXdmOperationL(aId, aReqId, aOperation ));
    if ( err )
        {
        User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoStartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoStartXdmOperationL(
    const MXIMPIdentity& aId,
    TXIMPRequestId aReqId,
    TPluginAuthOperation aOperation )
    {
    SetPresIdentityL( aId );
    DoStartXdmOperationL( aReqId, aOperation );
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::DoStartXdmOperationL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::DoStartXdmOperationL(
    TXIMPRequestId aReqId,
    TPluginAuthOperation aOperation )
    {
    __ASSERT_DEBUG( !IsActive(), User::Leave( KErrCorrupt ) );
    iPrFwId = aReqId;
    iOperation = aOperation;
    if ( !iXdmUtils )
        {
        iXdmUtils = iConnObs.XdmUtilsL();
        }
    if ( iXdmOk )
        {
        CallActualXdmOperationL();
        }
    else
        {
        iXdmUtils->InitializeXdmL( iStatus );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::HandleFullWinfoNotificationL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::HandleFullWinfoNotificationL(
    MSimpleWinfo& aWinfo )
    {

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: HandleFullWinfoNotificationL starts"));
#endif

    // Search for pending watchers and call HandlePresenceGrantRequestListL
    using namespace NSimplePlugin::NSimpleOma;

    RPointerArray<MSimpleElement> elems;
    CleanupClosePushL( elems );
    TInt err = aWinfo.SimpleElementsL( elems );
    User::LeaveIfError( err );
    TInt count = elems.Count();

    MSimpleElement* elem = elems[0];
    TPtrC8 p8 = elem->LocalName();
    err = p8.CompareF( KSimpleWatcherList8 );
    User::LeaveIfError( err );

    err = elem->SimpleElementsL( elems );
    User::LeaveIfError( err );
    count = elems.Count();

    // Search watchers who are pending (status = pending)
    HBufC* nodeContent = NULL;
    MXIMPObjectCollection *pendings = iConnObs.ObjectFactory().NewObjectCollectionLC();

    TBool pendingFound(EFalse);

    for ( TInt i = 0; i < count; i++ )
        {
        elem = elems[i];
        p8.Set( elem->LocalName());
        if (!( p8.CompareF( KSimpleWatcher8 )))
            {
            const TDesC8* pp8 = elem->AttrValue( KSimpleStatus8 );
            if ( pp8 && !pp8->CompareF( KSimplePending8 ))
                {
                // Build collection of grant requests
                // Find the child node containing the SIP entity
                nodeContent = elem->ContentUnicodeL();
                CleanupStack::PushL( nodeContent );
                // create the collection entry here
                MPresenceGrantRequestInfo* grInfo =
                   iConnObs.PresenceObjectFactory().NewPresenceGrantRequestInfoLC();
                MXIMPIdentity* identity = iConnObs.ObjectFactory().NewIdentityLC();
                identity->SetIdentityL( nodeContent->Des());
                grInfo->SetRequestorIdL( identity ); // ownership is taken
                CleanupStack::Pop( ); // identity
#ifdef _DEBUG
                TBuf<200> debug_buffer;
                debug_buffer = nodeContent->Des();
                PluginLogger::Log(
                    _L("PluginAuth: add pending into collection: %S"), &debug_buffer );
#endif
                pendings->AddObjectL( grInfo );  // ownership is taken
                CleanupStack::Pop( );            // grInfo
                pendingFound = ETrue;
                CleanupStack::PopAndDestroy( nodeContent );
                }
            }
        }

    if ( pendingFound )
        {
#ifdef _DEBUG
        PluginLogger::Log(_L("PluginAuthorization: callback HandlePresenceGrantRequestListL"));
#endif
        iPrFwIdOwn = iDataHost->HandlePresenceGrantRequestListL( pendings );
        CleanupStack::Pop();    // >> pendings
        }
    else
        {
        CleanupStack::PopAndDestroy( ); // >> pendings
        }

    CleanupStack::PopAndDestroy( &elems );

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: HandleFullWinfoNotificationL ends"));
#endif

    /* example:

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
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::HandlePartialWinfoNotificationL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::HandlePartialWinfoNotificationL(
    MSimpleWinfo& aWinfo )
    {

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: HandlePartialWinfoNotificationL starts"));
#endif

    using namespace NSimplePlugin::NSimpleOma;

    RPointerArray<MSimpleElement> elems;
    CleanupClosePushL( elems );
    TInt err = aWinfo.SimpleElementsL( elems );
    User::LeaveIfError( err );
    TInt count = elems.Count();

    MSimpleElement* elem = elems[0];
    TPtrC8 p8 = elem->LocalName();
    err = p8.CompareF( KSimpleWatcherList8 );
    User::LeaveIfError( err );

    err = elem->SimpleElementsL( elems );
    User::LeaveIfError( err );
    count = elems.Count();

    HBufC* nodeContent = NULL;

    for ( TInt i = 0; i < count; i++ )
        {
        elem = elems[i];
        p8.Set( elem->LocalName());
        if (!( p8.CompareF( KSimpleWatcher8 )))
            {
            const TDesC8* pp8 = elem->AttrValue( KSimpleStatus8 );
            if ( pp8 && !pp8->CompareF( KSimplePending8 ))
                {
                nodeContent = elem->ContentUnicodeL();
                CleanupStack::PushL( nodeContent );
                CallHandlePresenceGrantRequestReceivedL( nodeContent->Des() );
                CleanupStack::PopAndDestroy( nodeContent );
                }
            else if ( pp8 && !pp8->CompareF( KSimpleTerminated8 ))
                {
                nodeContent = elem->ContentUnicodeL();
                CleanupStack::PushL( nodeContent );
                CallHandlePresenceGrantRequestObsoletedL( nodeContent->Des() );
                CleanupStack::PopAndDestroy( nodeContent );
                }
            }
        }
    CleanupStack::PopAndDestroy( &elems );

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: HandlePartialWinfoNotificationL ends"));
#endif
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::CallHandlePresenceGrantRequestReceivedL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::CallHandlePresenceGrantRequestReceivedL(
    const TDesC& aUserId )
    {
    // create the collection entry here
    MPresenceGrantRequestInfo* grInfo =
       iConnObs.PresenceObjectFactory().NewPresenceGrantRequestInfoLC();  // +grInfo
    MXIMPIdentity* identity = iConnObs.ObjectFactory().NewIdentityLC();   // +identity
    identity->SetIdentityL( aUserId );
    grInfo->SetRequestorIdL( identity ); // ownership is taken
    CleanupStack::Pop( );    // -identity
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: callback HandlePresenceGrantRequestReceivedL"));
#endif
    iPrFwIdOwn = iDataHost->HandlePresenceGrantRequestReceivedL( grInfo ); // ownership is taken
    CleanupStack::Pop( );           // -grInfo
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::CallHandlePresenceGrantRequestObsoletedL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::CallHandlePresenceGrantRequestObsoletedL(
    const TDesC& aUserId)
    {
    MXIMPIdentity* identity = iConnObs.ObjectFactory().NewIdentityLC();
    identity->SetIdentityL( aUserId );
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: callback HandlePresenceGrantRequestObsoletedL"));
#endif
    iPrFwIdOwn = iDataHost->HandlePresenceGrantRequestObsoletedL( identity); // ownership is taken
    CleanupStack::Pop( );           // >> identity
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::HandleSubscribeBlockListL
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::HandleSubscribeBlockListL()
    {
    RPointerArray<MXIMPIdentity> blockedSouls;
    CleanupClosePushL( blockedSouls );

    const TInt KMyGran = 10;
    CPtrCArray* blockers = new (ELeave) CPtrCArray( KMyGran );
    CleanupStack::PushL( blockers );      // << blockers

    iXdmUtils->SubscribeBlockListL( *blockers );

    CopyBlockersToArrayL( *blockers, blockedSouls );

    CleanupStack::PopAndDestroy( blockers );

    MXIMPObjectCollection* coll =
        iConnObs.ObjectFactory().NewObjectCollectionLC();    // << coll
    TInt count = blockedSouls.Count();
    for ( TInt i = count-1; i >= 0; i-- )
        {
        MXIMPIdentity* currId = blockedSouls[i];
        MPresenceBlockInfo* bInfo = iConnObs.PresenceObjectFactory().NewPresenceBlockInfoLC();
        bInfo->SetBlockedEntityIdL( currId ); // ownership is taken
        bInfo->SetBlockedEntityDisplayNameL( currId->Identity() );
        blockedSouls.Remove( i );
        coll->AddObjectL( bInfo );      // ownership is taken
        CleanupStack::Pop();                                // >> bInfo
        }
    CompletePrFwReq( KErrNone );
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuth: callback HandlePresenceBlockListL"));
#endif
    // Callback for subscription result
    iDataHost->HandlePresenceBlockListL( coll );

    CleanupStack::Pop();                                // >> coll
    CleanupStack::PopAndDestroy( &blockedSouls );

    // Callback for subscription state (terminated).
    MXIMPDataSubscriptionState* myState = iConnObs.ObjectFactory().NewDataSubscriptionStateLC();
    MXIMPStatus* myStatus = iConnObs.ObjectFactory().NewStatusLC();
    // Notice: consider error codes
    myStatus->SetResultCode( KErrNone );
    myState->SetSubscriptionStateL(
        MXIMPDataSubscriptionState::ESubscriptionInactive );
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: callback SetPresenceBlockDataSubscriptionStateL"));
#endif
    iDataHost->SetPresenceBlockDataSubscriptionStateL(
        myState, myStatus );
    CleanupStack::Pop( 2 ); // >> myState, myStatus

    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::CompletePrFwReq
// ---------------------------------------------------------------------------
//
void CSimplePluginAuthorization::CompletePrFwReq( TInt aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: CompletePrFwReq status=%d"), aStatus );
#endif
    iOperation = ENoOperation;
    iConnObs.CompleteReq( iPrFwId, aStatus  );
    iPrFwId = TXIMPRequestId();
    }

// ---------------------------------------------------------------------------
// CSimplePluginAuthorization::HandleIfMine
// ---------------------------------------------------------------------------
//
TBool CSimplePluginAuthorization::HandleIfMine( TXIMPRequestId aReqId, TInt aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: HandleIfMine aStatus=%d iOper=%d"), aStatus, iOperation);
#endif
    TInt err = KErrNone;
    TBool ret = EFalse;

    // We check wheter the request is made by this entity.
    if ( iPrFwId != aReqId )
        {
        // Nothing to do, request was not orginated by this entity.
        }
    else
        {
        if ( !aStatus )
            {
            switch ( iOperation )
                {
                case ESubscribePresenceGrantRequestList:
                    // Modify rule to CONFIRM
#ifdef _DEBUG
                    PluginLogger::Log(_L("PluginAuthorization: HandleIfMine TEST COVERS 7"));
#endif
                    TRAP( err, DoStartXdmOperationL( aReqId, iOperation ));
                    err = CSimplePluginConnection::HarmonizeErrorCode( err );
                    ret = ETrue;
                    break;
                // In UnsubscribePresenceGrantRequestList case the rule modifcation
                // has been made first before SIP unsubscribe operation.
                case EUnsubscribePresenceGrantRequestList:
                default:
                    iWinfoSubs = EFalse;
                    break;
                }
            }
        else
            {
            CompletePrFwReq( err );
            ret = ETrue;
            }
        if ( err )
            {
            CompletePrFwReq( err );
            ret = ETrue;
            }
        else
            {
            }
        }

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginAuthorization: HandleIfMine returns %d"), ret );
#endif
    return ret;
    }

// End of file

