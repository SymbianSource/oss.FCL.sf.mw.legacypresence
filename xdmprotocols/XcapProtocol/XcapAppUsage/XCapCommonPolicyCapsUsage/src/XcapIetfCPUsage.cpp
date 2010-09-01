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
* Description:  CXcapIetfCommonPolicyUsage
*
*/




#include <XdmErrors.h>
#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include "XcapIetfCommonPolicyUsage.h"

// Used specifications
// draft-ietf-geopriv-common-policy-08, 05 March 2006, expires 06 September 2006

// ----------------------------------------------------
// CXcapIetfCommonPolicyUsage::CXcapIetfCommonPolicyUsage
// 
// ----------------------------------------------------
//
CXcapIetfCommonPolicyUsage::CXcapIetfCommonPolicyUsage( const CXdmEngine& aXdmEngine ) 
:CXcapAppUsage( aXdmEngine )
    {
    }

// ----------------------------------------------------
// CXcapIetfCommonPolicyUsage::NewL
// 
// ----------------------------------------------------
//
CXcapIetfCommonPolicyUsage* CXcapIetfCommonPolicyUsage::NewL( const TXcapAppUsageParams& aParameters )
    {
    CXcapIetfCommonPolicyUsage* self = new ( ELeave ) CXcapIetfCommonPolicyUsage( aParameters.iXdmEngine );
    CleanupStack::PushL( self );	// << self
    self->ConstructL();
    CleanupStack::Pop( self );      // >> self
    return self;
    }

// ----------------------------------------------------
// CXcapIetfCommonPolicyUsage::~CXcapIetfCommonPolicyUsage
// 
// ----------------------------------------------------
//
CXcapIetfCommonPolicyUsage::~CXcapIetfCommonPolicyUsage()
    {
    }
        
// ----------------------------------------------------
// CXcapIetfCommonPolicyUsage::ConstructL
// 
// ----------------------------------------------------
//
void CXcapIetfCommonPolicyUsage::ConstructL()
    {
    }

// ----------------------------------------------------
// CXcapIetfCommonPolicyUsage::AUID
// 
// ----------------------------------------------------
//
TPtrC8 CXcapIetfCommonPolicyUsage::AUID() const
    {
    return TPtrC8( KXdmIetfCommonPolicyUsageAUID );
    }
                
// ----------------------------------------------------
// CXcapIetfCommonPolicyUsage::ContentType
// 
// ----------------------------------------------------
//
TPtrC8 CXcapIetfCommonPolicyUsage::ContentType() const
    {
    return TPtrC8( KXdmIetfCommonPolicyUsageContType );
    }

// ----------------------------------------------------
// CXcapIetfCommonPolicyUsage::DefaultNamespace
// 
// ----------------------------------------------------
//
TPtrC8 CXcapIetfCommonPolicyUsage::DefaultNamespace() const
    {
    return TPtrC8( KXdmIetfCommonPolicyNamespace );
    }   
    
