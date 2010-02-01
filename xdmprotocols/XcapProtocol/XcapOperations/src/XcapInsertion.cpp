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
* Description:   CXcapInsertion
*
*/




// INCLUDES
//This is only for logging
#include <XdmErrors.h>
#include "XcapProtocol.h"
#include "XcapDocument.h"
#include "XcapUriParser.h"
#include "XcapInsertion.h"
#include "XdmXmlParser.h"
#include "XcapHttpReqPut.h"
#include "XdmNodeAttribute.h"
#include "XcapHttpTransport.h"
#include "XcapEngineDefines.h"
#include "XcapOperationFactory.h"


// ---------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CXcapInsertion::CXcapInsertion( CXcapDocument& aParentDoc,
                                CXcapDocumentNode* aDocumentSubset,
                                CXcapOperationFactory& aOperationFactory ) :
                                CXcapHttpOperation( aParentDoc, aDocumentSubset, aOperationFactory ),
                                iOperationType( EXdmPartialDocument )
                                                              
    {
    }

// ---------------------------------------------------------
// Two-phased constructor.
//
// ---------------------------------------------------------
//
CXcapInsertion* CXcapInsertion::NewL( CXcapDocument& aParentDoc,
                                      CXcapDocumentNode* aDocumentSubset,
                                      CXcapOperationFactory& aOperationFactory )
    {
    CXcapInsertion* self = new ( ELeave ) CXcapInsertion( aParentDoc, aDocumentSubset, aOperationFactory );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CXcapInsertion::ConstructL
//
// ---------------------------------------------------------
//
void CXcapInsertion::ConstructL()
    {
    CXcapHttpReqPut* request = Transport().PutL( iTargetDoc.Name() );
    CleanupStack::PushL( request );
    User::LeaveIfError( iRequestQueue.Append( request ) );
    request->SetExpiryTimeL( NULL, KDefaultHttpRequestTimeout * 1000000 );
    CleanupStack::Pop();  //request
    iUriParser->SetDocumentSubset( iDocumentSubset );
    }

// ---------------------------------------------------------
// CXcapAddition::FormatModRequestL
//
// ---------------------------------------------------------
//
TInt CXcapInsertion::FormatModRequestL( const CXdmDocumentNode* aDocumentNode )
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapInsertion::FormatModRequestL()" ) );
        iOperationFactory.WriteToLog( _L8( "  Operation type:  %d" ), iOperationType );
        iOperationFactory.WriteToLog( _L8( "  Document subset: %x" ), iDocumentSubset );
    #endif
    return CXcapHttpOperation::FormatModRequestL( aDocumentNode );
    }
    
// ---------------------------------------------------------
// CXcapInsertion::ExecuteL
//
// ---------------------------------------------------------
//
void CXcapInsertion::ExecuteL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapInsertion::ExecuteL()" ) );  
    #endif
    CXcapHttpOperation::ExecuteL( iActiveRequest, iDocumentSubset );
    iActiveRequest->SetHeaderL( KHttpHeaderIfMatch, _L8( "*" ) );
    }
        
// ---------------------------------------------------------
// CXcapInsertion::OperationCompleteL
//
// ---------------------------------------------------------
//
void CXcapInsertion::OperationCompleteL()
    {
    
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapInsertion::OperationCompleteL()" ) );  
    #endif
    TPtrC8 root = Transport().RootUri();
    iRequestData = iActiveRequest->ResponseData();
    TPtrC name = iTargetDoc.Name();
    TPtrC8 eTag = Descriptor( iRequestData->iETag );
    switch( iRequestData->iHttpStatus )
        {
        case 201:           //"Created" -> Put operation was succesful
            {
            #ifdef _DEBUG
                iOperationFactory.WriteToLog( _L8( " Status 201 - Created" ) );  
            #endif
            iResult = KErrNone;
            iTargetDoc.SetETag( eTag );
            CXcapHttpContSupplier* request = ( CXcapHttpContSupplier* )iActiveRequest;
            RXcapCache* cache = iTargetDoc.Protocol().Cache();
            if( cache != NULL && iOperationType == EXdmDocument )
                {
                TPtrC8 payload = request->RequestBody();
                cache->Store( eTag, name, root, payload );
                }
            iCompleted = ETrue;
            }
            break;
        case 412:           //"Precondition failed" -> Cache out of date
            {
            #ifdef _DEBUG
                iOperationFactory.WriteToLog( _L8( " Status 412 - Precondition failed" ) );  
            #endif
            iResult = KXcapErrorHttpPrecondFailed;
            //RXcapCache* cache = CXcapProtocol::Cache();
            //if( cache != NULL && iOperationType == EXcapDocument )
            //    cache->Delete( name, root );
            iCompleted = ETrue;
            iActiveRequest->ResetUriL( _L( "" ) );
            iActiveRequest->ReleaseResponseData();
            iRequestData->iCompletion = KInsertConflict;
            }
            break;
        default:
            #ifdef _DEBUG
                iOperationFactory.WriteToLog( _L8( " Default case - Status: %d" ),
                                                       iRequestData->iHttpStatus ); 
            #endif
            TInt status = iRequestData->iHttpStatus;
            iResult = ReinterpretStatus( status );
            iCompleted = ETrue;
            break;
        } 
    }
    
// ---------------------------------------------------------
// Symbian OS default constructor may leave.
//
// ---------------------------------------------------------
//
void CXcapInsertion::OperationFailedL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapInsertion::OperationFailedL() - Error: %d" ),
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
// Symbian OS default constructor may leave.
//
// ---------------------------------------------------------
//
TBool CXcapInsertion::Result() const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// Destructor
//
// ---------------------------------------------------------
//
CXcapInsertion::~CXcapInsertion()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapInsertion::~CXcapInsertion()" ) );  
    #endif
    }

//  End of File  

