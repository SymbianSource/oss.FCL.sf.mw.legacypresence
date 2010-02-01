/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <ximpdatasubscriptionstate.h>
#include <protocolpresentitygroupsdatahost.h>
#include <protocolpresencedatahost.h>
#include <ximpobjectfactory.h>
#include <presenceobjectfactory.h>
#include <ximpstatus.h>
#include <ximpidentity.h>
#include <ximpobjectcollection.h>
#include <xdmerrors.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <utf.h>
#include <avabilitytext.h>

#include "mpresencepluginconnectionobs.h"
#include "presenceplugingroup.h"
#include "presencepluginxdmutils.h"
#include "presenceplugincommon.h"
#include "presencepluginvirtualgroup.h"
#include "presenceplugindata.h"
#include "presencepluginwatcher.h"
#include "presencepluginauthorization.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginGroups::CPresencePluginGroups
// ---------------------------------------------------------------------------
//
CPresencePluginGroups::CPresencePluginGroups(
    MPresencePluginConnectionObs& aObs,
    CPresencePluginVirtualGroup* aSubscribedBuddies,
    CPresencePluginData* aPresenceData ):
  	CActive( CActive::EPriorityStandard ),
  	iConnObs(aObs),
  	iOperation( ENoOperation),
  	iState( EPluginIdle ), iCompleted( ETrue ),
  	iSubscribedBuddies( aSubscribedBuddies ),
  	iPresenceData( aPresenceData )
    {
    CActiveScheduler::Add( this );
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginGroups::NewL
// ---------------------------------------------------------------------------
//
CPresencePluginGroups* CPresencePluginGroups::NewL(
    MPresencePluginConnectionObs& aObs,
    CPresencePluginVirtualGroup* aSubscribedBuddies,
    CPresencePluginData* aPresenceData )
    {
    CPresencePluginGroups* self =
        CPresencePluginGroups::NewLC( 
             aObs,
             aSubscribedBuddies,
             aPresenceData );    
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::NewLC
// ---------------------------------------------------------------------------
//
CPresencePluginGroups* CPresencePluginGroups::NewLC(
    MPresencePluginConnectionObs& aObs,
    CPresencePluginVirtualGroup* aSubscribedBuddies,
    CPresencePluginData* aPresenceData )
    {
    CPresencePluginGroups* self =
        new( ELeave ) CPresencePluginGroups( 
             aObs,
             aSubscribedBuddies,
             aPresenceData );
    CleanupStack::PushL( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginGroups::~CPresencePluginGroups
// ---------------------------------------------------------------------------
//
CPresencePluginGroups::~CPresencePluginGroups()
    {
    DP_SDA("CPresencePluginGroups::~CPresencePluginGroups");
    Cancel();
    delete iPresIdentity;
    delete iDisplayName;
    DP_SDA("CPresencePluginGroups::~CPresencePluginGroups end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::DoSubscribePresentityGroupListL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DoSubscribePresentityGroupListL(
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginGroups::DoSubscribePresentityGroupListL");
    iCompleted = EFalse;
    // List of lists under buddylist
    StartXdmOperationL( aReqId, EGetListOfLists );
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::DoUnsubscribePresentityGroupListL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DoUnsubscribePresentityGroupListL(
    TXIMPRequestId /*aReqId*/ )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::DoCreatePresentityGroupL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DoCreatePresentityGroupL(
    const MXIMPIdentity& /*aGroupId*/,
    const TDesC16& /*aDisplayName*/,
    TXIMPRequestId /*aReqId*/ )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::DoDeletePresentityGroupL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DoDeletePresentityGroupL(
    const MXIMPIdentity& /*aGroupId*/,
    TXIMPRequestId /*aReqId*/ )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::DoUpdatePresentityGroupDisplayNameL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DoUpdatePresentityGroupDisplayNameL(
    const MXIMPIdentity& /*aGroupId*/,
    const TDesC16& /*aDisplayName*/,
    TXIMPRequestId /*aReqId*/ )
    {
    // Notice: later
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::DoSubscribePresentityGroupContentL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DoSubscribePresentityGroupContentL(
    const MXIMPIdentity& aGroupId,
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginGroups::DoSubscribePresentityGroupContentL");
    iCompleted = EFalse;
    // List of lists under buddylist
    StartXdmOperationL( aGroupId, aReqId, EGetListContent );
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::DoUnsubscribePresentityGroupContentL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DoUnsubscribePresentityGroupContentL(
    const MXIMPIdentity& /*aGroupId*/,
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginGroups::DoUnsubscribePresentityGroupContentL");
    
    iXIMPId = aReqId;
    iConnObs.WatcherHandlerL()->UnSubscribeAllL();    
    iCompleted = EFalse;
    CompleteXIMPReq( KErrNone );
    DP_SDA("CPresencePluginGroups::DoUnsubscribePresentityGroupContentL out");
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::DoAddPresentityGroupMemberL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DoAddPresentityGroupMemberL(
    const MXIMPIdentity& aGroupId,
    const MXIMPIdentity& aMemberId,
    const TDesC16& /*aMemberDisplayName*/,
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginGroups::DoAddPresentityGroupMemberL");
    __ASSERT_ALWAYS( !IsActive(), User::Leave( KErrNotReady ) );
    
    SetPresIdentityL( aMemberId, ETrue );
    
    iCompleted = EFalse;
    iXIMPId = aReqId;
        
    if( !aGroupId.Identity().Compare( KPresenceBuddyList ) )
        {
        DP_SDA("CPresencePluginGroups::DoAddPresentityGroupMemberL - buddylist");
        iOperation = EAddPresentityGroupMember;
        
        // write pending to presence cache
        TBuf<20> buf;
        buf.Copy( KPendingRequestExtensionValue );
        
        iPresenceData->WriteStatusToCacheL( aMemberId.Identity(), 
            MPresenceBuddyInfo2::ENotAvailable,
            buf,
            KNullDesC() ); 
        
        iConnObs.WatcherHandlerL()->DoPerformSubscribePresentityPresenceL( 
            aMemberId, iStatus );
        SetActive();
        }
    else if ( !aGroupId.Identity().Compare( KPresenceBlockedList ) )
        {
        DP_SDA("CPresencePluginGroups::DoAddPresentityGroupMemberL - blockedlist");
        iOperation = EBlockPresentityGroupMember;
        iConnObs.InternalPresenceAuthorization().
            DoPerformBlockPresenceForPresentityL( aMemberId, iStatus );
        SetActive();
        }
    else
        {
        DP_SDA("CPresencePluginGroups::DoAddPresentityGroupMemberL - unknown, leave");
        User::Leave( KErrNotSupported );
        }
    
    DP_SDA("CPresencePluginGroups::DoAddPresentityGroupMemberL out");
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::DoRemovePresentityGroupMemberL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DoRemovePresentityGroupMemberL(
    const MXIMPIdentity& aGroupId,
    const MXIMPIdentity& aMemberId,
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginGroups::DoRemovePresentityGroupMemberL");
    __ASSERT_ALWAYS( !IsActive(), User::Leave( KErrNotReady ) );

    SetPresIdentityL( aMemberId, ETrue );
    
    iCompleted = EFalse;
    iXIMPId = aReqId;
    iOperation = ERemovePresentityGroupMember;
    
    if( !aGroupId.Identity().Compare( KPresenceBuddyList ) )
        {
        DP_SDA(" DoRemovePresentityGroupMemberL - buddylist");
        iConnObs.WatcherHandlerL()->DoPerformUnsubscribePresentityPresenceL( 
            aMemberId, iStatus );
        SetActive();
        }
    else if ( !aGroupId.Identity().Compare( KPresenceBlockedList ) )
        {
        DP_SDA(" DoRemovePresentityGroupMemberL - blockedlist");
        iOperation = EUnblockPresentityGroupMember;
        iConnObs.InternalPresenceAuthorization().
            DoPerformCancelPresenceBlockFromPresentityL( aMemberId, iStatus );
        SetActive();
        }
    else
        {
        DP_SDA("DoRemovePresentityGroupMemberL - unknown, leave");
        User::Leave( KErrNotSupported );
        }
        
    DP_SDA("CPresencePluginGroups::DoRemovePresentityGroupMemberL out");    
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::DoUpdatePresentityGroupMemberDisplayNameL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DoUpdatePresentityGroupMemberDisplayNameL(
    const MXIMPIdentity& /*aGroupId*/,
    const MXIMPIdentity& /*aMemberId*/,
    const TDesC16& /*aMemberDisplayName*/,
    TXIMPRequestId /*aReqId*/ )
    {
    DP_SDA("CPresencePluginGroups::DoUpdatePresentityGroupMemberDisplayNameL");
    // Notice: Later
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::DoCancel()
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DoCancel(  )
    {
    if ( iOperation != ENoOperation )
        {
        iXdmUtils->Cancel();
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::RunL()
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::RunL()
    {
    DP_SDA("CPresencePluginGroups::RunL");
    TInt myStatus = iStatus.Int();
    DP_SDA2("CPresencePluginGroups::RunL myStatus %d", myStatus );
    
    if ( !myStatus && !iCompleted )
        {
        DP_SDA("CPresencePluginGroups::RunL !myStatus");
        // OK response
        CallActualXdmOperationL( myStatus );
        }
    else if ( !myStatus && iCompleted )
        {
        DP_SDA("CPresencePluginGroups::RunL SEND COMPLETE");
        CompleteXIMPReq( myStatus );
        }
    else
        {
        if ( iOperation == EDeletePresentityGroup && iState ==
        	EPluginCommitRls )
            {
            DP_SDA("CPresencePluginGroups::RunL CallActualXdmOperation");
            CallActualXdmOperationL( myStatus );
            }
        else
            {
            DP_SDA("CPresencePluginGroups::RunL SEND COMPLETE 2");
            // Other errors terminated the show
            CompleteXIMPReq( myStatus );
            }
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::CallActualXdmOperationL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::CallActualXdmOperationL( TInt aCompleteStatus )
    {
    DP_SDA("CPresencePluginGroups::CallActualXdmOperationL");
        
    DP_SDA2("CallActualXdmOperationL iOperation %d", iOperation);
    switch ( iOperation )
        {
        case EGetListOfLists:
            {
            DP_SDA("PluginGroups::CallActualXdmOperationL EGetListOfLists");
            iOperation = EGetListOfLists;
            GetListOfListsL();
            }
            break;
        
        case EGetListContent:
            {
            DP_SDA("PluginGroups::CallActualXdmOperationL EGetListContent");
            iOperation = EGetListContent;
            GetListContentL();
            }
            break;
            
        case EAddPresentityGroupMember:
            {
            DP_SDA(" CallActualXdmOperationL EAddPresentityGroupMember");
            
            iOperation = EGrantPresentityGroupMember;
            MXIMPIdentity* newIdentity = 
                iConnObs.ObjectFactory().NewIdentityLC();
            newIdentity->SetIdentityL( *iPresIdentity );
            
            iConnObs.InternalPresenceAuthorization().
                DoPerformGrantPresenceForPresentityL( *newIdentity, iStatus );
            CleanupStack::PopAndDestroy(); // >> newIdentity
            SetActive();
            }
            break;
            
        case ERemovePresentityGroupMember:
            {
            DP_SDA(" CallActualXdmOperationL ERemovePresentityGroupMember");
            iOperation = EWithdrawGrantFromMember;
            MXIMPIdentity* newIdentity = iConnObs.ObjectFactory().NewIdentityLC();
            newIdentity->SetIdentityL( *iPresIdentity );
            iConnObs.InternalPresenceAuthorization().
                DoPerformWithdrawPresGrantFromPresentityL( 
                    *newIdentity, iStatus );
            CleanupStack::PopAndDestroy(); // >> newIdentity
            SetActive();
            }
            break;
            
        case EGrantPresentityGroupMember:
            {
            DP_SDA(" CallActualXdmOperationL EGrantPresentityGroupMember");
            CompleteXIMPReq( aCompleteStatus );
            }
            break;
            
        case EWithdrawGrantFromMember:
            {
            DP_SDA(" CallActualXdmOperationL EWithdrawGrantFromMember");
            if ( !aCompleteStatus )
                {
                DP_SDA(" -> succesfully unblocked, store to cache");
                DeletePersonCacheL();                
                DP_SDA(" -> store info to cache done");                
                }            
            CompleteXIMPReq( aCompleteStatus );
            }
            break;
            
        case EBlockPresentityGroupMember:
            {
            DP_SDA(" CallActualXdmOperationL EBlockPresentityGroupMember");
            if ( !aCompleteStatus )
                {
                DP_SDA(" -> succesfully blocked, store to cache");

                DP_SDA(" -> strip prefix from uri");
                HBufC* withoutPrefix = iPresenceData->RemovePrefixLC( 
                    *iPresIdentity );
                DP_SDA(" -> store info to cache");
                
                TBuf<20> buf;
                buf.Copy( KBlockedExtensionValue );
                                
                iPresenceData->WriteStatusToCacheL( *withoutPrefix, 
                    MPresenceBuddyInfo2::EUnknownAvailability,
                    buf,
                    KNullDesC() );
                
                CleanupStack::PopAndDestroy( withoutPrefix );
                DP_SDA(" -> store info to cache done");
                }
            CompleteXIMPReq( aCompleteStatus );
            }
            break;
            
        case EUnblockPresentityGroupMember:
            {
            DP_SDA(" CallActualXdmOperationL EUnblockPresentityGroupMember");
            if ( !aCompleteStatus )
                {
                DP_SDA(" -> succesfully unblocked, store to cache");
                DeletePersonCacheL();                
                DP_SDA(" -> store info to cache done");                
                }            
            CompleteXIMPReq( aCompleteStatus );
            }
            break;
            
        default:
            User::Leave( KErrNotSupported );
            break;
        }   
    DP_SDA("CPresencePluginGroups::CallActualXdmOperationL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::RunError
// ---------------------------------------------------------------------------
//
TInt CPresencePluginGroups::RunError( TInt aError )
    {
    DP_SDA("CPresencePluginGroups::RunError SEND COMPLETE");
    // complete the open request
    CompleteXIMPReq( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CPresencePluginGroups::GetInterface(
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
        User::Panic( KPluginName, KErrExtensionNotSupported );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CPresencePluginGroups::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions ) const
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        const MProtocolPresentityGroups* myIf = this;
        return myIf;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( KPluginName, KErrExtensionNotSupported );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::GetInterfaceId()
// ---------------------------------------------------------------------------
//
TInt32 CPresencePluginGroups::GetInterfaceId() const
    {
    return MProtocolPresentityGroups::KInterfaceId;
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::SetPresIdentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::SetPresIdentityL(
     const MXIMPIdentity& aPresentityId,
     TBool aFormatUri )
    {
    DP_SDA2("CPresencePluginGroups::SetPresIdentityL: %S", 
        &aPresentityId.Identity() ); 
     
    delete iPresIdentity;
    iPresIdentity = NULL;
     
    if ( aFormatUri )
        {
        DP_SDA("CPresencePluginGroups::SetPresIdentityL, format uri");

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
        DP_SDA("CPresencePluginGroups::SetPresIdentityL, use as it is");
        iPresIdentity = aPresentityId.Identity().AllocL();
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::StartXdmOperationL(
    TXIMPRequestId aReqId,
    TPluginGroupsOperation aOperation )
    {
    DP_SDA("StartXdmOperationL StartXdmOperationL two param");
    __ASSERT_DEBUG( !IsActive(), User::Leave( KErrCorrupt ) );
    iXIMPId = aReqId;
    iOperation = aOperation;
    StartXdmOperationL();
    DP_SDA("StartXdmOperationL StartXdmOperationL two param end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::StartXdmOperationL(
    const MXIMPIdentity& aId,
    TXIMPRequestId aReqId,
    TPluginGroupsOperation aOperation )
    {
    DP_SDA("StartXdmOperationL StartXdmOperationL three param");
    __ASSERT_DEBUG( !IsActive(), User::Leave( KErrCorrupt ) );
    SetPresIdentityL( aId, EFalse );
    iXIMPId = aReqId;
    iOperation = aOperation;
    StartXdmOperationL();
    }
  
// ---------------------------------------------------------------------------
// CPresencePluginGroups::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::StartXdmOperationL()
    {
    if ( !iXdmUtils )
        {
        DP_SDA("CPresencePluginGroups::StartXdmOperationL Get xdmUtils");
        iXdmUtils = iConnObs.XdmUtilsL();
        }
    DP_SDA("CPresencePluginGroups::StartXdmOperationL StartXdmOperationL");
    
    iXdmUtils->InitializeXdmL( iStatus );
    iState = EPluginInitXdm;
    DP_SDA("CPresencePluginGroups::StartXdmOperationL SetActive");
    SetActive();
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::GetListOfListsL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::GetListOfListsL()
    {
    DP_SDA("CPresencePluginGroups::GetListOfListsL");

    RPointerArray<MXIMPIdentity> subscribed;
    CleanupClosePushL( subscribed );

    iXdmUtils->SubscribeBuddyListL( subscribed );
	DP_SDA("CPresencePluginGroups::GetListOfListsL SubscribeBudyList Ready");
	
    MXIMPObjectCollection* coll =
        iConnObs.ObjectFactory().NewObjectCollectionLC();    // << coll
        
    TInt count = subscribed.Count();
    DP_SDA2("CPresencePluginGroups::GetListOfListsL soul count %d", count);
    
    for ( TInt i = 0; i < count; i++ )
        {
        MXIMPIdentity* currId = subscribed[i];
        MPresentityGroupInfo* sInfo =
            iConnObs.PresenceObjectFactoryOwn().NewPresentityGroupInfoLC();
        sInfo->SetGroupIdL( currId ); // ownership is taken
        sInfo->SetGroupDisplayNameL( currId->Identity() );
        
        coll->AddObjectL( sInfo );      // ownership is taken
        CleanupStack::Pop();                                // >> bInfo
        }
    DP_SDA("SubscribeBudyList For ready SEND COMPLETE continue");    
     
    CompleteXIMPReq( KErrNone );
    
     // Callback for subscription state (terminated).
    MXIMPDataSubscriptionState* myState =
        iConnObs.ObjectFactory().NewDataSubscriptionStateLC();
        
    MXIMPStatus* myStatus = iConnObs.ObjectFactory().NewStatusLC();
    
    // Notice: consider XIMP error codes
    myStatus->SetResultCode( KErrNone );
    myState->SetSubscriptionStateL(
        MXIMPDataSubscriptionState::ESubscriptionInactive );
        
    MProtocolPresentityGroupsDataHost& dataHost =
         iConnObs.ProtocolPresenceHost().GroupsDataHost();
    
    dataHost.SetPresentityGroupListDataSubscriptionStateL(
        myState, myStatus );
        
    dataHost.HandlePresentityGroupListL( coll );    

    CleanupStack::Pop( 3 ); // >> myState, myStatus, coll
   	CleanupStack::Pop( &subscribed );
   	DP_SDA("CPresencePluginGroups::GetListOfListsL end"); 
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::GetListContentL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::GetListContentL()
    {
    DP_SDA("CPresencePluginGroups::GetListContentL");
    
    if( !iPresIdentity->Compare( KPresenceBuddyList ) )
        {
        DP_SDA("CPresencePluginGroups::GetListContentL - get buddies");
        DoGetSubscribedBuddyListL();
        
        // Consider waiting complete for each subscribe before completing
        CompleteXIMPReq( KErrNone ); 
        }
    else if( !iPresIdentity->Compare( KPresenceSubcribedBuddys ) )
    	{
    	//Subscribe subscribed buddyes virtual group
    	DP_SDA("GetListContentL KPresenceSubcribedBuddys");
    	DoGetSubscribedBuddyListL();
    	}
    else
    	{
    	DP_SDA("CPresencePluginGroups::GetListContentL Wrong list name");	
        User::Leave( KErrNotFound );
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::DoGetBuddyListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DoGetBuddyListL()
    {
    DP_SDA("CPresencePluginGroups::DoGetBuddyListL KPresenceBuddyList");
    MXIMPObjectFactory& myFactory = iConnObs.ObjectFactory();
    MProtocolPresentityGroupsDataHost& dataHost =
        iConnObs.ProtocolPresenceHost().GroupsDataHost();
    
    MXIMPObjectCollection* entities =
    	myFactory.NewObjectCollectionLC();          // << entities
    iXdmUtils->GetEntitiesInListL( iPresIdentity->Des(), *entities );
    DP_SDA("DoGetBuddyListL entities get ready SEND COMPLETE");
    
    // callback for data
    DP_SDA("CPresencePluginGroups::DoGetBuddyListL callback data");
    MXIMPIdentity* id = myFactory.NewIdentityLC();  // << id
    id->SetIdentityL( iPresIdentity->Des() );
    dataHost.HandlePresentityGroupContentL( id, entities );
    CleanupStack::Pop();// >> id
    CleanupStack::Pop();// >> entities
    DP_SDA("CPresencePluginGroups::DoGetBuddyListL callback ready");

    // Callback for subscription state (terminated).
    DP_SDA("CPresencePluginGroups::DoGetBuddyListL terminated ");
    id = myFactory.NewIdentityLC();  // << id
    id->SetIdentityL( iPresIdentity->Des() );
    MXIMPDataSubscriptionState* myState =
    myFactory.NewDataSubscriptionStateLC();
    MXIMPStatus* myStatus = myFactory.NewStatusLC();
    // Notice: consider XIMP error codes
    myStatus->SetResultCode( KErrNone );
    myState->SetSubscriptionStateL(
    		MXIMPDataSubscriptionState::ESubscriptionInactive );
    
    dataHost.SetPresentityGroupContentDataSubscriptionStateL(
    	id, myState, myStatus );
  
    DP_SDA("CPresencePluginGroups::DoGetBuddyListL Pop");
	CleanupStack::Pop( 3 );
	
    DP_SDA("CPresencePluginGroups::DoGetBuddyListL end");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginGroups::DoGetSubscribedBuddyListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DoGetSubscribedBuddyListL()
    {
    DP_SDA("CPresencePluginGroups::DoGetSubscribedBuddyListL");
    MXIMPObjectFactory& myFactory = iConnObs.ObjectFactory();
    MProtocolPresentityGroupsDataHost& dataHost =
        iConnObs.ProtocolPresenceHost().GroupsDataHost();
    
    // Get internal list content
    MXIMPObjectCollection* entities =
    	myFactory.NewObjectCollectionLC();          // << entities
    iXdmUtils->GetEntitiesInListL( iPresIdentity->Des(), *entities );
    DP_SDA("DoGetBuddyListL entities get ready SEND COMPLETE");
    CleanupStack::PopAndDestroy();	// entities
    
    // start getting virtualEntities
    MXIMPObjectCollection* virtualEntities =
        myFactory.NewObjectCollectionLC();// << virtualEntities

    iSubscribedBuddies->GetVirtualEntitiesL( *virtualEntities );
    
    DP_SDA("CPresencePluginGroups::DoGetSubscribedBuddyListL - subscribe all");
    iConnObs.WatcherHandlerL()->SubscribeAllL();
    DP_SDA("CPresencePluginGroups::DoGetSubscribedBuddyListL - subscribe all ok");

    // callback for data
    DP_SDA("DoGetSubscribedBuddyListL callback data");
    MXIMPIdentity* id = myFactory.NewIdentityLC();  // << id
    id->SetIdentityL( iPresIdentity->Des() );
    dataHost.HandlePresentityGroupContentL( id, virtualEntities );
    CleanupStack::Pop();                            // >> id
    CleanupStack::Pop();                            // >> entities
    DP_SDA("DoGetSubscribedBuddyListL callback ready");

    // Callback for subscription state (terminated).
    DP_SDA("CPresencePluginGroups::DoGetSubscribedBuddyListL terminated");
    id = myFactory.NewIdentityLC();  // << id
    id->SetIdentityL( iPresIdentity->Des() );
    MXIMPDataSubscriptionState* myState =
    	myFactory.NewDataSubscriptionStateLC();
    MXIMPStatus* myStatus = myFactory.NewStatusLC();
    // Notice: consider XIMP error codes
    myStatus->SetResultCode( KErrNone );
    myState->SetSubscriptionStateL(
    		MXIMPDataSubscriptionState::ESubscriptionActive );
    dataHost.SetPresentityGroupContentDataSubscriptionStateL(
    	id, myState, myStatus );
    
	CleanupStack::Pop( 3 );
	
    DP_SDA("DoGetSubscribedBuddyListL - KPresenceOnlineBuddy case end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::CompleteXIMPReq
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::CompleteXIMPReq( TInt aStatus )
    {
    DP_SDA("CPresencePluginGroups::CompleteXIMPReq");
    if ( iCompleted  )
        {
        DP_SDA("CPresencePluginGroups::CompleteXIMPReq return");
        return;
        }

    iCompleted = ETrue;
    iOperation = ENoOperation;
    iConnObs.CompleteReq( iXIMPId, aStatus );
    iXIMPId = TXIMPRequestId();
    DP_SDA("CPresencePluginGroups::CompleteXIMPReq end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginGroups::DeletePersonCacheL
// ---------------------------------------------------------------------------
//
void CPresencePluginGroups::DeletePersonCacheL()
    {
    DP_SDA("CPresencePluginGroups::DeletePersonCacheL");
    DP_SDA(" -> strip prefix from uri");
    HBufC* withoutPrefix = iPresenceData->RemovePrefixLC( *iPresIdentity );
    iPresenceData->DeletePresenceL( *withoutPrefix );
    CleanupStack::PopAndDestroy( withoutPrefix );
    DP_SDA("CPresencePluginGroups::DeletePersonCacheL out");
    }

// End of file
