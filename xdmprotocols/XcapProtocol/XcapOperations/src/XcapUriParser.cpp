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
* Description:   CXcapUriParser
*
*/




// INCLUDES
#include <f32file.h>
#include "XdmNamespace.h"
#include "XcapUriParser.h"
#include "XcapHttpOperation.h"
#include "XcapEngineDefines.h"
#include "XdmNodeAttribute.h"
#include "XcapOperationFactory.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CXcapUriParser::CXcapUriParser( CXdmDocument& aTargetDoc,
                                CXcapOperationFactory& aOperationFactory ) :
                                iTargetDoc( aTargetDoc ),
                                iNodeType( EXdmElementUnspecified ),
                                iOperationFactory( aOperationFactory )
    {
    }

// ---------------------------------------------------------
// Two-phased constructor.
//
// ---------------------------------------------------------
//
CXcapUriParser* CXcapUriParser::NewL( CXdmDocument& aTargetDoc,
                                      CXcapOperationFactory& aOperationFactory )
    {
    CXcapUriParser* self = new ( ELeave ) CXcapUriParser( aTargetDoc, aOperationFactory );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// Destructor
//
// ---------------------------------------------------------
//
CXcapUriParser::~CXcapUriParser()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapUriParser::~CXcapUriParser()" ) );  
    #endif
    delete iUriBuffer;
    delete iCompleteUri;
    iNamespaceMappings.ResetAndDestroy();
    iNamespaceMappings.Close();
    }
    
// ---------------------------------------------------------
// CXcapUriParser::ConstructL
//
// ---------------------------------------------------------
//
void CXcapUriParser::ConstructL()
    {
    }

// ---------------------------------------------------------
// CXcapUriParser::AddNamespaceMappingL
//
// ---------------------------------------------------------
//
void CXcapUriParser::AddNamespaceMappingL( const TDesC8& aUri, const TDesC8& aPrefix )
    {
    CXdmNamespace* ns = CXdmNamespace::NewL( aUri, aPrefix );
    CleanupStack::PushL( ns );
    User::LeaveIfError( iNamespaceMappings.Append( ns ) );
    CleanupStack::Pop();  //ns
    }

// ---------------------------------------------------------
// CXcapUriParser::AppendNamespaceMappingsL
//
// ---------------------------------------------------------
//
void CXcapUriParser::AppendNamespaceMappingsL()
    {
    CXdmNamespace* ns = NULL;
    const TInt count = iNamespaceMappings.Count();
    iUriBuffer->InsertL( iBufferPosition, KXcapUriQMark );
    iBufferPosition = iBufferPosition + KXcapUriQMark().Length();
    for( TInt i = 0;i < count;i++ )
        {
        ns = iNamespaceMappings[i];
        iUriBuffer->InsertL( iBufferPosition, KXcapUriXmlns );
        iBufferPosition = iBufferPosition + KXcapUriXmlns().Length();
        TPtrC8 prefix( ns->Prefix() );
        iUriBuffer->InsertL( iBufferPosition, prefix );
        iBufferPosition = iBufferPosition + prefix.Length();
        iUriBuffer->InsertL( iBufferPosition, KXcapUriEquals );
        iBufferPosition = iBufferPosition + KXcapUriEquals().Length();
        TPtrC8 uri( ns->Uri() );
        iUriBuffer->InsertL( iBufferPosition, uri );
        iBufferPosition = iBufferPosition + uri.Length();
        iUriBuffer->InsertL( iBufferPosition, KXcapUriQuote );
        iBufferPosition = iBufferPosition + KXcapUriQuote().Length();
        iUriBuffer->InsertL( iBufferPosition, KXcapParenthClose );
        iBufferPosition = iBufferPosition + KXcapParenthClose().Length();
        }
    }
    
// ---------------------------------------------------------
// CXcapUriParser::FinaliseL
//
// ---------------------------------------------------------
//
void CXcapUriParser::FinaliseL()
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapUriParser::FinaliseL()" ) );  
    #endif
    delete iCompleteUri;
    iCompleteUri = NULL;
    iCompleteUri = HBufC8::NewL( iUriBuffer->Size() );
    TPtr8 pointer( iCompleteUri->Des() );
    iUriBuffer->Read( 0, pointer, iUriBuffer->Size() );
    iUriBuffer->Reset();
    delete iUriBuffer;
    iUriBuffer = NULL;
    }

