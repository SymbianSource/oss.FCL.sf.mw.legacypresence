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
* Description:   CXmlParserNodePath
*
*/



#include <contenthandler.h>
#include "XdmDocument.h"
#include "XdmXmlParser.h"
#include "XdmDocumentNode.h"
#include "XdmNodeAttribute.h"
#include "XmlParserNodePath.h"
#include "XmlNodePathInterface.h"

// ---------------------------------------------------------
// CXmlParserNodePath::CXmlParserNodePath
//
// ---------------------------------------------------------
//
CXmlParserNodePath::CXmlParserNodePath( CXdmXmlParser& aParserMain,
                                        TXdmElementType aElementType,
                                        MXmlNodePathInterface* aPathInterface ) :
                                        iElementType( aElementType ),
                                        iPathInterface( aPathInterface ),
                                        iParserMain( aParserMain )
    {
    }
    
// ---------------------------------------------------------
// CXmlParserNodePath::ExtractString
//
// ---------------------------------------------------------
//
CXmlParserNodePath* CXmlParserNodePath::NewL( CXdmXmlParser& aParserMain,
                                              TXdmElementType aElementType,
                                              const CXdmDocumentNode& aTargetNode,
                                              MXmlNodePathInterface* aPathInterface )
    {
    CXmlParserNodePath* self = new ( ELeave ) CXmlParserNodePath( aParserMain, aElementType, aPathInterface );
    CleanupStack::PushL( self );
    self->ConstructL( aTargetNode );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CXmlParserNodePath::~CXmlParserNodePath
//
// ---------------------------------------------------------
//
CXmlParserNodePath::~CXmlParserNodePath()
    {
    iNodes.Close();
    }
    
// ---------------------------------------------------------
// CXmlParserNodePath::ConstructL
//
// ---------------------------------------------------------
//
void CXmlParserNodePath::ConstructL( const CXdmDocumentNode& aTargetNode )
    {
    #ifdef _DEBUG
        iParserMain.WriteToLog( _L8( "CXmlParserNodePath::ConstructL()" ) );
    #endif
    CXdmDocumentNode* current = CONST_CAST( CXdmDocumentNode*, &aTargetNode );
    while( current != NULL )
        {   
        iNodes.InsertL( current, 0 );
        current = current->Parent();
        }
    }
                               
// ---------------------------------------------------------
// CXmlParserNodePath::CheckNextNodeL
//
// ---------------------------------------------------------
//
TBool CXmlParserNodePath::CheckNextNodeL( const TDesC8& aNextElement,
                                          const RAttributeArray& aAttributes )
    {
    #ifdef _DEBUG
        //iParserMain.WriteToLog( _L8( "CXmlParserNodePath::CheckNextNodeL" ) );
    #endif
    if( !iComplete && aNextElement.Length() > 0 )
        {
        TInt attrCount = aAttributes.Count();
        RPointerArray<SXdmAttribute8> attributes;
        CleanupClosePushL( attributes );
        for( TInt i = 0;i < attrCount;i++ )
            {
            SXdmAttribute8 attribute; 
            attribute.iName.Set( aAttributes[i].Attribute().LocalName().DesC() );
            attribute.iValue.Set( aAttributes[i].Value().DesC() );
            User::LeaveIfError( attributes.Append( &attribute ) );
            }
        HBufC* unicode = HBufC::NewLC( aNextElement.Length() );
        unicode->Des().Copy( aNextElement );
        if( iNodes[iIndex]->Match( unicode->Des(), attributes ) )
            {
            iIndex++;
            if( iElementType == EXdmElementAttribute )
                iComplete = iNodes[iIndex] == iPathInterface->TargetAttribute()->Parent();
            else /* or else if, the next if is not needed */
                {
                if( iIndex == iNodes.Count() )
                    iComplete = ETrue;
                }
            }
        CleanupStack::PopAndDestroy( 2 );   //unicode, attributes
        }
    return iComplete;
    }
    
// ---------------------------------------------------------
// CXmlParserNodePath::CheckNextNodeL
//
// ---------------------------------------------------------
//
TBool CXmlParserNodePath::CheckNextNode( const CXdmDocumentNode& aNextNode )
    {
    #ifdef _DEBUG
        //iParserMain.WriteToLog( _L8( "CXmlParserNodePath::CheckNextNodeL" ) );
    #endif
    if( !iComplete )
        {
        if( iNodes[iIndex]->Match( aNextNode ) )
            {
            iIndex++;
            if( iElementType == EXdmElementAttribute )
                iComplete = iNodes[iIndex] == iPathInterface->TargetAttribute()->Parent();
            else /* or else if, the next if is not needed */
                {
                if( iIndex == iNodes.Count() )
                    iComplete = ETrue;
                }
            }
        }
    return iComplete;
    }

// End of File
