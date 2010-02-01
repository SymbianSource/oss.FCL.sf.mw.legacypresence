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
* Description: CXcapDocument
*
*/




// INCLUDE FILES
#include <f32file.h>
#include <XdmErrors.h>
#include "XdmOperationFactory.h"
#include "XcapProtocol.h"
#include "XcapDocument.h"
#include "XcapAppUsage.h"
#include "XdmXmlParser.h"
#include "XdmNamespace.h"
#include "CommonDefines.h"
#include "XcapDocumentNode.h"
#include "XcapHttpRequest.h"
#include "XcapHttpOperation.h"
#include "XcapHttpTransport.h"

// ================= MEMBER FUNCTIONS =======================
//


// ----------------------------------------------------------
// CXcapDocument::CXcapDocument
// 
// ----------------------------------------------------------
//
CXcapDocument::CXcapDocument( TXdmDocType aDocumentType,
                              const CXdmEngine& aXdmEngine,
                              const CXcapProtocol& aXcapProtocol ) :
                              CXdmDocument( CONST_CAST( CXdmEngine&, aXdmEngine ) ),
                              iDocumentType( aDocumentType ),
                              iXcapProtocol( CONST_CAST( CXcapProtocol&, aXcapProtocol ) )
                                                
    { 
    }

// ----------------------------------------------------------
// CXcapDocument::NewL
// 
// ----------------------------------------------------------
//
CXcapDocument* CXcapDocument::NewL( TXdmDocType aDocumentType,
                                    const TDesC& aDocumentName,
                                    const CXdmEngine& aXdmEngine,
                                    const CXcapProtocol& aXcapProtocol )
    {
    CXcapDocument* self = new ( ELeave ) CXcapDocument( aDocumentType, aXdmEngine, aXcapProtocol );
    CleanupStack::PushL( self );
    self->BaseConstructL( KXcapOperationFactory, aDocumentName );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXcapDocument::NewL
// 
// ----------------------------------------------------------
//
CXcapDocument* CXcapDocument::NewL( TXdmDocType aDocumentType,
                                    const TDesC8& aDocumentName,
                                    const CXdmEngine& aXdmEngine,
                                    const CXcapProtocol& aXcapProtocol )
    {
    CXcapDocument* self = new ( ELeave ) CXcapDocument( aDocumentType, aXdmEngine, aXcapProtocol );
    CleanupStack::PushL( self );
    self->ConstructL( aDocumentName );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXcapDocument::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapDocument::ConstructL( const TDesC8& aDocumentName )
    {
    BaseConstructL( KXcapOperationFactory, aDocumentName );
    ConstructL();
    }
    
// ----------------------------------------------------------
// CXcapDocument::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapDocument::ConstructL()
    {
    InstallAppUsageL();
    iErrorRoot = CXcapDocumentNode::NewL( iXdmEngine, iXcapProtocol );
    TPtrC8 root( iXcapProtocol.Transport().RootUri() );
    #ifdef _DEBUG
        HBufC8* buf = HBufC8::NewLC( iDocumentName->Des().Length() );
        buf->Des().Copy( iDocumentName->Des() );
        TPtr8 nameDesc( buf->Des() );
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::ConstructL()" ) );
        iXcapProtocol.WriteToLog( _L8( "  Root URI:   %S" ), &root );
        iXcapProtocol.WriteToLog( _L8( "  Document:   %S" ), &nameDesc );
        CleanupStack::PopAndDestroy();  //buf
    #endif
    RXcapCache* cache = iXcapProtocol.Cache();
    if( cache )
        UpdateDocumentInfoL( cache );
    else
        {
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( "  Cache disabled, no info for the document" ) );
        #endif
        }
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------
// CXcapDocument::~CXcapDocument
// 
// ----------------------------------------------------
//
CXcapDocument::~CXcapDocument()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::~CXcapDocument()" ) );
    #endif
    Cancel();
    delete iTempCopy;
    delete iEightBitName;
    delete iDocumentRoot;
    delete iErrorRoot;
    delete iAppUsage;
    iNamespaces.ResetAndDestroy();
    iNamespaces.Close();
    }

// ----------------------------------------------------------
// CXcapDocument::ResetContents
// 
// ----------------------------------------------------------
//
void CXcapDocument::ResetContents()
    {
    iOptions = 0;
    delete iDocumentRoot;
    iDocumentRoot = NULL;
    delete iErrorRoot;
    iErrorRoot = NULL;
    delete iTempCopy;
    iTempCopy = NULL;
    iNamespaces.ResetAndDestroy();
    }
    
// ----------------------------------------------------------
// CXcapDocument::UpdateDocumentInfoL
// 
// ----------------------------------------------------------
//
void CXcapDocument::UpdateDocumentInfoL( RXcapCache* aCacheClient )
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::UpdateDocumentInfo()" ) );
    #endif
    iETagBuffer.Zero();
    TCacheEntryInfo cacheEntryInfo;
    TPtrC8 root( iXcapProtocol.Transport().RootUri() );
    TInt error = aCacheClient->FetchDocumentInfo( 
                 iETagBuffer, iDocumentName->Des(), root, cacheEntryInfo );    
    if( error == KErrNone )
        {
        iLastAccess = cacheEntryInfo.iLastAccess;
        iLastModification = cacheEntryInfo.iLastUpdate;
        iDataLength = cacheEntryInfo.iDataLength;
        #ifdef _DEBUG
            const TInt KDateTimeMaxSize = 100;
            TBuf8<KDateTimeMaxSize> printBuffer;
            TBuf<KDateTimeMaxSize> dateTimeBuffer;
            _LIT( KDateTimeFormat, "%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S");
            iLastModification.FormatL( dateTimeBuffer, KDateTimeFormat );
            printBuffer.Copy( dateTimeBuffer );
            iXcapProtocol.WriteToLog( _L8( "  ETag:        %S" ), &iETagBuffer );
            iXcapProtocol.WriteToLog( _L8( "  Size:        %d bytes" ), iDataLength );
            iXcapProtocol.WriteToLog( _L8( "  Time stamp:  %S" ), &printBuffer );
        #endif
        }
    else
        {
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( "  Cache read complete - Error: %d" ), error );
        #endif
        }
    }
    
