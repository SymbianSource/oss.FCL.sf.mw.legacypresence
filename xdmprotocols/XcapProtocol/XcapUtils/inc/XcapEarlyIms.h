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
* Description:   CXcapEarlyIms
*
*/




#ifndef __XCAPEARLYIMS__
#define __XCAPEARLYIMS__

#include <etelmm.h>

//COMMON CONSTANTS
_LIT( KUtilsLogDir,                             "XDM" );
_LIT( KUtilsLogFile,                            "XcapUtils.txt" );
_LIT8( KXcapFakeImpu,                            "IMSI@FakeImpu.ims.3gpp.invalid" );
const TInt KUtilsLogBufMaxSize                  = 2000;


//For testing of EarlyIms in WINSCW
//#define __FAKE_IMPU__

//FORWARD DECLARATIONS
class MXcapEarlyImsObs;
class CAsyncRetrieveAuthorizationInfo;

// CLASS DECLARATION
class CXcapEarlyIms : public CActive

    {
    public:  // Constructors and destructor
        
        /**
        * Symbian OS default constructor.
        * @return CXcapEarlyIms New instance
        */     
        IMPORT_C static CXcapEarlyIms* NewL();
        
        /**
        * Symbian OS default constructor.
        * @return CXcapEarlyIms New instance
        */     
        IMPORT_C void RequestSimDataL( MXcapEarlyImsObs* aObserver );
        
        /**
        * Symbian OS default constructor.
        * @return CXcapEarlyIms New instance
        */
        IMPORT_C TPtrC8 PublicIDL();
        
        /**
        * Destructor.
        */      
        IMPORT_C virtual ~CXcapEarlyIms();
    
    #ifdef _DEBUG
        
        /**
        * Clean the log file
        * @return void
        */
        void DeleteLogFileL();
        
        /**
        * A global logging function for 8 bit data.
        * @param aCommand command to be handled
        */
        static void WriteToLog( TRefByValue<const TDesC8> aFmt,... );
    
    #endif //_DEBUG
         
    private:
        
        /**
        * C++ default constructor is private.
        */      
        CXcapEarlyIms();

        /**
        * Symbian OS second-phase constructor.
        * @return void
        */     
        void ConstructL();
        
        /**
        * From CActive
        * A request has completed
        * @return void
        */ 
        void RunL();
        
        /**
        * From CActive
        * Cancel an outstanding request.
        * @return void
        */ 
        void DoCancel();
        
        /**
        * Get the module name
        * @param TDes& Module name on return
        * @return void
        */ 
        void ModuleNameL( TDes& aModuleName ) const;
        
        /**
        * Find out the type of the SIM card
        * and its application
        * @return void
        */ 
        void SelectSimTypeL();
        
        /**
        * Derive IMPU and IMPI from the values
        * retrieved from the SIM card
        * @return TPtrC8 User's Public ID
        */
        TPtrC8 DeriveL();
           
    private: //Data
    
    
        enum TRequestPhase
            {
            TRequestIdle = 0,
            TRequestAuth,
            TRequestNetwork,
            TRequestUsimAuth
            };
        
        enum TXcapSimType
            {
            ESimTypeGsm = 0,
            ESimTypeCdma,
            ESimTypeWcdma,
            ESimTypeUnknown
            };
        
        TBool                                             iRequestComplete;
        HBufC8*                                           iImpu;
        HBufC8*                                           iImpi;
        RTelServer                                        iServer;
        RMobilePhone                                      iPhone;
        TXcapSimType                                      iSimType;
        TRequestPhase                                     iRequestPhase;
        MXcapEarlyImsObs*                                 iObserver;
        CAsyncRetrieveAuthorizationInfo*                  iRetriever;
        RMobilePhone::CImsAuthorizationInfoV5*            iAuthData;
        RMobilePhone::TMobilePhoneSubscriberId            iImsi;
        RMobilePhone::TMobilePhoneNetworkCountryCode      iCountryCode;
        RMobilePhone::TMobilePhoneNetworkIdentity         iNetworkCode;
        TPckgBuf<RMobilePhone::TMobilePhoneNetworkInfoV1> iNetworkInfoPckg;
    };

#endif

// End of File
