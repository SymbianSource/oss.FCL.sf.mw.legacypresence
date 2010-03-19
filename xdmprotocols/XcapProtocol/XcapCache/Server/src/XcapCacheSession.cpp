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
* Description:   CXcapCacheSession
*
*/




// INCLUDE FILES
#include <e32std.h>
#include <f32file.h>
#include <badesca.h>
#include "XcapCacheIndex.h"
#include "XcapCacheSession.h"
#include "XcapCacheIndexAdmin.h"
#include "XcapCacheIndexEntry.h"
#include "XcapCacheIndexTableEntry.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CXcapCacheSession::NewL
// 
// ----------------------------------------------------------
//
CXcapCacheSession* CXcapCacheSession::NewL( CXcapCacheServer* aServer )
    {
    CXcapCacheSession* self = new( ELeave ) CXcapCacheSession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXcapCacheSession::~CXcapCacheSession
// 
// ----------------------------------------------------------
//
CXcapCacheSession::~CXcapCacheSession()
    {
    delete iFileManager;
    //Reduce number of active clients within server
    if( iCacheServer )
        iCacheServer->DropSession();
    }

// ----------------------------------------------------------
// CXcapCacheSession::ServiceL
// 
// ----------------------------------------------------------
//
void CXcapCacheSession::ServiceL( const RMessage2& aMessage )
    {
    TInt ret = KErrNone;
    if( Capabilities( aMessage ) )
        {
        CheckTempBuffer();
        TRAPD( err, DispatchMessageL( aMessage ) );
        if( err != KErrNone )
            ret = err;
        }
    else ret = KErrPermissionDenied;
    aMessage.Complete( ret );
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "** Message completed - Error: %d" ), ret );
    #endif
    }

// ----------------------------------------------------------
// CXcapCacheSession::Capabilities
// 
// ----------------------------------------------------------
//
TBool CXcapCacheSession::Capabilities( const RMessage2& aMessage )
    {
    return aMessage.HasCapability( ECapabilityReadUserData ) &&
           aMessage.HasCapability( ECapabilityWriteUserData );
    }
    
// ----------------------------------------------------------
// CXcapCacheSession::CheckTempBuffer
// 
// ----------------------------------------------------------
//
void CXcapCacheSession::CheckTempBuffer()
    {
    if( iTempBuffer )
        {
        delete iTempBuffer;
        iTempBuffer = NULL;
        }
    }
    
// ----------------------------------------------------------
// CXcapCacheSession::CXcapCacheSession
// 
// ----------------------------------------------------------
//
CXcapCacheSession::CXcapCacheSession( CXcapCacheServer* aServer ) :
                                      iCacheServer( aServer ),
                                      iCacheIndex( *CXcapCacheServer::Index() ),
                                      iCacheIndexAdmin( *CXcapCacheServer::IndexAdmin() )
    {
    }

// ----------------------------------------------------------
// CXcapCacheSession::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapCacheSession::ConstructL()
    {
    iCacheServer->AddSession();
    iFileManager = CFileMan::NewL( iCacheServer->FileSession() );
    #ifdef _DEBUG
        TInt count = 0;
        iCacheServer->CacheSize( count );
    #endif
    }

// ----------------------------------------------------------
// CXcapCacheSession::PanicClient
// 
// ----------------------------------------------------------
//
void CXcapCacheSession::PanicClient( TInt aPanic )
    {
    _LIT( KTxtSessionPanic,"Test Server Session panic");
    User::Panic( KTxtSessionPanic, aPanic );
    }