// ----------------------------------------------------
// CXcapDocument::InstallAppUsageL
// 
// ----------------------------------------------------
//
void CXcapDocument::InstallAppUsageL()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::InstallAppUsageL()" ) );
    #endif  
    HBufC8* docName = NULL;
    iAppUsage = CXcapAppUsage::NewL( iXdmEngine, iDocumentType );
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::InstallAppUsageL() type = %d" ), iDocumentType );
    #endif  
    TPtrC8 auid = iAppUsage->AUID();
    if( IsGlobalTree( auid ) )
        {
        docName = HBufC8::NewLC( auid.Length() + KGlobalDirName().Length() + 
                                 KXcapGlobalDocName().Length() );
        docName->Des().Copy( auid );
        docName->Des().Append( KGlobalDirName );
        docName->Des().Append( KXcapGlobalDocName );
        delete iDocumentName;
        iDocumentName = NULL;
        iDocumentName = HBufC::NewL( docName->Des().Length() );
        iDocumentName->Des().Copy( docName->Des() );
        CleanupStack::PopAndDestroy();  //docName
        }
    else
        {
        TPtrC8 user = iXcapProtocol.UserName();
        docName = HBufC8::NewLC( auid.Length() + user.Length() + 
                                 KUserDirName().Length() + 1 /*'/'*/ + 
                                 iDocumentName->Des().Length() );
        docName->Des().Copy( auid );
        docName->Des().Append( KUserDirName );
        docName->Des().Append( user );
        docName->Des().Append( KSlash );
        docName->Des().Append( iDocumentName->Des() );
        delete iDocumentName;
        iDocumentName = NULL;
        iDocumentName = HBufC::NewL( docName->Des().Length() );
        iDocumentName->Des().Copy( docName->Des() );
        CleanupStack::PopAndDestroy();  //docName
        }
    
    }