// ---------------------------------------------------------
// CXcapUriParser::ParseL
//
// ---------------------------------------------------------
//
void CXcapUriParser::ParseL( const TPtrC8& aHttpUri )
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapUriParser::ParseL()" ) );  
    #endif
    iHttpUri = CONST_CAST( TPtrC8*, &aHttpUri );
    if( iNodeType != EXdmElementUnspecified )
        {
        HBufC8* path = DocSubsetPathL();
        CleanupStack::PushL( path );
        iBufferPosition = CreateBasicNodeUriL( path );
        CleanupStack::PopAndDestroy();  //path
        iNodeType == EXdmElementAttribute ? FormatAttributeUriL() : FormatNodeUriL();
        if( iNamespaceMappings.Count() > 0 )
            AppendNamespaceMappingsL();
        FinaliseL();
        #ifdef _DEBUG
            TPtr8 desc( iCompleteUri->Des() );
            iOperationFactory.WriteToLog( _L8( " Context: %S" ), &desc );  
        #endif
        }
    else
        {
        #ifdef _DEBUG
            iOperationFactory.WriteToLog( _L8( " Context is a whole document" ) );  
        #endif
        iCompleteUri = HBufC8::NewL( iHttpUri->Length() );
        iCompleteUri->Des().Copy( *iHttpUri );
        }
    }

// ----------------------------------------------------------
// CXcapDocumentNode::DocSubsetPathL
// 
// ----------------------------------------------------------
//
HBufC8* CXcapUriParser::DocSubsetPathL()
    {
    HBufC8* ret = NULL;
    CXdmDocumentNode* current = iDocumentSubset->Parent();
    CBufFlat* buffer = CBufFlat::NewL( 256 );
    CleanupStack::PushL( buffer );
    while( current != NULL )
        {   
        buffer->InsertL( 0, FormatOnePathElementLC( current )->Des() );
        CleanupStack::PopAndDestroy();  //FormatOnePathElementLC
        current = current->Parent();
        }
    TInt length = buffer->Size();
    ret = HBufC8::NewL( length );
    TPtr8 pointer( ret->Des() );
    buffer->Read( 0, pointer, length );
    CleanupStack::PopAndDestroy();  //buffer 
    return ret;
    }

// ---------------------------------------------------------
// CXcapUriParser::FormatAttributeUriL
//
// ---------------------------------------------------------
//
HBufC8* CXcapUriParser::FormatOnePathElementLC( const CXdmDocumentNode* aPathElement ) 
    {
    #ifdef _DEBUG
        HBufC8* name8 = aPathElement->EightBitNodeNameLC();
        TPtrC8 elemName( name8->Des() );
        TPtrC8 elemPref( aPathElement->Prefix() );
        iOperationFactory.WriteToLog( _L8( "CXcapUriParser::FormatOnePathElementL()" ) );
        if( elemPref.Length() > 0 )
            iOperationFactory.WriteToLog( _L8( "  Prefix: %S" ), &elemName );
        iOperationFactory.WriteToLog( _L8( "  Name:   %S" ), &elemName );  
        CleanupStack::PopAndDestroy();  //name8
    #endif
    TInt position = 0;
    CBufFlat* buffer = CBufFlat::NewL( 50 );
    CleanupStack::PushL( buffer );
    buffer->InsertL( position, KXCAPPathSeparator );
    position = position + KXCAPPathSeparator().Length();
    TPtrC8 prefix = aPathElement->Prefix();
    if( prefix.Length() > 0 )
        {
        buffer->InsertL( position, prefix );
        position = position + prefix.Length();
        buffer->InsertL( position, KXcapUriColon );
        position = position + KXcapUriColon().Length();
        }
    TPtrC8 name = aPathElement->EightBitNodeNameLC()->Des();
    buffer->InsertL( position, name );
    position = position + name.Length();
    CleanupStack::PopAndDestroy();  //EightBitNodeNameLC 
    if( aPathElement->AttributeCount() > 0 &&
        !( iNodeType == EXdmElementAttribute && aPathElement == iDocumentSubset->Parent() ) )
        AppendPredicatesL( position, buffer, aPathElement );
    HBufC8* retBuf = HBufC8::NewL( buffer->Size() );
    TPtr8 pointer( retBuf->Des() );
    buffer->Read( 0, pointer, buffer->Size() );
    CleanupStack::PopAndDestroy();  //buffer
    CleanupStack::PushL( retBuf );
    return retBuf;
    }

