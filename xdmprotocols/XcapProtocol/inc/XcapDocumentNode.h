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
* Description: CXcapDocumentNode
*
*/




#ifndef __XCAPDOCUMENTNODE__
#define __XCAPDOCUMENTNODE__

#include "XdmNodeFactory.h"
#include "XdmDocumentNode.h"

class CXdmEngine;
class CXcapProtocol;
class CXcapProtocol;
class CXdmNodeAttribute;

//CLASS DECLARATION
NONSHARABLE_CLASS( CXcapDocumentNode ) : public CXdmDocumentNode, public MXdmNodeFactory
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CXcapDocumentNode();

    public: // New functions
        
        /**
        * Two-phased constructor.
        *
        * @param TDesC8& Name of the new element 
		* @return CXdmDocumentNode* 
        */
        static CXcapDocumentNode* NewL( CXdmEngine& aXdmEngine,
                                        CXcapProtocol& aXcapProtocol );
        
        /**
        * Two-phased constructor.
        *
        * @param TDesC8& Name of the new element 
		* @return CXdmDocumentNode* 
        */
        static CXcapDocumentNode* NewL( CXdmEngine& aXdmEngine,
                                        const TDesC& aNodeName,
                                        CXcapProtocol& aXcapProtocol );
        
        /**
        * Two-phased constructor.
        *
        * @param TDesC8& Name of the new element 
		* @return CXdmDocumentNode* 
        */
        static CXcapDocumentNode* NewL( CXdmEngine& aXdmEngine,
                                        CXcapProtocol& aXcapProtocol,
                                        const CXdmDocumentNode& aAnotherNode,
                                        const CXdmDocumentNode& aParentNode );
        
        /**
        * Two-phased constructor.
		*
        * @param TDesC8& Name of the new element
        * @param CXdmDocumentNode* The parent of the new node
		* @return CXdmDocumentNode* 
        */
        static CXcapDocumentNode* NewL( CXdmEngine& aXdmEngine,
                                        const TDesC& aNodeName,
                                        CXcapProtocol& aXcapProtocol,
                                        CXdmDocumentNode* aParentNode );

        /**
        * Two-phased constructor.
		*
        * @param TBool Leaf node indicator
        * @param TDesC8& Name of the new element
        * @param CXdmDocumentNode* The parent of the new node
		* @return CXdmDocumentNode* 
        */
        static CXcapDocumentNode* NewL( CXdmEngine& aXdmEngine, TBool aLeafNode,
                                        const TDesC& aNodeName, CXcapProtocol& aXcapProtocol,
                                        CXdmDocumentNode* aParentNode );
                                       
        /**
        * Two-phased constructor.
		*
        * @param TBool Leaf node indicator
        * @param TDesC8& Name of the new element
        * @param CXdmDocumentNode* The parent of the new node
		* @return CXdmDocumentNode* 
        */
        static CXcapDocumentNode* NewL( CXdmEngine& aXdmEngine,
                                        const TBool aLeafNode,
                                        CXcapProtocol& aXcapProtocol,
                                        CXdmDocumentNode* aParentNode );
        
        /**
        * Set the next node in the list
        * @param The next node in the list
		* @return void 
        */
        void RemoveNode( CXcapDocumentNode* aChileNode );
                                               
    private:

         /**
        * C++ constructor, private version.
        */
        CXcapDocumentNode( CXdmEngine& aXdmEngine,
                           CXcapProtocol& aXcapProtocol );

        /**
        * C++ constructor, protected version.
        *
        * @param CXdmDocumentNode* The parent node of this element
        */
		CXcapDocumentNode( CXdmEngine& aXdmEngine,
		                   CXcapProtocol& aXcapProtocol,
		                   CXdmDocumentNode* aParentNode );
		      
        /**
        * C++ constructor, another private version.
        */
		CXcapDocumentNode( const TBool aLeafNode,
		                   CXdmEngine& aXdmEngine,
		                   CXcapProtocol& aXcapProtocol,
                           CXdmDocumentNode* aParentNode );
		
		/**
        * By default Symbian OS constructor is private.
        *
        * @param TDesC8& Name of this node
        * @return void
        */
		void CopyConstructL( const CXdmDocumentNode& aAnotherNode,
		                     const CXdmDocumentNode& aParentNode );
         
    private:  //From MXdmNodeFactory
                     
        /**
        * Create a new child element
        *
        * @param TDesC8& Name of the new element
		* @return CXdmDocumentNode* 
        */
        CXdmDocumentNode* ChileNodeL();
        
        /**
        * Create a new child element
        *
        * @param TDesC8& Name of the new element
		* @return CXdmDocumentNode* 
        */
        CXdmDocumentNode* ChileNodeL( const TDesC& aChildName );
        
        /**
        * Create a new child element
        *
        * @param TDesC8& Name of the new element
		* @return CXdmDocumentNode* 
        */
        CXdmDocumentNode* ChileNodeL( const CXdmDocumentNode* aAnotherNode  );
        
        /**
        * Create a new attribute
        *
        * @param TDesC8& Name of the new attribute
		* @return CXdmNodeAttribute* 
        */
        CXdmNodeAttribute* AttributeL();
        
        /**
        * Create a new attribute
        *
        * @param TDesC8& Name of the new attribute
		* @return CXdmNodeAttribute* 
        */
        CXdmNodeAttribute* AttributeL( const TDesC& aAttributeName );
        
    private:  //From MXdmNodeInterface                          

        /**
        * Two-phased constructor.
		*
        * @param TDesC& Namespace prefix string
		* @return void 
        */
        void SetPrefixL( const TDesC8& aNamespace );
        
        /**
        * Returns the namespace string for this element
		*
		* @return TPtrC Namespace string of this element 
        */
        TPtrC8 Prefix() const;
    
    private:
        
        HBufC8*                                     iPrefix;
        CXcapProtocol&                              iXcapProtocol;
    };

#endif      //__XCAPDOCUMENTNODE__
            
// End of File
