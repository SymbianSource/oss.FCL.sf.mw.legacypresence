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
* Description: CXcapAppUsage
*
*/





// INCLUDE FILES
#ifdef _DEBUG
#include <flogger.h>
#endif

#include <implementationproxy.h>

#include "XcapAppUsageDef.h"

#include "XcapDirectoryUsage.h"
#include "XcapCapabilityUsage.h"
#include "XcapPocGroupUsage.h"
#include "XcapPocUserAccessUsage.h"
#include "XcapSharedXDMUsage.h"
#include "XcapRlsServicesUsage.h"
#include "XcapResourceListsUsage.h"
#include "XcapIetfCommonPolicyUsage.h"
#include "XcapOmaCommonPolicyUsage.h"
#include "XcapIetfPresRulesUsage.h"
#include "XcapOmaPresRulesUsage.h"
// Add the header of the new app usage here
// #include "XcapTestAppUsage.h"
// PRES-CONTENT STARTS
#include "XcapC4.h"
// PRES-CONTENT ENDS

#include <XdmErrors.h>
#include <XdmProtocolUidList.h>
#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include "XcapUriInterface.h"


    
// CONSTANTS
// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] = 
	{
#ifdef __EABI__  
	IMPLEMENTATION_PROXY_ENTRY( KXdmCapabilityUsageUid, CXcapCapabilityUsage::NewL ),
	IMPLEMENTATION_PROXY_ENTRY( KXdmDirectoryUsageUid,  CXcapDirectoryUsage::NewL ),
	IMPLEMENTATION_PROXY_ENTRY( KXdmPocGroupUsageUid,   CXcapPocGroupUsage::NewL ),
	IMPLEMENTATION_PROXY_ENTRY( KXdmPocUserAccessUsageUid,  CXcapPocUserAccessUsage::NewL ),
	IMPLEMENTATION_PROXY_ENTRY( KXdmSharedXDMUsageUid,  CXcapSharedXDMUsage::NewL ),
	IMPLEMENTATION_PROXY_ENTRY( KXdmRlsServicesUsageUid,    CXcapRlsServicesUsage::NewL ),
	IMPLEMENTATION_PROXY_ENTRY( KXdmResourceListsUsageUid,  CXcapResourceListsUsage::NewL ),
	IMPLEMENTATION_PROXY_ENTRY( KXdmIetfCommonPolicyUsageUid,   CXcapIetfCommonPolicyUsage::NewL ),
	IMPLEMENTATION_PROXY_ENTRY( KXdmOmaCommonPolicyUsageUid,    CXcapOmaCommonPolicyUsage::NewL ),
	IMPLEMENTATION_PROXY_ENTRY( KXdmIetfPresRulesUsageUid,  CXcapIetfPresRulesUsage::NewL ),
	IMPLEMENTATION_PROXY_ENTRY( KXdmOmaPresRulesUsageUid,   CXcapOmaPresRulesUsage::NewL ),
    // add new usages here
    // IMPLEMENTATION_PROXY_ENTRY( KXdmTestAppUsageUid,    CXcapTestAppUsage::NewL ),
// PRES-CONTENT STARTS
	IMPLEMENTATION_PROXY_ENTRY( KXdmUsageUid, CXcapC4::NewL )
// PRES-CONTENT ENDS
#else  
    { { KXdmCapabilityUsageUid }, CXcapCapabilityUsage::NewL },
    { { KXdmDirectoryUsageUid }, CXcapDirectoryUsage::NewL },
    { { KXdmPocGroupUsageUid }, CXcapPocGroupUsage::NewL },
    { { KXdmPocUserAccessUsageUid }, CXcapPocUserAccessUsage::NewL },
    { { KXdmSharedXDMUsageUid }, CXcapSharedXDMUsage::NewL },
    { { KXdmRlsServicesUsageUid }, CXcapRlsServicesUsage::NewL },
    { { KXdmResourceListsUsageUid }, CXcapResourceListsUsage::NewL },
    { { KXdmIetfCommonPolicyUsageUid }, CXcapIetfCommonPolicyUsage::NewL },
    { { KXdmOmaCommonPolicyUsageUid }, CXcapOmaCommonPolicyUsage::NewL },
    { { KXdmIetfPresRulesUsageUid }, CXcapIetfPresRulesUsage::NewL },
    { { KXdmOmaPresRulesUsageUid }, CXcapOmaPresRulesUsage::NewL }, 
    // add new usages here
    // { { KXdmTestAppUsageUid }, CXcapTestAppUsage::NewL }, 
// PRES-CONTENT STARTS
    { { KXdmUsageUid }, CXcapC4::NewL }
// PRES-CONTENT ENDS
#endif
    };

