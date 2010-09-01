/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CXcapC4
*
*/

#include <XdmErrors.h>
#include <XdmDocumentNode.h>
#include <XdmNodeAttribute.h>
#include <centralrepository.h>
#ifdef _DEBUG
#include <flogger.h>
#endif

#include "XcapC4.h"
//#include "pasvariation.hrh"


// Used specification
// Presence Content XDM Specification, draft 1.0

// ----------------------------------------------------
// CXcapC4::CXcapC4
// 
// ----------------------------------------------------
//
CXcapC4::CXcapC4( const CXdmEngine& aXdmEngine ) :
	CXcapAppUsage( aXdmEngine )
    {
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapC4 initialized!" ) );
#endif
    }

// ----------------------------------------------------
// CXcapC4::NewL
// 
// ----------------------------------------------------
//
CXcapC4* CXcapC4::NewL( const TXcapAppUsageParams& aParameters )
    {
    CXcapC4* self = new ( ELeave ) CXcapC4( aParameters.iXdmEngine );
    CleanupStack::PushL( self );	// << self
    self->ConstructL();
    CleanupStack::Pop( self );      // >> self
    return self;
    }

// ----------------------------------------------------
// CXcapC4::~CXcapC4
// 
// ----------------------------------------------------
//
CXcapC4::~CXcapC4()
    {
    }
        
// ----------------------------------------------------
// CXcapC4::ConstructL
// 
// ----------------------------------------------------
//
void CXcapC4::ConstructL()
    {
    CRepository* cenRepRCSGlobal;
    cenRepRCSGlobal = CRepository::NewL( TUid::Uid( KC4SettingsCenRepUID ) );
	CleanupStack::PushL( cenRepRCSGlobal );

    // 0x06 is the index in central repository for content type setting
    User::LeaveIfError( cenRepRCSGlobal->Get( 0x06, iRCSGlobalOldContentType ) );
    CleanupStack::PopAndDestroy( ); // cenRepRCSGlobal
    }

// ----------------------------------------------------
// CXcapC4::AUID
// 
// ----------------------------------------------------
//
TPtrC8 CXcapC4::AUID() const
    {
    return TPtrC8( KXdmPresContentAUID );
    }
                
// ----------------------------------------------------
// CXcapC4::ContentType
// 
// ----------------------------------------------------
//
TPtrC8 CXcapC4::ContentType() const
    {
    if ( iRCSGlobalOldContentType )
    	{
	return TPtrC8( KXdmPresContentContTypeOld );
    	}
    else
    	{
	return TPtrC8( KXdmPresContentContType );
    	}
    }

// ----------------------------------------------------
// CXcapC4::DefaultNamespace
// 
// ----------------------------------------------------
//
TPtrC8 CXcapC4::DefaultNamespace() const
    {
    return TPtrC8( KXdmPresContentNamespace );
    }   
    
// ----------------------------------------------------
// CXcapC4::ValidateL
// 
// ----------------------------------------------------
//
TBool CXcapC4::ValidateNodeL( CXdmDocumentNode& aXdmNode )
    {
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapC4::ValidateNodeL" ) );
#endif
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

	// mime-type
	if ( Match( element, KXdmMimeType ) )
		{
		found = ETrue;
		}
	else if ( Match( element, KXdmEncoding ) )
		{
		found = ETrue;
		}
	else if ( Match( element, KXdmDescription ) )
		{
		found = ETrue;
		}
	else if ( Match( element, KXdmData ) )
		{
		found = ETrue;
		}

#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapC4::ValidateNodeL - found: %d" ), found );
#endif
    return found;
    }

// ----------------------------------------------------
// CXcapC4::ValidateAttributeL
// 
// ----------------------------------------------------
//
void CXcapC4::ValidateAttributeL( const CXdmNodeAttribute& aXdmNodeAttr )
    {
    // This method is called by base class for each attribute
    // in document, here we have to define data types
    // for attributes, and pass them to the base class
    // for the actual data checking.
    
	// there should not be any attributes in pres-content document
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapC4::ValidateAttributeL" ) );
#endif
//	User::Leave( KErrNotSupported );
    }   
                         
// ----------------------------------------------------
// CXcapC4::AddNamespaceInformationL
// 
// ----------------------------------------------------
//
void CXcapC4::AddNamespaceInformationL( CXdmDocument& aXdmDocument )
    {
#ifdef _DEBUG 
    WriteToLog( _L8( "CXcapC4::AddNamespaceInformationL" ) );
#endif
    aXdmDocument.AppendNamespaceL( KXdmPresContentNamespace, KNullDesC8 );
    }

// End of File

