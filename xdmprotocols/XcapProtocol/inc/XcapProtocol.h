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
* Description:  CXcapProtocol
*
*/




#ifndef __XCAPPROTOCOL__
#define __XCAPPROTOCOL__

#include <e32base.h>
#include <XdmEngine.h>
#include <msgbearerobscallback.h>
#include "XcapCache.h"
#include "XdmProtocol.h"
#include "XdmShutdownSwitch.h"
#include "XcapUtilsInterface.h"

enum TXcapAuthType
    {
    EXcapAuthGaa = 0,
    EXcapAuthEarlyIms,
    EXcapAuthHttpDigest
    };
    
//CONSTANTS
_LIT( KHttp,                                    "http://" );
_LIT( KHttps,                                   "https://" );
_LIT( KHostSeparator,                           "/" );
_LIT( KXcapEngLogDir,                           "XDM" );
_LIT( KXcapEngLogFile,                          "XcapProtocol" );
const TInt KXcapEngLogBufferMaxSize		        = 2000;

//FORWARD DECLARATIONS
class CXdmLogWriter;
class CXcapDocument;
class CXdmXmlParser;
class CXcapEarlyIms;
class MMsgConnManager;
class CXdmShutdownTimer;
class CXcapHttpTransport;
class CXdmOperationFactory;
class CXdmSettingsCollection;

//CLASS DECLARATION
class CXcapProtocol : public CXdmProtocol,
                      public MXcapEarlyImsObs,
                      public MXdmShutdownSwitch,
                      public MMsgBearerObsCallback
    {
    
    public:
                
        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        IMPORT_C TPtrC8 PublicID();
        
        /**
        * Return a pointer to the cache client interface
        * @return RXcapCache* Pointer to the cache interface
        */
        IMPORT_C RXcapCache* Cache();
           
        /**
        * Return a reference to the XML Parser
        * @return CXdmXmlParser& Reference to XML Parser
        */
        IMPORT_C CXdmXmlParser& Parser();
        
        /**
        * Return a reference to the XML Parser
        * @return CXdmXmlParser& Reference to XML Parser
        */
        IMPORT_C CXcapHttpTransport& Transport();
                   
        /**
        * Check whether network is available
        * @return TBool Is network available
        */
        IMPORT_C TBool IsNetworkAvailable();
        
    public:     //Not exported
        
        /**
        * Check whether there are requests being carried out
        * and start the idle timer if appropriate
        *
        * @return void
        */ 
        void CheckActivity();
        
        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        TPtrC8 UserName() const;
        
        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        TXcapAuthType AuthType() const;
        
        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        TBool IsSimRequestPending( TInt& aError ) const;
        
        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        void AppendNotifyeeL( CXcapDocument* aPendingDoc );
        
        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        void CancelImsResolver();
        
        /**
        * Logging function
        * @param TRefByValue<const TDesC8> aFmt,...
        * @return void
        */ 
        void WriteToLog( TRefByValue<const TDesC8> aFmt,... ) const;
   
    private:  //From CXdmProtocol
   
        /**
        * Initialise the media used to transfer Xdm data
        */
        void InitTransferMedium( TInt aIdleTimeout, TRequestStatus& aStatus );
                                
        /**
        * Cancel initialisation of the transfer media.
        */
        void CancelTransferMediumInit();
        
        /**
        * Cancel initialisation of the transfer media.
        */
        TBool IsTransferAvailable() const;
        
        /**
        * Create a new document node.
        */
        CXdmDocumentNode* CreateDocumentNodeL();
        
        /**
        * Create a new protocol dependent document model
        */
        CXdmDocument* CreateDocumentL( const TDesC& aDocumentName,
                                       const TXdmDocType aDocumentType );
        
        /**
        * Create a new protocol dependent document model
        */
        CXdmDirectory* CreateDirectoryL( const TDesC& aDirectoryPath );

        /**
        * Destructor.
        */      
        virtual ~CXcapProtocol();
        
    private:
    
        /**
        * From MXdmShutdownSwitch
        * @return void
        */ 
        void SwitchOff();
        
		/**
        * From MXcapEarlyImsObs.
        * @param TInt Error
        * @return void
        */
		void RequestComplete( TInt aError );

    public:  //Static
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CXcapProtocol* NewL( const TXdmProtocolParams& aProtocolInfo );
        
        /**
        * A Global logging function
        * @param TRefByValue<const TDesC8> aFmt,...
        * @return void
        */ 
        static TInt GenerateUniqueIdL();

    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CXcapProtocol();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void ConstructL( const TXdmProtocolParams& aProtocolParams );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void DeleteLogFileL();
        
        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        TInt SaveSettingsL();
        
        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        void SaveFromCollectionL( const CXdmSettingsCollection& aSettings );
        
        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        void ResolveIdentityL();
        
        /**
        * Select the type of the authentication 
        */
        void SelectAuthTypeL( const CXdmSettingsCollection& aSettings );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        RXcapCache* CacheHandle();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */  
        TPtrC8 Impu() const;
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        HBufC* ParseRootLocationLC( const TBool aSecurity,
                                    const TDesC& aRootLocation );
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        HBufC* CheckProtocolPrefixL( const TDesC& aRootLocation );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        HBufC* ConstructSecureUriL( const TDesC& aBasicUri );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        HBufC* ConstructStandardUriL( const TDesC& aBasicUri );
        
        /**
        * Handle a bearer event
        * @param TMsgBearerEvent The type of the event
        */
        void HandleBearerEventL( TBool aAuthoritativeClose,
                                 TMsgBearerEvent aBearerEvent );

    private:  //Data
        
        TInt                                     iIdleTimeout;
        TInt                                     iAccessPoint;
        TInt                                     iImsResolverError;
        TBool                                    iSuspend;
        TBool                                    iCacheUsage;
        TBool                                    iTransferMediaOpen;
        TBool                                    iSimRequestPending;
        HBufC*                                   iRootUri;
        HBufC8*                                  iPublicId;
        RXcapCache                               iCacheClient;
        CXdmEngine*                              iXdmEngine;
        CXdmDocument*                            iCapabilities;
        CXdmLogWriter*                           iLogWriter;
        TXcapAuthType                            iAuthType;
        CXdmXmlParser*                           iXmlParser;
        CXcapEarlyIms*                           iImsResolver;
        TXdmCredentials                          iCredentials;
        MMsgConnManager*                         iConnectionManager;
        CXdmShutdownTimer*                       iIdleTimer;
        CXcapHttpTransport*                      iHttpTransport;
        const CXdmProtocolInfo*                  iProtocolInfo;
        TBuf8<KXdmMaxUserNameLength>             iUserName;
        RPointerArray<CXcapDocument>             iNotifyeeQueue;
    };

#endif      //__XCAPPROTOCOL__
            
// End of File
