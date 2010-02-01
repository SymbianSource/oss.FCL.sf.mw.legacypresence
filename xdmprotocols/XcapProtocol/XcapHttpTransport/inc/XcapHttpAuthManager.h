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
* Description:   CXcapHttpAuthManager
*
*/




#ifndef __XCAPHTTPTAUTHMANAGER__
#define __XCAPHTTPTAUTHMANAGER__

// INCLUDES
#include <XdmCredentials.h>
#include <http/mhttpauthenticationcallback.h>
#include "XcapHttpConsts.h"

//FORWARD DECLARATION  
class CMD5;

//CLASS DECLARATION
class CXcapHttpAuthManager : public CBase

    {
    public:  // Constructors and destructor
        
        /**
        * Symbian OS default constructor.
        */     
        static CXcapHttpAuthManager* NewL( RHTTPSession& iHttpSession,
                                           CXcapHttpTransport& aTransportMain,
                                           const TXdmCredentials& aDigestCredentials );
        
        /**
        * Symbian OS default constructor.
        */   
        void SetAuthorized( TBool aAuthorized );
        
        /**
        * Symbian OS default constructor.
        */   
        TBool IsAuthorized() const;
        
        /**
        * Symbian OS default constructor.
        */                                      
        TBool ParseHeaderL( RHTTPTransaction& aTransaction, TInt aAuthType );
        
        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        HBufC8* AuthorizationL( CXcapHttpRequest& aHttpRequest );
        
        /**
        * Symbian OS default constructor.
        */                                      
        TBool ConsumeAuthInfoParamL( TAuthInfoParam aName, const TDesC8& aValue );
        
        /**
        * Destructor.
        */      
        virtual ~CXcapHttpAuthManager();
                               
    private:
        
        /**
        * C++ default constructor.
        */      
        CXcapHttpAuthManager( RHTTPSession& iHttpSession,
                              CXcapHttpTransport& aTransportMain );

        /**
        * Symbian OS default constructor.
        * @param aMimeType MIME type of the messages
        */     
        void ConstructL( const TXdmCredentials& aDigestCredentials );
        
        /**
        * Symbian OS default constructor.
        * @param aMimeType MIME type of the messages
        */   
        void Unquote( TPtr8& aParamValue );
        
        /**
        * Symbian OS default constructor.
        * @param aMimeType MIME type of the messages
        */    
        void ConstructHA1L( TDes8& aResult );
        
        /**
        * Symbian OS default constructor.
        * @param aMimeType MIME type of the messages
        */    
        void ConstructHA2L( TDes8& aResult, CXcapHttpRequest& aHttpRequest );
        
        /**
        * Symbian OS default constructor.
        * @param aMimeType MIME type of the messages
        */   
        void Hash( const TDesC8& aMessage, TDes8& aHash );
        
        /**
        * Symbian OS default constructor.
        * @param aMimeType MIME type of the messages
        */   
        HBufC8* RequestDigestLC( CXcapHttpRequest& aHttpRequest );
        
        /**
        * Symbian OS default constructor.
        * @param aMimeType MIME type of the messages
        */ 
        void SetNextNonceL( const TDesC8& aNextnonce );
        
        /**
        * Symbian OS default constructor.
        * @param aMimeType MIME type of the messages
        */   
        void GenerateClientNonce();
        
        /**
        * Symbian OS default constructor.
        * @param aMimeType MIME type of the messages
        */     
        void ParseQopL( const TDesC8& aQopString );

    private: //Data
        
        TInt                                        iNonceCount;
        TBool                                       iAuthorized;
        TBool                                       iUnauthReceived;
        TBool                                       iNextnonce;
        CMD5*                                       iMD5;
	    TInt64                                      iSeed;
	    HBufC8*                                     iOpaque;
        HBufC8*                                     iServerNonce;
        HBufC8*                                     iRealm;
        HBufC8*                                     iDomain;
        HBufC8*                                     iAlgorithm;
        HBufC8*                                     iQopString;
        TXcapAuthQop                                iQop;
        RHTTPSession                                iHttpSession;
        CXcapHttpTransport&                         iTransportMain;
        TBuf8<KXcapHashLength>                      iClientNonce;
        TBuf8<KXdmMaxUserNameLength>                iUserName;
        TBuf8<KXdmMaxPasswordLength>                iPassword;
        RStringPool                                 iStringPool;
    };

#endif

// End of File
