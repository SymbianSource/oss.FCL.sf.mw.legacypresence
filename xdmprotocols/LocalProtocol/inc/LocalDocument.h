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
* Description: CLocalDocument
*
*/




#ifndef __LOCALDOCUMENT__
#define __LOCALDOCUMENT__

#include <e32base.h>
#include "XdmDocument.h"

_LIT( KTimeStampFileExt,                           ".tmsp");
_LIT( KDateFormat,                                 "%1-%2-%3");
_LIT( KTimeFormat,                                 "%H:%T:%S");
const TInt KDateTimeMaxSize                        = 200;
const TInt KDateBufferMaxSize                      = 50;

//FORWARD DECLARATIONS
class CXdmNamespace;
class MXdmOperation;
class CLocalProtocol;
class CLocalDocumentNode;
class CXdmOperationFactory;

//CLASS DECLARATION
class CLocalDocument : public CXdmDocument
    {
    public:
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CLocalDocument* NewL( CXdmEngine& aXdmEngine,
                                     const TDesC& aDocumentName,
                                     CLocalProtocol& aLocalProtocol );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        //static CLocalDocument* NewL( const TDesC8& aDocumentName );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void StartInternalL( TRequestStatus& aStatus );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void CancelUpdate(); 

        /**
        * Destructor.
        */      
        virtual ~CLocalDocument();
    
    public:  //Exported methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        IMPORT_C CLocalDocument* TempCopyL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        IMPORT_C TPtrC XmlFilePath() const;
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        IMPORT_C TBool IsSubset() const;
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        IMPORT_C virtual void ResetSubset();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        IMPORT_C void RemoveData( CLocalDocumentNode* aDocumentNode );
    
    public:   //From CXdmDocument
    
        /**
        * Return the root element of this document
        *
        * @return CXdmDocumentNode The root element of the document
        *
        * NOTE: The returning node may or may not contain any data,
        *       since, in the case that this document model does not
        *       have content yet, the element is a simple "entry point"
        *       to the whole document. Thus, for instance, if the client
        *       application requests the Xdm Engine to fetch a particular
        *       document, the root element points to nothing until the 
        *       request has been completed.
        *         
        */
        CXdmDocumentNode* DocumentRoot() const;
        
    private:  //From CXdmDocument
        
        /**
        * Construct an "anonymous" entry point for this document.
        * This node effectively becomes the root of this document.
        * 
        */
        CXdmDocumentNode* CreateRootL();
        
        /*
        * Reset the contents of this document model.
        */
        void ResetContents();
        
        /**
        * Return the time stamp for this document
        *
		* @return TTime 
        */
        TTime TimeStamp() const;
        
        /**
        * Return an error document for a failed update
        * 
        * NOTE: Error document is not supported in this
        *       protocol. The returning pointer will be NULL.
        *
		* @return CXdmDocument* The error document
        */
        CXdmDocumentNode* ErrorRoot();
        
        /**
        * Append an element to the path identifying the 
        * the current document subset
        * @return void
        */
        void AppendPathPartL( const TDesC& aString );
        
        /**
        * Return the current document subset (including descendants)
        * @return void
        */
        CXdmDocumentNode* CurrentExtent() const;
        
        /**
        * Construct an "anonymous" entry point for this document.
        * This node effectively becomes the root of this document.
        */
        TXdmDocType DocumentType() const;
        
    private:  //From MXdmNamespaceContainer
    
        /**
        * Return the number of namespace declarations
        * @return TInt Namespace declaration count
        */
        TInt Count() const;
        
        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        void AppendNamespaceL( const TDesC8& aUri, const TDesC8& aPrefix );
        
        /**
        * Remove a namespace from the document
        * @param TDesC8& URI of the namespace
        * @param TDesC8& Prefix of the namespace
        */
        void RemoveNamespace( const TDesC8& aUri );
        
        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        TPtrC8 Uri( const TDesC8& aPrefix ) const;
        
        /**
        * Fetch the aIndex-th URI
        * @param TInt The index of the desired URI string
        * @return TPtrC URI string
        */
        TPtrC8 Uri( TInt aIndex ) const;
        
        /**
        * Fetch the aIndex-th prefix
        * @param TDesC8 The index of the desired prefix string
        * @return TPtrC Prefix string
        */
        TPtrC8 Prefix( TInt aIndex ) const;
    
        /**
        * Resets namespaces from the document
        */
        void ResetNamespaces( );
    
    private:
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CLocalDocument( CXdmEngine& aXdmEngine,
                        CLocalProtocol& aLocalProtocol );
        
        /**
        * From CActive.
        * @return void
        */
		void RunL();

        /**
        * From CActive.
        * @return void
        */
		void DoCancel();
		
		/**
        * From CActive.
        * @return void
        */
		void FinaliseOperation( TInt aIndex );
		
		/**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void StartUpdateL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void SaveClientStatus( TRequestStatus& aStatus );           
        
        /**
        * _DEBUG
        */
        void DumpResponseBodyL( const TDesC8& aResponseBody ) const;
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void ConstructL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void ConstructL( const TDesC8& aDocumentName );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void FetchTimeStampL();
        
    private:  //Data
        
        enum TLocalOperation
            {
            EFetch = 0,
            EReplace,
            EInsert,
            EFetchPartial,
            EInsertPartial,
            EReplacePartial,
            };
            
        enum TLocalDocumentState
            {
            EXdmDocIdle = 0,
            EXdmDocUpdating,
            EResolvingAsyncConflict,
            };
        
        TInt                                    iDataLength;
        TInt                                    iUpdateIndex;
        TInt                                    iOperationCount;                                  
        TInt                                    iOperationError;
        TTime                                   iLastAccess;
        TTime                                   iLastModification;
        TBool                                   iWholeDoc;
        HBufC*                                  iFullPath;
        HBufC*                                  iDocSubsetPath;
        TLocalOperation                         iOperationType;
        CLocalProtocol&                         iLocalProtocol;
        CLocalDocumentNode*                     iDocumentRoot;
        TLocalDocumentState                     iDocumentState;
        RPointerArray<CXdmNamespace>            iNamespaces;
    };

#endif      //__XCAPDOCUMENT__
            
// End of File
