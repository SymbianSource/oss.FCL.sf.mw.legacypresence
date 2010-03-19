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
* Description:   CXcapHttpOperation
*
*/



#include <XdmErrors.h>
#include <escapeutils.h>
#include "XcapDocument.h"
#include "XcapProtocol.h"
#include "XcapAppUsage.h"
#include "XcapUriParser.h"
#include "XdmXmlParser.h"
#include "XcapHttpRequest.h"
#include "XdmNodeAttribute.h"
#include "XcapEngineDefines.h"
#include "XcapHttpOperation.h"
#include "XcapHttpContSupplier.h"
#include "XcapOperationFactory.h"

// ---------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CXcapHttpOperation::CXcapHttpOperation( const CXdmDocument& aTargetDoc,
                                        CXcapDocumentNode* aDocumentSubset,
                                        CXcapOperationFactory& aOperationFactory ) :
                                        CActive( EPriorityStandard ),
                                        iDocumentSubset( aDocumentSubset ),
                                        iTargetDoc( ( CXcapDocument& )aTargetDoc ),
                                        iOperationFactory( aOperationFactory ) 
                                                                                
    {
    }

// ---------------------------------------------------------
// CXcapHttpOperation::BaseConstructL
//
// ---------------------------------------------------------
//
void CXcapHttpOperation::BaseConstructL()
    {
    iXmlParser = &iTargetDoc.Protocol().Parser();
    iUriParser = CXcapUriParser::NewL( iTargetDoc, iOperationFactory );
    #ifdef _DEBUG
        TPtrC8 contType( iTargetDoc.ApplicationUsage().ContentType() );
        iOperationFactory.WriteToLog( _L8( " Content-Type: %S" ), &contType ); 
    #endif
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// Destructor
//
// ---------------------------------------------------------
//
CXcapHttpOperation::~CXcapHttpOperation()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapHttpOperation::~CXcapHttpOperation()" ) );  
    #endif
    delete iUriParser;
    iRequestQueue.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CXcapOperationBase::IsCompleted
//
// ---------------------------------------------------------
//
TBool CXcapHttpOperation::IsCompleted() const
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapOperationBase::IsCompleted()" ) );  
    #endif
    return iCompleted;
    }
    
// ---------------------------------------------------------
// CXcapOperationBase::CompletionData
//
// ---------------------------------------------------------
//
const TXdmCompletionData& CXcapHttpOperation::CompletionData() const
    {
    return *iRequestData;
    }

// ---------------------------------------------------------
// CXcapOperationBase::CompletionData
//
// ---------------------------------------------------------
//
CXdmDocumentNode* CXcapHttpOperation::TargetNode() const
    {
    return iDocumentSubset;
    }
    
// ---------------------------------------------------------
// CXcapOperationBase::CompletionData
//
// ---------------------------------------------------------
//
void CXcapHttpOperation::Destroy()
    {
    delete this;
    }
    
// ---------------------------------------------------------
// CXcapHttpOperation::Result
//
// ---------------------------------------------------------
//
TInt CXcapHttpOperation::Result() const
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapHttpOperation::Result()" ) );  
    #endif
    return iRequestData != NULL ? iRequestData->iHttpStatus : KErrUnknown;
    }

// ---------------------------------------------------------
// CXcapHttpOperation::ExecuteL
//
// ---------------------------------------------------------
//
void CXcapHttpOperation::ExecuteL( TRequestStatus& aStatus, TUint aOptions )
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapHttpOperation::ExecuteL( &status )" ) );  
    #endif
    iOptionFlags = aOptions;
    iClientStatus = &aStatus;
    aStatus = KRequestPending;
    if( !IsActive() )
        {
        iActiveRequest = iRequestQueue[iCurrentIndex];
        TRAPD( error, ExecuteL() );
        if( error == KErrNone )
            {
            if( StartOperationL() )
                SetActive();
            else
                {
                iFailureData.iCompletion = KXcapErrorNetworkNotAvailabe;
                User::RequestComplete( iClientStatus, KXcapErrorNetworkNotAvailabe );
                iRequestData = &iFailureData;
                }      
            }
        else
            {
            #ifdef _DEBUG
                iOperationFactory.WriteToLog( _L8( " Execution failed with %d" ), error );  
            #endif
            iFailureData.iCompletion = error;
            User::RequestComplete( iClientStatus, error );
            iRequestData = &iFailureData;
            }
        }
    else  //This is a re-run of a previously failed request
        {
        if( !StartOperationL() )
            {
            #ifdef _DEBUG
                iOperationFactory.WriteToLog( _L8( " Execution succeeded, network not ready" ) );  
            #endif
            User::RequestComplete( iClientStatus, KXcapErrorNetworkNotAvailabe );
            }
        }  
    }

