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
* Description:   CXdmXmlContentHandler
*
*/



#include <XdmDocument.h>
#include <contenthandler.h>
#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include "XdmNamespaceContainer.h"
#include "XdmXmlParser.h"
#include "XdmNodeInterface.h"
#include "XmlParserNodePath.h"
#include "XdmXmlContentHandler.h"

// ---------------------------------------------------------
// CXdmXmlContentHandler::CXdmXmlContentHandler
//
// ---------------------------------------------------------
//
CXdmXmlContentHandler::CXdmXmlContentHandler( CXdmXmlParser& aParserMain ) :
                                              iFinished( EFalse ),
                                              iTargetFound( EFalse ),
                                              iPartialDocument( EFalse ),
                                              iParserMain( aParserMain )

    {   
    }
    
// ---------------------------------------------------------
// CXdmXmlContentHandler::NewL
//
// ---------------------------------------------------------
//
CXdmXmlContentHandler* CXdmXmlContentHandler::NewL( CXdmXmlParser& aParserMain )
    {
    CXdmXmlContentHandler* self = new ( ELeave ) CXdmXmlContentHandler( aParserMain );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CXdmXmlContentHandler::ConstructL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::ConstructL()
    {
    iContentBuffer = CBufFlat::NewL( 50 ); 
    }
            
// ---------------------------------------------------------
// CXdmXmlContentHandler::~CXdmXmlContentHandler
//
// ---------------------------------------------------------
//
CXdmXmlContentHandler::~CXdmXmlContentHandler()
    { 
    delete iNodePath;
    delete iContentBuffer;
    }

// ---------------------------------------------------------
// CXdmXmlContentHandler::SetTarget
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::SetTarget( const CXdmDocument& aTargetDocument )
    {
    iTargetDocument = CONST_CAST( CXdmDocument*, &aTargetDocument );
    iRootNode = iTargetDocument->DocumentRoot();
    iDocumentSubset = NULL;
    iIsRootNode = ETrue;
    }

// ---------------------------------------------------------
// CXdmXmlContentHandler::SetTarget
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::SetTarget( const CXdmDocumentNode& aDocumentRoot )
    {
    iTargetDocument = NULL;
    iRootNode = CONST_CAST( CXdmDocumentNode*, &aDocumentRoot );
    iDocumentSubset = NULL;
    iIsRootNode = ETrue;
    }
    
// ---------------------------------------------------------
// CXdmXmlContentHandler::SetTarget 
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::SetTargetL( const CXdmDocument& aTargetDocument,
                                        const CXdmDocumentNode& aTargetNode )
    {
    iElementType = aTargetNode.ElementType();
    iTargetDocument = CONST_CAST( CXdmDocument*, &aTargetDocument );
    if( iElementType == EXdmElementAttribute )
        {
        iTargetAttribute = ( CXdmNodeAttribute* )&aTargetNode;
        iDocumentSubset = CONST_CAST( CXdmDocumentNode*, aTargetNode.Parent() );
        }
    else 
        {
        iTargetAttribute = NULL;
        iDocumentSubset = CONST_CAST( CXdmDocumentNode*, &aTargetNode );
        }
    iNodePath = CXmlParserNodePath::NewL( iParserMain, iElementType, *iDocumentSubset, this );
    iRootNode = iDocumentSubset; 
    iPartialDocument = ETrue;
    iCurrentNode = NULL;
    }

// ---------------------------------------------------------
// CXdmXmlContentHandler::TargetAttribute
//
// ---------------------------------------------------------
//
CXdmNodeAttribute* CXdmXmlContentHandler::TargetAttribute() const
    {
    return iTargetAttribute;
    }
    
// ---------------------------------------------------------
// CXdmXmlContentHandler::Reset
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::Reset()
    {
    iTargetDocument = NULL;
    iTargetAttribute = NULL;
    iDocumentSubset = NULL;
    iRootNode = NULL;
    iCurrentNode = NULL;
    delete iNodePath;
    iNodePath = NULL;
    iFinished = EFalse;
    iTargetFound = EFalse;
    iPartialDocument = EFalse;
    }
       
// ---------------------------------------------------------
// CXdmXmlContentHandler::GetExtendedInterface
//
// ---------------------------------------------------------
//
TAny* CXdmXmlContentHandler::GetExtendedInterface( const TInt32 /*aUid*/ )    
    {
    return NULL; 
    }
    
// ---------------------------------------------------------
// CXdmXmlContentHandler::OnContentL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::OnContentL( const TDesC8 &aBytes,
                                        TInt aErrorCode )
    {
    #ifdef _DEBUG
        iParserMain.WriteToLog( _L8( "CXmlNode::OnContentL()" ), aErrorCode );
        iParserMain.WriteToLog( _L8( "  Bytes: %S" ), &aBytes );
        iParserMain.WriteToLog( _L8( "  Error: %d" ), aErrorCode );
    #endif
    if( iPartialDocument && !iTargetFound )
        return;
    TInt length = aBytes.Length();
    TPtr8 desc( CONST_CAST( TUint8*, aBytes.Ptr() ), length, length );
    desc.TrimAll();
    if( desc.Length() > 0 )
        {
        iContentBuffer->InsertL( iContentIndex, desc );
        iContentIndex = iContentIndex + desc.Length();
        }
    }
    
