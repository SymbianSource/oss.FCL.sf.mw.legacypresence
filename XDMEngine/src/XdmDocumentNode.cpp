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
* Description:   XDM Engine document node
*
*/




// INCLUDE FILES
#include "XdmEngine.h"
#include "XdmDocument.h"
#include "XdmNodeFactory.h"
#include "XdmDocumentNode.h"
#include "XdmNodeAttribute.h"

//XML Escape codes
_LIT8( KXmlBracketOpenEsc,                             "&lt;");
_LIT8( KXmlBracketCloseEsc,                            "&gt;");
_LIT8( KXmlAmpersandEsc,                               "&amp;");
      
// ----------------------------------------------------------
// CXdmDocumentNode::CXdmDocumentNode
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmDocumentNode::CXdmDocumentNode( CXdmEngine& aXdmEngine,
                                             MXdmNodeFactory& aNodeFactory ) :
                                             iPosition( KErrNotFound ),
                                             iEmptyNode( EFalse ),
                                             iXdmEngine( aXdmEngine ),
                                             iNodeFactory( aNodeFactory )                                           
    {
    }


// ----------------------------------------------------------
// CXdmDocumentNode::CXdmDocumentNode
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmDocumentNode::CXdmDocumentNode( CXdmEngine& aXdmEngine,
                                             MXdmNodeFactory& aNodeFactory,
                                             CXdmDocumentNode* aParentNode ) :
                                             iEmptyNode( EFalse ),
                                             iXdmEngine( aXdmEngine ),
                                             iNodeFactory( aNodeFactory ),
                                             iParentNode( aParentNode )
                                                
    {   
    }

// ----------------------------------------------------------
// CXdmDocumentNode::CXdmDocumentNode
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmDocumentNode::CXdmDocumentNode( const TBool aLeafNode,
                                             CXdmEngine& aXdmEngine,
                                             MXdmNodeFactory& aNodeFactory,
                                             CXdmDocumentNode* aParentNode ) :
                                             iLeafNode( aLeafNode ),
                                             iEmptyNode( EFalse ),
                                             iXdmEngine( aXdmEngine ),
                                             iNodeFactory( aNodeFactory ),
                                             iParentNode( aParentNode )
                                                
    {
    
    }

// ----------------------------------------------------------
// CXdmDocumentNode::ConstructL
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::BaseConstructL( const TDesC& aNodeName )
    {
    delete iNodeName;
    iNodeName = NULL;
    iNodeName = HBufC::NewL( aNodeName.Length() );
    iNodeName->Des().Copy( aNodeName );
    }

// ----------------------------------------------------------
// CXdmDocumentNode::CopyConstructL
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::CopyConstructL( const CXdmDocumentNode& aAnotherNode,
		                                        const CXdmDocumentNode& aParentNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::CopyConstructL()" ) );
    #endif
    if( aAnotherNode.ElementType() != EXdmElementAttribute )
        {
        ConstructChildrenL( aAnotherNode );
        ConstructAttributesL( aAnotherNode );
        }
    iLeafNode = aAnotherNode.IsLeafNode();
    iEmptyNode = aAnotherNode.IsEmptyNode();
    if( iLeafNode )
        {
        iLeafContent = HBufC8::New( aAnotherNode.LeafNodeContent().Length() );
        iLeafContent->Des().Copy( aAnotherNode.LeafNodeContent() );
        }
    iParentNode = CONST_CAST( CXdmDocumentNode*, &aParentNode ); 
    }

// ----------------------------------------------------------
// CXdmDocumentNode::ConstructAttributesL
// 
// ----------------------------------------------------------
//
void CXdmDocumentNode::ConstructAttributesL( const CXdmDocumentNode& aAnotherNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::CopyConstructL()" ) );
    #endif
    TInt attrCount = aAnotherNode.AttributeCount();
    for( TInt i = 0;i < attrCount;i++ )
        {
        TPtrC name = aAnotherNode.Attribute( i )->NodeName();
        TPtrC value = aAnotherNode.Attribute( i )->AttributeValue();
        CXdmNodeAttribute* attribute = iNodeFactory.AttributeL( name );
        CleanupStack::PushL( attribute );
        attribute->SetAttributeValueL( value );
        User::LeaveIfError( iAttributes.Append( attribute ) );
        CleanupStack::Pop();  //attribute
        }
    }
    