// ----------------------------------------------------
// ImplementationGroupProxy
//
// ----------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }


// ----------------------------------------------------
// CXcapAppUsage::CXcapAppUsage
// 
// ----------------------------------------------------
//
CXcapAppUsage::CXcapAppUsage( const CXdmEngine& aXdmEngine ) :
iXdmEngine( CONST_CAST( CXdmEngine&, aXdmEngine ) ),
iAddNamespaces( EFalse ),
iNsToNodeSelector( EFalse ),
iAddNsAttributes( EFalse ),
iIsGetOrDelete( EFalse )
    {
#ifdef _NAMESPACES_TO_NODE_SELECTOR
    iNsToNodeSelector = ETrue;
#endif   

#ifdef _ADD_NAMESPACE_ATTRIBUTES
	iAddNsAttributes = ETrue;
#endif  
    }

// ----------------------------------------------------
// CXcapAppUsage::~CXcapAppUsage
// 
// ----------------------------------------------------
//
CXcapAppUsage::~CXcapAppUsage()
    {
    REComSession::DestroyedImplementation( iDestructorKey );
    }

// ----------------------------------------------------
// CXcapAppUsage::NewL
// 
// ----------------------------------------------------
//
EXPORT_C CXcapAppUsage* CXcapAppUsage::NewL( const CXdmEngine& aXdmEngine,
                                           const TInt aDocumentType )
    {
    TAny* ptr = NULL;
    TXcapAppUsageParams params( aXdmEngine );
    switch( aDocumentType )
        {
        case KXdmCapabilityUsageUid:
        case KXdmDirectoryUsageUid:
        case KXdmTestAppUsageUid:    
        case KXdmPocGroupUsageUid:   
        case KXdmPocUserAccessUsageUid:   
        case KXdmSharedXDMUsageUid:
        case KXdmRlsServicesUsageUid:   
        case KXdmResourceListsUsageUid:  
        case KXdmIetfCommonPolicyUsageUid:
        case KXdmOmaCommonPolicyUsageUid:     
        case KXdmIetfPresRulesUsageUid:     
        case KXdmOmaPresRulesUsageUid:
        case KXdmUsageUid:
            {
            TUid KImplementationUid = { aDocumentType };
            ptr = REComSession::CreateImplementationL( 
                   KImplementationUid,
                   _FOFF( CXcapAppUsage, iDestructorKey ),
                   ( TAny* )&params );
            }
            break;
     
        default:
            User::Leave( KErrNotSupported );
        }
    return reinterpret_cast<CXcapAppUsage*>( ptr );
    }

// ----------------------------------------------------
// CXcapAppUsage::Validate
// 
// ----------------------------------------------------
//
EXPORT_C TInt CXcapAppUsage::Validate( CXdmDocument& aXdmDocument, TBool aAddNamespaces ) 
    {    
    iAddNamespaces = aAddNamespaces;
    iIsGetOrDelete = EFalse;
    // catch leaves here and return the error code
    TInt error ( KErrNone );
    TRAP( error, DoValidateL( aXdmDocument ) );
    // add namespace information for outgoing document
    if ( error == KErrNone && iAddNamespaces )
        {
	    aXdmDocument.ResetNamespaces();
	    TRAP( error, AddNamespaceInformationL( aXdmDocument ) );	
	    }
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapAppUsage::Validate document returns %d" ), error );
#endif
    return error;
    }

// ----------------------------------------------------
// CXcapAppUsage::Validate
//
// ----------------------------------------------------
//	
EXPORT_C TInt CXcapAppUsage::Validate( CXdmDocumentNode& aXdmNode, 
                                       MXcapUriInterface* aUriInterface,
                                       TBool aIsGetOrDelete ) 
    {
    iAddNamespaces = ETrue;  
    iIsGetOrDelete = aIsGetOrDelete;
    ResetUsedNamespaces();
    // catch leaves here and return the error code
    TInt error( KErrNone );
    TRAP( error, DoValidateL( aXdmNode ) );

    // add namespace if defined 
    if ( error == KErrNone && iAddNsAttributes )
    	{
    	TRAP( error, CreateNamespaceAttributesL( aXdmNode, aUriInterface ) );
    	}
    
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapAppUsage::Validate node returns %d" ), error );
#endif    
    return error;
    }
	    
			
