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
* Description:   CXcapHttpReqGet
*
*/




// INCLUDE FILES
#include "XcapHttpReqGet.h"
#include "XcapHttpTransport.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CXcapHttpReqGet::CXcapHttpReqGet
// 
// ----------------------------------------------------------
//
CXcapHttpReqGet::CXcapHttpReqGet( RHTTPSession& aHttpSession,
                                  CXcapHttpAuthManager& aAuthManager,
                                  CXcapHttpTransport& aTransportMain ) :
                                  CXcapHttpRequest( aHttpSession, aAuthManager, aTransportMain )
                                        
    {
    }

// ----------------------------------------------------------
// CXcapHttpReqGet::NewL
// 
// ----------------------------------------------------------
//
CXcapHttpReqGet* CXcapHttpReqGet::NewL( const TDesC& aRequestUri, 
                                        RHTTPSession& aHttpSession,
                                        CXcapHttpAuthManager& aAuthManager,
                                        CXcapHttpTransport& aTransportMain )
                                        
    {
    CXcapHttpReqGet* self = new ( ELeave ) CXcapHttpReqGet( aHttpSession, aAuthManager, aTransportMain );
    CleanupStack::PushL( self );
    self->BaseConstructL( aRequestUri );
    CleanupStack::Pop();  //self
    return self;
    }

// ----------------------------------------------------------
// CXcapHttpReqGet::~CXcapHttpReqGet
// 
// ----------------------------------------------------------
//
CXcapHttpReqGet::~CXcapHttpReqGet()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpReqGet::~CXcapHttpReqGet()" ) );    
    #endif
    }

// ----------------------------------------------------------
// CXcapHttpReqGet::ConstructRequestL
// 
// ----------------------------------------------------------
//
void CXcapHttpReqGet::ConstructRequestL()
    {
    CXcapHttpRequest::ConstructRequestL();
    }

// ----------------------------------------------------------
// CXcapHttpReqGet::ConstructRequestL
// 
// ----------------------------------------------------------
//
RStringF CXcapHttpReqGet::ConstructMethodStringL()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpReqGet::ConstructMethodStringL()" ) );    
    #endif
    return iHttpSession.StringPool().OpenFStringL( KXcapRequestGet );
    }
