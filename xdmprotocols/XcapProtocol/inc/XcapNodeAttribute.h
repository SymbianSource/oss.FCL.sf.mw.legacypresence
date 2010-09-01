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
* Description:  CXcapNodeAttribute
*
*/




#ifndef __XCAPNODEATTRIBUTE__
#define __XCAPNODEATTRIBUTE__

#include "XdmNodeAttribute.h"

//CLASS DECLARATION
NONSHARABLE_CLASS( CXcapNodeAttribute ) : public CXdmNodeAttribute
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param TDesC8& Name of this attribute
        * @param CXdmDocumentNode* Parent of this attribute
        * 
        * NOTE! "Parent", in this case, denotes the 
        *        element this attribute attributes.
        *
		* @return CXdmNodeAttribute* 
        */
        static CXcapNodeAttribute* NewL( CXdmEngine& aXdmEngine,
                                         const TDesC& aAttributeName,
                                         CXdmDocumentNode* aParentNode );
                                        
        /**
        * Two-phased constructor.
        *
        * @param CXdmDocumentNode* Parent of this attribute
        * 
        * NOTE! "Parent", in this case, denotes the 
        *        element this attribute attributes.
        *
		* @return CXdmNodeAttribute* 
        */
        static CXcapNodeAttribute* NewL( CXdmEngine& aXdmEngine,
                                         CXdmDocumentNode* aParentNode );                                        

        /**
        * Destructor.
        */
        virtual ~CXcapNodeAttribute();

    public: // New functions
    
        
                                               
    private:

        /**
        * C++ constructor, private version.
        */
        CXcapNodeAttribute( CXdmEngine& aXdmEngine );

        /**
        * C++ constructor, protected version.
        *
        * @param CXdmDocumentNode* The parent node of this element
        */
		CXcapNodeAttribute( CXdmEngine& aXdmEngine, CXdmDocumentNode* aParentNode );
        
        /**
        * By default Symbian OS constructor is private.
        *
        * @param TDesC8& Name of this node
        * @return void
        */
		void ConstructL( const TDesC& aNodeName );
        
        /**
        * C++ constructor, another private version.
        */
		CXcapNodeAttribute( const TBool aLeafNode,
		                    CXdmEngine& aXdmEngine,
                            CXdmDocumentNode* aParentNode );
        
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
        void SetPrefixL( const TDesC& aNamespacePrefix );
        
        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        void SetPrefixL( const TDesC8& aNamespacePrefix );                   
                            
         
    private:
    };

#endif      //__XCAPNODEATTRIBUTE__
            
// End of File
