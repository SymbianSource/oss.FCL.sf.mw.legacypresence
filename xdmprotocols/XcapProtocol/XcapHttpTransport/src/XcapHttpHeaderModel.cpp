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
* Description:   CXcapHttpHeaderModel
*
*/




// INCLUDE FILES
#include "XcapHttpTransport.h"
#include "XcapHttpHeaderModel.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CXcapHttpHeaderModel::CXcapHttpHeaderModel
// 
// ----------------------------------------------------------
//
CXcapHttpHeaderModel::CXcapHttpHeaderModel( CXcapHttpTransport& aTransportMain ) :
                                            iTransportMain( aTransportMain)
    {
    }

// ----------------------------------------------------------
// CXcapHttpHeaderModel::NewL
// 
// ----------------------------------------------------------
//
CXcapHttpHeaderModel* CXcapHttpHeaderModel::NewL( const TDesC8& aHeaderName,
                                                  const TDesC8& aHeaderValue,
                                                  CXcapHttpTransport& aTransportMain )
                                        
    {
    CXcapHttpHeaderModel* self = new ( ELeave ) CXcapHttpHeaderModel( aTransportMain );
    CleanupStack::PushL( self );
    self->ConstructL( aHeaderName, aHeaderValue );
    CleanupStack::Pop();  //self
    return self;
    }

// ----------------------------------------------------------
// CXcapHttpHeaderModel::HeaderValue
// 
// ----------------------------------------------------------
//
void CXcapHttpHeaderModel::ConstructL( const TDesC8& aHeaderName,
                                       const TDesC8& aHeaderValue )
    {
    delete iHeaderName;
    iHeaderName = NULL;
    iHeaderName = aHeaderName.AllocL();
    delete iHeaderValue;
    iHeaderValue = NULL;
    iHeaderValue = aHeaderValue.AllocL();
    
    }

// ----------------------------------------------------------
// CXcapHttpHeaderModel::HeaderValue
// 
// ----------------------------------------------------------
//
TPtrC8 CXcapHttpHeaderModel::HeaderValue() const
    {
    return iHeaderValue != NULL ? iHeaderValue->Des() : TPtrC8();
    }

// ----------------------------------------------------------
// CXcapHttpHeaderModel::HeaderValue
// 
// ----------------------------------------------------------
//
TPtrC8 CXcapHttpHeaderModel::HeaderName() const
    {
    return iHeaderName != NULL ? iHeaderName->Des() : TPtrC8();
    }
     
// ----------------------------------------------------------
// CXcapHttpHeaderModel::~CXcapHttpHeaderModel
// 
// ----------------------------------------------------------
//
CXcapHttpHeaderModel::~CXcapHttpHeaderModel()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpHeaderModel::~CXcapHttpHeaderModel()" ) );    
    #endif
    delete iHeaderName;
    delete iHeaderValue;
    }