// ---------------------------------------------------------
// CXdmXmlContentHandler::OnEndDocumentL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::OnEndDocumentL( TInt aErrorCode )
    {
    iIsRootNode = ETrue;
    #ifdef _DEBUG
        iParserMain.WriteToLog( _L8( "CXmlNode::OnEndDocumentL() - Error: %d "), aErrorCode );
    #endif
    }

// ---------------------------------------------------------
// CXdmXmlContentHandler::OnEndElementL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::OnEndElementL( const RTagInfo &aElement,
                                           TInt aErrorCode )
    {
    TPtrC8 element( aElement.LocalName().DesC() );
    #ifdef _DEBUG
        iParserMain.WriteToLog( _L8( "CXmlNode::OnEndElementL() - Element: %S  Error: %d "),
                                   &element, aErrorCode );
    #endif
    if( iCurrentNode != NULL && iContentBuffer->Size() > 0 )
        {
        TPtrC8 data( iContentBuffer->Ptr( 0 ) );
        iCurrentNode->SetLeafNode( ETrue );
        iCurrentNode->SetLeafNodeContentL( data );
        iContentBuffer->Reset();
        iContentIndex = 0;
        }
    if( iCurrentNode != NULL && iCurrentNode->NodeCount() == 0 )
        iCurrentNode->SetEmptyNode( ETrue );
    if( iPartialDocument )
        {
        if( !iFinished )
            {
            if( iCurrentNode != NULL )
                {
                CXdmDocumentNode* parent = iCurrentNode->Parent();
                #ifdef _DEBUG
                    TBuf8<64> parentName;
                    TBuf8<64> currentName;
                    parentName.Copy( parent->NodeName() );
                    currentName.Copy( iCurrentNode->NodeName() );
                    iParserMain.WriteToLog( _L8( " Parent: %x - %S "), parent, &parentName );
                    iParserMain.WriteToLog( _L8( " Current: %x - %S"), iCurrentNode, &currentName );
                #endif  
                if( parent != NULL && iCurrentNode != parent )
                    iCurrentNode = iCurrentNode->Parent();
                iFinished = iCurrentNode == iDocumentSubset->Parent();
                if( iFinished )
                    iCurrentNode = NULL;
                #ifdef _DEBUG
                    iParserMain.WriteToLog( _L8( " Current: %x"), iCurrentNode );
                #endif   
                }   
            }
        }
    else if ( iCurrentNode ) 
        {
        iCurrentNode = iCurrentNode->Parent();
        }
    else
        {
        // For PC-lint note
        }
    }
    
// ---------------------------------------------------------
// CXdmXmlContentHandler::OnEndPrefixMappingL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::OnEndPrefixMappingL( const RString& /*aPrefix*/,
                                                 TInt aErrorCode )
    {
    #ifdef _DEBUG
        iParserMain.WriteToLog( _L8( "CXmlNode::OnEndPrefixMappingL() - Error: %d "), aErrorCode );
    #endif
    }
    
