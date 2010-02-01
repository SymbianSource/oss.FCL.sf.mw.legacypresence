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
* Description: CXcapDocument
*
*/




#ifndef __XCAPDOCUMENT__
#define __XCAPDOCUMENT__

#include <e32base.h>
#include <XdmDocument.h>

//CONSTANTS
const TUint KETagBufferMaxLength                       = 64;
_LIT8( KSlash,                                         "/" );
_LIT8( KUserDirName,                                   "users/" );
_LIT8( KGlobalDirName,                                 "global/" );
_LIT8( KXcapGlobalDocName,                             "index" );

//FORWARD DECLARATIONS
class RXcapCache;
class CXdmEngine;
class CXdmNamespace;
class CXcapProtocol;
class CXcapAppUsage;
class MXdmOperation;
class CXcapDocumentNode;
class CXdmOperationFactory;

//CLASS DECLARATION
class CXcapDocument : public CXdmDocument
    {
    public:
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CXcapDocument* NewL( TXdmDocType aDocumentType,
                                    const TDesC& aDocumentName,
                                    const CXdmEngine& aXdmEngine,
                                    const CXcapProtocol& aXcapProtocol );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CXcapDocument* NewL( TXdmDocType aDocumentType,
                                    const TDesC8& aDocumentName,
                                    const CXdmEngine& aXdmEngine,
                                    const CXcapProtocol& aXcapProtocol );
        
        /**
        * Set one option
        * @return void
        * @param The completed update operation
        */
        void SetOption( TInt aOption );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */                         
        TXdmDocType DocumentType() const;
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */  
        void NotifyResolverCompleteL( TInt aError );
        
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
        virtual ~CXcapDocument();
    
    public:  //Exported methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        IMPORT_C TDesC8& ETag();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        IMPORT_C void SetETag( const TDesC8& aETagDesc );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        IMPORT_C TInt DataLength() const;
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        IMPORT_C CXcapAppUsage& ApplicationUsage() const;
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        IMPORT_C CXcapDocument* TempCopyL();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        IMPORT_C void RemoveData( CXcapDocumentNode* aDocumentNode );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        IMPORT_C CXcapProtocol& Protocol() const;
        
        /**
        * Reset the document subset
        */
        IMPORT_C virtual void ResetSubset();
        
    private:  //From CXdmDocument
        
        /**
        * Construct an "anonymous" entry point for this document.
        * This node effectively becomes the root of this document.
        * 
        */
        CXdmDocumentNode* CreateRootL();

        /**
        * Return the time stamp for this document
        *
		* @return TTime 
        */
        TTime TimeStamp() const;
        
        /**
        * Return an error document for a failed update
        * 
        * NOTE: The returning object will be NULL in case
        *       the used protocol does not support this feature.
        *
		* @return CXdmDocument* The error document
        */
        CXdmDocumentNode* ErrorRoot();
              
        /*
        * Reset the contents of this document model.
        */
        void ResetContents();
        
    public:  //From CXdmDocument
    
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
        *
        * @return void
        */
        void ResetNamespaces();
    
    private:
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CXcapDocument( TXdmDocType aDocumentType,
                       const CXdmEngine& aXdmEngine,
                       const CXcapProtocol& aXcapProtocol );
        
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
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */    
		void InstallAppUsageL();
		
		/**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */    
		void UpdateDocumentInfoL( RXcapCache* aCacheClient );
		
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
        void SaveClientStatus( TRequestStatus& aClientStatus );
        
        /**
        * Handle a failed update operation
        * @return void
        * @param CXdmDocOperation* The failed operation
        */
        void HandleErrorL();
        
        /**
        * Handle a successful update operation
        * @return void
        * @param The completed update operation
        */
        void HandleRequestCompletionL();
        
        /**
        * Handle a successful update operation
        * @return void
        * @param The completed update operation
        */
        void ResolveAsyncConflictL();
        
        /**
        * Handle a successful update operation
        * @return void
        * @param The completed update operation
        */
        void CreateTempCopyL();
        
        /**
        * Handle a successful update operation
        * @return void
        * @param The completed update operation
        */
        void ActivateOperationL();
        
        /**
        * Handle a successful update operation
        * @return void
        * @param The completed update operation
        */
        void CheckOperationQueueL();
        
        /**
        * Handle a successful update operation
        * @return void
        * @param The completed update operation
        */
        void DequeueOperation( MXdmOperation* aOperation );
        
        /**
        * Handle a successful update operation
        * @return void
        * @param The completed update operation
        */
        void DequeueAll();
        
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
        * Handle a successful update operation
        * @return void
        * @param The completed update operation
        */                            
        HBufC8* EightBitNameLC();                                       
        
        /**
        * Handle a successful update operation
        * @return void
        * @param The completed update operation
        */    
        TBool IsGlobalTree( const TDesC8& aAuid );
        
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
        
    private:  //Data

        enum TXcapDocumentState
            {
            EXdmDocIdle = 0,
            EXdmDocUpdating,
            EResolvingAsyncConflict,
            ERetryingFailedOperation
            };
        
        TInt                                    iDataLength;
        TInt                                    iUpdateIndex;
        TInt                                    iOperationCount;
        TUint                                   iOptions;
        TTime                                   iLastAccess;
        TTime                                   iLastModification;
        HBufC8*                                 iEightBitName;
        TXdmDocType                             iDocumentType;
        CXcapDocument*                          iTempCopy;
        CXcapAppUsage*                          iAppUsage;
        CXcapDocumentNode*                      iDocumentRoot;
        CXdmDocumentNode*                       iErrorRoot;
        TXcapDocumentState                      iDocumentState;
        CXcapProtocol&                          iXcapProtocol;
        TBuf8<KETagBufferMaxLength>             iETagBuffer;
        RPointerArray<CXdmNamespace>            iNamespaces;
    };

#endif      //__XCAPDOCUMENT__
            
// End of File
