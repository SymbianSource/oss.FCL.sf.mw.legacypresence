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
#include <e32svr.h>
#include <utf.h>
#include <s32strm.h>
#include <ximpprotocolconnectionhost.h>
#include <ximpidentity.h>
#include <ximperrors.hrh>
#include <msimpleconnection.h>
#include <protocolpresentitygroupsdatahost.h>
#include <protocolpresencedatahost.h>
#include <ximpobjectcollection.h>
#include <ximpobjectfactory.h>
#include <presenceobjectfactory.h>
#include <escapeutils.h>

#include "presencepluginvirtualgroup.h"
#include "mpresencepluginconnectionobs.h"
#include "presenceplugincommon.h"
#include "presencepluginwatcher.h"
#include "presencepluginentitywatcher.h"
#include "presencepluginxdmutils.h"
#include "presenceplugindata.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::CPresencePluginWatcher()
// ---------------------------------------------------------------------------
//
CPresencePluginWatcher::CPresencePluginWatcher(
    MPresencePluginConnectionObs& aObs,
    MSimpleConnection& aConn, CPresencePluginData* aPresenceData )
    : CActive( CActive::EPriorityStandard ),
    iConnObs(aObs), iConnection(aConn), iPresenceData( aPresenceData ),
    iXdmState( EPluginIdle ),iCompleted( ETrue ),
    iOperation( EPluginUndef )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::NewL()