// ---------------------------------------------------------
// CXdmXmlContentHandler::OnError
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::OnError( TInt aErrorCode )
    {
    #ifdef _DEBUG
        iParserMain.WriteToLog( _L8( "CXmlNode::OnError() - Error: %d" ), aErrorCode );
    #endif
    }
    
// ---------------------------------------------------------
// CXdmXmlContentHandler::OnIgnorableWhiteSpaceL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::OnIgnorableWhiteSpaceL( const TDesC8& /*aBytes*/,
                                                    TInt /*aErrorCode*/ )
    {
    #ifdef _DEBUG
        //iParserMain.WriteToLog( _L8( "CXmlNode::OnIgnorableWhiteSpaceL() - Error: %d" ), aErrorCode );
    #endif
    }

// ---------------------------------------------------------
// CXdmXmlContentHandler::OnProcessingInstructionL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::OnProcessingInstructionL( const TDesC8& /*aTarget*/,
                                                      const TDesC8& /*aData*/,
                                                      TInt /*aErrorCode*/ )
    {
    #ifdef _DEBUG
       // iParserMain.WriteToLog( _L8( "CXmlNode::OnProcessingInstructionL() - Error: %d" ), aErrorCode );
    #endif
    }
    
// ---------------------------------------------------------
// CXdmXmlContentHandler::OnSkippedEntityL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::OnSkippedEntityL( const RString& /*aName*/,
                                              TInt /*aErrorCode*/ )
    {
    #ifdef _DEBUG
        //iParserMain.WriteToLog( _L8( "CXmlNode::OnSkippedEntityL() - Error: %d" ), aErrorCode );
    #endif
    }

// ---------------------------------------------------------
// CXdmXmlContentHandler::OnStartDocumentL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::OnStartDocumentL( const RDocumentParameters& /*aDocParam*/, TInt aErrorCode )
    {
    #ifdef _DEBUG
        iParserMain.WriteToLog( _L8( "CXmlNode::OnStartDocumentL() - Error: %d "), aErrorCode );
    #endif
    }

// ---------------------------------------------------------
// CXdmXmlContentHandler::OnStartElementL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::OnStartElementL( const RTagInfo& aElement,
                                             const RAttributeArray& aAttributes,
                                             TInt aErrorCode )
    {
    TPtrC8 name( aElement.LocalName().DesC() );
    TPtrC8 uri( aElement.Uri().DesC() );
    TPtrC8 prefix( aElement.Prefix().DesC() );
    #ifdef _DEBUG
        iParserMain.WriteToLog( _L8( "CXmlNode::OnStartElementL(): Error: %d" ), aErrorCode );
        iParserMain.WriteToLog( _L8( "  URI:     %S" ), &uri );
        iParserMain.WriteToLog( _L8( "  Name:    %S" ), &name );
        iParserMain.WriteToLog( _L8( "  Prefix:  %S" ), &prefix );
    #endif
    if( iPartialDocument )
        {   
        if( !iFinished )
            {
            if( iTargetFound )
                {
                iCurrentNode = iCurrentNode->CreateChileNodeL();
                HandleNextElementL( aElement, aAttributes );
                }
            else
                {
                if( iNodePath->CheckNextNodeL( name, aAttributes ) )
                    {
                    if( iElementType == EXdmElementAttribute )
                        AppendAttributeValueL( aAttributes );
                    else
                        {
                        iTargetFound = ETrue;
                        iCurrentNode = iDocumentSubset;
                        }
                    }
                }
            }
        }
    else
        {
        iCurrentNode = iIsRootNode ? iRootNode : iCurrentNode->CreateChileNodeL();
        HandleNextElementL( aElement, aAttributes );
        iIsRootNode = EFalse;
        }    
    }

