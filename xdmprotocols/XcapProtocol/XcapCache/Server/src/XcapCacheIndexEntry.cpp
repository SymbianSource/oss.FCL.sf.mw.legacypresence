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
* Description:   CXcapCacheIndexEntry
*
*/




// INCLUDE FILES
#include "ServerDefines.h"
#include "XcapCacheServer.h"
#include "XcapCacheIndexEntry.h"
#include "XcapCacheEntryProperty.h"

// ----------------------------------------------------------
// CXcapCacheIndexEntry::CXcapCacheIndexEntry
// 
// ----------------------------------------------------------
//
CXcapCacheIndexEntry::CXcapCacheIndexEntry()
    {
    }

// ----------------------------------------------------------
// CXcapCacheIndexEntry::NewL
// 
// ----------------------------------------------------------
//
CXcapCacheIndexEntry* CXcapCacheIndexEntry::NewL( const TCacheEntryInfo* aHeader )
    {
    CXcapCacheIndexEntry* self = new( ELeave ) CXcapCacheIndexEntry();
    CleanupStack::PushL( self );
    self->ConstructL( aHeader );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXcapCacheIndexEntry::NewL
// 
// ----------------------------------------------------------
//
CXcapCacheIndexEntry* CXcapCacheIndexEntry::NewL( TPtr8& aEntryData )
    {
    CXcapCacheIndexEntry* self = new( ELeave ) CXcapCacheIndexEntry();
    CleanupStack::PushL( self );
    self->ConstructL( aEntryData );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXcapCacheIndexEntry::~CXcapCacheIndexEntry
// 
// ----------------------------------------------------------
//
CXcapCacheIndexEntry::~CXcapCacheIndexEntry()
    {
    iPropertyList.ResetAndDestroy();
    iPropertyList.Close();
    }

// ----------------------------------------------------------
// CXcapCacheIndexEntry::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndexEntry::ConstructL( TPtr8& aEntryData )
    {
    while( aEntryData.Length() > 0 )
        {
        CXcapCacheEntryProperty* prop = NULL;
        TInt index = aEntryData.FindF( KValueFieldSeparator );
        if( index > 0 )
            {
            TPtrC8 propData = aEntryData.Left( index );
            TInt separator = propData.FindF( KValuePropertySeparator );
            if( separator > 0 )
                {
                TPtrC8 name = propData.Left( separator );
                TPtrC8 value = propData.Right( propData.Length() - separator - 1 );
                const TInt nameId = FindPropNameId( name );
                __ASSERT_DEBUG( nameId != KErrNotFound, User::Leave( KErrCorrupt ) );
                prop = CreatePropertyL( nameId, value );
                CleanupStack::PushL( prop );
                User::LeaveIfError( iPropertyList.Append( prop ) );
                CleanupStack::Pop();  //prop
                }
            aEntryData.Delete( 0, index + 1 );
            }
        }
    }

// ----------------------------------------------------------
// CXcapCacheIndexEntry::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndexEntry::ConstructL( const TCacheEntryInfo* aHeader )
    {
    CreateFromHeaderL( aHeader );
    }
    
// ----------------------------------------------------------
// CXcapCacheIndexEntry::FindPropNameId
// 
// ----------------------------------------------------------
//
TInt CXcapCacheIndexEntry::FindPropNameId( const TDesC8& aNameString ) const
    {
    TBool found = EFalse;
    TInt ret = KErrNotFound;
    TInt arrayLength = sizeof( KCacheEntryArray ) /
                       sizeof( KCacheEntryArray[0] );
    for( TInt i = 0;!found && i < arrayLength;i++ )
        {
        if( aNameString.CompareF( TPtrC8( KCacheEntryArray[i] ) ) == 0 )
            {
            ret = i;
            found = ETrue;
            }
        }
    return ret;
    }

// ----------------------------------------------------------
// CXcapCacheIndexEntry::UpdateEntryL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndexEntry::UpdateEntryL( const TCacheEntryInfo* aHeader )
    {
    TTime access = LastAccess();
    DeleteFileL( FileNameL() );
    iPropertyList.ResetAndDestroy();
    CreateFromHeaderL( aHeader );
    }

// ----------------------------------------------------------
// CXcapCacheIndexEntry::DeleteFileL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndexEntry::DeleteFileL( const TDesC8& aFileName )
    {
    HBufC* buf = HBufC::NewLC( aFileName.Length() );
    buf->Des().Copy( aFileName );
    CFileMan* manager = CFileMan::NewL( CXcapCacheServer::FileSession() );
    CleanupStack::PushL( manager );
    TInt error = manager->Delete( buf->Des() );
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheServer::DeleteFileL() - Error: %d" ), error );
    #endif
    CleanupStack::PopAndDestroy( 2 );  //manager, buf
    }

// ----------------------------------------------------------
// CXcapCacheIndexEntry::CreatePropertyL
// 
// ----------------------------------------------------------
//
CXcapCacheEntryProperty* CXcapCacheIndexEntry::CreatePropertyL( const TInt aPropertyName,
                                                                const TDesC8& aPropertyValue )
    {
    return CXcapCacheEntryProperty::NewL( aPropertyName, aPropertyValue );
    }

// ---------------------------------------------------------
// CXcapCacheIndexTableEntry::FindProperty
// 
// ---------------------------------------------------------
//  
const CXcapCacheEntryProperty& CXcapCacheIndexEntry::FindProperty( const TInt aPropId ) const
    {
    TBool found = EFalse;
    TInt count = iPropertyList.Count();
    CXcapCacheEntryProperty* property = NULL;
    for( TInt i = 0;!found && i < count;i++ )
        {
        property = iPropertyList[i];
        if( property->Name() == aPropId )
            found = ETrue;
        }
    return *property;
    }

// ---------------------------------------------------------
// CXcapCacheIndexTableEntry::LastAccess
// 
// ---------------------------------------------------------
//  
TTime CXcapCacheIndexEntry::LastAccess() const
    {
    const CXcapCacheEntryProperty& property = FindProperty( KCacheLastAccess );
    return property.TimeValue();
    }

// ---------------------------------------------------------
// CXcapCacheIndexTableEntry::LastModified
// 
// ---------------------------------------------------------
//  
TTime CXcapCacheIndexEntry::LastModified() const
    {
    const CXcapCacheEntryProperty& property = FindProperty( KCacheLastModified );
    return property.TimeValue();
    }

// ---------------------------------------------------------
// CXcapCacheIndexTableEntry::XmlSize
// 
// ---------------------------------------------------------
//  
TInt CXcapCacheIndexEntry::XmlSize() const
    {
    const CXcapCacheEntryProperty& property = FindProperty( KCacheXmlSize );
    return property.IntValue();
    }

// ---------------------------------------------------------
// CXcapCacheIndexTableEntry::FileNameL
// 
// ---------------------------------------------------------
//  
TPtrC8 CXcapCacheIndexEntry::FileNameL() const
    {
    const CXcapCacheEntryProperty& property = FindProperty( KCacheXmlFile );
    return property.DesValue8();
    }

// ---------------------------------------------------------
// CXcapCacheIndexTableEntry::FileName16L
// 
// ---------------------------------------------------------
//
TPtrC CXcapCacheIndexEntry::FileName16L() const
	{
	const CXcapCacheEntryProperty& property = FindProperty( KCacheXmlFile );
	return property.DesValue16();
	}

// ---------------------------------------------------------
// CXcapCacheIndexTableEntry::ETag
// 
// ---------------------------------------------------------
//  
TPtrC8 CXcapCacheIndexEntry::ETag() const
    {
    const CXcapCacheEntryProperty& property = FindProperty( KCacheEntryETag );
    return property.DesValue8();
    }

// ----------------------------------------------------------
// CXcapCacheIndexEntry::CreateFromHeaderL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndexEntry::CreateFromHeaderL( const TCacheEntryInfo* aHeader )
    {
    TInt arrayLength = sizeof( KCacheEntryArray ) /
                       sizeof( KCacheEntryArray[0] );
    HBufC8* hour = HourStringLC();
    for( TInt i = 0;i < arrayLength;i++ )
        {
        CXcapCacheEntryProperty* prop = NULL;
        switch( i )
            {
            case KCacheEntryETag:
                prop = CreatePropertyL( KCacheEntryETag, *aHeader->iEtag );
                break;
            case KCacheXmlSize:
                {
                TBuf8<10> sizeBuf;
                sizeBuf.AppendNum( aHeader->iDataLength );
                prop = CreatePropertyL( KCacheXmlSize, sizeBuf );
                }
                break;
            case KCacheXmlFile:
                {
                HBufC8* fileName = StoreXmlFileLC( *aHeader->iRespData );
                prop = CreatePropertyL( KCacheXmlFile, fileName->Des() );
                CleanupStack::PopAndDestroy();  //fileName
                }
                break;
            case KCacheLastAccess:
                prop = CreatePropertyL( KCacheLastAccess, hour->Des() );
                break;
            case KCacheLastModified:
                prop = CreatePropertyL( KCacheLastModified, hour->Des() );
                break;
            default:
                break;
            }
        if( prop != NULL )
            User::LeaveIfError( iPropertyList.Append( prop ) );
        }
    CleanupStack::PopAndDestroy();  //hour
    }

// ----------------------------------------------------------
// CXcapCacheIndexEntry::HourStringLC
// 
// ----------------------------------------------------------
//
HBufC8* CXcapCacheIndexEntry::HourStringLC()
    {
    HBufC8* hourString = NULL;
    TPtrC date( CXcapCacheServer::Date() );
    TPtrC time( CXcapCacheServer::Time() );
    hourString = HBufC8::NewLC( date.Length() + time.Length() + 1 );
    TPtr8 pointer( hourString->Des() );
    pointer.Copy( date );
    pointer.Append( KPageFileSeparator );
    pointer.Append( time );
    return hourString;
    }

// ----------------------------------------------------------
// CXcapCacheIndexEntry::StoreXmlFileLC
// 
// ----------------------------------------------------------
//
HBufC8* CXcapCacheIndexEntry::StoreXmlFileLC( const TDesC8& aXmlData )
    {
    RFile file;
    HBufC* nameBuf = NULL;
    TPtrC randomName( _L( "" ) );
    TInt error = KErrAlreadyExists;
    //It is extremely unlikely that the random string generator
    //creates two exact same patterns, but let's check for it, anyway.
    while( error == KErrAlreadyExists )
        {
        randomName.Set( CXcapCacheServer::RandomString() );
        nameBuf = HBufC::NewLC( KCacheServerRoot().Length() + randomName.Length() );
        nameBuf->Des().Copy( KCacheServerRoot );
        nameBuf->Des().Append( randomName );
        error = file.Create( CXcapCacheServer::FileSession(),
                             nameBuf->Des(), EFileWrite );
        if( error != KErrNone )
            CleanupStack::PopAndDestroy();  //nameBuf
        }
    file.Write( aXmlData );
    file.Close();
    __ASSERT_ALWAYS( nameBuf != NULL, User::Leave( KErrCorrupt ) );
    HBufC8* ret = HBufC8::NewLC( nameBuf->Des().Length() );
    ret->Des().Copy( nameBuf->Des() );
    CleanupStack::Pop();  //ret
    CleanupStack::PopAndDestroy();  //nameBuf
    CleanupStack::PushL( ret );
    return ret;
    }

// ----------------------------------------------------------
// CXcapCacheIndexEntry::PrintLC
// 
// ----------------------------------------------------------
//
HBufC8* CXcapCacheIndexEntry::PrintLC() const
    {
    TInt position = 0;
    CBufFlat* buffer = NULL;
    HBufC8* entryString = NULL;
    TInt count = iPropertyList.Count();
    if( count > 0 )
        {
        buffer = CBufFlat::NewL( 50 );
        CleanupStack::PushL( buffer );
        for( TInt i = 0;i < count;i++ )
            {
            TPtrC8 name = TPtrC8( KCacheEntryArray[iPropertyList[i]->Name()] );
            TPtrC8 value = iPropertyList[i]->DesValue8();
            buffer->InsertL( position, name );
            position = position + name.Length();
            buffer->InsertL( position, KValuePropertySeparator );
            position = position + KValuePropertySeparator().Length();
            buffer->InsertL( position, value );
            position = position + value.Length();
            buffer->InsertL( position, KValueFieldSeparator );
            position = position + KValueFieldSeparator().Length();
            }
        buffer->InsertL( position, KIndexFileEndOfLine );
        TPtr8 pointer( buffer->Ptr( 0 ) );
        entryString = HBufC8::NewL( pointer.Length() );
        entryString->Des().Copy( pointer );
        CleanupStack::PopAndDestroy();  //buffer
        CleanupStack::PushL( entryString );
        }
    return entryString;
    }



