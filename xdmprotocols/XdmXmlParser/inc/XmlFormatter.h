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
* Description:   CXmlFormatter
*
*/




#ifndef __XMLFORMATTER__
#define __XMLFORMATTER__

// INCLUDES
#include <e32base.h>
#include "XmlParserNodePath.h"
#include "XmlNodePathInterface.h"

//Konsts
_LIT8( KXmldocumentStart,               "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );

//Forwar declarations
class CXdmDocument;
class CXdmXmlParser;
class CXdmDocumentNode;
class MXdmNodeInterface;
class CXmlParserNodePath;

// CLASS DECLARATION
NONSHARABLE_CLASS( CXmlFormatter ) : public CBase,
                                     public MXmlNodePathInterface
    {
    public:  // Constructors and destructor
        
        /**
        * Symbian OS Constructor
        * @return CXmlFormatter* New XML formatter
        */
        static CXmlFormatter* NewL( CXdmXmlParser& aParserMain );
        
        /**
        * Format a document
        * @param TBool Indicates whether parameter is a whole document
        * @param CXdmDocument* The target document
        * @param CXdmDocumentNode* The target element
        * @return HBufC8* The document in textual form
        */
        HBufC8* FormatLC( TBool aIsWholeDocument,
                          const CXdmDocument* aDocument,
                          const CXdmDocumentNode* aRootNode );
        
        /**
        * Format a document
        * @param TDesC8& An XML fragment to append
        * @param CXdmDocument* The target document
        * @param CXdmDocumentNode* The target element
        * @return HBufC8* The document in textual form
        */
        HBufC8* FormatLC( const TDesC8& aXmlFragment,
                          const CXdmDocument* aDocument,
                          const CXdmDocumentNode* aTargetNode );
                          
        /**
        * Destructor
        */
        virtual ~CXmlFormatter();
    
    private:  //From MXmlParserNodePathInterface
        
        /**
        * Return the target attribute
        * @return CXdmNodeAttribute* target attribute
        */
        CXdmNodeAttribute* TargetAttribute() const;
        
    private:
        
        /**
        * C++ constructor
        * @return CXmlFormatter
        */
        CXmlFormatter( CXdmXmlParser& aParserMain );
        
        /**
        * Symbian OS second-phase constructor
        * @return void
        */
        void ConstructL();

        /**
        * Read data from the format buffer into a contiguous segment
        * @return HBufC8* An XML document in textual form
        */  
        HBufC8* BufferToStringLC();
        
        /**
        * Check the next element in a document
        * @param CXdmDocumentNode& An XML element
        * @return void
        */
        void CheckSubsetPath( const CXdmDocumentNode& aNode );
        
        /**
        * Reset the internal format buffer
        * @return void
        */  
        void ResetFormatBuffer();
        
        /**
        * Find the (local) name of an element
        * @param TDesC8& The whole element string
        * @return TPtrC8 Name part of the element string
        */ 
        TPtrC8 FindElementName( const TDesC8& aElementString );
        
        /**
        * Format namespace declarations
        * @param CXdmDocumentNode& An XML element
        * @return MXdmNamespaceContainer& Namespace container
        */ 
        void FormatNamespaceDeclarationsL( const CXdmDocumentNode& aRootNode,
                                           const MXdmNamespaceContainer& aContainer );
                                           
        /**
        * Format the parameter element and its descendants
        * @param CXdmDocumentNode The element to format
        * @return void
        */  
        void FormatElementsL( CXdmDocumentNode* aCurrentNode );

        /**
        * Format a leaf element 
        * @param CXdmDocumentNode The element to format
        * @return void
        */    
        void FormatLeafElementL( CXdmDocumentNode* aCurrentNode );
        
        /**
        * Format the start of the parameter element
        * @param TDesC8& Element prefix 
        * @param TDesC8& Element URI 
        * @return void
        */ 
        void FormatElementStartL( const TDesC8& aPrefix,
                                  const TDesC8& aElementData ); 

        /**
        * Format the end of the parameter element
        * @param TDesC8& Element prefix 
        * @param TDesC8& Element URI 
        * @return void
        */ 
        void FormatElementEndL( const TDesC8& aPrefix,
                                const TDesC8& aElementData );
        
        /**
        * Format an empty element 
        * @param CXdmDocumentNode The element to format
        * @return void
        */    
        void FormatEmptyElementL( CXdmDocumentNode* aEmptyNode );
        
                                                   
        /**
        * Append a namespace mapping
        * @param TDesC8& Element URI 
        * @param TDesC8& Element prefix
        * @return void
        */                                      
        void AppendNamespaceL( const TDesC8& aUri, const TDesC8& aPrefix );
        
        /**
        * Append XML fragment into the format buffer
        * @return void
        */ 
        void AppendXmlFragmentL();
        
        /**
        * Return the prefix of the parameter element
        * @param MXdmNodeInterface& An XML element
        * @return TPtrC8 Prefix
        */  
        TPtrC8 Prefix( const MXdmNodeInterface& aInterface );
        
    private: //Data
        
        TInt                                iFormBufferPos;
        TBool                               iTargetFound;
        TBool                               iIsAppended;
        CBufSeg*                            iFormatBuffer;
        TPtrC8                              iDocumentFragment;
        CXdmXmlParser&                      iParserMain;
        CXdmNodeAttribute*                  iTargetAttribute;
        CXmlParserNodePath*                 iNodePath;
        TXdmElementType                     iElementType;
       
    };

#endif  //__XMLPARSERNODEPATH__


// End of File
