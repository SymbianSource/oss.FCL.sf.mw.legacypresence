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
* Description: CLocalReplacement
*
*/




// INCLUDES
#include <XdmDocumentNode.h>
#include "XdmXmlParser.h"
#include "LocalProtocol.h"
#include "LocalDocument.h"
#include "LocalReplacement.h"
#include "LocalDocumentNode.h"
#include "LocalNodeAttribute.h"
    
// ---------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CLocalReplacement::CLocalReplacement( CLocalDocument& aTargetDoc,
                                      CLocalDocumentNode* aOldNode,
                                      CLocalDocumentNode* aNewNode,
                                      CLocalOperationFactory& aOperationFactory ) :
                                      CLocalOperationBase( aTargetDoc, aOperationFactory ),
                                      iNewNode( ( CLocalDocumentNode* )aNewNode ),
                                      iTargetNode( ( CLocalDocumentNode* )aOldNode ),
                                      iOperationType( iTargetNode == NULL ?
                                      EXdmDocument : EXdmPartialDocument )                                                
    {
    }

// ---------------------------------------------------------
// Two-phased constructor.
//
// ---------------------------------------------------------
//
CLocalReplacement* CLocalReplacement::NewL( CLocalDocument& aParentDoc,
                                            CLocalDocumentNode* aOldNode,
                                            CLocalDocumentNode* aNewNode,
                                            CLocalOperationFactory& aOperationFactory )
    {
    CLocalReplacement* self = new ( ELeave ) CLocalReplacement( aParentDoc, aOldNode, aNewNode, aOperationFactory );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    CleanupStack::Pop();
    return self;
    }
      
// ---------------------------------------------------------
// CLocalReplacement::ExecuteL
//
// ---------------------------------------------------------
//
void CLocalReplacement::ExecuteL()
    {
    TPtrC fileName = iTargetDoc.XmlFilePath();
    TInt size = OpenDataFileL( fileName );
    if( size > 0 ) 
        {
        //Only a subset of a document
        if( iTargetDoc.IsSubset() )
            HandlePartialReplacementL( size );
        else if( iTargetNode != NULL )
            {
            CLocalDocumentNode* parent = ( CLocalDocumentNode* )iTargetNode->Parent();
            parent->ReplaceLocalL( iNewNode, iTargetNode );
            }
        else
            {
            iXmlFile.Close();
            ReplaceDataFileL();
            WriteFileL( iTargetDoc.DocumentRoot() );
            }
        }
    else WriteFileL( iTargetDoc.DocumentRoot() );
    iXmlFile.Close();
    }

// ---------------------------------------------------------
// CLocalReplacement::HandlePartialReplacementL
//
// ---------------------------------------------------------
//
void CLocalReplacement::HandlePartialReplacementL( TInt aDataLength )
    {
    TInt error = KErrNone;
    HBufC8* data = FetchXmlDataL( aDataLength );
    CleanupStack::PushL( data );
    TRAP( error, iXmlParser->ParseDocumentL( &iTargetDoc, *data ) );
    CXdmDocumentNode* parent = iTargetNode->Parent();
    RPointerArray<CXdmDocumentNode> parentResults;
    CleanupClosePushL( parentResults );
    RPointerArray<SXdmAttribute16> parentAttributes;
    CleanupClosePushL( parentAttributes );
    RPointerArray<CXdmDocumentNode> targetResults;
    CleanupClosePushL( targetResults );
    RPointerArray<SXdmAttribute16> targetAttributes;
    CleanupClosePushL( targetAttributes );
    TInt pAttrCount = parent->AttributeCount();
    TInt tAttrCount = iTargetNode->AttributeCount();
    if( pAttrCount > 0 )
        {
        SXdmAttribute16 attribute;
        for( TInt i = 0;i < pAttrCount;i++ )
            {
            attribute.iName.Set( parent->Attribute( i )->NodeName() );
            attribute.iValue.Set( parent->Attribute( i )->AttributeValue() );
            }
        User::LeaveIfError( parentAttributes.Append( &attribute ) );
        }
    if( tAttrCount > 0 )
        {
        SXdmAttribute16 attribute;
        for( TInt i = 0;i < tAttrCount;i++ )
            {
            attribute.iName.Set( iTargetNode->Attribute( i )->NodeName() );
            attribute.iValue.Set( iTargetNode->Attribute( i )->AttributeValue() );
            }
        User::LeaveIfError( targetAttributes.Append( &attribute ) );
        }
    iTargetDoc.Find( parent->NodeName(), parentResults, parentAttributes );
    iTargetDoc.Find( iTargetNode->NodeName(), targetResults, targetAttributes );
    if( parentResults.Count() == 1 && targetResults.Count() == 1 )
        {
        iXmlFile.Close();
        ReplaceDataFileL();
        CLocalDocumentNode* node = ( CLocalDocumentNode* )parentResults[0];
        CLocalDocumentNode* target = ( CLocalDocumentNode* )targetResults[0];
        node->ReplaceLocalL( iNewNode, target );
        WriteFileL( iTargetDoc.DocumentRoot() );
        }
    else User::Leave( KErrGeneral );
    CleanupStack::PopAndDestroy( 5 );  //targetAttributes, targetResults,
                                       //parentAttributes, parentResults, data
    }
                
// ---------------------------------------------------------
// Symbian OS default constructor may leave.
//
// ---------------------------------------------------------
//     
void CLocalReplacement::Destroy()
    {
    delete this;
    }                       

// ---------------------------------------------------------
// Destructor
//
// ---------------------------------------------------------
//
CLocalReplacement::~CLocalReplacement()
    {
    
    }

//  End of File  

