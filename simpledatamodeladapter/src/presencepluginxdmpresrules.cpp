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

#include <XdmEngine.h>
#include <XdmProtocolInfo.h>
#include <XdmDocument.h>
#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include <XdmErrors.h>
#include <XdmProtocolUidList.h>

#include "presencepluginxdmpresrules.h"
#include "mpresrulesasynchandler.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::NewLC()
// ---------------------------------------------------------------------------
//
CPresencePluginXdmPresRules* CPresencePluginXdmPresRules::NewLC( 
	const TInt aSettingId, TBool aLocalMode )
    {
    CPresencePluginXdmPresRules* self =
    	new (ELeave) CPresencePluginXdmPresRules( aSettingId );
    CleanupStack::PushL( self );
    self->ConstructL( aSettingId, aLocalMode );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::NewL()
// ---------------------------------------------------------------------------
//
CPresencePluginXdmPresRules* CPresencePluginXdmPresRules::NewL(
    const TInt aXmdId, TBool aLocalMode )
    {
    CPresencePluginXdmPresRules* self =
        new( ELeave ) CPresencePluginXdmPresRules( aXmdId );
    CleanupStack::PushL( self );
    self->ConstructL( aXmdId, aLocalMode  );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::CPresencePluginXdmPresRules()
// ---------------------------------------------------------------------------
//
CPresencePluginXdmPresRules::CPresencePluginXdmPresRules( const TInt aXdmId )
    :CActive( EPriorityStandard ),
    iSettingsId( aXdmId ),
    iXdmState( EStateIdle ), //Listeting xdState
    iRulesUpdateState(EStateNoOperation),
	iPresXdmOk( EFalse ),
	iAsyncReq( ENoOperation )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::ConstructL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmPresRules::ConstructL( 
	const TInt aXmdId,
	TBool aLocalMode )
    {
    DP_SDA2("CPresencePluginXdmPresRules::ConstructL - aXdmId: %d", aXmdId );
    using namespace NPresencePlugin::NPresence;
    if ( !aLocalMode )
        {
        DP_SDA("CPresencePluginXdmPresRules::ConstructL Normal Mode");
        CXdmProtocolInfo* xdmProtocolInfo = CXdmProtocolInfo::NewL( aXmdId );
        CleanupStack::PushL( xdmProtocolInfo );
        DP_SDA("CPresencePluginXdmPresRules::ConstructL - protocol info created");
        xdmProtocolInfo->SetCacheUsage( EFalse );
        DP_SDA("CPresencePluginXdmPresRules::ConstructL - create xdm engine");
        iXdmEngine = CXdmEngine::NewL( *xdmProtocolInfo );
        CleanupStack::PopAndDestroy( xdmProtocolInfo );
        DP_SDA("CPresencePluginXdmPresRules::ConstructL - xdm engine created");
        
        DP_SDA("CPresencePluginXdmPresRules::ConstructL - create doc model");
		//Create empty document model
		iPresRulesDoc =
            iXdmEngine->CreateDocumentModelL( 
                KPresencePresRules, EXdmOmaPresenceRules );
        DP_SDA("CPresencePluginXdmPresRules::ConstructL - create doc model ok");        
        }
    else
        {
        DP_SDA("CPresencePluginXdmPresRules::ConstructL LocalMode Mode");
        //Lets Try localmode 
        CXdmProtocolInfo* xdmProtocolInfo =
            CXdmProtocolInfo::NewL( 0, KLocalProtocol, KNullDesC );
        CleanupStack::PushL( xdmProtocolInfo );
        iXdmEngine = CXdmEngine::NewL( *xdmProtocolInfo );
        CleanupStack::PopAndDestroy( xdmProtocolInfo );

				//Create empty document model
		iPresRulesDoc =
            iXdmEngine->CreateDocumentModelL( 
                KPresencePresRules, EXdmOmaPresenceRules );
        }
    DP_SDA("CPresencePluginXdmPresRules::ConstructL create root");
	CreateRootIfNeededL();
	DP_SDA("CPresencePluginXdmPresRules::ConstructL - out" );
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::~CPresencePluginXdmPresRules()
// ---------------------------------------------------------------------------
//
CPresencePluginXdmPresRules::~CPresencePluginXdmPresRules()
    {
	DP_SDA("CPresencePluginXdmPresRules::~CPresencePluginXdmPresRules");
    
    if ( iPresRulesDoc )
        {
        DP_SDA("CPresencePluginXdmPresRules::~CPresencePluginXdmPresRules 2");
        if ( iXdmEngine )
            {
            DP_SDA("CPresencePluginXdmPresRules::~CPresencePluginXdmPresRules 3");
            TRAP_IGNORE( iXdmEngine->DeleteDocumentModelL( iPresRulesDoc ));
            }
        }
       
    delete iXdmEngine;  
    delete iEntityUri;

    DP_SDA("CPresencePluginXdmPresRules::~CPresencePluginXdmPresRules Done");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::GetXdmRulesL()
// ---------------------------------------------------------------------------
//    
TInt CPresencePluginXdmPresRules::GetXdmRulesL( 
    MPresRulesAsyncHandler* const aHandler )
    {
    DP_SDA("CPresencePluginXdmPresRules::GetXdmRulesL");
	__ASSERT_ALWAYS(aHandler, User::Leave(KErrArgument));
	if (IsActive())
        return KErrAlreadyExists;
	iAsyncReq = EXdmGetRulesFromServer;
	iAsyncHandler = aHandler;
	using namespace NPresencePlugin::NPresence;
	
    if ( !iPresXdmOk )
        {
        DP_SDA("CPresencePluginXdmPresRules::GetXdmRulesL !iPresXdmOk");

                
        iPresRulesDoc->FetchDataL();
        DP_SDA("CPresencePluginXdmPresRules::GetXdmRulesL !iPresXdmOk end");
        }
    else
        {
        // Document already exists, no need to search from a server
        DP_SDA("CPresencePluginXdmPresRules::GetXdmRulesL Already exists");
        iStatus = KRequestPending;
        SetActive();        
        }
  	return KErrNone;
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::UpdateXdmRulesL()
// ---------------------------------------------------------------------------
//     
TInt CPresencePluginXdmPresRules::UpdateXdmRulesL( 
	MPresRulesAsyncHandler* const aHandler )
    {
    __ASSERT_ALWAYS(aHandler, User::Leave(KErrArgument));
    if (IsActive())
		{
		return KErrAlreadyExists;
		}
	iAsyncReq = EXdmUpdateRules;
    iAsyncHandler = aHandler;
    
    iPresRulesDoc->AppendL();
    iXdmEngine->UpdateL(iPresRulesDoc, iStatus);
    SetActive();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::MakeInitialRulesDocumentL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginXdmPresRules::MakeInitialRulesDocumentL( 
    MPresRulesAsyncHandler* const aHandler )
    {
    DP_SDA("CPresencePluginXdmPresRules::MakeInitialXdmsDocumentL");
   	__ASSERT_ALWAYS(aHandler, User::Leave(KErrArgument));
	iAsyncReq = EXdmInitialPresDoc;
	iAsyncHandler = aHandler;
	
	CXdmDocumentNode* root = iPresRulesDoc->DocumentRoot();
	if ( !root )
	    {
	    DP_SDA("CPresencePluginXdmPresRules:: Create root");
	    root = iPresRulesDoc->CreateRootL();
	    }
   
    using namespace NPresencePlugin::NPresence;
    root->SetNameL( KPresenceRuleset );

    // send to the server and start wait a response
    iPresRulesDoc->AppendL( root );
    DP_SDA("CPresencePluginXdmPresRules::MakeInitialXdmsDocumentL append");
    iAsyncReq = EXdmInitialPresDoc;
    DP_SDA("CPresencePluginXdmPresRules::MakeInitialXdmsDocumentL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::AddWhiteListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmPresRules::AddWhiteListL( CXdmDocumentNode* aRoot )
    {
    DP_SDA("CPresencePluginXdmPresRules::AddWhiteList");
    using namespace NPresencePlugin::NPresence;
    using namespace NPresencePlugin::NPresenceTransformations;
    
    if ( aRoot->IsEmptyNode() )
        {
        DP_SDA("AddWhiteList is empty node");
        aRoot->SetEmptyNode( EFalse );
        }
    // whitelist
    CXdmDocumentNode* child2 = aRoot->CreateChileNodeL( KPresenceRule );
    CleanupStack::PushL( child2 );
    CXdmNodeAttribute* name2 = child2->CreateAttributeL( KPresenceId );
    CleanupStack::PushL( name2 );
    name2->SetAttributeValueL( KPresenceXdmWhiteList );
    DP_SDA("CPresencePluginXdmPresRules::AddWhiteList 2");
    
    CXdmDocumentNode* conditionsNode2 =
        child2->CreateChileNodeL( KPresenceConditions );

    conditionsNode2->CreateChileNodeL( KPresenceIdentity ); 
    CXdmDocumentNode* actionNode2 =
        child2->CreateChileNodeL( KPresenceAction );
        
    DP_SDA("CPresencePluginXdmPresRules::AddWhiteList 3"); 
    CXdmDocumentNode* subNode2 = actionNode2->CreateChileNodeL( KPresenceSub );
    DP_SDA("CPresencePluginXdmPresRules::MakeInitialXdmsDocumentL 4");
    
    subNode2->SetLeafNode( ETrue ); 
    subNode2->SetLeafNodeContentL( KPresenceAllow8 );
    DP_SDA("CPresencePluginXdmPresRules::AddWhiteList 5"); 
    
    //Add transformations
    /*<transformations>
    <pr:provide-services>
    <pr:all-services/>
    </pr:provide-services>
    
    <pr:provide-persons>
    <pr:all-persons/>
    </pr:provide-persons>
    
    <pr:provide-devices>
    <pr:all-devices/>
    </pr:provide-devices>
    
    <pr:provide-all-attributes/>
    </transformations>*/

    CXdmDocumentNode* trasform2 =
        child2->CreateChileNodeL( KPresenceTrasformations );
    CXdmDocumentNode* services2 =
        trasform2->CreateChileNodeL( KPresenceProvideServices );
    services2->CreateChileNodeL( KPresenceAllServices );
    CXdmDocumentNode* person2 =
        trasform2->CreateChileNodeL( KPresenceProvidePersons );
    person2->CreateChileNodeL( KPresenceAllPersons );
    CXdmDocumentNode* devices2 =
        trasform2->CreateChileNodeL( KPresenceProvidedevices );
    devices2->CreateChileNodeL( KPresenceAlldevices );
    trasform2->CreateChileNodeL( KPresenceProvideAllAttributes );
    
    CleanupStack::Pop( name2 );
    CleanupStack::Pop( child2 );
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::AddBlackList()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmPresRules::AddBlackListL( CXdmDocumentNode* aRoot )
    {
    DP_SDA("CPresencePluginXdmPresRules::AddBlackList");
    // add all the list nodes
    // buddylist
    using namespace NPresencePlugin::NPresence;
    using namespace NPresencePlugin::NPresenceTransformations;
    if ( aRoot->IsEmptyNode() )
        {
        DP_SDA("AddBlackList is empty node");
        aRoot->SetEmptyNode( EFalse );
        }
    CXdmDocumentNode* child1 = aRoot->CreateChileNodeL( KPresenceRule );
    CleanupStack::PushL( child1 );
    CXdmNodeAttribute* name = child1->CreateAttributeL( KPresenceId );
    CleanupStack::PushL( name );
    
    name->SetAttributeValueL( KPresenceXdmBlockRule );
    // add condition
    CXdmDocumentNode* conditionsNode =
        child1->CreateChileNodeL( KPresenceConditions );
    conditionsNode->CreateChileNodeL( KPresenceIdentity );
    
    CXdmDocumentNode* actionNode =
        child1->CreateChileNodeL( KPresenceAction );
        
    DP_SDA("CPresencePluginXdmPresRules::AddBlackList 1");
    CXdmDocumentNode* subNode =
        actionNode->CreateChileNodeL( KPresenceSub );
    DP_SDA("CPresencePluginXdmPresRules::MakeInitialXdmsDocumentL 1/2");
    subNode->SetLeafNode(ETrue);
    subNode->SetLeafNodeContentL( KPresenceBlock8 ); 
    DP_SDA("CPresencePluginXdmPresRules::AddBlackList 1/3");
    
    CXdmDocumentNode* trasform =
        child1->CreateChileNodeL( KPresenceTrasformations );
    CXdmDocumentNode* services =
        trasform->CreateChileNodeL( KPresenceProvideServices );
    services->CreateChileNodeL( KPresenceAllServices );
      
    CXdmDocumentNode* person =
        trasform->CreateChileNodeL( KPresenceProvidePersons );
    person->CreateChileNodeL( KPresenceAllPersons );
    CXdmDocumentNode* devices =
        trasform->CreateChileNodeL( KPresenceProvidedevices );
    devices->CreateChileNodeL( KPresenceAlldevices );
    trasform->CreateChileNodeL( KPresenceProvideAllAttributes );     
    CleanupStack::Pop( name );
    CleanupStack::Pop( child1 );
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::RunL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmPresRules::RunL( )
    {    
    DP_SDA("CPresencePluginXdmPresRules::RunL");    
    TPluginPresRulesXdmOperation origState = iAsyncReq;
    iAsyncReq = ENoOperation;
    TInt myStatus = iStatus.Int();
     
    DP_SDA2("CPresencePluginXdmPresRules::RunL mystatus %d ", myStatus);
    DP_SDA2("CPresencePluginXdmPresRules::RunL state %d ", origState);
    
    switch( origState )
        {
        case EXdmGetRulesFromServer:
            {
            DP_SDA("CPresencePluginXdmPresRules::RunL EXdmGetRulesFromServer");
            iAsyncHandler->HandlePresUpdateDocumentL( 
                iStatus.Int() );
            }     
        break;
        case EXdmInitialPresDoc:
            {
            DP_SDA("CPresencePluginXdmPresRules::RunL EXdmInitialPresDoc");
            iAsyncHandler->HandlePresUpdateDocumentL( 
                iStatus.Int() );
            }
        break;
        case EXdmUpdateRules:
            {
            DP_SDA("CPresencePluginXdmPresRules::RunL EXdmUpdateRules");
            iAsyncHandler->HandlePresUpdateDocumentL( 
                iStatus.Int() );
            }
        break;
        case EXdmUpdateRulesFromServer:
            {
            DP_SDA("RunL ExdmUpdateRulesFromServer");
			CreateRootIfNeededL(); //This check is added for Openser/OpenXcap
            iAsyncHandler->HandlePresUpdateDocumentL( iStatus.Int() );           
            iRulesUpdateState = EStateNoOperation;
            }
        break;
        case ENoOperation:
        default:
            break;
        }
     DP_SDA("CPresencePluginXdmPresRules::RunL end"); 
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::DoCancel()
// ---------------------------------------------------------------------------
//    
void CPresencePluginXdmPresRules::DoCancel(  )
    {
	DP_SDA("CPresencePluginXdmPresRules::DoCancel");
	iXdmEngine->CancelUpdate(iPresRulesDoc);
    iAsyncReq = ECancelDocument;   
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::RunError()
// ---------------------------------------------------------------------------
//
TInt CPresencePluginXdmPresRules::RunError( TInt /*aError*/ )
    {
	DP_SDA("CPresencePluginXdmPresRules::RunError");
    return KErrNone;
    }
        
// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::AddEntityToWhiteListL()
// ---------------------------------------------------------------------------
// 
void CPresencePluginXdmPresRules::AddEntityToWhiteListL(
	const TDesC&  aUri,
	MPresRulesAsyncHandler* const aHandler )
    {
    DP_SDA("CPresencePluginXdmPresRules::AddEntityToWhiteListL");
    iEntityUri = aUri.Alloc();
    DP_SDA2( "AddEntityToWhiteListL entity %S", iEntityUri );
    __ASSERT_ALWAYS(aHandler, User::Leave(KErrArgument));
    
    iAsyncReq = EXdmUpdateRules;
    iAsyncHandler = aHandler;
    iRulesUpdateState = EStateAddToWhiteList;
    DoOneAddUserL( KPresenceXdmWhiteList, iEntityUri->Des() );
    
    UpdateXdmRulesL( aHandler );   
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::RemoveEntityFromWhiteListL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginXdmPresRules::RemoveEntityFromWhiteListL(
	const TDesC& aUri,
	MPresRulesAsyncHandler* const aHandler )
    {
    DP_SDA("CPresencePluginXdmPresRules::RemoveEntityFromWhiteListL");
    iEntityUri =  aUri.Alloc();
    __ASSERT_ALWAYS(aHandler, User::Leave(KErrArgument));
    
    iAsyncReq = EXdmUpdateRules;
    iAsyncHandler = aHandler;
    iRulesUpdateState = EStateRemoveFromWhiteList;
    RemoveOneFromRuleL( KPresenceXdmWhiteList, iEntityUri->Des() );
    
    UpdateXdmRulesL( aHandler );
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::AddEntityToBlockedRulesL()
// ---------------------------------------------------------------------------
// 
void CPresencePluginXdmPresRules::AddEntityToBlockedRulesL(
    const TDesC& aUri,
	MPresRulesAsyncHandler* const aHandler )
    {
    DP_SDA("CPresencePluginXdmPresRules::AddEntityToBlockedRulesL");
    iEntityUri =  aUri.Alloc();
     __ASSERT_ALWAYS(aHandler, User::Leave(KErrArgument));

    iAsyncReq = EXdmUpdateRules;
    iAsyncHandler = aHandler;
    iRulesUpdateState = EStateAddToBlockList;
    DoOneAddUserL( KPresenceXdmBlockRule, iEntityUri->Des() );
    
    UpdateXdmRulesL( aHandler );    
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::RemoveEntityBlockedRulesL()
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmPresRules::RemoveEntityBlockedRulesL(
	const TDesC& aUri,
	MPresRulesAsyncHandler* const aHandler )
    {
    DP_SDA("CPresencePluginXdmPresRules::RemoveEntityBlockedRulesL");
    iEntityUri =  aUri.Alloc();
    __ASSERT_ALWAYS(aHandler, User::Leave(KErrArgument));
    
    iAsyncReq = EXdmUpdateRules;
    iAsyncHandler = aHandler;
	iRulesUpdateState = EStateRemoveFromBlackList;
    RemoveOneFromRuleL( KPresenceXdmBlockRule, iEntityUri->Des() );
    
    UpdateXdmRulesL( aHandler );
    DP_SDA("CPresencePluginXdmPresRules::RemoveEntityBlockedRulesL end");   
    }    

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::RemoveOneFromRule()
// ---------------------------------------------------------------------------
//    
void CPresencePluginXdmPresRules::RemoveOneFromRuleL(
   const TDesC& aRule, const TDesC&  aUri )
    {
    DP_SDA("CPresencePluginXdmPresRules::RemoveOneFromRule");
    CXdmDocumentNode* ruleNode = NULL;
    CXdmNodeAttribute* attr = NULL;
    TBool found( EFalse );
    TBool ruleAvaible( EFalse );
    
    using namespace NPresencePlugin::NPresence;

    RPointerArray<CXdmDocumentNode> rules;
    CleanupClosePushL( rules );

    RPointerArray<CXdmDocumentNode> nodes;
    CleanupClosePushL( nodes );
    
    RPointerArray<CXdmDocumentNode> ones;
    CleanupClosePushL( ones );
    
    if ( iPresRulesDoc )
        {
        DP_SDA("RemoveOneFromRule iPresRulesDoc ok");
        
        //Check if rule exist
        ruleAvaible = FindRuleL( aRule, EFalse );
        
        if ( ruleAvaible )
            {
            
            TRAPD( err, ruleNode =  GetRulesL( aRule, rules ) );
			if( !err )
			    {
			    found = ETrue;
			    }
            }
        }
    
    if ( found )
        {
        DP_SDA("CPresencePluginXdmPresRules::RemoveOneFromRule found");
        TInt findErr = ruleNode->Find( KPresenceOne, nodes );
        if ( !findErr )
            {
            TInt nodeCount = nodes.Count();
            for ( TInt i = 0; i < nodeCount; i++ )
                {
                CXdmDocumentNode* currNode = nodes[i];
                attr = ( currNode )->Attribute( KPresenceId );
                
                TBool uriMatch = CompareUriWithoutPrefixL( 
                    aUri, attr->AttributeValue() );
                 
                if ( attr && uriMatch )
                    {
                    DP_SDA("RemoveOneFromRule Remove");
                    //First remove currNode form model
                    iPresRulesDoc->RemoveFromModelL( currNode );
                    
                    //Check how many rules are in group now
                    DP_SDA("RemoveOneFromRule Find all one field from rule");
                    User::LeaveIfError( ruleNode->Find( KPresenceOne, ones ) );
                    TInt oneCount = ones.Count();
                    DP_SDA2("RemoveOneFromRule ones count %d", oneCount);
                    //if there is no any ones in rule remove it
                    if( KErrNone == oneCount )
                    	{
                    	//Remove group
        				DP_SDA("RemoveOneFromRule Remove GROUP");
        				RemoveRuleGroupL( aRule );	
                    	}
                    // Update model to server
                    iPresRulesDoc->AppendL();
                    break;
                    }
                }
            }
        }
      
    CleanupStack::PopAndDestroy( &ones ); // >>> nodes         
    CleanupStack::PopAndDestroy( &nodes ); // >>> nodes
    CleanupStack::PopAndDestroy( &rules ); // >>> lists
    }    
 
// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::DoOneAddUserL()
// ---------------------------------------------------------------------------
//
 void CPresencePluginXdmPresRules::DoOneAddUserL(
    const TDesC& aRule, const TDesC&  aUri )
    {
    DP_SDA("CPresencePluginXdmPresRules::DoOneAddUserL");
    using namespace NPresencePlugin::NPresence;
    
    CXdmDocumentNode* ruleNode = NULL;
    RPointerArray<CXdmDocumentNode> identityNode;
    CleanupClosePushL( identityNode );
    
    TBool found( EFalse );
    TBool ruleAvaible( EFalse );
    
    using namespace NPresencePlugin::NPresence;

    RPointerArray<CXdmDocumentNode> rules;
    CleanupClosePushL( rules );
    

    DP_SDA("CPresencePluginXdmPresRules::DoOneAddUserL iPresRulesDoc");
    if ( iPresRulesDoc )
        {
        DP_SDA("DoOneAddUserL iPresRulesDoc Exist");
        
        ruleAvaible = FindRuleL( aRule, ETrue );
        
        if ( ruleAvaible )
            {
			TRAPD( err, ruleNode =  GetRulesL( aRule, rules ) );
			if( !err )
			    {
			    found = ETrue;
			    }
            }
        }
   
    //Finaly add entity to rules
    if ( found )
        {
        //Check if enity already in list
        if( !CheckIfOneExistL( aRule, aUri ) )
            {
            DP_SDA("CPresencePluginXdmPresRules::DoOneAddUserL Found");
            // Find identityNode inside rulenode
            User::LeaveIfError( 
            ruleNode->Find( KPresenceIdentity, identityNode ) );
            if ( identityNode[0] )    
                {
                if ( identityNode[0]->IsEmptyNode() )
                    {
                    DP_SDA("DoOneAddUserL rule node empty");
                    identityNode[0]->SetEmptyNode( EFalse );
                    }
                
                // create new one node inside identity
                CXdmDocumentNode* newNode =
                    identityNode[0]->CreateChileNodeL( KPresenceOne );
                CXdmNodeAttribute* attributeOneId =
                    newNode->CreateAttributeL( KPresenceId );
                
                attributeOneId->SetAttributeValueL( aUri );
                iPresRulesDoc->AppendL();
                DP_SDA("DoOneAddUserL added");
                }
            }
        }
    else
        {
        DP_SDA("CPresencePluginXdmPresRules::DoOneAddUserL error not found");
        User::Leave( KErrNotFound );
        } 
    DP_SDA("CPresencePluginXdmPresRules::DoOneAddUserL end");     
    CleanupStack::PopAndDestroy( 2 );
              
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::GetRulesL()
// ---------------------------------------------------------------------------
//
CXdmDocumentNode* CPresencePluginXdmPresRules::GetRulesL( const TDesC& aRule,
    RPointerArray<CXdmDocumentNode>& aRuleArray )
    {
    DP_SDA("GetRulesL find Again if rule now exist");
    using namespace NPresencePlugin::NPresence;
    
    CXdmDocumentNode* ruleNode = NULL;
    CXdmNodeAttribute* attr = NULL;
    
    TBool result(EFalse);
    // find if rule tag is exist
    User::LeaveIfError( iPresRulesDoc->Find( KPresenceRule, aRuleArray ) );

    // How many rules found
    TInt count = aRuleArray.Count();
    DP_SDA2("GetRulesL ruleCount %d ", count);

    // Check is needed rule id exist
    for ( TInt i=0; i < count; i++ )
        {
        ruleNode = aRuleArray[i];
        TBuf<KBufSize100> buf;
        buf.Copy( 
        ruleNode->Attribute( KPresenceId )->AttributeValue() );
        DP_SDA2("GetRulesL ruleNode %S", &buf);

        attr = ruleNode->Attribute( KPresenceId );
        if ( attr && !attr->AttributeValue().CompareF( aRule ) )
            {
            DP_SDA("CPresencePluginXdmPresRules::GetRulesL FOUND");
            result = ETrue;
            break;
            }
        }
        
    // Make second compare
    if( !result )
        {
        User::LeaveIfError(iPresRulesDoc->Find( KPresenceRule, aRuleArray )); 
        TInt count2 = aRuleArray.Count();

        for ( TInt i=0; i < count2; i++ )
            {
            ruleNode = aRuleArray[i];
            attr = ruleNode->Attribute( KPresenceId );
            if ( attr && attr->AttributeValue().CompareF( aRule ) )
                {
                DP_SDA("CPresencePluginXdmPresRules::GetRulesL NOT FOUND");
                User::Leave( KErrNotFound );
                }
            }
        }

    return ruleNode;    
    }
                                
// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::FindRuleL()
// ---------------------------------------------------------------------------
//	
TBool CPresencePluginXdmPresRules::FindRuleL( 
    const TDesC& aRule, TBool aCreateRule )
    {
    DP_SDA("CPresencePluginXdmPresRules::FindRuleL");
    using namespace NPresencePlugin::NPresence;
    
   	 if ( iPresRulesDoc )
        {
        DP_SDA("CPresencePluginXdmPresRules::FindRuleL pres document exists");
        RPointerArray<CXdmDocumentNode> rule;
        CleanupClosePushL( rule );
        
        CXdmDocumentNode* ruleNode = NULL;
        CXdmNodeAttribute* attr = NULL;
        
        TBool found = EFalse;
        //Find all rules from document and but them to array
        DP_SDA("CPresencePluginXdmPresRules::FindRuleL find rule");
        TInt err = iPresRulesDoc->Find( KPresenceRule, rule );
        DP_SDA2("FindRuleL find rule error %d", err);
        
        // There is no any rules yet avaible create 
        if ( err == KErrNotFound && aCreateRule )
            {
            DP_SDA("FindRuleL Not any rules exist");
            if ( iRulesUpdateState == EStateAddToWhiteList )
                {
                DP_SDA("FindRuleL add white list");
                AddWhiteListL( iPresRulesDoc->DocumentRoot() );
                CleanupStack::PopAndDestroy( 1 );
                return ETrue;
                }
            else if ( iRulesUpdateState == EStateAddToBlockList  )
                {
                DP_SDA("FindRuleL add black list");
                AddBlackListL( iPresRulesDoc->DocumentRoot() );
                CleanupStack::PopAndDestroy( 1 );
                return ETrue;
                }
            }
            
        // if rule element found find if aRule exist
        TInt count = rule.Count();
        for ( TInt i=0; i < count; i++ )
            {
            ruleNode = rule[i];
            TBuf<KBufSize100> buf;
            buf.Copy( 
                ruleNode->Attribute( KPresenceId )->AttributeValue() );
            DP_SDA2("FindRuleL ruleNode %S", &buf);
      
            attr = ruleNode->Attribute( KPresenceId );
            if ( attr && !attr->AttributeValue().CompareF( aRule ) )
                {
                DP_SDA("CPresencePluginXdmPresRules::FindRuleL FOUND");
                found = ETrue;
                }
            }
                
        if ( !found && aCreateRule)
            {
            // if rule not found create it
            DP_SDA("CPresencePluginXdmPresRules::FindRuleL not found");
            //if rule not found add rule and check again
            if ( EStateAddToWhiteList == iRulesUpdateState )
                {
                DP_SDA("FindRuleL add white list2");
                AddWhiteListL( iPresRulesDoc->DocumentRoot() );
                CleanupStack::PopAndDestroy( 1 );
                return ETrue;
                }
                else if ( EStateAddToBlockList == iRulesUpdateState )
                {
                DP_SDA("FindRuleL add black list 2");
                AddBlackListL( iPresRulesDoc->DocumentRoot() );
                CleanupStack::PopAndDestroy( 1 );
                return ETrue;
                }         
            }
        
        CleanupStack::PopAndDestroy( 1 );    
        return ETrue;
        }
     else
        {
        // Should we create presrules document
        DP_SDA("CPresencePluginXdmPresRules::FindRuleL end");
        return EFalse;
        } 
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::CheckIfOneExistL
// ---------------------------------------------------------------------------
//
TBool CPresencePluginXdmPresRules::CheckIfOneExistL(
    const TDesC& aRule,
    const TDesC& aUri )
    {
    using namespace NPresencePlugin::NPresence;
    DP_SDA("CPresencePluginXdmPresRules::CheckIfOneExistL");
    
    using namespace NPresencePlugin::NPresence;
    
    CXdmNodeAttribute* attr = NULL;
    RPointerArray<CXdmDocumentNode> nodes;
    CleanupClosePushL( nodes );
    TBool state( EFalse );
    
    CXdmDocumentNode* root = iPresRulesDoc->DocumentRoot();
    CXdmDocumentNode* foundNode = SearchListUnderParentL( root, aRule );
    
    if ( foundNode )
        {
        DP_SDA("CPresencePluginXdmPresRules::CheckIfOneExistL check nodes");
        TInt findErr = foundNode->Find( KPresenceOne, nodes );
        DP_SDA2("CheckIfEnityExist - findErr: %d", findErr );

        if ( findErr )
            {
            DP_SDA("CheckIfOneExistL One not exist ");
            state = EFalse;
            }
        else
            {
            DP_SDA("CPresencePluginXdmPresRules::CheckIfOneExistL Else");
            TInt nodeCount = nodes.Count();
            for ( TInt i = 0; i < nodeCount; i++ )
                {
                CXdmDocumentNode* currNode = nodes[i];
                attr = ( currNode )->Attribute( KPresenceId );
                
                TBool uriMatch( EFalse );                
                uriMatch = CompareUriWithoutPrefixL( 
                    aUri, attr->AttributeValue() );
                
                if ( attr && uriMatch )
                    {
                    DP_SDA("CheckIfOneExistL entity exists");
                    //Enity is exist
                    state = ETrue;
                    break;
                    }
                }
            }
        }
    DP_SDA("CPresencePluginXdmPresRules::CheckIfOneExistL Destroy nodes");
    CleanupStack::PopAndDestroy( &nodes ); // >>> nodes    
    return state;
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::RemoveRuleGroupL( )
// ---------------------------------------------------------------------------
//
void CPresencePluginXdmPresRules::RemoveRuleGroupL( const TDesC& aRule )
    {
    DP_SDA("CPresencePluginXdmUtils::RemoveRuleGroupL");
    CXdmDocumentNode* ruleNode = NULL;
    CXdmNodeAttribute* attr = NULL;
    TBool found( EFalse );
    TBool ruleAvaible( EFalse );
    
    using namespace NPresencePlugin::NPresence;

    RPointerArray<CXdmDocumentNode> rules;
    CleanupClosePushL( rules );

    RPointerArray<CXdmDocumentNode> nodes;
    CleanupClosePushL( nodes );

    //Find rules
    if ( iPresRulesDoc )
        {
        DP_SDA("RemoveRuleGroupL iPresRulesDoc ok");
        
        //Check if rule exist
        ruleAvaible = FindRuleL( aRule, EFalse );
        
        if ( ruleAvaible )
            {
            // If rule found get count how many rules are in xml
            TInt findErr = iPresRulesDoc->Find( KPresenceRule, rules );
            DP_SDA2("RemoveRuleGroupL findError %d", findErr);
            if ( !findErr )
                {
                DP_SDA("RemoveRuleGroupL found EFalse");
                found = EFalse;
                }
            else
                {
                TInt count = rules.Count();
                DP_SDA2("RemoveRuleGroupL rule count %d", count);
                for ( TInt i=0; i < count; i++ )
                    {
                    ruleNode = rules[i];
                    attr = ruleNode->Attribute( KPresenceId );
                    if ( attr && !attr->AttributeValue().CompareF( aRule ))
                        {
                        found = ETrue;
                        break;
                        }
                    }
                }
            }
        }
        
    if ( found )
        {
        User::LeaveIfError( iPresRulesDoc->Find( KPresenceRule, nodes ));
        TInt ruleCount = nodes.Count();
        DP_SDA2("L node count %d", ruleCount);
        for ( TInt i = 0; i < ruleCount; i++ )
            {
            CXdmDocumentNode* currNode = nodes[i];
            attr = currNode->Attribute( KPresenceId );
            if ( attr && !attr->AttributeValue().CompareF( aRule ))
                {
                DP_SDA("RemoveRuleGroupL remove rule");
                // This is the user we are looking for deletion.
                iPresRulesDoc->RemoveFromModelL( currNode );
                //Update
                iPresRulesDoc->AppendL();
                break;
                }
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( &nodes ); // >>> nodes
    CleanupStack::PopAndDestroy( &rules ); // >>> rules
    DP_SDA("CPresencePluginXdmUtils::RemoveRuleGroupL end");
    } 

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::UpdateFromServerL()
// ---------------------------------------------------------------------------
//    
TInt CPresencePluginXdmPresRules::UpdateFromServerL( 
	MPresRulesAsyncHandler* const aHandler )
    {
    DP_SDA("CPresencePluginXdmPresRules::UpdateFromServer");
    using namespace NPresencePlugin::NPresence;
    __ASSERT_ALWAYS(aHandler, User::Leave(KErrArgument));

    if ( IsActive() )
	{
    return KErrAlreadyExists;
	}
	iPresRulesDoc->ResetContents();
    iPresRulesDoc->FetchDataL();
    iAsyncReq = EXdmUpdateRulesFromServer;
    iAsyncHandler = aHandler;
    iXdmEngine->UpdateL( iPresRulesDoc, iStatus );
    SetActive();
    DP_SDA("CPresencePluginXdmPresRules::UpdateFromServer end");
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::CreateRootIfNeededL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginXdmPresRules::CreateRootIfNeededL()
    {
    DP_SDA("CPresencePluginXdmPresRules::CreateRootIfNeededL");
    using namespace NPresencePlugin::NPresence;

    CXdmDocumentNode* myRootNode = iPresRulesDoc->DocumentRoot();
    
    if ( myRootNode )
        {
        DP_SDA("CreateRootIfNeededL document root exists");
        myRootNode->SetEmptyNode(EFalse);
        return;
        }
        
    DP_SDA("CPresencePluginXdmPresRules::CreateRootIfNeededL create root");    
    myRootNode = iPresRulesDoc->CreateRootL();
    DP_SDA("CreateRootIfNeededL create root pushL");
    CleanupStack::PushL( myRootNode );
    myRootNode->SetNameL( KPresenceRuleset );
    DP_SDA("CPresencePluginXdmPresRules::CreateRootIfNeededL create root POP");    
    CleanupStack::Pop( myRootNode );
    DP_SDA("CPresencePluginXdmPresRules::CreateRootIfNeededL create root end");
    }

void CPresencePluginXdmPresRules::ResetAndDestroy1( TAny* aPointerArray )
	{
    if ( aPointerArray )
        {
        RPointerArray<CXdmDocumentNode>* array =
            static_cast<RPointerArray<CXdmDocumentNode>*>( aPointerArray );
        array->ResetAndDestroy();
        array->Close();
        }
	}

void CPresencePluginXdmPresRules::ResetAndDestroy2( TAny* aPointerArray )
	{
    if ( aPointerArray )
        {
        RPointerArray<SXdmAttribute16>* array =
            static_cast<RPointerArray<SXdmAttribute16>*>( aPointerArray );
        array->ResetAndDestroy();
        array->Close();
        }
	}
// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::SearchListUnderParentL
// ---------------------------------------------------------------------------
//
CXdmDocumentNode* CPresencePluginXdmPresRules::SearchListUnderParentL(
    CXdmDocumentNode* aParent,
    const TDesC& aName )
    {
    DP_SDA("CPresencePluginXdmPresRules::SearchListUnderParentL");
    
    using namespace NPresencePlugin::NPresence;

    if ( !aParent )
        {
        DP_SDA("CPresencePluginXdmPresRules::SearchListUnderParentL return NULL");
        return NULL;
        }

    RPointerArray<CXdmDocumentNode> resultArray;
    RPointerArray<SXdmAttribute16>  attributeArray;

    CleanupClosePushL( resultArray );           // <<< resultArray
    CleanupClosePushL( attributeArray );        // <<< attributeArray

    SXdmAttribute16 attr;
    attr.iName.Set( KPresenceId );
    attr.iValue.Set( aName );
    attributeArray.Append( &attr );
    
    CXdmDocumentNode* currNode = NULL;
    
    aParent->Find( KPresenceRule, resultArray, attributeArray );

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

    CleanupStack::PopAndDestroy(); // clItem
    CleanupStack::PopAndDestroy(); // clItem2
    DP_SDA("CPresencePluginXdmPresRules::SearchListUnderParentL return");
    return currNode;
    }
   

// ---------------------------------------------------------------------------
// CPresencePluginXdmPresRules::SearchListUnderParentL
// ---------------------------------------------------------------------------
//
TBool CPresencePluginXdmPresRules::CompareUriWithoutPrefixL( 
    const TDesC& aUri, const TDesC& aAttribute )
    {
    DP_SDA("CPresencePluginXdmPresRules::CompareUriWithoutPrefixL");
    
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

// End of file
