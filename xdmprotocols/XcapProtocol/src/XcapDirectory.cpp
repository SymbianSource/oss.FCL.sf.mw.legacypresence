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
* Description: CXcapDirectory
*
*/




// INCLUDE FILES
#include <utf.h>
#include <f32file.h>
#include "XcapAppUsageDef.h"
#include "XdmProtocol.h"
#include "XcapDocument.h"
#include "XcapProtocol.h"
#include "XcapAppUsage.h"
#include "XcapRetrieval.h"
#include "XcapDirectory.h"
#include "XdmXmlParser.h"
#include "XcapHttpReqGet.h"
#include "XdmDocumentNode.h"
#include "XdmNodeAttribute.h"
#include "XcapHttpTransport.h"
#include "XdmOperationFactory.h"

// ----------------------------------------------------------
// CXcapDirectory::CXcapDirectory
// 
// ----------------------------------------------------------
//
CXcapDirectory::CXcapDirectory( CXdmDocument*& aDirectoryDoc,
                                CXdmEngine& aXdmEngine,
                                CXcapProtocol& aXcapProtocol ) :
                                CXdmDirectory( aXdmEngine ),
                                iDirectoryDoc( ( CXcapDocument* )aDirectoryDoc ),
                                iXcapProtocol( aXcapProtocol )                                             
    { 
    }

// ----------------------------------------------------------
// CXcapDirectory::NewL
// 
// ----------------------------------------------------------
//
CXcapDirectory* CXcapDirectory::NewL( const TDesC& aAUID,
                                      CXdmEngine& aXdmEngine,
                                      CXdmDocument*& aDirectoryDoc,
                                      CXcapProtocol& aXcapProtocol )
    {
    CXcapDirectory* self = new ( ELeave ) CXcapDirectory( aDirectoryDoc, aXdmEngine, aXcapProtocol );
    CleanupStack::PushL( self );
    self->ConstructL( aAUID );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CXcapDirectory::~CXcapDirectory
// 
// ----------------------------------------------------
//
CXcapDirectory::~CXcapDirectory()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDirectory::~CXcapDirectory()" ) );
    #endif
    Cancel();
    delete iAUID;
    delete iDirectoryDoc;
    delete iHttpRetrieval;
    iUpdateList.Close();
    iDocumentList.Close();
    }
    
// ----------------------------------------------------------
// CXcapDirectory::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapDirectory::ConstructL( const TDesC& aAUID )
    {
    if( aAUID.Length() > 0 )
        {
        _LIT( KNodePath,    "xcap-directory/folder" );
        iAUID = HBufC::NewL( aAUID.Length() );
        iAUID->Des().Copy( aAUID );
        CXdmDocumentNode* node = iDirectoryDoc->DocumentSubsetL( KNodePath );
        CleanupStack::PushL( node );
        CXdmNodeAttribute* auid = node->CreateAttributeL( KAuidAttribute );
        CleanupStack::PushL( auid );
        auid->SetAttributeValueL( aAUID );
        iDirectoryDoc->FetchDataL( node );
        //Set the option that prohibits caching
        iDirectoryDoc->SetOption( KXdmOption3 );
        CleanupStack::Pop( 2 );  //auid, node
        }
    else iDirectoryDoc->FetchDataL();
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------
// CXcapDirectory::StartUpdateL
// 
// ----------------------------------------------------------
//
void CXcapDirectory::StartUpdateL()
    
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDirectory::StartUpdateL()" ) );
    #endif
    switch( iUpdatePhase )
        {
        case EDirPhaseIdle:
            User::RequestComplete( iClientStatus, KErrArgument );
            break;
        case EUpdateDocumentList:
            HandleDocumentListRequestL();
            break;
        case ERefreshDocuments:
            HandleRefreshRequestL();
            break;
        default:
            break;
        } 
    }

// ----------------------------------------------------------
// CXcapDirectory::SaveRequestData
// 
// ----------------------------------------------------------
//
void CXcapDirectory::SaveRequestData( TDirUpdatePhase aUpdatePhase,
                                      TRequestStatus& aClientStatus )
    {
    iUpdatePhase = aUpdatePhase;
    iClientStatus = &aClientStatus;
    }
    
