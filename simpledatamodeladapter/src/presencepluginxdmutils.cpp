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
#include <utf.h>
#include <e32test.h>
#include <ximpobjectfactory.h>
#include <presenceobjectfactory.h>
#include <ximpidentity.h>
#include <ximpobjectcollection.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <XdmEngine.h>
#include <XdmDocument.h>
#include <XdmProtocolInfo.h>
#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include <XdmErrors.h>
#include <pressettingsapi.h> //for TPresSettingsSet
#include <XdmSettingsApi.h>
#include <XdmProtocolUidList.h>
#include <avabilitytext.h>

#include "presencepluginxdmpresrules.h"
#include "mpresencepluginconnectionobs.h"
#include "presenceplugincommon.h"
#include "presencepluginxdmutils.h"
#include "presencepluginvirtualgroup.h"
#include "presencepluginauthorization.h"
#include "presenceplugindata.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::CPresencePluginXdmUtils()
// ---------------------------------------------------------------------------
//
CPresencePluginXdmUtils::CPresencePluginXdmUtils(  
	MPresencePluginConnectionObs& aObs , TInt aXmdId, TBool aLocalmode )
    : CActive( CActive::EPriorityStandard ),
    iConnObs(aObs), iSettingsId(aXmdId),
    iXdmState(EStateIdle), iOperation( ENoOperation ),
	iPresXdmOk( EFalse ), iXdmOk( EFalse ),
	iRulesUpdateState(EStateNoOperation), iLocalMode( aLocalmode )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::ConstructL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::ConstructL( )
    {
    
    //If we are in normalmode we need presence rules too
    if ( !iLocalMode )
        {
        DP_SDA("CPresencePluginXdmUtils::ConstructL NormalMode");
        iXdmPresRules = CPresencePluginXdmPresRules::NewL( 
            iSettingsId, EFalse );
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::NewL()
// ---------------------------------------------------------------------------
//
CPresencePluginXdmUtils* CPresencePluginXdmUtils::NewL(
    MPresencePluginConnectionObs& aObs, TInt aXmdId, TBool aLocalmode )
    {
    CPresencePluginXdmUtils* self =
    	new( ELeave ) CPresencePluginXdmUtils( aObs, aXmdId, aLocalmode );
    CleanupStack::PushL( self );
    self->ConstructL(   );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::~CPresencePluginXdmUtils()
// ---------------------------------------------------------------------------
//
CPresencePluginXdmUtils::~CPresencePluginXdmUtils()
    {
    DP_SDA("CPresencePluginXdmUtils::~CPresencePluginXdmUtils");
    if ( iXdmDoc )
        {
        DP_SDA("CPresencePluginXdmUtils::~CPresencePluginXdmUtils 1");
        if (iXdmEngine)
            {
            DP_SDA("CPresencePluginXdmUtils::~CPresencePluginXdmUtils del doc");
            TRAP_IGNORE( iXdmEngine->DeleteDocumentModelL( iXdmDoc ));
            }
        }
        
    DP_SDA("CPresencePluginXdmUtils::~CPresencePluginXdmUtils 3");
    delete iXdmEngine;
    iXdmEngine = NULL;


    DP_SDA("CPresencePluginXdmUtils::~CPresencePluginXdmUtils 4");
    delete iXdmPresRules;
    iXdmPresRules = NULL;


    DP_SDA("CPresencePluginXdmUtils::~CPresencePluginXdmUtils 5");
    delete iEntityUri;
    iEntityUri = NULL;

    iBlockedContacts.ResetAndDestroy();

    DP_SDA("CPresencePluginXdmUtils::~CPresencePluginXdmUtils end"); 
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::InitializeXdmL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::InitializeXdmL( TRequestStatus& aStatus )
    {
    DP_SDA("CPresencePluginXdmUtils::InitializeXdmL");
    
    aStatus = KRequestPending;
	iClientStatus = &aStatus;
    
	// if resource-list not ready
    if ( !iXdmOk ) 
        {
        DP_SDA("CPresencePluginXdmUtils::InitializeXdmL !iXdmOk");
        if ( !iLocalMode )
            {
            DP_SDA("CPresencePluginXdmUtils::InitializeXdmL NormalMode");
            GetXdmListsL( ETrue, EFalse );
            }
        else
            {
            DP_SDA("CPresencePluginXdmUtils::InitializeXdmL LocalMode");
            GetXdmListsL( ETrue, ETrue );
            }
        }
    else if ( !iPresXdmOk && !iLocalMode )
        {
        DP_SDA("CPresencePluginXdmUtils::InitializeXdmL  !iPresXdmOk");
        GetXdmRulesL();  
        }
    else
        {
        DP_SDA("CPresencePluginXdmUtils::InitializeXdmL All is OK ");
        // All is ok
        DP_SDA("CPresencePluginXdmUtils::InitializeXdmL SEND COMPLETE")        
        CompleteClientReq( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::InitializeXdmL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginXdmUtils::InitializePresRulesL( )
    {
    DP_SDA("CPresencePluginXdmUtils:: MakeInitialRules");
    iXdmState = ECreateXdmRules;
	iRulesUpdateState = EStateInitializeRules;
    iXdmPresRules->MakeInitialRulesDocumentL( this );
    UpdateXdmRulesL();
    DP_SDA("CPresencePluginXdmUtils:: MakeInitialRules done");
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::GetXdmRulesL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginXdmUtils::GetXdmRulesL()
    {
    DP_SDA("CPresencePluginXdmUtils:: GetXdmRulesL");

	if ( !iXdmPresRules )
		{
    // get data from a network if not done so yet
		if ( !iLocalMode )
			{
			iXdmPresRules = CPresencePluginXdmPresRules::NewL( 
				iSettingsId, EFalse );
			}
		else
			{
			/*localmode*/
			//iXdmPresRules 
			//= CPresencePluginXdmPresRules::NewL( iSettingsId, ETrue );
			}
		}
    
	if( !iLocalMode && iXdmPresRules )
	    {
	    User::LeaveIfError( iXdmPresRules->UpdateFromServerL( this ));
	    }
    iXdmState = EGetXdmRules;
    DP_SDA("CPresencePluginXdmUtils:: GetXdmRulesL done");
    DP_SDA("CPresencePluginXdmUtils::GetXdmRulesL - add buddies to virtual");
    AddIdentityToVirtualListL( KPresenceBuddyList );
    
    DP_SDA("CPresencePluginXdmUtils::GetXdmRulesL - add blocked to virtual");
    AddIdentityToVirtualListL( KPresenceBlockedList );
    }
        
// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::InitializeXdmsOnlyL()
// ---------------------------------------------------------------------------
//   
void CPresencePluginXdmUtils::InitializeXdmsOnlyL( TRequestStatus& aStatus )
    {
    DP_SDA("CPresencePluginXdmUtils::InitializeXdmsOnlyL");             
	aStatus = KRequestPending;
    iClientStatus = &aStatus;

    if ( !iXdmOk )
        {
        DP_SDA("CPresencePluginXdmUtils::InitializeXdmsOnlyL 1");
        GetXdmListsL( EFalse, EFalse );        
        }
    else
        {
        DP_SDA("CPresencePluginXdmUtils::InitializeXdmsOnlyL all ok");
        // All is ok
        DP_SDA("CPresencePluginXdmUtils::InitializeXdmsOnlyL SEND COMPLETE")        
        CompleteClientReq( KErrNone );
        }
    DP_SDA("CPresencePluginXdmUtils::InitializeXdmsOnlyL end");
    }
        
// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::CheckXdmDoc()
// ---------------------------------------------------------------------------
//    
TBool CPresencePluginXdmUtils::CheckXdmDoc()
    {
    DP_SDA("CPresencePluginXdmUtils::CheckXdmDoc");
    
    if ( iXdmDoc )
        {
        CXdmDocumentNode* root = iXdmDoc->DocumentRoot();
        DP_SDA("CPresencePluginXdmUtils::CheckXdmDoc2");
        if( root )
        	{	
            TInt count = root->NodeCount();
            DP_SDA2("CPresencePluginXdmUtils::CheckXdmDoc count %d", count);
            if ( !count )
                {
                DP_SDA("CPresencePluginXdmUtils::CheckXdmDoc NotFound");
                iXdmOk = EFalse;
                }
            else
                {
                DP_SDA("CPresencePluginXdmUtils::CheckXdmDoc file ok");
                iXdmOk = ETrue;
                }
        	}
        }
    else
        {
        DP_SDA("CPresencePluginXdmUtils::CheckXdmDoc file False");
        iXdmOk = EFalse;
        }
    
    return iXdmOk;
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::CheckIfError
// ---------------------------------------------------------------------------
//
TInt CPresencePluginXdmUtils::CheckIfErrorL( TInt aMyStatus )
    {
    TInt myStatus = aMyStatus;
    DP_SDA2("CPresencePluginXdmUtils::CheckIfError ErrorCode %d ", myStatus);
    if ( myStatus == KErrTimedOut )
        {
        DP_SDA("CPresencePluginXdmUtils::CheckIfError  TimeOut error");
        myStatus = KErrTimedOut;
        User::Leave( KErrTimedOut );
        }
    else if ( myStatus == KErrCouldNotConnect || 
    	myStatus == KXcapErrorNetworkNotAvailabe )
        {
        DP_SDA("CheckIfError KErrCouldNotConnect error");
        myStatus = aMyStatus;
        }
	else if ( myStatus == KXcapErrorHttpNotFound )
		{
		DP_SDA("CheckIfError KXcapErrorHttpNotFound");
        myStatus = aMyStatus;
		}
    else if ( myStatus == KXcapErrorAuthentication )
        {
        DP_SDA("CPresencePluginXdmUtils::CheckIfError auth error");                        
        myStatus = aMyStatus;
        }
    else if ( myStatus == KXcapErrorTooManyOccurencies )
        {
        DP_SDA("CPresencePluginXdmUtils KXcapErrorTooManyOccurencies");                        
        myStatus = 0;
        }
    else if( myStatus == KErrDisconnected )
        {
        myStatus = aMyStatus;
        User::Leave( KErrCancel );
        }
    else if( myStatus == KXcapErrorHttpPrecondFailed )
    	{
    	DP_SDA("CheckIfError KXcapErrorHttpPrecondFailed");
        myStatus = aMyStatus;
    	}
    else if ( KErrNoMemory == myStatus )
        {
        DP_SDA("CheckIfError KErrNoMemory");
        User::Leave( KErrNoMemory );
        }
    else
        {
        DP_SDA("CPresencePluginXdmUtils::CheckIfError ERROR");
        myStatus = aMyStatus;
        }
    return myStatus;
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::RunL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::RunL(  )
    {    
    DP_SDA("CPresencePluginXdmUtils::RunL");    
    TPluginXdmState origState = iXdmState;
    iXdmState = EStateIdle;
    TInt myStatus = iStatus.Int();
    
    DP_SDA2("CPresencePluginXdmUtils::RunL mystatus %d ", myStatus);
    DP_SDA2("CPresencePluginXdmUtils::RunL state %d ", origState);
    DP_SDA2("CPresencePluginXdmUtils:: ClientStatus %d",iClientStatus->Int() );
    
    if ( myStatus == KErrCancel )
        {
        DP_SDA("CPresencePluginXdmUtils::RunL CANCEL SEND COMPLETE")
        CompleteClientReq( KErrCancel );
        }
    
    switch ( origState )
        {
        case EGetXdmLists:
		case EGetXdmListsOnly:
            {
            DoGetXdmListsL( myStatus, origState );
            }
        break;
                
        case ECreateXdmLists:
            {
            DoCreateXdmListsL( myStatus, origState, EFalse );
            }
        break;
        
        case ECreateXdmListsOnly:
            {
            DoCreateXdmListsL( myStatus, origState, ETrue );
            }
        break;
          
        case EGetXdmBlockList:
            {
            DP_SDA("RunL EGetXdmBlockList SEND COMPLETE");
            CompleteClientReq( KErrNone );
            }
        break;
        
        case EUpdateXdmList:
            {
            DoUpdateXdmListsL( myStatus, origState );
            }
        break;

        case EUpdateBlockedContactPresenceCache:
            DoUpdateBlockedContactPresenceCacheL( myStatus );
            break;

        default:
            {
            DP_SDA("CPresencePluginXdmUtils::RunL last else");        
            //check myStatus error
            myStatus = CheckIfErrorL( myStatus );
                
            if ( myStatus )
                {
                // complete reqular request
                DP_SDA("CPresencePluginXdmUtils::RunL SEND LAST COMPLETE");
                CompleteClientReq( myStatus );
                }
            DP_SDA("CPresencePluginXdmUtils::RunL default end")
            }
        break;
          
        }
    DP_SDA("CPresencePluginXdmUtils::RunL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::DoUpdateXdmLists()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::DoUpdateXdmListsL( 
    TInt aMyStatus,
    TPluginXdmState /*aOrigState*/)
    {
    DP_SDA("CPresencePluginXdmUtils::DoUpdateXdmLists EUpdateXdmList");
    if ( !iLocalMode )
        {
        DP_SDA("CPresencePluginXdmUtils::DoUpdateXdmLists updaterules");
        if( aMyStatus )
        	{
        	DP_SDA("DoUpdateXdmLists updaterules error");
        	aMyStatus = CheckIfErrorL( aMyStatus );
        	CompleteClientReq( aMyStatus );
        	}
        else
        	{
        	DP_SDA("DoUpdateXdmLists updaterules else");	 
        	UpdateXdmRulesL();
        	}
        }
    else
        {
        DP_SDA("EUpdateXdmList LOCALMODE SEND COMPLETE");
        aMyStatus = CheckIfErrorL( aMyStatus );
        CompleteClientReq( aMyStatus );
        } 
    iRulesUpdateState = EStateNoOperation;
    DP_SDA("CPresencePluginXdmUtils::DoUpdateXdmLists EUpdateXdmList end");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::DoCreateXdmLists()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::DoCreateXdmListsL( 
    TInt aMyStatus,
    TPluginXdmState /*aOrigState*/,
    TBool aOnlyResourceList )
    {
    DP_SDA("CPresencePluginXdmUtils::DoCreateXdmListsL ECreateXdmLists");
    //check myStatus error
    aMyStatus = CheckIfErrorL( aMyStatus );
    if( !aOnlyResourceList )
        {
        if ( aMyStatus )
            {
            DP_SDA("DoCreateXdmLists ECreateXdmLists SEND COMPLETE");
            CompleteClientReq( aMyStatus );
            }
        else if ( !iLocalMode )
            {
            DP_SDA("ECreateXdmLists !myStatus GetXdmRules");
            iXdmOk = ETrue;
            GetXdmRulesL();
            }
        else if ( iLocalMode )
            {
            DP_SDA("DoCreateXdmLists ECreateXdmLists SEND COMPLETE 2");
            iXdmOk = ETrue;
            CompleteClientReq( aMyStatus );
            }
        }
    else
        {
        //check myStatus error
        aMyStatus = CheckIfErrorL( aMyStatus );     
        if ( aMyStatus == KErrTimedOut  )
            { 
            DP_SDA("DoCreateXdmLists EGetXdmListsOnly 1");
            aMyStatus = 0;
            }           
        if ( !aMyStatus )
            {
            DP_SDA("DoCreateXdmLists EGetXdmListsOnly 2");
            iXdmOk = ETrue;
            }
        
        if( !(KXcapErrorNetworkNotAvailabe == aMyStatus || 
              KErrCouldNotConnect == aMyStatus ) )
            {
            DP_SDA("DoCreateXdmLists EGetXdmListsOnly SEND COMPLETE");
            CompleteClientReq( aMyStatus );
            }
        }
    DP_SDA("CPresencePluginXdmUtils::DoCreateXdmListsL ECreateXdmLists end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::DoGetXdmLists()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::DoGetXdmListsL( 
    TInt aMyStatus,
    TPluginXdmState aOrigState )
    {
    DP_SDA("CPresencePluginXdmUtils::DoGetXdmLists EGetXdmLists");
    //check myStatus error
    aMyStatus = CheckIfErrorL( aMyStatus );

    if ( aMyStatus )//Check is file includes nodes
        {
        DP_SDA("CPresencePluginXdmUtils::DoGetXdmLists CreateXDM");
        // no resource-list, let's start to create it.
        CreateXdmListsL( ETrue );
        }
    else
        {
        DP_SDA("CPresencePluginXdmUtils::DoGetXdmLists EGetXdmLists else");
        if( iLocalMode )
            {
            //Check if document empty
            TBool empty = CheckXdmDoc();
            if( !empty )
                {
                CreateXdmListsL( ETrue );
                }
            else
                {
                iXdmOk = ETrue;
                DP_SDA("DoGetXdmLists Localmode SEND COMPLETE"); 
                CompleteClientReq( KErrNone );
                }
            }
        else
            {
            iXdmOk = ETrue;
            if ( !iPresXdmOk && !iLocalMode && aOrigState == EGetXdmLists )
                {
                DP_SDA("CPresencePluginXdmUtils::DoGetXdmLists GetXDM");
                GetXdmRulesL();
                }
            else
                {
                DP_SDA("CPresencePluginXdmUtils::DoGetXdmLists SEND COMPLETE"); 
                CompleteClientReq( KErrNone );
                }
            }
        }
    DP_SDA("CPresencePluginXdmUtils::DoGetXdmLists end");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::DoCancel()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::DoCancel( )
    {										  
    DP_SDA("CPresencePluginXdmUtils::DoCancel");
    iOperation = EXdmCancel;

    if ( iXdmState == EGetXdmLists || iXdmState == ECreateXdmLists ||
         iXdmState == EUpdateXdmRules || iXdmState == EUpdateXdmList ||
         iXdmState == EGetXdmBlockList || iXdmState == ECreateXdmRules ||
         iXdmState == ESubsBlockList || 
         iXdmState == EGetResourceListFromServer ||
         iXdmState == EGetXdmListsOnly || iXdmState == ECreateXdmListsOnly )
        {
        DP_SDA("CPresencePluginXdmUtils::DoCancel list get or create");
        iXdmState = ECancelDocument;
        iXdmEngine->CancelUpdate( iXdmDoc );
        // cancel presrules too
        iXdmPresRules->Cancel();
        }
    else
        {
        iStatus = KRequestPending;
        TRequestStatus* s= &iStatus;
        DP_SDA("CPresencePluginXdmUtils::DoCancel SEND COMPLETE");
        User::RequestComplete( s, KErrCancel );
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::RunError()
// ---------------------------------------------------------------------------
//
TInt CPresencePluginXdmUtils::RunError( TInt aError )
    {
    DP_SDA("CPresencePluginXdmUtils::RunError");
    // complete the open request
    CompleteClientReq( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::AddEntityToGrantedL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::AddEntityToGrantedL(
    const TDesC&  aUri,
    TRequestStatus& aStatus )
    {
  	DP_SDA("CPresencePluginXdmUtils::AddEntityToGrantedL");
  	DP_SDA2("CPresencePluginXdmUtils::AddEntityToGrantedL, uri: %S", &aUri );
  	
  	TRAPD( err, iEntityUri =  aUri.AllocL() );
  	DP_SDA2(" AddEntityToGrantedL, uri alloc err: %d", err );
   
   //Add entity to virtualGroup
  	iConnObs.SubscribedContacts()->AddEntityL( iEntityUri->Des() );
  	
  	if ( !err )
  	    {
  	    DP_SDA("CPresencePluginXdmUtils::AddEntityToGrantedL !err");
  	    iXdmState = EUpdateXdmList;
        iClientStatus = &aStatus;
        iOperation = EXdmAddUserToGroup;
        iRulesUpdateState = EStateAddToWhiteList;
        
        //add user to resouce-list list
        DoAddUserToListL( KPresenceBuddyList, aUri );
        DP_SDA("CPresencePluginXdmUtils::AddEntityToGrantedL 1");
        UpdateXdmsL();
        
        *iClientStatus = KRequestPending;
        DP_SDA("CPresencePluginXdmUtils::AddEntityToGrantedL end");
  	    }
  	 else
  	    {
  	    DP_SDA("AddEntityToGrantedL else error SEND COMPLETE");
  	    //If error we can complete request
  	    CompleteClientReq( KErrNone );
  	    }
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::AddEntityToBlockedL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::AddEntityToBlockedL(
    const TDesC&  aUri,
    TRequestStatus& aStatus )
    {
    DP_SDA("CPresencePluginXdmUtils::AddEntityToBlockedL");
    iClientStatus = &aStatus;
        
    if( iLocalMode )
        {
        //Block is not used in localmode
        DP_SDA("AddEntityToBlockedL localmode rules not support");
        CompleteClientReq( KErrNotSupported );
        }
    else
        {
        DP_SDA("CPresencePluginXdmUtils::AddEntityToBlockedL !localmode");
        TRAPD(err, iEntityUri =  aUri.AllocL() );
		//Remove entity form subscribed virtual group
    	iConnObs.SubscribedContacts()->RemoveEntityL( iEntityUri->Des() );
    	
        if ( !err )
            {
            DP_SDA("CPresencePluginXdmUtils::AddEntityToBlockedL no error");
            iXdmState = EUpdateXdmList;
            iOperation = EXdmAddUserToGroup;
            iRulesUpdateState = EStateAddToBlockList;

            DoAddUserToListL( KPresenceBlockedList, aUri );
            UpdateXdmsL();

            *iClientStatus = KRequestPending;
            DP_SDA("CPresencePluginXdmUtils::AddEntityToBlockedL ready");
            }
        else
        	{
        	//If error we can complete request
        	DP_SDA("AddEntityToBlockedL SEND COMPLETE");
        	CompleteClientReq( KErrNone );
        	}
        }
    DP_SDA("CPresencePluginXdmUtils::AddEntityToBlockedL end");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::DoAddUserToListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::DoAddUserToListL(
    const TDesC& aList,
    const TDesC&  aUri )
    {
    DP_SDA("CPresencePluginXdmUtils::DoAddUserToListL");
    DP_SDA2("CPresencePluginXdmUtils::DoAddUserToListL aList: %S", &aList);
    DP_SDA2("CPresencePluginXdmUtils::DoAddUserToListL aUri: %S", &aUri);
    
    using namespace NPresencePlugin::NPresence;
     
    CXdmDocumentNode* root = iXdmDoc->DocumentRoot();
    CXdmDocumentNode* foundNode = SearchListUnderParentL( root, aList );
        
    if ( foundNode )
        {
        DP_SDA("CPresencePluginXdmUtils::DoAddUserToListL found");
        if ( !CheckIfEnityExistL( aList, aUri ) )
            {
            DP_SDA("DoAddUserToListL entity not exist");
            if ( foundNode->IsEmptyNode() )
                {
                DP_SDA("DoAddUserToListL IsEmptyNode");
                foundNode->SetEmptyNode( EFalse );
                }
                
			DP_SDA("CPresencePluginXdmUtils::DoAddUserToListL newNode");
            CXdmDocumentNode* newNode = 
                foundNode->CreateChileNodeL( KPresenceEntry );
            DP_SDA("DoAddUserToListL entry attribute");    
            CXdmNodeAttribute* attributeEntryUri = 
                newNode->CreateAttributeL( KPresenceUri );
            DP_SDA("CPresencePluginXdmUtils::DoAddUserToListL set entry");    
            attributeEntryUri->SetAttributeValueL( aUri );
            DP_SDA("CPresencePluginXdmUtils::DoAddUserToListL Append");
            iXdmDoc->AppendL( );
            }
        }
    else
        {
        DP_SDA("CPresencePluginXdmUtils::DoAddUserToListL leave");
        User::Leave( KErrNotFound );
        }
	DP_SDA("CPresencePluginXdmUtils::DoAddUserToListL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::CheckIfEnityExistL
// ---------------------------------------------------------------------------
//
TBool CPresencePluginXdmUtils::CheckIfEnityExistL(
    const TDesC& aList,
    const TDesC&  aUri )
    {
    DP_SDA("CPresencePluginXdmUtils::CheckIfEnityExist");
    
    using namespace NPresencePlugin::NPresence;
    
    CXdmNodeAttribute* attr = NULL;
    RPointerArray<CXdmDocumentNode> nodes;
    CleanupClosePushL( nodes );
    TBool state( EFalse );
    
    CXdmDocumentNode* root = iXdmDoc->DocumentRoot();
    CXdmDocumentNode* foundNode = SearchListUnderParentL( root, aList );
    
    if ( foundNode )
        {
        DP_SDA("CPresencePluginXdmUtils::CheckIfEnityExist check nodes");
        TInt findErr = foundNode->Find( KPresenceEntry, nodes );
        DP_SDA2("CheckIfEnityExist - findErr: %d", findErr );

        if ( findErr )
            {
            DP_SDA("CheckIfEnityExist Enity not exist ");
            state = EFalse;
            }
        else
            {
            DP_SDA("CPresencePluginXdmUtils::CheckIfEnityExist Else");
            TInt nodeCount = nodes.Count();
            for ( TInt i = 0; i < nodeCount; i++ )
                {
                CXdmDocumentNode* currNode = nodes[i];
                attr = ( currNode )->Attribute( KPresenceUri );
                
                TBool uriMatch( EFalse );
                uriMatch = CompareUriWithoutPrefixL( 
                    aUri, attr->AttributeValue() );
                
                if ( attr && uriMatch )
                    {
                    DP_SDA("CheckIfEnityExist entity exists");
                    //Enity is exist
                    state = ETrue;
                    break;
                    }
                }
            }
        }
    DP_SDA("CPresencePluginXdmUtils::CheckIfEnityExist Destroy nodes");
    CleanupStack::PopAndDestroy( &nodes ); // >>> nodes    
    return state;
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::SearchListUnderParentL
// ---------------------------------------------------------------------------
//
CXdmDocumentNode* CPresencePluginXdmUtils::SearchListUnderParentL(
    CXdmDocumentNode* aParent,
    const TDesC& aName )
    {
    DP_SDA("CPresencePluginXdmUtils::SearchListUnderParentL");
    
    using namespace NPresencePlugin::NPresence;

    if ( !aParent )
        {
        DP_SDA("CPresencePluginXdmUtils::SearchListUnderParentL return NULL");
        return NULL;
        }

    RPointerArray<CXdmDocumentNode> resultArray;
    RPointerArray<SXdmAttribute16>  attributeArray;

    CleanupClosePushL( resultArray );           // <<< resultArray
    CleanupClosePushL( attributeArray );        // <<< attributeArray

    SXdmAttribute16 attr;
    attr.iName.Set( KPresenceName );
    attr.iValue.Set( aName );
    attributeArray.Append( &attr );

    CXdmDocumentNode* currNode = NULL;

    aParent->Find( KPresenceList, resultArray, attributeArray );

    TInt count = resultArray.Count();
    DP_SDA2("SearchListUnderParentL node count %d",count);
    for ( TInt i=0; i < count; i++ )
        {
        currNode = resultArray[i];
        CXdmDocumentNode* parent = currNode->Parent();
        if ( parent == aParent )
            {
            DP_SDA("SearchListUnderParentL parent MATCH");
            break;
            }
        currNode = NULL;
        }

    CleanupStack::PopAndDestroy( &attributeArray); // >>> attributeArray
    CleanupStack::PopAndDestroy( &resultArray );   // >>> resultArray
    DP_SDA("CPresencePluginXdmUtils::SearchListUnderParentL return");
    return currNode;
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::RemoveEntityFromGrantedL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginXdmUtils::RemoveEntityFromGrantedL(
    const TDesC&  aUri,
    TRequestStatus& aStatus )
    {
    DP_SDA("CPresencePluginXdmUtils::RemoveEntityFromGrantedL");
    TRAPD(err, iEntityUri =  aUri.AllocL() );

  	iConnObs.SubscribedContacts()->RemoveEntityL( iEntityUri->Des() );
    
    if ( !err )
        {
        iClientStatus = &aStatus;
        iRulesUpdateState = EStateRemoveFromWhiteList;
        iOperation = EXdmRemoveUserFromGroup;

        DoRemoveUserFromListL( KPresenceBuddyList, aUri );
        // send to the server and start wait a response
        UpdateXdmsL();
        iXdmState = EUpdateXdmList;
        *iClientStatus = KRequestPending;
        }
    else
        {
        //If error we can complete request
        DP_SDA("RemoveEntityFromGrantedL SEND COMPLETE");
  	    CompleteClientReq( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::RemoveEntityFromBlockedL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::RemoveEntityFromBlockedL(
    const TDesC&  aUri,
    TRequestStatus& aStatus )
    {
    DP_SDA("CPresencePluginXdmUtils::RemoveEntityFromBlockedL");
    TRAPD(err, iEntityUri =  aUri.AllocL() );
    
    if ( !err )
        {
        iClientStatus = &aStatus;
        iRulesUpdateState = EStateRemoveFromBlackList;
        iOperation = EXdmRemoveUserFromGroup;

        DoRemoveUserFromListL( KPresenceBlockedList, aUri );
        // send to the server and start wait a response
        UpdateXdmsL();
        iXdmState = EUpdateXdmList;
        *iClientStatus = KRequestPending;
        }
    else
        {
        //If error we can complete request
        DP_SDA("RemoveEntityFromBlockedL SEND COMPLETE");
  	    CompleteClientReq( KErrNone );
        }  
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::DoRemoveUserFromListL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginXdmUtils::DoRemoveUserFromListL(
    const TDesC& aList,
    const TDesC&  aUri )
    {
    DP_SDA("CPresencePluginXdmUtils::DoRemoveUserFromListL");
    CXdmNodeAttribute* attr = NULL;

    using namespace NPresencePlugin::NPresence;

    RPointerArray<CXdmDocumentNode> lists;
    CleanupClosePushL( lists );

    RPointerArray<CXdmDocumentNode> nodes;
    CleanupClosePushL( nodes );

    CXdmDocumentNode* root = iXdmDoc->DocumentRoot();
    CXdmDocumentNode* foundNode = SearchListUnderParentL( root, aList );
    if ( foundNode )
        {
        DP_SDA("CPresencePluginXdmUtils::DoRemoveUserFromListL Node Found");		
        User::LeaveIfError( foundNode->Find( KPresenceEntry, nodes ));
        TInt nodeCount = nodes.Count();
        for ( TInt i = 0; i < nodeCount; i++ )
            {
            CXdmDocumentNode* currNode = nodes[i];         
            attr = ( currNode )->Attribute( KPresenceUri );
            
            TBool uriMatch( EFalse );
            uriMatch = CompareUriWithoutPrefixL( 
                aUri, attr->AttributeValue() );
            
            if ( attr && uriMatch )
                {
                DP_SDA(" DoRemoveUserFromListL Node Found, do delete");	
            
                // This is the user we are looking for deletion.
                //First remove currNode form model
                iXdmDoc->RemoveFromModelL( currNode );
                // Update model to server
                iXdmDoc->AppendL();
                break;
                }
            }
        }
    else
        {
        DP_SDA("CPresencePluginXdmUtils::DoRemoveUserFromListL Leave");	
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( &nodes ); // >>> nodes
    CleanupStack::PopAndDestroy( &lists ); // >>> lists
    DP_SDA("CPresencePluginXdmUtils::DoRemoveUserFromListL END");
    }
       
// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::SubscribeBuddyListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::SubscribeBuddyListL( 
    RPointerArray<MXIMPIdentity>& aMembers )
    {
    DP_SDA("CPresencePluginXdmUtils::SubscribeBuddyListL");

    iOperation = EXdmGetBuddyList;
    DoGetListMembersL( KPresenceBuddyList, aMembers );

    DP_SDA("CPresencePluginXdmUtils::SubscribeBuddyListL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::SubscribeBlockListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::SubscribeBlockListL( 
    RPointerArray<MXIMPIdentity>& aMembers )
    {
    DP_SDA("CPresencePluginXdmUtils::SubscribeBlockListL");

    if ( !iLocalMode )
        {
        DP_SDA("CPresencePluginXdmUtils::SubscribeBlockListL !iLocalMode");
        iOperation = EXdmGetBlockedList;
        DoGetListMembersL( KPresenceBlockedList, aMembers );
        }
    DP_SDA("CPresencePluginXdmUtils::SubscribeBlockListL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::DoGetListMembersL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::DoGetListMembersL(
    const TDesC& aList,
    RPointerArray<MXIMPIdentity>& aMembers  )
    {
    DP_SDA("CPresencePluginXdmUtils::DoGetListMembersL");
    using namespace NPresencePlugin::NPresence;

    aMembers.Reset();

    RPointerArray<CXdmDocumentNode> nodes;
    CleanupClosePushL( nodes );

    CXdmDocumentNode* listNode = NULL;
    CXdmNodeAttribute* attr = NULL;

    TBool found(EFalse);
        
    if ( iXdmDoc )
        {
        DP_SDA("CPresencePluginXdmUtils::DoGetListMembersL iXdmDoc");
        User::LeaveIfError( iXdmDoc->Find( KPresenceList, nodes ) );
        DP_SDA("CPresencePluginXdmUtils::DoGetListMembersL find ready");
        TInt count = nodes.Count();
        DP_SDA2("DoGetListMembersL find Count %d", count);
        for ( TInt i=0; i < count; i++ )
            {
            listNode = nodes[i];
            attr = listNode->Attribute( KPresenceName );
            if ( attr && !attr->AttributeValue().CompareF( aList ))
                {
                // That's the right list
                DP_SDA("DoGetListMembersL Target found");
                found = ETrue;
                break;
                }
            }
        }
   
    if ( found )
        {
        DP_SDA("CPresencePluginXdmUtils::DoGetListMembersL found");
        MXIMPObjectFactory& myFactory = iConnObs.ObjectFactory();
        nodes.Reset();
        TInt err = listNode->Find( KPresenceEntry, nodes );
        DP_SDA2("CPresencePluginXdmUtils::DoGetListMembersL err %d", err);
        if( !err )
            {
            DP_SDA("CPresencePluginXdmUtils::DoGetListMembersL Entry found");
            TInt nodeCount = nodes.Count();
            for ( TInt i = 0; i < nodeCount; i++ )
                {
                attr = (nodes[i])->Attribute( KPresenceUri );
                if ( attr )
                    {
                    DP_SDA("DoGetListMembersL attr check");
                    MXIMPIdentity* entity =
                        myFactory.NewIdentityLC(); // << entity
                    entity->SetIdentityL( attr->AttributeValue() );
                    aMembers.Append( entity );
                    CleanupStack::Pop(); // >> entity
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy( &nodes );
    DP_SDA("CPresencePluginXdmUtils::DoGetListMembersL end");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::GetXdmListsL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::GetXdmListsL( 
    TBool aCreateRules,
    TBool aLocalMode )
    {
    DP_SDA("CPresencePluginXdmUtils::GetXdmListsL");
    //Normal Mode start
    if ( !iXdmEngine && !aLocalMode )
        {
        DP_SDA2("!iXdmEngine settingID = %d", iSettingsId);
        
#ifdef _DEBUG
        //Get xdmSettings
        HBufC* accesPoint = TXdmSettingsApi::PropertyL( 
            iSettingsId, EXdmPropToNapId );
        CleanupStack::PushL( accesPoint );
        DP_SDA2("CPresencePluginXdmUtils::GetXdmListsL accesPoint %S", 
            accesPoint);
#endif
        
        // get data from a network if not done so yet
        CXdmProtocolInfo* info = CXdmProtocolInfo::NewL( iSettingsId );
        CleanupStack::PushL( info );
        
        // Notice: we do not use cache for XDM
        info->SetCacheUsage( EFalse ); //False in normal case
        
        iXdmEngine = CXdmEngine::NewL( *info );
        CleanupStack::PopAndDestroy( info );
#ifdef _DEBUG        
        CleanupStack::PopAndDestroy( accesPoint );
#endif        
        }
    //LocalMode Start, !!! THERE IS LIMIT !!!
    //Only one settings can use localmode 
    else if ( !iXdmEngine && aLocalMode )
        {
        DP_SDA("CPresencePluginXdmUtils::GetXdmListsL aLocalMode");
        CXdmProtocolInfo* info =
            CXdmProtocolInfo::NewL( 0, KLocalProtocol, KNullDesC );
        CleanupStack::PushL( info );
        
        iXdmEngine = CXdmEngine::NewL( *info );
        CleanupStack::PopAndDestroy( info );
        DP_SDA("CPresencePluginXdmUtils::GetXdmListsL aLocalMode end");
        }   
    if ( !iXdmOk )
        {
        DP_SDA("CPresencePluginXdmUtils::GetXdmListsL !XdmOK "); 
        using namespace NPresencePlugin::NPresence;
        // Get data from network
        DP_SDA("GetXdmListsL Create Document Model ");
        
        iXdmDoc = iXdmEngine->CreateDocumentModelL( 
            KPresenceResourceList, EXdmResourceLists );
       
        
        DP_SDA("Create Document Model FetchData");
        iXdmDoc->FetchDataL();    
        DP_SDA("CPresencePluginXdmUtils::GetXdmListsL !XdmOK SETACTIVE ");
        UpdateXdmsL();
        iXdmState = aCreateRules ? EGetXdmLists : EGetXdmListsOnly;
        DP_SDA("CPresencePluginXdmUtils::GetXdmListsL !XdmOK END"); 
        }
    else
        {
        DP_SDA("CPresencePluginXdmUtils::GetXdmListsL else");
        // Document already exists, no need to search from a server
        iStatus = KRequestPending;
        TRequestStatus* s= &iStatus;
        DP_SDA("CPresencePluginXdmUtils::GetXdmListsL SEND COMPLETE");
        User::RequestComplete( s, KErrNone );
        iXdmState = aCreateRules ? EGetXdmLists : EGetXdmListsOnly;
        SetActive();        
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::CreateXdmListsL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::CreateXdmListsL( TBool aCreateRules )
    {
    DP_SDA("CPresencePluginXdmUtils::CreateXdmListsL");
 
    CXdmDocumentNode* root = iXdmDoc->CreateRootL();
    CleanupStack::PushL( root );
    
    using namespace NPresencePlugin::NPresence;
    root->SetNameL( KPresenceResourceLists );

    // add all the list nodes
    // buddylist
    using namespace NPresencePlugin::NPresence;
    if ( root->IsEmptyNode() )
        {
        root->SetEmptyNode( EFalse );
        }
    //buddy list
    CXdmDocumentNode* child1 = root->CreateChileNodeL( KPresenceList );
    CleanupStack::PushL( child1 );
    CXdmNodeAttribute* name = child1->CreateAttributeL( KPresenceName );
    CleanupStack::PushL( name );
    name->SetAttributeValueL( KPresenceBuddyList );

    // blockedcontacts
    CXdmDocumentNode* child2 = root->CreateChileNodeL( KPresenceList );
    CleanupStack::PushL( child2 );
    CXdmNodeAttribute* name2 = child2->CreateAttributeL( KPresenceName );
    CleanupStack::PushL( name2 );
    name2->SetAttributeValueL( KPresenceBlockedList );

    // send to the server and start wait a response
    iXdmDoc->AppendL();
    UpdateXdmsL();
    iXdmState = aCreateRules ? ECreateXdmLists : ECreateXdmListsOnly;
    
    CleanupStack::Pop( name2 );
    CleanupStack::Pop( child2 );
    CleanupStack::Pop( name );
    CleanupStack::Pop( child1 );
    CleanupStack::Pop( root );
    DP_SDA("CPresencePluginXdmUtils::CreateXdmListsL end");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::GetEntitiesInListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::GetEntitiesInListL(
    const TDesC&  aList,
    MXIMPObjectCollection& aColl )
    {
    DP_SDA("CPresencePluginXdmUtils::GetEntitiesInListL ");
    using namespace NPresencePlugin::NPresence;

    CXdmDocumentNode* buddylist = DoGetBuddyListL( aList );

    // make a collection of MXIMPPresentityGroupMemberInfo
    RPointerArray<CXdmDocumentNode> entries;
    CleanupClosePushL(entries);       // <<< entries

    RPointerArray<CXdmDocumentNode> nodes;
    CleanupClosePushL(nodes);         // <<< nodes

    buddylist->Find( KPresenceEntry, entries );
    DP_SDA("CPresencePluginXdmUtils::GetEntitiesInListL entries");
    TInt count = entries.Count();
    DP_SDA2("GetEntitiesInListL entries count %d",count);
    for ( TInt i=0; i < count; i++ )
        {
        DP_SDA("CPresencePluginXdmUtils::GetEntitiesInListL 1");
        // create here MXIMPPresentityGroupMemberInfo
        MPresentityGroupMemberInfo* info =
            iConnObs.PresenceObjectFactoryOwn().
                NewPresentityGroupMemberInfoLC();

        // ID
        CXdmDocumentNode* entry = entries[i];
        CXdmNodeAttribute* attr = entry->Attribute( KPresenceUri );
        if ( attr )
            {
            DP_SDA("CPresencePluginXdmUtils::GetEntitiesInListL 2");
            MXIMPIdentity* id =
                iConnObs.ObjectFactory().NewIdentityLC();//<< id
            id->SetIdentityL( attr->AttributeValue() );
            info->SetGroupMemberIdL( id );
            CleanupStack::Pop(); // >> id
            }

        // Display name
        TInt err = entry->Find( KPresenceDisplayName, nodes );
        // Notice: assume there is only one display-name
        if ( !err )
            {
            DP_SDA("CPresencePluginXdmUtils::GetEntitiesInListL display name");
            CXdmDocumentNode* disName = nodes[0];
                HBufC16* uri16 = NULL;
                uri16 = 
                    CnvUtfConverter::ConvertToUnicodeFromUtf8L( 
                        disName->LeafNodeContent() );
                    
                CleanupStack::PushL( uri16 );  // << uri16
                info->SetGroupMemberDisplayNameL( uri16->Des() );
                CleanupStack::PopAndDestroy( uri16 );  // >> uri16
            }

		DP_SDA("CPresencePluginXdmUtils::GetEntitiesInListL 3");
        aColl.AddObjectL( info );
        
        //if contact is from buddylist add it to virtual subscribed list too
        if( !aList.CompareF( KPresenceBuddyList ) )
            {
            DP_SDA("GetEntitiesInListL add virtualgroup");
            //Add identity to virtual list
            iConnObs.SubscribedContacts()->AddEntityL( 
            	info->GroupMemberId().Identity() );
            }
            
        CleanupStack::Pop(); // >> info
        }

    CleanupStack::PopAndDestroy( &nodes ); // >>> nodes
    CleanupStack::PopAndDestroy( &entries ); // >>> entries*/
    DP_SDA("CPresencePluginXdmUtils::GetEntitiesInListL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::AddIdentityToVirtualListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::AddIdentityToVirtualListL( const TDesC&  aList )
	{
	DP_SDA("CPresencePluginXdmUtils::AddIdentityToVirtualListL ");
	using namespace NPresencePlugin::NPresence;

	if ( aList.Compare( KPresenceBlockedList ) == NULL )
	    {
	    iBlockedContacts.ResetAndDestroy();
	    }

    CXdmDocumentNode* buddylist = DoGetBuddyListL( aList );

    // make a collection of MXIMPPresentityGroupMemberInfo
    RPointerArray<CXdmDocumentNode> entries;
    CleanupClosePushL(entries);       // <<< entries

    buddylist->Find( KPresenceEntry, entries );
    DP_SDA("CPresencePluginXdmUtils::AddIdentityToVirtualListL entries");
    TInt count = entries.Count();
    DP_SDA2("AddIdentityToVirtualListL entries count %d",count);
    for ( TInt i=0; i < count; i++ )
        {
        DP_SDA("CPresencePluginXdmUtils::AddIdentityToVirtualListL 1");
        // ID
        CXdmDocumentNode* entry = entries[i];
        CXdmNodeAttribute* attr = entry->Attribute( KPresenceUri );
        if ( attr )
            {
            if( !aList.CompareF( KPresenceBuddyList ) )
                {
                DP_SDA("AddIdentityToVirtualListL add to buddies");
                //Add identity to virtual list
                iConnObs.SubscribedContacts()->AddEntityL( 
                		attr->AttributeValue() );
                }
            else if ( !aList.CompareF( KPresenceBlockedList ) )
                {
                DP_SDA("AddIdentityToVirtualListL add to blocked");
                HBufC* withoutPrefix = 
                    iConnObs.InternalPresenceAuthorization().
                        PluginData().RemovePrefixLC( attr->AttributeValue() );
                CleanupStack::Pop( withoutPrefix );
                iBlockedContacts.AppendL( withoutPrefix );
                }
            }
        }
    CleanupStack::PopAndDestroy( &entries ); // >>> entries
    DP_SDA("CPresencePluginXdmUtils::AddIdentityToVirtualListL end");
	}
    
// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::DoGetBuddyListL()
// ---------------------------------------------------------------------------
//
CXdmDocumentNode* CPresencePluginXdmUtils::DoGetBuddyListL( 
    const TDesC&  aParent )
    {
    DP_SDA("CPresencePluginXdmUtils::DoGetBuddyListL");
    CXdmDocumentNode* root = iXdmDoc->DocumentRoot();
    
    CXdmDocumentNode* ret = SearchListUnderParentL( root, aParent );
    if ( !ret )
        {
        DP_SDA("CPresencePluginXdmUtils::DoGetBuddyListL !found");
        User::Leave( KErrNotFound );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::DoGetUserListL()
// ---------------------------------------------------------------------------
//
CXdmDocumentNode* CPresencePluginXdmUtils::DoGetUserListL(
    const TDesC& aList, CXdmDocumentNode* aBuddyList )
    {
    DP_SDA("CPresencePluginXdmUtils::DoGetUserListL");
    CXdmDocumentNode* listNode = NULL;
    CXdmNodeAttribute* attr = NULL;
    TBool found( EFalse );

    using namespace NPresencePlugin::NPresence;

    RPointerArray<CXdmDocumentNode> lists;
    CleanupClosePushL( lists );

    User::LeaveIfError( aBuddyList->Find( KPresenceEntry, lists ));
    TInt count = lists.Count();
    DP_SDA2("CPresencePluginXdmUtils::DoGetUserListL count %d",count);
    for ( TInt i=0; i < count; i++ )
        {
        listNode = lists[i];
        attr = listNode->Attribute( KPresenceUri );
        if ( attr && !attr->AttributeValue().CompareF( aList ))
            {
            // List is found
            found = ETrue;
            break;
            }
        }
    if ( !found )
        {
        DP_SDA("CPresencePluginXdmUtils::DoGetUserListL !found");
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( &lists );
    DP_SDA("CPresencePluginXdmUtils::DoGetUserListL end");
    return listNode;
    }
        
// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::UpdateXdmsL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginXdmUtils::UpdateXdmsL()
    {
    DP_SDA("CPresencePluginXdmUtils::UpdateXdmsL");
    iXdmEngine->UpdateL( iXdmDoc, iStatus );
    if ( !IsActive() )
    	{
    	DP_SDA("CPresencePluginXdmUtils::UpdateXdmsL SetActive");
    	SetActive();
    	}
    DP_SDA("CPresencePluginXdmUtils::UpdateXdmsL end");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::UpdateXdmRulesL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginXdmUtils::UpdateXdmRulesL()
    {
    DP_SDA("CPresencePluginXdmUtils::UpdateXdmRulesL");
    iXdmState = EUpdateXdmRules;
    
	if( iRulesUpdateState == EStateInitializeRules )
    	{
    	User::LeaveIfError( iXdmPresRules->UpdateXdmRulesL( this ));
    	}
    //Add entity to presrules too
    if ( iRulesUpdateState == EStateAddToWhiteList )
        {
        DP_SDA("EStateAddToWhiteList");
        iXdmPresRules->AddEntityToWhiteListL( 
        		iEntityUri->Des(), this );
        }
    if ( iRulesUpdateState == EStateAddToBlockList )
        {
        DP_SDA("EStateAddToBlockList");
        iXdmPresRules->AddEntityToBlockedRulesL( 
        		iEntityUri->Des(), this );
        }
    if ( iRulesUpdateState == EStateRemoveFromWhiteList)
        {
        DP_SDA("EStateRemoveFromWhiteList");
        iXdmPresRules->RemoveEntityFromWhiteListL( 
        		iEntityUri->Des(), this );
        }
    if ( iRulesUpdateState == EStateRemoveFromBlackList)
        {
        DP_SDA("EStateRemoveFromBlackList");
        iXdmPresRules->RemoveEntityBlockedRulesL( 
        		iEntityUri->Des(), this );
        }
    DP_SDA("CPresencePluginXdmUtils::UpdateXdmRulesL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::DoHandlePresUpdateDocumentL()
// ---------------------------------------------------------------------------
//      
void CPresencePluginXdmUtils::DoHandlePresUpdateDocumentL( TInt aErrorCode )
    {
    DP_SDA("CPresencePluginXdmUtils::DoHandlePresUpdateDocumentL");
    
    // check if error
    CheckIfErrorL( aErrorCode );

    if ( iXdmState == EGetXdmRules )
        {
        DP_SDA("DoHandlePresUpdateDocumentL EGetXdmRules");
        iXdmState = EStateIdle;
		if ( aErrorCode && aErrorCode == KXcapErrorHttpNotFound )
            {
            DP_SDA("DoHandlePresUpdateDocumentL EGetXdmRules ini rules ERROR");
			InitializePresRulesL();
            }
        else
            {
            DP_SDA("DoHandlePresUpdateDocumentL EGetXdmRules SEND COMPLETE ");
            // Complete with ok or error the last initial opreration

            // update presence cache if needed before complete client req.
            iPresXdmOk = ETrue;

            if ( iBlockedContacts.Count() > 0 )
                {
                iConnObs.InternalPresenceAuthorization().
                    IsBlockedContactFriendRequestL(
                         *iBlockedContacts[ iBlockedContacts.Count() - 1 ],
                         *this, iStatus );

                iXdmState = EUpdateBlockedContactPresenceCache;
                SetActive();
                }
            else
                {
                CompleteClientReq( aErrorCode );
                }
            }
        }
    else if ( iXdmState == EUpdateXdmRules )
        {
        DP_SDA("DoHandlePresUpdateDocumentL EUpdateXdmRules SEND COMPLETE ");
        // Complete with ok or error the last initial opreration
        CompleteClientReq( aErrorCode );
        }
    else if ( iXdmState == ECreateXdmRules )
        {
        DP_SDA("DoHandlePresUpdateDocumentL ECreateXdmRules SEND COMPLETE ");
        iPresXdmOk = ETrue;
        CompleteClientReq( aErrorCode );
        }

	else if ( iXdmState == EUpdateXdmList)
		{
		DP_SDA("DoHandlePresUpdateDocumentL EUpdateXdmList SEND COMPLETE");
		CompleteClientReq( aErrorCode );
		}
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::CompleteClientReq()
// ---------------------------------------------------------------------------
//               
void CPresencePluginXdmUtils::CompleteClientReq( TInt aStatus )
    {
    DP_SDA("CPresencePluginXdmUtils::CompleteClientReq");
    DP_SDA2("CPresencePluginXdmUtils::CompleteClientReq status %d ", aStatus);
    iXdmState = EStateIdle;
    iOperation = ENoOperation;
    TRequestStatus* s = iClientStatus;
    User::RequestComplete( s, aStatus );
    DP_SDA("CPresencePluginXdmUtils::CompleteClientReq end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::HandlePresUpdateDocumentL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::HandlePresUpdateDocumentL( 
    TInt aErrorCode )
    {
#ifdef _DEBUG
    DP_SDA2("HandlePresUpdateDocumentL errorCode %d ", aErrorCode );
    DP_SDA2("HandlePresUpdateDocumentL mystatus %d ", iStatus.Int());
    DP_SDA2("HandlePresUpdateDocumentL state %d ", iXdmState);
    DP_SDA2("HandlePresUpdateDocumentL ClientStatus %d",iClientStatus->Int() );
#endif
    
    DP_SDA("CPresencePluginXdmUtils::HandlePresUpdateDocumentL");      
    if ( aErrorCode == KErrTimedOut  )
		{
		DP_SDA("CPresencePluginXdmUtils::HandlePresUpdateDocumentL TimeOut");                        
		aErrorCode  = 0;
		}
	    
    TRAPD( err, DoHandlePresUpdateDocumentL( aErrorCode ));
    if ( err )
        {
        DP_SDA("CPresencePluginXdmUtils::HandlePresUpdateDocumentL complete");
        // Complete with ok or error the last initial opreration
        CompleteClientReq( err );
        }
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::SearchListUnderParentL
// ---------------------------------------------------------------------------
//
TBool CPresencePluginXdmUtils::CompareUriWithoutPrefixL( 
    const TDesC& aUri, const TDesC& aAttribute )
    {
    DP_SDA("CPresencePluginXdmUtils::CompareUriWithoutPrefixL");
    
    TBool match( EFalse );
    
    RBuf uriWithoutPrefix;
    CleanupClosePushL( uriWithoutPrefix );
    uriWithoutPrefix.CreateL( aUri );
    TInt prefixLocation = uriWithoutPrefix.Locate( ':' );
                   
    if ( KErrNotFound != prefixLocation )
        {
        uriWithoutPrefix.Delete( 0, ( prefixLocation + 1 ) );
        }
        
     RBuf attributeWithoutprefix;
     CleanupClosePushL( attributeWithoutprefix );
     attributeWithoutprefix.CreateL( aAttribute );
     prefixLocation = attributeWithoutprefix.Locate( ':' );
                   
     if ( KErrNotFound != prefixLocation )
         {
         attributeWithoutprefix.Delete( 0, ( prefixLocation + 1 ) );
         }
                    
     DP_SDA2("    --> uri without prefix=%S", &uriWithoutPrefix );
     DP_SDA2("    --> attribute without prefix=%S", &attributeWithoutprefix );                 
                
     if ( uriWithoutPrefix.Compare( attributeWithoutprefix ) == 0 )
         {
         match = ETrue;
         }
                   
    CleanupStack::PopAndDestroy( &attributeWithoutprefix );
    CleanupStack::PopAndDestroy( &uriWithoutPrefix );
    
    return match;
    }


// ---------------------------------------------------------------------------
// CPresencePluginXdmUtils::DoUpdateBlockedContactPresenceCacheL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::DoUpdateBlockedContactPresenceCacheL(
    TInt aMyStatus )
    {
    DP_SDA( "CPresencePluginXdmUtils::DoUpdateBlockedContactPresenceCache" );

    const TInt KTextBufferSize = 10;
    TInt contactsCount = iBlockedContacts.Count();

    if ( contactsCount > 0 )
        {
        if ( iPresenceContactsAsyncReqResult )
            {
            DP_SDA(" RunL  -write blocked status to cache" );
            TBuf<KTextBufferSize> buf;
            buf.Copy( KBlockedExtensionValue );
            iConnObs.InternalPresenceAuthorization().PluginData().
                WriteStatusToCacheL( *iBlockedContacts[ contactsCount - 1 ],
                     MPresenceBuddyInfo2::EUnknownAvailability,
                     buf,
                     KNullDesC() );
           }
       delete iBlockedContacts[ contactsCount - 1 ];
       iBlockedContacts.Remove( contactsCount - 1 );

       if ( iBlockedContacts.Count() > 0 )
           {
            iConnObs.InternalPresenceAuthorization().
                IsBlockedContactFriendRequestL(
                    *iBlockedContacts[ iBlockedContacts.Count() - 1 ],
                    *this, iStatus );

           iXdmState = EUpdateBlockedContactPresenceCache;
           SetActive();
           }
       else
           {
           CompleteClientReq( aMyStatus );
           }
        }
    }


// ---------------------------------------------------------------------------
// From MPresencePluginContactsObs
// CPresencePluginXdmUtils::RequestComplete
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmUtils::RequestComplete( TAny* aResult,
    TPresenceContactsOperation /*aOperation*/, TInt aError )
    {
    iPresenceContactsAsyncReqResult = EFalse;
    if ( NULL != aResult && KErrNone == aError )
        {
        iPresenceContactsAsyncReqResult =
            *static_cast<TBool*>( aResult );
        }
    }

// End of file
