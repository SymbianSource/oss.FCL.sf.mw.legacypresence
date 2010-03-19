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
* Description:   CXcapCacheServer
*
*/




#ifndef __XCAPCACHESERVER_H__
#define __XCAPCACHESERVER_H__

// INCLUDES
#include <e32std.h>
#include <s32file.h>
#include "ServerDefines.h"
#include "CommonDefines.h"
#include "XcapShutdownTimer.h"

//CONSTANTS
_LIT( KCacheServerRoot,                             "C:\\private\\10207421\\" );
_LIT( KCacheServerIndex,                            "C:\\private\\10207421\\index.ch" );
_LIT( KCacheServerPageFile,                         "C:\\private\\10207421\\pagefile.ch" );
_LIT( KCacheServerIndexF,                        	"index.ch" );
_LIT( KCacheServerPageFileF,                     	"pagefile.ch" );

_LIT( KCacheServerLogDir,                           "XDM" );
_LIT( KCacheServerLogFile,                          "CacheServer.txt" );
const TUint KAppDefaultHeapSize                     = 0x10000;

//FORWARD DECLARATIONs
class CXcapCacheIndex;
class CXcapCacheIndexAdmin;

// CLASS DECLARATION
class CXcapCacheServer : public CServer2                         
    {
    public:             // Constructors and destructor
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        IMPORT_C static TInt StartThread();
        
        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        IMPORT_C static void WriteToLog( TRefByValue<const TDesC8> aFmt,... ); 

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        static CServer2* NewLC( void );

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        virtual CSession2* NewSessionL( const TVersion& aVersion,
        								const RMessage2& /*aMessage*/ ) const;

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        RFs* Session();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TInt CacheSize( TInt& aEntryCount );
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TInt MaxCacheSize() const;

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        ~CXcapCacheServer();

    public:  //static

        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        static RFs& FileSession();
        
        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        static CXcapCacheIndex* Index();

        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        static CXcapCacheIndexAdmin* IndexAdmin();
        
        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        static TInt ConvertDesc( const TDesC8& aNumberDesc );
        
        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        static TBuf<KDateMaxSize> Date();
        
        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        static TBuf<KDateMaxSize> Time();

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        static TBuf<KRandStringLength> RandomString();
        
        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        static TBuf<KDateTimeMaxSize> DateTime();
        
        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        static TBuf<KDateTimeMaxSize> DateTime( const TTime& aTime );
        
        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        static TPtr8 DescriptorCast( const TDesC8& aConstData );

    private:
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        CXcapCacheServer( TInt aPriority );
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void ConstructL();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void ReadMaxCacheSizeL();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void AddSession();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void DropSession();

    private:            // Data
        
        RFs                                     iFileSession;
        TInt                                    iMaxCacheSize;
        TUint                                   iSessionCount;
        CDir*                                   iCacheDirectory;
        CXcapShutdownTimer                      iShutdownTimer;
        friend class                            CXcapCacheSession;
    };

#endif

// End of File