// ----------------------------------------------------
// CXcapDirectory::HandleDocumentListRequestL
// 
// ----------------------------------------------------
//
void CXcapDirectory::HandleDocumentListRequestL()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDirectory::HandleDocumentListRequestL()" ) );
    #endif
    if( !IsActive() )
        {
        CXcapDocument* doc = ( CXcapDocument* )iDirectoryDoc;
        doc->StartInternalL( iStatus );
        iUpdatePhase = EUpdateDocumentList;
        SetActive();
        }
    }
    
// ----------------------------------------------------
// CXcapDirectory::HandleRefreshRequestL
// 
// ----------------------------------------------------
//
void CXcapDirectory::HandleRefreshRequestL()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDirectory::HandleRefreshRequestL()" ) );
    #endif
    if( iUpdatePhase == EUpdateDocumentList )
        {
        if( iUpdateList.Count() > 0 )
            {
            iUpdateIndex = -1;
            iHttpRetrieval = iXcapProtocol.Transport().GetL( TPtrC() );
            UpdateNextDocumentL();
            iUpdatePhase = ERefreshDocuments;
            }
        else
            {
            #ifdef _DEBUG
                iXcapProtocol.WriteToLog( _L8( "  All documents are up to date" ) );
            #endif
            User::RequestComplete( iClientStatus, KErrNone );
            }
        }
    else
        {
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( "  Wrong state [%d]" ), iUpdatePhase );
        #endif
        User::RequestComplete( iClientStatus, KErrNotReady );
        }
    }

// ----------------------------------------------------
// CXcapDirectory::UpdateNextDocumentL
// 
// ----------------------------------------------------
//
void CXcapDirectory::UpdateNextDocumentL()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDirectory::UpdateNextDocumentL()" ) );
    #endif
    iUpdateIndex++;
    if( iUpdateIndex < iUpdateList.Count() )
        {
        CXdmDocumentNode* node = iUpdateList[iUpdateIndex];
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( "  Index: %d  Document: %x" ), iUpdateIndex, node );
        #endif
        TPtrC8 root = iXcapProtocol.Transport().RootUri();
        TPtrC docUri( ExtractDocumentUriL( root, node ) );
        HBufC* uri = HBufC::NewLC( root.Length() + docUri.Length() );
        uri->Des().Copy( root );
        uri->Des().Append( docUri );
        iHttpRetrieval->ResetUriL( uri->Des() );
        iHttpRetrieval->SetExpiryTimeL( NULL, KDefaultHttpRequestTimeout * 1000000 );
        iHttpRetrieval->DispatchRequestL( iStatus );
        CleanupStack::PopAndDestroy();  //uri
        SetActive();
        }
    else
        {
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( "  Directory sync complete" ) );
        #endif
        User::RequestComplete( iClientStatus, KErrNone );
        }
    }

