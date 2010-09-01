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
* Description: CLocalOperationFactory
*
*/



#include <ecom.h>
#include <e32debug.h>
#include <xdmlogwriter.h>
#include <implementationproxy.h>
#include "XdmOperation.h"
#include "LocalRetrieval.h"
#include "LocalAddition.h"
#include "LocalReplacement.h"
#include "LocalDeletion.h"
#include "LocalProtocol.h"
#include "LocalOperationFactory.h"

// ---------------------------------------------------------
// CLocalOperationFactory::CLocalOperationFactory
// 
// ---------------------------------------------------------
//
CLocalOperationFactory::CLocalOperationFactory()
    {   
    }
    
// ---------------------------------------------------------
// CLocalOperationFactory::NewL
// 
// ---------------------------------------------------------
//
CLocalOperationFactory* CLocalOperationFactory::NewL()
    {
    CLocalOperationFactory* self = new ( ELeave ) CLocalOperationFactory();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CLocalOperationFactory::ConstructL
// 
// ---------------------------------------------------------
//
void CLocalOperationFactory::ConstructL()
    {
    #ifdef _DEBUG
        iLogWriter = CXdmLogWriter::NewL( KLocalOpLogFile );
        WriteToLog( _L8( "CLocalOperationFactory::ConstructL()" ) );
    #endif
    }
            
// ---------------------------------------------------------
// CLocalOperationFactory::~CLocalOperationFactory
// 
// ---------------------------------------------------------
//
CLocalOperationFactory::~CLocalOperationFactory()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CLocalOperationFactory::~CLocalOperationFactory()" ) );
    #endif
    delete iLogWriter;
    }
    
// ---------------------------------------------------------
// CLocalOperationFactory::FetchL
// 
// ---------------------------------------------------------
//	
MXdmOperation* CLocalOperationFactory::FetchL( CXdmDocument& aTargetDocument,
                                               CXdmDocumentNode* aTargetNode ) const
    {
    return CLocalRetrieval::NewL( ( CLocalDocument& )aTargetDocument,
                                  ( CLocalDocumentNode* )aTargetNode,
                                  const_cast<CLocalOperationFactory&>( *this ) );
    }       

// ---------------------------------------------------------
// CLocalOperationFactory::InsertL
// 
// ---------------------------------------------------------
//
MXdmOperation* CLocalOperationFactory::InsertL( CXdmDocument& aTargetDocument,
                                                CXdmDocumentNode* aTargetNode ) const
    {
    return CLocalAddition::NewL( ( CLocalDocument& )aTargetDocument,
                                 ( CLocalDocumentNode* )aTargetNode,
                                 const_cast<CLocalOperationFactory&>( *this ) );
    }

// ---------------------------------------------------------
// CLocalOperationFactory::ReplaceL
// 
// ---------------------------------------------------------
//
MXdmOperation* CLocalOperationFactory::ReplaceL( CXdmDocument& aOldDocument, 
                                                 CXdmDocumentNode* aNewNode,
                                                 CXdmDocumentNode* aOldNode  ) const
    {
    return CLocalReplacement::NewL( ( CLocalDocument& )aOldDocument,
                                    ( CLocalDocumentNode* )aNewNode,
                                    ( CLocalDocumentNode* )aOldNode,
                                    const_cast<CLocalOperationFactory&>( *this ) );
    }

// ---------------------------------------------------------
// CLocalOperationFactory::AppendL
// 
// ---------------------------------------------------------
//
MXdmOperation* CLocalOperationFactory::AppendL( CXdmDocument& aTargetDocument,
                                                CXdmDocumentNode* aTargetNode ) const
    { 
    return CLocalAddition::NewL( ( CLocalDocument& )aTargetDocument,
                                 ( CLocalDocumentNode* )aTargetNode,
                                 const_cast<CLocalOperationFactory&>( *this ) );
    }              

// ---------------------------------------------------------
// CLocalOperationFactory::PartialDeletionL
// 
// ---------------------------------------------------------
//
MXdmOperation* CLocalOperationFactory::DeletionL( CXdmDocument& aTargetDocument,
                                                  CXdmDocumentNode* aTargetNode ) const
    {
    return CLocalDeletion::NewL( ( CLocalDocument& )aTargetDocument,
                                 ( CLocalDocumentNode* )aTargetNode,
                                 const_cast<CLocalOperationFactory&>( *this ) );
    }              

// ----------------------------------------------------------
// CLocalOperationFactory::WriteToLog
// 
// ----------------------------------------------------------
//
void CLocalOperationFactory::WriteToLog( TRefByValue<const TDesC8> aFmt,... )                                 
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    iLogWriter->WriteToLog( buf );
    }
        
// ---------------------------------------------------------
// Map the interface UIDs to implementation factory functions
// 
// ---------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
	{
#ifdef __EABI__
    IMPLEMENTATION_PROXY_ENTRY( KLocalOperationFactory,	CLocalOperationFactory::NewL )
#else
    { { KLocalOperationFactory }, CLocalOperationFactory::NewL }
#endif
    };

// ---------------------------------------------------------
// Return the implementation table & number of implementations
// 
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

