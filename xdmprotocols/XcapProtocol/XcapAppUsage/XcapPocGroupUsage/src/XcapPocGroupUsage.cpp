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
* Description:  CXcapPocGroupUsage
*
*/




#include <XdmErrors.h>
#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include "XcapPocGroupUsage.h"



// Used specification
// OMA-TS-PoC_XDM-V1_0-20060609-A, 09 June 2006
// ----------------------------------------------------
// CXcapPocGroupUsage::CXcapPocGroupUsage
// 
// ----------------------------------------------------
//
CXcapPocGroupUsage::CXcapPocGroupUsage( const CXdmEngine& aXdmEngine ) :
CXcapAppUsage( aXdmEngine ),
iIetfCommonPolicies( NULL ),
iOmaCommonPolicies( NULL ),
iResourceLists( NULL )
    {
    }

// ----------------------------------------------------
// CXcapPocGroupUsage::NewL
// 
// ----------------------------------------------------
//
CXcapPocGroupUsage* CXcapPocGroupUsage::NewL( const TXcapAppUsageParams& aParameters )
    {
    CXcapPocGroupUsage* self = new ( ELeave ) CXcapPocGroupUsage( aParameters.iXdmEngine );
    CleanupStack::PushL( self );    // << self
    self->ConstructL();
    CleanupStack::Pop( self );      // >> self
    return self;
    }

// ----------------------------------------------------
// CXcapPocGroupUsage::~CXcapPocGroupUsage
// 
// ----------------------------------------------------
//
CXcapPocGroupUsage::~CXcapPocGroupUsage()
    {
    delete iIetfCommonPolicies;
    delete iOmaCommonPolicies;
    delete iResourceLists;
    }
        
// ----------------------------------------------------
// CXcapPocGroupUsage::ConstructL
// 
// ----------------------------------------------------
//
void CXcapPocGroupUsage::ConstructL()
    {
    iIetfCommonPolicies = CXcapAppUsage::NewL( iXdmEngine, EXdmIetfCommonPolicy );
    iOmaCommonPolicies = CXcapAppUsage::NewL( iXdmEngine, EXdmOmaCommonPolicy );
    iResourceLists = CXcapAppUsage::NewL( iXdmEngine, EXdmResourceLists );
    }

// ----------------------------------------------------
// CXcapPocGroupUsage::AUID
// 
// ----------------------------------------------------
//
TPtrC8 CXcapPocGroupUsage::AUID() const
    {
    return TPtrC8( KXdmPocGroupUsageAUID );
    }

// ----------------------------------------------------
// CXcapPocGroupUsage::ContentType
// 
// ----------------------------------------------------
//
TPtrC8 CXcapPocGroupUsage::ContentType() const
    {
    return TPtrC8( KXdmPocGroupUsageContType );
    }

// ----------------------------------------------------
// CXcapPocGroupUsage::DefaultNamespace
// 
// ----------------------------------------------------
//
TPtrC8 CXcapPocGroupUsage::DefaultNamespace() const
    {
    return TPtrC8( KXdmPocGroupNamespace );
    }   
    