// ----------------------------------------------------------
// CXdmDocumentNode::ConstructChildrenL
// 
// ----------------------------------------------------------
//
void CXdmDocumentNode::ConstructChildrenL( const CXdmDocumentNode& aAnotherNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::ConstructChildrenL()" ) );
    #endif
    TInt childCount = aAnotherNode.NodeCount();
    for( TInt i = 0;i < childCount;i++ )
        {
        CXdmDocumentNode* node = aAnotherNode.ChileNode( i );
        CXdmDocumentNode* copy = iNodeFactory.ChileNodeL( node );
        CleanupStack::PushL( copy );
        User::LeaveIfError( iChildren.Append( copy ) );
        CleanupStack::Pop();  //copy
        }
    }
       
// ----------------------------------------------------
// CXdmDocumentNode::~CXdmDocumentNode
// 
// ----------------------------------------------------
//
EXPORT_C CXdmDocumentNode::~CXdmDocumentNode()
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::~CXdmDocumentNode()" ) );
    #endif
    delete iNextNode;
    iNextNode = NULL;
    delete iLeafContent;
    delete iNodeName;
    iChildren.ResetAndDestroy();
    iChildren.Close();
    iAttributes.ResetAndDestroy();
    iAttributes.Close();
    }

// ----------------------------------------------------
// CXdmDocumentNode::CreateChileNodeL
// 
// ----------------------------------------------------
//
EXPORT_C CXdmDocumentNode* CXdmDocumentNode::CreateChileNodeL()
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::CreateChileNodeL()" ) );
    #endif*/
    __ASSERT_ALWAYS( !iEmptyNode, User::Panic( _L( "CXdmDocumentNode" ), ENodeNotEmpty ) );
    CXdmDocumentNode* node = iNodeFactory.ChileNodeL();
    CleanupStack::PushL( node );
    User::LeaveIfError( iChildren.Append( node ) );
    CleanupStack::Pop();  //node
    return node;
    }
    
// ----------------------------------------------------
// CXdmDocumentNode::CreateChileNodeL
// 
// ----------------------------------------------------
//
EXPORT_C CXdmDocumentNode* CXdmDocumentNode::CreateChileNodeL( const TDesC& aChildName )
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::CreateChileNodeL() - Name: %S" ), &aChildName );
    #endif*/
    __ASSERT_ALWAYS( !iEmptyNode, User::Panic( _L( "CXdmDocumentNode" ), ENodeNotEmpty ) );
    CXdmDocumentNode* node = iNodeFactory.ChileNodeL( aChildName );
    CleanupStack::PushL( node );
    User::LeaveIfError( iChildren.Append( node ) );
    CleanupStack::Pop();  //node
    return node;
    }

// ----------------------------------------------------
// CXdmDocumentNode::SetLeafNode
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::SetLeafNode( const TBool aIsLeafNode )
    {
    iLeafNode = aIsLeafNode;
    }
        
// ----------------------------------------------------
// CXdmDocumentNode::IsLeafNode
// 
// ----------------------------------------------------
//
EXPORT_C TBool CXdmDocumentNode::IsLeafNode() const
    {
    return iLeafNode;
    }

// ----------------------------------------------------
// CXdmDocumentNode::SetEmptyNode
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::SetEmptyNode( const TBool aIsEmptyNode )
    {
    //Do nothing if the state is not being changed
    if( aIsEmptyNode != iEmptyNode )
        {    
        if( aIsEmptyNode )
            {
            if( !iEmptyNode )
                {
                TInt count = iChildren.Count();
                if( count > 0 )
                    iChildren.ResetAndDestroy();
                }
            }
        iEmptyNode = aIsEmptyNode;
        }
    }
        
// ----------------------------------------------------
// CXdmDocumentNode::IsEmptyNode
// 
// ----------------------------------------------------
//
EXPORT_C TBool CXdmDocumentNode::IsEmptyNode() const
    {
    return iEmptyNode;
    }
    
// ----------------------------------------------------
// CXdmDocumentNode::CreateAttributeL
// 
// ----------------------------------------------------
//
EXPORT_C CXdmNodeAttribute* CXdmDocumentNode::CreateAttributeL()
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::CreateAttributeL()" ) );
    #endif*/
    CXdmDocumentNode* node = iNodeFactory.AttributeL();
    CleanupStack::PushL( node );
    User::LeaveIfError( iAttributes.Append( ( CXdmNodeAttribute* )node ) );
    CleanupStack::Pop();  //node
    return ( CXdmNodeAttribute* )node;
    }
    
