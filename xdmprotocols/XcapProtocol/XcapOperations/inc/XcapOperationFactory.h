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
* Description:  CXcapOperationFactory 
*
*/




#ifndef __XCAPOPERATIONFACTORY__
#define __XCAPOPERATIONFACTORY__

#include <e32base.h>
#include "XdmOperationFactory.h"

_LIT( KOperationsLogFile,                   "XcapOperations" );
const TInt KOpLogBufferMaxSize              = 2000;

//FORWARD DECLARATION
class CXdmDocument;
class CXdmDocumentNode;

//CLASS DECLARATION
NONSHARABLE_CLASS( CXcapOperationFactory ) : public CXdmOperationFactory
    {
    public:
        
        /**
        * Two-phased constructor.
        * @param CXdmProtocolInfo Protocol info
        * @return CXdmProtocol
        */
        static CXcapOperationFactory* NewL();
        
        /**
        * Destructor. A simple wrapper to the C++ destructor.
        *
        */
	    virtual ~CXcapOperationFactory();
	
	public:
        
        /**
        * Return an operation object that knows how to manipulate
        * whole documents
        */
        MXdmOperation* FetchL( CXdmDocument& aTargetDocument,
                               CXdmDocumentNode* aTargetNode = NULL ) const;
        
        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        MXdmOperation* InsertL( CXdmDocument& aTargetDocument,
                                CXdmDocumentNode* aTargetNode ) const;
        
        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        MXdmOperation* ReplaceL( CXdmDocument& aOldDocument, 
                                 CXdmDocumentNode* aNewNode,
                                 CXdmDocumentNode* aOldNode ) const;

        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        MXdmOperation* AppendL( CXdmDocument& aTargetDocument,
                                CXdmDocumentNode* aTargetNode ) const;

        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        MXdmOperation* DeletionL( CXdmDocument& aTargetDocument,
                                  CXdmDocumentNode* aTargetNode ) const;
    
    public:
        
        /**
        * Return an operation object that knows how to manipulate
        * partial document
        */
        void WriteToLog( TRefByValue<const TDesC8> aFmt,... ) const;
    
    private:
        
        /**
        * C++ constructor
        * @return CImpsTcpCirWatcher
        */      
        void ConstructL();
        
        /**
        * Write here something meaningful
        */  
        void WriteTraceLog( const TDesC8& aLogDescriptor );
        
        /**
        * C++ constructor
        * @return CImpsTcpCirWatcher
        */      
        CXcapOperationFactory();

    private:  //Data
    
        CXdmLogWriter*                        iLogWriter;
    };

#endif
