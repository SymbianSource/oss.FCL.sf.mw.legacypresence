/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   XDM Engine document
*
*/




// INCLUDE FILES
#include <f32file.h>
#include "XdmEngine.h"
#include "XdmProtocol.h"
#include "XdmDocument.h"
#include "XdmOperation.h"
#include "XdmDocumentNode.h"
#include "XdmNodeAttribute.h"
#include "XdmOperationFactory.h"

// ----------------------------------------------------------
// CXdmDocument::CXdmDocument
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmDocument::CXdmDocument( CXdmEngine& aXdmEngine ) :
                                     CActive( EPriorityStandard ),
                                     iDocSubset( EFalse ),
                                     iXdmEngine( aXdmEngine )
                                                
    {
    }

// ----------------------------------------------------------
// CXdmDocument::BaseConstructL( 16-bit )
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmDocument::BaseConstructL( TInt aOperationFactoryUid,
                                            const TDesC& aDocumentName )
    {
    delete iDocumentName;
    iDocumentName = NULL;
    iDocumentName = HBufC::NewL( aDocumentName.Length() );
    iDocumentName->Des().Copy( aDocumentName );
    iOperationFactory = CXdmOperationFactory::NewL( aOperationFactoryUid );
    }

// ----------------------------------------------------------
// CXdmDocument::BaseConstructL( 8-bit )
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmDocument::BaseConstructL( TInt aOperationFactoryUid,
                                            const TDesC8& aDocumentName )
    {
    delete iDocumentName;
    iDocumentName = NULL;
    iDocumentName = HBufC::NewL( aDocumentName.Length() );
    iDocumentName->Des().Copy( aDocumentName );
    iOperationFactory = CXdmOperationFactory::NewL( aOperationFactoryUid );
    }

// ----------------------------------------------------
// CXdmDocument::~CXdmDocument
// 
// ----------------------------------------------------
//
EXPORT_C CXdmDocument::~CXdmDocument()
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::~CXdmDocument()" ) );
    #endif
    delete iDocumentName;
    EmptyOperationQueue();
    iChangeRequests.Close();
    delete iOperationFactory;
    }

// ---------------------------------------------------------
// CXdmDocument::EmptyOperationQueue
// 
// ---------------------------------------------------------
//
void CXdmDocument::EmptyOperationQueue()
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::EmptyOperationQueue()" ) );
    #endif
    MXdmOperation* operation = NULL;
    TInt count = iChangeRequests.Count();
    for( TInt i = 0;i < count;i++ )
        {
        operation = iChangeRequests[i];
        iChangeRequests.Remove( i );    
        operation->Destroy();
        operation = NULL;
        }
    }
    
// ----------------------------------------------------
// CXdmDocument::FetchDataL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocument::FetchDataL( CXdmDocumentNode* aDocNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::FetchDataL()" ) );
    #endif
    MXdmOperation* operation = iOperationFactory->FetchL( *this, aDocNode );
    CleanupStack::PushL( operation );
    User::LeaveIfError( iChangeRequests.Append( operation ) );
    CleanupStack::Pop();  //operation
    }

// ----------------------------------------------------
// CXdmDocument::ReplaceL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocument::ReplaceL( CXdmDocumentNode* aOldNode,
                                      CXdmDocumentNode* aNewNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::ReplaceL()" ) );
    #endif
    if( aOldNode->Parent() == aNewNode->Parent() )
        {
        MXdmOperation* operation = iOperationFactory->ReplaceL(
                                    *this, aOldNode, aNewNode );
        CleanupStack::PushL( operation );
        User::LeaveIfError( iChangeRequests.Append( operation ) );
        CleanupStack::Pop();  //operation
        }
    else
        {
        #ifdef _DEBUG
            iXdmEngine.WriteToLog( _L8( "  Different parents, leaves!" ) );
        #endif
        User::Leave( KErrGeneral );
        }
    }

// ----------------------------------------------------
// CXdmDocument::ReplaceL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocument::ReplaceL()
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::ReplaceL()" ) );
    #endif
    MXdmOperation* operation = iOperationFactory->ReplaceL( *this, NULL, NULL ); 
    CleanupStack::PushL( operation );
    User::LeaveIfError( iChangeRequests.Append( operation ) );
    CleanupStack::Pop();  //operation
    }

// ----------------------------------------------------
// CXdmDocument::InsertL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocument::InsertL( CXdmDocumentNode* aDocNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::InsertL()" ) );
    #endif
    __ASSERT_DEBUG( aDocNode != NULL, User::Panic( _L( "CXcapDocument" ), EDocNodeNull ) );
    MXdmOperation* operation = iOperationFactory->InsertL(
                                *this, aDocNode );
    CleanupStack::PushL( operation );
    User::LeaveIfError( iChangeRequests.Append( operation ) );
    CleanupStack::Pop();  //operation
    }

// ----------------------------------------------------
// CXdmDocument::AppendL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocument::AppendL( CXdmDocumentNode* aDocNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::AppendL()" ) );
    #endif
    MXdmOperation* operation = 
        iOperationFactory->AppendL( *this, aDocNode );
    CleanupStack::PushL( operation );
    User::LeaveIfError( iChangeRequests.Append( operation ) );
    CleanupStack::Pop();  //operation
    }
      
