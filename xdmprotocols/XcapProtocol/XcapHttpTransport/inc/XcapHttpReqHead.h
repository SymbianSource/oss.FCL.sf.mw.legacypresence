/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CXcapHttpReqHead
*
*/

#ifndef __XCAPHTTPREQHEAD__
#define __XCAPHTTPREQHEAD__

// INCLUDES
#include "http.h"
#include "XcapHttpRequest.h"

//CONSTANTS
_LIT8( KXcapRequestHead, "HEAD" );

//FORWARD DECLARATIONS
class CXdmHttpResponse;
class CXcapHttpAuthManager;

// CLASS DECLARATION
NONSHARABLE_CLASS( CXcapHttpReqHead ) : public CXcapHttpRequest
    {
    public:
        
        /**
        * C++ default constructor is private.
        */      
        static CXcapHttpReqHead* NewL( const TDesC& aRequestUri,         
                                      RHTTPSession& aHttpSession,
                                      CXcapHttpAuthManager& aAuthManager,
                                      CXcapHttpTransport& aTransportMain );

         /**
        * Destructor.
        */      
        virtual ~CXcapHttpReqHead();

    private:  // Constructors and destructor
        
        /**
        * C++ default constructor is private.
        */      
        CXcapHttpReqHead( RHTTPSession& aHttpSession,
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
