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
* Description: CXcapDocumentNode 
*
*/




// INCLUDE FILES
#include "XdmDocument.h"
#include "XcapProtocol.h"
#include "XcapDocumentNode.h"
#include "XcapNodeAttribute.h"
                                        
// ----------------------------------------------------------
// CXcapDocumentNode::CXcapDocumentNode
// 
// ----------------------------------------------------------
//
CXcapDocumentNode::CXcapDocumentNode( CXdmEngine& aXdmEngine,
                                      CXcapProtocol& aXcapProtocol ) :
                                      CXdmDocumentNode( aXdmEngine, ( MXdmNodeFactory& )*this ),
                                      iXcapProtocol( aXcapProtocol )                                    
                                                
    {
    }


// ----------------------------------------------------------
// CXcapDocumentNode::CXcapDocumentNode
// 
// ----------------------------------------------------------
//
CXcapDocumentNode::CXcapDocumentNode( CXdmEngine& aXdmEngine,
                                      CXcapProtocol& aXcapProtocol,
                                      CXdmDocumentNode* aParentNode ) :
                                      CXdmDocumentNode( aXdmEngine,
                                    ( MXdmNodeFactory& )*this, aParentNode ),
                                      iXcapProtocol( aXcapProtocol )
                                                
    {   
    }

// ----------------------------------------------------------
// CXcapDocumentNode::CXcapDocumentNode
// 
// ----------------------------------------------------------
//
CXcapDocumentNode::CXcapDocumentNode( const TBool aLeafNode,
                                      CXdmEngine& aXdmEngine,
                                      CXcapProtocol& aXcapProtocol,
                                      CXdmDocumentNode* aParentNode ) :
                                      CXdmDocumentNode( aLeafNode, aXdmEngine, 
                                      ( MXdmNodeFactory& )*this, aParentNode ),
                                      iXcapProtocol( aXcapProtocol )                                                
    {
    
    }

// ----------------------------------------------------------
// CXcapDocumentNode::NewL
// 
// ----------------------------------------------------------
//
CXcapDocumentNode* CXcapDocumentNode::NewL( CXdmEngine& aXdmEngine,
                                            CXcapProtocol& aXcapProtocol )
    {
    return new ( ELeave ) CXcapDocumentNode( aXdmEngine, aXcapProtocol );
    }

