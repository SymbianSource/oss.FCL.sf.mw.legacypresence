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




#ifndef __XCAPCACHESESSION_H__
#define __XCAPCACHESESSION_H__

// INCLUDES
#include <e32std.h>
#include <e32cons.h>
#include <s32file.h>
#include <e32base.h>
#include "XcapCacheServer.h"

class MDesC16Array;
class CXcapCacheIndex;
class CXcapCacheIndexAdmin;

// CLASS DECLARATION
NONSHARABLE_CLASS( CXcapCacheSession ) : public CSession2
    {
    public:  // Constructors and destructor
        
        /**
        * Symbian OS default constructor
        *
        * @param CXcapCacheServer Pointer to the server object
        * @return CXcapCacheSession* New session object
        */ 
        static CXcapCacheSession* NewL( CXcapCacheServer* aServer );

        /**
        * Service a request
        *
        * @param RMessage& Kernel message
        * @return void
        */ 
        virtual void ServiceL ( const RMessage2& aMessage );

        /**
        * Destructor.
        */      
        virtual ~CXcapCacheSession();

    private:
        
        /**
        * Second-phase constructor
        *
        * @return void
        */  
        void ConstructL();

        /**
        * C++ default constructor is private.
        *
        * @param CXcapCacheServer Pointer to the server object
        * @return CXcapCacheSession New session object
        */ 
        CXcapCacheSession( CXcapCacheServer* aServer );
        
        /**
        * Check the capabilities of the connecting client
        *
        * @param RMessage2& Kernel message
        * @return TBool Does the client have proper capabilities
        */
        TBool Capabilities( const RMessage2& aMessage );
        
        /**
        * Panic the client
        *
        * @param TInt Panic code
        * @return void
        */
        void PanicClient( TInt aPanic );
        
        /**
        * Dispatch the client's request
        *
        * @param RMessage2& Kernel message
        * @return void
        */ 
        void DispatchMessageL( const RMessage2& aMessage );
        
        /**
        * Fetch the meta information for a document
        *
        * @param RMessage2& Kernel message
        * @return void
        */ 
        void FetchDocumentInfoL( const RMessage2& aMessage );
        
        /**
        * Fetch the contents of a document
        *
        * @param RMessage2& Kernel message
        * @return void
        */  
        void FetchDocumentContentsL( const RMessage2& aMessage );
        
        /**
        * Store a document
        *
        * @param RMessage2& Kernel message
        * @return void
        */  
        void CacheXcapDataL( const RMessage2& aMessage );
        
        /**
        * Delete a document from the cache
        *
        * @param RMessage2& Kernel message
        * @return void
        */  
        void DeleteCacheDataL( const RMessage2& aMessage );
        
        /**
        * Check the currency of a document 
        *
        * @param RMessage2& Kernel message
        * @return void
        */  
        void CheckValidityL( const RMessage2& aMessage );
        
        /**
        * Externalize the meta information
        *
        * @return void
        */ 
        void FlushCacheDataL();
        
        /**
        * Delete unindexed documents from the cache
        *
        * @param MDesC16Array Indexed documents
        * @return void
        */
        void DeleteExcessL( const MDesC16Array& aIndexedEntries );
        
        /**
        * Check for unindexed entries in the cache
        *
        * @param TInt Total number of documents in the cache
        * @return void
        */
        void CheckUnindexedEntriesL( TInt aTotalCount );

        /**
        * Check whether the "aFileName" document is indexed
        *
        * @param MDesC16Array& The indexed entries
        * @param TDesC& Name of the dile to be checked
        * @return TBool Is the document indexed
        */
        TBool IsIndexed( const MDesC16Array& aIndexedEntries, const TDesC& aFileName );
        
        /**
        * Read the specified parameter from the kernel message
        *
        * @param TInt The parameter to be read
        * @param RMessage2& Kernel message
        * @return HBufC* The parameter data (16-bit)
        */
        HBufC* ReadMsgParam16LC( TInt aMsgIndex, const RMessage2& aMessage );
        
        /**
        * Read the specified parameter from the kernel message
        *
        * @param TInt The parameter to be read
        * @param RMessage2& Kernel message
        * @return HBufC8* The parameter data (8-bit)
        */
        HBufC8* ReadMsgParam8LC( TInt aMsgIndex, const RMessage2& aMessage );
        
        /**
        * Check the temporary buffer for data
        *
        * @return void
        */ 
        void CheckTempBuffer();
        
        /**
        * Delete a named document from the store
        *
        * @param TDesC& Name of the document to be deleted
        * @return TInt Error code
        */ 
        TInt DeleteFromStorageL( const TDesC& aFileName );
        
        /**
        * Read a named document from the store
        *
        * @param TDesC& Name of the document to be deleted
        * @return HBufC* The contents of the document
        */ 
        HBufC8* ReadFromStorageL( const TDesC& aFileName );

    private: //Data
        
        HBufC8*                                 iTempBuffer;
        CFileMan*                               iFileManager;
        CXcapCacheServer*                       iCacheServer;
        CXcapCacheIndex&                        iCacheIndex;
        CXcapCacheIndexAdmin&                   iCacheIndexAdmin;
    };


#endif

// End of File
