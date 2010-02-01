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
* Description:   CXmlFormatter
*
*/




// INCLUDES
#include <XdmDocument.h>
#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>

#include "XdmLogWriter.h"
#include "XmlFormatter.h"
#include "XmlParserDefines.h"
#include "XmlParserNodePath.h"
#include "XdmXmlParser.h"

// ----------------------------------------------------------
// CXmlFormatter::CXmlFormatter
// 
// ----------------------------------------------------------
//
CXmlFormatter::CXmlFormatter( CXdmXmlParser& aParserMain ) :
                              iTargetFound( EFalse ),
                              iIsAppended( EFalse ),
                              iParserMain( aParserMain )
    {
    }

// ----------------------------------------------------------
// CXmlFormatter::NewL
// 
// ----------------------------------------------------------
//    
CXmlFormatter* CXmlFormatter::NewL( CXdmXmlParser& aParserMain )
    {
    CXmlFormatter* self = new ( ELeave ) CXmlFormatter( aParserMain );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXmlFormatter::~CXmlFormatter
// 
// ----------------------------------------------------------
//
CXmlFormatter::~CXmlFormatter()
    {        
    }

// ----------------------------------------------------------
// CXmlFormatter::ConstructL
// 
// ----------------------------------------------------------
//        
void CXmlFormatter::ConstructL()
    {
    }

// ---------------------------------------------------------
// CXmlFormatter::Prefix
//
// ---------------------------------------------------------
//
TPtrC8 CXmlFormatter::Prefix( const MXdmNodeInterface& aInterface )
    {
    return aInterface.Prefix();
    }

// ---------------------------------------------------------
// CXdmXmlContentHandler::Reset
//
// ---------------------------------------------------------
//
CXdmNodeAttribute* CXmlFormatter::TargetAttribute() const
    {
    return iTargetAttribute;
    }
    
// ---------------------------------------------------------
// CXmlFormatter::FindElementName
//
// ---------------------------------------------------------
//
TPtrC8 CXmlFormatter::FindElementName( const TDesC8& aElementString )
    {
    TInt index = aElementString.LocateF( 32 );
    if( index > 0 )
        return aElementString.Left( index );
    else return aElementString;
    }

// ---------------------------------------------------------
// CXmlFormatter::ResetFormatBuffer
//
// ---------------------------------------------------------
//
void CXmlFormatter::ResetFormatBuffer()
    {
    iFormBufferPos = 0;
    delete iFormatBuffer;
    iFormatBuffer = NULL;
    delete iNodePath;
    iNodePath = NULL;
    }


// ---------------------------------------------------------
// CXmlFormatter::BufferToStringLC
//
// ---------------------------------------------------------
//
HBufC8* CXmlFormatter::BufferToStringLC()
    {
    HBufC8* ret = HBufC8::NewLC( iFormatBuffer->Size() );
    TPtr8 pointer( ret->Des() );
    iFormatBuffer->Read( 0, pointer, iFormatBuffer->Size() );
    ResetFormatBuffer();
    return ret;
    }
      
// ---------------------------------------------------------
// CXmlFormatter::CheckSubsetPath
//
// ---------------------------------------------------------
//
void CXmlFormatter::CheckSubsetPath( const CXdmDocumentNode& aNode )
    {
    iTargetFound = iNodePath->CheckNextNode( aNode );
    }

// ----------------------------------------------------------
// CXmlFormatter::FormatLC
// 
// ----------------------------------------------------------
//
HBufC8* CXmlFormatter::FormatLC( const TDesC8& aXmlFragment,
                                 const CXdmDocument* aDocument,
                                 const CXdmDocumentNode* aTargetNode )
    {
    iIsAppended = EFalse;
    iTargetFound = EFalse;
    TBool addToRoot = EFalse;
    iFormatBuffer = CBufSeg::NewL( 128 );
    iElementType = aTargetNode->ElementType();
    iDocumentFragment.Set( aXmlFragment );
    iNodePath = CXmlParserNodePath::NewL( iParserMain, iElementType, *aTargetNode, this );
    CXdmDocumentNode* element = aDocument->DocumentRoot();
    CXdmDocumentNode* root = element;
    CheckSubsetPath( *root );
    if( iTargetFound )
        {
        addToRoot = ETrue;
        iIsAppended = ETrue;
        }
    iFormatBuffer->InsertL( iFormBufferPos, KXmldocumentStart );
    iFormBufferPos = iFormBufferPos + KXmldocumentStart().Length();
    FormatNamespaceDeclarationsL( *root, *aDocument );
    TInt count = element->NodeCount();
    if( count > 0 )
        {
        for( TInt i = 0;i < count;i++ )
            {
            element = element->ChileNode( i );
            FormatElementsL( element );
            }
        }
    else FormatElementsL( element );
    if( addToRoot )
        AppendXmlFragmentL();
    TPtrC8 name( root->EightBitNodeNameLC()->Des() );
    TPtrC8 prefix = root->Prefix();
    FormatElementEndL( prefix, name );
    CleanupStack::PopAndDestroy();  //EightBitNodeNameLC()
    HBufC8* ret = BufferToStringLC();
    return ret;
    }

// ---------------------------------------------------------
// CXmlFormatter::FormatElementsL
//
// ---------------------------------------------------------
//
void CXmlFormatter::FormatElementsL( CXdmDocumentNode* aCurrentNode )
    {
    TBool ready = EFalse;
    TBool addNow = EFalse;
    if( !iTargetFound && !iIsAppended )
        {
        CheckSubsetPath( *aCurrentNode );
        addNow = iTargetFound && !iIsAppended;
        }
    TPtrC8 prefix = Prefix( *aCurrentNode );
    TPtr8 pointer( aCurrentNode->ElementDataLC()->Des() );
    FormatElementStartL( prefix, pointer );
    while( !ready )
        {
        if( aCurrentNode->IsLeafNode() )
            FormatLeafElementL( aCurrentNode );
        TInt nodeCount = aCurrentNode->NodeCount();
        if( nodeCount > 0 )
            {
            CXdmDocumentNode* child = NULL;
            for( TInt i = 0;i < nodeCount;i++ )
                {
                child = aCurrentNode->ChileNode( i ); 
                child->IsEmptyNode() && !child->IsLeafNode() ?
                                     FormatEmptyElementL( child ):
                                     FormatElementsL( child );
                }
            ready = ETrue;
            }
        else ready = ETrue;
        }
    if( addNow )
        AppendXmlFragmentL();
    FormatElementEndL( prefix, FindElementName( pointer ) );        
    CleanupStack::PopAndDestroy(); //ElementDataLC()
    }

// ---------------------------------------------------------
// CXmlFormatter::AppendXmlFragmentL
//
// ---------------------------------------------------------
//
void CXmlFormatter::AppendXmlFragmentL()
    {
    iFormatBuffer->InsertL( iFormBufferPos, iDocumentFragment );
    iFormBufferPos = iFormBufferPos + iDocumentFragment.Length();
    iIsAppended = ETrue;        
    }
                             
// ----------------------------------------------------------
// CXmlFormatter::FormatLC
// 
// ----------------------------------------------------------
//
HBufC8* CXmlFormatter::FormatLC( TBool aIsWholeDocument,
                                 const CXdmDocument* aDocument,
                                 const CXdmDocumentNode* aRootNode )
    {
    iIsAppended = ETrue; 
    iFormatBuffer = CBufSeg::NewL( 128 );
    CXdmDocumentNode* root = CONST_CAST( CXdmDocumentNode*, aRootNode );
    if( aIsWholeDocument )
        {
        CXdmDocumentNode* element = NULL;
        iFormatBuffer->InsertL( iFormBufferPos, KXmldocumentStart );
        iFormBufferPos = iFormBufferPos + KXmldocumentStart().Length();
        FormatNamespaceDeclarationsL( *root, *aDocument );
        TInt count = root->NodeCount();
        if( count > 0 )
            {
            for( TInt i = 0;i < count;i++ )
                {
                element = root->ChileNode( i );
                FormatElementsL( element );
                }
            }
        }
    else FormatElementsL( root );
    if( aIsWholeDocument )
        {
        TPtrC8 name( aRootNode->EightBitNodeNameLC()->Des() );
        TPtrC8 prefix = aRootNode->Prefix();
        FormatElementEndL( prefix, name );
        CleanupStack::PopAndDestroy();  //EightBitNodeNameLC()
        }
    HBufC8* ret = BufferToStringLC();
    return ret;
    }

// ----------------------------------------------------------
// CXmlFormatter::FormatNamespaceDeclarationsL
// 
// ----------------------------------------------------------
//
void CXmlFormatter::FormatNamespaceDeclarationsL( const CXdmDocumentNode& aRootNode,
                                                  const MXdmNamespaceContainer& aContainer )
    {
#ifdef _DEBUG
    iParserMain.WriteToLog( _L8( "CXmlFormatter::FormatNamespaceDeclarationsL start" ) );
#endif
    TPtrC8 name( aRootNode.EightBitNodeNameLC()->Des() );
    TPtrC8 prefix = Prefix( aRootNode );
    iFormatBuffer->InsertL( iFormBufferPos, KStartBracketString );
    iFormBufferPos = iFormBufferPos + KStartBracketString().Length();
    if( prefix.Length() > 0 )
        {
        iFormatBuffer->InsertL( iFormBufferPos, prefix );
        iFormBufferPos = iFormBufferPos + prefix.Length();
        iFormatBuffer->InsertL( iFormBufferPos, KPrefixSeparatorString );
        iFormBufferPos = iFormBufferPos + KPrefixSeparatorString().Length();
        }
    iFormatBuffer->InsertL( iFormBufferPos, name );
    iFormBufferPos = iFormBufferPos + name.Length();
    CleanupStack::PopAndDestroy();  //EightBitNodeNameLC()
    if( aContainer.Count() > 0 )
        {
        TPtrC8 uri( _L8( "" ) );
        TPtrC8 prefix( _L8( "" ) );
        for( TInt i = 0;i < aContainer.Count();i++ )
            {
            uri.Set( aContainer.Uri( i ) );
            prefix.Set( aContainer.Prefix( i ) );
            AppendNamespaceL( uri, prefix );
            }
        }

    // go through possible attributes after namespaces
    for ( TInt i = 0; i < aRootNode.AttributeCount(); i++ )
        {
        iFormatBuffer->InsertL( iFormBufferPos, KSpaceString );
        iFormBufferPos = iFormBufferPos + KSpaceString().Length();

        CXdmNodeAttribute* attr = aRootNode.Attribute( i );
        TPtrC8 name( attr->EightBitNodeNameLC()->Des() );
        iFormatBuffer->InsertL( iFormBufferPos, name );
        iFormBufferPos = iFormBufferPos + name.Length();
        CleanupStack::PopAndDestroy();  //EightBitNodeNameLC()
        iFormatBuffer->InsertL( iFormBufferPos, KEquelsSignString );
        iFormBufferPos = iFormBufferPos + KEquelsSignString().Length();
        iFormatBuffer->InsertL( iFormBufferPos, KQuotationSignString );
        iFormBufferPos = iFormBufferPos + KQuotationSignString().Length();
        TPtrC8 attrValue( attr->EightBitValueLC()->Des() );
        iFormatBuffer->InsertL( iFormBufferPos, attrValue );
        iFormBufferPos = iFormBufferPos + attrValue.Length();
        CleanupStack::PopAndDestroy();  //EightBitValueLC()
        iFormatBuffer->InsertL( iFormBufferPos, KQuotationSignString );
        iFormBufferPos = iFormBufferPos + KQuotationSignString().Length();
        }

    iFormatBuffer->InsertL( iFormBufferPos, KEndBracketString );
    iFormBufferPos = iFormBufferPos + KEndBracketString().Length();
    iFormatBuffer->InsertL( iFormBufferPos, KNewlineString );
    iFormBufferPos = iFormBufferPos + KNewlineString().Length();
    #ifdef _DEBUG
        iParserMain.WriteToLog( _L8( "CXmlFormatter::FormatNamespaceDeclarationsL exit" ) );
    #endif
    }

// ---------------------------------------------------------
// CXmlFormatter::FormatLeafElementL
//
// ---------------------------------------------------------
//
void CXmlFormatter::FormatLeafElementL( CXdmDocumentNode* aCurrentNode )
    {
    TPtrC8 escape( aCurrentNode->EscapeLeafNodeContentLC()->Des() );
    iFormatBuffer->InsertL( iFormBufferPos, escape );
    iFormBufferPos = iFormBufferPos + escape.Length();
    CleanupStack::PopAndDestroy();  //EscapeLeafNodeContentLC()
    }

// ---------------------------------------------------------
// CXmlFormatter::FormatEmptyElementL
//
// ---------------------------------------------------------
//
void CXmlFormatter::FormatEmptyElementL( CXdmDocumentNode* aEmptyNode ) 
    {
    TPtr8 empty( aEmptyNode->ElementDataLC()->Des() );
    iFormatBuffer->InsertL( iFormBufferPos, KStartBracketString );
    iFormBufferPos = iFormBufferPos + KStartBracketString().Length();
    TPtrC8 prefix = Prefix( *aEmptyNode );
    if( prefix.Length() > 0 )
        {
        iFormatBuffer->InsertL( iFormBufferPos, prefix );
        iFormBufferPos = iFormBufferPos + prefix.Length();
        iFormatBuffer->InsertL( iFormBufferPos, KPrefixSeparatorString );
        iFormBufferPos = iFormBufferPos + KPrefixSeparatorString().Length();
        }
    iFormatBuffer->InsertL( iFormBufferPos, empty );
    iFormBufferPos = iFormBufferPos + empty.Length();
    CleanupStack::PopAndDestroy();  //ElementDataLC()
    iFormatBuffer->InsertL( iFormBufferPos, KEmptyElementString );
    iFormBufferPos = iFormBufferPos + TPtrC8( KEmptyElementString ).Length();
    }
    
// ---------------------------------------------------------
// CXmlFormatter::FormatElementStartL
//
// ---------------------------------------------------------
//
void CXmlFormatter::FormatElementStartL( const TDesC8& aPrefix,
                                         const TDesC8& aElementData ) 
    {
    iFormatBuffer->InsertL( iFormBufferPos, KStartBracketString );
    iFormBufferPos = iFormBufferPos + KStartBracketString().Length();
    if( aPrefix.Length() > 0 )
        {
        iFormatBuffer->InsertL( iFormBufferPos, aPrefix );
        iFormBufferPos = iFormBufferPos + aPrefix.Length();
        iFormatBuffer->InsertL( iFormBufferPos, KPrefixSeparatorString );
        iFormBufferPos = iFormBufferPos + KPrefixSeparatorString().Length();
        }
    iFormatBuffer->InsertL( iFormBufferPos, aElementData );
    iFormBufferPos = iFormBufferPos + aElementData.Length();
    iFormatBuffer->InsertL( iFormBufferPos, KEndBracketString );
    iFormBufferPos = iFormBufferPos + KEndBracketString().Length();
    }

// ---------------------------------------------------------
// CXmlFormatter::FormatElementEndL
//
// ---------------------------------------------------------
//
void CXmlFormatter::FormatElementEndL( const TDesC8& aPrefix,
                                       const TDesC8& aElementName ) 
    {
    iFormatBuffer->InsertL( iFormBufferPos, KStartBracketString );
    iFormBufferPos = iFormBufferPos + KStartBracketString().Length();
    iFormatBuffer->InsertL( iFormBufferPos, KSlashString );
    iFormBufferPos = iFormBufferPos + KSlashString().Length();
    if( aPrefix.Length() > 0 )
        {
        iFormatBuffer->InsertL( iFormBufferPos, aPrefix );
        iFormBufferPos = iFormBufferPos + aPrefix.Length();
        iFormatBuffer->InsertL( iFormBufferPos, KPrefixSeparatorString );
        iFormBufferPos = iFormBufferPos + KPrefixSeparatorString().Length();
        }
    iFormatBuffer->InsertL( iFormBufferPos, aElementName );
    iFormBufferPos = iFormBufferPos + aElementName.Length();
    iFormatBuffer->InsertL( iFormBufferPos, KEndBracketString );
    iFormBufferPos = iFormBufferPos + KEndBracketString().Length();
    }

// ---------------------------------------------------------
// CXmlFormatter::AppendNamespaceL
//
// ---------------------------------------------------------
//
void CXmlFormatter::AppendNamespaceL( const TDesC8& aUri, const TDesC8& aPrefix )
    {
    iFormatBuffer->InsertL( iFormBufferPos, KXmlNamespaceString );
    iFormBufferPos = iFormBufferPos + KXmlNamespaceString().Length();
    if( aPrefix.Length() > 0 )  //Not the default namespace
        {   
        iFormatBuffer->InsertL( iFormBufferPos, KPrefixSeparatorString );
        iFormBufferPos = iFormBufferPos + KPrefixSeparatorString().Length();
        iFormatBuffer->InsertL( iFormBufferPos, aPrefix );
        iFormBufferPos = iFormBufferPos + aPrefix.Length();
        }
    iFormatBuffer->InsertL( iFormBufferPos, KEquelsSignString );
    iFormBufferPos = iFormBufferPos + KEquelsSignString().Length();
    iFormatBuffer->InsertL( iFormBufferPos, KQuotationSignString );
    iFormBufferPos = iFormBufferPos + KQuotationSignString().Length();
    iFormatBuffer->InsertL( iFormBufferPos, aUri );
    iFormBufferPos = iFormBufferPos + aUri.Length();
    iFormatBuffer->InsertL( iFormBufferPos, KQuotationSignString );
    iFormBufferPos = iFormBufferPos + KQuotationSignString().Length();
    }
    

// End of File