// ----------------------------------------------------
// CXcapDocument::IsGlobalTree
// 
// ----------------------------------------------------
//
TBool CXcapDocument::IsGlobalTree( const TDesC8& aAuid )
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::IsGlobalTree()" ) );
    #endif
    TBool found = EFalse;
    //Update this array when necessary
    const TPtrC8 KGlobalTreeAuids[] = { TPtrC8( KXdmCapabilityUsageAUID ) };
    const TInt count = sizeof( KGlobalTreeAuids ) / sizeof( KGlobalTreeAuids[0] );
    for( TInt i = 0;!found && i < count;i++ )
        found = aAuid.Compare( KGlobalTreeAuids[i] ) == 0;
    return found;
    }
    
// ----------------------------------------------------
// CXcapDocument::CreateTempCopyL
// 
// ----------------------------------------------------
//
void CXcapDocument::CreateTempCopyL()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::CreateTempCopyL()" ) );
    #endif
    CXdmDocument* copy = iXdmEngine.CreateDocumentModelL( Name(), iDocumentType );
    iTempCopy = ( CXcapDocument* )copy;
    }

// ----------------------------------------------------
// CXcapDocument::TempCopyL
// 
// ----------------------------------------------------
//
EXPORT_C CXcapDocument* CXcapDocument::TempCopyL()
    {
    return CXcapDocument::NewL( iDocumentType, Name(), iXdmEngine, iXcapProtocol );
    }

// ----------------------------------------------------
// CXcapDocument::CreateRootL
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CXcapDocument::CreateRootL()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXdmDocument::CreateRootL()" ) );
    #endif
    delete iDocumentRoot;
    iDocumentRoot = NULL;
    iDocumentRoot = CXcapDocumentNode::NewL( iXdmEngine, iXcapProtocol );
    return iDocumentRoot;
    }

// ----------------------------------------------------
// CXcapDocument::AppendPathPartL
// 
// ----------------------------------------------------
//
void CXcapDocument::AppendPathPartL( const TDesC& aString )
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::PathPartL()" ) );
    #endif
    if( iDocumentRoot != NULL )
        {
        CXdmDocumentNode* node = NULL;
        CXdmDocumentNode* parent = iDocumentRoot;
        while( parent->NextNode() != NULL )
            parent = parent->NextNode();
        node = CXcapDocumentNode::NewL( iXdmEngine, aString, iXcapProtocol, parent );
        parent->SetNextNode( node );
        }
    else iDocumentRoot = CXcapDocumentNode::NewL( iXdmEngine, aString, iXcapProtocol );
    }

// ----------------------------------------------------
// CXcapDocument::CurrentExtent
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CXcapDocument::CurrentExtent() const
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXdmDocument::CurrentExtent()" ) );
    #endif
    CXdmDocumentNode* ret = NULL;
    if( iDocumentRoot != NULL )
        {
        CXdmDocumentNode* node = iDocumentRoot;
        while( node->NextNode() != NULL )
            node = node->NextNode();
        ret = node;
        }
    else
        {
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( " This document does not yet have a root, leave with KErrGeneral" ) );
        #endif
        User::Leave( KErrGeneral );
        }
    return ret;
    }
    
// ----------------------------------------------------
// CXcapDocument::DocumentRoot
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CXcapDocument::DocumentRoot() const
    {
    return iDocumentRoot;
    }     

// ----------------------------------------------------
// CXcapDocument::ErrorRoot
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CXcapDocument::ErrorRoot()
    {
    if( !iErrorRoot )
        {
        iErrorRoot = CXcapDocumentNode::NewL( iXdmEngine, iXcapProtocol );
        }
    return iErrorRoot;
    }

// ----------------------------------------------------
// CXcapDocument::ResetSubset
// 
// ----------------------------------------------------
//
EXPORT_C void CXcapDocument::ResetSubset()
    {
    CXdmDocument::ResetSubset();
    delete iDocumentRoot;
    iDocumentRoot = NULL;
    iOptions = 0;
    }
           