// ----------------------------------------------------
// CXdmDocumentNode::CreateAttributeL
// 
// ----------------------------------------------------
//
EXPORT_C CXdmNodeAttribute* CXdmDocumentNode::CreateAttributeL( const TDesC& aAttributeName )
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::CreateAttributeL() - Name: %S" ), &aAttributeName );
    #endif*/
    CXdmDocumentNode* node = iNodeFactory.AttributeL( aAttributeName );
    CleanupStack::PushL( node );
    User::LeaveIfError( iAttributes.Append( ( CXdmNodeAttribute* )node ) );
    CleanupStack::Pop();  //node
    return ( CXdmNodeAttribute* )node;
    }
    
// ----------------------------------------------------
// CXdmDocumentNode::SetLeafNodeContentL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::SetLeafNodeContentL( const TDesC& aLeafContent )
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::SetLeafNodeContentL( 16 ) - Content: %S" ),
                                &aLeafContent );
    #endif*/
    if( iLeafNode )
        {
        delete iLeafContent;
        iLeafContent = NULL;
        iLeafContent = CXdmEngine::ConvertToUTF8L( aLeafContent );
        }
    else
        User::Leave( KErrGeneral );
    }

// ----------------------------------------------------
// CXdmDocumentNode::SetLeafNodeContentL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::SetLeafNodeContentL( const TDesC8& aLeafContent )
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::SetLeafNodeContentL( 8 ) - Content: %S" ),
                                &aLeafContent );
    #endif*/
    if( iLeafNode )
        {
        delete iLeafContent;
        iLeafContent = NULL;
        iLeafContent = HBufC8::NewL( aLeafContent.Length() );
        iLeafContent->Des().Copy( aLeafContent );
        }
    else
        User::Leave( KErrGeneral );
    }

// ----------------------------------------------------
// CXdmDocumentNode::ElementData
// 
// ----------------------------------------------------
//
EXPORT_C HBufC8* CXdmDocumentNode::ElementDataLC() const
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::ElementDataLC()" ) );
    #endif*/
    TInt position = 0;
    CBufFlat* buffer = CBufFlat::NewL( 48 );
    CleanupStack::PushL( buffer );
    buffer->InsertL( position, EightBitNodeNameLC()->Des() );
    CleanupStack::PopAndDestroy();   //EightBitNodeNameLC()
    position = position + NodeName().Length();
    TInt attributeCount = iAttributes.Count();
    if( attributeCount > 0 )
        {
        for( TInt i = 0;i < attributeCount;i++ )
            {
            buffer->InsertL( position, KGeneralSpace );
            position = position + KGeneralSpace().Length();
            CXdmNodeAttribute* attribute = iAttributes[i];
            TPtrC8 name( attribute->EightBitNodeNameLC()->Des() );
            buffer->InsertL( position, name );
            position = position + name.Length();
            buffer->InsertL( position, KGeneralEqualsAndQuotes );
            position = position + KGeneralEqualsAndQuotes().Length();
            TPtrC8 value( attribute->EscapedValueLC()->Des() );
            buffer->InsertL( position, value );
            position = position + value.Length();
            buffer->InsertL( position, KGeneralQuotes );
            position = position + KGeneralQuotes().Length();
            CleanupStack::PopAndDestroy( 2 );  //EscapedValueLC(),
                                               //EightBitNodeNameLC()
            }
        }
    HBufC8* ret = HBufC8::NewL( buffer->Size() );
    TPtr8 pointer( ret->Des() );
    buffer->Read( 0, pointer, buffer->Size() );
    CleanupStack::PopAndDestroy();  //buffer
    CleanupStack::PushL( ret );
    return ret;
    }

// ----------------------------------------------------------
// CXdmNodeAttribute::NodeCount
// 
// ----------------------------------------------------------
//
EXPORT_C TInt CXdmDocumentNode::NodeCount() const
    {
    return iNextNode ? 1 : iChildren.Count();
    }

// ----------------------------------------------------------
// CXdmNodeAttribute::NodeCount
// 
// ----------------------------------------------------------
//
EXPORT_C TInt CXdmDocumentNode::AttributeCount() const
    {
    return iAttributes.Count();
    }

// ----------------------------------------------------------
// CXdmDocumentNode::Parent
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmDocumentNode* CXdmDocumentNode::ChileNode( const TInt aPosition ) const
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::ChileNode() - Position: %d" ), aPosition );
    #endif*/
    if( iNextNode )
        return iNextNode;
    else
        {   
        TInt count = NodeCount();
        if( count > 0 && aPosition >= 0 && aPosition < count )
            return iChildren[aPosition];
        else return NULL;
        }
    }

