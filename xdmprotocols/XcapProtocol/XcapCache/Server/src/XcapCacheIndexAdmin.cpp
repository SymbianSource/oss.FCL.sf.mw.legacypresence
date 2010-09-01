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
* Description:   CXcapCacheIndexAdmin
*
*/




// INCLUDE FILES
#include "ServerDefines.h"
#include "XcapCacheServer.h"
#include "XcapCacheIndexAdmin.h"
#include "XcapCacheIndexTableEntry.h"

// ----------------------------------------------------------
// CXcapCacheIndexAdmin::CXcapCacheIndexAdmin
// 
// ----------------------------------------------------------
//
CXcapCacheIndexAdmin::CXcapCacheIndexAdmin()
    {
    }

// ----------------------------------------------------------
// CXcapCacheIndexAdmin::NewL
// 
// ----------------------------------------------------------
//
CXcapCacheIndexAdmin* CXcapCacheIndexAdmin::NewL()
    {
    CXcapCacheIndexAdmin* self = new( ELeave ) CXcapCacheIndexAdmin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXcapCacheIndexAdmin::~CXcapCacheIndexAdmin
// 
// ----------------------------------------------------------
//
CXcapCacheIndexAdmin::~CXcapCacheIndexAdmin()
    {
    iEntryList.ResetAndDestroy();
    iEntryList.Close();
    }

// ----------------------------------------------------------
// CXcapCacheIndexAdmin::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndexAdmin::ConstructL()
    {
    TInt length = 0;
    RFile indexTable = InitialisePageFileL();                                         
    CleanupClosePushL( indexTable );
    User::LeaveIfError( indexTable.Size( length ) );
    if( length > 0 )
        {
        HBufC8* data = HBufC8::NewLC( length );
        TPtr8 pointer( data->Des() );
        User::LeaveIfError( indexTable.Read( pointer ) );
        ReadEntriesL( pointer );
        CleanupStack::PopAndDestroy(); //data
        }
    CleanupStack::PopAndDestroy(); //index
    }

// ----------------------------------------------------------
// CXcapCacheIndex::InitialisePageFileL
// 
// ----------------------------------------------------------
//
RFile CXcapCacheIndexAdmin::InitialisePageFileL()
    {
    RFile ret;
    TInt error = ret.Open( CXcapCacheServer::FileSession(), KCacheServerPageFile,
                           EFileShareExclusive | EFileRead | EFileWrite );
    //No need to check the path at this time, because needed
    //directories have already been created by the CacheIndex
    //object if they didn't exist before the server was launched                           
    if( error != KErrNone )
        User::LeaveIfError( ret.Create( CXcapCacheServer::FileSession(), KCacheServerPageFile,
                                        EFileShareExclusive | EFileRead | EFileWrite ) );                           
    return ret;
    }
    
// ----------------------------------------------------------
// CXcapCacheIndexAdmin::UpdateIndexTableL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndexAdmin::UpdateIndexTableL( TInt aIndex, const TCacheEntryInfo* aHeader )
    {
    CXcapCacheIndexTableEntry* entry =
        CXcapCacheIndexTableEntry::NewL( aIndex, *aHeader->iRootUri,
                                         *aHeader->iDocumentUri );
    CleanupStack::PushL( entry );
    User::LeaveIfError( iEntryList.Append( entry ) );
    CleanupStack::Pop();  //entry
    }

// ----------------------------------------------------------
// CXcapCacheSession::ReadIndexL
// 
// ----------------------------------------------------------
//
TInt CXcapCacheIndexAdmin::ReadIndexL( const CXcapCacheIndexTableEntry& aEntry )
    {
    TBool found = EFalse;
    TInt ret = KErrNotFound;
    TInt count = iEntryList.Count();
    CXcapCacheIndexTableEntry* entry = NULL;
    for( TInt i = 0;!found && i < count;i++ )
        {
        entry = iEntryList[i];
        if( *entry == aEntry )
            { 
            ret = entry->Index();
            found = ETrue;
            }
        }
    return ret;
    }

// ----------------------------------------------------------
// CXcapCacheSession::FindL
// 
// ----------------------------------------------------------
//
CXcapCacheIndexTableEntry* CXcapCacheIndexAdmin::FindL( TInt& aTableIndex,
                                                        const TDesC8& aRootUri,
                                                        const TDesC& aDocumentUri )
    {
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheIndexAdmin::FindL()" ) );
    #endif
    TBool found = EFalse;
    TInt count = iEntryList.Count();
    CXcapCacheIndexTableEntry* entry = NULL;
    if( count > 0 )
        {
        CXcapCacheIndexTableEntry* newEntry = CXcapCacheIndexTableEntry::NewL(
                                              aRootUri, aDocumentUri );
        for( TInt i = 0;!found && i < count;i++ )
            {
            CXcapCacheIndexTableEntry* listEntry = iEntryList[i];
            if( *newEntry == *listEntry )
                {
                aTableIndex = i;
                entry = iEntryList[i];
                found = ETrue;
                }
            }
        delete newEntry;
        newEntry = NULL;
        }
    return entry;
    }

// ----------------------------------------------------------
// CXcapCacheSession::RemoveTableIndexL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndexAdmin::RemoveTableIndex( TInt aIndex )
    {
    TInt count = iEntryList.Count();
    if( count > 0 && aIndex >= 0 && aIndex < count )
        {
        CXcapCacheIndexTableEntry* entry = iEntryList[aIndex];
        iEntryList.Remove( aIndex );
        delete entry;
        entry = NULL;
        for( TInt i = aIndex;i < count - 1;i++ )
            {
            CXcapCacheIndexTableEntry* z = iEntryList[i];
            z->Decrease();
            }   
        }
    }

// ----------------------------------------------------------
// CXcapCacheSession::StoreIndexTableL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndexAdmin::StoreIndexTableL()
    {
    RFile indexTable;
    TInt count = iEntryList.Count();
    if( count > 0 )
        {
        TBuf8<7> numBuf;
        User::LeaveIfError( indexTable.Replace( CXcapCacheServer::FileSession(),
                                                KCacheServerPageFile, EFileWrite ) );
        CleanupClosePushL( indexTable );
        for( TInt i = 0;i < count;i++ )
            {
            numBuf.AppendNum( iEntryList[i]->Index() );
            indexTable.Write( iEntryList[i]->RootUri() );
            indexTable.Write( KValueFieldSeparator );
            indexTable.Write( iEntryList[i]->DocumentUri() );
            indexTable.Write( KPageFileSeparator );
            indexTable.Write( numBuf );
            indexTable.Write( KIndexFileEndOfLine );
            numBuf.Zero();
            }
        CleanupStack::PopAndDestroy();  //indexTable
        }
    else
        {
        CFileMan* manager = CFileMan::NewL( CXcapCacheServer::FileSession() );
        manager->Delete( KCacheServerPageFile );
        delete manager;
        manager = NULL;
        }
    }

// ----------------------------------------------------------
// CXcapCacheSession::UpdatePageFileL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndexAdmin::ReadEntriesL( TPtr8& aDataDesc )
    {
    TBool finished = EFalse;
    while( !finished && aDataDesc.Length() > 0 )
        {
        //If we find that there is still more data, but no \r\n
        //to denote the end of an entry, simply quit. The data
        //format has most probably been corrupted at some point.
        TInt entryIndex = aDataDesc.FindF( KIndexFileEndOfLine );
        if( entryIndex > 0 )
            {
            TPtrC8 one = aDataDesc.Left( entryIndex );
            CXcapCacheIndexTableEntry* entry = ParseOneEntryL( one );
            CleanupStack::PushL( entry );
            User::LeaveIfError( iEntryList.Append( entry ) );
            CleanupStack::Pop();
            aDataDesc.Delete( 0, entryIndex + 2 );
            }
        else finished = ETrue;
        }
    }

// ----------------------------------------------------------
// CXcapCacheSession::ParseOneEntryL
// 
// ----------------------------------------------------------
//
CXcapCacheIndexTableEntry* CXcapCacheIndexAdmin::ParseOneEntryL( const TDesC8& aEntryData )
    {
    CXcapCacheIndexTableEntry* ret = NULL;
    TInt rootIndex = aEntryData.FindF( KValueFieldSeparator );
    if( rootIndex > 0 )
        {
        TPtrC8 root( aEntryData.Left( rootIndex ) );
        TInt docIndex = aEntryData.FindF( KPageFileSeparator );
        TPtrC8 doc( aEntryData.Mid( rootIndex + 1, docIndex - ( rootIndex + 1 ) ) );
        TPtrC8 pos( aEntryData.Mid( docIndex + 1, aEntryData.Length() - ( docIndex + 1 ) ) );
        TInt index( CXcapCacheServer::ConvertDesc( pos ) );
        ret = CXcapCacheIndexTableEntry::NewL( index, root, doc );
        }
    return ret;
    }