// ---------------------------------------------------------
// CXcapDirectory::RunL()
// 
// ---------------------------------------------------------
//
void CXcapDirectory::RunL()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDirectory::RunL() - Request: %d" ), iStatus.Int() );
    #endif
    if( iStatus == KErrNone )
        {
        switch( iUpdatePhase )
            {
            case EUpdateDocumentList:
                {
                iDocumentCount = iDirectoryDoc->Find( KDirectoryEntry, iDocumentList );
                if( iDocumentCount > 0 )
                    {
                    TInt index = 0;
                    while( index < iDocumentCount )
                        {
                        TBool uptodate = CheckLocalCopyL( iDocumentList[index] );
                        #ifdef _DEBUG
                            iXcapProtocol.WriteToLog( _L8( "  Cache: %d" ), uptodate );
                        #endif
                        if( !uptodate )
                            User::LeaveIfError( iUpdateList.Append( iDocumentList[index] ) );
                        index++;
                        }
                    }
                User::RequestComplete( iClientStatus, iStatus.Int() );
                }
                break;
            case ERefreshDocuments:
                {
                CXdmDocumentNode* node = iUpdateList[iUpdateIndex];
                #ifdef _DEBUG
                    iXcapProtocol.WriteToLog( _L8( "  Index:    %d" ), iUpdateIndex );
                    iXcapProtocol.WriteToLog( _L8( "  Document: %x" ), node );
                    iXcapProtocol.WriteToLog( _L8( "  Error:    %d" ), iStatus.Int() );
                #endif
                TPtrC8 root = iXcapProtocol.Transport().RootUri();
                TPtrC docName = ExtractDocumentUriL( root, node );
                TXdmCompletionData* data = iHttpRetrieval->ResponseData();
                TPtrC8 responseData( data->iResponseData->Des() );
                TPtrC8 eTag( data->iETag->Des() );
                RXcapCache* cache = iXcapProtocol.Cache();
                cache->Store( eTag, docName, root, responseData  );
                iHttpRetrieval->ReleaseResponseData();
                UpdateNextDocumentL();
                }
                break;
            default:
                #ifdef _DEBUG
                    iXcapProtocol.WriteToLog( _L8( "  Default case, unknown state %d - Status: %d" ),
                                               iUpdatePhase, iStatus.Int() );
                #endif
                if( iClientStatus != NULL )
                    User::RequestComplete( iClientStatus, KErrUnknown );  
                break;
            }   
        }
    else
        {
        #ifdef _DEBUG
            iXcapProtocol.WriteToLog( _L8( "  Operation failed with %d" ), iStatus.Int() );
        #endif
        User::RequestComplete( iClientStatus, iStatus.Int() );
        }
    }

// ---------------------------------------------------------
// CXcapDirectory::CheckLocalCopyL
// 
// ---------------------------------------------------------
//
TBool CXcapDirectory::CheckLocalCopyL( const CXdmDocumentNode* aDocumentNode )
    {
    /*#ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDirectory::CheckLocalCopyL()" ) );
    #endif*/
    TBool ret = EFalse;
    TPtrC8 root = iXcapProtocol.Transport().RootUri();
    TPtrC docName = ExtractDocumentUriL( root, aDocumentNode );
    TPtrC etag( aDocumentNode->Attribute( KDocumentETag )->AttributeValue() );
    HBufC8* etag8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( etag );
    CleanupStack::PushL( etag8 );
    RXcapCache* cache = iXcapProtocol.Cache();
    ret = cache != NULL ? cache->IsCurrent( etag8->Des(), docName, root ) == KErrNone : ETrue;
    CleanupStack::PopAndDestroy();  //etag8
    return ret;
    }

// ---------------------------------------------------------
// CXcapDirectory::ExtractDocumentUriL
// 
// ---------------------------------------------------------
//
TPtrC CXcapDirectory::ExtractDocumentUriL( const TDesC8& aRootUri,
                                           const CXdmDocumentNode* aDocumentNode )
    {
    /*#ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDirectory::ExtractDocumentUri()" ) );
    #endif*/
    const TUint8 KHttpHostSeparator = 47;
    _LIT8( KHttpDoubleHostSeparator,  "//" );
    TPtrC uri( aDocumentNode->Attribute( KDocumentUri )->AttributeValue() );
    if( uri[0] == KHttpHostSeparator ) //Relative URI
        return uri;
    else
        {
        TInt index = aRootUri.Find( KHttpDoubleHostSeparator );
        TPtrC8 root( aRootUri.Right( aRootUri.Length() - index - 2 ) );
        HBufC8* uri8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( uri );
        CleanupStack::PushL( uri8 );
        TPtr8 desc( uri8->Des() );
        index = desc.Find( root );
        CleanupStack::PopAndDestroy();  //uri8
        return index >= 0 ? uri.Right( uri.Length() - root.Length() - index ) : TPtrC();
        }
    }

// ----------------------------------------------------
// CXcapDirectory::DocumentCount
// 
// ----------------------------------------------------
//
TInt CXcapDirectory::DocumentCount()
    {
    return iDocumentCount;
    }

