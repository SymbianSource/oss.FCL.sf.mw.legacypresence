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
* Description:  CXcapHttpHeaderModel 
*
*/




#ifndef __XCAPHTTPHEADERMODEL__
#define __XCAPHTTPHEADERMODEL__

// INCLUDES
#include "XcapHttpRequest.h"

//FORWARD DECLARATIONS
class CXcapHttpResponse;

// CLASS DECLARATION
NONSHARABLE_CLASS( CXcapHttpHeaderModel ) : public CBase
    {
    public:

        /**
        * C++ default constructor is private.
        */      
        static CXcapHttpHeaderModel* NewL( const TDesC8& aHeaderName,
                                           const TDesC8& aHeaderValue,
                                           CXcapHttpTransport& aTransportMain );
        
        /**
        * C++ default constructor is private.
        */      
        TPtrC8 HeaderValue() const; 
        
        /**
        * C++ default constructor is private.
        */      
        TPtrC8 HeaderName() const; 

        /**
        * Destructor.
        */      
        virtual ~CXcapHttpHeaderModel();
    
    private:
        
        /**
        * C++ default constructor is private.
        */     
        CXcapHttpHeaderModel( CXcapHttpTransport& aTransportMain );
        
        /**
        * C++ default constructor is private.
        */     
        void ConstructL( const TDesC8& aHeaderName,
                         const TDesC8& aHeaderValue );

    protected: //Data

    private: //Data
        
        HBufC8*                         iHeaderName;
        HBufC8*                         iHeaderValue;
        CXcapHttpTransport&             iTransportMain; 

    };

#endif

// End of File
