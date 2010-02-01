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
* Description:  CXcapIetfPresRulesUsage
*
*/




#include "XcapIetfPresRulesUsage.h"
#include "XdmDocumentNode.h"
#include "XdmNodeAttribute.h"
#include "XdmErrors.h"

// Used specification
// draft-ietf-simple-presence-rules-03, 18 July 2005
// expires 19 January, 2006
// draft-ietf-simple-presence-rules-05, 06 March 2006
// expires 07 September, 2006

// ----------------------------------------------------
// CXcapIetfPresRulesUsage::CXcapIetfPresRulesUsage
// 
// ----------------------------------------------------
//
CXcapIetfPresRulesUsage::CXcapIetfPresRulesUsage( const CXdmEngine& aXdmEngine ) :
CXcapAppUsage( aXdmEngine ),
iIetfCommonPolicy( NULL )
    {
    }

// ----------------------------------------------------
// CXcapIetfPresRulesUsage::NewL
// 
// ----------------------------------------------------
//
CXcapIetfPresRulesUsage* CXcapIetfPresRulesUsage::NewL( const TXcapAppUsageParams& aParameters )
    {
    CXcapIetfPresRulesUsage* self = new ( ELeave ) CXcapIetfPresRulesUsage( aParameters.iXdmEngine );
    CleanupStack::PushL( self );	// << self
    self->ConstructL();
    CleanupStack::Pop( self );      // >> self
    return self;
    }

// ----------------------------------------------------
// CXcapIetfPresRulesUsage::~CXcapIetfPresRulesUsage
// 
// ----------------------------------------------------
//
CXcapIetfPresRulesUsage::~CXcapIetfPresRulesUsage()
    {
    delete iIetfCommonPolicy;
    }
        
// ----------------------------------------------------
// CXcapIetfPresRulesUsage::ConstructL
// 
// ----------------------------------------------------
//
void CXcapIetfPresRulesUsage::ConstructL()
    {
    iIetfCommonPolicy = CXcapAppUsage::NewL( iXdmEngine, KXdmIetfCommonPolicyUsageUid );
    }

// ----------------------------------------------------
// CXcapIetfPresRulesUsage::AUID
// 
// ----------------------------------------------------
//
TPtrC8 CXcapIetfPresRulesUsage::AUID() const
    {
    return TPtrC8( KXdmIetfPresRulesUsageAUID );
    }
                
// ----------------------------------------------------
// CXcapIetfPresRulesUsage::ContentType
// 
// ----------------------------------------------------
//
TPtrC8 CXcapIetfPresRulesUsage::ContentType() const
    {
    return TPtrC8( KXdmIetfPresRulesUsageContType );
    }
    
// ----------------------------------------------------
// CXcapIetfPresRulesUsage::DefaultNamespace
// 
// ----------------------------------------------------
//
TPtrC8 CXcapIetfPresRulesUsage::DefaultNamespace() const
    {
    return TPtrC8( KXdmIetfPresRulesNamespace );
    }    

