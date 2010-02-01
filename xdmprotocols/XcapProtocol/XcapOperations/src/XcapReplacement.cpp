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
* Description:   CXcapReplacement
*
*/




// INCLUDES
#include "XcapAppUsage.h"
#include "XcapDocument.h"
#include "XcapUriParser.h"
#include "XdmXmlParser.h"
#include "XcapHttpReqPut.h"
#include "XcapReplacement.h"
#include "XcapHttpReqDelete.h"
#include "XcapHttpTransport.h"
#include "XcapHttpOperation.h"
#include "XdmNodeAttribute.h"
#include "XcapEngineDefines.h"
        
// ---------------------------------------------------------
// C++ constructor can NOT contain any code, that might leave.
//
// ---------------------------------------------------------
//
CXcapReplacement::CXcapReplacement( CXcapDocument& aParentDoc,
                                    CXcapDocumentNode* aOldNode,
                                    CXcapDocumentNode* aNewNode,
                                    CXcapOperationFactory& aOperationFactory ) :
                                    CXcapHttpOperation( aParentDoc, aOldNode, aOperationFactory ),
                                    iNewNode( ( CXcapDocumentNode* )aNewNode ),
                                    iOperationType( aOldNode == NULL ?
                                    EXdmDocument : EXdmPartialDocument )
                                    
    {
    }

// ---------------------------------------------------------
// Two-phased constructor.
//
// ---------------------------------------------------------
//
CXcapReplacement* CXcapReplacement::NewL( CXcapDocument& aParentDoc,
                                          CXcapDocumentNode* aOldNode,
                                          CXcapDocumentNode* aNewNode,
                                          CXcapOperationFactory& aOperationFactory )
    {
    CXcapReplacement* self = new ( ELeave ) CXcapReplacement( aParentDoc, aOldNode, aNewNode, aOperationFactory );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CXcapReplacement::ConstructL
//
// ---------------------------------------------------------
//
void CXcapReplacement::ConstructL()
    {
    CXcapHttpReqPut* request = Transport().PutL( iTargetDoc.Name() );
    CleanupStack::PushL( request );
    User::LeaveIfError( iRequestQueue.Append( request ) );
    request->SetExpiryTimeL( NULL, KDefaultHttpRequestTimeout * 1000000 );
    CleanupStack::Pop();  //request
    //If they point to the same node, we're dealing with a complete document
    if( iDocumentSubset != NULL && iTargetDoc.DocumentRoot() != iDocumentSubset )
        iUriParser->SetDocumentSubset( iDocumentSubset );
    }

// ---------------------------------------------------------
// CXcapReplacement::FormatModRequestL
//
// ---------------------------------------------------------
//
TInt CXcapReplacement::FormatModRequestL( const CXcapDocumentNode* aDocumentNode )
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapReplacement::FormatModRequestL()" ) );
        iOperationFactory.WriteToLog( _L8( "  Operation type:  %d" ), iOperationType );
        iOperationFactory.WriteToLog( _L8( "  Document subset: %x" ), iDocumentSubset );
    #endif
    return CXcapHttpOperation::FormatModRequestL( aDocumentNode );   
    }
    
// ---------------------------------------------------------
// CXcapReplacement::ExecuteL
//
// ---------------------------------------------------------
//
void CXcapReplacement::ExecuteL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapReplacement::ExecuteL()" ) );  
    #endif
    User::LeaveIfError( FormatModRequestL( iNewNode ) );
    if( iOperationType == EXdmPartialDocument )
        {
        TPtrC8 eTag = iTargetDoc.ETag();
        if( eTag.Length() > 0 )
            iActiveRequest->SetHeaderL( KHttpHeaderIfMatch, eTag );
        //If they match semantically, there's no need for DELETE operation
        if( iDocumentSubset->Match( *iNewNode ) )
            iOperationPhase = EHttpPutting;
        else
            {
            CXcapHttpReqDelete* request = Transport().DeleteL( iTargetDoc.Name() );
            CleanupStack::PushL( request );
            User::LeaveIfError( iRequestQueue.Insert( request, 0 ) );
            CleanupStack::Pop();  //request
            if( eTag.Length() > 0 )
                request->SetHeaderL( KHttpHeaderIfMatch, eTag );
            request->SetHeaderL( KHttpHeaderAccept, _L8( "application/xcap-diff+xml" ) );
            iActiveRequest = request;
            iOperationPhase = EHttpDeleting;
            }
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
    else
        {
        
        }
    }
        
// ---------------------------------------------------------
// CXcapReplacement::OperationCompleteL
//
// ---------------------------------------------------------
//
void CXcapReplacement::OperationCompleteL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog(
            _L8( "CXcapReplacement::OperationCompleteL() - Phase: %d" ), iOperationPhase );  
    #endif
    switch( iOperationPhase )
        {
        case EHttpDeleting:
            break;
        case EHttpPutting:
            break;
        default:
            iCompleted = ETrue;
            break;
        }
    }
    
// ---------------------------------------------------------
// CXcapReplacement::OperationFailedL
//
// ---------------------------------------------------------
//
void CXcapReplacement::OperationFailedL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapReplacement::OperationFailedL() - Error: %d" ),
                                        iStatus.Int() );
        TInt httpStatus = iRequestData == NULL ? KErrUnknown :
                          iRequestData->iHttpStatus;
    #endif

    switch( iOperationPhase )
        {
        case EHttpDeleting:
            #ifdef _DEBUG
                iOperationFactory.WriteToLog(
                    _L8( "DELETE failed - Error: %d  Status: %d" ), iStatus.Int(), httpStatus );  
            #endif
            break;
        case EHttpPutting:
            #ifdef _DEBUG
                iOperationFactory.WriteToLog(
                    _L8( "PUT failed - Error: %d  Status: %d" ), iStatus.Int(), httpStatus );  
            #endif
            break;
        default:
            break;
        }
    iResult = iStatus.Int();
    iCompleted = ETrue;   
    }

// ---------------------------------------------------------
// CXcapReplacement::Result
//
// ---------------------------------------------------------
//
TBool CXcapReplacement::Result() const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CXcapReplacement::~CXcapReplacement
//
// ---------------------------------------------------------
//
CXcapReplacement::~CXcapReplacement()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapReplacement::~CXcapReplacement()" ) );  
    #endif
    }

//  End of File  