// ----------------------------------------------------------
// CXdmDocumentNode::Attribute
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmNodeAttribute* CXdmDocumentNode::Attribute( const TInt aPosition ) const
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::Attribute() - Position: %d" ), aPosition );
    #endif*/
    TInt count = iAttributes.Count();
    if( count > 0 && aPosition >= 0 && aPosition < count )
        return iAttributes[aPosition];
    else return NULL;
    }

// ----------------------------------------------------------
// CXdmDocumentNode::Attribute
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmNodeAttribute* CXdmDocumentNode::Attribute( const TDesC& aAttributeName ) const
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::Attribute() - Name: %S" ), &aAttributeName );
    #endif*/
    TInt count = iAttributes.Count();
    if( count > 0 )
        {
        TBool found = EFalse;
        CXdmNodeAttribute* target = NULL;
        for( TInt i = 0;!found && i < count;i++ )
            {
            if( iAttributes[i]->NodeName().Compare( aAttributeName ) == 0 )
                {
                target = iAttributes[i];
                found = ETrue;
                }
            }
        return target;
        }
    else return NULL;
    }
    
// ----------------------------------------------------------
// CXdmDocumentNode::Parent
// 
// ----------------------------------------------------------
//
EXPORT_C TPtrC8 CXdmDocumentNode::LeafNodeContent() const
    {
    return iLeafNode && iLeafContent != NULL ? iLeafContent->Des() : TPtrC8();
    }

// ----------------------------------------------------------
// CXdmDocumentNode::Parent
// 
// ----------------------------------------------------------
//
EXPORT_C HBufC8* CXdmDocumentNode::EscapeLeafNodeContentLC() const
    {
    return iLeafNode && iLeafContent != NULL ? EscapeDescLC( iLeafContent->Des() ) : NULL;
    }

// ----------------------------------------------------------
// CXdmDocumentNode::SetNextNode
// 
// ----------------------------------------------------------
//
EXPORT_C TPtrC CXdmDocumentNode::NodeName() const
    {
    return iNodeName != NULL ? iNodeName->Des() : TPtrC();
    }

// ----------------------------------------------------------
// CXdmDocumentNode::SetNextNode
// 
// ----------------------------------------------------------
//
EXPORT_C HBufC8* CXdmDocumentNode::EightBitNodeNameLC() const
    {
    HBufC8* eightBit = CXdmEngine::ConvertToUTF8L( iNodeName->Des() );
    CleanupStack::PushL( eightBit );
    return eightBit;
    }

// ----------------------------------------------------------
// CXdmDocumentNode::SetNextNode
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::SetNameL( const TDesC& aNodeName )
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::SetNameL( 16-Bit ) - Name: %S" ),
                                &aNodeName );
    #endif*/
    delete iNodeName;
    iNodeName = NULL;
    iNodeName = HBufC::NewL( aNodeName.Length() );
    iNodeName->Des().Copy( aNodeName );
    }

// ----------------------------------------------------------
// CXdmDocumentNode::SetNextNode
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::SetNameL( const TDesC8& aNodeName )
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::SetNameL( 8-Bit ) - Name: %S" ),
                                &aNodeName );
    #endif*/
    delete iNodeName;
    iNodeName = NULL;
    iNodeName = HBufC::NewL( aNodeName.Length() );
    iNodeName->Des().Copy( aNodeName );
    }
        
// ----------------------------------------------------------
// CXdmNodeAttribute::ElementType
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmDocumentNode* CXdmDocumentNode::Parent() const
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::Parent()" ) );
    #endif*/
    return iParentNode;
    }

// ----------------------------------------------------------
// CXdmNodeAttribute::ElementType
// 
// ----------------------------------------------------------
//
EXPORT_C TXdmElementType CXdmDocumentNode::ElementType() const
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::ElementType()" ) );
    #endif*/
    return EXdmElementNode;
    }

// ----------------------------------------------------------
// CXdmDocumentNode::Match
// 
// ----------------------------------------------------------
//
EXPORT_C TBool CXdmDocumentNode::Match( const CXdmDocumentNode& aAnotherNode ) const
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::Match()" ) );
    #endif
    if( NodeName().Compare( aAnotherNode.NodeName() ) == 0 )
        {
        TBool match = ETrue;
        TInt attrCount = iAttributes.Count();
        if( attrCount == aAnotherNode.AttributeCount() )
            {
            for( TInt i = 0;match && i < attrCount;i++ )
                match = HasAttribute( *aAnotherNode.Attribute( i ) );
            }
        else match = EFalse;
        return match;
        }
    else return EFalse;
    }
    
