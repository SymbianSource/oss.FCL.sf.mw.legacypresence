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
* Description:   CXcapCacheIndexTableEntry
*
*/




#ifndef __XCAPCACHEINDEXTABLEENTRY_H__
#define __XCAPCACHEINDEXTABLEENTRY_H__

// INCLUDES
#include <e32std.h>
#include "CommonDefines.h"

//CONSTANTS

//FORWARD DECLARATIONs

// CLASS DECLARATION
NONSHARABLE_CLASS( CXcapCacheIndexTableEntry ) : public CBase                         
    {
    public:             // Constructors and destructor
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        static CXcapCacheIndexTableEntry* NewL( const TInt aEntryIndex,
                                                const TDesC8& aRootUri,
                                                const TDesC& aDocumentUri );
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        static CXcapCacheIndexTableEntry* NewL( const TInt aEntryIndex,
                                                const TDesC8& aRootUri,
                                                const TDesC8& aDocumentUri );
                                                
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        static CXcapCacheIndexTableEntry* NewL( const TDesC8& aRootUri,
                                                const TDesC& aDocumentUri );
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TPtrC8 RootUri() const;

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TPtrC8 DocumentUri() const;
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void Increase();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void Decrease();

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TBool operator==( const CXcapCacheIndexTableEntry& aEntry ) const;
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TInt Index() const;
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        ~CXcapCacheIndexTableEntry();

    private:
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */       
        CXcapCacheIndexTableEntry();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void ConstructL( const TDesC8& aRootUri,
                         const TDesC& aDocumentUri );
                         
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void ConstructL( const TDesC8& aRootUri,
                         const TDesC8& aDocumentUri );                         

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        CXcapCacheIndexTableEntry( const TInt aEntryIndex );

    private:            // Data
        
        TInt                                    iEntryIndex;
        HBufC8*                                 iRootUri;
        HBufC8*                                 iDocumentUri;
    };

#endif

// End of File