// ----------------------------------------------------
// CXcapAppUsage::DoValidateL
//
// ----------------------------------------------------
//
void CXcapAppUsage::DoValidateL( CXdmDocumentNode& aXdmNode ) 
    {
    // call method from derived class
    ValidateNodeL( aXdmNode );
    
    // go through all attributes
    TInt attrCount( aXdmNode.AttributeCount() );
    for ( TInt i( 0 ); i < attrCount; i++ )
        {   
        CXdmNodeAttribute* attr = aXdmNode.Attribute( i );
        ValidateAttributeL( *attr ); 
        }
		   
    // go through all descendants
    TInt count( aXdmNode.NodeCount( ) );
    for ( TInt i = 0; i < count; i++ )
        {
        DoValidateL ( *aXdmNode.ChileNode( i ) );
        }
    }
	
// ----------------------------------------------------
// CXcapAppUsage::DoValidateL 
// 
// ----------------------------------------------------
//  
void CXcapAppUsage::DoValidateL ( CXdmDocument& aXdmDocument ) 
    {
    DoValidateL( *aXdmDocument.DocumentRoot() );
    }

// ----------------------------------------------------
// CXcapAppUsage::ValidateDataL
// 
// ----------------------------------------------------
//
void CXcapAppUsage::ValidateDataL ( TDataType aDataType, TDesC8& aData )
    {
    if ( iIsGetOrDelete )
        {
        return;
        }
    switch ( aDataType )
        {
        case EDataTypeString:
            {
            // TODO 
            }
        break;
        case EDataTypeNonNegativeInteger:
            {
            TInt value( 0 );
            TLex8 lex( aData );
            lex.Val( value );   
            if ( value < 0 )
                {
                User::Leave( KXcapErrorIllegalDataValue );
                }
            }
            break;
        case EDataTypeAnyURI:
            {
            // TODO 
            }
            break;
        case EDataTypeDateTime:
            {
            // TODO 
            }
            break;
        case EDataTypeBoolean:
            {
            if ( aData.CompareF( KXdmTrue )
                && aData.CompareF( KXdmFalse )
                && aData.CompareF( KXdmZero )
                && aData.CompareF( KXdmOneNbr )  )
                {
                User::Leave( KXcapErrorIllegalDataValue );  
                }
            }
            break;
        case EDataTypeToken:
            {
            // elements that are using datatype token
            // scheme, class, occurence-id 
            }
            break;
        case EDataTypeEmpty:
            {
            if ( aData.Length() > 0 )
                {
                User::Leave( KXcapErrorIllegalDataValue );
                } 
            }
            break;      
        default:    
        break;
        }
    }
    
// ----------------------------------------------------
// CXcapAppUsage::ValidateDataL
// 
// ----------------------------------------------------
//
void CXcapAppUsage::ValidateDataL ( TDataType aDataType, CXdmDocumentNode& aXdmNode )
    { 
    TPtrC8 data = aXdmNode.LeafNodeContent();
    ValidateDataL( aDataType, data );
    }
    
// ----------------------------------------------------
// CXcapAppUsage::ValidateDataL
// 
// ----------------------------------------------------
//  
void CXcapAppUsage::ValidateDataL ( TDataType aDataType, const CXdmNodeAttribute& aXdmNodeAttr )
    {
    HBufC8* dataBuf = aXdmNodeAttr.EightBitValueLC();   // << dataBuf
    TPtrC8 data = dataBuf->Des();
    ValidateDataL( aDataType, data );
    CleanupStack::PopAndDestroy( dataBuf );            // >>> dataBuf 
    }

// ----------------------------------------------------
// CXcapAppUsage::SetPrefixL
// 
// ----------------------------------------------------
//
void CXcapAppUsage::SetPrefixL( CXdmDocumentNode& aXdmNode, const TDesC8& aPrefix )
    {
    if ( iAddNamespaces )
        {
        aXdmNode.SetPrefixL( aPrefix ); 
        AddUsedNamespace( aPrefix );
        }
    }
    
