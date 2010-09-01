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
* Description: CLocalDirectory
*
*/




#ifndef __LOCALDIRECTORY__
#define __LOCALDIRECTORY__

#include <e32base.h>
#include "XdmDirectory.h"

//CONSTANTS

//FORWARD DECLARATIONS
class CXdmDocument;
class CLocalProtocol;
class CLocalDirectoryEntry;

//CLASS DECLARATION
NONSHARABLE_CLASS( CLocalDirectory ) : public CXdmDirectory
    {
    public:
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CLocalDirectory* NewL( const TDesC& aFilePath,
                                      CXdmEngine& aXdmEngine,
                                      CLocalProtocol& aLocalProtocol );

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        //void CancelUpdate();

        /**
        * Destructor.
        */      
        virtual ~CLocalDirectory();
        
    private:  //From CXdmDirectory
        
        /**
        * Start updating the contents of this directory model
        * @param TRequestStatus& Request status of the client
        * @param TDirUpdatePhase Phase of the update
        * @return void
        */
        void StartUpdateL();
        
        /**
        * Cancel a directory update
		* @return void 
        */
        void CancelUpdate();
        
        /**
        * Save the client's request status and update status
        * @param TDirUpdatePhase The update phase
        * @param TRequestStatus The client's request status
		* @return void 
        */
        void SaveRequestData( TDirUpdatePhase aUpdatePhase,
                              TRequestStatus& aClientStatus );
                                        
        /**
        * Return the root element of this document
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
        CLocalDirectory( CXdmEngine& aXdmEngine,
                         CLocalProtocol& aLocalProtocol );
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void ConstructL();
        
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
        void FindTimeStampFilesL( const CDir* aDirectory );
        
        /**
        * From CXdmDirectory.
        * @return void
        */       
        void MatchWithStampFilesL( const CDir* aDirectory );
        
        /**
        * From CXdmDirectory.
        * @return void
        */    
        TInt OfferEntryL( const TDesC& aEntryName );
        
        /**
        * From CXdmDirectory.
        * @return void
        */
        void RefreshEntryArrayL();
        
    private:  //Data
        
        TRequestStatus*                             iClientStatus;
        TDirUpdatePhase                             iUpdatePhase;
        CLocalProtocol&                             iLocalProtocol;
        RPointerArray<CLocalDirectoryEntry>         iEntryArray;
    };

#endif      //__LOCALDIRECTORY__
            
// End of File