// ----------------------------------------------------------
// CXdmDocumentNode::Match
// 
// ----------------------------------------------------------
//
EXPORT_C TBool CXdmDocumentNode::Match( const TDesC& aNodeName,
                                        const RPointerArray<SXdmAttribute8>&
                                        aAttributeArray ) const
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::Match()" ) );
    #endif
    if( NodeName().Compare( aNodeName ) == 0 )
        {
        TBool match = ETrue;
        TInt attrCount = iAttributes.Count();
        if( attrCount == aAttributeArray.Count() )
            {
            for( TInt i = 0;match && i < attrCount;i++ )
                match = HasAttributeL( *aAttributeArray[i] );
            }
        else match = EFalse;
        return match;
        }
    else return EFalse;
    }

// ---------------------------------------------------------
// CXdmDocumentNode::HasAttribute
// 
// ---------------------------------------------------------
//
EXPORT_C TBool CXdmDocumentNode::HasAttribute( const TDesC& aAttributeName ) const
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::HasAttribute()" ) );
    #endif
    TInt count = iAttributes.Count();
    for( TInt i = 0; i < count;i++ )
        {
        if( !iAttributes[i]->NodeName().Compare( aAttributeName ) )
        	return ETrue;
        }
    return EFalse;
    }  

// ---------------------------------------------------------
// CXdmDocumentNode::Print
// !!! For debugging ONLY !!!
// ---------------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::Print()
	{
    iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::Print()" ) );
	HBufC8* nameBuf = HBufC8::NewLC( iNodeName->Des().Length() );
	TPtr8 name( nameBuf->Des() );
	name.Copy( iNodeName->Des() );
	if( ElementType() == EXdmElementAttribute )
	    {
	    CXdmNodeAttribute* attr = ( CXdmNodeAttribute* )this;
	    TPtrC8 value( attr->EightBitValueLC()->Des() );
	    iXdmEngine.WriteToLog( _L8( "* Name: %S  Value: %S" ), &name, &value );
	    CleanupStack::PopAndDestroy();  //value
	    }
	else
    	{
	   	iXdmEngine.WriteToLog( _L8( "* Name: %S" ), &name );
	    TInt count = iAttributes.Count();
        if( count > 0 )
            {
            for( TInt i = 0;i < count;i++ )
                {
                CXdmNodeAttribute* attr = iAttributes[i];
                TPtrC8 value( attr->EightBitValueLC()->Des() );
                TPtrC8 name( attr->EightBitNodeNameLC()->Des() );
                iXdmEngine.WriteToLog( _L8( "* Attribute %d - Name: %S  Value: %S" ), i, &name, &value );
                CleanupStack::PopAndDestroy( 2 );  //name, value
                }
        
            }
        if( iLeafNode )
            {
            TPtr8 data( iLeafContent->Des() );
            iXdmEngine.WriteToLog( _L8( "* Data: %S" ), &data );
            }
        CXdmDocumentNode* node = NULL;
        TInt nodeCount = NodeCount();
        for( TInt i = 0;i < nodeCount;i++ )
            {
            node = ChileNode( i );
            node->Print();
            }
	    }
    CleanupStack::PopAndDestroy();  //nameBuf
	}

// ---------------------------------------------------------
// CXdmDocumentNode::Find
// 
// ---------------------------------------------------------
//
EXPORT_C TInt CXdmDocumentNode::Find( const TDesC& aNodeName,
                                      RPointerArray<CXdmDocumentNode>& aResultArray ) const
    {
    TInt error = KErrNotFound;
    TInt childCount = NodeCount();
    if( childCount > 0 )
        {
        for( TInt i = 0;i < childCount;i++ )
            error = ChileNode( i )->DoFind( aNodeName, aResultArray );
        }
    return error;
    }

// ---------------------------------------------------------
// CXdmDocumentNode::DoFind
// 
// ---------------------------------------------------------
//
TInt CXdmDocumentNode::DoFind( const TDesC& aNodeName,
                               RPointerArray<CXdmDocumentNode>& aResultArray ) const
    {
    TInt error = KErrNone;
    if( NodeName().Compare( aNodeName ) == 0 )
        error = aResultArray.Append( this );
    if( error == KErrNone )
        {
        TInt childCount = NodeCount();
        if( childCount > 0 )
            {
            for( TInt i = 0;i < childCount;i++ )
                error = ChileNode( i )->DoFind( aNodeName, aResultArray );
            }
        }
    return error;
    }
        
