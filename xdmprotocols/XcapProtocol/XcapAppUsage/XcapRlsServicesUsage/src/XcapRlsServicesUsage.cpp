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
* Description:  CXcapRlsServicesUsage
*
*/




#include <XdmErrors.h>
#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include "XcapRlsServicesUsage.h"

// Used specification
// OMA-RLS_XDM_Specification-V1_0-20060214-C, 14 February 2006

// ----------------------------------------------------
// CXcapRlsServicesUsage::CXcapRlsServicesUsage
// 
// ----------------------------------------------------
//
CXcapRlsServicesUsage::CXcapRlsServicesUsage( const CXdmEngine& aXdmEngine ) :
CXcapAppUsage( aXdmEngine ),
iResourceLists( NULL )
    {
    }

// ----------------------------------------------------
// CXcapRlsServicesUsage::NewL
// 
// ----------------------------------------------------
//
CXcapRlsServicesUsage* CXcapRlsServicesUsage::NewL( const TXcapAppUsageParams& aParameters )
    {
    CXcapRlsServicesUsage* self = new ( ELeave ) CXcapRlsServicesUsage( aParameters.iXdmEngine );
    CleanupStack::PushL( self );	// << self
    self->ConstructL();
    CleanupStack::Pop( self );      // >> self
    return self;
    }

// ----------------------------------------------------
// CXcapRlsServicesUsage::~CXcapRlsServicesUsage
// 
// ----------------------------------------------------
//
CXcapRlsServicesUsage::~CXcapRlsServicesUsage()
    {
    delete iResourceLists;
    }
        
// ----------------------------------------------------
// CXcapRlsServicesUsage::ConstructL
// 
// ----------------------------------------------------
//
void CXcapRlsServicesUsage::ConstructL()
    {
    iResourceLists = CXcapAppUsage::NewL( iXdmEngine, KXdmResourceListsUsageUid );
    }

// ----------------------------------------------------
// CXcapRlsServicesUsage::AUID
// 
// ----------------------------------------------------
//
TPtrC8 CXcapRlsServicesUsage::AUID() const
    {
    return TPtrC8( KXdmRlsServicesUsageAUID );
    }
                
// ----------------------------------------------------
// CXcapRlsServicesUsage::ContentType
// 
// ----------------------------------------------------
//
TPtrC8 CXcapRlsServicesUsage::ContentType() const
    {
    return TPtrC8( KXdmRlsServicesUsageContType );
    }

// ----------------------------------------------------
// CXcapRlsServicesUsage::DefaultNamespace
// 
// ----------------------------------------------------
//
TPtrC8 CXcapRlsServicesUsage::DefaultNamespace() const
    {
    return TPtrC8( KXdmRlsServicesNamespace );
    }   
    
// ----------------------------------------------------
// CXcapRlsServicesUsage::ValidateNodeL
// 
// ----------------------------------------------------
//
TBool CXcapRlsServicesUsage::ValidateNodeL( CXdmDocumentNode& aXdmNode )
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
    // <rls-services>
    if ( Match( element, KXdmRlsServices ) )   
        {
        found = ETrue;
        }
     // <service>
    else if ( Match( element, KXdmService ) )    
        {
        // required attribute uri
        if ( !aXdmNode.HasAttribute( KXdmUri ) )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredAttribute );
            }
            
        // there should be either <resource-list> or
        // <list> element and mandatory <packages> element
        TBool resourcelist( EFalse );
        TBool list( EFalse );
        TBool packages( EFalse );
        // go through all childs
        TInt count( aXdmNode.NodeCount() );
        for ( TInt i(0); i < count; i++ )
            {
            TPtrC childName = aXdmNode.ChileNode(i)->NodeName();
            if ( Match( childName, KXdmResourceList ) )  
                {
                resourcelist = ETrue;
                }
            else if ( Match( childName, KXdmList ) ) 
                {
                list = ETrue;
                }
            else if ( Match( childName, KXdmPackages ) ) 
                {
                packages = ETrue;
                }
            }
        // <resource-list> or <list>, not both
        if ( resourcelist && list )
            {
            LeaveWithErrorL( KXcapErrorSchemaViolation );   
            }
        // each <service> must have at least one <packages> element
        if ( !packages )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredElement );    
            }
        found = ETrue;
        }
    // <resource-list>
    else if ( Match( element, KXdmResourceList ) )   
        {
        dataType = EDataTypeAnyURI;
        found = ETrue;
        }
    // <packages>
    else if ( Match( element, KXdmPackages ) )   
        {
        // each <packages> shall specify at least presence event package,
        // go through all childs
        TBool presence ( EFalse );
        TInt count( aXdmNode.NodeCount() );
        for ( TInt i(0); i < count; i++ )
            {   
            if ( Match( aXdmNode.ChileNode(i)->NodeName(), KXdmPackage ) ) 
                {
                TPtrC8 data = aXdmNode.ChileNode(i)->LeafNodeContent();
                if ( !data.CompareF( KXdmPresence ) )
                    {
                    presence = ETrue;
                    }
                }
            }
        // check that the <package>presence<package> exists
        if ( !presence )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredElementValue );   
            }
        found = ETrue;
        }
    // <package>
    else if ( Match( element, KXdmPackage ) )   
        {
        dataType = EDataTypeString;
        found = ETrue;
        }
    // <list> type=rl:listType from urn:ietf:params:xml:ns:resource-lists
    // however belongs to the default namespace
    // ( btw, there is mistake in specification example )
    else if ( Match( element, KXdmList ) ) 
        {
        found = iResourceLists->ValidateNodeL( aXdmNode );
        }
    // <entry>,<display-name> from urn:ietf:params:xml:ns:resource-lists
    if ( !found ) 
        {
        found = iResourceLists->ValidateNodeL( aXdmNode );
        if ( found )
            {
            SetPrefixL( aXdmNode, KXdmResourceListsNsPrefix );   
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
// CXcapRlsServicesUsage::ValidateAttributeL
// 
// ----------------------------------------------------
//
void CXcapRlsServicesUsage::ValidateAttributeL( const CXdmNodeAttribute& aXdmNodeAttr )
    {
    // This method is called by base class for each attribute
    // in document, here we have to define data types
    // for attributes, and pass them to the base class
    // for the actual data checking.
    
    // uri 
    if ( Match( aXdmNodeAttr.NodeName(), KXdmUri ) )
        {
        ValidateDataL( EDataTypeAnyURI, aXdmNodeAttr );
        }
    // the rest are rom urn:ietf:params:xml:ns:resource-lists
    else
        {
        iResourceLists->ValidateAttributeL( aXdmNodeAttr ); 
        }
    }   
                         
// ----------------------------------------------------
// CXcapRlsServicesUsage::AddNamespaceInformationL
// 
// ----------------------------------------------------
//
void CXcapRlsServicesUsage::AddNamespaceInformationL( CXdmDocument& aXdmDocument )
    {
    aXdmDocument.AppendNamespaceL( KXdmRlsServicesNamespace, KNullDesC8 );
    aXdmDocument.AppendNamespaceL( KXdmResourceListsNamespace, KXdmResourceListsNsPrefix );
    }


// End of File

