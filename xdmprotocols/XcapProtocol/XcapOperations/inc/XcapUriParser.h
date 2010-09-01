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
* Description:   CXcapUriParser
*
*/




#ifndef __XCAPURIPARSER__
#define __XCAPURIPARSER__

#include <e32base.h>
#include "XcapDocumentNode.h"
#include "XcapUriInterface.h"

//Constants
_LIT8( KXcapUriColon,                           ":" );
_LIT8( KXcapUriQMark,                           "?" );
_LIT8( KXcapUriXmlns,                           "xmlns(" );
_LIT8( KXcapUriEquals,                          "=\"" );
_LIT8( KXcapUriQuote,                           "\"" );
_LIT8( KXcapParenthClose,                       ")" );

class CXdmDocument;
class CXdmDocumentNode;
class CXcapOperationFactory;

//CLASS DECLARATION
NONSHARABLE_CLASS ( CXcapUriParser ) : public CBase, public MXcapUriInterface
    {
    public:

        /**
        * Destructor.
        */      
        static CXcapUriParser* NewL( CXdmDocument& aTargetDoc,
                                     CXcapOperationFactory& aOperationFactory );
        
        /**
        * Destructor.
        */      
        void SetDocumentSubset( const CXdmDocumentNode* aDocumentSubset );                          
        
        /**
        * Destructor.
        */      
        void ParseL( const TPtrC8& aHttpUri );
        
        /**
        * Destructor.
        */      
        TPtrC8 DesC8() const;
        
        /**
        * Destructor.
        */   
        TXdmElementType NodeType() const;
        
        /**
        * Destructor.
        */      
        virtual ~CXcapUriParser();
    
    private:  //From MXcapUriInterface
    
        /**
        * Add new namespace mapping
        * @param TDesC8& Namespace URI
        * @param TDesC8& Namespace prefix
        * @return void
        */      
        void AddNamespaceMappingL( const TDesC8& aUri, const TDesC8& aPrefix ); 

    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CXcapUriParser( CXdmDocument& aTargetDoc,
                        CXcapOperationFactory& aOperationFactory );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void ConstructL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXcapDocOperation*
        */   
        TInt CreateBasicNodeUriL( HBufC8* aNodePath );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXcapDocOperation*
        */   
        HBufC8* DocSubsetPathL();

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXcapDocOperation*
        */   
        void AppendPredicatesL( TInt& aPosition,
                                CBufBase* aBuffer,
                                const CXdmDocumentNode* aPathElement );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXcapDocOperation*
        */   
        HBufC8* FormatOnePathElementLC( const CXdmDocumentNode* aPathElement );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXcapDocOperation*
        */   
        void FormatAttributeUriL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXcapDocOperation*
        */   
        void FormatNodeUriL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXcapDocOperation*
        */   
        void AppendNamespaceMappingsL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXcapDocOperation*
        */     
        void FinaliseL();
        
    #ifdef _DEBUG
    
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXcapDocOperation*
        */   
        void DumpUriL( HBufC8* aNodePath ); 
    
    #endif

    private:  //Data
        
        TInt                                 iBufferPosition;
        TPtrC8*                              iHttpUri;
        HBufC8*                              iCompleteUri;
        CBufFlat*                            iUriBuffer;
        CXdmDocument&                        iTargetDoc;
        TXdmElementType                      iNodeType;
        CXdmDocumentNode*                    iDocumentSubset;
        CXcapOperationFactory&               iOperationFactory;
        RPointerArray<CXdmNamespace>         iNamespaceMappings;
    };

#endif      //__XCAPURIPARSER__
            
// End of File
