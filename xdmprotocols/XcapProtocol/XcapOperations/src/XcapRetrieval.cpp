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
* Description:   CXcapRetrieval
*
*/




// INCLUDES
#include "XcapCache.h"
#include "XcapAppUsage.h"
#include "XcapDocument.h"
#include "XcapProtocol.h"
#include "XcapRetrieval.h"
#include "XcapUriParser.h"
#include "XdmXmlParser.h"
#include "XcapHttpReqGet.h"
#include "XcapHttpTransport.h"
#include "XcapOperationFactory.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CXcapRetrieval::CXcapRetrieval( CXcapDocument& aParentDoc,
                                CXcapDocumentNode* aTargetNode,
                                CXcapOperationFactory& aOperationFactory ) :
                                CXcapHttpOperation( aParentDoc, aTargetNode, aOperationFactory ),
                                iCacheOperation( EFalse ),
                                iOperationType( aTargetNode == NULL ?
                                EXdmDocument : EXdmPartialDocument )                                  
    {
    }

// ---------------------------------------------------------
// CXcapRetrieval::NewL
//
// ---------------------------------------------------------
//
CXcapRetrieval* CXcapRetrieval::NewL( CXcapDocument& aParentDoc,
                                      CXcapDocumentNode* aTargetNode,
                                      CXcapOperationFactory& aOperationFactory )
    {
    CXcapRetrieval* self = new ( ELeave ) CXcapRetrieval( aParentDoc, aTargetNode, aOperationFactory );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CXcapRetrieval::ConstructL
//
// ---------------------------------------------------------
//
void CXcapRetrieval::ConstructL()
    {
#ifdef _DEBUG
    iOperationFactory.WriteToLog( _L8( "-> CXcapRetrieval::ConstructL" ) );  
#endif
    CXcapHttpReqGet* request = Transport().GetL( iTargetDoc.Name() );
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
#ifdef _DEBUG
    iOperationFactory.WriteToLog( _L8( "<- CXcapRetrieval::ConstructL" ) );  
#endif
    }

// ---------------------------------------------------------
// CXcapRetrieval::ExecuteL
//
// ---------------------------------------------------------
//
void CXcapRetrieval::ExecuteL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapRetrieval::ExecuteL()" ) );  
    #endif
    TPtrC8 eTag = iTargetDoc.ETag();
    if( eTag.Length() > 0 )
        {
        #ifdef _DEBUG
            iOperationFactory.WriteToLog( _L8( " Using ETag \"%S\" - Length: %d" ),
                                               &eTag, eTag.Length() );  
        #endif
        //iActiveRequest->SetHeaderL( KHttpHeaderIfNoneMatch, eTag );
        }
    TRAPD( error, iUriParser->ParseL( iActiveRequest->RequestUriL() ) );
    if( error == KErrNone )
        {
        TPtrC8 uri = iUriParser->DesC8();
        HBufC8* escape = CXcapHttpOperation::EscapeLC( uri );
        iActiveRequest->UpdateRequestUriL( escape->Des() );
        CleanupStack::PopAndDestroy();  //escape
        #ifdef _DEBUG
            iOperationFactory.WriteToLog( _L8( " New URI: %S" ), &uri );  
        #endif
        }
    }
        
// ---------------------------------------------------------
// CXcapRetrieval::OperationCompleteL
//
/* const TDesC8& aETag, 
   const TDesC& aDocumentName,
   const TDesC8& aRootLocation,
   const TDesC8& aResponseData*/
// ---------------------------------------------------------
//
void CXcapRetrieval::OperationCompleteL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapRetrieval::OperationCompleteL()" ) );  
    #endif
    TInt generalErr = ReinterpretStatus( iRequestData->iHttpStatus );
    if( generalErr == KErrNone )
        {
        #ifdef _DEBUG
            iOperationFactory.WriteToLog( _L8( " No general errors found" ) );  
        #endif
        iCacheOperation ? HandleCacheOperationL() : HandleNetworkOperationL();
        }
    else
        {
        #ifdef _DEBUG
            iOperationFactory.WriteToLog( _L8( " General errors found - Status: %d  Error: %d" ),
                                                   iRequestData->iHttpStatus, generalErr );  
        #endif
        iCompleted = ETrue;
        iResult = generalErr;
        }
    }

// ---------------------------------------------------------
// CXcapRetrieval::HandleCacheOperationL
//
// ---------------------------------------------------------
//
void CXcapRetrieval::HandleCacheOperationL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapRetrieval::HandleCacheOperationL() - Operation type: %d  Target node: %x" ),
                                           iOperationType, iDocumentSubset );  
    #endif
    TPtrC8 responseData = Descriptor( iRequestData->iResponseData );
    iOperationType == EXdmDocument && iDocumentSubset == NULL ? 
        iXmlParser->ParseDocumentL( &iTargetDoc, responseData ) :
        iXmlParser->ParseDocumentL( &iTargetDoc, responseData, iDocumentSubset );
    iCompleted = ETrue;
    }
    
