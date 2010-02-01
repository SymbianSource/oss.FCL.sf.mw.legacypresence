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
* Description:  CXcapOperationFactory 
*
*/



#include <ecom.h>
#include <flogger.h>
#include <implementationproxy.h>
#include "XcapAddition.h"
#include "XcapDeletion.h"
#include "XdmOperation.h"
#include "XcapRetrieval.h"
#include "XcapInsertion.h"
#include "XcapReplacement.h"
#include "xdmlogwriter.h"
#include "XcapOperationFactory.h"

// ---------------------------------------------------------
// CXcapOperationFactory::CXcapOperationFactory
// 
// ---------------------------------------------------------
//
CXcapOperationFactory::CXcapOperationFactory()
    {   
    }
    
// ---------------------------------------------------------
// CXcapOperationFactory::NewL
// 
// ---------------------------------------------------------
//
CXcapOperationFactory* CXcapOperationFactory::NewL()
    {
    CXcapOperationFactory* self = new ( ELeave ) CXcapOperationFactory();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CXcapOperationFactory::ConstructL
// 
// ---------------------------------------------------------
//
void CXcapOperationFactory::ConstructL()
    {
    #ifdef _DEBUG
        iLogWriter = CXdmLogWriter::NewL( KOperationsLogFile );
        WriteToLog( _L8( "-> CXcapOperationFactory::ConstructL" ) );
        WriteToLog( _L8( "<- CXcapOperationFactory::ConstructL" ) );
    #endif
    }
            
// ---------------------------------------------------------
// CXcapOperationFactory::~CXcapOperationFactory
// 
// ---------------------------------------------------------
//
CXcapOperationFactory::~CXcapOperationFactory()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "-> CXcapOperationFactory::~CXcapOperationFactory()" ) );
    #endif
    #ifdef _DEBUG
        WriteToLog( _L8( "<- CXcapOperationFactory::~CXcapOperationFactory()" ) );
      #endif
    delete iLogWriter;
    }
    
// ---------------------------------------------------------
// CXcapOperationFactory::FetchL
// 
// ---------------------------------------------------------
//	
MXdmOperation* CXcapOperationFactory::FetchL( CXdmDocument& aTargetDocument,
                                              CXdmDocumentNode* aTargetNode ) const
    {
    return CXcapRetrieval::NewL( ( CXcapDocument& )aTargetDocument,
                                 ( CXcapDocumentNode* )aTargetNode,
                                 const_cast<CXcapOperationFactory&>( *this ) );
    }       

// ---------------------------------------------------------
// CXcapOperationFactory::InsertL
// 
// ---------------------------------------------------------
//
MXdmOperation* CXcapOperationFactory::InsertL( CXdmDocument& aTargetDocument,
                                               CXdmDocumentNode* aTargetNode ) const
    {
    return CXcapInsertion::NewL( ( CXcapDocument& )aTargetDocument,
                                 ( CXcapDocumentNode* )aTargetNode,
                                 const_cast<CXcapOperationFactory&>( *this ) );
    }

// ---------------------------------------------------------
// CXcapOperationFactory::ReplaceL
// 
// ---------------------------------------------------------
//
MXdmOperation* CXcapOperationFactory::ReplaceL( CXdmDocument& aOldDocument, 
                                                CXdmDocumentNode* aNewNode,
                                                CXdmDocumentNode* aOldNode ) const
    {
    return CXcapReplacement::NewL( ( CXcapDocument& )aOldDocument,
                                   ( CXcapDocumentNode* )aNewNode,
                                   ( CXcapDocumentNode* )aOldNode,
                                   const_cast<CXcapOperationFactory&>( *this )  );
    }

// ---------------------------------------------------------
// CXcapOperationFactory::AppendL
// 
// ---------------------------------------------------------
//
MXdmOperation* CXcapOperationFactory::AppendL( CXdmDocument& aTargetDocument,
                                               CXdmDocumentNode* aTargetNode ) const
    { 
    return CXcapAddition::NewL( ( CXcapDocument& )aTargetDocument,
                                ( CXcapDocumentNode* )aTargetNode,
                                const_cast<CXcapOperationFactory&>( *this ) );
    }              

// ---------------------------------------------------------
// CXcapOperationFactory::PartialDeletionL
// 
// ---------------------------------------------------------
//
MXdmOperation* CXcapOperationFactory::DeletionL( CXdmDocument& aTargetDocument,
                                                 CXdmDocumentNode* aTargetNode ) const
    {
    return CXcapDeletion::NewL( ( CXcapDocument& )aTargetDocument,
                                ( CXcapDocumentNode* )aTargetNode,
                                const_cast<CXcapOperationFactory&>( *this ) );
    }              
    
// ----------------------------------------------------
// CXcapOperationFactory::WriteToLog
// 
// ----------------------------------------------------
//
void CXcapOperationFactory::WriteToLog( TRefByValue<const TDesC8> aFmt,... ) const
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
    IMPLEMENTATION_PROXY_ENTRY( KXcapOperationFactory,	CXcapOperationFactory::NewL )
#else
    { { KXcapOperationFactory }, CXcapOperationFactory::NewL }
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

