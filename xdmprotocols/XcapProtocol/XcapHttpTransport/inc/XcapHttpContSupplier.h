/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CXcapHttpContSupplier
*
*/




#ifndef __XCAPHTTPCONTSUPPLIER__
#define __XCAPHTTPCONTSUPPLIER__

// INCLUDES
#include "XcapHttpRequest.h"

//FORWARD DECLARATIONS
class CXdmHttpResponse;

// CLASS DECLARATION
class CXcapHttpContSupplier : public CXcapHttpRequest,
                              public MHTTPDataSupplier
    {
    public:

        /**
        * C++ default constructor is private.
        */      
        IMPORT_C void SetRequestBodyL( const TDesC8& aRequestBody );
        
        /**
        * C++ default constructor is private.
        */      
        IMPORT_C TPtrC8 RequestBody() const;

    protected:
        
        /**
        * C++ default constructor is private.
        */      
        CXcapHttpContSupplier( RHTTPSession& aHttpSession,
                               CXcapHttpAuthManager& aAuthManager,
                               CXcapHttpTransport& aTransportMain );
        
         /**
        * Destructor.
        */      
        virtual ~CXcapHttpContSupplier();
    
    private:
        
        /**
        * Second-phase constructor.
        */ 
        TBool GetNextDataPart( TPtrC8& aDataPart );
        
        /**
        * Second-phase constructor.
        */ 
        void ReleaseData();
        
        /**
        * Second-phase constructor.
        */ 
        TInt Reset();

	    /**
        * Second-phase constructor.
        */ 
        TInt OverallDataSize();

    protected: //Data

    private: //Data
        
        HBufC8*                         iRequestBody;
    };

#endif

// End of File
