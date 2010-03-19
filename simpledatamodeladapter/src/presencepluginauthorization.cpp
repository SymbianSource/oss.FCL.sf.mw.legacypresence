/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <s32strm.h>
#include <utf.h>
#include <ximpdatasubscriptionstate.h>
#include <protocolpresenceauthorizationdatahost.h>
#include <ximpobjectfactory.h>
#include <presenceobjectfactory.h>
#include <ximpstatus.h>
#include <ximpidentity.h>
#include <presencegrantrequestinfo.h>
#include <ximpobjectcollection.h>
#include <presenceblockinfo.h>
#include <presenceinfofilter.h>
#include <msimplewinfo.h>
#include <msimpleelement.h>

#include <presenceinfo.h>
#include <presenceinfofieldcollection.h>
#include <personpresenceinfo.h>
#include <presenceinfofield.h>
#include <presenceinfofieldvalueenum.h>
#include <avabilitytext.h>

#include "presencepluginauthorization.h"
#include "presencepluginwinfo.h"
#include "presenceplugincommon.h"
#include "presencepluginxdmutils.h"
#include "presencepluginwatcher.h"
#include "presenceplugindata.h"
#include "presencepluginutility.h"
#include "presenceplugingroup.h"
#include "presencepluginvirtualgroup.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::CPresencePluginAuthorization()
// ---------------------------------------------------------------------------
//
CPresencePluginAuthorization::CPresencePluginAuthorization(
    MPresencePluginConnectionObs& aObs,
    CPresencePluginData* aPresenceData )
    : CActive( CActive::EPriorityStandard ),
    iConnObs(aObs), iSubscribed( EFalse ),
    iOperation( ENoOperation ),
    iXdmOk( EFalse ), iComplete( EFalse ),iPresenceData( aPresenceData )
    {
    CActiveScheduler::Add(this); 
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::NewL()
// ---------------------------------------------------------------------------
//
CPresencePluginAuthorization* CPresencePluginAuthorization::NewL(
    MPresencePluginConnectionObs& aObs,
    CPresencePluginData* aPresenceData )
    {
    DP_SDA("CPresencePluginAuthorization::NewL ");
    CPresencePluginAuthorization* self =
        new( ELeave ) CPresencePluginAuthorization( aObs, aPresenceData );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::~CPresencePluginAuthorization()
// ---------------------------------------------------------------------------
//
CPresencePluginAuthorization::~CPresencePluginAuthorization()
    {
    delete iPresIdentity;
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::SetDataHost()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::SetDataHost( 
    MProtocolPresenceAuthorizationDataHost& aDataHost )
    {
    DP_SDA("CPresencePluginAuthorization::SetDataHost ");
    iDataHost = &aDataHost;
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::WinfoTerminatedL()
// ---------------------------------------------------------------------------
// 
void CPresencePluginAuthorization::WinfoTerminatedL( TInt aReason )
    {
    DP_SDA("CPresencePluginAuthorization::WinfoTerminatedL ");
    
    if ( iSubscribed )
        {
        MXIMPDataSubscriptionState* myState =
            iConnObs.ObjectFactory().NewDataSubscriptionStateLC();
        MXIMPStatus* myStatus = iConnObs.ObjectFactory().NewStatusLC();
        myStatus->SetResultCode( aReason ? aReason : KErrCompletion );
        myState->SetSubscriptionStateL( 
            MXIMPDataSubscriptionState::ESubscriptionInactive );
        
        // parameters' OWNERSHIP is taken   
        iDataHost->SetPresenceGrantRequestDataSubscriptionStateL( 
            myState,  
            myStatus );
        
        CleanupStack::Pop( /*myStatus*/ );
        CleanupStack::Pop( /*myState*/ );
        }
    
    DP_SDA("CPresencePluginAuthorization::WinfoTerminatedL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoSubscribePresenceGrantRequestListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoSubscribePresenceGrantRequestListL(
    TXIMPRequestId aReqId )
    {
    DP_SDA("DoSubscribePresenceGrantRequestListL ");
    iConnObs.WinfoHandlerL()->SubscribeWinfoListL( aReqId );
    iSubscribed = ETrue;
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoUnsubscribePresenceGrantRequestListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoUnsubscribePresenceGrantRequestListL(
    TXIMPRequestId aReqId )
    {
    DP_SDA("DoUnsubscribePresenceGrantRequestListL ");
    iConnObs.WinfoHandlerL()->UnsubscribeWinfoListL( aReqId );
    iSubscribed = EFalse;
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoGrantPresenceForPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoGrantPresenceForPresentityL(
    const MXIMPIdentity& aPresentityId,
    const MPresenceInfoFilter& /*aPif*/,  // notice: pif support
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginAuthorization::DoGrantPresenceForPresentityL ");
    
    // Workaround for clients that can only grant when accepting buddy request.
    // SIMPLE also requires grant + subscribe so do both here even though
    // it is agains ximp api definitions.
    
    _LIT( KDefaultBuddyList, "buddylist" );
    
    iAuthState = EStateAcceptBuddyRequest;
    MXIMPIdentity* buddyList = iConnObs.ObjectFactory().NewIdentityLC(); 
    buddyList->SetIdentityL( KDefaultBuddyList() ); 
    
    iConnObs.GroupsL().DoAddPresentityGroupMemberL( *buddyList, aPresentityId,
        KNullDesC(), aReqId );
    CleanupStack::PopAndDestroy( 1 ); // buddyList 
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoPerformGrantPresenceForPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoPerformGrantPresenceForPresentityL(
    const MXIMPIdentity& aPresentityId,
    TRequestStatus& aClientStatus )
    {
    DP_SDA("CPresencePluginAuthorization::DoPerformGrantPresenceForPresentityL");
    StartXdmOperationL( aPresentityId, 
        EGrantPresenceForPresentity, aClientStatus );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoUpdatePresenceGrantPifForPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoUpdatePresenceGrantPifForPresentityL(
    const MXIMPIdentity& /*aPresentityId*/,
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
    // Notice: later
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoWithdrawPresenceGrantFromPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoWithdrawPresenceGrantFromPresentityL(
    const MXIMPIdentity& aPresentityId,
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginAuth::DoWithdrawPresenceGrantFromPresentityL ");
    StartXdmOperationL( aPresentityId, aReqId, EWithdrawFromPresentity );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoPerformWithdrawPresGrantFromPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoPerformWithdrawPresGrantFromPresentityL(
    const MXIMPIdentity& aPresentityId,
    TRequestStatus& aClientStatus )
    {
    DP_SDA("CPresencePluginAuthorization");
    DP_SDA(" -> DoPerformWithdrawPresGrantFromPresentityL");
    StartXdmOperationL( aPresentityId,
        EWithdrawFromPresentity, aClientStatus );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoGrantPresenceForPresentityGroupMembersL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoGrantPresenceForPresentityGroupMembersL(
    const MXIMPIdentity& /*aGroupId*/,
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
    DP_SDA("CPresencePluginAuthorization::DoGrantPresenceForPresentityGroupMembersL -not supported");
    
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::
// DoUpdatePresenceGrantPifForPresentityGroupMembersL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::
    DoUpdatePresenceGrantPifForPresentityGroupMembersL(
    const MXIMPIdentity& /*aGroupId*/,
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
    DP_SDA("CPresencePluginAuthorization::DoUpdatePresenceGrantPifForPresentityGroupMembersL -not supported");
    
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::
// DoWithdrawPresenceGrantFromPresentityGroupMembersL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::
    DoWithdrawPresenceGrantFromPresentityGroupMembersL(
    const MXIMPIdentity& /*aGroupId*/,
    TXIMPRequestId /*aReqId*/ )
    {
    DP_SDA("CPresencePluginAuthorization::DoWithdrawPresenceGrantFromPresentityGroupMembersL -not supported");
    
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoGrantPresenceForEveryoneL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoGrantPresenceForEveryoneL(
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
    DP_SDA("CPresencePluginAuthorization::DoGrantPresenceForEveryoneL -not supported");
    
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoUpdatePresenceGrantPifForEveryoneL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoUpdatePresenceGrantPifForEveryoneL(
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
    DP_SDA("CPresencePluginAuthorization::DoUpdatePresenceGrantPifForEveryoneL -not supported");
    
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoWithdrawPresenceGrantFromEveryoneL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoWithdrawPresenceGrantFromEveryoneL(
    TXIMPRequestId /*aReqId*/ )
    {
    DP_SDA("CPresencePluginAuthorization::DoWithdrawPresenceGrantFromEveryoneL -not supported");
    
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoSubscribePresenceBlockListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoSubscribePresenceBlockListL(
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginAuthorization::DoSubscribePresenceBlockListL ");
    MXIMPIdentity* nobody = iConnObs.ObjectFactory().NewIdentityLC(); 
    StartXdmOperationL( *nobody, aReqId, ESubscribeBlockList );
    // Cannot PopAndDestroy by name
    CleanupStack::PopAndDestroy( );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoUnsubscribePresenceBlockListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoUnsubscribePresenceBlockListL(
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginAuthorization::DoUnsubscribePresenceBlockListL ");
    MXIMPIdentity* nobody = iConnObs.ObjectFactory().NewIdentityLC();
    StartXdmOperationL( *nobody, aReqId, EUnsubscribeBlockList );
    // Cannot PopAndDestroy by name
    CleanupStack::PopAndDestroy();
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoBlockPresenceForPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoBlockPresenceForPresentityL(
    const MXIMPIdentity& aPresentityId,
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginAuthorization::DoBlockPresenceForPresentityL ");
    StartXdmOperationL( aPresentityId, aReqId, EBlockPresentity );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoPerformBlockPresenceForPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoPerformBlockPresenceForPresentityL(
    const MXIMPIdentity& aPresentityId,
    TRequestStatus& aClientStatus )
    {
    DP_SDA(
        "CPresencePluginAuthorization::DoPerformBlockPresenceForPresentityL");
    StartXdmOperationL( aPresentityId, EBlockPresentity, aClientStatus );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoCancelPresenceBlockFromPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoCancelPresenceBlockFromPresentityL(
    const MXIMPIdentity& aPresentityId,
    TXIMPRequestId aReqId )
    {
    DP_SDA(" DoCancelPresenceBlockFromPresentityL ");
    StartXdmOperationL( aPresentityId, aReqId, EUnblockPresentity );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoPerformCancelPresenceBlockFromPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoPerformCancelPresenceBlockFromPresentityL(
    const MXIMPIdentity& aPresentityId,
    TRequestStatus& aClientStatus )
    {
    DP_SDA(" DoPerformCancelPresenceBlockFromPresentityL (list)");
    StartXdmOperationL( aPresentityId, EUnblockPresentity, aClientStatus );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::DoCancel()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::DoCancel(  )
    {
    iXdmUtils->Cancel();
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::RunL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::RunL(  )
    {
    TInt myStatus = iStatus.Int();
    
    DP_SDA2( "CPresencePluginAuthorization::RunL mystatus %d ", myStatus );
    DP_SDA2( "CPresencePluginAuthorization::RunL state %d ",iOperation );
    
    if ( !iXdmOk && !myStatus )
        {
        DP_SDA( "CPresencePluginAuthorization::RunL !iXdmOk && !myStatus" );
        iXdmOk = ETrue;
        CallActualXdmOperationL();
        }
    else if ( iClientStatus )
        {
        DP_SDA( "CPresencePluginAuthorization::RunL - complete client" );
        TRequestStatus* s = iClientStatus;
        User::RequestComplete( s, myStatus );
        iClientStatus = NULL;
        DP_SDA( "CPresencePluginAuthorization::RunL - complete client ok" );
        }
    else if ( !myStatus )
        {
        switch ( iAuthState )
            {
            case EStateDoBlock:
                {
                DP_SDA( "CPresencePluginAuthorization::RunL - Block presentity" );
                BlockPresentityL();
                iAuthState = EStateRemovePresentityFromGranted;
                }
                break;
                
            case EStateRemovePresentityFromGranted:
                {
                DP_SDA( "CPresencePluginAuthorization::RunL - Remove presentity from granted" );
                WithdrawFromPresentityL();
                iAuthState = EStateBlocked;
                }
                break;
            
            case EStateBlocked:
                {
                SetPresentityBlockedToXIMPL();
                iAuthState = EStateIdle;
                CompleteXIMPReq( myStatus );
                }
                break;
                
            case EStateDoUnBlock:
                {
                DP_SDA( "CPresencePluginAuthorization::RunL - Grant presence for presentity" );  
                GrantPresenceForPresentityL();
                iAuthState = EStatePresenceGranted;
                }
                break;
                
            case EStatePresenceGranted:
                {
                DP_SDA( "CPresencePluginAuthorization::RunL -Subscribe presentity presence" );  
                
                MXIMPIdentity* identity = iConnObs.ObjectFactory().NewIdentityLC();
                identity->SetIdentityL( iPresIdentity->Des() ); 
                iConnObs.WatcherHandlerL()->DoPerformSubscribePresentityPresenceL( *identity, iStatus );
                CleanupStack::PopAndDestroy( ); //identity 
                iAuthState = EStateIdle;
                SetActive();
                }
                break;
                
            default:
                {
                DP_SDA( "CPresencePluginAuthorization::RunL - default case" );
                CompleteXIMPReq( myStatus );
                }
                break;
            }
        }
    else
        {
        DP_SDA( "CPresencePluginAuthorization::RunL -error" );
        CompleteXIMPReq( myStatus );
        }
        
    if ( EStateAcceptBuddyRequest == iAuthState && EGrantPresenceForPresentity == iOperation )
        {
        iAuthState = EStateIdle;
        HBufC* withoutPrefix = iPresenceData->RemovePrefixLC( *iPresIdentity );
        CallHandlePresenceGrantRequestObsoletedL( *withoutPrefix );
        iConnObs.SubscribedContacts()->HandlePresentityGroupMemberAddedL( *withoutPrefix );
        CleanupStack::PopAndDestroy( withoutPrefix );
        }
    
    DP_SDA( "CPresencePluginAuthorization::RunL - exit" );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::CallActualXdmOperationL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::CallActualXdmOperationL()
    {
    DP_SDA("CPresencePluginAuthorization::CallActualXdmOperationL ");
    DP_SDA2("CallActualXdmOperationL operation %d", iOperation );
    iComplete = EFalse;
    switch ( iOperation )
        {
        case EGrantPresenceForPresentity:
            {
            DP_SDA("CallActualXdmOperationL EGrantPresenceForPresentity");
            GrantPresenceForPresentityL();
            }
            break;
            
        case EWithdrawFromPresentity:
            {
            DP_SDA("CallActualXdmOperationL EWithdrawFromPresentity");
            WithdrawFromPresentityL();
            }
            break;
            
        case ESubscribeBlockList:
            {
            SubscribeBlockListL( );
            }
            break;
            
        case EUnsubscribeBlockList:
            {
            UnSubscribeBlockListL();
            }
            break;
            
        case EBlockPresentity:
            {
            DP_SDA( "CallActualXdmOperationL EBlockPresentity" );
            
            TRAPD( error, UnsubscribePresentityPresenceL() );
            
            if( error )
                {
                DP_SDA2( "Unsubscribe presentity FAIL, error = %d" , error );
                
                if ( KErrNotFound != error  )
                    {
                    User::Leave( error );
                    }
                BlockPresentityL();
                iAuthState = EStateBlocked;
                }
            else
                {
                iAuthState = EStateDoBlock;
                }
            }
            break;
            
        case EUnblockPresentity:
            {
            DP_SDA("CallActualXdmOperationL EUnblockPresentity"); 
            UnblockPresentityL();
            }
            break;
            
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    DP_SDA("CPresencePluginAuthorization::CallActualXdmOperationL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::RunError()
// ---------------------------------------------------------------------------
//
TInt CPresencePluginAuthorization::RunError( TInt aError )
    {
    // complete the open request
    CompleteXIMPReq( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CPresencePluginAuthorization::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions )
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        return this;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( KPluginName, KErrExtensionNotSupported );
        }  
    return NULL;
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::GetInterface() const
// ---------------------------------------------------------------------------
//
const TAny* CPresencePluginAuthorization::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions ) const
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        return const_cast<CPresencePluginAuthorization*>(this);
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( KPluginName, KErrExtensionNotSupported );
        }  
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::GetInterfaceId()
// ---------------------------------------------------------------------------
//
TInt32 CPresencePluginAuthorization::GetInterfaceId() const
    {
    return MProtocolPresenceAuthorization::KInterfaceId;
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::SetPresIdentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::SetPresIdentityL(
    const MXIMPIdentity& aPresentityId,
    TBool aFormatUri )
    {  
    DP_SDA2("CPresencePluginAuthorization::SetPresIdentityL: %S",
        &aPresentityId.Identity() );
    
    delete iPresIdentity;
    iPresIdentity = NULL;
    
    if ( aFormatUri )
        {
        DP_SDA("CPresencePluginAuthorization::SetPresIdentityL, add prefix");
        
        HBufC8* identityCopy = HBufC8::NewLC( KBufSize255 );
        TPtr8 identityCopyPtr( identityCopy->Des() );
        identityCopyPtr.Copy( aPresentityId.Identity() );
        
        HBufC8* prefixUri = iPresenceData->CreatePresenceUri8LC( 
            identityCopyPtr );
        
        HBufC* prefixUri16 = HBufC::NewLC( KBufSize255 );
        TPtr prefixUri16Ptr( prefixUri16->Des() );
        prefixUri16Ptr.Copy( *prefixUri );
        
        iPresIdentity = prefixUri16;
        CleanupStack::Pop( prefixUri16 );
        CleanupStack::PopAndDestroy( prefixUri );
        CleanupStack::PopAndDestroy( identityCopy );
        }
    else
        {
        DP_SDA(" SetPresIdentityL, use as it is");
        iPresIdentity = aPresentityId.Identity().AllocL();
        }
    DP_SDA( "CPresencePluginAuthorization::SetPresIdentityL -exit" );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::WinfoNotificationL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::WinfoNotificationL(
    MSimpleWinfo& aWinfo )
    {
    DP_SDA("CPresencePluginAuthorization::WinfoNotificationL");
    // Ignore notification if not subscribed
    using namespace NPresencePlugin::NPresence;
    
    RPointerArray<MSimpleElement> elems;
    CleanupClosePushL( elems );
    TInt err = aWinfo.SimpleElementsL( elems );
    User::LeaveIfError( err );
    TInt count = elems.Count();
    
    DP_SDA2("CPresencePluginAuthorization::WinfoNotificationL count %d",
        count);
    
    if ( !count ) 
        { 
        // stop parsing empty notification 
        DP_SDA("CPresencePluginAuthorization::WinfoNotificationL LEAVE");
        User::Leave( KErrArgument ); 
        } 
    
    const TDesC8* stateVal = aWinfo.AttrValue( KPresenceState8 ); 
    if ( stateVal && !stateVal->CompareF( KPresenceFull8 )) 
        { 
        // Call full list method
        DP_SDA("CPresencePluginAuthorization::WinfoNotificationL FULL winfo");
        HandleFullWinfoNotificationL( aWinfo ); 
        } 
    else 
        { 
        // Indicate changes in WINFO list one by one.
        DP_SDA("CPresencePluginAuthorization::WinfoNotificationLPARTIAL winfo");
        HandlePartialWinfoNotificationL( aWinfo );
        } 
    CleanupStack::PopAndDestroy( &elems );
    DP_SDA("CPresencePluginAuthorization::WinfoNotificationL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::StartXdmOperationL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::StartXdmOperationL(
    const MXIMPIdentity& aId,
    TXIMPRequestId aReqId,
    TPluginAuthOperation aOperation )
    {
    __ASSERT_DEBUG( !IsActive(), User::Leave( KErrNotReady ) );
    DP_SDA("CPresencePluginAuthorization::StartXdmOperationL 2");
    SetPresIdentityL( aId, ETrue );
    
    iXIMPId = aReqId;
    iOperation = aOperation;
    if ( !iXdmUtils )
        {
        DP_SDA("StartXdmOperationL !iXdmUtils");
        iXdmUtils = iConnObs.XdmUtilsL();
        }
    if ( iXdmUtils->CheckXdmDoc() )
        {
        DP_SDA("StartXdmOperationL iXdmOk");
        iXdmOk = ETrue;
        CallActualXdmOperationL();
        }
    else
        {
        DP_SDA("StartXdmOperationL else");
        iXdmUtils->InitializeXdmL( iStatus );
        SetActive();
        }
    DP_SDA("CPresencePluginAuthorization::StartXdmOperationL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::StartXdmOperationL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::StartXdmOperationL(
    const MXIMPIdentity& aId,
    TPluginAuthOperation aOperation,
    TRequestStatus& aClientStatus )
    {
    __ASSERT_DEBUG( !IsActive(), User::Leave( KErrNotReady ) );
    DP_SDA("CPresencePluginAuthorization::StartXdmOperationL 3 (list)");
    
    SetPresIdentityL( aId, EFalse );
    
    iClientStatus = &aClientStatus;
    *iClientStatus = KRequestPending;
    
    iOperation = aOperation;
    
    if ( !iXdmUtils )
        {
        DP_SDA("StartXdmOperationL !iXdmUtils");
        iXdmUtils = iConnObs.XdmUtilsL();
        }
    if ( iXdmUtils->CheckXdmDoc() )
        {
        DP_SDA("StartXdmOperationL iXdmOk");
        iXdmOk = ETrue;
        CallActualXdmOperationL();
        }
    else
        {
        DP_SDA("StartXdmOperationL else");
        iXdmUtils->InitializeXdmL( iStatus );
        SetActive();
        }
    DP_SDA("CPresencePluginAuthorization::StartXdmOperationL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::CompleteXIMPReq()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::CompleteXIMPReq( TInt aStatus )
    {
    DP_SDA("CPresencePluginAuthorization::CompleteXIMPReq");
    iOperation = ENoOperation;
    iConnObs.CompleteReq( iXIMPId, aStatus );
    iXIMPId = TXIMPRequestId();
    DP_SDA("CPresencePluginAuthorization::CompleteXIMPReq end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::SubscribeBlockListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::SubscribeBlockListL()
    {
    DP_SDA("CPresencePluginAuthorization::SubscribeBlockList");
    
    RPointerArray<MXIMPIdentity> blocked;
    CleanupStack::PushL( TCleanupItem(
        TPresencePluginUtility::ResetAndDestroyIdentities,
        &blocked ) );
    
    iXdmUtils->SubscribeBlockListL( blocked );
    MXIMPObjectCollection* coll =
        iConnObs.ObjectFactory().NewObjectCollectionLC(); // << coll
    
    TInt count = blocked.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        MXIMPIdentity* currId = blocked[i];
        HBufC* withoutPrefix = iPresenceData->RemovePrefixLC( currId->Identity() );
        currId->SetIdentityL( *withoutPrefix );
        CleanupStack::PopAndDestroy( withoutPrefix );
        MPresenceBlockInfo* bInfo =
            iConnObs.PresenceObjectFactoryOwn().NewPresenceBlockInfoLC();
        bInfo->SetBlockedEntityIdL( currId ); // ownership is taken
        blocked.Remove( i ); // remove currId from blocked
        blocked.InsertL( NULL, i );
        bInfo->SetBlockedEntityDisplayNameL( currId->Identity() );
        coll->AddObjectL( bInfo );// ownership is taken
        CleanupStack::Pop();// >> bInfo
        }
    
    DP_SDA("SubscribeBlockList complete request");
    CompleteXIMPReq( KErrNone );
    
    MXIMPDataSubscriptionState* myState =
       iConnObs.ObjectFactory().NewDataSubscriptionStateLC();
    
    MXIMPStatus* myStatus = iConnObs.ObjectFactory().NewStatusLC();
    
    // Notice: consider XIMP error codes
    myStatus->SetResultCode( KErrNone );
    myState->SetSubscriptionStateL(
        MXIMPDataSubscriptionState::ESubscriptionInactive );
    
    iDataHost->SetPresenceBlockDataSubscriptionStateL(
        myState, myStatus );
    
    iDataHost->HandlePresenceBlockListL( coll );
    //Callback for subscription result
    CleanupStack::Pop( 3 ); // >> myState, myStatus, coll
    CleanupStack::PopAndDestroy( &blocked );
    DP_SDA("CPresencePluginAuthorization::SubscribeBlockList end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::UnSubscribeBlockListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::UnSubscribeBlockListL()
    {
    DP_SDA("CPresencePluginAuthorization::UnSubscribeBlockListL");
    
    MXIMPObjectCollection* coll =
        iConnObs.ObjectFactory().NewObjectCollectionLC();
    
    MXIMPDataSubscriptionState* myState =
        iConnObs.ObjectFactory().NewDataSubscriptionStateLC();
    
    MXIMPStatus* myStatus = iConnObs.ObjectFactory().NewStatusLC();
    
    myStatus->SetResultCode( KErrNone );
    myState->SetSubscriptionStateL(
        MXIMPDataSubscriptionState::ESubscriptionInactive );
    
    iDataHost->SetPresenceBlockDataSubscriptionStateL(
        myState, myStatus );
    
    iDataHost->HandlePresenceBlockListL( coll );
    
    CleanupStack::Pop( 3 );
    
    CompleteXIMPReq( KErrNone );
    
    DP_SDA("CPresencePluginAuthorization::UnSubscribeBlockListL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::ReturnListSubsState()
// ---------------------------------------------------------------------------
//
TBool CPresencePluginAuthorization::ReturnListSubsState()
    {
    return iSubscribed;
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::PluginData()
// ---------------------------------------------------------------------------
//
CPresencePluginData& CPresencePluginAuthorization::PluginData()
    {
    return *iPresenceData;
    }

// --------------------------------------------------------------------------- 
// CPresencePluginAuthorization::HandleFullWinfoNotificationL 
// --------------------------------------------------------------------------- 
// 
void CPresencePluginAuthorization::HandleFullWinfoNotificationL( 
    MSimpleWinfo& aWinfo )
    {
    using namespace NPresencePlugin::NPresence;
    
    RPointerArray<MSimpleElement> elems;
    CleanupClosePushL( elems );
    TInt err = aWinfo.SimpleElementsL( elems );
    User::LeaveIfError( err );
    TInt count = elems.Count();
    
    MSimpleElement* elem = elems[0];
    TPtrC8 p8 = elem->LocalName();
    err = p8.CompareF( KPresenceWatcherList8 ); //watcher-list
    DP_SDA2("HandleFullWinfoNotificationL leave if watcherlist error %d", err);
    User::LeaveIfError( err );
    
    err = elem->SimpleElementsL( elems );
    DP_SDA2("HandleFullWinfoNotificationL error %d", err);
    User::LeaveIfError( err );
    
    count = elems.Count();
    DP_SDA("HandleFullWinfoNotificationL watcher list is valid");
    
    // Search watchers who are pending (status = pending)
    HBufC* nodeContent = NULL;
    MXIMPObjectCollection *pendings =
        iConnObs.ObjectFactory().NewObjectCollectionLC();
    
    TBool pendingFound(EFalse);
    
    for ( TInt i = 0; i < count; i++ )
        {
        elem = elems[i];
        p8.Set( elem->LocalName());
        if (!( p8.CompareF( KPresenceWatcher8 )))
            {
            const TDesC8* pp8 = elem->AttrValue( KPresenceStatus8 );
            if ( pp8 && !pp8->CompareF( KPresencePending8 ))
                {
                DP_SDA("HandleFullWinfoNotificationL least one pending");
                // Build collection of grant requests
                // Find the child node containing the SIP entity
                nodeContent = elem->ContentUnicodeL();
                CleanupStack::PushL( nodeContent );
                // create the collection entry here 
                MPresenceGrantRequestInfo* grInfo =
                   iConnObs.PresenceObjectFactoryOwn().
                       NewPresenceGrantRequestInfoLC();
                MXIMPIdentity* identity =
                    iConnObs.ObjectFactory().NewIdentityLC();
                
                // Remove prefix to keep client side protocol agnostic
                HBufC16* withoutPrefix = iPresenceData->RemovePrefixLC( 
                    *nodeContent );
                identity->SetIdentityL( *withoutPrefix );
                CleanupStack::PopAndDestroy( withoutPrefix );
                
                grInfo->SetRequestorIdL( identity ); // ownership is taken
                
                pendings->AddObjectL( grInfo ); // ownership is taken
                pendingFound = ETrue;
                CleanupStack::Pop( 2 ); //grInfo, identity
                CleanupStack::PopAndDestroy( nodeContent );
                }
            }
        }
    
    if ( pendingFound )
        {
        DP_SDA("HandleFullWinfoNotificationL send pending");
        iXIMPIdOwn = iDataHost->HandlePresenceGrantRequestListL( pendings );
        CleanupStack::Pop();    // >> pendings
        }
    else
        {
        DP_SDA("HandleFullWinfoNotificationL pop pending");
        CleanupStack::PopAndDestroy( ); // >> pendings
        }
    
    CleanupStack::PopAndDestroy( &elems );
    DP_SDA("CPresencePluginAuthorization::HandleFullWinfoNotificationL END");
    } 
 
// --------------------------------------------------------------------------- 
// CPresencePluginAuthorization::HandlePartialWinfoNotificationL 
// --------------------------------------------------------------------------- 
// 
void CPresencePluginAuthorization::HandlePartialWinfoNotificationL( 
    MSimpleWinfo& aWinfo )
    {
    DP_SDA("CPresencePluginAuthorization::HandlePartialWinfoNotificationL");
    using namespace NPresencePlugin::NPresence;
    
    RPointerArray<MSimpleElement> elems;
    CleanupClosePushL( elems );
    TInt err = aWinfo.SimpleElementsL( elems );
    User::LeaveIfError( err );
    TInt count = elems.Count();
    
    MSimpleElement* elem = elems[0];
    TPtrC8 p8 = elem->LocalName();
    err = p8.CompareF( KPresenceWatcherList8 );
    User::LeaveIfError( err );
    
    err = elem->SimpleElementsL( elems );
    User::LeaveIfError( err );
    count = elems.Count();
    
    HBufC* nodeContent = NULL;
    
    for ( TInt i = 0; i < count; i++ )
        {
        elem = elems[i]; 
        p8.Set( elem->LocalName());
        if (!( p8.CompareF( KPresenceWatcher8 )))
            { 
            const TDesC8* pp8 = elem->AttrValue( KPresenceStatus8 );
            if ( pp8 && !pp8->CompareF( KPresencePending8 ))
                {
                nodeContent = elem->ContentUnicodeL();
                CleanupStack::PushL( nodeContent );
                
                // Remove prefix to keep client side protocol agnostic
                HBufC16* withoutPrefix = iPresenceData->RemovePrefixLC( 
                    *nodeContent );
                CallHandlePresenceGrantRequestReceivedL( *withoutPrefix ); 
                CleanupStack::PopAndDestroy( withoutPrefix );
                CleanupStack::PopAndDestroy( nodeContent ); 
                } 
            else if ( pp8 && !pp8->CompareF( KPresenceTerminated8 )) 
                {
                nodeContent = elem->ContentUnicodeL(); 
                CleanupStack::PushL( nodeContent ); 

                // Remove prefix to keep client side protocol agnostic
                HBufC16* withoutPrefix = iPresenceData->RemovePrefixLC( 
                    *nodeContent );
                CallHandlePresenceGrantRequestObsoletedL( *withoutPrefix ); 
                CleanupStack::PopAndDestroy( withoutPrefix );
                CleanupStack::PopAndDestroy( nodeContent ); 
                }
            else if ( pp8 && !pp8->CompareF( KPresenceActive8 ))
                {
                DP_SDA( "CPresencePluginAuthorization::HandlePartialWinfoNotificationL presence active" );
                
                nodeContent = elem->ContentUnicodeL();
                CleanupStack::PushL( nodeContent );
                TInt pos( 0 );
                if ( iConnObs.SubscribedContacts()->
                    GetVirtualIdentityArray()->Find( *nodeContent, pos ) == 0  )
                    {
                    HBufC8* buffer = HBufC8::NewLC( nodeContent->Length() );
                    buffer->Des().Copy( *nodeContent );
                    if ( !iConnObs.WatcherHandlerL()->MatchWatcherId( *buffer ) )
                        {
                        DP_SDA( "CPresencePluginAuthorization::HandlePartialWinfoNotificationL -subscribe presence" );
                        iConnObs.WatcherHandlerL()->SubscribeL( *nodeContent );
                        }
                    CleanupStack::PopAndDestroy( buffer );
                    }
                CleanupStack::PopAndDestroy( nodeContent );
                }
            }
        }
    CleanupStack::PopAndDestroy( &elems );
    DP_SDA("CPresencePluginAuthorization::HandlePartialWinfoNotificationL END");
    } 

// --------------------------------------------------------------------------- 
// CPresencePluginAuthorization::CallHandlePresenceGrantRequestReceivedL 
// --------------------------------------------------------------------------- 
// 
void CPresencePluginAuthorization::CallHandlePresenceGrantRequestReceivedL(
    const TDesC& aUserId ) 
    {
    DP_SDA("PluginAuthorization::CallHandlePresenceGrantRequestReceivedL");
    // create the collection entry here 
    MPresenceGrantRequestInfo* grInfo = 
        iConnObs.PresenceObjectFactoryOwn().NewPresenceGrantRequestInfoLC();
    MXIMPIdentity* identity = iConnObs.ObjectFactory().NewIdentityLC();
    identity->SetIdentityL( aUserId ); 
    grInfo->SetRequestorIdL( identity ); // ownership is taken
    CleanupStack::Pop( ); // -identity 
    
    iDataHost->HandlePresenceGrantRequestReceivedL( grInfo );//ownership taken 
    CleanupStack::Pop( );           // -grInfo
    DP_SDA("PluginAuthorization::CallHandlePresenceGrantRequestReceivedL END");
    } 

// --------------------------------------------------------------------------- 
// CPresencePluginAuthorization::CallHandlePresenceGrantRequestObsoletedL 
// --------------------------------------------------------------------------- 
// 
void CPresencePluginAuthorization::CallHandlePresenceGrantRequestObsoletedL(
    const TDesC& aUserId )
    {
    DP_SDA("PluginAuthorization::CallHandlePresenceGrantRequestObsoletedL");
    MXIMPIdentity* identity = iConnObs.ObjectFactory().NewIdentityLC(); 
    identity->SetIdentityL( aUserId ); 
    iDataHost->HandlePresenceGrantRequestObsoletedL( identity );//taken 
    CleanupStack::Pop( );// >> identity
    DP_SDA("PluginAuthorization::CallHandlePresenceGrantRequestObsoletedL END");
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::GrantPresenceForPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::GrantPresenceForPresentityL()
    {
    iXdmUtils->AddEntityToGrantedL( 
        iPresIdentity->Des(), iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::WithdrawFromPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::WithdrawFromPresentityL()
    {
    iXdmUtils->RemoveEntityFromGrantedL( 
        iPresIdentity->Des(),iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::UnsubscribePresentityPresenceL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::UnsubscribePresentityPresenceL()
    {
    DP_SDA( "CPresencePluginAuthorization::UnsubscribePresentityPresence" );
    
    MXIMPIdentity* identity =
        iConnObs.ObjectFactory().NewIdentityLC();
    identity->SetIdentityL( iPresIdentity->Des() );
    iConnObs.WatcherHandlerL()->
        DoPerformUnsubscribePresentityPresenceL( *identity, iStatus );
    CleanupStack::PopAndDestroy(); // identity
    SetActive();
    
    DP_SDA( "CPresencePluginAuthorization::UnsubscribePresentityPresence -Exit" );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::BlockPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::BlockPresentityL()
    {
    DP_SDA( "CPresencePluginAuthorization::BlockPresentityL" );
    // add entity to block list
    iXdmUtils->AddEntityToBlockedL( 
        iPresIdentity->Des(), iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::BlockPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::SetPresentityBlockedToXIMPL()
    {
    DP_SDA( "CPresencePluginAuthorization::SetPresentityBlockedToXIMPL" );
    
    MPresenceBlockInfo* bInfo =
        iConnObs.PresenceObjectFactoryOwn().NewPresenceBlockInfoLC();
    MXIMPIdentity* identity = iConnObs.ObjectFactory().NewIdentityLC();
    HBufC* withoutPrefix = iPresenceData->RemovePrefixLC( *iPresIdentity );
    identity->SetIdentityL( *withoutPrefix ); 
    bInfo->SetBlockedEntityIdL( identity ); // ownership is taken
    // Write blocked state into the presence cache
    TBuf<20> buf;
    buf.Copy( KBlockedExtensionValue );
    iPresenceData->WriteStatusToCacheL(
        *withoutPrefix, 
        MPresenceBuddyInfo2::EUnknownAvailability,
        buf,
        KNullDesC() );
    CleanupStack::PopAndDestroy( withoutPrefix );
    iDataHost->HandlePresenceBlockedL( bInfo );
    CleanupStack::Pop();// >> identity
    CleanupStack::Pop();// >> bInfo
    
    DP_SDA( "CPresencePluginAuthorization::SetPresentityBlockedToXIMPL -Exit" );
    }

// ---------------------------------------------------------------------------
// CPresencePluginAuthorization::UnblockPresentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginAuthorization::UnblockPresentityL()
    {
    DP_SDA( "CPresencePluginAuthorization::UnblockPresentityL" );
     
    //remove entity from blocked list 
    iXdmUtils->RemoveEntityFromBlockedL( 
        iPresIdentity->Des(), iStatus );
    //remove enity from ximp blocked too
    MXIMPIdentity* identity = iConnObs.ObjectFactory().NewIdentityLC();
    HBufC* withoutPrefix = iPresenceData->RemovePrefixLC( *iPresIdentity );
    identity->SetIdentityL( *withoutPrefix );
    CleanupStack::PopAndDestroy( withoutPrefix );
    iDataHost->HandlePresenceBlockCanceledL( identity );
    CleanupStack::Pop();// >> identity
    iAuthState = EStateDoUnBlock;
    SetActive();
    
    DP_SDA( "CPresencePluginAuthorization::UnblockPresentityL -Exit" );
    }

// End of file
