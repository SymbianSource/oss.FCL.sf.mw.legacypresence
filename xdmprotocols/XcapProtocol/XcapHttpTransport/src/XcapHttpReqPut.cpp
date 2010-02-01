/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDE FILES
#include "XcapHttpReqPut.h"
#include "XcapHttpTransport.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CXcapHttpReqPut::CXcapHttpReqPut
// 
// ----------------------------------------------------------
//
CXcapHttpReqPut::CXcapHttpReqPut( RHTTPSession& aHttpSession,
                                  CXcapHttpAuthManager& aAuthManager,
                                  CXcapHttpTransport& aTransportMain ) :
                                  CXcapHttpContSupplier( aHttpSession, aAuthManager, aTransportMain )
                                        
    {
    }

// ----------------------------------------------------------
// CXcapHttpReqPut::NewL
// 
// ----------------------------------------------------------
//
CXcapHttpReqPut* CXcapHttpReqPut::NewL( const TDesC& aRequestUri, 
                                        RHTTPSession& aHttpSession,
                                        CXcapHttpAuthManager& aAuthManager,
                                        CXcapHttpTransport& aTransportMain )
                                        
    {
    CXcapHttpReqPut* self = new ( ELeave ) CXcapHttpReqPut( aHttpSession, aAuthManager, aTransportMain );
    CleanupStack::PushL( self );
    self->BaseConstructL( aRequestUri );
    CleanupStack::Pop();  //self
    return self;
    }

// ----------------------------------------------------------
// CXcapHttpReqPut::~CXcapHttpReqPut
// 
// ----------------------------------------------------------
//
CXcapHttpReqPut::~CXcapHttpReqPut()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpReqPut::~CXcapHttpReqPut()" ) );    
    #endif
    }

// ----------------------------------------------------------
// CXcapHttpReqPut::ConstructRequestL
// 
// ----------------------------------------------------------
//
void CXcapHttpReqPut::ConstructRequestL()
    {
    CXcapHttpRequest::ConstructRequestL();
    iHttpTransaction.Request().SetBody( *this );
    }

// ----------------------------------------------------------
// CXcapHttpReqPut::ConstructRequestL
// 
// ----------------------------------------------------------
//
RStringF CXcapHttpReqPut::ConstructMethodStringL()
    {
    return iHttpSession.StringPool().OpenFStringL( KXcapRequestPut );
    }

