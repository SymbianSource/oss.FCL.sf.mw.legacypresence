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
* Description: CLocalAddition
*
*/




// INCLUDES
#include "LocalProtocol.h"
#include "LocalDocument.h"
#include "LocalAddition.h"
#include "XdmXmlParser.h"
#include "LocalDocumentNode.h"
#include "LocalNodeAttribute.h"
#include "LocalOperationFactory.h"
    
// ---------------------------------------------------------
// CLocalAddition::CLocalAddition
//
// ---------------------------------------------------------
//
CLocalAddition::CLocalAddition( CLocalDocument& aParentDoc,
                                CLocalDocumentNode* aDocumentSubset,
                                CLocalOperationFactory& aOperationFactory ) :
                                CLocalOperationBase( aParentDoc, aOperationFactory ),
                                iOperationType( aDocumentSubset == NULL ?
                                EXdmDocument : EXdmPartialDocument ),         
                                iDocumentSubset( aDocumentSubset )
                                                      
    {
    }

// ---------------------------------------------------------
// CLocalAddition::NewL
//
// ---------------------------------------------------------
//
CLocalAddition* CLocalAddition::NewL( CLocalDocument& aParentDoc,
                                      CLocalDocumentNode* aDocumentSubset,
                                      CLocalOperationFactory& aOperationFactory )
    {
    CLocalAddition* self = new ( ELeave ) CLocalAddition( aParentDoc, aDocumentSubset, aOperationFactory );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CLocalAddition::ExecuteL
//
// ---------------------------------------------------------
//
void CLocalAddition::ExecuteL()
    {
    TPtrC fileName = iTargetDoc.XmlFilePath();
    TInt size = OpenDataFileL( fileName );
    if( size > 0 )      //The document already exists
        {
        //Only a subset of a document
        if( iTargetDoc.IsSubset() && iDocumentSubset != NULL )  
            HandlePartialUpdateL( size );
        else WriteFileL( iTargetDoc.DocumentRoot() );
        }
    else WriteFileL( iTargetDoc.DocumentRoot() );
    iXmlFile.Close();
    }

// ---------------------------------------------------------
// CLocalAddition::HandlePartialUpdateL
//
// ---------------------------------------------------------
//
void CLocalAddition::HandlePartialUpdateL( TInt aDataLength )
    {
    HBufC8* data = FetchXmlDataL( aDataLength );
    CleanupStack::PushL( data );
    CLocalDocument* copy = iTargetDoc.TempCopyL();
    CleanupStack::PushL( copy );
    iXmlParser->ParseDocumentL( copy, *data );
    RPointerArray<CXdmDocumentNode> results;
    CleanupClosePushL( results );
    RPointerArray<SXdmAttribute16> attributes;
    CleanupClosePushL( attributes );
    TInt attrCount = iDocumentSubset->AttributeCount();
    if( attrCount > 0 )
        {
        SXdmAttribute16 attribute;
        for( TInt i = 0;i < attrCount;i++ )
            {
            attribute.iName.Set( iDocumentSubset->Attribute( i )->NodeName() );
            attribute.iValue.Set( iDocumentSubset->Attribute( i )->AttributeValue() );
            }
        User::LeaveIfError( attributes.Append( &attribute ) );
        }
    copy->Find( iDocumentSubset->NodeName(), results, attributes );
    if( results.Count() == 1 )
        {
        iXmlFile.Close();
        ReplaceDataFileL();
        CLocalDocumentNode* node = ( CLocalDocumentNode* )results[0];
        node->AppendLocalL( ( CLocalDocumentNode* )iDocumentSubset );
        WriteFileL( copy->DocumentRoot() );
        }
    else User::Leave( KErrGeneral );
    CleanupStack::PopAndDestroy( 4 );  //attributes, results, copy, data
    }
        
// ---------------------------------------------------------
// CLocalAddition::Destroy
//
// ---------------------------------------------------------
//     
void CLocalAddition::Destroy()
    {
    delete this;
    } 

// ---------------------------------------------------------
// Destructor
//
// ---------------------------------------------------------
//
CLocalAddition::~CLocalAddition()
    {
    
    }

//  End of File  

