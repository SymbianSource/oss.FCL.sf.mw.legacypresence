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
* Description: CLocalRetrieval
*
*/




#ifndef __LOCALRETRIEVAL__
#define __LOCALRETRIEVAL__

#include <e32base.h>
#include "LocalOperationBase.h"

class CLocalDocumentNode;
   
//CLASS DECLARATION
NONSHARABLE_CLASS( CLocalRetrieval ) : public CLocalOperationBase
    {
    public:

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CLocalRetrieval* NewL( CLocalDocument& aParentDoc,
                                      CLocalDocumentNode* aTargetNode,
                                      CLocalOperationFactory& aOperationFactory );
        
        /**
        * Destructor.
        */      
        virtual ~CLocalRetrieval();

    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CLocalRetrieval( CLocalDocument& aTargetDoc,
                         CLocalDocumentNode* aDocumentSubset,
                         CLocalOperationFactory& aOperationFactory );
    
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
        
        CLocalDocumentNode*                 iDocumentSubset;
        TXdmOperationType                   iOperationType;
    };

#endif      //__LOCALRETRIEVAL__
            
// End of File
