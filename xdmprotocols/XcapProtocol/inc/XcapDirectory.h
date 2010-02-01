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
* Description: CXcapDirectory
*
*/




#ifndef __XCAPDIRECTORY__
#define __XCAPDIRECTORY__

#include <e32base.h>
#include "XdmDirectory.h"
#include "XdmDocument.h"

//CONSTANTS
_LIT( KDirectoryEntry,                     "entry" );
_LIT( KAuidAttribute,                      "auid" );
_LIT( KDocumentUri,                        "uri" );
_LIT( KDocumentETag,                       "etag" );

//AUIDs
_LIT( KCapabilities,            "/services/org.openmobilealliance.xcap-caps/" );
_LIT( KPocRules,                "/services/org.openmobilealliance.poc-rules" );
_LIT( KPocGroup,                "/services/org.openmobilealliance.poc-groups" );
_LIT( KResourceLists,           "/services/org.openmobilealliance.resource-lists/" );
_LIT( KPresenceRules,           "/services/org.openmobilealliance.pres-rules/" );
_LIT( KDirectoryUsage,          "/services/org.openmobilealliance.xcap-directory/" );
_LIT( KSharedXDMS,              "/services/org.openmobilealliance.resource-lists/" );
_LIT( KRlsServices,             "/services/org.openmobilealliance.rls-services/" );

//FORWARD DECLARATIONS
class CXdmDocument;
class CXcapProtocol;
class CXcapRetrieval;
class CXcapHttpReqGet;
class CXcapOperationFactory;

//CLASS DECLARATION
NONSHARABLE_CLASS( CXcapDirectory ) : public CXdmDirectory
    {
    public:
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CXcapDirectory* NewL( const TDesC& aAUID,
                                     CXdmEngine& aXdmEngine,
                                     CXdmDocument*& aDirectoryDoc,
                                     CXcapProtocol& aXcapProtocol );

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void CancelUpdate();

        /**
        * Destructor.
        */      
        virtual ~CXcapDirectory();
    
    private: //From CXdmDirectory
        
        /**
        * Destructor.
        */
        TInt DocumentCount();
        
        /**
        * Return the root element of this document
        */
        TPtrC Document( TInt aIndex ) const;

        /**
        * Returns the path to this directory
        *
		* @return TPtrC8 The path to the directory this object models 
        */
        TPtrC DirectoryPath() const;
        
        /**
        * Returns the path to this directory
        *
		* @return TPtrC8 The path to the directory this object models 
        */
        TXdmDocType DocumentTypeL( TInt aIndex ) const;

    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CXcapDirectory( CXdmDocument*& aDirectoryDoc,
                        CXdmEngine& aXdmEngine,
                        CXcapProtocol& aXcapProtocol );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void ConstructL( const TDesC& aAUID );
        
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
        * From CXdmDirectory.
        * @return void
        */
        void StartUpdateL();
        
        /**
        * Save the client's request status and update status
        * @param TDirUpdatePhase The update phase
        * @param TRequestStatus The client's request status
		* @return void 
        */
        void SaveRequestData( TDirUpdatePhase aUpdatePhase,
                              TRequestStatus& aClientStatus );
        
        /**
        * From CXdmDirectory.
        * @return void
        */                 
        void HandleDocumentListRequestL();
        
        /**
        * From CXdmDirectory.
        * @return void
        */
        void HandleRefreshRequestL();
        
        /**
        * From CXdmDirectory.
        * @return void
        */
        void UpdateNextDocumentL();
        
        /**
        * From CXdmDirectory.
        * @return void
        */
        TBool CheckLocalCopyL( const CXdmDocumentNode* aDocumentNode );
        
        /**
        * From CXdmDirectory.
        * @return void
        */
        TPtrC ExtractDocumentUriL( const TDesC8& aRootUri,
                                   const CXdmDocumentNode* aDocumentNode );
        
        /**
        * From CXdmDirectory.
        * @return void
        */                          
        TXdmDocType FindAUID( const CXdmDocumentNode& aEntryNode ) const;
        
    private:  //Data
        
        TInt                            iUpdateIndex;
        TInt                            iDocumentCount;
        HBufC*                          iAUID;
        CXcapDocument*                  iDirectoryDoc;
        TRequestStatus*                 iClientStatus;
        TDirUpdatePhase                 iUpdatePhase;
        CXcapHttpReqGet*                iHttpRetrieval;
        CXcapProtocol&                  iXcapProtocol;
        RPointerArray<CXdmDocumentNode> iUpdateList;
        RPointerArray<CXdmDocumentNode> iDocumentList;
    };

#endif      //__XCAPDIRECTORY__
            
// End of File
