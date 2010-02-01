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
* Description:  CLocalDeletion
*
*/




#ifndef __LOCALDELETION__
#define __LOCALDELETION__

#include <e32base.h>
#include "XdmOperation.h"
#include "LocalOperationBase.h"

//FORWARD DECLARATIONS
class CXdmDocumentNode;

//CLASS DECLARATION
NONSHARABLE_CLASS( CLocalDeletion ) : public CLocalOperationBase
    {
    public:

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CLocalDeletion* NewL( CLocalDocument& aParentDoc,
                                     CLocalDocumentNode* aDocumentSubset,
                                     CLocalOperationFactory& aOperationFactory );
       
        /**
        * Destructor.
        */      
        virtual ~CLocalDeletion();

    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CLocalDeletion( CLocalDocument& aTargetDoc,
                        CLocalDocumentNode* aDocumentSubset,
                        CLocalOperationFactory& aOperationFactory );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */    
        void HandlePartialDeletionL( TInt aDataLength );
    
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
    
        CLocalDocumentNode*             iDocumentSubset;
        TXdmOperationType               iOperationType;
    };

#endif      //__LOCALDELETION__
            
// End of File
