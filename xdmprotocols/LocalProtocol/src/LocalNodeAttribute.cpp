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
* Description: CLocalNodeAttribute
*
*/




// INCLUDE FILES
#include "LocalProtocol.h"
#include "LocalNodeAttribute.h"

// ----------------------------------------------------------
// CLocalNodeAttribute::CLocalNodeAttribute
// 
// ----------------------------------------------------------
//
CLocalNodeAttribute::CLocalNodeAttribute( CXdmEngine& aXdmEngine,
                                          CLocalProtocol& aLocalProtocol ) :
                                          CXdmNodeAttribute( aXdmEngine,
                                        ( MXdmNodeFactory& )*this ),
                                          iLocalProtocol( aLocalProtocol )
                                                
    {   
    }
    
// ----------------------------------------------------------
// CLocalNodeAttribute::CLocalNodeAttribute
// 
// ----------------------------------------------------------
//
CLocalNodeAttribute::CLocalNodeAttribute( CXdmEngine& aXdmEngine,
                                          CXdmDocumentNode* aParentNode,
                                          CLocalProtocol& aLocalProtocol ) :
                                          CXdmNodeAttribute( aXdmEngine, 
                                        ( MXdmNodeFactory& )*this, aParentNode ),
                                          iLocalProtocol( aLocalProtocol )
                                                
    {   
    }

// ----------------------------------------------------------
// CLocalNodeAttribute::CLocalNodeAttribute
// 
// ----------------------------------------------------------
//
CLocalNodeAttribute* CLocalNodeAttribute::NewL( CXdmEngine& aXdmEngine,
                                                const TDesC& aAttributeName,
                                                CXdmDocumentNode* aParentNode,
                                                CLocalProtocol& aLocalProtocol )
    {
    CLocalNodeAttribute* self = new ( ELeave ) CLocalNodeAttribute( aXdmEngine, aParentNode,aLocalProtocol );
    CleanupStack::PushL( self );
    self->CXdmNodeAttribute::BaseConstructL( aAttributeName );
    CleanupStack::Pop();
    return self;
    }
    
// ----------------------------------------------------------
// CLocalNodeAttribute::CLocalNodeAttribute
// 
// ----------------------------------------------------------
//                                        
CLocalNodeAttribute* CLocalNodeAttribute::NewL( CXdmEngine& aXdmEngine,
                                                CXdmDocumentNode* aParentNode,
                                                CLocalProtocol& aLocalProtocol )
    {
    CLocalNodeAttribute* self = new ( ELeave ) CLocalNodeAttribute( aXdmEngine, aParentNode, aLocalProtocol );
    /*CleanupStack::PushL( self );
    self->BaseConstructL();
    CleanupStack::Pop();*/
    return self;
    }

// ----------------------------------------------------------
// CLocalNodeAttribute::~CLocalNodeAttribute
// 
// ----------------------------------------------------------
//
CLocalNodeAttribute::~CLocalNodeAttribute()
    {
    }

// ----------------------------------------------------------
// CLocalDocumentNode::NamespacePrefix
// 
// ----------------------------------------------------------
//
TPtrC8 CLocalNodeAttribute::Prefix() const
    {
    return TPtrC8();
    }
        
// ----------------------------------------------------------
// CLocalDocumentNode::SetNamespacePrefixL
// 
// ----------------------------------------------------------
//
void CLocalNodeAttribute::SetPrefixL( const TDesC& /*aNamespacePrefix*/ )
    {
    }
        
// ----------------------------------------------------------
// CLocalDocumentNode::SetNamespacePrefixL
// 
// ----------------------------------------------------------
//
void CLocalNodeAttribute::SetPrefixL( const TDesC8& /*aNamespacePrefix*/ )
    {
    }