// ----------------------------------------------------
// CXcapDocument::ETag
// 
// ----------------------------------------------------
//
EXPORT_C TDesC8& CXcapDocument::ETag()
    {
    return iETagBuffer;
    }
        
// ----------------------------------------------------
// CXcapDocument::SetETag
// 
// ----------------------------------------------------
//     
EXPORT_C void CXcapDocument::SetETag( const TDesC8& aETagDesc )
    {
    iETagBuffer.Zero();
    iETagBuffer.Copy( aETagDesc );
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::SetETag()" ) );
        iXcapProtocol.WriteToLog( _L8( "  New ETag: %S" ), &iETagBuffer );
    #endif
    }

// ----------------------------------------------------
// CXcapDocument::DataLength
// 
// ----------------------------------------------------
//
EXPORT_C TInt CXcapDocument::DataLength() const
    {
    return iDataLength;
    }

// ----------------------------------------------------
// CXcapDocument::ApplicationUsage
// 
// ----------------------------------------------------
//
EXPORT_C CXcapAppUsage& CXcapDocument::ApplicationUsage() const
    {
    return *iAppUsage;
    }

// ----------------------------------------------------
// CXcapDocument::Protocol
// 
// ----------------------------------------------------
//
EXPORT_C CXcapProtocol& CXcapDocument::Protocol() const
    {
    return iXcapProtocol;
    }
    
// ----------------------------------------------------
// CXcapDocument::RemoveData
// 
// ----------------------------------------------------
//
EXPORT_C void CXcapDocument::RemoveData( CXcapDocumentNode* aDocumentNode )
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::RemoveData()" ) );
    #endif
    CXcapDocumentNode* parent = ( CXcapDocumentNode* )aDocumentNode->Parent();
    if( parent != NULL )
        parent->RemoveNode( aDocumentNode );
    }
    
// ----------------------------------------------------
// CXcapDocument::StartUpdateL
// 
// ----------------------------------------------------
//
void CXcapDocument::StartUpdateL()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::StartUpdateL()" ) );
    #endif
    TInt resolverError = 0;
    if( iXcapProtocol.IsSimRequestPending( resolverError ) )
        {
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( "  IMS Resolver pending => add to queue" ) );
        #endif
        iXcapProtocol.AppendNotifyeeL( this );
        }
    else
        {
        if( resolverError == KErrNone && !IsActive() && iChangeRequests.Count() > 0 )
            {
            iUpdateIndex = 0;
            iDocumentState = EXdmDocUpdating;
            iOperationCount = iChangeRequests.Count();
            ActivateOperationL();
            }
        else
            {
            #ifdef _DEBUG
                iXcapProtocol.WriteToLog( _L8(
                 "  Already active, no operations or IMS resolver failed => complete with error" ) );
            #endif
            TInt error = resolverError == KErrNone ? KErrNotReady : resolverError; 
            User::RequestComplete( iClientStatus, error );
            }
        }
    }

// ----------------------------------------------------
// CXcapDocument::StartInternalL
// 
// ----------------------------------------------------
//
void CXcapDocument::StartInternalL( TRequestStatus& aStatus )
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::StartInternalL()" ) );
    #endif
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    StartUpdateL();
    }

// ----------------------------------------------------
// CXcapDocument::NotifyResolverCompleteL
// 
// ----------------------------------------------------
//
void CXcapDocument::NotifyResolverCompleteL( TInt aError )
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::NotifyResolverCompleteL()" ) );
    #endif
    if( aError == KErrNone )
        {
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( "  IMS Resolver ready" ) );
        #endif
        StartUpdateL();
        }
    else
        {
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( "  IMS Resolver failed => complete with %d" ), aError );
        #endif
        DequeueAll();
        User::RequestComplete( iClientStatus, aError );
        }
    }
    
