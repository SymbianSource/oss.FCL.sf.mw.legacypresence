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
* Description:   CXdmXmlParser
*
*/




#ifndef __XDMXMLPARSER__
#define __XDMXMLPARSER__

#include <parser.h>

//CONSTANTS
_LIT( KParserLogFile,                   "XcapXml" );
_LIT8( KDefaultMimeType,                "text/xml" );

using namespace Xml;

//FORWARD DECLARATION
class CXdmDocument;
class CXdmLogWriter;
class CXmlFormatter;
class CXdmDocumentNode;
class MXdmNodeInterface;
class CXdmXmlContentHandler;
class MXdmNamespaceContainer;

// CLASS DECLARATION
class CXdmXmlParser: public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Symbian OS Constructor
        * @return CXdmXmlParser New parser intance  
        */     
        IMPORT_C static CXdmXmlParser* NewL();
        
        /**
        * Parse an XML document
        * @param CXdmDocument Document instance
        * @param TDesC8& The raw XML data
        * @return void
        */    
        IMPORT_C void ParseDocumentL( CXdmDocument* aDocument,
                                       const TDesC8& aXmlDocument );
        
        /**
        * Parse an XML document
        * @param CXdmDocument Document instance
        * @param TDesC8& The raw XML data
        * @param Pointer to the document/fragment root
        * @return void
        */     
        IMPORT_C void ParseDocumentL( CXdmDocument* aDocument,
                                      const TDesC8& aXmlDocument,
                                      CXdmDocumentNode* aDocumentSubset );
        
        /**
        * Parse an XML document
        * @param TDesC8& The raw XML data
        * @param Pointer to the document/fragment root
        * @return void
        */       
        IMPORT_C void ParseDocumentL( const TDesC8& aXmlDocument,
                                      CXdmDocumentNode* aDocumentRoot );

        /**
        * Format a document model into textual form
        * @param TBool Indicates whether the document is whole
        * @param CXdmDocument The document instance
        * @param CXdmDocumentNode Pointer to the document/fragment root
        * @return HBufC8* The document in textual form
        */       
        IMPORT_C HBufC8* FormatToXmlLC( TBool aIsWholeDocument,
                                        const CXdmDocument* aDocument,
                                        const CXdmDocumentNode* aRootNode );
                                               
        /**
        * Format a document model into textual form and append the fragment
        * in the first parameter to the correct place inside the document.
        * @param TDesC8& The fragment to append
        * @param CXdmDocument The document instance
        * @param CXdmDocumentNode Pointer to the fragment
        * @return HBufC8* The document in textual form
        */        
        IMPORT_C HBufC8* FormatToXmlLC( const TDesC8& aXmlFragment,
                                        const CXdmDocument* aDocument,
                                        const CXdmDocumentNode* aTargetNode );
                                               
        /**
        * Destructor.
        */      
        IMPORT_C virtual ~CXdmXmlParser();

    public: 
    
    #ifdef _DEBUG
        
        /**
        * Write to log file
        * @param TRefByValue<const TDesC8> The log line
        * @return void
        */  
        void WriteToLog( TRefByValue<const TDesC8> aFmt,... ) const;
    
    #endif
        
    public:   
    
        /**
        * Stop the parser
        * @return void
        */  
        void FinishParsingL();
        
    private:
        
        /**
        * C++ constructor is private
        * @return CXdmXmlParser
        */    
        CXdmXmlParser();

        /**
        * Second-phase constructor
        * @return void
        */    
        void ConstructL();
                          
    #ifdef _DEBUG
    
        /**
        * Return the current system time
        * @return TInt Current system time in integer format
        */  
        static TInt TimeL();
        
        /**
        * Write the formatted document to the log folder
        * @param HBufC8* Pointer to the data segment
        * @return void
        */  
        void DumpDocumentL( HBufC8* aDocData );
    
    #endif
            
    private: //Data
        
        TInt                                iDumpIndex;
        TPtr8                               iXmlDocument;
        CParser*                            iXmlParser;
        CXdmLogWriter*                      iLogWriter;
        CXmlFormatter*                      iXmlFormatter;
        TRequestStatus*                     iClientStatus;
        CXdmXmlContentHandler*              iContentHandler;
    };

#endif  //__XDMXMLPARSER__


// End of File
