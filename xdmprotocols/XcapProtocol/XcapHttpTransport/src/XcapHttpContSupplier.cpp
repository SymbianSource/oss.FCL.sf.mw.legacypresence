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
* Description:   CXcapHttpContSupplier
*
*/




// INCLUDE FILES
#include "XcapHttpTransport.h"
#include "XcapHttpContSupplier.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CXcapHttpContSupplier::CXcapHttpContSupplier
// 
// ----------------------------------------------------------
//
CXcapHttpContSupplier::CXcapHttpContSupplier( RHTTPSession& aHttpSession,
                                              CXcapHttpAuthManager& aAuthManager,
                                              CXcapHttpTransport& aTransportMain ) :
                                              CXcapHttpRequest( aHttpSession, aAuthManager, aTransportMain )
                                        
    {
    }

// ----------------------------------------------------------
// CXcapHttpContSupplier::~CXcapHttpContSupplier
// 
// ----------------------------------------------------------
//
CXcapHttpContSupplier::~CXcapHttpContSupplier()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpContSupplier::~CXcapHttpContSupplier()" ) );    
    #endif
    delete iRequestBody;
    }

// ----------------------------------------------------------
// CXcapHttpContSupplier::SetRequestBodyL
// 
// ----------------------------------------------------------
//
EXPORT_C void CXcapHttpContSupplier::SetRequestBodyL( const TDesC8& aRequestBody )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpContSupplier::SetRequestBodyL" ) );    
    #endif
    delete iRequestBody;
    iRequestBody = NULL;
    iRequestBody = HBufC8::NewL( aRequestBody.Length() );
    iRequestBody->Des().Copy( aRequestBody );
    }

// ----------------------------------------------------------
// CXcapHttpContSupplier::RequestBody
// 
// ----------------------------------------------------------
//
EXPORT_C TPtrC8 CXcapHttpContSupplier::RequestBody() const
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpContSupplier::RequestBody" ) );    
    #endif
    return iRequestBody != NULL ? iRequestBody->Des() : TPtrC8();
    }
    
// ----------------------------------------------------------
// CXcapHttpContSupplier::GetNextDataPart
// 
// ----------------------------------------------------------
//
TBool CXcapHttpContSupplier::GetNextDataPart( TPtrC8& aDataPart )
    {
    aDataPart.Set( iRequestBody->Des() );
    return ETrue;
    }

// ----------------------------------------------------------
// CXcapHttpContSupplier::ReleaseData
// 
// ----------------------------------------------------------
//
void CXcapHttpContSupplier::ReleaseData()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpContSupplier::ReleaseData()" ) );    
    #endif
    //Not just yet, lets see if the server accepts the data.
    //If it does, the data needs to be cached and this is
    //probably  - and hopefully - the only copy of the raw 
    //data still available at that point in time
    //delete iRequestBody;
    //iRequestBody = NULL;
    }

// ----------------------------------------------------------
// CXcapHttpContSupplier::Reset
// 
// ----------------------------------------------------------
//
TInt CXcapHttpContSupplier::Reset()
	{
	//This method is called when client Cancel()s a request.
	//There's nothing to reset in our case, because all needed
	//resources are destructed in the destructor.
	//NOTE: Never EVER set this method to return anything but
	//KErrNone, since values < 0 will result in practically
	//untraceable errors thanks to Symbian's lack of proper
	//documentation on the issue.
	return KErrNone;
	}

// ----------------------------------------------------------
// CXcapHttpContSupplier::GetNextDataPart
// 
// ----------------------------------------------------------
//
TInt CXcapHttpContSupplier::OverallDataSize()
	{
    return iRequestBody != NULL ? iRequestBody->Length() : 0;
	}


