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
* Description:   CXcapInsertion
*
*/




#ifndef __XCAPINSERTION__
#define __XCAPINSERTION__

#include <e32base.h>
#include "XcapHttpOperation.h"

//FORWARD DECLARATIONS
class CXcapHttpReqPut;
class CXcapDocumentNode;

//CLASS DECLARATION
NONSHARABLE_CLASS( CXcapInsertion ) : public CXcapHttpOperation
    {
    public:

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CXcapInsertion* NewL( CXcapDocument& aParentDoc,
                                     CXcapDocumentNode* aDocumentExtents,
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
        virtual ~CXcapInsertion();

    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CXcapInsertion( CXcapDocument& aParentDoc,
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
        virtual TInt FormatModRequestL( const CXdmDocumentNode* aDocumentNode );

    private:  //Data

        TXdmOperationType               iOperationType;
    };

#endif      //__XCAPINSERTION__
            
// End of File