// ----------------------------------------------------
// CXcapPocGroupUsage::ValidateNodeL
// 
// ----------------------------------------------------
//
TBool CXcapPocGroupUsage::ValidateNodeL( CXdmDocumentNode& aXdmNode )
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
            
    // <group> 
    if ( Match( element, KXdmGroup ) )
        {
        // no other elements than <list-service>
        TInt count( aXdmNode.NodeCount() );
        for ( TInt i(0); i < count; i++ )
            {
            if ( !Match( aXdmNode.ChileNode(i)->NodeName(), KXdmListService ) )
                {
                LeaveWithErrorL( KXcapErrorSchemaViolation );   
                }
            }
        found = ETrue;
        }           
    // <list-service>
    else if ( Match( element, KXdmListService ) )
        {
        // required attribute uri
        if ( !aXdmNode.HasAttribute( KXdmUri ) )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredAttribute );
            }
        found = ETrue;
        }
    // <display-name>   
    else if ( Match( element, KXdmDisplayName ) )
        {
        CXdmDocumentNode* parent = aXdmNode.Parent();
        if ( parent && !parent->NodeName().CompareF( KXdmEntry ) )
            {
            found = iResourceLists->ValidateNodeL( aXdmNode );	
            if ( found )
            	{
            	SetPrefixL( aXdmNode, KXdmResourceListsNsPrefix );  
            	}
            }
        else
            {
        dataType = EDataTypeString;
        found = ETrue;
        }
        }
    // <invite-members>     
    else if ( Match( element, KXdmInviteMembers ) )
        {
        dataType = EDataTypeBoolean;
        found = ETrue;
        }
    // <max-participant-count>
    else if ( Match( element, KXdmMaxParticipantCount ) )
        {
        dataType = EDataTypeNonNegativeInteger;
        found = ETrue;
        }
    // <is-list-member>
    else if ( Match( element, KXdmIsListMember ) )
        {
        // dataType = EDataTypeBoolean; // OMA-TS-PoC_XDM-V1_0-20050317-C, 17 March 2005
        // changed 4-Nov-2005 PoC XDM
        dataType = EDataTypeEmpty;
        found = ETrue;
        }   
    // <allow-conference-state> 
    else if ( Match( element, KXdmAllowConfState ) )
        {
        dataType = EDataTypeBoolean;
        found = ETrue;
        }
    // <allow-invite-users-dynamically>
    else if ( Match( element, KXdmAllowInvUsersDyn ) )
        {
        dataType = EDataTypeBoolean;
        found = ETrue;
        }
    // <join-handling> 
    else if ( Match( element, KXdmJoinHandling ) )
        {
        // OMA-TS-PoC_XDM-V1_0-20050317-C, 17 March 2005
        // the value must be "allow" or "block"
        // 0 (block) and 1 (allow)
        /* TPtrC8 data = aXdmNode.LeafNodeContent();
        if ( data.CompareF( KXdmAllow )
             && data.CompareF( KXdmBlock ) )
            {
            LeaveWithErrorL( KXcapErrorEnumerationRule );
            }
        */
        // changed OMA-TS-PoC_XDM-V1_0-20060127-C, 27 January 2006
        dataType = EDataTypeBoolean;
        found = ETrue;
        }
    // <allow-initiate-conference>
    else if ( Match( element, KXdmAllowInitiateConf ) )
        {
        dataType = EDataTypeBoolean;
        found = ETrue;
        }
    // <allow-anonymity>
    else if ( Match( element, KXdmAllowAnonymity ) )
        {
        dataType = EDataTypeBoolean;
        found = ETrue;
        }
    // <is-key-participant>
    else if ( Match( element, KXdmIsKeyParticipant ) )
        {
        dataType = EDataTypeBoolean;
        found = ETrue;
        }
    // <external> 
    // type = rl:externalType from urn:ietf:params:xml:ns:resource-lists 
    // pass to the appropriate usage for validation
    // but use default namespace
    else if ( Match( element, KXdmExternal ) )
        {
        found = iResourceLists->ValidateNodeL( aXdmNode );
        }
    // <entry> 
    // type = rl:entryType from urn:ietf:params:xml:ns:resource-lists 
    // pass to the appropriate usage for validation
    // but use default namespace
    else if ( Match( element, KXdmEntry ) )    
        {
        found = iResourceLists->ValidateNodeL( aXdmNode );
        }

    // urn:ietf:params:xml:ns:common-policy
    if ( !found )
        {
        found = iIetfCommonPolicies->ValidateNodeL( aXdmNode );
        if ( found )
            {
            SetPrefixL( aXdmNode, KXdmIetfCommonPolicyNsPrefix );  
            }
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
        
    // if the node is some of the basic types,
    // pass it to the base class for data validation
    if ( dataType != EDataTypeUndefined )
        {
        ValidateDataL( dataType, aXdmNode );
        }
    return found;   
    }

// ----------------------------------------------------
// CXcapPocGroupUsage::ValidateAttributeL
// 
// ----------------------------------------------------
//
void CXcapPocGroupUsage::ValidateAttributeL( const CXdmNodeAttribute& aXdmNodeAttr )
    {
    // This method is called by base class for each attribute
    // in document, here we have to define data types
    // for attributes, and pass them to the base class
    // for the actual data checking.

    TPtrC attribute = aXdmNodeAttr.NodeName();
    
    // uri
    if ( Match( attribute, KXdmUri ) ) 
        {
        // pass to base class for data validation
        // check if this should be SIP uri
        ValidateDataL( EDataTypeAnyURI, aXdmNodeAttr ); 
        }
    else
        {
        // add parent element namespace checking when available,
        // now we don't know the attribute's namespace
        iIetfCommonPolicies->ValidateAttributeL( aXdmNodeAttr );
        iOmaCommonPolicies->ValidateAttributeL( aXdmNodeAttr ); 
        }
    }
                            
// ----------------------------------------------------
// CXcapPocGroupUsage::AddNamespaceInformationL
// 
// ----------------------------------------------------
//
void CXcapPocGroupUsage::AddNamespaceInformationL( CXdmDocument& aXdmDocument )
    {
    aXdmDocument.AppendNamespaceL( KXdmPocGroupNamespace, KNullDesC8 );
    aXdmDocument.AppendNamespaceL( KXdmResourceListsNamespace, KXdmResourceListsNsPrefix );
    aXdmDocument.AppendNamespaceL( KXdmIetfCommonPolicyNamespace, KXdmIetfCommonPolicyNsPrefix );
    aXdmDocument.AppendNamespaceL( KXdmOmaCommonPolicyNamespace, KXdmOmaCommonPolicyNsPrefix );
    }


// End of File

