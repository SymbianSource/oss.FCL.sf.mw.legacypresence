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
* Description: CLocalRetrieval
*
*/




// INCLUDES
#include "XdmXmlParser.h"
#include "LocalProtocol.h"
#include "LocalDocument.h"
#include "LocalRetrieval.h"
#include "LocalDocumentNode.h"
#include "LocalOperationFactory.h"
    
// ---------------------------------------------------------
// CLocalRetrieval::CLocalRetrieval
//
// ---------------------------------------------------------
//
CLocalRetrieval::CLocalRetrieval( CLocalDocument& aTargetDoc,
                                  CLocalDocumentNode* aDocumentSubset,
                                  CLocalOperationFactory& aOperationFactory ) :
                                  CLocalOperationBase( aTargetDoc, aOperationFactory ),
                                  iDocumentSubset( aDocumentSubset ),
                                  iOperationType( aDocumentSubset == NULL ?
                                  EXdmDocument : EXdmPartialDocument )
                                                      
    {
    }

// ---------------------------------------------------------
// CLocalRetrieval::NewL
//
// ---------------------------------------------------------
//
CLocalRetrieval* CLocalRetrieval::NewL( CLocalDocument& aParentDoc,
                                        CLocalDocumentNode* aDocumentSubset,
                                        CLocalOperationFactory& aOperationFactory )
    {
    CLocalRetrieval* self = new ( ELeave ) CLocalRetrieval( aParentDoc, aDocumentSubset, aOperationFactory );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    CleanupStack::Pop();
    return self;
    }
      
// ---------------------------------------------------------
// CLocalRetrieval::ExecuteL
//
// ---------------------------------------------------------
//
void CLocalRetrieval::ExecuteL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CLocalRetrieval::ExecuteL()" ) );
    #endif
    HBufC8* data = NULL;
    TInt error = KErrNone;
    TInt length = OpenDataFileL( iTargetDoc.XmlFilePath() );
    TRAP( error, data = FetchXmlDataL( length ) );
    CleanupStack::PushL( data );
    if( data != NULL )
        {
        if( iDocumentSubset == NULL )
            {
            TRAP( error, iXmlParser->ParseDocumentL( &iTargetDoc, *data ) );
            #ifdef _DEBUG
                iOperationFactory.WriteToLog( _L8( " Document fetch complete - Error: %d" ), error );
            #endif
            }
        else
            {
            iDocumentSubset->SetEmptyNode( ETrue );
            iDocumentSubset->SetEmptyNode( EFalse );
            TRAP( error, iXmlParser->ParseDocumentL( &iTargetDoc, *data, ( CXdmDocumentNode* )iDocumentSubset ) );
            #ifdef _DEBUG
                iOperationFactory.WriteToLog( _L8( " Document fetch complete: %d" ), error );
            #endif
            }
        }
    else
        {
        #ifdef _DEBUG
            iOperationFactory.WriteToLog( _L8( " Reading of the document data failed: %d" ), error );
        #endif
        User::Leave( error );
        }
    CleanupStack::PopAndDestroy();  //data
    iXmlFile.Close();
    }
            
// ---------------------------------------------------------
// CLocalRetrieval::Destroy
//
// ---------------------------------------------------------
//     
void CLocalRetrieval::Destroy()
    {
    delete this;
    }  
                         
// ---------------------------------------------------------
// CLocalRetrieval::~CLocalRetrieval
//
// ---------------------------------------------------------
//
CLocalRetrieval::~CLocalRetrieval()
    {
    
    }

//  End of File  