// ---------------------------------------------------------
// CXcapRetrieval::HandleNetworkOperationL
//
// ---------------------------------------------------------
//
void CXcapRetrieval::HandleNetworkOperationL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapRetrieval::HandleNetworkOperationL()" ) );  
    #endif
    TPtrC name = iTargetDoc.Name();
    TPtrC8 root = Transport().RootUri();
    switch( iRequestData->iHttpStatus )
        {
        case 200:           //ETag was stale
            {
            #ifdef _DEBUG
                iOperationFactory.WriteToLog( _L8( " Status 200 - ETag was stale" ) );  
            #endif
            TPtrC8 responseData = Descriptor( iRequestData->iResponseData );
            if( responseData.Length() <= 0 )
                break;
            if( iOperationType == EXdmDocument )
                {
                TPtrC8 eTag = Descriptor( iRequestData->iETag );
                //If the ETag has not changed, server should return
                //304 (Not modified), but just in case, let's check the 
                //ETag value, anyway.
                if( eTag.Length() > 0 && eTag.Compare( iTargetDoc.ETag() ) != 0 )
                    {
                    iTargetDoc.SetETag( eTag );
                    RXcapCache* cache = iTargetDoc.Protocol().Cache();
                    if( cache && !( iOptionFlags & KNoCache ) )
                        cache->Store( iTargetDoc.ETag(), name, root, responseData  );
                    }
                else
                    {
                    #ifdef _DEBUG
                        iOperationFactory.WriteToLog( _L8( " ETag values match, do not update cache" ) );  
                    #endif
                    }
                iXmlParser->ParseDocumentL( &iTargetDoc, responseData );
                }
            else if( iOperationType == EXdmPartialDocument )
                {
                #ifdef _DEBUG  
                    iOperationFactory.WriteToLog( _L8( " Parse a partial document" ) );  
                #endif
                //The target node must be emptied before inserting new content
                iDocumentSubset->SetEmptyNode( ETrue );
                iDocumentSubset->SetEmptyNode( EFalse );
                iXmlParser->ParseDocumentL( responseData, iDocumentSubset );
               	}
            iCompleted = ETrue;
            }
            break;
        case 304:
            {
            RXcapCache* cache = iTargetDoc.Protocol().Cache();
            if( cache )
                {
                #ifdef _DEBUG   //ETag is up to date - cache still valid
                    iOperationFactory.WriteToLog( _L8( " Status 304 - Cached version is valid" ) );  
                #endif
                TInt length = iTargetDoc.DataLength();
                __ASSERT_DEBUG( length > 0, User::Panic( _L( "CXcapRetrieval" ), 1 ) );
                delete iRequestData->iResponseData;
                iRequestData->iResponseData = NULL;
                iRequestData->iResponseData = HBufC8::NewL( length );
                TPtr8 desc( iRequestData->iResponseData->Des() );
                cache->FetchDocumentContent( desc, name, root );
                if( iOperationType == EXdmDocument && !iTargetDoc.DocumentRoot() )
                	{
                	#ifdef _DEBUG  
                    	iOperationFactory.WriteToLog( _L8( " No content => parse cached document" ) );  
                	#endif
					iXmlParser->ParseDocumentL( &iTargetDoc, desc );
                	}
               	else if( iOperationType == EXdmPartialDocument )
               		{
               		#ifdef _DEBUG  
                    	iOperationFactory.WriteToLog( _L8( " Parse a partial document" ) );  
                	#endif
					iXmlParser->ParseDocumentL( &iTargetDoc, desc, iDocumentSubset );
               		}
                }
            iCompleted = ETrue;
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
    }
  
// ---------------------------------------------------------
// CXcapRetrieval::OperationFailedL
//
// ---------------------------------------------------------
//
void CXcapRetrieval::OperationFailedL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapRetrieval::OperationFailedL() - Error: %d" ),
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
// CXcapRetrieval::Result
//
// ---------------------------------------------------------
//
TInt CXcapRetrieval::Result() const
    {
    return iRequestData->iCompletion;
    }

// ---------------------------------------------------------
// CXcapRetrieval::~CXcapRetrieval
//
// ---------------------------------------------------------
//
CXcapRetrieval::~CXcapRetrieval()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapRetrieval::~CXcapRetrieval()" ) );  
    #endif
    }

//  End of File  

