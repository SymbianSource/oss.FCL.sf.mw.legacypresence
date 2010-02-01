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
* Description: CLocalDocumentNode
*
*/




// INCLUDE FILES
#include "XdmDocument.h"
#include "LocalProtocol.h"
#include "LocalDocumentNode.h"
#include "LocalNodeAttribute.h"


// ----------------------------------------------------------
// CLocalDocumentNode::CLocalDocumentNode
// 
// ----------------------------------------------------------
//
CLocalDocumentNode::CLocalDocumentNode( CXdmEngine& aXdmEngine,
                                        CLocalProtocol& aLocalProtocol ) :
                                        CXdmDocumentNode( aXdmEngine, ( MXdmNodeFactory& )*this ),
                                        iLocalProtocol( aLocalProtocol )            
    {
    }


// ----------------------------------------------------------
// CLocalDocumentNode::CLocalDocumentNode
// 
// ----------------------------------------------------------
//
CLocalDocumentNode::CLocalDocumentNode( CXdmEngine& aXdmEngine,
                                        CXdmDocumentNode* aParentNode,
                                        CLocalProtocol& aLocalProtocol ) :
                                        CXdmDocumentNode( aXdmEngine,
                                      ( MXdmNodeFactory& )*this, aParentNode ),
                                        iLocalProtocol( aLocalProtocol )
                                                
    {   
    }

// ----------------------------------------------------------
// CLocalDocumentNode::CLocalDocumentNode
// 
// ----------------------------------------------------------
//
CLocalDocumentNode::CLocalDocumentNode( const TBool aLeafNode,
                                        CXdmEngine& aXdmEngine,
                                        CXdmDocumentNode* aParentNode,
                                        CLocalProtocol& aLocalProtocol ) :
                                        CXdmDocumentNode( aLeafNode, aXdmEngine,
                                      ( MXdmNodeFactory& )*this, aParentNode ),
                                        iLocalProtocol( aLocalProtocol )
    {
    
    }

// ----------------------------------------------------------
// CLocalDocumentNode::NewL
// 
// ----------------------------------------------------------
//
CLocalDocumentNode* CLocalDocumentNode::NewL( CXdmEngine& aXdmEngine,
                                              CLocalProtocol& aLocalProtocol )
    {
    return new ( ELeave ) CLocalDocumentNode( aXdmEngine, aLocalProtocol );
    }

