/*
* ============================================================================
*  Name     : CXcapHttpReqHead from XcapHttpReqHead.cpp
*  Part of  : Transport Adapter of the Symbian OS WV Engine 
*
*  Description:
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
* Description:   CXcapHttpReqMkcol
*
*/

// INCLUDE FILES
#include "XcapHttpReqMkcol.h"
#include "XcapHttpTransport.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CXcapHttpReqMkcol::CXcapHttpReqMkcol
// 
// ----------------------------------------------------------
//
CXcapHttpReqMkcol::CXcapHttpReqMkcol( RHTTPSession& aHttpSession,
                                  CXcapHttpAuthManager& aAuthManager,
                                  CXcapHttpTransport& aTransportMain ) :
                                  CXcapHttpContSupplier( aHttpSession, aAuthManager, aTransportMain )
                                        
    {
    }

// ----------------------------------------------------------
// CXcapHttpReqMkcol::NewL
// 
// ----------------------------------------------------------
//
CXcapHttpReqMkcol* CXcapHttpReqMkcol::NewL( const TDesC& aRequestUri, 
                                        RHTTPSession& aHttpSession,
                                        CXcapHttpAuthManager& aAuthManager,
                                        CXcapHttpTransport& aTransportMain )
                                        
    {
    CXcapHttpReqMkcol* self = new ( ELeave ) CXcapHttpReqMkcol( aHttpSession, aAuthManager, aTransportMain );
    CleanupStack::PushL( self );
    self->BaseConstructL( aRequestUri );
    CleanupStack::Pop();  //self
    return self;
    }

// ----------------------------------------------------------
// CXcapHttpReqMkcol::~CXcapHttpReqMkcol
// 
// ----------------------------------------------------------
//
CXcapHttpReqMkcol::~CXcapHttpReqMkcol()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpReqMkcol::~CXcapHttpReqMkcol()" ) );    
    #endif
    }

// ----------------------------------------------------------
// CXcapHttpReqMkcol::ConstructRequestL
// 
// ----------------------------------------------------------
//
void CXcapHttpReqMkcol::ConstructRequestL()
    {
    CXcapHttpRequest::ConstructRequestL();
    iHttpTransaction.Request().SetBody( *this );
    }

// ----------------------------------------------------------
// CXcapHttpReqMkcol::ConstructRequestL
// 
// ----------------------------------------------------------
//
RStringF CXcapHttpReqMkcol::ConstructMethodStringL()
    {
    return iHttpSession.StringPool().OpenFStringL( KXcapRequestMkcol );
    }

