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
* Description:  CXcapHttpTransport 
*
*/




#ifndef __XCAPHTTPTRANSPORT__
#define __XCAPHTTPTRANSPORT__

// INCLUDES
#include <http.h>
#include <XdmCredentials.h>

//COMMON CONSTANTS
_LIT( KTransportLogFile,                        "HttpTransport" );
const TInt KTransportLogBufMaxSize              = 2000;
_LIT( KTransportDateFormat,                     "%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S.%C%:3");
const TInt KMaxSubmitSize                       = 1024;
const TInt KMaxHeaderNameLen                    = 32;
const TInt KMaxHeaderValueLen                   = 128;
const TInt KBufferMaxSize                       = 2048;

//FORWARD DECLARATIONS
class CXdmLogWriter;
class MMsgConnManager;
class CXcapHttpReqGet;
class CXcapHttpReqPut;
class CXcapHttpReqDelete;
class CXcapHttpAuthManager;

// CLASS DECLARATION
class CXcapHttpTransport : public CBase

    {
    public:  // Constructors and destructor
        
        /**
        * Symbian OS default constructor.
        */     
        IMPORT_C static CXcapHttpTransport* NewL( const TDesC& aRootUri,
                                                  MMsgConnManager& aConnManager,
                                                  const TXdmCredentials& aDigestCredentials );
        
        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        IMPORT_C TPtrC8 RootUri();

        /**
        * Send one IMPS message.
        */
        IMPORT_C CXcapHttpReqGet* GetL( const TDesC& aUri );

        /**
        * Send one IMPS message.
        */
        IMPORT_C CXcapHttpReqPut* PutL( const TDesC& aUri );

        /**
        * Send one IMPS message.
        */
        IMPORT_C CXcapHttpReqDelete* DeleteL( const TDesC& aUri );
        
        /**
        * Stores the address of the proxy currently in use
        * @param aProxy Address of the HTTP proxy server
        */
        IMPORT_C void SetRootUriL( const TDesC& aRootUri );
        
        /**
        * Destructor.
        */      
        IMPORT_C virtual ~CXcapHttpTransport();

    public:   //New functions   
        
        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        static TXdmCredentials& Credentials();
    
    #ifdef _DEBUG
    
        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        void WriteToLog( TRefByValue<const TDesC8> aFmt,... ) const;
        
        /**
        * A global logging function for 16 bit data.
        * @param aCommand command to be handled
        */
        void WriteToLog( TRefByValue<const TDesC> aFmt,... ) const;
    
    #endif //_DEBUG
         
    private:
        
        /**
        * C++ default constructor.
        */      
        CXcapHttpTransport( MMsgConnManager& aConnManager );

        /**
        * Symbian OS default constructor.
        * @param aMimeType MIME type of the messages
        */     
        void ConstructL( const TDesC& aRootUri, const TXdmCredentials& aDigestCredentials );
                              
        /**
        * Symbian OS default constructor.
        * @param aMimeType MIME type of the messages
        */     
        HBufC8* ConstructRequestUriLC( const TDesC& aDocumentSelector );
        
        /**
        * Initialises an HTTP session
        */
        void InitialiseSessionL();
        
    private: //Data
        
        TBool                                       iCredValidity;
        TBool                                       iSessionClosed;
        HBufC8*                                     iRootUri;
        HBufC8*                                     iProxyBuffer;
        RHTTPSession                                iHttpSession;
        CXdmLogWriter*                              iLogWriter;
        MMsgConnManager&                            iConnManager;
        CXcapHttpAuthManager*                       iAuthManager;
    };

#endif

// End of File
