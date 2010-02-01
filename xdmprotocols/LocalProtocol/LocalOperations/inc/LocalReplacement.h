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
* Description:  CLocalReplacement
*
*/




#ifndef __LOCALREPLACEMENT__
#define __LOCALREPLACEMENT__

#include <e32base.h>
#include "XdmOperation.h"
#include "LocalOperationBase.h"

//FORWARD DECLARATIONS
class CLocalDocumentNode;

//CLASS DECLARATION
NONSHARABLE_CLASS( CLocalReplacement ) : public CLocalOperationBase
    {
    public:

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CLocalReplacement* NewL( CLocalDocument& aParentDoc,
                                        CLocalDocumentNode* aOldNode,
                                        CLocalDocumentNode* aNewNode,
                                        CLocalOperationFactory& aOperationFactory );
        
        /**
        * Destructor.
        */      
        virtual ~CLocalReplacement();

    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CLocalReplacement( CLocalDocument& aTargetDoc,
                           CLocalDocumentNode* aOldNode,
                           CLocalDocumentNode* aNewNode,
                           CLocalOperationFactory& aOperationFactory );
        
    private:        //From MXdmOperation
        
        /**
        * Execute this operation synchronously
        */      
        void ExecuteL();
        
        /**
        * Execute this operation synchronously
        */    
        void HandlePartialReplacementL( TInt aDataLength );
        
        /**
        * Set this operation to completed state.
        */      
        void Destroy();                

    private:  //Data

        CLocalDocumentNode*             iNewNode;
        CLocalDocumentNode*             iTargetNode;
        TXdmOperationType               iOperationType;
        
    };

#endif      //__LOCALREPLACEMENT__
            
// End of File
