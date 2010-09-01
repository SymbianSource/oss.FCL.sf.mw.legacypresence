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
* Description:   CXcapHttpReqPut
*
*/




#ifndef __XCAPHTTPREQPUT__
#define __XCAPHTTPREQPUT__

// INCLUDES
#include "http.h"
#include "XcapHttpContSupplier.h"

//CONSTANTS

//FORWARD DECLARATIONS
class CXcapHttpResponse;

// CLASS DECLARATION
NONSHARABLE_CLASS( CXcapHttpReqPut ) : public CXcapHttpContSupplier
    {
    public:
        
        /**
        * C++ default constructor is private.
        */      
        static CXcapHttpReqPut* NewL( const TDesC& aRequestUri, 
                                      RHTTPSession& aHttpSession,
                                      CXcapHttpAuthManager& aAuthManager,
                                      CXcapHttpTransport& aTransportMain );

         /**
        * Destructor.
        */      
        virtual ~CXcapHttpReqPut();

    private:  // Constructors and destructor
        
        /**
        * C++ default constructor is private.
        */      
        CXcapHttpReqPut( RHTTPSession& aHttpSession,
                         CXcapHttpAuthManager& aAuthManager,
                         CXcapHttpTransport& aTransportMain );

        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        virtual void ConstructRequestL();

        /**
        * Append data to the response buffer.
        */
        virtual RStringF ConstructMethodStringL();

    private:
        
        /**
        * Second-phase constructor.
        */ 
        void ConstructL();
        
    private: //Data
        
    };

#endif

// End of File