// ----------------------------------------------------------
// CXcapCacheSession::DispatchMessageL
// 
// ----------------------------------------------------------
//
void CXcapCacheSession::DispatchMessageL( const RMessage2& aMessage )
    {
    switch( aMessage.Function() )
        {
        case EXcapCacheFetchInfo:
            FetchDocumentInfoL( aMessage );
            break;
        case EXcapCacheFetchData:
            FetchDocumentContentsL( aMessage );
            break;
        case EXcapCacheStore:
            {
            TInt entryCount = 0;
            CacheXcapDataL( aMessage );
            TInt size = iCacheServer->CacheSize( entryCount );
            CheckUnindexedEntriesL( entryCount );
            //Recheck the size
            size = iCacheServer->CacheSize( entryCount );
            const TInt maxSize = iCacheServer->MaxCacheSize();
            //If there's only one document filling the
            //whole data area, there's not much to be done
            //NOTE: Index & pagefile are always there => 3
            if( size >= maxSize && entryCount > 3 )
                {
                #ifdef _DEBUG
                    CXcapCacheServer::WriteToLog( _L8( " Cache too large: %d bytes" ), size );
                #endif
                iCacheIndex.SortEntries();
                const TInt compression( iCacheServer->MaxCacheSize() * KDefaultCompression );
                //NOTE: Index & pagefile are always there -> 3
                TBool ready = EFalse;
                while( entryCount > 3 && size > compression && !ready )
                    {
                    //The entries have been sorted => FIFO
                    CXcapCacheIndexEntry* entry = iCacheIndex.Entry( 0 );
                    if( entry )
                    	{
                        DeleteFromStorageL( entry->FileName16L() );
                        iCacheIndex.RemoveEntry( 0 );
                        iCacheIndexAdmin.RemoveTableIndex( 0 );
                        FlushCacheDataL();
                        size = iCacheServer->CacheSize( entryCount );
                    	}
                    else ready = ETrue;
                    }
                } 
            }  
            break;
        case EXcapCacheDelete:
            DeleteCacheDataL( aMessage );
            break;
        case EXcapCacheCheckValidity:
            CheckValidityL( aMessage );
            break;
        case EXcapCacheFlush:
            FlushCacheDataL();
            break;
        default:
            PanicClient( EBadRequest );
        }
    }

// ----------------------------------------------------------
// CXcapCacheSession::CheckUnindexedEntriesL
// 
// ----------------------------------------------------------
//
void CXcapCacheSession::CheckUnindexedEntriesL( TInt aTotalCount )
    {
    #ifdef _DEBUG
    	CXcapCacheServer::WriteToLog( _L8( "CXcapCacheSession::CheckUnindexedEntriesL()" ) );
        CXcapCacheServer::WriteToLog( _L8( "  Total count: %d" ), aTotalCount );
	#endif
    const TInt indexed = iCacheIndex.EntryCount();
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "  Indexed entries: %d" ), indexed );
	#endif
    if( aTotalCount > indexed + 2 )  //index + pagefile = 2
        {
        CDesC16ArraySeg* array = new ( ELeave ) CDesC16ArraySeg( indexed );
        CleanupStack::PushL( array );
        array->AppendL( KCacheServerIndexF() );
        array->AppendL( KCacheServerPageFileF() );
        for( TInt i = 0;i < indexed;i++ )
            {
            const TChar delimiter = 92;
            TPtrC temp( iCacheIndex.Entry( i )->FileName16L() );
            TPtrC entry( temp.Right( temp.LocateReverse( delimiter ) + 1 ) );
            array->AppendL( entry );
            }
        DeleteExcessL( *array );
        array->Reset();
        CleanupStack::PopAndDestroy();  //array
        }
    }

// ----------------------------------------------------------
// CXcapCacheSession::DeleteExcessL
// 
// ----------------------------------------------------------
//
void CXcapCacheSession::DeleteExcessL( const MDesC16Array& aIndexedEntries )
	{
	#ifdef _DEBUG
    	CXcapCacheServer::WriteToLog( _L8( "CXcapCacheSession::DeleteExcessL()" ) );
	#endif
    CDir* directory = NULL;
    RFs& session = CXcapCacheServer::FileSession();
    User::LeaveIfError( session.GetDir( KCacheServerRoot, KEntryAttNormal,
    					ESortNone, directory ) );
    CleanupStack::PushL( directory );
    const TInt count = directory->Count();
    for( TInt i = 0;i < count;i++ )
    	{
    	TPtrC targetName( ( *directory )[i].iName );
    	if( !IsIndexed( aIndexedEntries, targetName ) )
    		{
        	HBufC* fileName = HBufC::NewLC( targetName.Length() + 
    						  KCacheServerRoot().Length() );
    		fileName->Des().Copy( KCacheServerRoot );
    		fileName->Des().Append( targetName );
    		TPtrC name( fileName->Des() );
    		User::LeaveIfError( iFileManager->Delete( name ) );
    		CleanupStack::PopAndDestroy();  //fileName
    		}
    	}
    CleanupStack::PopAndDestroy();  //directory
	}