// ----------------------------------------------------
// CXcapDocument::ActivateOperationL
// 
// ----------------------------------------------------
//
void CXcapDocument::ActivateOperationL()
    {
    MXdmOperation* request = iChangeRequests[iUpdateIndex];
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::ActivateOperation() - Operation: %x" ), request );
    #endif
    if( iDocSubset )
        {
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( " Document subset => set KXdmOption1 and KXdmOption2" ) );
        #endif
        SetOption( KXdmOption1 );
        SetOption( KXdmOption2 );
        }
    if( iXcapProtocol.AuthType() == EXcapAuthEarlyIms )
        {
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( " EarlyIMS is in use => set KXdmOption4" ) );
        #endif
        SetOption( KXdmOption4 );
        }
    request->ExecuteL( iStatus, iOptions );
    SetActive();
    }

// ----------------------------------------------------
// CXcapDocument::SetOption
// 
// ----------------------------------------------------
//
void CXcapDocument::SetOption( TInt aOption )
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::SetOption()" ) );
    #endif
    iOptions |= aOption;
    }
       
// ----------------------------------------------------
// CXcapDocument::CancelUpdate
// 
// ----------------------------------------------------
//
void CXcapDocument::CancelUpdate()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::CancelUpdate()" ) );
    #endif
    TInt resolverError = 0;
    if( iXcapProtocol.IsSimRequestPending( resolverError ) )
        {
        iXcapProtocol.CancelImsResolver();
        }
    else
        {
        switch( iDocumentState )
            {
            case EResolvingAsyncConflict:
                iTempCopy->CancelUpdate();
                break;
            default:
                {
                TInt count = iChangeRequests.Count();
                if( count > 0 )
                    {
                    MXdmOperation* request = iChangeRequests[iUpdateIndex];
                    request->CancelOperation();
                    #ifdef _DEBUG
                        iXcapProtocol.WriteToLog( _L8( "  Operation %x cancelled" ), request );
                    #endif
                    }
                }
            }
        }
    User::RequestComplete( iClientStatus, KErrCancel );
    }

// ---------------------------------------------------------
// CXcapDocument::RunL()
// 
// ---------------------------------------------------------
//
void CXcapDocument::RunL()
    {
    #ifdef _DEBUG
        HBufC8* buf = HBufC8::NewLC( iDocumentName->Des().Length() );
        buf->Des().Copy( iDocumentName->Des() );
        TPtr8 nameDesc( buf->Des() );
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::RunL()" ) );
        iXcapProtocol.WriteToLog( _L8( "  Name:         %S" ), &nameDesc );
        iXcapProtocol.WriteToLog( _L8( "  Error:        %d" ), iStatus.Int() );
        iXcapProtocol.WriteToLog( _L8( "  Update index: %d" ), iUpdateIndex );
        CleanupStack::PopAndDestroy();  //buf
    #endif
    if( iStatus == KErrNone )
        {
        switch( iDocumentState )
            {
            case EXdmDocUpdating:
                #ifdef _DEBUG
                    iXcapProtocol.WriteToLog( _L8( "  Document state: EXdmDocUpdating" ) );
                #endif
                HandleRequestCompletionL();
                break;
            case EResolvingAsyncConflict:
                #ifdef _DEBUG
                    iXcapProtocol.WriteToLog( _L8( "  Document state: EResolvingAsyncConflict" ) );
                #endif
                ResolveAsyncConflictL();
                break;
            case ERetryingFailedOperation:
                #ifdef _DEBUG
                    iXcapProtocol.WriteToLog( _L8( "  Document state: ERetryingFailedOperation" ) );
                #endif
                ResolveAsyncConflictL();
                break;
            default:
                break;
            }
        }
    else HandleErrorL();
    #ifdef _DEBUG
        TInt biggestBlock( 0 );
        TInt memory( User::Available( biggestBlock ) );
        iXcapProtocol.WriteToLog( _L8( "** RunL() completes - Available memory: %d" ), memory );
    #endif
    }

