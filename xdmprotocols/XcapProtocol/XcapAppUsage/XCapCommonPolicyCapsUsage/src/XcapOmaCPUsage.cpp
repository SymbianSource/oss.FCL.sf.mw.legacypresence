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
* Description:  CXcapOmaCommonPolicyUsage
*
*/




// Used specifications
// OMA-TS-XDM_Core-V1_0-20060612, 12 June 2006

#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include "XcapOmaCommonPolicyUsage.h"

// ----------------------------------------------------
// CXcapOmaCommonPolicyUsage::CXcapOmaCommonPolicyUsage
// 
// ----------------------------------------------------
//
CXcapOmaCommonPolicyUsage::CXcapOmaCommonPolicyUsage( const CXdmEngine& aXdmEngine ) :
CXcapAppUsage( aXdmEngine )
    {
    }

// ----------------------------------------------------
// CXcapOmaCommonPolicyUsage::NewL
// 
// ----------------------------------------------------
//
CXcapOmaCommonPolicyUsage* CXcapOmaCommonPolicyUsage::NewL( const TXcapAppUsageParams& aParameters )
    {
    CXcapOmaCommonPolicyUsage* self = new ( ELeave ) CXcapOmaCommonPolicyUsage( aParameters.iXdmEngine );
    CleanupStack::PushL( self );    // << self
    self->ConstructL();
    CleanupStack::Pop( self );      // >> self
    return self;
    }

// ----------------------------------------------------
// CXcapOmaCommonPolicyUsage::~CXcapOmaCommonPolicyUsage
// 
// ----------------------------------------------------
//
CXcapOmaCommonPolicyUsage::~CXcapOmaCommonPolicyUsage()
    {
    }
        
// ----------------------------------------------------
// CXcapOmaCommonPolicyUsage::ConstructL
// 
// ----------------------------------------------------
//
void CXcapOmaCommonPolicyUsage::ConstructL()
    {
    }

// ----------------------------------------------------
// CXcapOmaCommonPolicyUsage::AUID
// 
// ----------------------------------------------------
//
TPtrC8 CXcapOmaCommonPolicyUsage::AUID() const
    {
    return TPtrC8( KXdmOmaCommonPolicyUsageAUID );
    }
                
// ----------------------------------------------------
// CXcapOmaCommonPolicyUsage::ContentType
// 
// ----------------------------------------------------
//
TPtrC8 CXcapOmaCommonPolicyUsage::ContentType() const
    {
    return TPtrC8( KXdmOmaCommonPolicyUsageContType );
    }

// ----------------------------------------------------
// CXcapOmaCommonPolicyUsage::DefaultNamespace
// 
// ----------------------------------------------------
//
TPtrC8 CXcapOmaCommonPolicyUsage::DefaultNamespace() const
    {
    return TPtrC8( KXdmOmaCommonPolicyNamespace );
    }   
    
// ----------------------------------------------------
// CXcapOmaCommonPolicyUsage::ValidateNodeL
// 
// ----------------------------------------------------
//
TBool CXcapOmaCommonPolicyUsage::ValidateNodeL( CXdmDocumentNode& aXdmNode )
    {
    // This method is called by base class for each element
    // in document, here we have to declare every element, check element 
    // datatype, restrictions for values and also do all checking that concerns  
    // the structure of the element. If the datatype is some of
    // the common datatypes defined in xcapappusage.h, the node
    // can pe passed to the base class for value validation.
    // If the node belongs to this namespace, the return value
    // should be true, false otherwise.
    
    TBool found( EFalse );
    TDataType dataType ( EDataTypeUndefined );
    TPtrC element = aXdmNode.NodeName();
            
    // <other-identity> 
    if ( Match( element, KXdmOtherIdentity ) )
        {
        dataType = EDataTypeEmpty;
        found = ETrue;
        }
    // <external-list>
    else if ( Match( element, KXdmExternalList ) )
        {
        found = ETrue;
        }
    // <entry> 
    else if ( Match( element, KXdmEntry ) )
        {
        // should we check the parent's namespace here
        found = ETrue;
        }
    // <anonymous-request> 
    else if ( Match( element, KXdmAnonymousRequest ) )
        {
        dataType = EDataTypeEmpty;
        found = ETrue;
        }
        
    // if the node is some of the basic types,
    // pass it to the base class for data validation
    if ( dataType != EDataTypeUndefined )
        {
        ValidateDataL( dataType, aXdmNode );
        }
    return found;   
    }
    
// ----------------------------------------------------
// CXcapOmaCommonPolicyUsage::ValidateAttributeL
// 
// ----------------------------------------------------
//
void CXcapOmaCommonPolicyUsage::ValidateAttributeL( const CXdmNodeAttribute& aXdmNodeAttr )
    {
    // This method is called by base class for each attribute
    // in document, here we have to define data types
    // for attributes, and pass them to the base class
    // for the actual data checking.

    // anc 
    if ( Match( aXdmNodeAttr.NodeName(), KXdmAnc ) )
        {
        // pass to base class for data validation
        ValidateDataL( EDataTypeAnyURI, aXdmNodeAttr );
        }   
    }
                            
// ----------------------------------------------------
// CXcapOmaCommonPolicyUsage::AddNamespaceInformationL
// 
// ----------------------------------------------------
//
void CXcapOmaCommonPolicyUsage::AddNamespaceInformationL( CXdmDocument& aXdmDocument )
    {
    aXdmDocument.AppendNamespaceL( KXdmOmaCommonPolicyNamespace, KNullDesC8 );
    }

// End of File

