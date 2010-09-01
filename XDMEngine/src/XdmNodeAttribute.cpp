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
* Description:   XDM Engine node attribute
*
*/




// INCLUDE FILES
#include <escapeutils.h>

#include "XdmEngine.h"
#include "XdmNodeFactory.h"
#include "XdmNodeAttribute.h"

// ----------------------------------------------------------
// CXdmNodeAttribute::CXdmNodeAttribute
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmNodeAttribute::CXdmNodeAttribute( CXdmEngine& aXdmEngine,
                                               MXdmNodeFactory& aNodeFactory ) :
                                               CXdmDocumentNode( aXdmEngine, aNodeFactory )
                                                
    {    
    }
    
// ----------------------------------------------------------
// CXdmNodeAttribute::CXdmNodeAttribute
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmNodeAttribute::CXdmNodeAttribute( CXdmEngine& aXdmEngine,
                                               MXdmNodeFactory& aNodeFactory,
                                               CXdmDocumentNode* aParentNode ) :
                                               CXdmDocumentNode( aXdmEngine, aNodeFactory, aParentNode )                                          
    {  
    }
       
// ----------------------------------------------------------
// CXdmNodeAttribute::ConstructL
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmNodeAttribute::BaseConstructL( const TDesC& aAttributeName )
    {
    SetNameL( aAttributeName );
    }

// ----------------------------------------------------------
// CXdmNodeAttribute::ElementType
// 
// ----------------------------------------------------------
//
EXPORT_C TXdmElementType CXdmNodeAttribute::ElementType() const
    {
    return EXdmElementAttribute;
    }

// ----------------------------------------------------
// CXdmNodeAttribute::~CXdmNodeAttribute
// 
// ----------------------------------------------------
//
EXPORT_C CXdmNodeAttribute::~CXdmNodeAttribute()
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmNodeAttribute::~CXdmNodeAttribute()" ) );
    #endif
    delete iAttributeValue;
    }

// ----------------------------------------------------
// CXdmNodeAttribute::AttributeValue
// 
// ----------------------------------------------------
//
EXPORT_C TPtrC CXdmNodeAttribute::AttributeValue() const
    {
    return iAttributeValue != NULL ? iAttributeValue->Des() : TPtrC();
    }

// ----------------------------------------------------
// CXdmNodeAttribute::AttributeValue
// 
// ----------------------------------------------------
//
EXPORT_C HBufC8* CXdmNodeAttribute::EightBitValueLC() const
    {
    HBufC8* eightBit = CXdmEngine::ConvertToUTF8L( iAttributeValue->Des() );
    CleanupStack::PushL( eightBit );
    return eightBit;
    }

// ----------------------------------------------------
// CXdmNodeAttribute::EscapedValueLC
// 
// ----------------------------------------------------
//
EXPORT_C HBufC8* CXdmNodeAttribute::EscapedValueLC() const
    {
    HBufC8* eightBit = CXdmEngine::ConvertToUTF8L( iAttributeValue->Des() );
    CleanupStack::PushL( eightBit );
    HBufC8* escape = EscapeDescLC( eightBit->Des() );
    CleanupStack::Pop();  //escape
    CleanupStack::PopAndDestroy();  //eightBit
    CleanupStack::PushL( escape );
    return escape;
    }
    
// ----------------------------------------------------
// CXdmNodeAttribute::SetAttributeValue
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmNodeAttribute::SetAttributeValueL( const TDesC& aAttributeValue )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmNodeAttribute::SetAttributeValueL() - Value: %S" ),
                                &aAttributeValue );
    #endif
    delete iAttributeValue;
    iAttributeValue = NULL;
    iAttributeValue = HBufC::NewL( aAttributeValue.Length() );
    iAttributeValue->Des().Copy( aAttributeValue );
    }

// ----------------------------------------------------
// CXdmNodeAttribute::SetAttributeValue
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmNodeAttribute::SetAttributeValueL( const TDesC8& aAttributeValue )
    {
    // change to UNICODE from UTF8 format
    HBufC* unicode = EscapeUtils::ConvertToUnicodeFromUtf8L( aAttributeValue );
    CleanupStack::PushL( unicode );
    
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmNodeAttribute::SetAttributeValueL() - Value: %S" ),
                                unicode );
    #endif
    delete iAttributeValue;
    iAttributeValue = NULL;
    iAttributeValue = HBufC::NewL( unicode->Length() );
    iAttributeValue->Des().Copy( *unicode );
    CleanupStack::PopAndDestroy( unicode );
    }


