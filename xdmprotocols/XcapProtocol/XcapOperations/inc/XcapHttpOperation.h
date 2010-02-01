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
* Description:   CXcapHttpOperation
*
*/




#ifndef __XCAPHTTPOPERATION__
#define __XCAPHTTPOPERATION__

#include <e32base.h>
#include "XdmOperation.h"
#include "XcapOperationFactory.h"
#include "XcapHttpRequestTimerCallback.h"

//The default timout for HTTP requests in seconds
const TInt KDefaultHttpRequestTimeout           = 30;

//Set, if document should be fetched from cache when processing a doc subset
const TInt KFetchMasterFromCache                             = KXdmOption1;

//Set, if rules concerning document subsets should be applied when processing responses
const TInt KDocumentSubset                                   = KXdmOption2;

//Set, if the document an operation targets should not be cached (a directory document, for example)
const TInt KNoCache                                          = KXdmOption3;

//Set, if EARLY-IMS is in use => add the public ID in X-3GPP-Intended-Identity header
const TInt KUseIntendedIdentity                              = KXdmOption4;

//FORWARD DECLARATION
class RXcapCache;
class CXdmDocument;
class CXcapDocument;
class CXdmXmlParser;
class CXcapUriParser;
class CXcapHttpRequest;
class CXdmDocumentNode;
class CXcapDocumentNode;
class CXcapHttpTransport;

//Override application usage validation
//Must ONLY be used for testing, comment out before release
//#define                           __OVERRIDE_VALIDATION__

//CLASS DECLARATION
NONSHARABLE_CLASS( CXcapHttpOperation ) : public CActive,
                                          public MXdmOperation
                           
    {
    public:
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static void EscapeLC( CXdmDocumentNode& aRootNode );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static HBufC8* EscapeLC( const TDesC8& aDescriptor );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static HBufC8* Escape2LC( const TDesC& aDescriptor );
        
    public:
        
        /**
        * Destructor.
        */      
        void ExecuteL( TRequestStatus& aStatus, TUint aOptions );

        /**
        * Destructor.
        */      
        TInt Result() const;
        
        /**
        * Destructor.
        */      
        const TXdmCompletionData& CompletionData() const;
        
        /**
        * Destructor.
        */    
        TBool IsCompleted() const;
        
        /**
        * Set this operation to completed state.
        */      
        void Destroy();
        
        /**
        * Set this operation to completed state.
        */      
        virtual CXdmDocumentNode* TargetNode() const;
        
        /**
        * Destructor.
        */    
        CXcapHttpRequest* HttpRequest() const;

        /**
        * Destructor.
        */      
        virtual ~CXcapHttpOperation();

    protected:  //Methods

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CXcapHttpOperation( const CXdmDocument& aTargetDoc,
                            CXcapDocumentNode* aDocumentSubset,
                            CXcapOperationFactory& aOperationFactory );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void BaseConstructL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        TPtrC8 Descriptor( HBufC8* aBuffer );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        TInt ReinterpretStatus( const TInt aHttpStatus ) const;
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */ 
        CXcapHttpTransport& Transport() const;
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        virtual void OperationCompleteL() = 0;
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        virtual void OperationFailedL() = 0;
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        virtual void ExecuteL() = 0;
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        virtual void ExecuteL( CXcapHttpRequest* aHttpRequest,
                               CXdmDocumentNode* aDocumentNode );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */    
        virtual TInt FormatModRequestL( const CXdmDocumentNode* aDocumentNode );
        
    private:
    
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void RunL();

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void DoCancel();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */    
        TBool StartOperationL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void CancelOperation();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void HandleTimerEventL();

    protected:
        
        TInt                                 iResult;
        TInt                                 iCurrentIndex;
        TBool                                iCompleted;
        TBool                                iPartial;
        TUint                                iOptionFlags;
        CXdmXmlParser*                       iXmlParser;
        TXdmCompletionData                   iFailureData;
        TXdmCompletionData*                  iRequestData;
        TRequestStatus*                      iClientStatus;
        CXcapUriParser*                      iUriParser;
        CXcapHttpRequest*                    iActiveRequest;
        CXcapDocumentNode*                   iDocumentSubset;
        CXcapDocument&                       iTargetDoc;
        CXcapOperationFactory&               iOperationFactory;
        RPointerArray<CXcapHttpRequest>      iRequestQueue;

    private:  //Data
    };

#endif      //__XCAPHTTPOPERATION__
            
// End of File
