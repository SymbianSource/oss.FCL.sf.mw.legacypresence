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
* Description:   CXcapResourceListsUsage
*
*/




#include <XdmErrors.h>
#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include "XcapResourceListsUsage.h"

// Used specification
// draft-ietf-simple-xcap-list-usage-05, 7 February 2005
// expires 8 August 2005

// ----------------------------------------------------
// CXcapResourceListsUsage::CXcapResourceListsUsage
// 
// ----------------------------------------------------
//
CXcapResourceListsUsage::CXcapResourceListsUsage( const CXdmEngine& aXdmEngine ) :
                                            CXcapAppUsage( aXdmEngine )
    {
    }

// ----------------------------------------------------
// CXcapResourceListsUsage::NewL
// 
// ----------------------------------------------------
//
CXcapResourceListsUsage* CXcapResourceListsUsage::NewL( const TXcapAppUsageParams& aParameters )
    {
    CXcapResourceListsUsage* self = new ( ELeave ) CXcapResourceListsUsage( aParameters.iXdmEngine );
    CleanupStack::PushL( self );	// << self
    self->ConstructL();
    CleanupStack::Pop( self );      // >> self
    return self;
    }

// ----------------------------------------------------
// CXcapResourceListsUsage::~CXcapResourceListsUsage
// 
// ----------------------------------------------------
//
CXcapResourceListsUsage::~CXcapResourceListsUsage()
    {
    }
        
// ----------------------------------------------------
// CXcapResourceListsUsage::ConstructL
// 
// ----------------------------------------------------
//
void CXcapResourceListsUsage::ConstructL()
    {
    }

// ----------------------------------------------------
// CXcapResourceListsUsage::AUID
// 
// ----------------------------------------------------
//
TPtrC8 CXcapResourceListsUsage::AUID() const
    {
    return TPtrC8( KXdmResourceListsUsageAUID );
    }
                
// ----------------------------------------------------
// CXcapResourceListsUsage::ContentType
// 
// ----------------------------------------------------
//
TPtrC8 CXcapResourceListsUsage::ContentType() const
    {
    return TPtrC8( KXdmResourceListsUsageContType );
    }

// ----------------------------------------------------
// CXcapResourceListsUsage::DefaultNamespace
// 
// ----------------------------------------------------
//
TPtrC8 CXcapResourceListsUsage::DefaultNamespace() const
    {
    return TPtrC8( KXdmResourceListsNamespace );
    }   
    
// ----------------------------------------------------
// CXcapResourceListsUsage::ValidateL
// 
// ----------------------------------------------------
//
TBool CXcapResourceListsUsage::ValidateNodeL( CXdmDocumentNode& aXdmNode )
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

    // <resource-lists> 
    if ( Match( element, KXdmResourceLists) )    
        {
        TInt count( aXdmNode.NodeCount() );
        for ( TInt i(0); i < count; i++ )
            {
            // there should not be any other elements than <list>
            if ( !Match( aXdmNode.ChileNode(i)->NodeName(), KXdmList ) )
                {
                LeaveWithErrorL( KXcapErrorSchemaViolation );
                }
            }
        found = ETrue;
        }
    // <list> 
    else if ( Match( element, KXdmList ) )    
        {
        found = ETrue;
        }
    // <entry> 
    else if ( Match( element, KXdmEntry ) )    
        {
        // required attribute uri
        if ( !aXdmNode.HasAttribute( KXdmUri ) )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredAttribute );
            }
        found = ETrue;
        }
    // <entry-ref> 
    else if ( Match( element, KXdmEntryRef ) )    
        {
        if ( !aXdmNode.HasAttribute( KXdmRef ) )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredAttribute );
            }
        found = ETrue;
        }
    // <display-name> 
    else if ( Match( element, KXdmDisplayName ) )    
        {
        dataType = EDataTypeString;
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
// CXcapResourceListsUsage::ValidateAttributeL
// 
// ----------------------------------------------------
//
void CXcapResourceListsUsage::ValidateAttributeL( const CXdmNodeAttribute& aXdmNodeAttr )
    {
    // This method is called by base class for each attribute
    // in document, here we have to define data types
    // for attributes, and pass them to the base class
    // for the actual data checking.
    
    TDataType dataType ( EDataTypeUndefined );
    TPtrC attribute = aXdmNodeAttr.NodeName();
        
    // name 
    if ( Match( attribute, KXdmName ) )
        {
        dataType = EDataTypeString;
        }
    // uri 
    else if ( Match( attribute, KXdmUri ) )
        {
        dataType = EDataTypeString;
        }
    // ref 
    else if ( Match( attribute, KXdmRef ) )
        {
        dataType = EDataTypeAnyURI;
        }
    // anchor
    else if ( Match( attribute, KXdmAnchor ) )
        {
        dataType = EDataTypeAnyURI;
        }   
        
    if ( dataType != EDataTypeUndefined )
        {
        // pass to base class for data validation
        ValidateDataL( dataType, aXdmNodeAttr );
        }   
    }   
                         
// ----------------------------------------------------
// CXcapResourceListsUsage::AddNamespaceInformationL
// 
// ----------------------------------------------------
//
void CXcapResourceListsUsage::AddNamespaceInformationL( CXdmDocument& aXdmDocument )
    {
    aXdmDocument.AppendNamespaceL( KXdmResourceListsNamespace, KNullDesC8 );
    }

// End of File

