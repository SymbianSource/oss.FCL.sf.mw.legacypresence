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
* Description:  CXcapOmaPresRulesUsage
*
*/




#include <XdmErrors.h>
#include <XdmDocumentNode.h>
#include "XcapOmaPresRulesUsage.h"

// Used specification
// OMA-TS-Presence_SIMPLE_XDM-V1_0-20060110-C, 10 January 2006
// OMA-TS-Presence_SIMPLE_XDM-V1_0-20060418-C, 18 April 2006

// ----------------------------------------------------
// CXcapOmaPresRulesUsage::CXcapOmaPresRulesUsage
// 
// ----------------------------------------------------
//
CXcapOmaPresRulesUsage::CXcapOmaPresRulesUsage( const CXdmEngine& aXdmEngine ) :
CXcapAppUsage( aXdmEngine ),
iIetfPresRules( NULL ),
iIetfCommonPolicy( NULL ),
iOmaCommonPolicy( NULL )
    {
    }

// ----------------------------------------------------
// CXcapOmaPresRulesUsage::NewL
// 
// ----------------------------------------------------
//
CXcapOmaPresRulesUsage* CXcapOmaPresRulesUsage::NewL( const TXcapAppUsageParams& aParameters )
    {
    CXcapOmaPresRulesUsage* self = new ( ELeave ) CXcapOmaPresRulesUsage( aParameters.iXdmEngine );
    CleanupStack::PushL( self );	// << self
    self->ConstructL();
    CleanupStack::Pop( self );      // >> self
    return self;
    }

// ----------------------------------------------------
// CXcapOmaPresRulesUsage::~CXcapOmaPresRulesUsage
// 
// ----------------------------------------------------
//
CXcapOmaPresRulesUsage::~CXcapOmaPresRulesUsage()
    {
    delete iIetfPresRules;
    delete iIetfCommonPolicy;
    delete iOmaCommonPolicy;
    }
        
// ----------------------------------------------------
// CXcapOmaPresRulesUsage::ConstructL
// 
// ----------------------------------------------------
//
void CXcapOmaPresRulesUsage::ConstructL()
    {
    iIetfPresRules = CXcapAppUsage::NewL( iXdmEngine, KXdmIetfPresRulesUsageUid );
    iIetfCommonPolicy = CXcapAppUsage::NewL( iXdmEngine, KXdmIetfCommonPolicyUsageUid );
    iOmaCommonPolicy = CXcapAppUsage::NewL( iXdmEngine, KXdmOmaCommonPolicyUsageUid );
    }

// ----------------------------------------------------
// CXcapOmaPresRulesUsage::AUID
// 
// ----------------------------------------------------
//
TPtrC8 CXcapOmaPresRulesUsage::AUID() const
    {
    return TPtrC8( KXdmOmaPresRulesUsageAUID );
    }
                
// ----------------------------------------------------
// CXcapOmaPresRulesUsage::ContentType
// 
// ----------------------------------------------------
//
TPtrC8 CXcapOmaPresRulesUsage::ContentType() const
    {
    return TPtrC8( KXdmOmaPresRulesUsageContType );
    }
    
// ----------------------------------------------------
// CXcapOmaPresRulesUsage::DefaultNamespace
// 
// ----------------------------------------------------
//
TPtrC8 CXcapOmaPresRulesUsage::DefaultNamespace() const
    {
    //return TPtrC8( KXdmOmaPresRulesNamespace );
    // NOTE the default namespace is IetfCommonPolicy
    return TPtrC8( KXdmIetfCommonPolicyNamespace );
    }    