// ----------------------------------------------------------
// CXcapDocumentNode::NewL
// 
// ----------------------------------------------------------
//
CXcapDocumentNode* CXcapDocumentNode::NewL( CXdmEngine& aXdmEngine,
                                            const TDesC& aNodeName,
                                            CXcapProtocol& aXcapProtocol )
    {
    CXcapDocumentNode* self = new ( ELeave ) CXcapDocumentNode( aXdmEngine, aXcapProtocol );
    CleanupStack::PushL( self );
    self->BaseConstructL( aNodeName );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXcapDocumentNode::NewL
// 
// ----------------------------------------------------------
//
CXcapDocumentNode* CXcapDocumentNode::NewL( CXdmEngine& aXdmEngine,
                                            CXcapProtocol& aXcapProtocol,
                                            const CXdmDocumentNode& aAnotherNode,
                                            const CXdmDocumentNode& aParentNode )
    {
    CXcapDocumentNode* self = new ( ELeave ) CXcapDocumentNode( aXdmEngine, aXcapProtocol );
    CleanupStack::PushL( self );
    self->BaseConstructL( aAnotherNode.NodeName() );
    self->CopyConstructL( aAnotherNode, aParentNode );
    CleanupStack::Pop();
    return self;
    }
    
// ----------------------------------------------------------
// CXcapDocumentNode::NewL
// 
// ----------------------------------------------------------
//
CXcapDocumentNode* CXcapDocumentNode::NewL( CXdmEngine& aXdmEngine,
                                            const TDesC& aNodeName,
                                            CXcapProtocol& aXcapProtocol,
                                            CXdmDocumentNode* aParentNode )
    {
    CXcapDocumentNode* self = new ( ELeave ) CXcapDocumentNode( aXdmEngine, aXcapProtocol, aParentNode );
    CleanupStack::PushL( self );
    self->BaseConstructL( aNodeName );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXcapDocumentNode::NewL
// 
// ----------------------------------------------------------
//
CXcapDocumentNode* CXcapDocumentNode::NewL( CXdmEngine& aXdmEngine,
                                            const TBool aLeafNode,
                                            const TDesC& aNodeName,
                                            CXcapProtocol& aXcapProtocol,
                                            CXdmDocumentNode* aParentNode  )
    {
    CXcapDocumentNode* self = new ( ELeave ) CXcapDocumentNode( aLeafNode, aXdmEngine, aXcapProtocol, aParentNode );
    CleanupStack::PushL( self );
    self->BaseConstructL( aNodeName );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXdmDocumentNode::NewL
// 
// ----------------------------------------------------------
//
CXcapDocumentNode* CXcapDocumentNode::NewL( CXdmEngine& aXdmEngine,
                                            const TBool aLeafNode,
                                            CXcapProtocol& aXcapProtocol,
                                            CXdmDocumentNode* aParentNode )
    {
    return new ( ELeave ) CXcapDocumentNode( aLeafNode, aXdmEngine, aXcapProtocol, aParentNode );
    }

// ----------------------------------------------------------
// CXcapDocumentNode::CopyConstructL
// 
// ----------------------------------------------------------
//
void CXcapDocumentNode::CopyConstructL( const CXdmDocumentNode& aAnotherNode,
		                                const CXdmDocumentNode& aParentNode )
    {
    CXdmDocumentNode::CopyConstructL( aAnotherNode, aParentNode );
    }
        
// ----------------------------------------------------
// CXcapDocumentNode::~CXcapDocumentNode
// 
// ----------------------------------------------------
//
EXPORT_C CXcapDocumentNode::~CXcapDocumentNode()
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocumentNode::~CXcapDocumentNode()" ) );
    #endif
    delete iPrefix;
    iPrefix = NULL;
    }

// ----------------------------------------------------
// CXcapDocumentNode::ChileNodeL
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CXcapDocumentNode::ChileNodeL()
    {
    return CXcapDocumentNode::NewL( iXdmEngine, EFalse, iXcapProtocol, this );
    }
    
// ----------------------------------------------------
// CXcapDocumentNode::ChileNodeL
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CXcapDocumentNode::ChileNodeL( const TDesC& aChildName )
    {
    return CXcapDocumentNode::NewL( iXdmEngine, EFalse, aChildName, iXcapProtocol, this );
    }

// ----------------------------------------------------
// CXcapDocumentNode::ChileNodeL
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CXcapDocumentNode::ChileNodeL( const CXdmDocumentNode* aAnotherNode  )
    {
    return CXcapDocumentNode::NewL( iXdmEngine, iXcapProtocol, *aAnotherNode, *this );
    }

// ----------------------------------------------------
// CXcapDocumentNode::CreateAttributeL
// 
// ----------------------------------------------------
//
CXdmNodeAttribute* CXcapDocumentNode::AttributeL()
    {
    return CXcapNodeAttribute::NewL( iXdmEngine, this );
    }
    
// ----------------------------------------------------
// CXcapDocumentNode::CreateAttributeL
// 
// ----------------------------------------------------
//
CXdmNodeAttribute* CXcapDocumentNode::AttributeL( const TDesC& aAttributeName )
    {
    return CXcapNodeAttribute::NewL( iXdmEngine, aAttributeName, this );
    }

// ----------------------------------------------------------
// CXcapDocumentNode::RemoveNode
// 
// ----------------------------------------------------------
//
void CXcapDocumentNode::RemoveNode( CXcapDocumentNode* aChileNode )
    {
    #ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXcapDocumentNode::RemoveNode()" ) );
    #endif
    CXdmDocumentNode* remove = NULL;
    TInt index = iChildren.Find( aChileNode );
    __ASSERT_DEBUG( index >= 0, User::Panic( _L( "CXcapDocumentNode" ), 0 ) );
    remove = iChildren[index];
    iChildren.Remove( index );
    delete remove;
    remove = NULL;
    }

// ----------------------------------------------------------
// CXcapDocumentNode::SetPrefix
// 
// ----------------------------------------------------------
//
void CXcapDocumentNode::SetPrefixL( const TDesC8& aPrefix )
    {
    delete iPrefix;
    iPrefix = NULL;
    iPrefix = aPrefix.AllocL();
    }
    
// ----------------------------------------------------------
// CXcapDocumentNode::NamespacePrefix
// 
// ----------------------------------------------------------
//
TPtrC8 CXcapDocumentNode::Prefix() const
    {
    return iPrefix != NULL ? iPrefix->Des() : TPtrC8();
    }