// ---------------------------------------------------------
// CXdmDocumentNode::Find
// 
// ---------------------------------------------------------
//
EXPORT_C TInt CXdmDocumentNode::Find( const CXdmDocumentNode& aTargetNode,
                                      RPointerArray<CXdmDocumentNode>& aResultArray ) const
    {
    TInt error = KErrNotFound;
    TInt childCount = NodeCount();
    if( childCount > 0 )
        {
        for( TInt i = 0;i < childCount;i++ )
            error = ChileNode( i )->DoFind( aTargetNode, aResultArray );
        }
    return error;                        
    }

// ---------------------------------------------------------
// CXdmDocumentNode::Find
// 
// ---------------------------------------------------------
//
TInt CXdmDocumentNode::DoFind( const CXdmDocumentNode& aTargetNode,
                               RPointerArray<CXdmDocumentNode>& aResultArray ) const
    {
    TInt error = KErrNone;
    if( *this == aTargetNode )
        error = aResultArray.Append( this );
    if( error == KErrNone )
        {
        TInt childCount = NodeCount();
        if( childCount > 0 )
            {
            for( TInt i = 0;i < childCount;i++ )
                error = ChileNode( i )->DoFind( aTargetNode, aResultArray );
            }
        }
    return error;                        
    }
       
// ---------------------------------------------------------
// CXdmDocumentNode::Find
// 
// ---------------------------------------------------------
//
EXPORT_C TInt CXdmDocumentNode::Find( const TDesC& aNodeName,
                                      RPointerArray<CXdmDocumentNode>& aResultArray,
                                      const RPointerArray<SXdmAttribute16>& aAttributeArray ) const
    {
    TInt error = KErrNone;
    TInt childCount = NodeCount();
    if( childCount > 0 )
        {
        for( TInt j = 0;j < childCount;j++ )
            error = ChileNode( j )->DoFind( aNodeName, aResultArray, aAttributeArray );
        }
    return error;
    }

// ---------------------------------------------------------
// CXcapHttpOperation::RemoveNamespaceAttributes
//
// ---------------------------------------------------------
//
void CXdmDocumentNode::RemoveNamespaceAttributes()
    {
    _LIT( KXmlNamespaceAttr, "xmlns" );
    for( TInt i( iAttributes.Count() - 1 ); i > KErrNotFound; i--  )
        {
        CXdmNodeAttribute* attr( iAttributes[i] );
        if( attr->NodeName().Find( KXmlNamespaceAttr ) == 0 )
            {
            iAttributes.Remove( i );
            delete attr;
            attr = NULL;
            }
        }
    }

// ---------------------------------------------------------
// CXdmDocumentNode::DoFind
// 
// ---------------------------------------------------------
//
TInt CXdmDocumentNode::DoFind( const TDesC& aNodeName,
                               RPointerArray<CXdmDocumentNode>& aResultArray,
                               const RPointerArray<SXdmAttribute16>& aAttributeArray )
    {
    TBool match = EFalse;
    TInt error = KErrNone;
    if( NodeName().Compare( aNodeName ) == 0 )
        {
        match = ETrue;
        RemoveNamespaceAttributes();
        TInt attrCount = iAttributes.Count();
        if( attrCount == aAttributeArray.Count() )
            {
            if( attrCount > 0 )
                {
                for( TInt i = 0;i < attrCount;i++ )
                    match = HasAttribute( *aAttributeArray[i] );
                }
            }
        else match = EFalse;
        error = match ? aResultArray.Append( this ) : KErrNotFound;
        }
    TInt childCount = NodeCount();
    if( childCount > 0 )
        {
        for( TInt j = 0;j < childCount;j++ )
            error = ChileNode( j )->DoFind( aNodeName, aResultArray, aAttributeArray );
        }
    return error;
    }
   