// ----------------------------------------------------
// CXdmDocument::DeleteDataL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocument::DeleteDataL( CXdmDocumentNode* aDocNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::DeleteDataL()" ) );
    #endif
    MXdmOperation* operation = aDocNode == NULL ?
        iOperationFactory->DeletionL( *this, NULL ) :
        iOperationFactory->DeletionL( *this, aDocNode );
    CleanupStack::PushL( operation );
    User::LeaveIfError( iChangeRequests.Append( operation ) );
    CleanupStack::Pop();  //operation
    }

// ----------------------------------------------------
// CXdmDocument::Find
// 
// ----------------------------------------------------
//
EXPORT_C TInt CXdmDocument::Find( const CXdmDocumentNode& aTargetNode,
                                  RPointerArray<CXdmDocumentNode>& aResultArray ) const
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::Find()" ) );
    #endif
    TInt error = KErrNone;
    CXdmDocumentNode* root = DocumentRoot();
    __ASSERT_ALWAYS( root != NULL, User::Panic( _L( "CXcapDocument" ), EDocNodeNull ) );
    if( root->NodeName().Compare( aTargetNode.NodeName() ) == 0 )
        {
        TBool found = EFalse;
        TInt attrCount = root->AttributeCount();
        TInt attrCount2 = aTargetNode.AttributeCount();
        if( attrCount > 0 && attrCount == attrCount2 )
            {
            SXdmAttribute16 attribute;
            for( TInt i = 0;i < attrCount;i++ )
                {
                attribute.iName.Set( aTargetNode.Attribute( i )->NodeName() );
                attribute.iValue.Set( aTargetNode.Attribute( i )->AttributeValue() );
                found = root->HasAttribute( attribute );
                }
            }
        error = found ? aResultArray.Append( root ) : KErrNotFound;
        }
    else
        {
        #ifdef _DEBUG
            iXdmEngine.WriteToLog( _L8( " Root does not match, search child nodes" ) );
        #endif
        error = root->Find( aTargetNode, aResultArray ); 
        }
    TInt count = aResultArray.Count();
    return error == KErrNone && count > 0 ? count : KErrNotFound;
    }
    
// ----------------------------------------------------
// CXdmDocument::Find
// 
// ----------------------------------------------------
//
EXPORT_C TInt CXdmDocument::Find( const TDesC& aNodeName,
                                  RPointerArray<CXdmDocumentNode>& aResultArray,
                                  const RPointerArray<SXdmAttribute16>& aAttributeArray ) const
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::Find()" ) );
    #endif
    TInt error = KErrNone;
    CXdmDocumentNode* root = DocumentRoot();
    __ASSERT_ALWAYS( root != NULL, User::Panic( _L( "CXcapDocument" ), EDocNodeNull ) );
    if( root->NodeName().Compare( aNodeName ) == 0 )
        {
        TBool found = EFalse;
        TInt attrCount = root->AttributeCount();
        if( attrCount > 0 )
            {
            for( TInt i = 0;i < attrCount;i++ )
                found = root->HasAttribute( *aAttributeArray[i] );
            }
        error = found ? aResultArray.Append( root ) : KErrNotFound;
        }
    else
        {
        #ifdef _DEBUG
            iXdmEngine.WriteToLog( _L8( " Root does not match, search child nodes" ) );
        #endif
        error = root->Find( aNodeName, aResultArray, aAttributeArray ); 
        }
    TInt count = aResultArray.Count();
    return error == KErrNone && count > 0 ? count : KErrNotFound;
    }

// ----------------------------------------------------
// CXdmDocument::Find
// 
// ----------------------------------------------------
//
EXPORT_C TInt CXdmDocument::Find( const TDesC& aNodeName,
                                  RPointerArray<CXdmDocumentNode>& aResultArray ) const
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::Find()" ) );
    #endif
    TInt error = KErrNone;
    CXdmDocumentNode* root = DocumentRoot();
    __ASSERT_ALWAYS( root != NULL, User::Panic( _L( "CXcapDocument" ), EDocNodeNull ) );
    if( root->NodeName().Compare( aNodeName ) == 0 )
        error = aResultArray.Append( root );
    else
        {
        #ifdef _DEBUG
            iXdmEngine.WriteToLog( _L8( " Root does not match, search child nodes" ) );
        #endif
        error = root->Find( aNodeName, aResultArray ); 
        }
    TInt count = aResultArray.Count();
    return error == KErrNone && count > 0 ? count : KErrNotFound;                            
    }
    
// ----------------------------------------------------
// CXcapDocument::operator==
// 
// ----------------------------------------------------
//
EXPORT_C TBool CXdmDocument::operator==( CXdmDocument& aDocument ) const
    {
    CXdmDocumentNode* root1 = DocumentRoot();
    CXdmDocumentNode* root2 = aDocument.DocumentRoot();
    __ASSERT_ALWAYS( root1 != NULL, User::Panic( _L( "CXcapDocument" ), EDocNodeNull ) );
    __ASSERT_ALWAYS( root2 != NULL, User::Panic( _L( "CXcapDocument" ), EDocNodeNull ) );
    return Name().Compare( aDocument.Name() ) == 0 ? *root1 == *root2 : EFalse;
    }
    