// ---------------------------------------------------------
// CXcapDocument::HandleRequestCompletionL
// 
// ---------------------------------------------------------
//
void CXcapDocument::HandleRequestCompletionL()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::HandleRequestCompletionL()" ) );
    #endif
    MXdmOperation* request = iChangeRequests[iUpdateIndex];
    const TXdmCompletionData& data = request->CompletionData();
    switch( data.iCompletion )
        {
        case KInsertConflict:
            ResolveAsyncConflictL();
            break;
        default:
            {
            RXcapCache* cache = iXcapProtocol.Cache();
            if( cache )
                UpdateDocumentInfoL( cache );
            DequeueOperation( request );
            CheckOperationQueueL();
            break;
            }
        }
    }

// ---------------------------------------------------------
// CXcapDocument::ResolveAsyncConflictL
// 
// ---------------------------------------------------------
//
void CXcapDocument::ResolveAsyncConflictL()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::ResolveAsyncConflictL()" ) );
    #endif
    switch( iDocumentState )
        {
        //This means the cached version of this document
        //is out-of-date and that it needs to be updated.
        case EXdmDocUpdating:
            {
            CreateTempCopyL();
            iTempCopy->SetETag( iETagBuffer );
            iTempCopy->FetchDataL();
            iTempCopy->SaveClientStatus( iStatus );
            iTempCopy->StartUpdateL();
            iDocumentState = EResolvingAsyncConflict;
            SetActive();
            }
            break;
        case EResolvingAsyncConflict:
            #ifdef _DEBUG
                iXcapProtocol.WriteToLog( _L8( "  Async conflict resolved, retry insert" ) );
            #endif
            //CheckOperationQueueL() will increment the index
            iUpdateIndex--;
            SetETag( iTempCopy->ETag() );
            iDocumentState = ERetryingFailedOperation;
            CheckOperationQueueL();
            break;
        case ERetryingFailedOperation:
            #ifdef _DEBUG
                iXcapProtocol.WriteToLog( _L8( "  Operation retry ready" ) );
            #endif
            //Nothing can be done now but instruct the
            //client to re-fetch the whole document and
            //retry the failed operation.
            User::RequestComplete( iClientStatus, KErrPathNotFound );
            break;
        default:
            break;
        }
    }
    
// ---------------------------------------------------------
// CXcapDocument::HandleErrorL
// 
// ---------------------------------------------------------
//
void CXcapDocument::HandleErrorL()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::HandleErrorL()" ) );
    #endif
    TInt clientCode = KErrGeneral;
    MXdmOperation* request = request = iChangeRequests[iUpdateIndex];
    //The TXdmCompletionData is guaranteed to point at something 
    const TXdmCompletionData& data = request->CompletionData();
    switch( data.iCompletion )
        {
        case KErrCancel:
            #ifdef _DEBUG
                iXcapProtocol.WriteToLog( _L8( " Update cancelled" ) );
            #endif
            clientCode = KErrCancel;
            break;
        case ERetryingFailedOperation:
            #ifdef _DEBUG
                iXcapProtocol.WriteToLog( _L8( "  Operation retry failed" ) );
            #endif
            //Nothing can be done now but instruct the
            //client to re-fetch the whole document and
            //retry the failed operation.
            User::RequestComplete( iClientStatus, KXcapErrorUnrecoverableConflict );
            break;
        case KXcapErrorHttpConflict:
            #ifdef _DEBUG
                iXcapProtocol.WriteToLog( _L8( "  Conflict, parse error document if available" ) );
            #endif
            if( data.iResponseData != NULL )
                {
                TInt error = KErrNone;
                TPtrC8 errorData( data.iResponseData->Des() );
                TRAP( error, iXcapProtocol.Parser().ParseDocumentL( errorData, ErrorRoot() ) );
                #ifdef _DEBUG
                    iXcapProtocol.WriteToLog( _L8( "  Parsing completed: %d" ), error );
                    ErrorRoot()->Print();
                #endif
                //Suppress build warning
                error = KErrNone;
                }
		        User::RequestComplete( iClientStatus, KXcapErrorHttpConflict );
            break;
        case KXcapErrorNetworkNotAvailabe:
            {
            #ifdef _DEBUG
                iXcapProtocol.WriteToLog( _L8( "  Network not available, check cache" ) );
            #endif
            if( iDataLength > 0 )
                {
                RXcapCache* cache = iXcapProtocol.Cache();
                if( cache != NULL )
                    {
                    TPtrC name( Name() );
                    TInt error = KErrNone;
                    TPtrC8 root = iXcapProtocol.Transport().RootUri();
                    HBufC8* data = HBufC8::NewLC( iDataLength );
                    TPtr8 dataPtr( data->Des() );
                    cache->FetchDocumentContent( dataPtr, name, root );
                    TRAP( error, iXcapProtocol.Parser().ParseDocumentL( this, dataPtr ) );
                    #ifdef _DEBUG
                        iXcapProtocol.WriteToLog( _L8( "  Parsing completed: %d" ), error );
                    #endif
                    //Suppress build warning
                    error = KErrNone;
                    CleanupStack::PopAndDestroy();  //data
                    }
                }
            else
                {
                #ifdef _DEBUG
                    iXcapProtocol.WriteToLog( _L8( "  No data in cache, nothing to do" ) );
                #endif
                }
            User::RequestComplete( iClientStatus, KXcapErrorNetworkNotAvailabe );
            }
            break;
        default:
            #ifdef _DEBUG
                iXcapProtocol.WriteToLog( _L8( " Default case - Result: %d" ), data.iCompletion );
            #endif
            clientCode = iStatus.Int();
            User::RequestComplete( iClientStatus, clientCode );
        }
    DequeueOperation( request );
    }
     
