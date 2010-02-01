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
* Description:   CXcapReplacement
*
*/




#ifndef __XCAPREPLACEMENT__
#define __XCAPREPLACEMENT__

#include <e32base.h>
#include "XcapHttpOperation.h"

//FORWARD DECLARATIONS

class CXcapHttpReqPut;
class CXcapDocumentNode;

//CLASS DECLARATION
NONSHARABLE_CLASS( CXcapReplacement ) : public CXcapHttpOperation
    {
    public:

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CXcapReplacement* NewL( CXcapDocument& aParentDoc,
                                       CXcapDocumentNode* aOldNode,
                                       CXcapDocumentNode* aNewNode,
                                       CXcapOperationFactory& iOperationFactory );
        
        /**
        * Handle a successful operation
        */      
        virtual void OperationCompleteL();
        
        /**
        * Handle a failed operation
        */      
        virtual void OperationFailedL();
        
        /**
        * Destructor.
        */      
        virtual TBool Result() const;

        /**
        * Destructor.
        */      
        virtual ~CXcapReplacement();

    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CXcapReplacement( CXcapDocument& aParentDoc,
                          CXcapDocumentNode* aOldNode,
                          CXcapDocumentNode* aNewNode,
                          CXcapOperationFactory& iOperationFactory );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void ConstructL();
        
        /**
        * Destructor.
        */      
        void ExecuteL();
        
        /**
        * Destructor.
        */      
        virtual TInt FormatModRequestL( const CXcapDocumentNode* aDocumentNode );

    private:  //Data
    
        enum TXcapReplacementState
            {
            EHttpIdle = 0,
            EHttpDeleting,
            EHttpPutting
            };

        CXcapDocumentNode*              iNewNode;
        TXdmOperationType               iOperationType;
        TXcapReplacementState           iOperationPhase;
        
    };

#endif      //__XCAPREPLACEMENT__
            
// End of File
