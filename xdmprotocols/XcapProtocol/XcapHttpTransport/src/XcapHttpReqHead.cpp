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
* Description:   CXcapHttpReqHead
*
*/

// INCLUDE FILES
#include "XcapHttpReqHead.h"
#include "XcapHttpTransport.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CXcapHttpReqHead::CXcapHttpReqHead
// 
// ----------------------------------------------------------
//
CXcapHttpReqHead::CXcapHttpReqHead( RHTTPSession& aHttpSession,
                                  CXcapHttpAuthManager& aAuthManager,
                                  CXcapHttpTransport& aTransportMain ) :
                                  CXcapHttpRequest( aHttpSession, aAuthManager, aTransportMain )
                                        
    {
    }

// ----------------------------------------------------------
// CXcapHttpReqHead::NewL
// 
// ----------------------------------------------------------
//
CXcapHttpReqHead* CXcapHttpReqHead::NewL( const TDesC& aRequestUri, 
                                        RHTTPSession& aHttpSession,
                                        CXcapHttpAuthManager& aAuthManager,
                                        CXcapHttpTransport& aTransportMain )
                                        
    {
    CXcapHttpReqHead* self = new ( ELeave ) CXcapHttpReqHead( aHttpSession, aAuthManager, aTransportMain );
    CleanupStack::PushL( self );
    self->BaseConstructL( aRequestUri );
    CleanupStack::Pop();  //self
    return self;
    }

// ----------------------------------------------------------
// CXcapHttpReqHead::~CXcapHttpReqHead
// 
// ----------------------------------------------------------
//
CXcapHttpReqHead::~CXcapHttpReqHead()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpReqHead::~CXcapHttpReqHead()" ) );    
    #endif
    }

// ----------------------------------------------------------
// CXcapHttpReqHead::ConstructRequestL
// 
// ----------------------------------------------------------
//
void CXcapHttpReqHead::ConstructRequestL()
    {
    CXcapHttpRequest::ConstructRequestL();
    }

// ----------------------------------------------------------
// CXcapHttpReqHead::ConstructRequestL
// 
// ----------------------------------------------------------
//
RStringF CXcapHttpReqHead::ConstructMethodStringL()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpReqHead::ConstructMethodStringL()" ) );    
    #endif
    return iHttpSession.StringPool().OpenFStringL( KXcapRequestHead );
    }
