/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CXcapHttpRequest
*
*/




#ifndef __XCAPHTTPREQUEST__
#define __XCAPHTTPREQUEST__

// INCLUDES
#include <http.h>
#include "XdmOperation.h"
#include "XcapHttpRequestTimerCallback.h"

//CONSTANTS
//const TInt KMaxNumberOfRetries                    = 3;
const TInt KAuthTypeProxy                           = 407;
const TInt KAuthTypeNormal                          = 401;

_LIT8( KXcapRequestPost,                            "POST" );
_LIT8( KXcapRequestGet,                             "GET" );
_LIT8( KXcapRequestPut,                             "PUT" );
_LIT8( KXcapRequestDelete,                          "DELETE" );
		
_LIT8( KHttpHeaderAccept,                           "Accept" );
_LIT8( KHttpHeaderIfMatch,                          "If-Match" );
_LIT8( KHttpHeaderIfNoneMatch,                      "If-None-Match" );
_LIT8( KHttpHeaderContentType,                      "Content-Type" );
_LIT8( KHttpHeaderIntIdentity,                      "X-3GPP-Intended-Identity" );

//FORWARD DECLARATIONS
class TXdmCredentials;
class CXcapHttpResponse;
class CXcapHttpTransport;
class CXcapHttpHeaderModel;
class CXcapHttpAuthManager;
class CXcapHttpRequestTimer;