// ----------------------------------------------------
// CXcapIetfPresRulesUsage::ValidateL
// 
// ----------------------------------------------------
//
TBool CXcapIetfPresRulesUsage::ValidateNodeL( CXdmDocumentNode& aXdmNode )
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
    
    // <service-uri-scheme>       
    if ( Match( element, KXdmServiceUriScheme ) )
        {
        dataType = EDataTypeToken;  
        found = ETrue;
        }
    // <class>
    else if ( Match( element, KXdmClass ) )
        {
        dataType = EDataTypeToken;
        found = ETrue;  
        }
    // <occurence-id>
    else if ( Match( element, KXdmOccurenceId ) ) 
        {
        dataType = EDataTypeToken;  
        found = ETrue;      
        }
    // <service-uri>
    else if ( Match( element, KXdmServiceUri ) )
        {
        dataType = EDataTypeAnyURI; 
        found = ETrue;      
        }
    // <all-services>
    else if ( Match( element, KXdmAllServices ) ) 
        {
        found = ETrue;
        }
    // <provide-services>
    else if ( Match( element, KXdmProvideServices ) )
        {
        // type = provideServicePermissions
        // here we should have either <all-services>
        // or sequence of other elements
        TBool allServices( EFalse );
        TBool sequence( EFalse );
        TBool serviceUri( EFalse );
        TBool serviceUriScheme( EFalse );
        TBool occurenceId( EFalse );
        TBool prclass( EFalse );
        TInt count( aXdmNode.NodeCount() );
        for ( TInt i(0); i < count; i++ )
            {
            TPtrC childName = aXdmNode.ChileNode(i)->NodeName();
            if ( Match( childName, KXdmAllServices ) ) 
                {
                allServices = ETrue;
                }
            else
                {
                sequence = ETrue;
                // in sequence should be either <service-uri>, 
                // <service-uri-scheme>, <occurence-id> or <class>
                if ( Match( childName, KXdmServiceUri ) ) 
                    {
                    serviceUri = ETrue;
                    }
                else if ( Match( childName, KXdmServiceUriScheme ) ) 
                    {
                    serviceUriScheme = ETrue;
                    }
                else if ( Match( childName, KXdmOccurenceId ) ) 
                    {
                    occurenceId = ETrue;
                    }
                else if ( Match( childName, KXdmClass ) ) 
                    {
                    prclass = ETrue;
                    }
                }
            }
        if ( allServices && sequence )
            {
            LeaveWithErrorL( KXcapErrorSchemaViolation );
            }
        if ( sequence ) 
            {
            if ( serviceUri && ( serviceUriScheme || occurenceId || prclass ) )
                {
                LeaveWithErrorL( KXcapErrorSchemaViolation );
                }
            if ( serviceUriScheme && ( occurenceId || prclass ) )
                {
                LeaveWithErrorL( KXcapErrorSchemaViolation );
                }
            if ( occurenceId && prclass )
                {
                LeaveWithErrorL( KXcapErrorSchemaViolation );
                }       
            }
        found = ETrue;
        }
    // used in presence-rules-03
    // <device-id>
    else if ( Match( element, KXdmDeviceId ) )
        {
        dataType = EDataTypeAnyURI; 
        found = ETrue;          
        }
    // used in presence-rules-05
    // <deviceID>
    else if ( Match( element, KXdmDeviceID ) )
        {
        dataType = EDataTypeAnyURI; 
        found = ETrue;          
        }
    // <all-devices>
    else if ( Match( element, KXdmAllDevices ) )
        {
        found = ETrue;
        }
    // <provide-devices>
    else if ( Match( element, KXdmProvideDevices ) )
        {
        // type = provideDevicePermissions
        // here we should have either <all-devices>
        // or sequence of other elements
        TBool allDevices( EFalse );
        TBool sequence( EFalse );
        TBool deviceId( EFalse );
        TBool occurenceId( EFalse );
        TBool prclass( EFalse );
        TInt count( aXdmNode.NodeCount() );
        for ( TInt i(0); i < count; i++ )
            {
            TPtrC childName = aXdmNode.ChileNode(i)->NodeName();
            if ( Match( childName, KXdmAllDevices ) ) 
                {
                allDevices = ETrue;
                }
            else
                {
                sequence = ETrue;
                // in sequence should be either pres-rules-03 <device-id> pres-rules-05 <deviceID>, 
                // <occurence-id> or <class>
                if ( Match( childName, KXdmDeviceId ) 
                    || Match( childName, KXdmDeviceID ) ) 
                    {
                    deviceId = ETrue;
                    }
                else if ( Match( childName, KXdmOccurenceId ) ) 
                    {
                    occurenceId = ETrue;
                    }
                else if ( Match( childName, KXdmClass ) ) 
                    {
                    prclass = ETrue;
                    }
                }
            }
            if ( allDevices && sequence )
                {
                LeaveWithErrorL( KXcapErrorSchemaViolation );
                }
            if ( sequence ) 
                {
                if ( deviceId && ( occurenceId || prclass ) )
                    {
                    LeaveWithErrorL( KXcapErrorSchemaViolation );
                    }
                if ( occurenceId && prclass )
                    {
                    LeaveWithErrorL( KXcapErrorSchemaViolation );
                    }
                }
        found = ETrue;
        }
    // <all-persons>
    else if ( Match( element, KXdmAllPersons ) )
        {
        found = ETrue;
        }
    //  <provide-persons>
    else if ( !element.CompareF( KXdmProvidePersons ) )
        {
        // type = providePersonPermissions
        // here we should have either <all-devices>
        // or sequence of other elements
        TBool allPersons( EFalse );
        TBool sequence( EFalse );
        TBool occurenceId( EFalse );
        TBool prclass( EFalse );
        TInt count( aXdmNode.NodeCount() );
        for ( TInt i(0); i < count; i++ )
            {
            TPtrC childName = aXdmNode.ChileNode(i)->NodeName();
            if ( Match( childName, KXdmAllPersons ) ) 
                {
                allPersons = ETrue;
                }
            else
                {
                sequence = ETrue;
                // in sequence should be either 
                // <occurence-id> or <class>
                if ( Match( childName, KXdmOccurenceId ) ) 
                    {
                    occurenceId = ETrue;
                    }
                else if ( Match( childName, KXdmClass ) ) 
                    {
                    prclass = ETrue;
                    }
                }
            }
            if ( allPersons && sequence )
                {
                LeaveWithErrorL( KXcapErrorSchemaViolation );
                }
            if ( sequence ) 
                {
                if ( occurenceId && prclass )
                    {
                    LeaveWithErrorL( KXcapErrorSchemaViolation );
                    }
                }
        found = ETrue;              
        }
    // <provide-activities>
    else if ( Match( element, KXdmProvideActivities ) )
        {
        dataType = EDataTypeBoolean;
        found = ETrue;              
        }
    // <provide-class>
    else if ( Match( element, KXdmProvideClass ) )
        {
        dataType = EDataTypeBoolean;
        found = ETrue;          
        }
    // <provide-device-id>
    else if ( Match( element, KXdmProvideDeviceId ) )
        {
        dataType = EDataTypeBoolean;            
        found = ETrue;
        }
    // <provide-mood>
    else if ( Match( element, KXdmProvideMood ) )
        {
        dataType = EDataTypeBoolean;            
        found = ETrue;
        }
    // <provide-place-is>
    else if ( Match( element, KXdmProvidePlaceIs ) )
        {
        dataType = EDataTypeBoolean;            
        found = ETrue;
        }
    // <provide-place-type>
    else if ( Match( element, KXdmProvidePlaceType ) )
        {
        dataType = EDataTypeBoolean;            
        found = ETrue;
        }
    // <provide-privacy>
    else if ( Match( element, KXdmProvidePrivacy ) )
        {
        dataType = EDataTypeBoolean;            
        found = ETrue;
        }
    // <provide-relationship>
    else if ( Match( element, KXdmProvideRelationship ) )
        {
        dataType = EDataTypeBoolean;            
        found = ETrue;
        }
    // <provide-status-icon>
    else if ( Match( element, KXdmProvideStatusIcon ) )
        {
        dataType = EDataTypeBoolean;            
        found = ETrue;
        }
    // <provide-sphere>
    else if ( Match( element, KXdmProvideSphere ) )
        {
        dataType = EDataTypeBoolean;            
        found = ETrue;
        }
    // <provide-time-offset>
    else if ( Match( element, KXdmProvideTimeOffset ) )
        {
        dataType = EDataTypeBoolean;            
        found = ETrue;
        }
    // <provide-user-input>
    else if ( Match( element, KXdmProvideUserInput ) )
        {
        // value is restricted, allowed values
        // are "false", "bare", "thresholds" and "full" 
        TPtrC8 data = aXdmNode.LeafNodeContent();
        if ( data.CompareF( KXdmFalse )
             && data.CompareF( KXdmBare )
             && data.CompareF( KXdmThresholds )
             && data.CompareF( KXdmFull ) )
            {
            LeaveWithErrorL( KXcapErrorEnumerationRule );
            }
        found = ETrue;
        }
    // <provide-note>
    else if ( Match( element, KXdmProvideNote ) )
        {
        dataType = EDataTypeBoolean;            
        found = ETrue;
        }
    // <provide-unknown-attribute>
    else if ( Match( element, KXdmProvideUnknownAttribute ) )
        {
        // required attribute name
        if ( !aXdmNode.HasAttribute( KXdmName ) )
            {
            LeaveWithErrorL( KXcapErrorMissingRequiredAttribute );
            }   
        found = ETrue;
        }
    // <provide-all-attributes>
    else if ( Match( element, KXdmProvideAllAttributes ) )
        {
        found = ETrue;
        }
    // <sub-handling>
    else if ( Match( element, KXdmSubHandling ) )
        {
        found = ETrue;
        // value is restricted, allowed values
        // are "block", "confirm", "polite-block" and "allow" 
        TPtrC8 data = aXdmNode.LeafNodeContent();
        if ( data.CompareF( KXdmBlock )
             && data.CompareF( KXdmConfirm )
             && data.CompareF( KXdmPoliteBlock )
             && data.CompareF( KXdmAllow ) )
            {
            LeaveWithErrorL( KXcapErrorEnumerationRule );
            }
        }
    // urn:ietf:params:xml:ns:common-policy
    if ( !found )
        {
        found = iIetfCommonPolicy->ValidateNodeL( aXdmNode );
        if ( found )
            {
            SetPrefixL( aXdmNode, KXdmIetfCommonPolicyNsPrefix );  
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
// CXcapIetfPresRulesUsage::ValidateAttributeL
// 
// ----------------------------------------------------
//
void CXcapIetfPresRulesUsage::ValidateAttributeL( const CXdmNodeAttribute& aXdmNodeAttr )
    {
    // This method is called by base class for each attribute
    // in document, here we have to define data types
    // for attributes, and pass them to the base class
    // for the actual data checking.
    
    // name 
    if ( Match( aXdmNodeAttr.NodeName(), KXdmName ) )
        {       
        // pass to base class for data validation
        ValidateDataL( EDataTypeString, aXdmNodeAttr );    
        }
    // the rest are from urn:ietf:params:xml:ns:common-policy
    else
        {
        iIetfCommonPolicy->ValidateAttributeL( aXdmNodeAttr );  
        }
     }   
                         
// ----------------------------------------------------
// CXcapIetfPresRulesUsage::AddNamespaceInformationL
// 
// ----------------------------------------------------
//
void CXcapIetfPresRulesUsage::AddNamespaceInformationL( CXdmDocument& aXdmDocument )
    {
    aXdmDocument.AppendNamespaceL( KXdmIetfPresRulesNamespace, KNullDesC8 );
    aXdmDocument.AppendNamespaceL( KXdmIetfPresRulesNamespace, KXdmIetfPresRulesNsPrefix ); // ESTI-7AY95P 
    aXdmDocument.AppendNamespaceL( KXdmIetfCommonPolicyNamespace, KXdmIetfCommonPolicyNsPrefix ); // ESTI-7AY95P 
    }


// End of File