// ---------------------------------------------------------
// CXcapUriParser::FormatAttributeUriL
//
// ---------------------------------------------------------
//
void CXcapUriParser::FormatAttributeUriL() 
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( " CXcapUriParser::FormatAttributeUriL()" ) );  
    #endif
    iUriBuffer->InsertL( iBufferPosition, KXCAPAttributeIdentifier );
    iBufferPosition = iBufferPosition + KXCAPAttributeIdentifier().Length();
    iUriBuffer->InsertL( iBufferPosition, iDocumentSubset->EightBitNodeNameLC()->Des() );
    CleanupStack::PopAndDestroy();  //EightBitNodeNameLC
    HBufC8* newUri = HBufC8::NewLC( iUriBuffer->Size() );
    TPtr8 pointer( newUri->Des() );
    iUriBuffer->Read( 0, pointer, iUriBuffer->Size() );
    #ifdef _DEBUG
        DumpUriL( newUri );
    #endif
    CleanupStack::PopAndDestroy();  //newUri
    }

// ---------------------------------------------------------
// CXcapUriParser::FormatNodeUriL
//
// ---------------------------------------------------------
//
void CXcapUriParser::FormatNodeUriL() 
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( " CXcapUriParser::FormatNodeUriL()" ) );  
    #endif
    iNodeType = EXdmElementNode;
    TPtrC8 prefix = iDocumentSubset->Prefix();
    if( prefix.Length() > 0 )
        {
        iUriBuffer->InsertL( iBufferPosition, prefix );
        iBufferPosition = iBufferPosition + prefix.Length();
        iUriBuffer->InsertL( iBufferPosition, KXcapUriColon );
        iBufferPosition = iBufferPosition + KXcapUriColon().Length();
        }
    TPtrC8 name = iDocumentSubset->EightBitNodeNameLC()->Des();
    iUriBuffer->InsertL( iBufferPosition, name );
    iBufferPosition = iBufferPosition + name.Length();
    CleanupStack::PopAndDestroy();  //EightBitNodeNameLC
    if( iDocumentSubset->AttributeCount() > 0 )
        AppendPredicatesL( iBufferPosition, iUriBuffer, iDocumentSubset );
    //For now, do not put a trailing '/' to the node path. This may have to be changed, though.
    //iUriBuffer->InsertL( iBufferPosition, KXCAPPathSeparator );
    //iBufferPosition = iBufferPosition + TPtrC8( KXCAPPathSeparator ).Length();
    HBufC8* newUri = HBufC8::NewLC( iUriBuffer->Size() );
    TPtr8 pointer2( newUri->Des() );
    iUriBuffer->Read( 0, pointer2, iUriBuffer->Size() );
    #ifdef _DEBUG
        DumpUriL( newUri );
    #endif
    CleanupStack::PopAndDestroy();  //newUri
    }