// ----------------------------------------------------
// CXcapIetfCommonPolicyUsage::ValidateNodeL
// 
// ----------------------------------------------------
//
TBool CXcapIetfCommonPolicyUsage::ValidateNodeL( CXdmDocumentNode& aXdmNode )
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
            
    // <ruleset> 
    if ( Match( element, KXdmRuleset ) )
        {
        TInt count( aXdmNode.NodeCount() );
        for ( TInt i(0); i < count; i++ )
            {
            // ruleset should not contain any other type elements
            // than <rule>
            if ( !Match( aXdmNode.ChileNode(i)->NodeName(), KXdmRule ) ) 
                {
                LeaveWithErrorL( KXcapErrorSchemaViolation );
                }
            }
        found = ETrue;
        }
    // <rule> 
    else if ( Match( element, KXdmRule ) )
        {
        // required attribute id
        if ( !aXdmNode.HasAttribute( KXdmId ) )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredAttribute );
            }
        found = ETrue;
        }
    // <conditions> 
    else if ( Match( element, KXdmConditions ) )
        {
        found = ETrue;
        }
    // <actions> 
    else if ( Match( element, KXdmActions ) )
        {
        found = ETrue;
        }
    // <transformations> 
    else if ( Match( element, KXdmTransformations ) )
        {
        found = ETrue;
        }
    // <validity> 
    else if ( Match( element, KXdmValidity ) )
        {
            // <validity> should contain <from> and <until>
            TBool from( EFalse );
            TBool until( EFalse );
            TInt count( aXdmNode.NodeCount() );
            for ( TInt i(0); i < count; i++ )
                {
                TPtrC childName = aXdmNode.ChileNode(i)->NodeName();
                if ( Match( childName, KXdmFrom ) ) 
                    {
                    from = ETrue;
                    }
                else if ( Match( childName, KXdmUntil ) ) 
                    {
                    until = ETrue;
                    }
                }
            if ( !from || !until ) 
                {
                LeaveWithErrorL( KXcapErrorMissingRequiredElement );    
                }    
        found = ETrue;
        }
    // <from> 
    else if ( Match( element, KXdmFrom ) )
        {
        dataType = EDataTypeDateTime;
        found = ETrue;
        }
    // <to> 
    else if ( Match( element, KXdmTo ) )
        {
        dataType = EDataTypeDateTime;
        found = ETrue;
        }
    // <identity> 
    else if ( Match( element, KXdmIdentity ) )
        {
        // <identity> should contain either <one> or <many>
        // elements, not both
        TBool one( EFalse );
        TBool many( EFalse );
        TInt count( aXdmNode.NodeCount() );
        for ( TInt i(0); i < count; i++ )
            {
            TPtrC childName = aXdmNode.ChileNode(i)->NodeName();
            if ( Match( childName, KXdmOne ) ) 
                {
                one = ETrue;
        }
            else if ( Match( childName, KXdmMany ) ) 
                {
                many = ETrue;
                }    
            }
        if ( one && many )
            {
            LeaveWithErrorL( KXcapErrorSchemaViolation );
            }
        found = ETrue;
        }
    // <id> 
    else if ( Match( element, KXdmId ) )
        {
        // required attribute entity
        if ( !aXdmNode.HasAttribute( KXdmEntity ) )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredAttribute );
            }
        found = ETrue;
        }
    // <any-identity> 
    else if ( Match( element, KXdmAnyIdentity ) )
        {
        found = ETrue;
        }
    // <except> 
    else if ( Match( element, KXdmExcept ) )
        {
        found = ETrue;
        }   
    // <sphere> 
    else if ( Match( element, KXdmSphere ) )
        {
        // required attribute value
        if ( !aXdmNode.HasAttribute( KXdmValue ) )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredAttribute );
            }
        found = ETrue;
        }
    // <one> 
    else if ( Match( element, KXdmOne ) )
        {
        // required attribute id
        if ( !aXdmNode.HasAttribute( KXdmId ) )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredAttribute );
            }
        found = ETrue;
        }
    // <many> 
    else if ( Match( element, KXdmMany) )
        {
        found = ETrue;
        }
    // <until> 
    else if ( Match( element, KXdmUntil ) )
        {
        dataType = EDataTypeDateTime;
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
// CXcapIetfCommonPolicyUsage::ValidateAttributeL
// 
// ----------------------------------------------------
//
void CXcapIetfCommonPolicyUsage::ValidateAttributeL( const CXdmNodeAttribute& aXdmNodeAttr )
    {
    // This method is called by base class for each attribute
    // in document, here we have to define data types
    // for attributes, and pass them to the base class
    // for the actual data checking.
        
    TDataType dataType ( EDataTypeUndefined );
    TPtrC attribute = aXdmNodeAttr.NodeName();

    // id 
    if ( Match( attribute, KXdmId ) )
        {
        dataType = EDataTypeString;
        }
        // domain 
    else if ( Match( attribute, KXdmDomain ) )
        {
        dataType = EDataTypeString;
        }
    // value 
    else if ( Match( attribute, KXdmValue ) )
        {
        dataType = EDataTypeString;
        }

    // pass to base class for data validation
    ValidateDataL( dataType, aXdmNodeAttr );
    }
                            
// ----------------------------------------------------
// CXcapIetfCommonPolicyUsage::AddNamespaceInformationL
// 
// ----------------------------------------------------
//
void CXcapIetfCommonPolicyUsage::AddNamespaceInformationL( CXdmDocument& aXdmDocument )
	{
    aXdmDocument.AppendNamespaceL( KXdmIetfCommonPolicyNamespace, KNullDesC8 );
	}

// End of File