// ----------------------------------------------------
// CXcapDirectory::DocumentType
// 
// ----------------------------------------------------
//
TXdmDocType CXcapDirectory::DocumentTypeL( TInt aIndex ) const
    {
    TXdmDocType type = EXdmDocGeneral;
    if( iUpdatePhase <= EDirPhaseIdle )
        User::Leave( KErrNotReady );
    else if( iDocumentCount > 0 && ( aIndex >= 0 && aIndex < iDocumentCount ) )
        type = FindAUID( *iDocumentList[aIndex] );
    return type;
    } 

// ----------------------------------------------------
// CXcapDirectory::FindAUID
// 
// ----------------------------------------------------
//
TXdmDocType CXcapDirectory::FindAUID( const CXdmDocumentNode& aEntryNode ) const
    {
    TXdmDocType documentType = EXdmDocGeneral;
    HBufC8* buf = aEntryNode.Attribute( KDocumentUri )->EightBitValueLC();
    if( buf )
        {
        TPtrC8 uri( buf->Des() );
        if( uri.Find( KXdmCapabilityUsageAUID ) >= 0 )
            documentType = EXdmCapabilities;
        else if( uri.Find( KXdmPocUserAccessUsageAUID ) >= 0 )
            documentType = EXdmPoCUserAccessPolicy;
        else if( uri.Find( KXdmPocGroupUsageAUID ) >= 0 )
            documentType = EXdmPoCGroup;
        else if( uri.Find( KXdmResourceListsUsageAUID ) >= 0 )
            documentType = EXdmResourceLists;
        else if( uri.Find( KXdmOmaPresRulesUsageAUID ) >= 0 )
            documentType = EXdmIetfPresenceRules;
        else if( uri.Find( KXdmDirectoryUsageAUID ) >= 0 )
            documentType = EXdmDirectory;
        else if( uri.Find( KXdmSharedXDMUsageAUID ) >= 0 )
            documentType = EXdmSharedXdm;
        else if( uri.Find( KXdmRlsServicesUsageAUID ) >= 0 )
            documentType = EXdmRlsServices;
        CleanupStack::PopAndDestroy();  //EightBitValueLC()
        }
    return documentType;
    }
                 
// ----------------------------------------------------
// CXcapDirectory::Document
// 
// ----------------------------------------------------
//
TPtrC CXcapDirectory::Document( TInt aIndex ) const
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDirectory::Document() - Index: %d" ), aIndex );
    #endif
    TChar slash = 47;
    if( aIndex >= 0 && aIndex < iDocumentList.Count() ) 
        {
        TPtrC ret;
        TPtrC uri = iDocumentList[aIndex]->Attribute( KDocumentUri )->AttributeValue();
        TInt index = uri.LocateReverse( slash );
        index > 0 ? ret.Set( uri.Right( uri.Length() - index - 1 ) ) : ret.Set( uri );
        return ret;
        }
    else return TPtrC();
    }
        
// ----------------------------------------------------
// CXcapDirectory::DirectoryPath
// 
// ----------------------------------------------------
//
TPtrC CXcapDirectory::DirectoryPath() const
    {
    return iAUID != NULL ? iAUID->Des() : TPtrC();
    }

// ----------------------------------------------------
// CXcapDirectory::CancelUpdate
// 
// ----------------------------------------------------
//
void CXcapDirectory::CancelUpdate()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDirectory::CancelUpdate()" ) );
    #endif
    Cancel();
    }
    
// ---------------------------------------------------------
// CXcapDirectory::DoCancel
// 
// ---------------------------------------------------------
//
void CXcapDirectory::DoCancel()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDirectory::DoCancel()" ) );
    #endif
    switch( iUpdatePhase )
        {
        case EDirPhaseIdle:
            break;
        case EUpdateDocumentList:
            {
            CXcapDocument* doc = ( CXcapDocument* )iDirectoryDoc;
            doc->CancelUpdate();
            }
            break;
        case ERefreshDocuments:
            iHttpRetrieval->CancelRequest();
            break;
        default:
            break;
        }
    User::RequestComplete( iClientStatus, KErrCancel );
    }




