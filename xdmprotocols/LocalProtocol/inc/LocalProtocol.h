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
* Description: CLocalProtocol
*
*/




#ifndef __LOCALPROTOCOL__
#define __LOCALPROTOCOL__

#include <e32base.h>
#include "XdmEngine.h"
#include "XdmProtocol.h"
#include "XdmShutdownSwitch.h"

//CONSTANTS
_LIT( KLocalEngLogFile,                          "LocalProtocol" );
_LIT( KLocalEngPrivateRoot,                      "C:\\private\\" );
const TInt KLocalEngLogBufferMaxSize		    = 2000;

//FORWARD DECLARATIONS
class CXdmXmlParser;
class CXdmLogWriter;
class CXdmOperationFactory;

//CLASS DECLARATION
class CLocalProtocol : public CXdmProtocol
    {
    
    public:
        
        /**
        * Fetch the capabilities for this protocol. May require
        * network operations.
        */
        IMPORT_C static RFs& FileSession(); 
        
                
        /**
        * A Global logging function
        * @param TRefByValue<const TDesC8> aFmt,...
        * @return void
        */ 
        IMPORT_C static TPtrC RootFolder();
        
    private:  //From CXdmProtocol
   
        /**
        * Initialise the media used to transfer Xdm data
        */
        void InitTransferMedium( TInt aIdleTimeout,
                                 TRequestStatus& aStatus  );
        
        /**
        * Cancel initialisation of the transfer media.
        */
        void CancelTransferMediumInit();
        
        /**
        * Is the transfer medium ready
        */
        TBool IsTransferAvailable() const;
        
        /**
        * Create a new document node.
        */
        CXdmDocumentNode* CreateDocumentNodeL();
        
        /**
        * Create a new protocol dependent document model
        */
        CXdmDocument* CreateDocumentL( const TDesC& aDocumentName,
                                       const TXdmDocType aDocumentType );
        
        /**
        * Create a new protocol dependent document model
        */
        CXdmDirectory* CreateDirectoryL( const TDesC& aDirectoryPath );

        /**
        * Destructor.
        */      
        virtual ~CLocalProtocol();

    public:  //Static
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CLocalProtocol* NewL( const TXdmProtocolParams& aProtocolInfo );
        
        /**
        * A Global logging function
        * @param TRefByValue<const TDesC8> aFmt,...
        * @return void
        */ 
        RFs& RFSession();
        
        /**
        * A Global logging function
        * @param TRefByValue<const TDesC8> aFmt,...
        * @return void
        */ 
        TPtrC Root();
        
    public:
    
        /**
        * A logging function
        * @param TRefByValue<const TDesC8> aFmt,...
        * @return void
        */ 
        void WriteToLog( TRefByValue<const TDesC8> aFmt,... ) const;

    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CLocalProtocol( const CXdmEngine& aXdmEngine,
                        const CXdmProtocolInfo& aProtocolInfo );
        
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
        void SetRootDirectoryL( const TDesC& aRootPath );
    
    private:
    
        /**
        * From MXdmShutdownSwitch
        * @return void
        */ 
        //void SwitchOff();

    private:  //Data
        
        RFs                                      iFileSession;
        TBool                                    iTransferMediaOpen;
        HBufC*                                   iRootFolder;
        CXdmEngine&                              iXdmEngine;
        CXdmLogWriter*                           iLogWriter;
        CXdmXmlParser*                           iXmlParser;
        const CXdmProtocolInfo&                  iProtocolInfo;
    };

#endif      //__LOCALPROTOCOL__
            
// End of File
