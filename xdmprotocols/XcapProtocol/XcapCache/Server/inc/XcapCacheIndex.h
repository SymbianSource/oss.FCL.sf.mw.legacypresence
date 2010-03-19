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
* Description:  CXcapCacheIndex 
*
*/




#ifndef __XCAPCACHEINDEX_H__
#define __XCAPCACHEINDEX_H__

// INCLUDES
#include <s32file.h>
#include <e32std.h>
#include "CommonDefines.h"

//FORWARD DECLARATIONs
class CXcapCacheIndexEntry;

// CLASS DECLARATION
NONSHARABLE_CLASS( CXcapCacheIndex ) : public CBase                         
    {
    public:             // Constructors and destructor
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        static CXcapCacheIndex* NewL();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        static TInt Compare( const CXcapCacheIndexEntry& aFirst,
                             const CXcapCacheIndexEntry& aSecond );
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TInt UpdateIndexL( const CXcapCacheIndexEntry* aEntry );
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        CXcapCacheIndexEntry* Entry( TInt aIndex ) const;
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TInt EntryCount() const;
         
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void SortEntries();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void RemoveEntry( TInt aIndex );

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void StoreCacheDataL();

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        ~CXcapCacheIndex();

    private:
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        CXcapCacheIndex();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void ConstructL();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        RFile InitialiseIndexL();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void ReadEntriesL( TPtr8& aIndexData );

    private:            // Data
        
        RPointerArray<CXcapCacheIndexEntry>         iEntryList;
    };

#endif

// End of File
