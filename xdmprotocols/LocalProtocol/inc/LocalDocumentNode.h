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
* Description: CLocalDocumentNode
*
*/




#ifndef __LOCALDOCUMENTNODE__
#define __LOCALDOCUMENTNODE__

#include "XdmNodeFactory.h"
#include "XdmDocumentNode.h"

class CLocalProtocol;
class CXdmNodeAttribute;

//CLASS DECLARATION
NONSHARABLE_CLASS( CLocalDocumentNode ) : public CXdmDocumentNode,
                                          public MXdmNodeFactory
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CLocalDocumentNode();

    public: // New functions
        
        /**
        * Two-phased constructor.
        *
        * @param TDesC8& Name of the new element 
		* @return CXdmDocumentNode* 
        */
        static CLocalDocumentNode* NewL( CXdmEngine& aXdmEngine,
                                         CLocalProtocol& aLocalProtocol );
        
        /**
        * Two-phased constructor.
        *
        * @param TDesC8& Name of the new element 
		* @return CXdmDocumentNode* 
        */
        static CLocalDocumentNode* NewL( CXdmEngine& aXdmEngine,
                                         const TDesC& aNodeName,
                                         CLocalProtocol& aLocalProtocol );
        
        /**
        * Two-phased constructor.
        *
        * @param TDesC8& Name of the new element 
		* @return CXdmDocumentNode* 
        */
        static CLocalDocumentNode* NewL( CXdmEngine& aXdmEngine,
                                         CLocalProtocol& aLocalProtocol,
                                         const CXdmDocumentNode& aAnotherNode,
                                         const CXdmDocumentNode& aParentNode );
        
        /**
        * Two-phased constructor.
		*
        * @param TDesC8& Name of the new element
        * @param CXdmDocumentNode* The parent of the new node
		* @return CXdmDocumentNode* 
        */
        static CLocalDocumentNode* NewL( CXdmEngine& aXdmEngine,
                                         const TDesC& aNodeName,
                                         CXdmDocumentNode* aParentNode,
                                         CLocalProtocol& aLocalProtocol );

        /**
        * Two-phased constructor.
		*
        * @param TBool Leaf node indicator
        * @param TDesC8& Name of the new element
        * @param CXdmDocumentNode* The parent of the new node
		* @return CXdmDocumentNode* 
        */
        static CLocalDocumentNode* NewL( const TBool aLeafNode,
                                         CXdmEngine& aXdmEngine,
                                         const TDesC& aNodeName,
                                         CXdmDocumentNode* aParentNode,
                                         CLocalProtocol& aLocalProtocol );
                                       
        /**
        * Two-phased constructor.
		*
        * @param TBool Leaf node indicator
        * @param TDesC8& Name of the new element
        * @param CXdmDocumentNode* The parent of the new node
		* @return CXdmDocumentNode* 
        */
        static CLocalDocumentNode* NewL( const TBool aLeafNode,
                                         CXdmEngine& aXdmEngine,
                                         CXdmDocumentNode* aParentNode,
                                         CLocalProtocol& aLocalProtocol );
        
        /**
        * Set the next node in the list
        * @param The next node in the list
		* @return void 
        */
        void SetParentNode( CLocalDocumentNode*& aParentNode );
        
    public:
    
        /**
        * Set the next node in the list
        * @param The next node in the list
		* @return void 
        */
        IMPORT_C void RemoveLocalL( CXdmDocumentNode* aChileNode );
        
        /**
        * Set the next node in the list
        * @param The next node in the list
		* @return void 
        */
        IMPORT_C void AppendLocalL( CXdmDocumentNode* aChileNode );
        
        /**
        * Set the next node in the list
        * @param The next node in the list
		* @return void 
        */
        IMPORT_C void ReplaceLocalL( CXdmDocumentNode* aNewNode,
                                     CXdmDocumentNode* aTargetNode );
        
    private:

        /**
        * C++ constructor, private version.
        */
        CLocalDocumentNode( CXdmEngine& aXdmEngine,
                            CLocalProtocol& aLocalProtocol );

        /**
        * C++ constructor, protected version.
        *
        * @param CXdmDocumentNode* The parent node of this element
        */
		CLocalDocumentNode( CXdmEngine& aXdmEngine,
		                    CXdmDocumentNode* aParentNode,
		                    CLocalProtocol& aLocalProtocol );
		
        /**
        * C++ constructor, another private version.
        */
		CLocalDocumentNode( const TBool aLeafNode,
		                    CXdmEngine& aXdmEngine,
		                    CXdmDocumentNode* aParentNode,
		                    CLocalProtocol& aLocalProtocol );
        
        /**
        * By default Symbian OS constructor is private.
        *
        * @param TDesC8& Name of this node
        * @return void
        */
		virtual void CopyConstructL( const CXdmDocumentNode& aAnotherNode,
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
        CXdmDocumentNode* ChileNodeL( const CXdmDocumentNode* aAnotherNode );
        
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
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        TPtrC8 Prefix() const;
        
        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        void SetPrefixL( const TDesC8& aNamespacePrefix );
        
    private:
        
        HBufC8*                                      iPrefix;
        CLocalProtocol&                              iLocalProtocol;
        CLocalDocumentNode*                          iNextNode;
    };

#endif      //__LOCALDOCUMENTNODE__
            
// End of File
