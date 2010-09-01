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
* Description: CLocalDirectoryEntry
*
*/




#include "LocalProtocol.h"
#include "LocalDocument.h"
#include "LocalDirectoryEntry.h"

// ----------------------------------------------------
// CLocalDirectoryEntry::CLocalDirectoryEntry
// 
// ----------------------------------------------------
//
CLocalDirectoryEntry::CLocalDirectoryEntry()
    {
    }

// ----------------------------------------------------
// CLocalDirectoryEntry::CLocalDirectoryEntry
// 
// ----------------------------------------------------
//
CLocalDirectoryEntry* CLocalDirectoryEntry::NewL( const TDesC& aFileName )
    {
    CLocalDirectoryEntry* self = new ( ELeave ) CLocalDirectoryEntry();
    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CLocalDirectoryEntry::ConstructL
// 
// ----------------------------------------------------
//
void CLocalDirectoryEntry::ConstructL( const TDesC& aFileName )
    {
    TChar comma = 46;
    TInt index = aFileName.LocateReverse( comma );
    TPtrC temp = aFileName.Left( index );
    iTimeStampEntryName = HBufC::NewL( temp.Length() );
    iTimeStampEntryName->Des().Copy( temp );
    }

// ----------------------------------------------------
// CLocalDirectoryEntry::EntryName
// 
// ----------------------------------------------------
//   
TPtrC CLocalDirectoryEntry::EntryName() const
    {
    return iXmlEntryName != NULL ? iXmlEntryName->Des() : TPtrC();
    }

// ----------------------------------------------------
// CLocalDirectoryEntry::CLocalDirectoryEntry
// 
// ----------------------------------------------------
//       
TBool CLocalDirectoryEntry::OfferEntryL( const TDesC& aEntryName )
    {
    TChar comma = 46;
    TInt index = aEntryName.LocateReverse( comma );
    TPtrC name( index > 0 ? aEntryName.Left( index ) : aEntryName );
    if( iTimeStampEntryName->Des().Compare( name ) == 0 )
        {
        iXmlEntryName = HBufC::NewL( aEntryName.Length() );
        iXmlEntryName->Des().Copy( aEntryName );
        return ETrue;
        }
    else return EFalse;
    }

// ----------------------------------------------------
// CLocalDirectoryEntry::CLocalDirectoryEntry
// 
// ----------------------------------------------------
//
CLocalDirectoryEntry::~CLocalDirectoryEntry()
    {
    delete iXmlEntryName;
    delete iTimeStampEntryName;
    }
             
// End of File