// ----------------------------------------------------
// CXcapAppUsage::AddUsedNamespace
// 
// ----------------------------------------------------
//
void CXcapAppUsage::AddUsedNamespace( const TDesC8& aPrefix ) 
    {
    // rl
    if ( Match( aPrefix, KXdmResourceListsNsPrefix ) )
        {
        iRL = ETrue;
        }
    // cr
    else if ( Match( aPrefix, KXdmIetfCommonPolicyNsPrefix ) )
        {
        iCR = ETrue;
        }
    // cp
    else if ( Match( aPrefix, KXdmOmaCommonPolicyNsPrefix ) )
        {
        iCP = ETrue;
        }
    // pr
    else if ( Match( aPrefix, KXdmIetfPresRulesNsPrefix ) )
        {
        iPR = ETrue;
        }
    // ls
    else if ( Match( aPrefix, KXdmPocGroupNsPrefix ) )
        {
        iLS = ETrue;
        }
    // poc
    else if ( Match( aPrefix, KXdmPocUserAccessNsPrefix ) )
        {
        iPOC = ETrue;
        }
    // opr
    else if ( Match( aPrefix, KXdmOmaPresRulesNsPrefix ) )
        {
        iOPR = ETrue;
        }
    // rls
    else if ( Match( aPrefix, KXdmRlsServicesNsPrefix ) )
        {
        iRLS = ETrue;
        }
    // ou
    else if ( Match( aPrefix, KXdmSharedXDMUriUsageNsPrefix ) )
        {
        iOU = ETrue;
        }
    // add new prefix definitions here 
    }
    
// ----------------------------------------------------
// CXcapAppUsage::ResetUsedNamespaces
// 
// ----------------------------------------------------
//
void CXcapAppUsage::ResetUsedNamespaces() 
    {
    iRL = EFalse;
    iCR = EFalse;
    iCP = EFalse;
    iPR = EFalse;
    iLS = EFalse;
    iPOC = EFalse;
    iOPR = EFalse;
    iRLS = EFalse;
    iOU = EFalse;
    // add new prefix definitions here 
    }
    
// ----------------------------------------------------
// CXcapAppUsage::AppendUsedNamespacesL
// 
// ----------------------------------------------------
//
void CXcapAppUsage::AppendUsedNamespacesL( MXcapUriInterface* aUriInterface ) 
    {
    if( iRL )
        {
        aUriInterface->AddNamespaceMappingL( KXdmResourceListsNamespace, KXdmResourceListsNsPrefix );
        }
    if( iCR )
        {
        aUriInterface->AddNamespaceMappingL( KXdmIetfCommonPolicyNamespace, KXdmIetfCommonPolicyNsPrefix );
        }
    if( iCP )
        {
        aUriInterface->AddNamespaceMappingL( KXdmOmaCommonPolicyNamespace, KXdmOmaCommonPolicyNsPrefix );        
        }
    if( iPR )
        {
        aUriInterface->AddNamespaceMappingL( KXdmIetfPresRulesNamespace, KXdmIetfPresRulesNsPrefix );        
        }
    if( iLS )
        {
         aUriInterface->AddNamespaceMappingL( KXdmPocGroupNamespace, KXdmPocGroupNsPrefix );       
        }
    if( iPOC )
        {
        aUriInterface->AddNamespaceMappingL( KXdmPocUserAccessNamespace, KXdmPocUserAccessNsPrefix );       
        }
    if( iOPR )
        {
        aUriInterface->AddNamespaceMappingL( KXdmOmaPresRulesNamespace, KXdmOmaPresRulesNsPrefix );       
        }
    if( iRLS )
        {
        aUriInterface->AddNamespaceMappingL( KXdmRlsServicesNamespace, KXdmRlsServicesNsPrefix );        
        }
    if( iOU )
        {
        aUriInterface->AddNamespaceMappingL( KXdmSharedXDMUriUsageNamespace, KXdmSharedXDMUriUsageNsPrefix );
        }
    // add new prefix definitions here 
    }
    