// ---------------------------------------------------------
// CXcapHttpOperation::ExecuteL
//       
// ---------------------------------------------------------
//
void CXcapHttpOperation::ExecuteL( CXcapHttpRequest* aHttpRequest,
                                   CXdmDocumentNode* aDocumentNode )
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapAddition::ExecuteL()" ) );  
    #endif
    CXdmDocumentNode* node = aDocumentNode == NULL ?
                             iTargetDoc.DocumentRoot() : aDocumentNode;
    __ASSERT_ALWAYS( node != NULL, User::Panic( _L( "CXcapAddition" ), 1 ) );
    User::LeaveIfError( FormatModRequestL( node ) );
    TRAPD( error, iUriParser->ParseL( aHttpRequest->RequestUriL() ) );
    if( error == KErrNone )
        {
        TPtrC8 uri = iUriParser->DesC8();
        HBufC8* escape = CXcapHttpOperation::EscapeLC( uri );
        aHttpRequest->UpdateRequestUriL( escape->Des() );
        CleanupStack::PopAndDestroy();  //escape
        }
    }
    
// ---------------------------------------------------------
// CXcapHttpOperation::StartOperationL
//
// ---------------------------------------------------------
//
TBool CXcapHttpOperation::StartOperationL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapHttpOperation::StartOperationL()" ) );  
    #endif
    if( iTargetDoc.Protocol().IsNetworkAvailable() )
        {
        #ifdef _DEBUG
            iOperationFactory.WriteToLog( _L8( " All set, dispatch request to transport" ) );  
        #endif
        if( iOptionFlags & KUseIntendedIdentity )
            {
            TPtrC8 identity( iTargetDoc.Protocol().PublicID() );
            #ifdef _DEBUG
                iOperationFactory.WriteToLog( _L8( " Early-IMS set, add X-3GPP-Intended-Identity header" ) );
                iOperationFactory.WriteToLog( _L8( "  Public ID: %S" ), &identity );  
            #endif
            iActiveRequest->SetHeaderL( KHttpHeaderIntIdentity, identity );
            }
        iActiveRequest->DispatchRequestL( iStatus );
        return ETrue;
        }
    else return EFalse;
    }
    
// ---------------------------------------------------------
// CXcapHttpOperation::RunL
//
// ---------------------------------------------------------
//
void CXcapHttpOperation::RunL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( 
        "CXcapHttpOperation::RunL() - Status: %d" ), iStatus.Int() );  
    #endif
    TInt count = iRequestQueue.Count();
    iRequestData = iActiveRequest->ResponseData();
    __ASSERT_DEBUG( iRequestData != NULL, User::Panic( _L( "CXcapHttpOperation" ), 1 ) );
    TRAPD( error, iStatus != KErrNone ? OperationFailedL() : OperationCompleteL() );
    if( error == KErrNone && !iCompleted )
        SetActive();
    else
        {
        #ifdef _DEBUG
            iOperationFactory.WriteToLog( _L8( 
                "  ** Operation completed - Result: %d  Error: %d **" ), iResult, error );  
        #endif
        iCurrentIndex = 0;
        iResult = error < KErrNone ? error : iResult;
        iRequestData->iCompletion = iResult;
        User::RequestComplete( iClientStatus, iResult );
        }
    }

// ---------------------------------------------------------
// CXcapRetrieval::CancelOperation
//
// ---------------------------------------------------------
//
void CXcapHttpOperation::CancelOperation()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapHttpOperation::CancelOperation()" ) );  
    #endif
    if( IsActive() )
        {
        #ifdef _DEBUG
            iOperationFactory.WriteToLog( _L8( "Active, cancel HTTP request" ) );  
        #endif
        Cancel();
        iRequestData = &iFailureData;
        iFailureData.iCompletion = KErrCancel;
        User::RequestComplete( iClientStatus, KErrCancel );
        }
    }
    
