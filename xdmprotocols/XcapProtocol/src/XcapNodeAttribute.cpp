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
* Description: CXcapNodeAttribute
*
*/




// INCLUDE FILES
#include "XcapProtocol.h"
#include "XcapNodeAttribute.h"

// ----------------------------------------------------------
// CXcapNodeAttribute::CXcapNodeAttribute
// 
// ----------------------------------------------------------
//
CXcapNodeAttribute::CXcapNodeAttribute( CXdmEngine& aXdmEngine ) :
                  CXdmNodeAttribute( aXdmEngine, ( MXdmNodeFactory& )*this )
                                                
    {   
    }
    
// ----------------------------------------------------------
// CXcapNodeAttribute::CXcapNodeAttribute
// 
// ----------------------------------------------------------
//
CXcapNodeAttribute::CXcapNodeAttribute( CXdmEngine& aXdmEngine,
                                        CXdmDocumentNode* aParentNode ) :
                                        CXdmNodeAttribute( aXdmEngine,
                                      ( MXdmNodeFactory& )*this, aParentNode )
                                                
    {   
    }

// ----------------------------------------------------------
// CXcapNodeAttribute::CXcapNodeAttribute
// 
// ----------------------------------------------------------
//
CXcapNodeAttribute* CXcapNodeAttribute::NewL( CXdmEngine& aXdmEngine,
                                              const TDesC& aAttributeName,
                                              CXdmDocumentNode* aParentNode )
    {
    CXcapNodeAttribute* self = new ( ELeave ) CXcapNodeAttribute( aXdmEngine, aParentNode );
    CleanupStack::PushL( self );
    self->CXdmNodeAttribute::BaseConstructL( aAttributeName );
    CleanupStack::Pop();
    return self;
    }
    
// ----------------------------------------------------------
// CXcapNodeAttribute::CXcapNodeAttribute
// 
// ----------------------------------------------------------
//                                        
CXcapNodeAttribute* CXcapNodeAttribute::NewL( CXdmEngine& aXdmEngine, 
                                              CXdmDocumentNode* aParentNode )
    {
    CXcapNodeAttribute* self = new ( ELeave ) CXcapNodeAttribute( aXdmEngine, aParentNode );
    return self;
    }
    
// ----------------------------------------------------------
// CXcapNodeAttribute::~CXcapNodeAttribute
// 
// ----------------------------------------------------------
//
CXcapNodeAttribute::~CXcapNodeAttribute()
    {
    }

// ----------------------------------------------------------
// CXcapNodeAttribute::~CXcapNodeAttribute
// 
// ----------------------------------------------------------
//
TPtrC8 CXcapNodeAttribute::Prefix() const
    {
    return TPtrC8();
    }

// ----------------------------------------------------------
// CXcapNodeAttribute::~CXcapNodeAttribute
// 
// ----------------------------------------------------------
//
void CXcapNodeAttribute::SetPrefixL( const TDesC& /*aNamespacePrefix*/ )
    {
    }

// ----------------------------------------------------------
// CXcapNodeAttribute::~CXcapNodeAttribute
// 
// ----------------------------------------------------------
//
void CXcapNodeAttribute::SetPrefixL( const TDesC8& /*aNamespacePrefix*/ )
    {
    }
