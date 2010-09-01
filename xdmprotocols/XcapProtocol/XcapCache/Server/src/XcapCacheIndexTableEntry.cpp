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
* Description: CXcapCacheIndexTableEntry  
*
*/




// INCLUDE FILES
#include "ServerDefines.h"
#include "XcapCacheServer.h"
#include "XcapCacheIndexTableEntry.h"

// ----------------------------------------------------------
// CXcapCacheIndexTableEntry::CXcapCacheIndexTableEntry
// 
// ----------------------------------------------------------
//
CXcapCacheIndexTableEntry::CXcapCacheIndexTableEntry() : iEntryIndex( KErrNotFound ) 
    {
    }

// ----------------------------------------------------------
// CXcapCacheIndexTableEntry::CXcapCacheIndexTableEntry
// 
// ----------------------------------------------------------
//
CXcapCacheIndexTableEntry::CXcapCacheIndexTableEntry( const TInt aEntryIndex ) :
                                                      iEntryIndex( aEntryIndex ) 
    {
    }

// ----------------------------------------------------------
// CXcapCacheIndexTableEntry::NewL
// 
// ----------------------------------------------------------
//
CXcapCacheIndexTableEntry* CXcapCacheIndexTableEntry::NewL( const TInt aEntryIndex,
                                                            const TDesC8& aRootUri,
                                                            const TDesC& aDocumentUri )
    {
    CXcapCacheIndexTableEntry* self = new( ELeave ) CXcapCacheIndexTableEntry( aEntryIndex );
    CleanupStack::PushL( self );
    self->ConstructL( aRootUri, aDocumentUri );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXcapCacheIndexTableEntry::NewL
// 
// ----------------------------------------------------------
//
CXcapCacheIndexTableEntry* CXcapCacheIndexTableEntry::NewL( const TInt aEntryIndex,
                                                            const TDesC8& aRootUri,
                                                            const TDesC8& aDocumentUri )
    {
    CXcapCacheIndexTableEntry* self = new( ELeave ) CXcapCacheIndexTableEntry( aEntryIndex );
    CleanupStack::PushL( self );
    self->ConstructL( aRootUri, aDocumentUri );
    CleanupStack::Pop();
    return self;
    }
    
// ----------------------------------------------------------
// CXcapCacheIndexTableEntry::NewL
// 
// ----------------------------------------------------------
//
CXcapCacheIndexTableEntry* CXcapCacheIndexTableEntry::NewL( const TDesC8& aRootUri,
                                                            const TDesC& aDocumentUri )
    {
    CXcapCacheIndexTableEntry* self = new( ELeave ) CXcapCacheIndexTableEntry();
    CleanupStack::PushL( self );
    self->ConstructL( aRootUri, aDocumentUri );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXcapCacheIndexTableEntry::~CXcapCacheIndexTableEntry
// 
// ----------------------------------------------------------
//
CXcapCacheIndexTableEntry::~CXcapCacheIndexTableEntry()
    {
    delete iRootUri;
    delete iDocumentUri;
    }

// ----------------------------------------------------------
// CXcapCacheIndexTableEntry::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndexTableEntry::ConstructL( const TDesC8& aRootUri,
                                            const TDesC& aDocumentUri )
    {
    #ifdef _DEBUG
        HBufC8* eightBuffer = HBufC8::NewLC( aDocumentUri.Length() );
        TPtr8 desc( eightBuffer->Des() );
        desc.Copy( aDocumentUri );
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheIndexTableEntry::ConstructL" ) );
        CXcapCacheServer::WriteToLog( _L8( "  Root:     %S" ), &aRootUri );
        CXcapCacheServer::WriteToLog( _L8( "  Document: %S" ), &desc );
        CleanupStack::PopAndDestroy();  //eightBuffer
    #endif
    iRootUri = aRootUri.AllocL();
    iDocumentUri = HBufC8::NewL( aDocumentUri.Length() );
    iDocumentUri->Des().Copy( aDocumentUri );
    }

// ----------------------------------------------------------
// CXcapCacheIndexTableEntry::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapCacheIndexTableEntry::ConstructL( const TDesC8& aRootUri,
                                            const TDesC8& aDocumentUri )
    {
    #ifdef _DEBUG
        HBufC8* eightBuffer = HBufC8::NewLC( aDocumentUri.Length() );
        TPtr8 desc( eightBuffer->Des() );
        desc.Copy( aDocumentUri );
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheIndexTableEntry::ConstructL" ) );
        CXcapCacheServer::WriteToLog( _L8( "  Root:     %S" ), &aRootUri );
        CXcapCacheServer::WriteToLog( _L8( "  Document: %S" ), &desc );
        CleanupStack::PopAndDestroy();  //eightBuffer
    #endif
    iRootUri = aRootUri.AllocL();
    iDocumentUri = HBufC8::NewL( aDocumentUri.Length() );
    iDocumentUri->Des().Copy( aDocumentUri );
    }
    
// ---------------------------------------------------------
// CXcapCacheIndexTableEntry::EntryData
// 
// ---------------------------------------------------------
//  
TPtrC8 CXcapCacheIndexTableEntry::RootUri() const
  {
  return iRootUri != NULL ? iRootUri->Des() : TPtrC8();
  }

// ---------------------------------------------------------
// CXcapCacheIndexTableEntry::EntryData
// 
// ---------------------------------------------------------
//  
TPtrC8 CXcapCacheIndexTableEntry::DocumentUri() const
  {
  return iDocumentUri != NULL ? iDocumentUri->Des() : TPtrC8();
  }

// ---------------------------------------------------------
// CXcapCacheIndexTableEntry::EntryData
// 
// ---------------------------------------------------------
//  
void CXcapCacheIndexTableEntry::Increase()
  {
  iEntryIndex++;
  }
  
// ---------------------------------------------------------
// CXcapCacheIndexTableEntry::EntryData
// 
// ---------------------------------------------------------
//  
void CXcapCacheIndexTableEntry::Decrease()
  {
  iEntryIndex--;
  }
  
// ---------------------------------------------------------
// CXcapCacheIndexTableEntry::operator++
// 
// ---------------------------------------------------------
//  
TBool CXcapCacheIndexTableEntry::operator==( const CXcapCacheIndexTableEntry& aEntry ) const
    {
    return ( iRootUri != NULL && iDocumentUri != NULL ) &&
           ( iRootUri->Des().CompareF( aEntry.RootUri() ) == 0  ) &&
           ( iDocumentUri->Des().CompareF( aEntry.DocumentUri() ) == 0 );
    }

// ---------------------------------------------------------
// CXcapCacheIndexTableEntry::operator++
// 
// ---------------------------------------------------------
//  
TInt CXcapCacheIndexTableEntry::Index() const
    {
    return iEntryIndex;
    }