// ----------------------------------------------------------
// CLocalDocumentNode::NewL
// 
// ----------------------------------------------------------
//
CLocalDocumentNode* CLocalDocumentNode::NewL( CXdmEngine& aXdmEngine,
                                              const TDesC& aNodeName,
                                              CLocalProtocol& aLocalProtocol )
    {
    CLocalDocumentNode* self = new ( ELeave ) CLocalDocumentNode( aXdmEngine, aLocalProtocol );
    CleanupStack::PushL( self );
    self->BaseConstructL( aNodeName );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CLocalDocumentNode::NewL
// 
// ----------------------------------------------------------
//
CLocalDocumentNode* CLocalDocumentNode::NewL( CXdmEngine& aXdmEngine,
                                              CLocalProtocol& aLocalProtocol,
                                              const CXdmDocumentNode& aAnotherNode,
                                              const CXdmDocumentNode& aParentNode )
    {
    CLocalDocumentNode* self = new ( ELeave ) CLocalDocumentNode( aXdmEngine, aLocalProtocol );
    CleanupStack::PushL( self );
    self->BaseConstructL( aAnotherNode.NodeName() );
    self->CopyConstructL( aAnotherNode, aParentNode );
    CleanupStack::Pop();
    return self;
    }
    
// ----------------------------------------------------------
// CLocalDocumentNode::NewL
// 
// ----------------------------------------------------------
//
CLocalDocumentNode* CLocalDocumentNode::NewL( CXdmEngine& aXdmEngine,
                                              const TDesC& aNodeName,
                                              CXdmDocumentNode* aParentNode,
                                              CLocalProtocol& aLocalProtocol )
    {
    CLocalDocumentNode* self = new ( ELeave ) CLocalDocumentNode( aXdmEngine, aParentNode,
                                                                  aLocalProtocol );
    CleanupStack::PushL( self );
    self->BaseConstructL( aNodeName );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CLocalDocumentNode::NewL
// 
// ----------------------------------------------------------
//
CLocalDocumentNode* CLocalDocumentNode::NewL( const TBool aLeafNode,
                                              CXdmEngine& aXdmEngine,
                                              const TDesC& aNodeName,
                                              CXdmDocumentNode* aParentNode,
                                              CLocalProtocol& aLocalProtocol )
    {
    CLocalDocumentNode* self = new ( ELeave ) CLocalDocumentNode( aLeafNode, aXdmEngine,
                                                                  aParentNode, aLocalProtocol );
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
CLocalDocumentNode* CLocalDocumentNode::NewL( const TBool aLeafNode,
                                              CXdmEngine& aXdmEngine,
                                              CXdmDocumentNode* aParentNode,
                                              CLocalProtocol& aLocalProtocol )
    {
    return new ( ELeave ) CLocalDocumentNode( aLeafNode, aXdmEngine,
                                              aParentNode, aLocalProtocol );
    }

// ----------------------------------------------------------
// CLocalDocumentNode::CopyConstructL
// 
// ----------------------------------------------------------
//
void CLocalDocumentNode::CopyConstructL(  const CXdmDocumentNode& aAnotherNode,
                                          const CXdmDocumentNode& aParentNode  )
    {
    CXdmDocumentNode::CopyConstructL( aAnotherNode, aParentNode );
    //TODO: Write the protocol specific copy operations
    }
    
// ----------------------------------------------------
// CLocalDocumentNode::~CLocalDocumentNode
// 
// ----------------------------------------------------
//
EXPORT_C CLocalDocumentNode::~CLocalDocumentNode()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocumentNode::~CLocalDocumentNode(): %x" ), this );
    #endif
    delete iPrefix;
    delete iNextNode;
    }

// ----------------------------------------------------
// CLocalDocumentNode::RemoveLocalL
// 
// ----------------------------------------------------
//
EXPORT_C void CLocalDocumentNode::RemoveLocalL( CXdmDocumentNode* aChileNode )
    {
    RemoveChileNodeL( aChileNode );
    }
  
// ----------------------------------------------------
// CLocalDocumentNode::AppendLocalL
// 
// ----------------------------------------------------
//      
EXPORT_C void CLocalDocumentNode::AppendLocalL( CXdmDocumentNode* aChileNode )
    {
    if( aChileNode->Match( *this ) )
        {
        const TInt count = aChileNode->NodeCount();
        for( TInt i = 0;i < count;i++ )
            AppendChileNodeL( aChileNode->ChileNode( i ) );
        }
    else AppendChileNodeL( aChileNode );
    }

// ----------------------------------------------------
// CLocalDocumentNode::ReplaceLocalL
// 
// ----------------------------------------------------
//      
EXPORT_C void CLocalDocumentNode::ReplaceLocalL( CXdmDocumentNode* aNewNode,
                                                 CXdmDocumentNode* aTargetNode )
    {
    ReplaceChileNodeL( aNewNode, aTargetNode );                                            
    }
                                            
// ----------------------------------------------------
// CLocalDocumentNode::CreateChileNodeL
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CLocalDocumentNode::ChileNodeL()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocumentNode::CreateChileNodeL()" ) );
    #endif
    return CLocalDocumentNode::NewL( EFalse, iXdmEngine, this, iLocalProtocol );
    }
    
// ----------------------------------------------------
// CLocalDocumentNode::CreateChileNodeL
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CLocalDocumentNode::ChileNodeL( const TDesC& aChildName )
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocumentNode::CreateChileNodeL() - Name: %S" ), &aChildName );
    #endif
    return CLocalDocumentNode::NewL( EFalse, iXdmEngine, aChildName, this, iLocalProtocol );
    }

// ----------------------------------------------------
// CLocalDocumentNode::CreateChileNodeL
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CLocalDocumentNode::ChileNodeL( const CXdmDocumentNode* aAnotherNode )
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocumentNode::ChileNodeL() - Object: %x" ), aAnotherNode );
    #endif
    return CLocalDocumentNode::NewL( iXdmEngine, iLocalProtocol, *aAnotherNode, *this );
    }

// ----------------------------------------------------
// CLocalDocumentNode::CreateAttributeL
// 
// ----------------------------------------------------
//
CXdmNodeAttribute* CLocalDocumentNode::AttributeL()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocumentNode::CreateAttributeL()" ) );
    #endif
    return CLocalNodeAttribute::NewL( iXdmEngine, this, iLocalProtocol );
    }
    
// ----------------------------------------------------
// CLocalDocumentNode::CreateAttributeL
// 
// ----------------------------------------------------
//
CXdmNodeAttribute* CLocalDocumentNode::AttributeL( const TDesC& aAttributeName )
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocumentNode::CreateAttributeL() - Name: %S" ), &aAttributeName );
    #endif
    return CLocalNodeAttribute::NewL( iXdmEngine, aAttributeName, this, iLocalProtocol );
    }

// ----------------------------------------------------------
// CLocalDocumentNode::SetParentNode
// 
// ----------------------------------------------------------
//
void CLocalDocumentNode::SetParentNode( CLocalDocumentNode*& aParentNode )
    {
    iParentNode = aParentNode;
    }

// ----------------------------------------------------------
// CLocalDocumentNode::NamespacePrefix
// 
// ----------------------------------------------------------
//
TPtrC8 CLocalDocumentNode::Prefix() const
    {
    return iPrefix != NULL ? iPrefix->Des() : TPtrC8();
    }
        
// ----------------------------------------------------------
// CLocalDocumentNode::SetNamespacePrefixL
// 
// ----------------------------------------------------------
//
void CLocalDocumentNode::SetPrefixL( const TDesC8& aPrefix )
    {
    delete iPrefix;
    iPrefix = NULL;
    iPrefix = HBufC8::NewL( aPrefix.Length() );
    iPrefix->Des().Copy( aPrefix );
    }
                       