// ---------------------------------------------------------
// CXcapHttpOperation::CancelOperation
//
// ---------------------------------------------------------
//
void CXcapHttpOperation::DoCancel()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapHttpOperation::DoCancel()" ) );  
    #endif
    iRequestQueue[iCurrentIndex]->CancelRequest();   
    }

// ---------------------------------------------------------
// CXcapHttpOperation::Descriptor
//
// ---------------------------------------------------------
//
TPtrC8 CXcapHttpOperation::Descriptor( HBufC8* aBuffer )
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapHttpOperation::Descriptor()" ) );  
    #endif
    if( aBuffer != NULL )
        {
        TPtrC8 descriptor( aBuffer->Des() );
        return descriptor.Length() > 0 ? descriptor : TPtrC8();
        }
    else
        {
        #ifdef _DEBUG
            iOperationFactory.WriteToLog( _L8( " Buffer is NULL, return empty descriptor" ) );  
        #endif
        return TPtrC8();
        }
    }

// ---------------------------------------------------------
// CXcapHttpOperation::Transport
//
// ---------------------------------------------------------
//
CXcapHttpTransport& CXcapHttpOperation::Transport() const
    {
    return iTargetDoc.Protocol().Transport();
    }
    
// ---------------------------------------------------------
// CXcapHttpOperation::FormatModRequestL
//
// ---------------------------------------------------------
//
TInt CXcapHttpOperation::FormatModRequestL( const CXdmDocumentNode* aDocumentNode )
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapHttpOperation::FormatModRequestL()" ) );  
    #endif
    HBufC8* body = NULL;
    TInt error = KErrNone;
    CXcapAppUsage& usage = iTargetDoc.ApplicationUsage();
    CXcapHttpContSupplier* request = ( CXcapHttpContSupplier* )iActiveRequest;
    switch( iUriParser->NodeType() )
        {
        case EXdmElementAttribute:
            {
            CXdmNodeAttribute* attribute = ( CXdmNodeAttribute* )aDocumentNode;
            body = attribute->EightBitValueLC();
            request->SetRequestBodyL( body->Des() );
            CleanupStack::PopAndDestroy();  //body
            request->SetHeaderL( KHttpHeaderAccept, _L8( "application/xcap-diff+xml" ) );
            request->SetHeaderL( KHttpHeaderContentType, _L8( "application/xcap-att+xml" ) );
            }
            break;
        case EXdmElementNode:
            {
            #ifndef __OVERRIDE_VALIDATION__
                error = usage.Validate( CONST_CAST( CXdmDocumentNode&, *aDocumentNode ), iUriParser, EFalse );
            #endif
            if( error == KErrNone )
                {
                #ifdef _DEBUG
                    iOperationFactory.WriteToLog( _L8( " Validation of a partial document OK" ) );  
                #endif
                body = iXmlParser->FormatToXmlLC( EFalse, &iTargetDoc, aDocumentNode );
                request->SetRequestBodyL( body->Des() );
                CleanupStack::PopAndDestroy();  //body
                request->SetHeaderL( KHttpHeaderAccept, _L8( "application/xcap-diff+xml" ) );
                request->SetHeaderL( KHttpHeaderContentType, _L8( "application/xcap-el+xml" ) );
                }
            else
                {
                #ifdef _DEBUG
                    iOperationFactory.WriteToLog( _L8( " Validation failed: %d" ), error );  
                #endif
                }
            }
            break;
        default:
        #ifndef __OVERRIDE_VALIDATION__
            error = usage.Validate( iTargetDoc, ETrue );
        #endif
            if( error == KErrNone )
                {
                #ifdef _DEBUG
                    iOperationFactory.WriteToLog( _L8( " Validation OK" ) );  
                #endif
                body = iXmlParser->FormatToXmlLC( ETrue, &iTargetDoc, aDocumentNode );
                request->SetRequestBodyL( body->Des() );
                CleanupStack::PopAndDestroy();  //body
                }
            else
                {
                #ifdef _DEBUG
                    iOperationFactory.WriteToLog( _L8( " Validation failed: %d" ), error );  
                #endif
                }
            break;
        }
    return error;
    }

