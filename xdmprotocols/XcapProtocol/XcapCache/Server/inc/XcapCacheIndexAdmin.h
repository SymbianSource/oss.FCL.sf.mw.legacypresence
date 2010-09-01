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




#ifndef __XCAPCACHEINDEXADMIN_H__
#define __XCAPCACHEINDEXADMIN_H__

// INCLUDES
#include <e32std.h>
#include <s32file.h>
#include "CommonDefines.h"

//CONSTANTS

//FORWARD DECLARATIONs
class CXcapCacheIndexTableEntry;

// CLASS DECLARATION
NONSHARABLE_CLASS( CXcapCacheIndexAdmin ) : public CBase                         
    {
    public:             // Constructors and destructor
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        static CXcapCacheIndexAdmin* NewL();

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void UpdateIndexTableL( TInt aIndex, const TCacheEntryInfo* aStorageHeader );

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        CXcapCacheIndexTableEntry* FindL( TInt& aTableIndex,
                                          const TDesC8& aRootUri,
                                          const TDesC& aDocumentUri );

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TInt ReadIndexL( const CXcapCacheIndexTableEntry& aEntry );

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void RemoveTableIndex( TInt aIndex );

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void StoreIndexTableL();

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        ~CXcapCacheIndexAdmin();

    private:
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void ConstructL();

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        CXcapCacheIndexAdmin();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        RFile InitialisePageFileL();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void ReadEntriesL( TPtr8& aDataDesc );
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        CXcapCacheIndexTableEntry* ParseOneEntryL( const TDesC8& aEntryData );

    private:            // Data
        
        RPointerArray<CXcapCacheIndexTableEntry>        iEntryList;
    };

#endif

// End of File
