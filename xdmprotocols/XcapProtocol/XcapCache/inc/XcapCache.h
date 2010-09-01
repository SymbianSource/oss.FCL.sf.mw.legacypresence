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
* Description:   RXcapCache
*
*/




#ifndef __XCAPCACHE_H__
#define __XCAPCACHE_H__

// INCLUDES
#include <e32std.h>
#include <f32file.h>

//Constants
#ifdef _DEBUG
    _LIT( KCacheClientLogDir,                       "XDM" );
    _LIT( KCacheClientLogFile,                      "CacheClient.txt" );
#endif

//FORWARD DECLARATIONS
class TCacheEntryInfo;
class CXcapCacheClient;

//CLASS DECLARATION
class RXcapCache : public RSessionBase
    {
    public:  // New functions
        
        /**
        * C++ Constructor.
        */
        IMPORT_C RXcapCache();
        
        /**
        * Return the version of this component
        *
        * @return TVersion The version number
        */
        IMPORT_C TVersion Version() const;
        
        /**
        * Connect to the server side session object
        *
        * @return TInt Error code
        */
        IMPORT_C TInt Connect();
        
        /**
        * Destructor
        */
        IMPORT_C ~RXcapCache();
        
        /**
        * Store a new document
        *
        * @param TDesC8& ETag of the new document
        * @param TDesC& Name of the document
        * @param TDesC8& Root location of the document
        * @param TDesC8& The document content
        * @return void
        */
        IMPORT_C void Store( const TDesC8& aETag, 
                             const TDesC& aDocumentName,
                             const TDesC8& aRootLocation,
                             const TDesC8& aResponseData );
        
        /**
        * Remove a named document from the cache
        *
        * @param TDesC& Name of the document
        * @param TDesC8& Root location of the document
        * @return TInt Error code
        */
        IMPORT_C TInt Delete( const TDesC& aDocumentName,
                              const TDesC8& aRootLocation );
        
        /**
        * Fetch information on a named document
        *
        * @param TDes8& Contains the ETag string on completion
        * @param TCacheEntryInfo& Encapsulates root & name 
        * @return TInt Error code
        */
        IMPORT_C TInt FetchDocumentInfo( TDes8& aETagDescriptor,
                                         const TDesC& aDocumentName,
                                         const TDesC8& aRootLocation,
                                         TCacheEntryInfo& aEntryInfo );
        
        /**
        * Fetch content for the named document
        *
        * @param TRequestStatus& Request status of the client
        * @param TDes8& Contains the document contents on completion
        * @param TCacheEntryInfo& Encapsulates root & name 
        * @return TInt Error code
        */                             
        IMPORT_C void FetchDocumentContent( TDes8& aContentDescriptor,
                                            const TDesC& aDocumentName,
                                            const TDesC8& aRootLocation );
        
        /**
        * Check validity of the named document
        *
        * @param TDesC8& ETag of the document
        * @param TDesC& Name of the document
        * @param TDesC8& Root location of the document
        * @return TInt Error code
        */                            
        IMPORT_C TInt IsCurrent( const TDesC8& aETag, 
                                 const TDesC& aDocumentName,
                                 const TDesC8& aRootLocation );
                                                                                                                            
        /**
        * Flush (externalise) cache metadata to disk
        *
        * @return void
        */
        IMPORT_C void Flush();
        
    public:   //Not exported
        
        /**
        * Start the server
        *
        * @return TInt Error code
        */
        TInt StartServer();
        
        /**
        * Check whether the server is already running
        *
        * @return TBool Is the server running
        */
        TBool IsRunning();


    private: //Data
        
       TBool                                            iConnected;
    };

#endif      //__XCAPCACHE_H__