// ----------------------------------------------------------
// CXcapCacheSession::IsIndexed
// 
// ----------------------------------------------------------
//
TBool CXcapCacheSession::IsIndexed( const MDesC16Array& aIndexedEntries, const TDesC& aFileName )
    {
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheSession::IsIndexed()" ) );
    #endif
    TBool indexed = EFalse;
    const TInt count = aIndexedEntries.MdcaCount();
    for( TInt i = 0;!indexed && i < count;i++ )
    	{
    	if( aIndexedEntries.MdcaPoint( i ).Compare( aFileName ) == 0 )
    		{
    		indexed = ETrue;
    		}
    	}
    return indexed;
    }

// ----------------------------------------------------------
// CXcapCacheSession::ReadMsgParamLC
// 
// ----------------------------------------------------------
//
HBufC* CXcapCacheSession::ReadMsgParam16LC( TInt aMsgIndex, const RMessage2& aMessage )
    {
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheServer::ReadMsgParam16LC()" ) );
    #endif
    TInt length = aMessage.GetDesLength( aMsgIndex );
    HBufC* buffer = HBufC::NewLC( length );
    TPtr descriptor( buffer->Des() );
    aMessage.ReadL( aMsgIndex, descriptor );
    return buffer;
    }

// ----------------------------------------------------------
// CXcapCacheSession::ReadMsgParam8LC
// 
// ----------------------------------------------------------
//
HBufC8* CXcapCacheSession::ReadMsgParam8LC( TInt aMsgIndex, const RMessage2& aMessage )
    {
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheServer::ReadMsgParam8LC()" ) );
    #endif
    TInt length = aMessage.GetDesLength( aMsgIndex );
    HBufC8* buffer = HBufC8::NewLC( length );
    TPtr8 descriptor( buffer->Des() );
    aMessage.ReadL( aMsgIndex, descriptor );
    return buffer;
    }
       
// ----------------------------------------------------------
// CXcapCacheSession::CheckValidityL
// 
// ----------------------------------------------------------
//
void CXcapCacheSession::CheckValidityL( const RMessage2& aMessage )
    {
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "** CXcapCacheServer::CheckValidityL()" ) );
    #endif
    TPtrC8 etag( ReadMsgParam8LC( 0, aMessage )->Des() );
    TPtrC name( ReadMsgParam16LC( 1, aMessage )->Des() );
    TPtrC8 root( ReadMsgParam8LC( 2, aMessage )->Des() );
    TInt tableIndex = KErrNotFound;
    CXcapCacheIndexTableEntry* tableEntry = iCacheIndexAdmin.FindL( tableIndex, root, name );
    if( tableEntry != NULL )
        {
        CXcapCacheIndexEntry* entry = iCacheIndex.Entry( tableEntry->Index() );
        User::Leave( entry->ETag().Compare( etag ) == 0 ? KErrNone : KErrGeneral );
        }
    else User::Leave( KErrNotFound );
    CleanupStack::PopAndDestroy( 3 );  //root, name, etag
    }
    
// ----------------------------------------------------------
// CXcapCacheSession::DeleteCacheDataL
// 
// ----------------------------------------------------------
//
void CXcapCacheSession::DeleteCacheDataL( const RMessage2& aMessage )
    {
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "** CXcapCacheServer::DeleteCacheDataL()" ) );
    #endif
    TCacheEntryInfo header;
    TPckg<TCacheEntryInfo> pack( header );
    aMessage.ReadL( 0, pack );
    TInt tableIndex = KErrNotFound;
    CXcapCacheIndexTableEntry* tableEntry =
        iCacheIndexAdmin.FindL( tableIndex, *header.iRootUri, *header.iDocumentUri );
    if( tableEntry )
        {
        TInt error = KErrNone;
        TInt index( tableEntry->Index() );
        CXcapCacheIndexEntry* entry = iCacheIndex.Entry( index );
        error = DeleteFromStorageL( entry->FileName16L() );
        #ifdef _DEBUG
            CXcapCacheServer::WriteToLog(
                    _L8( "CXcapCacheSession::DeleteCacheDataL() - error: %d"), error  );
        #endif
        iCacheIndex.RemoveEntry( index );
        iCacheIndexAdmin.RemoveTableIndex( tableIndex );
        FlushCacheDataL();
        }
    else User::Leave( KErrNotFound );
    }
    
