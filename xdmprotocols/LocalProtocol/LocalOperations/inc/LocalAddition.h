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
* Description:   CLocalAddition
*
*/




#ifndef __LOCALADDITION__
#define __LOCALADDITION__

#include <e32base.h>
#include "XdmOperation.h"
#include "LocalOperationBase.h"

//FORWARD DECLARATIONS
class CXdmDocumentNode;

//CLASS DECLARATION
NONSHARABLE_CLASS( CLocalAddition ) : public CLocalOperationBase
    {
    public:

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CLocalAddition* NewL( CLocalDocument& aParentDoc,
                                     CLocalDocumentNode* aDocumentSubset,
                                     CLocalOperationFactory& aOperationFactory );
        
        /**
        * Destructor.
        */      
        virtual ~CLocalAddition();

    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CLocalAddition( CLocalDocument& aParentDoc,
                        CLocalDocumentNode* aDocumentSubset,
                        CLocalOperationFactory& aOperationFactory );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void HandlePartialUpdateL( TInt aDataLength );
    
    private:        //From MXdmOperation
        
        /**
        * Execute this operation synchronously
        */      
        void ExecuteL();
        
        /**
        * Set this operation to completed state.
        */      
        void Destroy();                

    private:  //Data
        

        TXdmOperationType               iOperationType;
        CXdmDocumentNode*               iDocumentSubset;
    };

#endif      //__LOCALADDITION__
            
// End of File