// ---------------------------------------------------------------------------
//
CPresencePluginWatcher* CPresencePluginWatcher::NewL(
    MPresencePluginConnectionObs& aObs,
    MSimpleConnection& aConn,
    CPresencePluginData* aPresenceData )
    {
    DP_SDA("CPresencePluginWatcher::NewL");
    CPresencePluginWatcher* self =
        new( ELeave ) CPresencePluginWatcher( aObs, aConn, aPresenceData );
    CleanupStack::PushL( self );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::~CPresencePluginWatcher()
// ---------------------------------------------------------------------------
//
CPresencePluginWatcher::~CPresencePluginWatcher()
    {
    iWatchers.ResetAndDestroy();
    iWatchers.Close();
    iWatcherCandidates.ResetAndDestroy();
    delete iPresIdentity;
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginWatcher::DoSubscribePresentityPresenceL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::DoSubscribePresentityPresenceL(
    const MXIMPIdentity& aPresentityId,
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginWatcher::DoSubscribePresentityPresenceL");
    DP_SDA2(" DoSubscribePresentityPresenceL - identity: %S", 
        &aPresentityId.Identity() );

    iOperation = EPluginSubscribeSingle;
    iXdmState = EPluginIdle;    
   
    SetPresIdentityL( aPresentityId, ETrue );
    
    //Saving reguestID
    iXIMPId = aReqId;    
    iCompleted = EFalse;
    
    HBufC8* pres8 = NULL;
    pres8 =
        CnvUtfConverter::ConvertFromUnicodeToUtf8L( iPresIdentity->Des() );
    CleanupStack::PushL( pres8 ); // << pres8
        
    CPresencePluginEntityWatcher* watcher =
        MatchWatcherL( pres8->Des(), ETrue );      
    watcher->StartSubscribeL( pres8->Des() );
    CleanupStack::PopAndDestroy( pres8 );  // >> pres8
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginWatcher::DoPerformSubscribePresentityPresenceL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::DoPerformSubscribePresentityPresenceL(
    const MXIMPIdentity& aPresentityId,
    TRequestStatus& aStatus )
    {        
    DP_SDA( "CPresencePluginWatcher::DoPerformSubscribePresentityPresenceL ");
    DP_SDA( " -> From list operation");
    
    iOperation = EPluginSubscribeByAdding;
    iXdmState = EPluginIdle;    
   
    SetPresIdentityL( aPresentityId, ETrue );
    
    iCompleted = EFalse;
    
    HBufC8* pres8 = NULL;
    pres8 =
        CnvUtfConverter::ConvertFromUnicodeToUtf8L( iPresIdentity->Des() );
    CleanupStack::PushL( pres8 ); // << pres8
    
    // remove escapes
    HBufC8* encodedUsername = EscapeUtils::EscapeEncodeL( *pres8, EscapeUtils::EEscapeNormal );
    CleanupStack::PopAndDestroy( pres8 );
    CleanupStack::PushL( encodedUsername );
    
    CPresencePluginEntityWatcher* watcher =
        MatchWatcherL( encodedUsername->Des(), ETrue );      
    watcher->StartSubscribeL( encodedUsername->Des(), aStatus );
    CleanupStack::PopAndDestroy( encodedUsername );
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::DoUpdatePresentityPresenceSubscriptionPifL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::DoUpdatePresentityPresenceSubscriptionPifL(
    const MXIMPIdentity& /*aPresentityId*/,
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
    // Notice: later
    User::Leave( KXIMPErrServiceRequestTypeNotSupported );
    }             

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::DoUnsubscribePresentityPresenceL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::DoUnsubscribePresentityPresenceL(
    const MXIMPIdentity& aPresentityId,
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginWatcher::DoUnsubscribePresentityPresenceL");
    SetPresIdentityL( aPresentityId, ETrue );
    iXIMPId = aReqId;  
    iCompleted = EFalse;
    
    HBufC8* pres8 = NULL;
    pres8 =
        CnvUtfConverter::ConvertFromUnicodeToUtf8L( iPresIdentity->Des() );
    CleanupStack::PushL( pres8 ); // << pres8
            
    CPresencePluginEntityWatcher* watcher =
        MatchWatcherL( pres8->Des(), EFalse );
    if ( !watcher )
        {
        DP_SDA("DoUnsubscribePresentityPresenceL error");
        User::Leave( KErrNotFound ); // Notice: error code
        }
    else
        {
      	DP_SDA("DoUnsubscribePresentityPresenceL stop subscripe");
        watcher->StopSubscribeL();      
        }    
    CleanupStack::PopAndDestroy( pres8 );  // >> pres8   
    }             

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::DoPerformUnsubscribePresentityPresenceL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::DoPerformUnsubscribePresentityPresenceL(
    const MXIMPIdentity& aPresentityId,
    TRequestStatus& aStatus )
    {
    DP_SDA("CPresencePluginWatcher::DoPerformUnsubscribePresentityPresenceL");
    
    SetPresIdentityL( aPresentityId, ETrue );
    iCompleted = EFalse;
    
    iOperation = EPluginUnSubscribeByRemoving;
    
    HBufC8* pres8 = NULL;
    pres8 =
        CnvUtfConverter::ConvertFromUnicodeToUtf8L( iPresIdentity->Des() );
    CleanupStack::PushL( pres8 ); // << pres8
            
    CPresencePluginEntityWatcher* watcher =
        MatchWatcherL( pres8->Des(), EFalse );
    if ( !watcher )
        {
        DP_SDA("DoUnsubscribePresentityPresenceL watcher not found");
        aStatus = KRequestPending;
        TRequestStatus* temp = &aStatus;
        User::RequestComplete( temp, KErrNotFound );
        }
    else
        {
      	DP_SDA("DoUnsubscribePresentityPresenceL stop subscribe");
        watcher->StopSubscribeL( aStatus );
        }
    CleanupStack::PopAndDestroy( pres8 );  // >> pres8
    
    DP_SDA("CPresencePluginWatcher::DoPerformUnsubscribePresentityPresenceL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::DoSubscribePresentityGroupMembersPresenceL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::DoSubscribePresentityGroupMembersPresenceL(
    const MXIMPIdentity& aGroupId,
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId aReqId )
    {
    DP_SDA("DoSubscribePresentityGroupMembersPresenceL");
    //Subscribe all buddys from virtual group

    iOperation = EPluginSubscribeGroup;
    iXdmState = EPluginIdle;    
    
    SetPresIdentityL( aGroupId, EFalse );
    iXIMPId = aReqId;
    iCompleted = EFalse;
  
    StartXdmOperationL();
    }             

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::
//  DoUpdatePresentityGroupMembersPresenceSubscriptionPifL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::
    DoUpdatePresentityGroupMembersPresenceSubscriptionPifL(
    const MXIMPIdentity& /*aGroupId*/,
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
    User::Leave( KErrNotSupported );
    }             

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::DoUnsubscribePresentityGroupMembersPresenceL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::DoUnsubscribePresentityGroupMembersPresenceL(
    const MXIMPIdentity& aGroupId,
    TXIMPRequestId aReqId )
    {
    DP_SDA("DoUnsubscribePresentityGroupMembersPresenceL");

    iOperation = EPluginUnsubscribeGroup;
    iXdmState = EPluginIdle;    
    
    SetPresIdentityL( aGroupId, EFalse );
    iXIMPId = aReqId;
    iCompleted = EFalse;
  
    StartXdmOperationL();
    }             
       
// ---------------------------------------------------------------------------
// CPresencePluginWatcher::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CPresencePluginWatcher::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions )
    {
    DP_SDA("CPresencePluginWatcher::GetInterface (......) ");
    if ( aInterfaceId == GetInterfaceId() )
        {
        DP_SDA("GetInterface : aInterfaceId == GetInterfaceId()");
        // caller wants this interface
        MProtocolPresenceWatching* myIf = this;
        return myIf;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        DP_SDA("GetInterface: aOptions == MXIMPBase::EPanicIfUnknown");
        User::Panic( KPluginName, KErrExtensionNotSupported );
        }  
    return NULL;
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CPresencePluginWatcher::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions ) const
    {
    DP_SDA("CPresencePluginWatcher::GetInterface (......) const ");
    if ( aInterfaceId == GetInterfaceId() )
        {
        DP_SDA("GetInterface : aInterfaceId == GetInterfaceId()");
        // caller wants this interface
        const MProtocolPresenceWatching* myIf = this;
        return myIf;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        DP_SDA("GetInterface: aOptions == MXIMPBase::EPanicIfUnknown");
        User::Panic( KPluginName, KErrExtensionNotSupported );
        }  
    return NULL;
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::GetInterfaceId()
// ---------------------------------------------------------------------------
//
TInt32 CPresencePluginWatcher::GetInterfaceId() const
    {
    DP_SDA("CPresencePluginWatcher::GetInterfaceId() const");
    return MProtocolPresenceWatching::KInterfaceId;
    }
     
// ---------------------------------------------------------------------------
// CPresencePluginWatcher::MatchWatcherL
// ---------------------------------------------------------------------------
//
CPresencePluginEntityWatcher* CPresencePluginWatcher::MatchWatcherL( 
    const TDesC8& aPresentityid, TBool aCreate )
    {
    DP_SDA("CPresencePluginWatcher::MatchWatcherL");
    DP_SDA2("CPresencePluginWatcher::MatchWatcherL %d", aCreate )
    TInt count = iWatchers.Count();
    DP_SDA2( "MatchWatcherL watcher count %d", count );
    for ( TInt i = 0; i < count; i++ )
        {
        CPresencePluginEntityWatcher* temp = ( iWatchers[i] );
        //Check if prentity found from watcher
        if ( !temp->PresentityId().CompareF( aPresentityid ))
            {
            DP_SDA("CPresencePluginWatcher::MatchWatcherL return temp");
            return temp;
            }
        }
    if ( !aCreate )
        {
        DP_SDA("CPresencePluginWatcher::MatchWatcherL return NULL");
        return NULL;
        }
    else
        {
        // Run this if like create new wathcer
        DP_SDA("CPresencePluginWatcher::MatchWatcherL Else");                
        CPresencePluginEntityWatcher* watcher =
            CPresencePluginEntityWatcher::NewL( 
            		iConnObs, iConnection, *this, iPresenceData );
        CleanupStack::PushL( watcher );
        iWatcherCandidates.AppendL( watcher );
        CleanupStack::Pop();
        DP_SDA("CPresencePluginWatcher::MatchWatcherL end");
        return watcher;
        }
    }    
    
// ---------------------------------------------------------------------------
// CPresencePluginWatcher::DeleteWatcher
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::DeleteWatcher(
    const TDesC8& aPresentityid )
    {
    DP_SDA( "CPresencePluginWatcher::DeleteWatcher" );
    TInt count = iWatchers.Count();
    DP_SDA2( "CPresencePluginWatcher::DeleteWatcher count %d", count);
    
    for ( TInt i = 0; i < count; i++ )
        {
        CPresencePluginEntityWatcher* temp = ( iWatchers[i] );
        if ( !temp->PresentityId().CompareF( aPresentityid ) )
            {
            DP_SDA2( "CPresencePluginWatcher::DeleteWatcher %d", i );
            iWatchers.Remove( i );
            iWatchers.GranularCompress();
            delete temp;
            temp = NULL;
            break; // delete all watchers?
            }
        }
    DP_SDA( "CPresencePluginWatcher::DeleteWatcher end" );
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::SetPresIdentityL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::SetPresIdentityL(
     const MXIMPIdentity& aPresentityId,
     TBool aFormatUri )
    {
    DP_SDA2("CPresencePluginWatcher::SetPresIdentityL: %S", 
        &aPresentityId.Identity() ); 
     
    delete iPresIdentity;
    iPresIdentity = NULL;
     
    if( aFormatUri )
        {
        HBufC8* identityCopy = HBufC8::NewLC( KBufSize255 );
        TPtr8 identityCopyPtr( identityCopy->Des() );
        identityCopyPtr.Copy( aPresentityId.Identity() );
        DP_SDA2(" SetPresIdentityL, org identity: %S", identityCopy );
        
        HBufC8* prefixUri = iPresenceData->CreatePresenceUri8LC( 
            identityCopyPtr );
        
        HBufC* prefixUri16 = HBufC::NewLC( KBufSize255 );
        TPtr prefixUri16Ptr( prefixUri16->Des() );
        prefixUri16Ptr.Copy( *prefixUri );

        DP_SDA2("CPresencePluginWatcher::SetPresIdentityL: with prefix: %S", 
            prefixUri16 ); 
        
        iPresIdentity = prefixUri16;
        CleanupStack::Pop( prefixUri16 );
        CleanupStack::PopAndDestroy( 2, identityCopy );
        }
    else
        {
        DP_SDA("CPresencePluginWatcher::SetPresIdentityL, use as it is");
        iPresIdentity = aPresentityId.Identity().AllocL();
        }
    } 
        
// ---------------------------------------------------------------------------
// CPresencePluginWatcher::CompleteXIMPReq()
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::CompleteXIMPReq( TInt aStatus )
    {
    DP_SDA("CPresencePluginWatcher::CompleteXIMPReq"); 
    iXdmState = EPluginIdle;
    if ( iCompleted )
        {
        return;
        }
    iCompleted = ETrue;
    iConnObs.CompleteReq( iXIMPId, aStatus );
    iXIMPId = TXIMPRequestId();
    DP_SDA("CPresencePluginWatcher::CompleteXIMPReq END");
    iOperation = EPluginUndef;    
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::MatchWatcherId
// ---------------------------------------------------------------------------
//
TBool CPresencePluginWatcher::MatchWatcherId(
    const TDesC8& aPresentityId ) const
    {
    DP_SDA("CPresencePluginWatcher::MatchWatcherId");
    
    TBool watcherFound( EFalse );
    TInt count = iWatchers.Count();
    
    DP_SDA2("CPresencePluginWatcher::MatchWatcherId watcher count %d", count);
    
    for ( TInt i = 0; i < count; i++ )
        {
        CPresencePluginEntityWatcher* temp = ( iWatchers[i] );
        
        //Check if ID is found from watcher
        if ( !temp->PresentityId().CompareF( aPresentityId ) )
            {
            DP_SDA("CPresencePluginWatcher::MatchWatcherId - Match!");
            watcherFound = ETrue;
            }
        }

    return watcherFound;
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::CompleteWatcher
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::CompleteWatcher( TInt aStatus )
    {
    
    DP_SDA("CPresencePluginWatcher::CompleteWatcher");
    DP_SDA2("CPresencePluginWatcher::CompleteWatcher status %d", aStatus);
    // start to remove RLS service when needed.
    if ( iOperation == EPluginUnsubscribeGroup )
        {
        DP_SDA("CompleteWatcher EPluginUnsubscribeGroup");
        if ( iXdmState == EPluginIdle )
            {
            TRAPD( err, iXdmUtils->InitializeXdmsOnlyL( iStatus ));
            if ( err )
                {
                DP_SDA2("CompleteWatcher EPluginUnsubscribeGroup err %d",err);
                CompleteXIMPReq( err );                 
                }
            else
                {
                DP_SDA("CompleteWatcher EPluginUnsubscribeGroup fetch RLS");
                iXdmState = EPluginFetchRls;
                if( !IsActive() )
                	{
                	SetActive();
                	}
                }   
            return;                  
            }                    
        }
    else
        {
        DP_SDA("CPresencePluginWatcher::CompleteWatcher complete");
        CompleteXIMPReq( aStatus );
        }    
    DP_SDA("CPresencePluginWatcher::CompleteWatcher end");  
    }    

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::StartXdmOperationL()
    {
    DP_SDA("CPresencePluginWatcher::StartXdmOperationL");  
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
// CPresencePluginWatcher::DoCancel
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::DoCancel(  )
    {
    iXdmUtils->Cancel();
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::RunL
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::RunL()
    {

    TInt status = iStatus.Int();
    
    DP_SDA2("CPresencePluginWatcher::RunL mystatus %d ", status);
    
    if ( !status )
        {
        if( iOperation == EPluginUnsubscribeGroup )
            {
            DP_SDA("CPresencePluginWatcher::RunL EPluginUnsubscribeGroup ");
            UnSubscribeAllL();	
            }
        if ( iOperation == EPluginSubscribeGroup )
            {
            //First check we have complete buddy list from server
            if ( iXdmState == EPluginInitXdm )
                {
                // get members of the list first         
                SubscribeAllL();
                return; 
                }
            }
        if ( iOperation == EPluginSubscribeSingle )
            {
            DP_SDA("CPresencePluginWatcher::RunL EPluginSubscribeSingle ");
            if ( iXdmState == EPluginIdle )
                {
                DP_SDA("RunL EPluginSubscribeSingle complete");
                iOperation = EPluginUndef;
                CompleteXIMPReq( status );                  
                }
            else
                {
                iOperation = EPluginUndef; 
                CompleteXIMPReq( status );
                }
            }
        }
    else
        {
        DP_SDA("CPresencePluginWatcher::RunL error, complete");
        CompleteXIMPReq( status );
        }
    DP_SDA("CPresencePluginWatcher::RunL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::SubscribeL
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::SubscribeL( const TDesC& aIdentity )
    {
    DP_SDA2("CPresencePluginWatcher::SubscribeL identity: %S", &aIdentity );
    
    MXIMPIdentity* member = iConnObs.ObjectFactory().NewIdentityLC(); 
    member->SetIdentityL( aIdentity );
   
    SetPresIdentityL( *member, EFalse );
                    
    HBufC8* pres8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( 
        member->Identity() );
    CleanupStack::PushL( pres8 );
   
    DP_SDA("CPresencePluginWatcher::SubscribeL try to subsribe");
   
    CPresencePluginEntityWatcher* watcher = MatchWatcherL( 
        pres8->Des(),ETrue );    
    if ( !watcher )
        {
        DP_SDA("CPresencePluginWatcher::SubscribeL no watcher, create");
        watcher = CPresencePluginEntityWatcher::NewL( 
            iConnObs, iConnection, *this, iPresenceData );
        CleanupStack::PushL( watcher );
        iWatcherCandidates.AppendL( watcher );
        CleanupStack::Pop( watcher );
        }
    DP_SDA("CPresencePluginWatcher::SubscribeL do subsribe");
    watcher->StartSubscribeL( pres8->Des() );
    DP_SDA("CPresencePluginWatcher::SubscribeL do subsribe ok");
    
    CleanupStack::PopAndDestroy( pres8 );
    CleanupStack::PopAndDestroy( 1 ); // >> member
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::SubscribeAllL
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::SubscribeAllL()
    {
    DP_SDA("CPresencePluginWatcher::SubscribeAllL");
    
    CDesCArray* subsribed = 
        iConnObs.SubscribedContacts()->GetVirtualIdentityArray();
    
    //Get count
    TInt count = subsribed->Count();
    DP_SDA2( "CPresencePluginWatcher::SubscribeAllL contact count %d", count);
    
    //Subsribe contact one by one
    if( count > 0 )
        {
        for( TInt i = 0;i < count;i++)
    	    {
    	    DP_SDA2( "SubscribeAllL handle contact no: %d", i );
    	    SubscribeL( subsribed->MdcaPoint( i ) );
    	    }
        }
    DP_SDA( "CPresencePluginWatcher::SubscribeAllL end" );
    }

// ---------------------------------------------------------------------------
// CPresencePluginWatcher::UnSubscribeAllL
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::UnSubscribeAllL()
    {
    DP_SDA("CPresencePluginWatcher::UnSubscribeAllL");
    CDesCArray* subsribed = 
        iConnObs.SubscribedContacts()->GetVirtualIdentityArray();
    
    //Get count
    TInt count = subsribed->Count();
    DP_SDA2("CPresencePluginWatcher::UnSubscribeAllL contact count %d",count);
    //UnSubsribe contact one by one

    for( TInt i = 0;i < count;i++)
	    {
	    DP_SDA2("UnSubscribeAllL handle contact no: %d",i);
	    //Get identity form array
	    MXIMPIdentity* member = iConnObs.ObjectFactory().NewIdentityLC(); 
	    member->SetIdentityL( (*subsribed)[i] );
	    
	    SetPresIdentityL( *member, EFalse );

	    HBufC8* pres8 =
	        CnvUtfConverter::ConvertFromUnicodeToUtf8L( member->Identity() );
	    CleanupStack::PushL( pres8 ); // << pres8
	    
	    DP_SDA("CPresencePluginWatcher::UnSubscribeAllL try to unsubsribe");
	    TInt watcherCount = iWatchers.Count();
	    DP_SDA2("UnSubscribeAllL watcher count %d", watcherCount);
	    
	    TBool notFound( EFalse );    
	    for ( TInt j = 0; j < watcherCount; j++ )
	        {
	        CPresencePluginEntityWatcher* watcher =
	        	MatchWatcherL( pres8->Des(), EFalse );
	        if ( !watcher )
	            {
	            DP_SDA("UnSubscribeAllL error not found");
	            notFound = ETrue;
	            }
	        else
	            {
	          	DP_SDA("UnSubscribeAllL stop subscripe");
	            watcher->StopSubscribeL();      
	            }
	        DP_SDA2("CPresencePluginWatcher: done= handle next = %d", i);    
	        }
	   	if ( notFound )
	    	{
	    	DP_SDA("CPresencePluginWatcher::UnSubscribeAllL cotact not found");
	    	}
	    CleanupStack::PopAndDestroy( pres8 );    
	   	CleanupStack::PopAndDestroy( 1 ); // >> member
	    }
    DP_SDA("CPresencePluginWatcher::UnSubscribeAllL end");
    }
       
// ---------------------------------------------------------------------------
// CPresencePluginWatcher::RunError
// ---------------------------------------------------------------------------
//
TInt CPresencePluginWatcher::RunError( TInt aError )
    {
    DP_SDA2("CPresencePluginWatcher::RunError = %d", aError);
    CompleteXIMPReq( aError );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginWatcher::AcceptL()
// ---------------------------------------------------------------------------
//
void CPresencePluginWatcher::AcceptL( const CPresencePluginEntityWatcher* aWatcher )
    {
    DP_SDA( "CPresencePluginWatcher::AcceptL" );
    
    if ( KErrNotFound == iWatchers.Find( aWatcher ) )
        {
        iWatchers.AppendL( aWatcher );
        
        TInt index = iWatcherCandidates.Find( aWatcher );
        if ( KErrNotFound != index )
            {
            iWatcherCandidates.Remove( index );
            }
        }
    }

// End of file