// ----------------------------------------------------------
// CXcapCacheSession::FetchDocumentInfoL
// 
// ----------------------------------------------------------
//
void CXcapCacheSession::FetchDocumentInfoL( const RMessage2& aMessage )
    {
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "** CXcapCacheServer::FetchDocumentInfoL()" ) );
    #endif
    TInt error = KErrNone;
    //Read info header
    TCacheEntryInfo header;
    TPckg<TCacheEntryInfo> pack( header );
    aMessage.ReadL( 0, pack );
    TPtrC name( ReadMsgParam16LC( 2, aMessage )->Des() );
    TPtrC8 root( ReadMsgParam8LC( 3, aMessage )->Des() );
    TInt tableIndex = KErrNotFound;
    #ifdef _DEBUG
        HBufC8* temp = HBufC8::NewLC( name.Length() );
        TPtr8 tempDesc( temp->Des() );
        tempDesc.Copy( name );
        CXcapCacheServer::WriteToLog( _L8( "   Root:     %S" ), &root );
        CXcapCacheServer::WriteToLog( _L8( "   Document: %S" ), &tempDesc );
        CleanupStack::PopAndDestroy();  //temp
    #endif
    CXcapCacheIndexTableEntry* tableEntry = iCacheIndexAdmin.FindL( tableIndex, root, name );
    if( tableEntry != NULL )
        {
        TInt index = tableEntry->Index();
        CXcapCacheIndexEntry* entry = iCacheIndex.Entry( index );
        TPtrC8 eTag = entry->ETag();
        header.iDataLength = entry->XmlSize();
        header.iLastAccess = entry->LastAccess();
        header.iLastUpdate = entry->LastModified();
        TPckgC<TCacheEntryInfo> package( header );
        TRAP( error, aMessage.WriteL( 0, package ) );
        #ifdef _DEBUG
            CXcapCacheServer::WriteToLog( _L8( "  Write header: %d" ), error );
        #endif
        TRAP( error, aMessage.WriteL( 1, eTag ) );
        #ifdef _DEBUG
            CXcapCacheServer::WriteToLog( _L8( "  Write ETag:   %d" ), error );
        #endif
        }
    else User::Leave( KErrNotFound );
    CleanupStack::PopAndDestroy( 2 );  //root, name
    }
    
// ----------------------------------------------------------
// CXcapCacheSession::FetchDocumentContentsL
// 
// ----------------------------------------------------------
//
void CXcapCacheSession::FetchDocumentContentsL( const RMessage2& aMessage )
    {
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "** CXcapCacheServer::FetchDocumentContentsL()" ) );
    #endif
    TPtrC name( ReadMsgParam16LC( 0, aMessage )->Des() );
    TPtrC8 root( ReadMsgParam8LC( 1, aMessage )->Des() );
    #ifdef _DEBUG
        HBufC8* document = HBufC8::NewLC( name.Length() );
        TPtr8 docDesc( document->Des() );
        docDesc.Copy( name );
        CXcapCacheServer::WriteToLog( _L8( "  Document:    %S" ), &docDesc );
        CXcapCacheServer::WriteToLog( _L8( "  Root URI:    %S" ), &root );
        CleanupStack::PopAndDestroy();  //document
    #endif
    TInt tableIndex = KErrNotFound;
    CXcapCacheIndexTableEntry* tableEntry = iCacheIndexAdmin.FindL( tableIndex, root, name );
    if( tableEntry != NULL )
        {
        CXcapCacheIndexEntry* entry = iCacheIndex.Entry( tableEntry->Index() );
        iTempBuffer = ReadFromStorageL( entry->FileName16L() );
        TPtrC8 dataDesc = iTempBuffer->Des();
        aMessage.Write( 2, dataDesc );
        }
    else User::Leave( KErrNotFound );
    CleanupStack::PopAndDestroy( 2 );  //name, root
    }
    
