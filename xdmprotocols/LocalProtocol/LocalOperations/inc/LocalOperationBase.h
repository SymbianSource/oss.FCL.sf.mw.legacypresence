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
* Description:  CLocalOperationBase
*
*/




#ifndef __LOCALOPERATIONBASE__
#define __LOCALOPERATIONBASE__

#include <e32base.h>
#include "XdmOperation.h"

//FORWARD DECLARATIONS
class CXdmXmlParser;
class CLocalDocument;
class CXdmDocumentNode;
class CLocalDocumentNode;
class CLocalOperationFactory;

//CLASS DECLARATION
NONSHARABLE_CLASS( CLocalOperationBase ) : public CBase,
                                           public MXdmOperation
    {
    public:
        
        /**
        * Destructor.
        */      
        virtual ~CLocalOperationBase();

    protected:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CLocalOperationBase( const CLocalDocument& aTargetDoc,
                             CLocalOperationFactory& aOperationFactory );
        
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
        void WriteFileL( const CXdmDocumentNode* aRootNode );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        HBufC8* FetchXmlDataL( TInt aFileSize  );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */   
        void ReplaceDataFileL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */   
        void DeleteDataFileL();
                
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        TInt OpenDataFileL( const TDesC& aFilePath );
        
    private:
    
        /**
        * Execute this operation asynchronously.
        */      
        void ExecuteL( TRequestStatus& aStatus, TUint aOptions );
        
        /**
        * Cancel this operation.
        */      
        void CancelOperation();
        
        /**
        * Get the result of this operation.
        */      
        TInt Result() const;

        /**
        * Get the completion data of this operation.
        */      
        const TXdmCompletionData& CompletionData() const;
        
        /**
        * Set this operation to completed state.
        */      
        TBool IsCompleted() const;
        
        /**
        * Set this operation to completed state.
        */      
        CXdmDocumentNode* TargetNode() const;
        
        /**
        * Set this operation to completed state.
        */   
        void GenerateTimeStampL();
        
        HBufC* ConstructTimestampNameL();
        
        /**
        * Set this operation to completed state.
        */   
        HBufC8* TimeLC();
    
    protected:  //Data
        
        RFile                           iXmlFile;
        TPtrC                           iFullPath;
        CFileMan*                       iFileManager;
        CLocalDocument&                 iTargetDoc;
        CXdmXmlParser*                  iXmlParser;
        TXdmOperationType               iOperationType;
        TXdmCompletionData              iCompletionData;
        CLocalOperationFactory&         iOperationFactory;
    };

#endif      //__LOCALOPERATIONBASE__
            
// End of File