// ---------------------------------------------------------
// CXcapDocument::CheckOperationQueueL
// 
// ---------------------------------------------------------
//
void CXcapDocument::CheckOperationQueueL()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::CheckOperationQueueL()" ) );
    #endif
    iUpdateIndex++;
    if( iUpdateIndex < iOperationCount )
        ActivateOperationL();
    else
        {
        DequeueAll();
        iUpdateIndex = 0;
        iXcapProtocol.CheckActivity();
        User::RequestComplete( iClientStatus, KErrNone );   
        }
    }

// ---------------------------------------------------------
// CXcapDocument::DequeueOperation
// 
// ---------------------------------------------------------
//
void CXcapDocument::DequeueOperation( MXdmOperation* aOperation )
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::DequeueOperation()" ) );
    #endif
    TInt index = iChangeRequests.Find( aOperation );
    if( index >= 0 )
    	{	
		iChangeRequests.Remove( index );    
        aOperation->Destroy();
        aOperation = NULL;
    	}  
   	iOperationCount = iChangeRequests.Count();
    }

// ---------------------------------------------------------
// CXcapDocument::DequeueAll
// 
// ---------------------------------------------------------
//
void CXcapDocument::DequeueAll()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::DequeueAll()" ) );
    #endif
    TInt count = iChangeRequests.Count();
    for( TInt i = 0;i < count;i++ )
    	{
    	MXdmOperation* request = iChangeRequests[i];
    	iChangeRequests.Remove( i );
    	request->Destroy();
    	request = NULL;
    	} 
    }
      
// ---------------------------------------------------------
// CXcapDocument::EightBitNameLC
// 
// ---------------------------------------------------------
//
HBufC8* CXcapDocument::EightBitNameLC()
    {
    /*#ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::EightBitNameLC()" ) );
    #endif*/
    __ASSERT_DEBUG( Name().Length() > 0, User::Panic( _L( "CXcapDocument" ), 1 ) );
    HBufC8* buf = HBufC8::NewLC( Name().Length() );
    buf->Des().Copy( Name() );
    return buf;
    }

// ---------------------------------------------------------
// CXcapDocument::DocumentType
// 
// ---------------------------------------------------------
//
TXdmDocType CXcapDocument::DocumentType() const
    {
    return iDocumentType;
    }