// ----------------------------------------------------------
// CXcapCacheSession::CacheXcapData
// 
// ----------------------------------------------------------
//
void CXcapCacheSession::CacheXcapDataL( const RMessage2& aMessage )
    {
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "** CXcapCacheServer::CacheXcapDataL()" ) );
    #endif
    TPtrC doc( ReadMsgParam16LC( 0, aMessage )->Des() );
    TPtrC8 root( ReadMsgParam8LC( 1, aMessage )->Des() );
    TPtrC8 etag( ReadMsgParam8LC( 2, aMessage )->Des() );
    TPtrC8 data( ReadMsgParam8LC( 3, aMessage )->Des() );
    #ifdef _DEBUG
        HBufC8* document = HBufC8::NewLC( doc.Length() );
        TPtr8 docDesc( document->Des() );
        docDesc.Copy( doc );
        CXcapCacheServer::WriteToLog( _L8( "  Document:    %S" ), &docDesc );
        CXcapCacheServer::WriteToLog( _L8( "  ETag:        %S" ), &etag );
        CXcapCacheServer::WriteToLog( _L8( "  Root URI:    %S" ), &root );
        CXcapCacheServer::WriteToLog( _L8( "  Data:        %d bytes" ), data.Length() );
        CleanupStack::PopAndDestroy();  //document
    #endif
    TInt tableIndex = KErrNotFound;
    TCacheEntryInfo header;
    header.iEtag = &etag;
    header.iRootUri = &root;
    header.iRespData = &data;
    header.iDocumentUri = &doc;
    header.iDataLength = data.Length();
    CXcapCacheIndexTableEntry* tableEntry = iCacheIndexAdmin.FindL( tableIndex, root, doc );
    CXcapCacheIndexEntry* entry = NULL;
    if( tableEntry != NULL )
        {
        entry = iCacheIndex.Entry( tableEntry->Index() );
        entry->UpdateEntryL( &header );
        }
    else
        {
        entry = CXcapCacheIndexEntry::NewL( &header );
        CleanupStack::PushL( entry );
        TInt index = iCacheIndex.UpdateIndexL( entry );
        CleanupStack::Pop();  //entry
        iCacheIndexAdmin.UpdateIndexTableL( index, &header );
        }
    CleanupStack::PopAndDestroy( 4 );  //data, etag, root, name
    FlushCacheDataL();
    }

// ----------------------------------------------------------
// CXcapCacheSession::FlushCacheDataL
// 
// ----------------------------------------------------------
//
void CXcapCacheSession::FlushCacheDataL()
    {
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheServer::FlushCacheDataL()" ) );
    #endif
    iCacheIndex.StoreCacheDataL();
    iCacheIndexAdmin.StoreIndexTableL();
    }

// ----------------------------------------------------------
// CXcapCacheSession::DeleteFromStorageL
// 
// ----------------------------------------------------------
//
TInt CXcapCacheSession::DeleteFromStorageL( const TDesC& aFileName )
    {
    #ifdef _DEBUG
        HBufC8* name = HBufC8::NewLC( aFileName.Length() );
        TPtr8 nameDesc( name->Des() );
        nameDesc.Copy( aFileName );
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheServer::DeleteFromStorageL() - Filename: %S" ), &nameDesc );
        CleanupStack::PopAndDestroy();  //name
    #endif
    TInt error = iFileManager->Delete( aFileName );
    return error;
    }
            
// ----------------------------------------------------------
// CXcapCacheSession::FlushCacheDataL
// 
// ----------------------------------------------------------
//
HBufC8* CXcapCacheSession::ReadFromStorageL( const TDesC& aFileName )
    {
    #ifdef _DEBUG
        HBufC8* name = HBufC8::NewLC( aFileName.Length() );
        TPtr8 nameDesc( name->Des() );
        nameDesc.Copy( aFileName );
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheServer::ReadFromStorageL() - Filename: %S" ), &nameDesc );
        CleanupStack::PopAndDestroy();  //name
    #endif
    RFile file;
    HBufC8* data = NULL;
    User::LeaveIfError( file.Open( CXcapCacheServer::FileSession(), aFileName, EFileRead ) );
    CleanupClosePushL( file );
    TInt size = 0;
    User::LeaveIfError( file.Size( size ) );
    data = HBufC8::NewL( size );
    TPtr8 pointer( data->Des() );
    file.Read( pointer );
    CleanupStack::PopAndDestroy();  //file
    return data;
    }