// ---------------------------------------------------------
// CXcapUriParser::AppendPredicatesL
//
// ---------------------------------------------------------
//
void CXcapUriParser::AppendPredicatesL( TInt& aPosition,
                                        CBufBase* aBuffer,
                                        const CXdmDocumentNode* aPathElement )
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( " CXcapUriParser::AppendPredicatesL()" ) );  
    #endif
    TInt count = aPathElement->AttributeCount();
    for( TInt i = 0;i < count;i++ )
        {
        CXdmNodeAttribute* attribute = aPathElement->Attribute( i );
        //Namespaces cannot be used as attributes
        if( attribute->NodeName().Find( _L( "xmlns" ) ) < 0 )
            {
            aBuffer->InsertL( aPosition, KHTTPBracketOpen );
            aPosition = aPosition + KHTTPBracketOpen().Length();
            aBuffer->InsertL( aPosition, KXCAPAttributeIdentifier );
            aPosition = aPosition + KXCAPAttributeIdentifier().Length();
            TPtrC8 name = attribute->EightBitNodeNameLC()->Des();
            aBuffer->InsertL( aPosition, name );
            aPosition = aPosition + name.Length();
            CleanupStack::PopAndDestroy();  //EightBitNodeNameLC
            aBuffer->InsertL( aPosition, KXCAPEquality );
            aPosition = aPosition + KXCAPEquality().Length();
            aBuffer->InsertL( aPosition, KHTTPQuotationMark );
            aPosition = aPosition + KHTTPQuotationMark().Length();
            TPtrC8 value = attribute->EightBitValueLC()->Des();
            aBuffer->InsertL( aPosition, value );
            aPosition = aPosition + value.Length();
            CleanupStack::PopAndDestroy();  //EightBitValueLC
            aBuffer->InsertL( aPosition, KHTTPQuotationMark );
            aPosition = aPosition + KHTTPQuotationMark().Length();
            aBuffer->InsertL( aPosition, KHTTPBracketClose );
            aPosition = aPosition + KHTTPBracketClose().Length();
            }
        }
    }

// ---------------------------------------------------------
// CXcapUriParser::CreateBasicNodeUriL
//
// ---------------------------------------------------------
//
TInt CXcapUriParser::CreateBasicNodeUriL( HBufC8* aNodePath )
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( " CXcapUriParser::CreateBasicNodeUriL()" ) );  
    #endif
    TInt position = 0;
    TPtr8 pointer( aNodePath->Des() );
    iUriBuffer = CBufFlat::NewL( 256 );
    iUriBuffer->InsertL( position, *iHttpUri );
    position = iHttpUri->Length();
    if( iHttpUri->LocateReverseF( KXcapPathSeparatorChar ) != iHttpUri->Length() - 1 )
        {
        iUriBuffer->InsertL( position, KXCAPPathSeparator );
        position = position + KXCAPPathSeparator().Length();
        }
    iUriBuffer->InsertL( position, KXCAPNodeSeparator ); //"~~"
    position = position + KXCAPNodeSeparator().Length();
    iUriBuffer->InsertL( position, pointer );            
    position = position + pointer.Length();
    iUriBuffer->InsertL( position, KXCAPPathSeparator );
    position = position + KXCAPPathSeparator().Length();
    return position;
    }



// ---------------------------------------------------------
// CXcapUriParser::SetDocumentSubset
//
// ---------------------------------------------------------
//
void CXcapUriParser::SetDocumentSubset( const CXdmDocumentNode* aDocumentSubset )
    {
    #ifdef _DEBUG
        iOperationFactory.WriteToLog( _L8( "CXcapUriParser::SetDocumentSubset() - Type: %d" ),
                                            aDocumentSubset->ElementType() );  
    #endif
    __ASSERT_DEBUG( aDocumentSubset != NULL, User::Panic( _L( "CXcapUriParser" ), 1 ) );
    iDocumentSubset = CONST_CAST( CXdmDocumentNode*, aDocumentSubset );
    iNodeType = iDocumentSubset->ElementType();
    }

// ---------------------------------------------------------
// CXcapUriParser::DesC8
//
// ---------------------------------------------------------
//
TPtrC8 CXcapUriParser::DesC8() const 
    {
    return iCompleteUri != NULL ? iCompleteUri->Des() : TPtrC8();
    }

// ---------------------------------------------------------
// CXcapUriParser::NodeType
//
// ---------------------------------------------------------
//
TXdmElementType CXcapUriParser::NodeType() const 
    {
    return iNodeType;
    }
    
#ifdef _DEBUG        
// ---------------------------------------------------------
// CXcapUriParser::DumpUriL
//
// ---------------------------------------------------------
//
void CXcapUriParser::DumpUriL( HBufC8* aNodePath ) 
    {
    RFile file;
    RFs session;
    TPtr8 pointer( aNodePath->Des() );
    _LIT( KFilePath, "C:\\logs\\XDM\\Dump.txt" );
    User::LeaveIfError( session.Connect() );
    TInt error = file.Replace( session, KFilePath, EFileWrite );
    if( error == KErrNone )
        {
        file.Write( pointer );
        file.Close();
        }
    session.Close();
    }
#endif
//  End of File  