// ----------------------------------------------------------
// CXcapHttpOperation::ReinterpretStatus
// 
// ----------------------------------------------------------
//
TInt CXcapHttpOperation::ReinterpretStatus( const TInt aHttpStatus ) const
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapHttpOperation::ReinterpretStatus()" ) );  
    #endif
    switch( aHttpStatus )
        {
        case 500:  //Internal Server Error
        case 501:  //Not Implemented
        case 502:  //Bad Gateway
        case 503:  //Service Unavailable
        case 504:  //Gateway Timeout
        case 505:  //HTTP Version Not Supported
        case 405:  //Method Not Allowed
		case 408:  //Request Timeout
		case 410:  //Gone
		case 411:  //Length Required
		case 413:  //Request Entity Too Large
		case 414:  //Request-URI Too Long
		case 415:  //Unsupported Media Type
		case 416:  //Requested Range Not Satisfiable
		case 417:  //Expectation Failed
            return KXcapErrorHttpServer;
        case 400:
            return KXcapErrorHttpBadRequest;
        case 401:
            return KXcapErrorHttpUnauthorised;
        case 403:
            return KXcapErrorHttpForbidden;
        case 404:
            return KXcapErrorHttpNotFound;
        case 409:
            return KXcapErrorHttpConflict;
        case 412:
            return KXcapErrorHttpPrecondFailed;
        default:
            return KErrNone;
        }
    }

// ---------------------------------------------------------
// CXcapHttpOperation::Escape2LC
// 
// ---------------------------------------------------------
//
HBufC8* CXcapHttpOperation::Escape2LC( const TDesC& aDescriptor )
    {
    HBufC8* eight = EscapeUtils::ConvertFromUnicodeToUtf8L( aDescriptor );
    CleanupStack::PushL( eight );
    HBufC8* buf = EscapeUtils::EscapeEncodeL( eight->Des(), EscapeUtils::EEscapeNormal );
    CleanupStack::PopAndDestroy();  //eight
    CleanupStack::PushL( buf );
    return buf;
    }
    
// ---------------------------------------------------------
// CXcapHttpOperation::EscapeLC
// 
// ---------------------------------------------------------
//
HBufC8* CXcapHttpOperation::EscapeLC( const TDesC8& aDescriptor )
    {
    CBufFlat* buffer = CBufFlat::NewL( 50 );
    CleanupStack::PushL( buffer );
    TInt bufPos = 0;
    TBuf8<10> format;
    for( TInt i = 0;i < aDescriptor.Length();i++ )
        {
        TUint8 byte = aDescriptor[i];
        if( byte < 0x7F )                 //US-ASCII
            {
            switch( byte )
                {
                case 0x20:                // ' '
                case 0x22:                // '"'
                case 0x3B:                // ';'                 
                case 0x26:                // '&'                  
                case 0x3C:                // '<'                  
                case 0x3E:                // '>'
                case 0x5B:                // '['
                case 0x5D:                // ']'                              
                    format.Append( _L8( "%" ) );
                    format.AppendFormat( _L8( "%x" ), byte );
                    break;               
                default:
                    format.Append( byte );
                    break;
                }
            }
        else
            {
            format.Append( _L8( "%" ) );  //UNICODE
            format.AppendFormat( _L8( "%x" ), byte );
            }
        buffer->InsertL( bufPos, format );
        bufPos = bufPos + format.Length();
        format.Zero();
        }
    HBufC8* ret = HBufC8::NewL( buffer->Size() );
    TPtr8 pointer( ret->Des() );
    buffer->Read( 0, pointer, buffer->Size() );
    CleanupStack::PopAndDestroy();  //buffer
    CleanupStack::PushL( ret );
    return ret;
    }
               
// ---------------------------------------------------------
// CXcapHttpOperation::HttpRequest
//
// ---------------------------------------------------------
//
CXcapHttpRequest* CXcapHttpOperation::HttpRequest() const
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapHttpOperation::HttpRequest()" ) );  
    #endif
    return iActiveRequest;
    }



//  End of File  

