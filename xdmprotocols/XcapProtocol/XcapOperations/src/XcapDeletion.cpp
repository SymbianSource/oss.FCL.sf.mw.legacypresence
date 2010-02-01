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
* Description:   CXcapDeletion
*
*/




// INCLUDES
#include "XcapCache.h"
#include "XcapProtocol.h"
#include "XcapDeletion.h"
#include "XcapAppUsage.h"
#include "XcapDocument.h"
#include "XcapUriParser.h"
#include "XdmXmlParser.h"
#include "XcapDocumentNode.h"
#include "XcapHttpTransport.h"
#include "XcapHttpOperation.h"
#include "XcapHttpReqDelete.h"
#include "XcapOperationFactory.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CXcapDeletion::CXcapDeletion
//
// ---------------------------------------------------------
//
CXcapDeletion::CXcapDeletion( CXcapDocument& aParentDoc,
                              CXcapDocumentNode* aDocumentSubset,
                              CXcapOperationFactory& aOperationFactory ) :
                              CXcapHttpOperation( aParentDoc, aDocumentSubset, aOperationFactory ),
                              iOperationType( iDocumentSubset == NULL ?
                              EXdmDocument : EXdmPartialDocument )                                 
    {
    }

// ---------------------------------------------------------
// CXcapDeletion::NewL
//
// ---------------------------------------------------------
//
CXcapDeletion* CXcapDeletion::NewL( CXcapDocument& aParentDoc,
                                    CXcapDocumentNode* aDocumentSubset,
                                    CXcapOperationFactory& aOperationFactory )
    {
    CXcapDeletion* self = new ( ELeave ) CXcapDeletion( aParentDoc, aDocumentSubset, aOperationFactory );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CXcapDeletion::ConstructL
//
// ---------------------------------------------------------
//
void CXcapDeletion::ConstructL()
    {
    CXcapHttpReqDelete* request = Transport().DeleteL( iTargetDoc.Name() );
    CleanupStack::PushL( request );
    User::LeaveIfError( iRequestQueue.Append( request ) );
    request->SetExpiryTimeL( NULL, KDefaultHttpRequestTimeout * 1000000 );
    CleanupStack::Pop();  //request
    if( iOperationType != EXdmDocument && iDocumentSubset != NULL )
    	{
    	iUriParser->SetDocumentSubset( iDocumentSubset );
    	//Add namespace mappings
    	User::LeaveIfError( iTargetDoc.ApplicationUsage().Validate(
                        	*iDocumentSubset, iUriParser, ETrue ) );
    	}
    }
    
// ---------------------------------------------------------
// CXcapDeletion::ExecuteL
//
// ---------------------------------------------------------
//
void CXcapDeletion::ExecuteL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapDeletion::ExecuteL()" ) );  
    #endif
    TPtrC8 eTag = iTargetDoc.ETag();
    if( eTag.Length() > 0 )
        iActiveRequest->SetHeaderL( KHttpHeaderIfMatch, eTag );
    TRAPD( error, iUriParser->ParseL( iActiveRequest->RequestUriL() ) );
    if( error == KErrNone )
        {
        TPtrC8 uri = iUriParser->DesC8();
        #ifdef _DEBUG
            iOperationFactory.WriteToLog( _L8( " New URI: %S" ), &uri );  
        #endif
        HBufC8* escape = CXcapHttpOperation::EscapeLC( uri );
        iActiveRequest->UpdateRequestUriL( escape->Des() );
        CleanupStack::PopAndDestroy();  //escape
        } 
    }
        
// ---------------------------------------------------------
// CXcapDeletion::OperationCompleteL
//
// ---------------------------------------------------------
//
void CXcapDeletion::OperationCompleteL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapDeletion::OperationCompleteL()" ) );  
    #endif
    switch( iRequestData->iHttpStatus )
        {
        case 200:           //Document was deleted
            {
            #ifdef _DEBUG
                iOperationFactory.WriteToLog( _L8( " Status 200 - Document deleted" ) );  
            #endif
            iOperationType == EXdmDocument ?
                              HandleDocDeletion() :
                              HandlePartialDeletion();
            iCompleted = ETrue;
            }
            break;
        case 404:           //Document was not found
            {
            #ifdef _DEBUG   
                iOperationFactory.WriteToLog( _L8( " Status 404 - Document not found" ) );  
            #endif
            iCompleted = ETrue;
            iResult = KErrNotFound;
            }
            break;
        default:
            #ifdef _DEBUG
                iOperationFactory.WriteToLog( _L8( " Default case - Status: %d" ),
                                                   iRequestData->iHttpStatus );  
            #endif
            iCompleted = ETrue;
            break;
        }
    iResult = ReinterpretStatus( iRequestData->iHttpStatus );
    }

// ---------------------------------------------------------
// CXcapDeletion::HandleDocDeletion
//
// ---------------------------------------------------------
//
void CXcapDeletion::HandleDocDeletion()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapDeletion::HandleDocDeletion()" ) );  
    #endif
    TPtrC name = iTargetDoc.Name();
    TPtrC8 root = Transport().RootUri();
    RXcapCache* cache = iTargetDoc.Protocol().Cache();
    if( cache != NULL )
        {
        TInt error = cache->Delete( name, root );
        #ifdef _DEBUG
            iOperationFactory.WriteToLog( _L8( "  Document deleted from the cache" ) );  
        #endif
        }
    }

// ---------------------------------------------------------
// CXcapDeletion::HandlePartialDeletion
//
// ---------------------------------------------------------
//
void CXcapDeletion::HandlePartialDeletion()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapDeletion::HandlePartialDeletion()" ) );  
    #endif
    TPtrC name = iTargetDoc.Name();
    TPtrC8 root = Transport().RootUri();
    iTargetDoc.RemoveFromModelL( iDocumentSubset );
    TPtrC8 eTag = Descriptor( iRequestData->iETag );
    RXcapCache* cache = iTargetDoc.Protocol().Cache();
    if( cache != NULL )
        {
        HBufC8* newDoc = iXmlParser->FormatToXmlLC( ETrue, &iTargetDoc, iTargetDoc.DocumentRoot() );
        cache->Store( eTag, name, root, newDoc->Des() );
        CleanupStack::PopAndDestroy();  //newDoc
        }
    iTargetDoc.SetETag( eTag );
    }
      
// ---------------------------------------------------------
// CXcapDeletion::OperationFailedL
//
// ---------------------------------------------------------
//
void CXcapDeletion::OperationFailedL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapDeletion::OperationFailedL() - Error: %d" ),
                                        iStatus.Int() );  
    #endif
    if( iStatus.Int() >= KErrNone )
        {
        TInt status = iActiveRequest->ResponseData()->iHttpStatus;
        TInt completion = iActiveRequest->ResponseData()->iCompletion;
        iResult = status < KErrNone || completion < KErrNone ? status : ReinterpretStatus( status );
        }
    else iResult = iStatus.Int();
    iCompleted = ETrue;
    }
    
// ---------------------------------------------------------
// CXcapDeletion::Result
//
// ---------------------------------------------------------
//
TBool CXcapDeletion::Result() const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CXcapDeletion::~CXcapDeletion
//
// ---------------------------------------------------------
//
CXcapDeletion::~CXcapDeletion()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapDeletion::~CXcapDeletion()" ) );  
    #endif
    }

//  End of File  

