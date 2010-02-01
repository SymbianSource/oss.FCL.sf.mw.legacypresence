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
* Description:  CXcapDirectoryUsage
*
*/




#include <XdmErrors.h>
#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include "XcapDirectoryUsage.h"


// Used specification
// OMA-TS-XDM_Core-V1_0-20051103-C, 03 November 2005

// ----------------------------------------------------
// CXcapDirectoryUsage::CXcapDirectoryUsage
// 
// ----------------------------------------------------
//
CXcapDirectoryUsage::CXcapDirectoryUsage( const CXdmEngine& aXdmEngine ) :
                                          CXcapAppUsage( aXdmEngine )
    {
    }

// ----------------------------------------------------
// CXcapDirectoryUsage::NewL
// 
// ----------------------------------------------------
//
CXcapDirectoryUsage* CXcapDirectoryUsage::NewL( const TXcapAppUsageParams& aParameters )
    {
    CXcapDirectoryUsage* self = new ( ELeave ) CXcapDirectoryUsage( aParameters.iXdmEngine );
    CleanupStack::PushL( self );    // << self
    self->ConstructL();
    CleanupStack::Pop( self );      // >> self
    return self;
    }

// ----------------------------------------------------
// CXcapDirectoryUsage::~CXcapDirectoryUsage
// 
// ----------------------------------------------------
//
CXcapDirectoryUsage::~CXcapDirectoryUsage()
    {
    }
        
// ----------------------------------------------------
// CXcapDirectoryUsage::ConstructL
// 
// ----------------------------------------------------
//
void CXcapDirectoryUsage::ConstructL()
    {
    }

// ----------------------------------------------------
// CXcapDirectoryUsage::AUID
// 
// ----------------------------------------------------
//
TPtrC8 CXcapDirectoryUsage::AUID() const
    {
    return TPtrC8( KXdmDirectoryUsageAUID );
    }
                 
// ----------------------------------------------------
// CXcapDirectoryUsage::ContentType
// 
// ----------------------------------------------------
//
TPtrC8 CXcapDirectoryUsage::ContentType() const
    {
    return TPtrC8( KXdmDirectoryUsageContType );
    }

// ----------------------------------------------------
// CXcapDirectoryUsage::DefaultNamespace
// 
// ----------------------------------------------------
//
TPtrC8 CXcapDirectoryUsage::DefaultNamespace() const
    {
    return TPtrC8( KXdmDirectoryNamespace );
    }   
    
// ----------------------------------------------------
// CXcapDirectoryUsage::ValidateNodeL
// 
// ----------------------------------------------------
//
TBool CXcapDirectoryUsage::ValidateNodeL( CXdmDocumentNode& aXdmNode ) 
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
    TPtrC element = aXdmNode.NodeName();
        
    // <xcap-directory>
    if ( Match( element, KXdmXcapDirectory ) ) 
        {
        found = ETrue;  
        }
    // <folder>
    else if ( Match( element, KXdmFolder ) )
        {
        // required attribute auid
        if ( !aXdmNode.HasAttribute( KXdmAuid ) )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredAttribute );
            }
        // there should be either <entry> or <error-code>
        // element, not both
        TBool entry( EFalse );
        TBool errorcode( EFalse ) ;
        TInt count( aXdmNode.NodeCount() );
        for ( TInt i(0); i < count; i++ )
            {
            TPtrC childName = aXdmNode.ChileNode(i)->NodeName();            
            if ( childName.CompareF( KXdmEntry ) ) 
                {
                entry = ETrue;
                }
            else if ( childName.CompareF( KXdmErrorCode ) ) 
                {
                errorcode = ETrue;
                }
            }
        if ( entry && errorcode )   
            {
            LeaveWithErrorL( KXcapErrorSchemaViolation );   
            }
        found = ETrue;  
        }
    // <entry>
    else if ( Match( element, KXdmEntry ) )
        {
        // required attributes uri and etag
        if ( !aXdmNode.HasAttribute( KXdmUri ) )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredAttribute );
            }
        if ( !aXdmNode.HasAttribute( KXdmEtag ) )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredAttribute);
            }
        found = ETrue;  
        }
    // <error-code>
    else if ( Match( element, KXdmErrorCode ) ) 
        {
        ValidateDataL( EDataTypeString, aXdmNode );
        found = ETrue;  
        }
    return found;
    }
        
// ----------------------------------------------------
// CXcapDirectoryUsage::ValidateAttributeL
// 
// ----------------------------------------------------
//
void CXcapDirectoryUsage::ValidateAttributeL( const CXdmNodeAttribute& aXdmNodeAttr )
    {
    // This method is called by base class for each attribute
    // in document, here we have to define data types
    // for attributes, and pass them to the base class
    // for the actual data checking.
    
    TDataType dataType ( EDataTypeUndefined );
    TPtrC attribute = aXdmNodeAttr.NodeName();
    // uri
    if ( Match( attribute, KXdmUri ) )
        {
        dataType = EDataTypeAnyURI;
        }
    // etag
    else if ( Match( attribute, KXdmEtag ) )
        {
        dataType = EDataTypeString;
        }
    // last-modified
    else if ( Match( attribute, KXdmLastModified ) )
        {
        dataType = EDataTypeDateTime;
        }
    // size
    else if ( Match( attribute, KXdmSize ) )
        {
        dataType = EDataTypeNonNegativeInteger;
        }
    // auid
    else if ( Match( attribute, KXdmAuid ) )
        {
        dataType = EDataTypeString;
        }
    // pass to base class for data validation
    ValidateDataL( dataType, aXdmNodeAttr );
    }

// ----------------------------------------------------
// CXcapDirectoryUsage::AddNamespaceInformationL
// 
// ----------------------------------------------------
//
void CXcapDirectoryUsage::AddNamespaceInformationL( CXdmDocument& aXdmDocument )
    {
    aXdmDocument.AppendNamespaceL( KXdmDirectoryNamespace, KNullDesC8 );
    }
    
// End of File

