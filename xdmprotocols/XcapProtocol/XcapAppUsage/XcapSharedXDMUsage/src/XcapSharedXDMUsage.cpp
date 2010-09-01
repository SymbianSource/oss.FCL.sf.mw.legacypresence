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
* Description:   CXcapSharedXDMUsage
*
*/




#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include "XcapSharedXDMUsage.h"

// Used spesifications
// OMA-TS-XDM_Core-V1_0-20051103-C, 03 November 2005
// OMA-TS-XDM_Shared-V1_0-20051006-C, 12 June 2006

// ----------------------------------------------------
// CXcapSharedXDMUsage::CXcapSharedXDMUsage
// 
// ----------------------------------------------------
//
CXcapSharedXDMUsage::CXcapSharedXDMUsage( 
const CXdmEngine& aXdmEngine ) : CXcapAppUsage( aXdmEngine ),
iResListUsage( NULL )
    {
    }

// ----------------------------------------------------
// CXcapSharedXDMUsage::NewL
// 
// ----------------------------------------------------
//
CXcapSharedXDMUsage* CXcapSharedXDMUsage::NewL( const TXcapAppUsageParams& aParameters )
    {
    CXcapSharedXDMUsage* self = new ( ELeave ) CXcapSharedXDMUsage( aParameters.iXdmEngine );
    CleanupStack::PushL( self );	// << self
    self->ConstructL();
    CleanupStack::Pop( self );      // >> self
    return self;
    }

// ----------------------------------------------------
// CXcapSharedXDMUsage::~CXcapSharedXDMUsage
// 
// ----------------------------------------------------
//
CXcapSharedXDMUsage::~CXcapSharedXDMUsage()
    {
	delete iResListUsage;
    }
        
// ----------------------------------------------------
// CXcapSharedXDMUsage::ConstructL
// 
// ----------------------------------------------------
//
void CXcapSharedXDMUsage::ConstructL()
    {
    iResListUsage = CXcapAppUsage::NewL( iXdmEngine, KXdmResourceListsUsageUid );
    }

// ----------------------------------------------------
// CXcapSharedXDMUsage::AUID
// 
// ----------------------------------------------------
//
TPtrC8 CXcapSharedXDMUsage::AUID() const
    {
    return TPtrC8( KXdmSharedXDMUsageAUID );
    }
                
// ----------------------------------------------------
// CXcapSharedXDMUsage::ContentType
// 
// ----------------------------------------------------
//
TPtrC8 CXcapSharedXDMUsage::ContentType() const
    {
    return TPtrC8( KXdmSharedXDMUsageContType );
    }

// ----------------------------------------------------
// CXcapSharedXDMUsage::DefaultNamespace
// 
// ----------------------------------------------------
//
TPtrC8 CXcapSharedXDMUsage::DefaultNamespace() const
    {
    // we use urn:ietf:params:xml:ns:resource-lists 
    // as a default namespace
    return TPtrC8( KXdmResourceListsNamespace );
    }   
    
// ----------------------------------------------------
// CXcapSharedXDMUsage::ValidateNodeL
// 
// ----------------------------------------------------
//
TBool CXcapSharedXDMUsage::ValidateNodeL( CXdmDocumentNode& aXdmNode )
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
  
    // <uriusages>
    if ( Match( element, KXdmUriUsages ) )    
        {
        SetPrefixL( aXdmNode, KXdmSharedXDMUriUsageNsPrefix );
        found = ETrue;
        }
    // <uriusage>
    else if ( Match( element, KXdmUriUsage ) )    
        {
        SetPrefixL( aXdmNode, KXdmSharedXDMUriUsageNsPrefix );
        found = ETrue;
        }
    // the rest are from urn:ietf:params:xml:ns:resource-lists
    else
        {
        // do not append the namespace prefix here,
        // since the used default namespace is
        // urn:ietf:params:xml:ns:resource-lists
        found = iResListUsage->ValidateNodeL( aXdmNode );
        }
    return found;
    }

// ----------------------------------------------------
// CXcapSharedXDMUsage::ValidateAttributeL
// 
// ----------------------------------------------------
//
void CXcapSharedXDMUsage::ValidateAttributeL( const CXdmNodeAttribute& aXdmNodeAttr )
    {
    // This method is called by base class for each attribute
    // in document, here we have to define data types
    // for attributes, and pass them to the base class
    // for the actual data checking.

    // this usage doesn't have any attributes,
    // all attributes are from urn:ietf:params:xml:ns:resource-lists
    iResListUsage->ValidateAttributeL( aXdmNodeAttr );
   
    }   
                         
// ----------------------------------------------------
// CXcapSharedXDMUsage::AddNamespaceInformationL
// 
// ----------------------------------------------------
//
void CXcapSharedXDMUsage::AddNamespaceInformationL( CXdmDocument& aXdmDocument )
    {
    // we use urn:ietf:params:xml:ns:resource-lists 
    // as a default namespace
    aXdmDocument.AppendNamespaceL( KXdmResourceListsNamespace, KNullDesC8 );
    aXdmDocument.AppendNamespaceL( KXdmSharedXDMUriUsageNamespace, KXdmSharedXDMUriUsageNsPrefix );
    }

// End of File

