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
* Description:   CXcapRetrieval
*
*/




#ifndef __XCAPRETRIEVAL__
#define __XCAPRETRIEVAL__

#include <e32base.h>
#include "CommonDefines.h"
#include "XcapHttpOperation.h"

class CXcapDocumentNode;
   
//CLASS DECLARATION
NONSHARABLE_CLASS( CXcapRetrieval ) : public CXcapHttpOperation
    {
    public:

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CXcapRetrieval* NewL( CXcapDocument& aParentDoc,
                                     CXcapDocumentNode* aTargetNode,
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
        virtual TInt Result() const;

        /**
        * Destructor.
        */      
        virtual ~CXcapRetrieval();

    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CXcapRetrieval( CXcapDocument& aParentDoc,
                        CXcapDocumentNode* aTargetNode,
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
        void HandleCacheOperationL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void HandleNetworkOperationL();

    private:  //Data
        
        TBool                                iCacheOperation;
        TCacheEntryInfo                      iCacheEntryInfo;
        TXdmOperationType                    iOperationType;
    };

#endif      //__XCAPRETRIEVAL__
            
// End of File