// ----------------------------------------------------
// CXcapAppUsage::CreateNamespaceAttributesL
// 
// ----------------------------------------------------
// 
void CXcapAppUsage::CreateNamespaceAttributesL( CXdmDocumentNode& aXdmNode, MXcapUriInterface* aUriInterface )
	{
	// First remove all existing xmlns attributes
	RemoveNamespaceAttributesL( aXdmNode );
	
	TPtrC8 nameSpace;
	TPtrC8 prefix = aXdmNode.Prefix();
	
    // if the root node belongs to the default namespace,
    // it's enough that we add attribute for it and also
    // attributes for all other used namespaces
    if ( Match( prefix, KNullDesC8 ) )
			{
       	CXdmNodeAttribute* ns = aXdmNode.CreateAttributeL( KXdmDefaultNsAttrName );
        ns->SetAttributeValueL( DefaultNamespace() );
        AppendNamespaceAttributesL( aXdmNode ); 
        
        if ( iNsToNodeSelector )
            {
            //aUriInterface->AddNamespaceMappingL( DefaultNamespace(), KNullDesC8 );
            CXdmDocumentNode* parent = aXdmNode.Parent();
            while ( parent )
                {
                AddUsedNamespace ( parent->Prefix() );
                parent = parent->Parent();
                }
            AppendUsedNamespacesL( aUriInterface );   
            } 
        // we are done
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapAppUsage::CreateNamespaceAttributesL root belongs to default" ) );
#endif
        return;        
        }
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapAppUsage::CreateNamespaceAttributesL root doesn't belong to default" ) );
#endif   
    // if the default namespace is used in fragment,
    // the attribute for it has to be added
    if ( DefaultNamespaceUsed( aXdmNode ) )
        {
      	CXdmNodeAttribute* ns = aXdmNode.CreateAttributeL( KXdmDefaultNsAttrName );
        ns->SetAttributeValueL( DefaultNamespace() );
        }
	// finally, create new xmlns attributes
    AppendNamespaceAttributesL( aXdmNode );
    if ( iNsToNodeSelector )
        {
        //aUriInterface->AddNamespaceMappingL( DefaultNamespace(), KNullDesC8 );
        CXdmDocumentNode* parent = aXdmNode.Parent();
        while ( parent )
            {
            AddUsedNamespace ( parent->Prefix() );
            parent = parent->Parent();
            }
        AppendUsedNamespacesL( aUriInterface );   
        } 
	}	

// ----------------------------------------------------
// CXcapAppUsage::RemoveNamespaceAttributesL
// 
// ----------------------------------------------------
// 
void CXcapAppUsage::RemoveNamespaceAttributesL( CXdmDocumentNode& aXdmNode )
	{
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapAppUsage::RemoveNamespaceAttributesL begin" ) );
#endif	
	// go through all attributes
    for ( TInt i( aXdmNode.AttributeCount() - 1 ); i > KErrNotFound; i-- )
        {   
        CXdmNodeAttribute* attr = aXdmNode.Attribute( i );
       	if ( attr->NodeName().FindF( KXdmDefaultNsAttrName ) != KErrNotFound )
			{
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapAppUsage::RemoveNamespaceAttributesL AttrCount=%d i=%d" ), aXdmNode.AttributeCount(), i );
#endif
			aXdmNode.RemoveChileNodeL( attr );
			}
        }
		   
    // go through all descendants
    TInt count( aXdmNode.NodeCount( ) );
    for ( TInt i(0); i < count; i++ )
        {
        RemoveNamespaceAttributesL ( *aXdmNode.ChileNode( i ) );
        }
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapAppUsage::RemoveNamespaceAttributesL done" ) );
#endif	
	}
	
// ----------------------------------------------------
// CXcapAppUsage::DefaultNamespaceUsed
// 
// ----------------------------------------------------
// 
TBool CXcapAppUsage::DefaultNamespaceUsed( CXdmDocumentNode& aXdmNode )
	{
	if ( Match( aXdmNode.Prefix(), KNullDesC8 ) )
		{
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapAppUsage::DefaultNamespaceUsed returns true" ) );
#endif
		return ETrue;	
		}
    // go through all descendants
    TInt count( aXdmNode.NodeCount( ) );
    for ( TInt i = 0; i < count; i++ )
        {
        if ( DefaultNamespaceUsed ( *aXdmNode.ChileNode( i ) ) )
            {
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapAppUsage::DefaultNamespaceUsed returns true" ) );
#endif
            return ETrue;
            }
        }
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapAppUsage::DefaultNamespaceUsed returns false" ) );
#endif
    return EFalse;	
	}