// ----------------------------------------------------
// CXdmDocument::Name
// 
// ----------------------------------------------------
//
EXPORT_C TPtrC CXdmDocument::Name() const
    {
    return iDocumentName != NULL ? iDocumentName->Des() : TPtrC();
    }

// ----------------------------------------------------
// CXdmDocument::Find
// 
// ----------------------------------------------------
//
EXPORT_C TInt CXdmDocument::Find( const TDesC& aAttributeName,
                                  RPointerArray<CXdmNodeAttribute>& aResultArray ) const
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::Find()" ) );
    #endif
    TInt error = KErrNone;
    CXdmDocumentNode* root = DocumentRoot();
    __ASSERT_ALWAYS( root != NULL, User::Panic( _L( "CXdmDocument" ), EDocNodeNull ) );
    error = DoFindAttributes( root, aAttributeName, aResultArray ); 
    TInt count = aResultArray.Count();
    return error == KErrNone && count > 0 ? count : KErrNotFound;                
    }

// ----------------------------------------------------
// CXdmDocument::RemoveFromModelL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocument::RemoveFromModelL( CXdmDocumentNode* aChileNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::RemoveFromModel()" ) );
    #endif
    if( !iDocSubset )
        {       
        CXdmDocumentNode* parent = aChileNode->Parent();
        parent != NULL ? parent->RemoveChileNodeL( aChileNode ) : User::Leave( KErrArgument);
        }
    else User::Leave( KErrGeneral );
    }

// ----------------------------------------------------
// CXdmDocument::ResetSubset
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocument::ResetSubset()
    {
    iDocSubset = EFalse;
    }
      
// ----------------------------------------------------
// CXdmDocument::AppendToModelL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocument::AppendToModelL( CXdmDocumentNode* aNewNode,
                                            CXdmDocumentNode* aParentNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::AppendToModelL()" ) );
    #endif
    if( !iDocSubset )
        {
        if( aNewNode != NULL && aParentNode != NULL )
            aParentNode->AppendChileNodeL( aNewNode );
        }
    else User::Leave( KErrGeneral );
    }
        
// ----------------------------------------------------
// CXdmDocument::ReplaceInModelL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocument::ReplaceInModelL( CXdmDocumentNode* aNewNode,
                                             CXdmDocumentNode* aTargetNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::ReplaceInModelL()" ) );
    #endif
    if( !iDocSubset )
        {
        CXdmDocumentNode* parent = aTargetNode->Parent();
        parent != NULL ? parent->ReplaceChileNodeL( aNewNode, aTargetNode ) : User::Leave( KErrArgument);                                    
        }
    else User::Leave( KErrGeneral );
    }
                                       
// ----------------------------------------------------
// CXdmDocument::DocumentSubsetL
// 
// ----------------------------------------------------
//
EXPORT_C CXdmDocumentNode* CXdmDocument::DocumentSubsetL( const TDesC& aNodePath )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::DocumentSubsetL()" ) );
    #endif
    TBool ready = EFalse;
    HBufC* temp = HBufC::NewLC( aNodePath.Length() );
    temp->Des().Copy( aNodePath );
    TPtr nodePath( temp->Des() );
    while( !ready && nodePath.Length() != 0 )
        {
        TPtrC part;
        TInt index = nodePath.LocateF( KXdmPathSeparatorChar );
        if( index > 0 )
            part.Set( nodePath.Left( index ) );
        else
            {
            part.Set( nodePath );
            ready = ETrue;
            }
        AppendPathPartL( part );
        nodePath.Delete( 0, index + 1 );
        }
    iDocSubset = ETrue;
    CleanupStack::PopAndDestroy();  //temp
    return CurrentExtent();
    }
        
// ----------------------------------------------------
// CXdmDocument::DoFindAttributes
// 
// ----------------------------------------------------
//
TInt CXdmDocument::DoFindAttributes( CXdmDocumentNode* aNode,
								     const TDesC& aAttributeName,
                                     RPointerArray<CXdmNodeAttribute>& aResultArray ) const
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocument::DoFindAttributes()" ) );
    #endif
    TInt error = KErrNone;
	CXdmDocumentNode* node = aNode;
	TInt attrCount = aNode->AttributeCount();
    for( TInt i = 0; i < attrCount; i++ )
        {
		CXdmNodeAttribute* attr = node->Attribute( i );
	   	if( !attr->NodeName().CompareF( aAttributeName ) )
    	   	error = aResultArray.Append( attr );
	   	}
	TInt nodeCount = node->NodeCount();
	for( TInt i = 0; i < nodeCount; i++ )
		{
		CXdmDocumentNode* child = node->ChileNode( i );
	   	DoFindAttributes ( child, aAttributeName, aResultArray );
		}  
    TInt count = aResultArray.Count();
    return error == KErrNone && count > 0 ? count : KErrNotFound;            
    }
        




