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
* Description:   CXcapAddition
*
*/




// INCLUDES
#include "XcapCache.h"
#include "XcapAppUsage.h"
#include "XcapProtocol.h"
#include "XcapDocument.h"
#include "XcapAddition.h"
#include "XdmXmlParser.h"
#include "XcapUriParser.h"
#include "CommonDefines.h"
#include "XcapHttpReqPut.h"
#include "XcapHttpOperation.h"
#include "XcapHttpTransport.h"
#include "XcapOperationFactory.h"
       
// ---------------------------------------------------------
// CXcapAddition::CXcapAddition
//
// ---------------------------------------------------------
//
CXcapAddition::CXcapAddition( CXcapDocument& aParentDoc,
                              CXcapDocumentNode* aDocumentSubset,
                              CXcapOperationFactory& aOperationFactory ) :
                              CXcapHttpOperation( aParentDoc, aDocumentSubset, aOperationFactory ),
                              iOperationType( aDocumentSubset == NULL ?
                              EXdmDocument : EXdmPartialDocument )
                                                      
    {
    }

// ---------------------------------------------------------
// CXcapAddition::NewL
//
// ---------------------------------------------------------
//
CXcapAddition* CXcapAddition::NewL( CXcapDocument& aParentDoc,
                                    CXcapDocumentNode* aDocumentSubset,
                                    CXcapOperationFactory& aOperationFactory )
    {
    CXcapAddition* self = new ( ELeave ) CXcapAddition( aParentDoc, aDocumentSubset, aOperationFactory );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CXcapAddition::ConstructL
//
// ---------------------------------------------------------
//
void CXcapAddition::ConstructL()
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
// CXcapAddition::FormatModRequestL
//
// ---------------------------------------------------------
//
TInt CXcapAddition::FormatModRequestL( const CXdmDocumentNode* aDocumentNode )
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapAddition::FormatModRequestL()" ) );
        iOperationFactory.WriteToLog( _L8( "  Operation type:  %d" ), iOperationType );
        iOperationFactory.WriteToLog( _L8( "  Document subset: %x" ), iDocumentSubset );
    #endif
    TInt validation = CXcapHttpOperation::FormatModRequestL( aDocumentNode );
    if( validation == KErrNone )
        {
        //Then set the properties particular to this type of request
        iOperationType = DetermineOperationType();
        if( iOperationType == EXdmDocument )
            iActiveRequest->SetHeaderL( KHttpHeaderContentType, iTargetDoc.ApplicationUsage().ContentType() );
        }
    else
        {
        #ifdef _DEBUG
            iOperationFactory.WriteToLog( _L8( " AppUsage reported the following error: %d" ), validation );
        #endif
        }
    return validation;
    }

// ---------------------------------------------------------
// CXcapAddition::DetermineOperationType
//
// ---------------------------------------------------------
//
TXdmOperationType CXcapAddition::DetermineOperationType()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapAddition::DetermineDocType()" ) );
        iOperationFactory.WriteToLog( _L8( "  Operation type:  %d" ), iOperationType );
        iOperationFactory.WriteToLog( _L8( "  Document subset: %x" ), iDocumentSubset );
        iOperationFactory.WriteToLog( _L8( "  Document root:   %x" ), iTargetDoc.DocumentRoot() ); 
    #endif
    return iOperationType == EXdmDocument || ( EXdmPartialDocument &&
                             iDocumentSubset == iTargetDoc.DocumentRoot() ) ?
                             EXdmDocument : EXdmPartialDocument;
    }
      
// ---------------------------------------------------------
// CXcapAddition::ExecuteL
//
// ---------------------------------------------------------
//
void CXcapAddition::ExecuteL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapAddition::ExecuteL()" ) );  
    #endif
    CXcapHttpOperation::ExecuteL( iActiveRequest, iDocumentSubset );
    }
        
// ---------------------------------------------------------
// CXcapAddition::OperationCompleteL
//
// ---------------------------------------------------------
//
void CXcapAddition::OperationCompleteL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapAddition::OperationCompleteL()" ) );  
    #endif
    TPtrC8 root = Transport().RootUri();
    iRequestData = iActiveRequest->ResponseData();
    TPtrC name = iTargetDoc.Name();
    switch( iRequestData->iHttpStatus )
        {
        case 200:           //Replaced, the document was already there
        case 201:           //"Created" -> Put operation was successful
            {
            #ifdef _DEBUG
                iOperationFactory.WriteToLog( _L8( " Status %d" ), iRequestData->iHttpStatus );  
            #endif
            iResult = KErrNone;
            TPtrC8 eTag = Descriptor( iRequestData->iETag );
            iTargetDoc.SetETag( eTag );
            RXcapCache* cache = iTargetDoc.Protocol().Cache();
            if( cache != NULL )
                {
                CXcapHttpContSupplier* request = ( CXcapHttpContSupplier* )iActiveRequest;
                TPtrC8 payload = request->RequestBody();
                iOperationType == EXdmDocument ? cache->Store( eTag, name, root, payload ) :
                                                 UpdatePartialToCacheL( cache, payload );
                }
            iCompleted = ETrue;
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
// CXcapAddition::UpdatePartialToCacheL
//
// ---------------------------------------------------------
//
void CXcapAddition::UpdatePartialToCacheL( RXcapCache* aCache, const TDesC8& aNewData )
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapAddition::UpdatePartialToCacheL()" ) );  
    #endif
    HBufC8* newData = NULL;
    TPtrC name = iTargetDoc.Name();
    TPtrC8 root = Transport().RootUri();
    if( iOptionFlags & KFetchMasterFromCache )
        {
        TInt length = iTargetDoc.DataLength();
        //Do nothing, if the document is not in cache
        if( length > 0 )
            {
            HBufC8* data = HBufC8::NewLC( length );
            TPtr8 desc( data->Des() );
            aCache->FetchDocumentContent( desc, name, root );
            CXcapDocument* copy = iTargetDoc.TempCopyL();
            CleanupStack::PushL( copy );
            iXmlParser->ParseDocumentL( copy, desc );
            RPointerArray<CXdmDocumentNode> array;
            CleanupClosePushL( array );
            copy->Find( *iDocumentSubset, array );
            //If the element the subset points to is not present
            //in the original document, the new data must be
            //appended to the the subset element parent, instead.
            CXdmDocumentNode* node = array.Count() > 0 ?
                iDocumentSubset : iDocumentSubset->Parent();
            newData = iXmlParser->FormatToXmlLC( aNewData, copy, node );
            CleanupStack::Pop();  //newData
            CleanupStack::PopAndDestroy( 3 );  //array, tempCopy, data
            CleanupStack::PushL( newData );
            }           
        }
    else
        {
        CXdmDocumentNode* root = iTargetDoc.DocumentRoot();
        newData = iXmlParser->FormatToXmlLC( ETrue, &iTargetDoc, root );
        }
    if( newData )
        {
        aCache->Store( iTargetDoc.ETag(), name, root, newData->Des() );
        CleanupStack::PopAndDestroy();  //newData
        }
    }

// ---------------------------------------------------------
// CXcapAddition::OperationFailedL
//
// ---------------------------------------------------------
//
void CXcapAddition::OperationFailedL()
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
// CXcapAddition::Result
//
// ---------------------------------------------------------
//
TBool CXcapAddition::Result() const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CXcapAddition::~CXcapAddition
//
// ---------------------------------------------------------
//
CXcapAddition::~CXcapAddition()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapAddition::~CXcapAddition()" ) );  
    #endif
    }

//  End of File  