// ---------------------------------------------------------
// CXdmDocumentNode::operator==
// 
// ---------------------------------------------------------
//
EXPORT_C TBool CXdmDocumentNode::operator==( const CXdmDocumentNode& aNode ) const
    {
    TBool match = EFalse;
    if( NodeName().Compare( aNode.NodeName() ) == 0 )
        {
        match = ETrue;
        TInt attrCount = iAttributes.Count();
        if( attrCount == aNode.AttributeCount() )
            {
            if( attrCount > 0 )
                {
                for( TInt i = 0;i < attrCount;i++ )
                    match = aNode.HasAttribute( *iAttributes[i] );
                }
            }
        if( match )
            {
            TInt childCount = NodeCount();
            if( childCount == aNode.NodeCount() )
                {   
                if( childCount > 0 )
                    {
                    for( TInt j = 0;j < childCount;j++ )
                        {
                        match = *ChileNode( j ) == *aNode.ChileNode( j );
                        if( !match )
                            break;
                        }
                    } 
                }
            else match = EFalse; 
            }
        else match = EFalse; 
        }
    return match;
    }

// ----------------------------------------------------
// CXdmDocumentNode::AppendChileNodeL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::AppendChileNodeL( CXdmDocumentNode* aDocumentNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CLocalDocument::AppendChileNodeL()" ) );
    #endif
    CXdmDocumentNode* node = iNodeFactory.ChileNodeL( aDocumentNode );
    CleanupStack::PushL( node );
    User::LeaveIfError( iChildren.Append( node ) );
    CleanupStack::Pop();  //node
    }

// ----------------------------------------------------
// CXdmDocumentNode::InsertChileNodeL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::InsertChileNodeL( TInt aIndex, CXdmDocumentNode* aDocumentNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CLocalDocument::InsertChileNodeL()" ) );
    #endif
    __ASSERT_ALWAYS( aIndex >= 0, User::Panic( _L( "CXdmDocumentNode" ), EIndexOutOfBounds ) );
    CXdmDocumentNode* node = iNodeFactory.ChileNodeL( aDocumentNode );
    CleanupStack::PushL( node );
    User::LeaveIfError( iChildren.Insert( node, aIndex ) );
    CleanupStack::Pop();  //node
    }
    
// ----------------------------------------------------
// CXdmDocumentNode::ReplaceChileNodeL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::ReplaceChileNodeL( CXdmDocumentNode* aNewNode,
                                                   CXdmDocumentNode* aTargetNode )
    {
    CXdmDocumentNode* remove = NULL;
    TInt index = iChildren.Find( aTargetNode );
    #ifdef _DEBUG
        CXdmDocumentNode* a = NULL;
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::ReplaceChileNodeL()" ) );
        iXdmEngine.WriteToLog( _L8( "  New node:    %x" ), aNewNode );
        iXdmEngine.WriteToLog( _L8( "  Target node: %x" ), aTargetNode );
        for( TInt i = 0;i < iChildren.Count();i++ )
            {
            a = iChildren[i];
            iXdmEngine.WriteToLog( _L8( "  Child %d: %x" ), i, a );
            }
    #endif
    __ASSERT_DEBUG( index >= 0, User::Panic( _L( "CXdmDocumentNode" ), EIndexOutOfBounds ) );
    remove = iChildren[index];
    iChildren.Remove( index );
    delete remove;
    remove = NULL;
    CXdmDocumentNode* node = iNodeFactory.ChileNodeL( aNewNode );
    CleanupStack::PushL( node );
    User::LeaveIfError( iChildren.Insert( node, index ) );
    CleanupStack::Pop();  //node
    }

// ----------------------------------------------------------
// CXdmDocumentNode::RemoveChileNodeL
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::RemoveChileNodeL( CXdmDocumentNode* aChileNode )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::RemoveChileNodeL()" ) );
    #endif
    CXdmDocumentNode* remove = NULL;
    switch( aChileNode->ElementType() )
        {
        case EXdmElementNode:
            {
            #ifdef _DEBUG
                iXdmEngine.WriteToLog( _L8( "  Element is a node" ) );
            #endif
            TInt index( iChildren.Find( aChileNode ) );
            __ASSERT_DEBUG( index >= 0, User::Panic( _L( "CXdmDocumentNode" ), EIndexOutOfBounds ) );
            remove = iChildren[index];
            iChildren.Remove( index );
            delete remove;
            remove = NULL;
            }
            break;
        case EXdmElementAttribute:
            {
            #ifdef _DEBUG
                iXdmEngine.WriteToLog( _L8( "  Element is an attribute" ) );
            #endif
            TInt index( iAttributes.Find( ( CXdmNodeAttribute* )aChileNode ) );
            __ASSERT_DEBUG( index >= 0, User::Panic( _L( "CXdmDocumentNode" ), EIndexOutOfBounds ) );
            remove = iAttributes[index];
            iAttributes.Remove( index );
            delete remove;
            remove = NULL;
            }
            break;
        default: 
            #ifdef _DEBUG
                iXdmEngine.WriteToLog( _L8( "  Element is of undefined type, ignore" ) );
            #endif
            break;
        }
    }

