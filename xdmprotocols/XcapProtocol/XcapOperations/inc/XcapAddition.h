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
* Description:   CXcapAddition
*
*/




#ifndef __XCAPADDITION__
#define __XCAPADDITION__

#include <e32base.h>
#include "XcapHttpOperation.h"

//FORWARD DECLARATIONS
class CXcapHttpReqPut;
class CXcapDocumentNode;

//CLASS DECLARATION
NONSHARABLE_CLASS( CXcapAddition ) : public CXcapHttpOperation
    {
    public:

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CXcapAddition* NewL( CXcapDocument& aParentDoc,
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
        virtual ~CXcapAddition();

    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CXcapAddition( CXcapDocument& aParentDoc,
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
        TXdmOperationType DetermineOperationType();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */  
        void UpdatePartialToCacheL( RXcapCache* aCache, const TDesC8& aNewData );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */    
        virtual TInt FormatModRequestL( const CXdmDocumentNode* aDocumentNode );

    private:  //Data

        TXdmOperationType               iOperationType;
    };

#endif      //__XCAPPARTIALADDITION__
            
// End of File
