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
* Description:   CXcapDeletion
*
*/




#ifndef __XCAPDELETION__
#define __XCAPDELETION__

#include <e32base.h>
#include "XcapHttpOperation.h"

//FORWARD DECLARATIONS
class CXcapDocumentNode;

//CLASS DECLARATION
NONSHARABLE_CLASS( CXcapDeletion ) : public CXcapHttpOperation
    {
    public:

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CXcapDeletion* NewL( CXcapDocument& aParentDoc,
                                    CXcapDocumentNode* aDocumentSubset,
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
        virtual ~CXcapDeletion();

    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CXcapDeletion( CXcapDocument& aParentDoc,
                       CXcapDocumentNode* aDocumentSubset,
                       CXcapOperationFactory& iOperationFactory );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void ConstructL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void ExecuteL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void HandleDocDeletion();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void HandlePartialDeletion();

    private:  //Data
    
        TXdmOperationType               iOperationType;
    };

#endif      //__XCAPDELETION__
            
// End of File
