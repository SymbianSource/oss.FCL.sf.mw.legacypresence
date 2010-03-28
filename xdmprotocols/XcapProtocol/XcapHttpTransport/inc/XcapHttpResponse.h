/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CXcapHttpResponse
*
*/




#ifndef __XCAPHTTPRESPONSE__
#define __XCAPHTTPRESPONSE__

// INCLUDES
#include <http.h>
#include "XcapHttpConsts.h"

//FORWARD DECLARATIONs
class CXcapHttpRequest;
class CXcapHttpAuthManager;

// CLASS DECLARATION
NONSHARABLE_CLASS( CXcapHttpResponse ) : public CBase,
                                         public MHTTPTransactionCallback
    {
    public:  // Constructors and destructor
        
        /**
        * A global logging function for 16 bit data.
        * @param aCommand command to be handled
        */
        static CXcapHttpResponse* NewL( CXcapHttpRequest* aHttpRequest,
                                        CXcapHttpTransport& aTransportMain );
        
        /**
        * Destructor.
        */      
        virtual ~CXcapHttpResponse();

    public:   //New functions   
        
        /**
        * Stores the address of the proxy currently in use
        * @param aProxy Address of the HTTP proxy server
        */
        TInt ContentLengthL() const;
       
    private:
    
        /**
        * A global logging function for 16 bit data.
        * @param aCommand command to be handled
        */
        CXcapHttpResponse( CXcapHttpRequest* aHttpRequest,
                           CXcapHttpTransport& aTransportMain );
        
        /**
        * A global logging function for 16 bit data.
        * @param aCommand command to be handled
        */
        void ConstructL();
   
        /**
        * Stores the address of the proxy currently in use
        * @param aProxy Address of the HTTP proxy server
        */
        RHTTPHeaders ResponseHeaderCollection() const;
        
        /**
        * Stores the address of the proxy currently in use
        * @param aProxy Address of the HTTP proxy server
        */
        RHTTPHeaders RequestHeaderCollection() const;
        
        /**
        * Stores the address of the proxy currently in use
        * @param aProxy Address of the HTTP proxy server
        */
        TBool HandleReceivedBodyDataL( const RHTTPTransaction aTransaction );
        
        /**
        * Stores the address of the proxy currently in use
        * @param aProxy Address of the HTTP proxy server
        */
        TBool IsUnauthRequest( RHTTPTransaction aTransaction, TInt& aAuthType );
        
        /**
        * Stores the address of the proxy currently in use
        * @param aProxy Address of the HTTP proxy server
        */
        TBool ParseAuthInfoParam( TInt& aLength, TAuthInfoParam& aName,
                                  TPtrC8& aValue, TPtr8& aParam );
        
        /**
        * Stores the address of the proxy currently in use
        * @param aProxy Address of the HTTP proxy server
        */
        TBool CheckAuthInfoHeaderL( RHTTPHeaders aHeaders );
        
    #ifdef _DEBUG
        
        /**
        * Convert undefined HTTP stack errors
        */
        void DumpHeadersL( RHTTPHeaders aHeaders, TInt aId );
            
    #endif
        
    private:  //From MHTTPTransactionCallback
    
        /**
        * Handles all responses
        * coming from the remote host
        * @param aTransaction The finished transaction
        * @param aEvent Type of the event
        */
        void MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent );
        
        /**
        * From MHTTPTransactionCallback, called when the RunL() of a transaction leaves
        * @param aInt The error the HTTP framework left with
        * @param aTransaction The failed transaction
        * @param aEvent The event that was being processed
        * @return KErrNone (HTTP framework panics if client returns any other error)
        */
        TInt MHFRunError( TInt aInt, RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

    private: //Data
        
        TBool                                    iAuthPending;
        CXcapHttpRequest*                        iHttpRequest;
        CXcapHttpTransport&                      iTransportMain;
        CXcapHttpAuthManager&                    iAuthManager;
        TInt                                     iUnauthRequestCounter;
    };


#endif

// End of File
