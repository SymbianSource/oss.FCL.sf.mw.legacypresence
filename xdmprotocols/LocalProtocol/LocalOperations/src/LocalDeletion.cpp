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
* Description: CLocalDeletion
*
*/




// INCLUDES
#include "XdmXmlParser.h"
#include "LocalProtocol.h"
#include "LocalDocument.h"
#include "LocalDeletion.h"
#include "LocalDocumentNode.h"
#include "LocalNodeAttribute.h"
    
// ---------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CLocalDeletion::CLocalDeletion( CLocalDocument& aTargetDoc,
                                CLocalDocumentNode* aDocumentSubset,
                                CLocalOperationFactory& aOperationFactory ) :
                                CLocalOperationBase( aTargetDoc, aOperationFactory ),
                                iDocumentSubset( aDocumentSubset ),
                                iOperationType( aDocumentSubset == NULL ?
                                EXdmDocument : EXdmPartialDocument )                                                      
    {
    }

// ---------------------------------------------------------
// CLocalDeletion::NewL
//
// ---------------------------------------------------------
//
CLocalDeletion* CLocalDeletion::NewL( CLocalDocument& aParentDoc,
                                      CLocalDocumentNode* aDocumentSubset,
                                      CLocalOperationFactory& aOperationFactory )
    {
    CLocalDeletion* self = new ( ELeave ) CLocalDeletion( aParentDoc, aDocumentSubset, aOperationFactory );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    CleanupStack::Pop();
    return self;
    }
      
// ---------------------------------------------------------
// CLocalDeletion::ExecuteL
//
// ---------------------------------------------------------
//
void CLocalDeletion::ExecuteL()
    {
    TPtrC fileName = iTargetDoc.XmlFilePath();
    TInt size = OpenDataFileL( fileName );
    if( size > 0 )    
        {
        //Only a subset of a document
        if( iTargetDoc.IsSubset() )
            HandlePartialDeletionL( size );
        else if( iDocumentSubset != NULL )
            {
            iTargetDoc.RemoveData( ( CLocalDocumentNode* )iDocumentSubset );
            iXmlFile.Close();
            ReplaceDataFileL();
            WriteFileL( iTargetDoc.DocumentRoot() );
            }
        else 
            {
            iXmlFile.Close();
            DeleteDataFileL();  //Delete the whole document
            }
        }
    else User::Leave( KErrNotFound );
    iXmlFile.Close();
    }

// ---------------------------------------------------------
// CLocalAddition::HandlePartialUpdateL
//
// ---------------------------------------------------------
//
void CLocalDeletion::HandlePartialDeletionL( TInt aDataLength )
    {
    TInt error = KErrNone;
    HBufC8* data = FetchXmlDataL( aDataLength );
    CleanupStack::PushL( data );
    TRAP( error, iXmlParser->ParseDocumentL( &iTargetDoc, *data ) );
    CXdmDocumentNode* parent = iDocumentSubset->Parent();
    RPointerArray<CXdmDocumentNode> parentResults;
    CleanupClosePushL( parentResults );
    RPointerArray<SXdmAttribute16> parentAttributes;
    CleanupClosePushL( parentAttributes );
    RPointerArray<CXdmDocumentNode> nodeResults;
    CleanupClosePushL( nodeResults );
    RPointerArray<SXdmAttribute16> nodeAttributes;
    CleanupClosePushL( nodeAttributes );
    TInt pAttrCount = parent->AttributeCount();
    TInt dAttrCount = iDocumentSubset->AttributeCount();
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
    if( dAttrCount > 0 )
        {
        SXdmAttribute16 attribute;
        for( TInt i = 0;i < dAttrCount;i++ )
            {
            attribute.iName.Set( iDocumentSubset->Attribute( i )->NodeName() );
            attribute.iValue.Set( iDocumentSubset->Attribute( i )->AttributeValue() );
            }
        User::LeaveIfError( nodeAttributes.Append( &attribute ) );
        }
    iTargetDoc.Find( parent->NodeName(), parentResults, parentAttributes );
    iTargetDoc.Find( iDocumentSubset->NodeName(), nodeResults, nodeAttributes );
    if( parentResults.Count() == 1 && nodeResults.Count() == 1 )
        {
        iXmlFile.Close();
        ReplaceDataFileL();
        CLocalDocumentNode* p = ( CLocalDocumentNode* )parentResults[0];
        CLocalDocumentNode* r = ( CLocalDocumentNode* )nodeResults[0];
        p->RemoveLocalL( r );
        WriteFileL( iTargetDoc.DocumentRoot() );
        }
    else User::Leave( KErrGeneral );
    CleanupStack::PopAndDestroy( 5 );  //nodeAttributes, nodeResults,
                                       //parentAttributes, parentResults, data
    }
        
// ---------------------------------------------------------
// Symbian OS default constructor may leave.
//
// ---------------------------------------------------------
//     
void CLocalDeletion::Destroy()
    {
    delete this;
    }                       

// ---------------------------------------------------------
// Destructor
//
// ---------------------------------------------------------
//
CLocalDeletion::~CLocalDeletion()
    {
    
    }

//  End of File  

