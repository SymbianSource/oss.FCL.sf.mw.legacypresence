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
* Description:  CXcapPocUserAccessUsage
*
*/




#include <XdmErrors.h>
#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include "XcapPocUserAccessUsage.h"

// Used specification
// OMA-TS-PoC_XDM-V1_0-20050317-C, 17 March 2005
// OMA-TS-PoC_XDM-V1_0-20060127-C, 27 January 2006

// ----------------------------------------------------
// CXcapPocUserAccessUsage::CXcapPocUserAccessUsage
// 
// ----------------------------------------------------
//
CXcapPocUserAccessUsage::CXcapPocUserAccessUsage( const CXdmEngine& aXdmEngine ) :
CXcapAppUsage( aXdmEngine ),
iIetfCommonPolicies( NULL ),
iOmaCommonPolicies( NULL )
    {
    }

// ----------------------------------------------------
// CXcapPocUserAccessUsage::NewL
// 
// ----------------------------------------------------
//
CXcapPocUserAccessUsage* CXcapPocUserAccessUsage::NewL( const TXcapAppUsageParams& aParameters )
    {
    CXcapPocUserAccessUsage* self = new ( ELeave ) CXcapPocUserAccessUsage( aParameters.iXdmEngine );
    CleanupStack::PushL( self );	// << self
    self->ConstructL();
    CleanupStack::Pop( self );      // >> self
    return self;
    }

// ----------------------------------------------------
// CXcapPocUserAccessUsage::~CXcapPocUserAccessUsage
// 
// ----------------------------------------------------
//
CXcapPocUserAccessUsage::~CXcapPocUserAccessUsage()
    {
    delete iIetfCommonPolicies;
    delete iOmaCommonPolicies;
    }
        
// ----------------------------------------------------
// CXcapPocUserAccessUsage::ConstructL
// 
// ----------------------------------------------------
//
void CXcapPocUserAccessUsage::ConstructL()
    {
    iIetfCommonPolicies = CXcapAppUsage::NewL( iXdmEngine, EXdmIetfCommonPolicy );
    iOmaCommonPolicies = CXcapAppUsage::NewL( iXdmEngine, EXdmOmaCommonPolicy );
    }

// ----------------------------------------------------
// CXcapPocUserAccessUsage::AUID
// 
// ----------------------------------------------------
//
TPtrC8 CXcapPocUserAccessUsage::AUID() const
    {
    return TPtrC8( KXdmPocUserAccessUsageAUID );
    }

// ----------------------------------------------------
// CXcapPocUserAccessUsage::ContentType
// 
// ----------------------------------------------------
//
TPtrC8 CXcapPocUserAccessUsage::ContentType() const
    {
    return TPtrC8( KXdmPocUserAccessUsageContType );
    }

// ----------------------------------------------------
// CXcapPocUserAccessUsage::DefaultNamespace
// 
// ----------------------------------------------------
//
TPtrC8 CXcapPocUserAccessUsage::DefaultNamespace() const
    {
//    return TPtrC8( KXdmPocUserAccessNamespace );
    // NOTE the default namespace is IetfCommonPolicy
    return TPtrC8( KXdmIetfCommonPolicyNamespace );
    }   
    
// ----------------------------------------------------
// CXcapPocUserAccessUsage::ValidateNodeL
// 
// ----------------------------------------------------
//
TBool CXcapPocUserAccessUsage::ValidateNodeL( CXdmDocumentNode& aXdmNode )
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
    TPtrC element = aXdmNode.NodeName();
    
    // there is restriction for the value 
    // of <allow-invite>
    // check that value is either "reject", "accept" or "pass"
    // 0 (reject), 1 (pass), 2 (accept)
    if ( Match( element, KXdmAllowInvite ) )
        {
        TPtrC8 data = aXdmNode.LeafNodeContent();
        if ( data.CompareF( KXdmReject )
             && data.CompareF( KXdmAccept )
             && data.CompareF( KXdmPass ) )
            {
            LeaveWithErrorL( KXcapErrorEnumerationRule );
            }
        found = ETrue;
        // NOTE the default namespace is IetfCommonPolicy
        SetPrefixL( aXdmNode, KXdmPocUserAccessNsPrefix ); 
        }
    // urn:ietf:params:xml:ns:common-policy
    if ( !found )
        {
        found = iIetfCommonPolicies->ValidateNodeL( aXdmNode );
        // NOTE the default namespace is IetfCommonPolicy
        /*if ( found )
            {
            SetPrefixL( aXdmNode, KXdmIetfCommonPolicyNsPrefix );  
            }*/
        }
    // urn:oma:params:xml:ns:common-policy
    if ( !found )
        {
        found = iOmaCommonPolicies->ValidateNodeL( aXdmNode );
                if ( found )
            {
            SetPrefixL( aXdmNode, KXdmOmaCommonPolicyNsPrefix );   
            }
        }
    return found;
    }
        
// ----------------------------------------------------
// CXcapPocUserAccessUsage::ValidateAttributeL
// 
// ----------------------------------------------------
//
void CXcapPocUserAccessUsage::ValidateAttributeL( const CXdmNodeAttribute& aXdmNodeAttr)
    {
    // This method is called by base class for each attribute
    // in document, here we have to define data types
    // for attributes, and pass them to the base class
    // for the actual data checking.
    
    // there aren't any attributes, all attributes are
    // either from urn:ietf:params:xml:ns:common-policy
    // or from urn:oma:params:xml:ns:common-policy
    
    // add parent element namespace checking when available,
    // now we don't know the attribute's namespace
    iIetfCommonPolicies->ValidateAttributeL( aXdmNodeAttr );
    iOmaCommonPolicies->ValidateAttributeL( aXdmNodeAttr ); 
    }
        

 // ----------------------------------------------------
// CXcapPocUserAccessUsage::AddNamespaceInformationL
// 
// ----------------------------------------------------
//
void CXcapPocUserAccessUsage::AddNamespaceInformationL( CXdmDocument& aXdmDocument )
    {
    aXdmDocument.AppendNamespaceL( KXdmIetfCommonPolicyNamespace, KNullDesC8 );
    aXdmDocument.AppendNamespaceL( KXdmPocUserAccessNamespace, KXdmPocUserAccessNsPrefix );
    aXdmDocument.AppendNamespaceL( KXdmOmaCommonPolicyNamespace, KXdmOmaCommonPolicyNsPrefix );
/*    aXdmDocument.AppendNamespaceL( KXdmPocUserAccessNamespace, KNullDesC8 );
    aXdmDocument.AppendNamespaceL( KXdmIetfCommonPolicyNamespace, KXdmIetfCommonPolicyNsPrefix );
    aXdmDocument.AppendNamespaceL( KXdmOmaCommonPolicyNamespace, KXdmOmaCommonPolicyNsPrefix );*/
    }                           
                           
                           
// End of File
  