// CLASS DECLARATION
class CXcapHttpRequest : public CBase,
                         public MXcapHttpRequestTimerCallback

    {
    public:
        
        /**
        * C++ default constructor is private.
        */     
        IMPORT_C void SetHeaderL( const TDesC8& aHeaderName,
                                  const TDesC8& aHeaderValue );

        /**
        * Sets the expiry time of this transaction
        * @param aExpiryTime Time period after which this transaction expires
        * @param aCallback Handle to the receiver of the expiry event
        */
        IMPORT_C void SetExpiryTimeL( MXcapHttpRequestTimerCallback* aCallback,
                                      const TTimeIntervalMicroSeconds32 aExpiryTime );
        
        /**
        * Cancel this transaction
        */
        IMPORT_C void CancelRequest();
        
        /**
        * C++ default constructor is private.
        */     
        IMPORT_C void DispatchRequestL( TRequestStatus& aClientStatus );
        
        /**
        * Sets the expiry time of this transaction
        * @param aExpiryTime Time period after which this transaction expires
        * @param aCallback Handle to the receiver of the expiry event
        */
        IMPORT_C void ResetUriL( const TDesC& aNewUri );
        
        /**
        * Sets the expiry time of this transaction
        * @param aExpiryTime Time period after which this transaction expires
        * @param aCallback Handle to the receiver of the expiry event
        */
        IMPORT_C void UpdateRequestUriL( const TDesC8& aUpdatedUri );
        
        /**
        * Sets the expiry time of this transaction
        * @param aExpiryTime Time period after which this transaction expires
        * @param aCallback Handle to the receiver of the expiry event
        */
        IMPORT_C TPtrC8 RequestUriL() const;
        
        /**
        * Sets the expiry time of this transaction
        * @param aExpiryTime Time period after which this transaction expires
        * @param aCallback Handle to the receiver of the expiry event
        */
        IMPORT_C TPtrC8 HeaderValue( const HTTP::TStrings aHeaderName ) const;

        /**
        * C++ default constructor is private.
        */   
        IMPORT_C TXdmCompletionData* ResponseData();
        
        /**
        * C++ default constructor is private.
        */   
        IMPORT_C void ReleaseResponseData();
        
        /**
        * Destructor.
        */      
        IMPORT_C virtual ~CXcapHttpRequest();

    public:
        
        /**
        * Try to resend this request
        * @return TBool Resent or not
        */
        TPtrC8 RelativeUri() const;
        
        /**
        * Try to resend this request
        * @return TBool Resent or not
        */
        void ResendWithAuthL( TInt aAuthtype );
                
        /**
        * Has this transaction been cancelled.
        * @return TBool
        */
        CXcapHttpAuthManager& AuthManager();
        
        /**
        * C++ default constructor is private.
        */      
        RHTTPSession& Session();

        /**
        * Return the HTTP transaction associated with this transaction
        * @return RHTTPTransaction
        */
        RHTTPTransaction& Transaction();
        
        /**
        * C++ default constructor is private.
        */     
        void AppendDataL( const TPtrC8& aBodyPart );

        /**
        * Complete this request
        * @param aErrorCode The completion code
        */
        void FinaliseRequestL( TInt aErrorCode );


    protected:  // Constructors and destructor
        
        /**
        * C++ default constructor is private.
        */      
        CXcapHttpRequest( RHTTPSession& aHttpSession,
                          CXcapHttpAuthManager& aAuthManager,
                          CXcapHttpTransport& aTransportMain );

        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        void BaseConstructL( const TDesC& aRequestUri );

        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        virtual void ConstructRequestL();

        /**
        * Append data to the response buffer.
        */
        virtual RStringF ConstructMethodStringL() = 0;
        
        /**
        * Sets the status of this transaction
        * @param aSent Has this transaction been sent or not
        */
        void SetStatus( const TBool aSent );

        /**
        * Has this transaction been cancelled.
        * @return TBool
        */
        TBool IsCancelled();

        /**
        * Has this transaction been sent
        * @return TBool
        */
        TBool Status() const;

        /**
        * Return the time stamp of this transaction
        * @return TInt 
        */
        const TInt SendTime() const;

        /**
        * Get the current time (System tick count)
        */
        TInt TimeL() const;

    private:
        
        /**
        * Second-phase constructor.
        */ 
        void ConstructL( const TDesC8& aRequestBody );
        
        /**
        * Second-phase constructor.
        */ 
        void DoSetHeadersL();
        
        /**
        * Second-phase constructor.
        */ 
        void RemoveDuplicateAuth();
        
        /**
        * Append authorization header
        */ 
        void AppendAuthorizationHeaderL( RHTTPHeaders& aHeaderCollection, TInt aAuthType );
        
        /**
        * Second-phase constructor.
        */ 
        void SetRequestUriL( const TDesC& aRequestUri );
        
        /**
        * Second-phase constructor.
        */ 
        void PrepareResponseBodyL();
    
    #ifdef _DEBUG
       
        /**
        * Second-phase constructor.
        */ 
        void DumpResponseL( const TDesC8& aRespData, const TDesC& aDumpName );
        
    #endif
    
        /**
        * Second-phase constructor.
        */ 
        void CompileResponseDataL( TInt aErrorCode );
        
        /**
        * Second-phase constructor.
        */ 
        TInt ReinterpretErrorCode( const TInt aHttpStatus ) const;
        
        /**
        * Second-phase constructor.
        */ 
        virtual void HandleTimerEventL();
    
    protected: //Data
        
        RHTTPSession&                           iHttpSession;
        RHTTPTransaction                        iHttpTransaction;
        CXcapHttpTransport&                     iTransportMain;

    private: //Data
        
        HBufC8*                                 iFlatResponse;
        HBufC8*                                 iRequestUriBuf;
        HBufC8*                                 iWholeUri;
        CXcapHttpResponse*                      iHttpResponse;
        CBufSeg*                                iResponseBuffer;
        TRequestStatus*                         iClientStatus;
        CXcapHttpRequestTimer*                  iExpiryTimer;
        TUriParser8                             iRequestUri;
        TInt                                    iCurrentDataLength;
        TInt                                    iNumberOfRetries;
        TInt                                    iSendTime;
        TBool                                   iSent;
        TBool                                   iActive;
        TBool                                   iCancelled;
        TBool                                   iConstructed;
        TXdmCompletionData                      iRespData;
        CXcapHttpAuthManager&                   iAuthManager;
        TTimeIntervalMicroSeconds32             iExpiryTime;
        RPointerArray<CXcapHttpHeaderModel>     iHeaderCollection;
    };

#endif

// End of File
