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
* Description:   CXcapCacheIndex
*
*/




// INCLUDE FILES
#include "ServerDefines.h"
#include "XcapCacheIndex.h"
#include "XcapCacheServer.h"
#include "XcapCacheIndexEntry.h"

// ----------------------------------------------------------
// CXcapCacheIndex::CXcapCacheIndex
// 
// ----------------------------------------------------------
//
CXcapCacheIndex::CXcapCacheIndex()
    {
    }

// ----------------------------------------------------------
// CXcapCacheIndex::NewL
// 
// ----------------------------------------------------------
//
CXcapCacheIndex* CXcapCacheIndex::NewL()
    {
    CXcapCacheIndex* self = new( ELeave ) CXcapCacheIndex();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXcapCacheIndex::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndex::ConstructL()
    {
    TInt length = 0;   
    RFile index = InitialiseIndexL();                                    
    CleanupClosePushL( index );
    User::LeaveIfError( index.Size( length ) );
    if( length > 0 )
        {
        HBufC8* data = HBufC8::NewLC( length );
        TPtr8 pointer( data->Des() );
        User::LeaveIfError( index.Read( pointer ) );
        ReadEntriesL( pointer );
        CleanupStack::PopAndDestroy(); //data
        }
    CleanupStack::PopAndDestroy(); //index
    }

// ----------------------------------------------------------
// CXcapCacheIndex::InitialiseIndexL
// 
// ----------------------------------------------------------
//
RFile CXcapCacheIndex::InitialiseIndexL()
    {
    RFile ret;
    TUid process = RProcess().Identity();
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheIndex::InitialiseIndexL() - Process: %x" ), process ) ;
    #endif
    TInt error = ret.Open( CXcapCacheServer::FileSession(), KCacheServerIndex,
                           EFileShareExclusive | EFileRead | EFileWrite );
    if( error != KErrNone )
        {
        if( error == KErrPathNotFound )
            User::LeaveIfError( CXcapCacheServer::FileSession().MkDirAll( KCacheServerRoot ) );
        User::LeaveIfError( ret.Create( CXcapCacheServer::FileSession(), KCacheServerIndex,
                            EFileShareExclusive | EFileRead | EFileWrite ) );
        }
    return ret;
    }
    
// ----------------------------------------------------------
// CXcapCacheIndex::~CXcapCacheIndex
// 
// ----------------------------------------------------------
//
CXcapCacheIndex::~CXcapCacheIndex()
    {
    iEntryList.ResetAndDestroy();
    iEntryList.Close();
    }

// ----------------------------------------------------------
// CXcapCacheIndex::ConstructL
// 
// ----------------------------------------------------------
//
TInt CXcapCacheIndex::UpdateIndexL( const CXcapCacheIndexEntry* aEntry )
    {
    if( aEntry != NULL )
        {
        User::LeaveIfError( iEntryList.Append( aEntry ) );
        return iEntryList.Find( aEntry );
        }
    else return KErrNotFound;
    }

// ----------------------------------------------------------
// CXcapCacheIndex::Entry
// 
// ----------------------------------------------------------
//
CXcapCacheIndexEntry* CXcapCacheIndex::Entry( TInt aIndex ) const
    {
    TInt count = iEntryList.Count();
    CXcapCacheIndexEntry* entry = NULL;
    if( count > 0 && aIndex >= 0 && aIndex < count )
        entry = iEntryList[aIndex];
    return entry;
    }

// ----------------------------------------------------------
// CXcapCacheIndex::EntryCount
// 
// ----------------------------------------------------------
//
TInt CXcapCacheIndex::EntryCount() const
	{
	return iEntryList.Count();
	}

// ----------------------------------------------------------
// CXcapCacheIndex::Compare
// 
// ----------------------------------------------------------
//
TInt CXcapCacheIndex::Compare( const CXcapCacheIndexEntry& aFirst,
                               const CXcapCacheIndexEntry& aSecond )
    {
    if( aFirst.LastModified() == aSecond.LastModified() )
        return 0;
    else if( aFirst.LastModified() > aSecond.LastModified() )
        return 1;
    else return -1;
    }

// ----------------------------------------------------------
// CXcapCacheIndex::SortEntries
// 
// ----------------------------------------------------------
//
void CXcapCacheIndex::SortEntries()
    {
    TLinearOrder<CXcapCacheIndexEntry> order( CXcapCacheIndex::Compare );
    iEntryList.Sort( order );
    #ifdef _DEBUG
        TPtrC8 entryDesc;
        TInt count = iEntryList.Count();
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheIndex::SortEntries()" ) );
        CXcapCacheServer::WriteToLog( _L8( "------------------------------------------" ) );
        CXcapCacheServer::WriteToLog( _L8( " Sorted list:" ) );
        for( TInt i = 0;i < count;i++ )
            {
            TBuf<KDateTimeMaxSize> buffer =
                CXcapCacheServer::DateTime( iEntryList[i]->LastModified() );
            TBuf8<KDateTimeMaxSize> printBuffer( _L8( "  " ) );
            printBuffer.AppendFormat( _L8( "%d: " ), i );
            printBuffer.Append( buffer );
            CXcapCacheServer::WriteToLog( printBuffer );
            }
        CXcapCacheServer::WriteToLog( _L8( "------------------------------------------" ) );
    #endif
    }

// ----------------------------------------------------------
// CXcapCacheIndex::RemoveEntry
// 
// ----------------------------------------------------------
//
void CXcapCacheIndex::RemoveEntry( TInt aIndex )
    {
    TInt count = iEntryList.Count();
    if( count > 0 && aIndex >= 0 && aIndex < count )
        {
        CXcapCacheIndexEntry* entry = iEntryList[aIndex];
        iEntryList.Remove( aIndex );
        delete entry;
        entry = NULL;
        }
    }

// ----------------------------------------------------------
// CXcapCacheIndex::ReadEntriesL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndex::ReadEntriesL( TPtr8& aIndexData )
    {
    TBool finished = EFalse;
    while( !finished && aIndexData.Length() > 0 )
        {
        //If we find that there is still more data, but no \r\n
        //to denote the end of an entry, simply quit. The data
        //format has most probably been corrupted at some point.
        TInt entryIndex = aIndexData.FindF( KIndexFileEndOfLine );
        if( entryIndex > 0 )
            {
            TPtr8 entryData = CXcapCacheServer::DescriptorCast( 
                              aIndexData.Left( entryIndex ) );
            CXcapCacheIndexEntry* entry = CXcapCacheIndexEntry::NewL( entryData );
            CleanupStack::PushL( entry );
            User::LeaveIfError( iEntryList.Append( entry ) );
            CleanupStack::Pop();
            aIndexData.Delete( 0, entryIndex + 2 );
            }
        else finished = ETrue;
        }
    }

// ----------------------------------------------------------
// CXcapCacheIndex::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndex::StoreCacheDataL()
    {
    RFile index;
    TInt count = iEntryList.Count();
    if( count > 0 )
        {
        User::LeaveIfError( index.Replace( CXcapCacheServer::FileSession(),
                            KCacheServerIndex, EFileWrite ) );
        CleanupClosePushL( index );
        for( TInt i = 0;i < count;i++ )
            {
            CXcapCacheIndexEntry* entry = iEntryList[i];
            HBufC8* entryData = entry->PrintLC();
            if( entryData )
                {
                User::LeaveIfError( index.Write( entryData->Des() ) );
                CleanupStack::PopAndDestroy();  //entryData
                }
            }
        CleanupStack::PopAndDestroy();  //index
        }
    else
        {
        CFileMan* manager = CFileMan::NewL( CXcapCacheServer::FileSession() );
        manager->Delete( KCacheServerIndex );
        delete manager;
        manager = NULL;
        }
    }
        
