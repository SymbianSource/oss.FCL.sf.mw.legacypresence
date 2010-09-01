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
* Description:   CXcapHttpReqDelete
*
*/




// INCLUDE FILES
#include "XcapHttpReqDelete.h"
#include "XcapHttpTransport.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CXcapHttpReqDelete::CXcapHttpReqDelete
// 
// ----------------------------------------------------------
//
CXcapHttpReqDelete::CXcapHttpReqDelete( RHTTPSession& aHttpSession,
                                        CXcapHttpAuthManager& aAuthManager,
                                        CXcapHttpTransport& aTransportMain ) :
                                        CXcapHttpRequest( aHttpSession, aAuthManager, aTransportMain )
                                        
    {
    }

// ----------------------------------------------------------
// CXcapHttpReqDelete::NewL
// 
// ----------------------------------------------------------
//
CXcapHttpReqDelete* CXcapHttpReqDelete::NewL( const TDesC& aRequestUri, 
                                              RHTTPSession& aHttpSession,
                                              CXcapHttpAuthManager& aAuthManager,
                                              CXcapHttpTransport& aTransportMain )
                                        
    {
    CXcapHttpReqDelete* self = new ( ELeave ) CXcapHttpReqDelete( aHttpSession, aAuthManager, aTransportMain );
    CleanupStack::PushL( self );
    self->BaseConstructL( aRequestUri );
    CleanupStack::Pop();  //self
    return self;
    }

// ----------------------------------------------------------
// CXcapHttpReqDelete::~CXcapHttpReqDelete
// 
// ----------------------------------------------------------
//
CXcapHttpReqDelete::~CXcapHttpReqDelete()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpReqDelete::~CXcapHttpReqDelete()" ) );    
    #endif
    }

// ----------------------------------------------------------
// CXcapHttpReqDelete::ConstructRequestL
// 
// ----------------------------------------------------------
//
void CXcapHttpReqDelete::ConstructRequestL()
    {
    CXcapHttpRequest::ConstructRequestL();
    }

// ----------------------------------------------------------
// CXcapHttpReqDelete::ConstructRequestL
// 
// ----------------------------------------------------------
//
RStringF CXcapHttpReqDelete::ConstructMethodStringL()
    {
    return iHttpSession.StringPool().OpenFStringL( KXcapRequestDelete );
    }

