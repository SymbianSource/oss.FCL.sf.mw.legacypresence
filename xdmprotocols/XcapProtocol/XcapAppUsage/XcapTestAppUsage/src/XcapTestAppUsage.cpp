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
* Description:   CXcapTestAppUsage
*
*/




#include "XcapTestAppUsage.h"

// ----------------------------------------------------
// CXcapTestAppUsage::CXcapTestAppUsage
// 
// ----------------------------------------------------
//
CXcapTestAppUsage::CXcapTestAppUsage( const CXdmEngine& aXdmEngine ) :
                                      CXcapAppUsage( aXdmEngine )
    {
    }

// ----------------------------------------------------
// CXcapTestAppUsage::NewL
// 
// ----------------------------------------------------
//
CXcapTestAppUsage* CXcapTestAppUsage::NewL( const TXcapAppUsageParams& aParameters )
    {
    CXcapTestAppUsage* self = new ( ELeave ) CXcapTestAppUsage( aParameters.iXdmEngine );
    CleanupStack::PushL( self );    // << self
    self->ConstructL();
    CleanupStack::Pop( self );      // >> self
    return self;
    }

// ----------------------------------------------------
// CXcapTestAppUsage::~CXcapTestAppUsage
// 
// ----------------------------------------------------
//
CXcapTestAppUsage::~CXcapTestAppUsage()
    {
    }
        
// ----------------------------------------------------
// CXcapTestAppUsage::ConstructL
// 
// ----------------------------------------------------
//
void CXcapTestAppUsage::ConstructL()
    {
    }
            
// ----------------------------------------------------
// CXcapTestAppUsage::AUID
// 
// ----------------------------------------------------
//
TPtrC8 CXcapTestAppUsage::AUID() const
    {
    return TPtrC8( KXdmTestAppUsageAUID );
    }

// ----------------------------------------------------
// CXcapTestAppUsage::ContentType
// 
// ----------------------------------------------------
//
TPtrC8 CXcapTestAppUsage::ContentType() const
    {
    return TPtrC8( KXdmTestAppUsageContType );
    }

// ----------------------------------------------------
// CXcapTestAppUsage::DefaultNamespace
// 
// ----------------------------------------------------
//
TPtrC8 CXcapTestAppUsage::DefaultNamespace() const
    {
    return TPtrC8( KXdmTestAppUsageDefaultNamespace );
    }   
    
// ----------------------------------------------------
// CXcapTestAppUsage::ValidateNodeL
// 
// ----------------------------------------------------
//
TBool CXcapTestAppUsage::ValidateNodeL( CXdmDocumentNode& /*aXdmNode*/ )
    {
    // This method is called by base class for each element
    // in document, here we have to declare every element, check element 
    // datatype, restrictions for values and also do all checking that concerns  
    // the structure of the element. If the datatype is some of
    // the common datatypes defined in xcapappusage.h, the node
    // can pe passed to the base class for value validation.
    // If the node belongs to this namespace, the return value
    // should be true, false otherwise.


    // TBool found ( EFalse );   
    // TDataType dataType ( EDataTypeUndefined );
    // TPtrC element = aXdmNode.NodeName();
     
    // <example-element>
    // if ( Match( element, KExampleElement ) )
        // {
        // supported data types are defined in AppUsageDef
        // dataType = EDataTypeBoolean;
        // found = ETrue;
        // }
    // <example-element2>
    // else if ( Match( element, KExampleElement2 ) )
        // {
           // required attribute attribute
           // if ( !aXdmNode.HasAttribute( KAttribute ) )
           // {
           // LeaveWithErrorL( KXcapErrorMissingRequiredAttribute );
           // }
        // found = ETrue;
        // }   
    // <example-element3>     
    // else if ( Match( element, KExampleElement3 ) )
        // {
        // TInt count( aXdmNode.NodeCount() );
        // for ( TInt i(0); i < count; i++ )
           // {
            // there should not be any other elements than <example>
           // if ( !Match( aXdmNode.ChileNode(i)->NodeName(), KExample ) )
               // {
               // LeaveWithErrorL( KXcapErrorSchemaViolation );
               // }
            // }
        // found = ETrue; 
        //  }
        
   // if ( dataType != EDataTypeUndefined )
        // {
        // pass to the base class for data validation
        // ValidateDataL( dataType, aXdmNode );
        // }
    // return found;
     
    return EFalse;
    }

// ----------------------------------------------------
// CXcapTestAppUsage::ValidateAttributeL
// 
// ----------------------------------------------------
//
void CXcapTestAppUsage::ValidateAttributeL( const CXdmNodeAttribute& /*aXdmNodeAttr*/ )
    {
    // This method is called by base class for each attribute
    // in document, here we have to define data types
    // for attributes, and pass them to the base class
    // for the actual data checking.   
    
    // TDataType dataType ( EDataTypeUndefined );
    // TPtrC attribute = aXdmNodeAttr.NodeName();
        
    // exampleAttr1 
    // if ( Match( attribute, KExampleAttr1 ) )
        // {
        // dataType = EDataTypeString;
        // }
    // exampleAttr2 
    // else if ( Match( attribute, KExampleAttr2 ) )
        // {
        // dataType = EDataTypeString;
        // }
        
    // if ( dataType != EDataTypeUndefined )
        // {
        // pass to base class for data validation
        // ValidateDataL( dataType, aXdmNodeAttr );
        // }   
    }
    
// ----------------------------------------------------
// CXcapTestAppUsage::AddNamespaceInformationL
// 
// ----------------------------------------------------
//
void CXcapTestAppUsage::AddNamespaceInformationL( CXdmDocument& aXdmDocument )
    {
    // default namespace prefix should be empty
    aXdmDocument.AppendNamespaceL( KXdmTestAppUsageDefaultNamespace, KNullDesC8 );
    }

// End of File