// ----------------------------------------------------------
// CXdmDocumentNode::NextNode
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmDocumentNode* CXdmDocumentNode::NextNode() const
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXcapDocumentNode::NextNode()" ) );
    #endif*/
    return iNextNode;
    }

// ----------------------------------------------------------
// CXcapDocumentNode::SetNextNode
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmDocumentNode::SetNextNode( CXdmDocumentNode* aNextNode )
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::SetNextNode()" ) );
    #endif*/
    __ASSERT_DEBUG( aNextNode != NULL, User::Panic( _L( "CXdmDocumentNode" ), EDocNodeNull ) );
    iNextNode = aNextNode;
    }
    
// ---------------------------------------------------------
// CXdmDocumentNode::EscapeDescLC
//
// ---------------------------------------------------------
//
HBufC8* CXdmDocumentNode::EscapeDescLC( const TDesC8& aDescriptor ) const
    {
    /*#ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDocumentNode::EscapeDescLC()" ) );
    #endif*/
    TInt bufPos = 0;
    TBuf8<10> format;
    HBufC8* ret = NULL;
    CBufFlat* buffer = CBufFlat::NewL( 50 );
    CleanupStack::PushL( buffer );
    for( TInt i = 0;i < aDescriptor.Length();i++ )
        {
        TUint8 byte = aDescriptor[i];
        switch( byte )
            {
            case 0x26:                  //'&'
                format.Append( KXmlAmpersandEsc );
                break;
            case 0x3C:                  //'<'
                format.Append( KXmlBracketOpenEsc );
                break;
            case 0x3E:                  //'>'
                format.Append( KXmlBracketCloseEsc );
                break;
            default:
                format.Append( byte );
                break;
            }
        buffer->InsertL( bufPos, format );
        bufPos = bufPos + format.Length();
        format.Zero();
        }
    ret = HBufC8::NewL( buffer->Size() );
    ret->Des().Copy( buffer->BackPtr( buffer->Size() ) );
    CleanupStack::PopAndDestroy();  //buffer
    CleanupStack::PushL( ret );
    return ret;
    }
           
// ---------------------------------------------------------
// CXdmDocumentNode::HasAttribute
// 
// ---------------------------------------------------------
//
TBool CXdmDocumentNode::HasAttribute( const CXdmNodeAttribute& aAttribute ) const
    {
    TBool ret = EFalse;
    TInt count = iAttributes.Count();
    for( TInt i = 0;i < count && !ret;i++ )
        {
        ret = iAttributes[i]->NodeName().Compare( aAttribute.NodeName() ) == 0 &&
              iAttributes[i]->AttributeValue().Compare( aAttribute.AttributeValue() ) == 0;
        }
    return ret;
    }

// ---------------------------------------------------------
// CXdmDocumentNode::HasAttribute
// 
// ---------------------------------------------------------
//
TBool CXdmDocumentNode::HasAttributeL( const SXdmAttribute8& aAttribute ) const
    {
    TBool ret = EFalse;
    TInt count = iAttributes.Count();
    for( TInt i = 0;i < count && !ret;i++ )
        {
        HBufC8* name = CXdmEngine::ConvertToUTF8L( iAttributes[i]->NodeName() );
        CleanupStack::PushL( name );
        HBufC8* value = CXdmEngine::ConvertToUTF8L( iAttributes[i]->AttributeValue() );
        CleanupStack::PushL( value );
        ret = name->Des().Compare( aAttribute.iName ) == 0 &&
              value->Des().Compare( aAttribute.iValue ) == 0;
        CleanupStack::PopAndDestroy( 2 );  //value, name
        }
    return ret;
    }  
       
// ---------------------------------------------------------
// CXdmDocumentNode::HasAttribute
// 
// ---------------------------------------------------------
//
TBool CXdmDocumentNode::HasAttribute( const SXdmAttribute16& aAttribute ) const
    {
    TBool ret = EFalse;
    TInt count = iAttributes.Count();
    for( TInt i = 0;i < count && !ret;i++ )
        {
        ret = iAttributes[i]->NodeName().Compare( aAttribute.iName ) == 0 &&
              iAttributes[i]->AttributeValue().Compare( aAttribute.iValue ) == 0;
        }
    return ret;
    }  