// ---------------------------------------------------------
// CXcapDocument::AppendNamespaceL
// 
// ---------------------------------------------------------
//
void CXcapDocument::AppendNamespaceL( const TDesC8& aUri, const TDesC8& aPrefix )
    {
    CXdmNamespace* ns = CXdmNamespace::NewL( aUri, aPrefix );
    CleanupStack::PushL( ns );
    User::LeaveIfError( iNamespaces.Append( ns ) );
    CleanupStack::Pop();  //ns
    }

// ---------------------------------------------------------
// CXcapDocument::RemoveNamespace
// 
// ---------------------------------------------------------
//
void CXcapDocument::RemoveNamespace( const TDesC8& aUri )
    {
    TBool found = EFalse;
    CXdmNamespace* ns = NULL;
    TInt count = iNamespaces.Count();
    for( TInt i = 0;!found && i < count;i++ )
        {
        ns = iNamespaces[i];
        if( ns->Uri().Compare( aUri ) == 0 )
            {
            found = ETrue;
            iNamespaces.Remove( i );
            delete ns;
            ns = NULL;
            }
        }
    }
                    
// ---------------------------------------------------------
// CXcapDocument::Uri
// 
// ---------------------------------------------------------
//
TPtrC8 CXcapDocument::Uri( const TDesC8& aPrefix ) const
    {
    TPtrC8 uri( _L8( "" ) );
    TBool found = EFalse;
    TInt count = iNamespaces.Count();
    for( TInt i = 0;!found && i < count ;i++ )
        {
        if( iNamespaces[i]->Prefix().Compare( aPrefix ) == 0 )
            {
            uri.Set( iNamespaces[i]->Uri() );
            found = ETrue;
            }
        }
    return uri;
    }

// ---------------------------------------------------------
// CXcapDocument::Count
// 
// ---------------------------------------------------------
//
TInt CXcapDocument::Count() const
    {
    return iNamespaces.Count();
    }

// ---------------------------------------------------------
// CXcapDocument::Prefix
// 
// ---------------------------------------------------------
//
TPtrC8 CXcapDocument::Prefix( TInt aIndex ) const
    {
    TInt count = iNamespaces.Count();
    if( count > 0 && ( aIndex >= 0 && aIndex < count ) )
        return iNamespaces[aIndex]->Prefix();
    else return TPtrC8();
    }
    
// ---------------------------------------------------------
// CXcapDocument::Uri
// 
// ---------------------------------------------------------
//
TPtrC8 CXcapDocument::Uri( TInt aIndex ) const
    {
    TInt count = iNamespaces.Count();
    if( count > 0 && ( aIndex >= 0 && aIndex < count ) )
        return iNamespaces[aIndex]->Uri();
    else return TPtrC8();
    }
    
// ---------------------------------------------------------
// CXcapDocument::ResetNamespaces
// 
// ---------------------------------------------------------
//
void CXcapDocument::ResetNamespaces( ) 
    {   
    iNamespaces.ResetAndDestroy();
    } 
    
// ---------------------------------------------------------
// CXcapDocument::TimeStamp
// 
// ---------------------------------------------------------
//
TTime CXcapDocument::TimeStamp() const
    {
    return iLastModification;
    }

// ----------------------------------------------------
// CXdmDocument::SaveClientStatus
// 
// ----------------------------------------------------
//
void CXcapDocument::SaveClientStatus( TRequestStatus& aClientStatus )
    {
    iClientStatus = &aClientStatus;        
    }
            
// ---------------------------------------------------------
// CXcapDocument::DoCancel
// 
// ---------------------------------------------------------
//
void CXcapDocument::DoCancel()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocument::DoCancel()" ) );
    #endif
    if( iClientStatus )
        {
        User::RequestComplete( iClientStatus, KErrCancel );
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( " Request completed" ) );
        #endif
        }
    else
        {
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( " iClientStatus == NULL => do nothing" ) );
        #endif
        }
    }




