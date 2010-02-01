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
* Description:   CXdmXmlContentHandler
*
*/




#ifndef __XDMXMLCONTENTHANDLER__
#define __XDMXMLCONTENTHANDLER__

//INCLUDES
#include <contenthandler.h>
#include "XmlNodePathInterface.h"

using namespace Xml;

class CXdmDocument;
class CXdmXmlParser;
class MXdmNodeInterface;
class CXmlParserNodePath;

//CLASS DECLARATION
class CXdmXmlContentHandler : public CBase,
                              public MContentHandler,
                              public MXmlNodePathInterface
    {
    public:  // Constructors and destructor
        
        /**
		* Symbian OS Constructor
		* @param CXdmXmlParser Reference to the main parser class
		* @return CXdmXmlContentHandler A new content handler
		*/
        static CXdmXmlContentHandler* NewL( CXdmXmlParser& aParserMain );
        
        /**
        * Destructor.
        */
        virtual ~CXdmXmlContentHandler();

    public: // New functions
		
	    /**
		* Reset content handler
		* @return void
		*/
		void Reset();
		
		/**
		* Set the target document
		* @param CXdmDocument& Reference to a document model
		* @return void
		*/
		void SetTarget( const CXdmDocument& aTargetDocument );
		
		/**
		* Set the target element
		* @param CXdmDocumentNode Referene to document/fragment root
		* @return void
		*/
		void SetTarget( const CXdmDocumentNode& aDocumentRoot );
		
	    /**
		* Set the target document and element
		* @param CXdmDocument Reference to a document model
		* @param CXdmDocumentNode Referene to document/fragment root
		* @return void
		*/
		void SetTargetL( const CXdmDocument& aTargetDocument,
		                 const CXdmDocumentNode& aTargetNode );
		                
    public: //From MXmlNodePathInterface
		
		/**
		* Return the target attribute
		* @return CXdmNodeAttribute* Target attribute
		*/
		CXdmNodeAttribute* TargetAttribute() const;

    private:
        
        /**
        * C++ Constructor
        * @param CXdmXmlParser Reference to the main parser class
        * @return CXdmXmlContentHandler
        */
		CXdmXmlContentHandler( CXdmXmlParser& aParserMain );
		
		/**
        * Symbian OS second-phase constructor
        */
		void ConstructL();
		
    private:  //From Xml::MContentHandler
        
        /**
        * Get extended interface
        * @param TInt32 UID of the interface
        * @return TAny* a general pointer to the interface
        */
        TAny* GetExtendedInterface( const TInt32 aUid );
        
		/**
        * Called on content: <elem>content</elem>
        * @param TDesC& Content bytes
        * @param TInt Error code
        * @return void
        */
		void OnContentL( const TDesC8 &aBytes,
		                 TInt aErrorCode );
		
		/**
        * Called on document end
        * @param TInt Error code
        * @return void
        */
        void OnEndDocumentL( TInt aErrorCode );
        
        /**
        * Called on element end
        * @param RTagInfo& Tag info
        * @param TInt Error code
        * @return void
        */
        void OnEndElementL( const RTagInfo& aElement,
                            TInt aErrorCode );
        
        /**
        * Called on prefix mapping end
        * @param RString& Prefix
        * @param TInt Error code
        * @return void
        */
        void OnEndPrefixMappingL( const RString &aPrefix,
                                  TInt aErrorCode );
        
        /**
        * Called on error
        * @param TInt Error code
        * @return void
        */
        void OnError( TInt aErrorCode );
    
        /**
        * Called on ingnorable whitespace
        * @param TDesC8& Whitespace bytes
        * @param TInt Error code
        * @return void
        */
        void OnIgnorableWhiteSpaceL( const TDesC8& aBytes,
                                     TInt aErrorCode );
   
        /**
        * Called on processing instructions
        * @param TDesC8& Target
        * @param TDesC8& Data
        * @param TInt Error code
        * @return void
        */
        void OnProcessingInstructionL( const TDesC8& aTarget,
                                       const TDesC8& aData,
                                       TInt aErrorCode );
    
        /**
        * Called on skipped entity
        * @param RString& Name of the entity
        * @param TInt Error code
        * @return void
        */
        void OnSkippedEntityL( const RString &aName,
                               TInt aErrorCode );
    
        /**
        * Called on document start
        * @param RDocumentParameters& Parameters
        * @param TInt Error code
        * @return void
        */
        void OnStartDocumentL( const RDocumentParameters& aDocParam,
                               TInt aErrorCode );
   
        /**
        * Called on element start
        * @param RTagInfo& The starting element
        * @param Xml::RAttributeArray Attributes
        * @param TInt Error code
        * @return void
        */
        void OnStartElementL( const RTagInfo& aElement,
                              const Xml::RAttributeArray &aAttributes,
                              TInt aErrorCode );
    
        /**
        * Called on prefix mapping start
        * @param RString& Prefix
        * @param RString& URI
        * @param TInt Error code
        * @return void
        */
        void OnStartPrefixMappingL( const RString& aPrefix,
                                    const RString& aUri,
                                    TInt aErrorCode );
                                    
    private:
        
        /**
        * Append attributes from the parameter array
        * @param Xml::RAttributeArray& Attributes
        * @return void
        */
        void AppendAttributeValueL( const Xml::RAttributeArray& aAttributes );
        
        /**
        * Handle next element
        * @param RTagInfo& The new element
        * @param Xml::RAttributeArray& Attributes
        * @return void
        */
        void HandleNextElementL( const Xml::RTagInfo& aElement,
                                 const Xml::RAttributeArray& aAttributes );
        /**
        * Is the parameter descriptor a whitespace
        * @param TDesC8& Bytes to check
        * @return TBool 
        */
        TBool IsWhiteSpace( const TDesC8& aBytes ) const;
        
        /**
        * Append a new namespace
        * @param TDesC8& Prefix
        * @param TDesC8& URI
        * @return void
        */
        void AppendNameSpaceL( const TDesC8& aPrefix, const TDesC8& aUri );
                                   
        /**
        * Set the namespace information to the current node
        * @param RTagInfo& Element
        * @param MXdmNodeInterface* Node interface
        * @return void
        */
        void SetNamespaceInformationL( const RTagInfo& aElement,
                                       MXdmNodeInterface* aXcapNode );
        
    private:    // Data
        
        TInt                             iContentIndex;
        TBool                            iFinished;
        TBool                            iIsRootNode;
        TBool                            iTargetFound;
        TXdmElementType                  iElementType;
        TBool                            iPartialDocument;
        CBufFlat*                        iContentBuffer;
        CXdmDocument*                    iTargetDocument;
        CXdmDocumentNode*                iRootNode;
        CXdmDocumentNode*                iDocumentSubset;
		CXdmDocumentNode*                iCurrentNode;
		CXdmNodeAttribute*               iTargetAttribute;
		CXmlParserNodePath*              iNodePath;
		CXdmXmlParser&                   iParserMain;
    };

#endif      // __XDMXMLCONTENTHANDLER__
            
// End of File
