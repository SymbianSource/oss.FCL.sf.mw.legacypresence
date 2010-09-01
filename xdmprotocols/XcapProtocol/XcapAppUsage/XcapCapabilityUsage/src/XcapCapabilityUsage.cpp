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
* Description:  CXcapCapabilityUsage
*
*/




#include <XdmDocumentNode.h>
#include "XcapCapabilityUsage.h"

// Used specification
// OMA-TS-XDM_Core-V1_0-20051103-C, 03 November 2005

// ----------------------------------------------------
// CXcapCapabilityUsage::CXcapCapabilityUsage
// 
// ----------------------------------------------------
//
CXcapCapabilityUsage::CXcapCapabilityUsage( const CXdmEngine& aXdmEngine ) :
                                            CXcapAppUsage( aXdmEngine )
    {
    }

// ----------------------------------------------------
// CXcapCapabilityUsage::NewL
// 
// ----------------------------------------------------
//
CXcapCapabilityUsage* CXcapCapabilityUsage::NewL( const TXcapAppUsageParams& aParameters )
    {
    CXcapCapabilityUsage* self = new ( ELeave ) CXcapCapabilityUsage( aParameters.iXdmEngine );
    CleanupStack::PushL( self );    // << self
    self->ConstructL();
    CleanupStack::Pop( self );      // >> self
    return self;
    }

// ----------------------------------------------------
// CXcapCapabilityUsage::~CXcapCapabilityUsage
// 
// ----------------------------------------------------
//
CXcapCapabilityUsage::~CXcapCapabilityUsage()
    {
    }
        
// ----------------------------------------------------
// CXcapCapabilityUsage::ConstructL
// 
// ----------------------------------------------------
//
void CXcapCapabilityUsage::ConstructL()
    {
    }

// ----------------------------------------------------
// CXcapCapabilityUsage::AUID
// 
// ----------------------------------------------------
//
TPtrC8 CXcapCapabilityUsage::AUID() const
    {
    return TPtrC8( KXdmCapabilityUsageAUID );
    }

// ----------------------------------------------------
// CXcapCapabilityUsage::ContentType
// 
// ----------------------------------------------------
//
TPtrC8 CXcapCapabilityUsage::ContentType() const
    {
    return TPtrC8( KXdmCapabilityUsageContType );
    }

// ----------------------------------------------------
// CXcapCapabilityUsage::DefaultNamespace
// 
// ----------------------------------------------------
//
TPtrC8 CXcapCapabilityUsage::DefaultNamespace() const
    {
    return TPtrC8( KXdmCapabilityNamespace );
    }    

// ----------------------------------------------------
// CXcapCapabilityUsage::ValidateNodeL
// 
// ----------------------------------------------------
//
TBool CXcapCapabilityUsage::ValidateNodeL( CXdmDocumentNode& aXdmNode )
    {
    // This method is called by base class for each element
    // in document, here we have to declare every element, check element 
    // datatype, restrictions for values and also do all checking that concerns  
    // the structure of the element. If the datatype is some of
    // the common datatypes defined in xcapappusage.h, the node
    // can pe passed to the base class for value validation.
    // If the node belongs to this namespace, the return value
    // should be true, false otherwise.
    
    TBool found ( EFalse );
    TDataType dataType ( EDataTypeUndefined );
    TPtrC element = aXdmNode.NodeName();
    
    // <auids>
    if ( Match( element, KXdmAuids ) )
        {
        found = ETrue;
        } 
    // <auid>
    else if ( Match( element, KXdmAuid ) )
        {
        dataType = EDataTypeString;
        found = ETrue;
        }
    // <extensions>
    else if ( Match( element, KXdmExtensions ) )
        {
        found = ETrue;
        }
    // <extension>
    else if ( Match( element, KXdmExtension ) )
        {
        dataType = EDataTypeString;
        found = ETrue;
        }
    // <namespaces>
    else if ( Match( element, KXdmNamespace ) )
        {
        found = ETrue;
        }
    // <namespace>
    else if ( Match( element, KXdmNamespace ) )
        {
        dataType = EDataTypeAnyURI;
        found = ETrue;
        }
        
    if ( dataType != EDataTypeUndefined )
        {
        // pass to the base class for data validation
        ValidateDataL( dataType, aXdmNode );
        }
    return found;
    }

// ----------------------------------------------------
// CXcapCapabilityUsage::ValidateAttributeL
// 
// ----------------------------------------------------
//
void CXcapCapabilityUsage::ValidateAttributeL( const CXdmNodeAttribute& /*aXdmNodeAttr*/ )
    {
    // This method is called by base class for each attribute
    // in document, here we have to define data types
    // for attributes, and pass them to the base class
    // for the actual data checking.
    
    // There aren't any attributes in
    // capability usage schema.
    }
    
// ----------------------------------------------------
// CXcapCapabilityUsage::AddNamespaceInformationL
// 
// ----------------------------------------------------
//
void CXcapCapabilityUsage::AddNamespaceInformationL( CXdmDocument& aXdmDocument )
    {
    aXdmDocument.AppendNamespaceL( KXdmCapabilityNamespace, KNullDesC8 );
    }

// End of File