// ----------------------------------------------------
// CXcapOmaPresRulesUsage::ValidateL
// 
// ----------------------------------------------------
//
TBool CXcapOmaPresRulesUsage::ValidateNodeL( CXdmDocumentNode& aXdmNode )
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
     
    // <provide-willigness>
    if ( Match( element, KXdmProvideWillingness ) )
        {
        dataType = EDataTypeBoolean;
        found = ETrue;
        }
    // <provide-network-availability>
    else if ( Match( element, KXdmProvideNetworkAvailability ) )
        {
        dataType = EDataTypeBoolean;
        found = ETrue;
        }
    // <provide-session-participation>
    else if ( Match( element, KXdmProvideSessionParticipation ) )
        {
        dataType = EDataTypeBoolean;
        found = ETrue;
        }
    // <provide-registration-state>
    else if ( Match( element, KXdmProvideRegistrationState ) )
        {
        dataType = EDataTypeBoolean;
        found = ETrue;
        }
    // <provide-barring-state>
    else if ( Match( element, KXdmProvideBarringState ) )
        {
        dataType = EDataTypeBoolean;
        found = ETrue;
        }
    // <provide-geopriv>
    else if ( Match( element, KXdmProvideGeopriv ) )
        {
        // value must be "false" or "full"
        // 0 (false), 1 (full)
        TPtrC8 data = aXdmNode.LeafNodeContent();
        if ( data.CompareF( KXdmFalse )
             && data.CompareF( KXdmFull ) )
            {
            LeaveWithErrorL( KXcapErrorEnumerationRule );
            }
        found = ETrue;
        }
    // <service-id>
    else if ( Match( element, KXdmServiceId ) )
        {
        dataType = EDataTypeString;
        found = ETrue;
        }
        
    // NOTE the default namespace is IetfCommonPolicy
    if ( found )
        {
        SetPrefixL( aXdmNode, KXdmOmaPresRulesNsPrefix );
        }

    // urn:ietf:params:xml:ns:pres-rules
    // urn:ietf:params:xml:ns:common-policy
    // urn:oma:xml:xdm:common-policy
    else
        {
        // urn:ietf:params:xml:ns:common-policy
        found = iIetfCommonPolicy->ValidateNodeL( aXdmNode );
        if ( found )
            {
            // this means that this belongs to urn:ietf:params:xml:ns:common-policy,
            // there may be the prefix already
            // NOTE the default namespace is IetfCommonPolicy
                SetPrefixL( aXdmNode, KNullDesC8 );
                }
        else
            {
            // urn:ietf:params:xml:ns:pres-rules
            found = iIetfPresRules->ValidateNodeL( aXdmNode );
            if ( found )
                {
                SetPrefixL( aXdmNode, KXdmIetfPresRulesNsPrefix );
                }
            else
                {
                // urn:oma:xml:xdm:common-policy
                found = iOmaCommonPolicy->ValidateNodeL( aXdmNode );
                if ( found )
                    {
                    SetPrefixL( aXdmNode, KXdmOmaCommonPolicyNsPrefix );
                    }
                }
            }
        }
        
    if ( dataType != EDataTypeUndefined )
        {
        // pass to the base class for data validation
        ValidateDataL( dataType, aXdmNode );
        }
    return found;
    }

// ----------------------------------------------------
// CXcapOmaPresRulesUsage::ValidateAttributeL
// 
// ----------------------------------------------------
//
void CXcapOmaPresRulesUsage::ValidateAttributeL( const CXdmNodeAttribute& aXdmNodeAttr )
    {
    // This method is called by base class for each attribute
    // in document, here we have to define data types
    // for attributes, and pass them to the base class
    // for the actual data checking.
    
    // there aren't any attributes in
    // oma presence rules usage schema
    // urn:ietf:params:xml:ns:pres-rules
    // urn:ietf:params:xml:ns:common-policy
    iIetfPresRules->ValidateAttributeL( aXdmNodeAttr );
    }   
                         
// ----------------------------------------------------
// CXcapOmaPresRulesUsage::AddNamespaceInformationL
// 
// ----------------------------------------------------
//
void CXcapOmaPresRulesUsage::AddNamespaceInformationL( CXdmDocument& aXdmDocument )
    {
    // NOTE the default namespace is IetfCommonPolicy
    aXdmDocument.AppendNamespaceL( KXdmIetfCommonPolicyNamespace, KNullDesC8 );
    aXdmDocument.AppendNamespaceL( KXdmOmaCommonPolicyNamespace, KXdmOmaCommonPolicyNsPrefix );
    aXdmDocument.AppendNamespaceL( KXdmOmaPresRulesNamespace, KXdmOmaPresRulesNsPrefix );
    aXdmDocument.AppendNamespaceL( KXdmIetfPresRulesNamespace, KXdmIetfPresRulesNsPrefix );
/*    aXdmDocument.AppendNamespaceL( KXdmOmaPresRulesNamespace, KNullDesC8 );
    aXdmDocument.AppendNamespaceL( KXdmIetfPresRulesNamespace, KXdmIetfPresRulesNsPrefix );
    aXdmDocument.AppendNamespaceL( KXdmIetfCommonPolicyNamespace, KXdmIetfCommonPolicyNsPrefix );*/
    }

// End of File