// ---------------------------------------------------------
// CXdmXmlContentHandler::HandleNextElementL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::HandleNextElementL( const RTagInfo& aElement,
                                                const RAttributeArray& aAttributes )
    {
    #ifdef _DEBUG
        //iParserMain.WriteToLog( _L8( "CXdmXmlContentHandler::HandleNextElementL()") );
    #endif
    iCurrentNode->SetNameL( aElement.LocalName().DesC() );
    SetNamespaceInformationL( aElement, iCurrentNode );
    TInt count = aAttributes.Count();
    if( count > 0 )
        {
        CXdmNodeAttribute* attribute = NULL;
        for( TInt i = 0;i < count;i++ )
            {
            attribute = iCurrentNode->CreateAttributeL();
            CleanupStack::PushL( attribute );
            attribute->SetNameL( aAttributes[i].Attribute().LocalName().DesC() );
            attribute->SetAttributeValueL( aAttributes[i].Value().DesC() );
            CleanupStack::Pop();  //attribute
            }
        }
    }
                        
// ---------------------------------------------------------
// CXdmXmlContentHandler::CompileAttributesL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::AppendAttributeValueL( const RAttributeArray& aAttributes )
    {
    #ifdef _DEBUG
        //iParserMain.WriteToLog( _L8( "CXdmXmlContentHandler::AppendAttributeValueL()") );
    #endif
    TInt count = aAttributes.Count();
    TPtrC8 targetName( iTargetAttribute->EightBitNodeNameLC()->Des() );
    for( TInt i = 0;!iFinished && i < count;i++ )
        {
        TPtrC8 desc = aAttributes[i].Attribute().LocalName().DesC();
        #ifdef _DEBUG
            iParserMain.WriteToLog( _L8( "  Name of attribute %d: %S"), i, &desc );
        #endif
        if( targetName.Compare( desc ) == 0 )
            {
            iFinished = ETrue;
            TPtrC8 value( aAttributes[i].Value().DesC() );
            iTargetAttribute->SetAttributeValueL( value );
            }
        }
    CleanupStack::PopAndDestroy();  //desc
    }
    
// ---------------------------------------------------------
// CXdmXmlContentHandler::OnStartPrefixMappingL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::OnStartPrefixMappingL( const RString& aPrefix,
                                                   const RString& aUri,
                                                   TInt aErrorCode )
    {
    #ifdef _DEBUG
        iParserMain.WriteToLog( _L8( "CXdmXmlContentHandler::OnStartPrefixMappingL() - Error: %d" ), aErrorCode );
    #endif
    AppendNameSpaceL( aPrefix.DesC(), aUri.DesC() );
    }
    
// ---------------------------------------------------------
// CXdmXmlContentHandler::IsWhiteSpace
//
// ---------------------------------------------------------
//
TBool CXdmXmlContentHandler::IsWhiteSpace( const TDesC8 &aBytes ) const
    {
    #ifdef _DEBUG
        //iParserMain.WriteToLog( _L8( "CXdmXmlContentHandler::IsWhiteSpace()") );
    #endif
    TBool ret = ETrue;
    if( aBytes.Length() > 0 )
        {
        TChar ch = aBytes[0];
        ret = ch == 32 || ch == 10 || ch == 13 || ch == 9;
        }
    return ret;
    }

// ---------------------------------------------------------
// CXdmXmlContentHandler::AppendNameSpaceL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::AppendNameSpaceL( const TDesC8& aPrefix, const TDesC8& aUri )
    {
    #ifdef _DEBUG
        //iParserMain.WriteToLog( _L8( "CXdmXmlContentHandler::SetNameSpaceInformationL()") );
    #endif
    if( iTargetDocument != NULL )
        {
        MXdmNamespaceContainer* container = iTargetDocument;
        container->AppendNamespaceL( aUri, aPrefix );
        }
    }
       
// ---------------------------------------------------------
// CXdmXmlContentHandler::SetNameSpaceInformationL
//
// ---------------------------------------------------------
//
void CXdmXmlContentHandler::SetNamespaceInformationL( const RTagInfo& aElement,
                                                      MXdmNodeInterface* aXcapNode )
    {
    #ifdef _DEBUG
        //iParserMain.WriteToLog( _L8( "CXdmXmlContentHandler::SetNameSpaceInformationL()") );
    #endif
    TPtrC8 prefix( aElement.Prefix().DesC() );
    if( prefix.Length() > 0 )
        {
        aXcapNode->SetPrefixL( prefix );
        }
    }
            
// End of File