// ----------------------------------------------------
// CXcapAppUsage::AppendNamespaceAttributesL
// 
// ----------------------------------------------------
//
void CXcapAppUsage::AppendNamespaceAttributesL( CXdmDocumentNode& aXdmNode ) 
    {
    if( iRL )
        {
		CreateNamespaceAttributeL( aXdmNode, KXdmResourceListsNsPrefix, KXdmResourceListsNamespace );
        }
    if( iCR )
        {
        CreateNamespaceAttributeL( aXdmNode, KXdmIetfCommonPolicyNsPrefix, KXdmIetfCommonPolicyNamespace );
        }
    if( iCP )
        {
       	CreateNamespaceAttributeL( aXdmNode, KXdmOmaCommonPolicyNsPrefix, KXdmOmaCommonPolicyNamespace );        
        }
    if( iPR )
        {
        CreateNamespaceAttributeL( aXdmNode, KXdmIetfPresRulesNsPrefix, KXdmIetfPresRulesNamespace );        
        }
    if( iLS )
        {
       	CreateNamespaceAttributeL( aXdmNode, KXdmPocGroupNsPrefix, KXdmPocGroupNamespace );      
        }
    if( iPOC )
        {
       	CreateNamespaceAttributeL( aXdmNode, KXdmPocUserAccessNsPrefix, KXdmPocUserAccessNamespace );   
        }
    if( iOPR )
        {
       	CreateNamespaceAttributeL( aXdmNode, KXdmOmaPresRulesNsPrefix, KXdmOmaPresRulesNamespace );       
        }
    if( iRLS )
        {
        CreateNamespaceAttributeL( aXdmNode, KXdmRlsServicesNsPrefix, KXdmRlsServicesNamespace );  
        }
    if( iOU )
        {
       	CreateNamespaceAttributeL( aXdmNode, KXdmSharedXDMUriUsageNsPrefix, KXdmSharedXDMUriUsageNamespace );
        }
    // add new prefix definitions here 
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapAppUsage::AppendNamespaceAttributesL done" ) );
#endif
    }
    
// ----------------------------------------------------
// CXcapAppUsage::CreateNamespaceAttributeL
// 
// ----------------------------------------------------
// 
void CXcapAppUsage::CreateNamespaceAttributeL( CXdmDocumentNode& aXdmNode, const TDesC8& aPrefix, const TDesC8& aNamespace )
    {
	TBuf8<KXdmMaxPrefixLength> name;
    name.Copy( KXdmNonDefaultNsAttrName );
    name.Append( aPrefix );
    HBufC8* attrName = name.AllocLC();	// << attrName
    CXdmNodeAttribute* ns = aXdmNode.CreateAttributeL(  );
    CleanupStack::PushL( ns );	// << ns
    ns->SetNameL( *attrName );
    ns->SetAttributeValueL( aNamespace );
    CleanupStack::Pop( ns );	// >> ns
    CleanupStack::PopAndDestroy();	// >>> attrName
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapAppUsage::CreateNamespaceAttributeL done" ) );
#endif
	}
    
// ----------------------------------------------------
// CXcapAppUsage::Match
// 
// ----------------------------------------------------
// 
TBool CXcapAppUsage::Match( const TDesC& aLeft, const TDesC& aRight )
    {
    if ( !aLeft.CompareF( aRight ) )
        {
        return ETrue;
        }
    return EFalse;
    }
    
// ----------------------------------------------------
// CXcapAppUsage::Match
// 
// ----------------------------------------------------
// 
TBool CXcapAppUsage::Match( const TDesC8& aLeft, const TDesC8& aRight )
    {
    if ( !aLeft.CompareF( aRight ) )
        {
        return ETrue;
        }
    return EFalse;
    }    
    
// ----------------------------------------------------
// CXcapAppUsage::LeaveWithErrorL
// 
// ----------------------------------------------------
//
void CXcapAppUsage::LeaveWithErrorL( TInt aErrorCode )
    {
    if ( !iIsGetOrDelete )
        {
        User::Leave( aErrorCode );
        }
    }    
    

// ----------------------------------------------------
// CXcapAppUsage::WriteToLog
// _DEBUG only
// ----------------------------------------------------
//
#ifdef _DEBUG
void CXcapAppUsage::WriteToLog( TRefByValue<const TDesC8> aFmt,... )                                 
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KAppUsageLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    RFileLogger::Write( KXdmAppUsageLogDir, KXdmAppUsageLogFile, EFileLoggingModeAppend, buf );
    }
#endif
    
//  End of File  


