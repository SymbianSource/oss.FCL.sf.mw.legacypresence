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
* Description:   XDM Engine
*
*/




// INCLUDE FILES
#include <utf.h>
#include <ecom.h>
#include <f32file.h>
#include <flogger.h>
#include "XdmEngine.h"
#include "XdmDocument.h"
#include "XdmProtocol.h"
#include "XdmDirectory.h"
#include "XdmLogWriter.h"
#include "XdmStaticUtils.h"
#include "XdmSettingsApi.h"
#include "XdmProtocolInfo.h"

// ----------------------------------------------------------
// CXdmEngine::CXdmEngine
// 
// ----------------------------------------------------------
//
CXdmEngine::CXdmEngine() : CActive( EPriorityStandard )                                           
    {  
    }

// ----------------------------------------------------------
// CXdmEngine::NewL
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmEngine* CXdmEngine::NewL( const CXdmProtocolInfo& aProtocolInfo )
    {
    CXdmEngine* self = new ( ELeave ) CXdmEngine();
    CleanupStack::PushL( self );
    self->ConstructL( CONST_CAST( CXdmProtocolInfo&, aProtocolInfo ) );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXdmEngine::XdmSettingsLC
// 
// ----------------------------------------------------------
//
EXPORT_C CDesC16Array* CXdmEngine::XdmSettingsLC( RArray<TInt>& aSettingIds )
    {
    return TXdmSettingsApi::CollectionNamesLC( aSettingIds );
    }
    
// ----------------------------------------------------------
// CXdmEngine::ConstructL
// 
// ----------------------------------------------------------
//
void CXdmEngine::ConstructL( CXdmProtocolInfo& aProtocolInfo )
    {
    #ifdef _DEBUG
        iLogWriter = CXdmLogWriter::NewL( KXdmEngLogFile );
        WriteToLog( _L8( "CXdmEngine::ConstructL()" ) );
    #endif
    TRAPD( error, iXdmProtocol = CXdmProtocol::NewL( *this, aProtocolInfo ) );
    if( error == KErrNone && iXdmProtocol != NULL )
        {
        #ifdef _DEBUG
            WriteToLog( _L8( " Protocol implementation created successfully" ) );
        #endif
        CActiveScheduler::Add( this );
        }
    else
        {
        #ifdef _DEBUG
            WriteToLog( _L8( " Protocol creation failed - Error: %d  Protocol: %x" ), error, iXdmProtocol );
        #endif
        User::Leave( KErrUnknown );
        }
    }

// ----------------------------------------------------
// CXdmEngine::~CXdmEngine
// 
// ----------------------------------------------------
//
CXdmEngine::~CXdmEngine()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::~CXdmEngine()" ) );
    #endif
    iDocUpdateQueue.Close();
    iDirUpdateQueue.Close();
    iDocumentQueue.Close();
    iDirectoryQueue.Close();
    #ifdef _DEBUG
        WriteToLog( _L8( "  Queues closed" ) );
    #endif
    delete iXdmProtocol;
    #ifdef _DEBUG
        WriteToLog( _L8( "  Protocol deleted" ) );
    #endif
    REComSession::FinalClose();
    #ifdef _DEBUG
        WriteToLog( _L8( "  Destructor finished" ) );
    #endif
    delete iLogWriter;
    }

// ----------------------------------------------------------
// CXdmEngine::WriteToLog
// 
// ----------------------------------------------------------
//
void CXdmEngine::WriteToLog( TRefByValue<const TDesC8> aFmt,... ) const                                 
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    iLogWriter->WriteToLog( buf );
    }
    
// ----------------------------------------------------
// CXdmEngine::DirectoryCollection
// 
// ----------------------------------------------------
//
EXPORT_C const RPointerArray<CXdmDocument>& CXdmEngine::DocumentCollection() const
    {
    return iDocumentQueue;
    }

// ----------------------------------------------------
// CXdmEngine::DirectoryCollection
// 
// ----------------------------------------------------
//
EXPORT_C const RPointerArray<CXdmDirectory>& CXdmEngine::DirectoryCollection() const
    {
    return iDirectoryQueue;
    }
        
    
// ----------------------------------------------------
// CXdmEngine::CancelUpdate
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmEngine::CancelUpdate( CXdmDocument* aDocument )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::CancelUpdate( document )" ) );
    #endif
    TInt index = iDocumentQueue.Find( aDocument );
    if( IsActive() )
        {
        #ifdef _DEBUG
            WriteToLog( _L8( " Transfer media init pending, cancel self" ) );
        #endif
        Cancel();
        }
    else if( index != KErrNotFound && aDocument->IsActive() )
        {
        #ifdef _DEBUG
            WriteToLog( _L8( " Cancel document update" ) );
        #endif
        aDocument->CancelUpdate();
        if( iDocUpdateQueue.Find( aDocument ) == KErrNone )
            iDocUpdateQueue.Remove( index );
        }
    }

// ----------------------------------------------------
// CXdmEngine::CancelUpdate
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmEngine::CancelUpdate( CXdmDirectory* aDirectory )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::CancelUpdate( directory )" ) );
    #endif
    TInt index = iDirectoryQueue.Find( aDirectory );
    if( IsActive() )
        {
        #ifdef _DEBUG
            WriteToLog( _L8( " Transfer media init pending, cancel self" ) );
        #endif
        Cancel();
        }
    else if( index != KErrNotFound && aDirectory->IsActive() )
        {
        #ifdef _DEBUG
            WriteToLog( _L8( " Cancel directory update" ) );
        #endif
        aDirectory->CancelUpdate();
        if( iDirUpdateQueue.Find( aDirectory ) == KErrNone )
            iDirUpdateQueue.Remove( index );
        }
    }
    
// ----------------------------------------------------
// CXdmEngine::CreateDocumentModelL
// 
// ----------------------------------------------------
//
EXPORT_C CXdmDocument* CXdmEngine::CreateDocumentModelL( const TDesC& aDocumentName,
                                                         const TXdmDocType aDocumentType )                                 
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::CreateDocumentModelL()" ) );
    #endif
    CXdmDocument* document = iXdmProtocol->CreateDocumentL( aDocumentName, aDocumentType );
    CleanupStack::PushL( document );
    User::LeaveIfError( iDocumentQueue.Append( document ) );
    CleanupStack::Pop();  //document
    return document;
    }
    
// ----------------------------------------------------
// CXdmEngine::DeleteDocumentModelL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmEngine::DeleteDocumentModelL( const CXdmDocument* aDocument )                                 
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::DeleteDocumentModelL()" ) );
    #endif
    TInt index = iDocumentQueue.Find( aDocument );
    __ASSERT_ALWAYS( index != KErrNotFound, User::Leave( KErrNotFound ) );
    iDocumentQueue.Remove( index );
    delete aDocument;
    aDocument = NULL;
    }

// ----------------------------------------------------
// CXdmEngine::CreateDirectoryModelL
// 
// ----------------------------------------------------
//
EXPORT_C CXdmDirectory* CXdmEngine::CreateDirectoryModelL( const TDesC& aDirectoryPath )                                 
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::CreateDirectoryModelL()" ) );
    #endif
    CXdmDirectory* directory = iXdmProtocol->CreateDirectoryL( aDirectoryPath );
    CleanupStack::PushL( directory );
    User::LeaveIfError( iDirectoryQueue.Append( directory ) );
    CleanupStack::Pop();  //document
    return directory;
    }

// ----------------------------------------------------
// CXdmEngine::DeleteDirectoryModelL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmEngine::DeleteDirectoryModelL( const CXdmDirectory* aDirectory )                                 
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::DeleteDirectoryModelL()" ) );
    #endif
    TInt index = iDirectoryQueue.Find( aDirectory );
    __ASSERT_ALWAYS( index != KErrNotFound, User::Panic( _L( "CXdmEngine" ), EDirModelNotFound ) );
    iDirectoryQueue.Remove( index );
    delete aDirectory;
    aDirectory = NULL;
    }

// ----------------------------------------------------
// CXdmEngine::CreateDocumentNodeL
// 
// ----------------------------------------------------
//
EXPORT_C CXdmDocumentNode* CXdmEngine::CreateDocumentNodeL()                                 
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::CreateDocumentNodeL()" ) );
    #endif
    return iXdmProtocol->CreateDocumentNodeL();
    }
        
// ----------------------------------------------------
// CXdmEngine::UpdateL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmEngine::UpdateL( CXdmDocument* aDocument,
                                   TRequestStatus& aStatus )                                 
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::UpdateL( document )" ) );
    #endif
    iClientStatus = &aStatus;
    aStatus = KRequestPending;
    aDocument->SaveClientStatus( aStatus );
    User::LeaveIfError( iDocUpdateQueue.Append( aDocument ) );
    if( !IsActive() )
        {
        iXdmProtocol->InitTransferMedium( KDefaultIdleTimeout, iStatus );
        SetActive();
        }
    }

// ----------------------------------------------------
// CXdmEngine::UpdateL
// 
// ----------------------------------------------------
//
EXPORT_C void CXdmEngine::UpdateL( TRequestStatus& aStatus,
                                   CXdmDirectory* aDirectory,
                                   TDirUpdatePhase aUpdatePhase )                                 
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::UpdateL( directory )" ) );
    #endif
    iClientStatus = &aStatus;
    aStatus = KRequestPending;
    aDirectory->SaveRequestData( aUpdatePhase, aStatus );
    User::LeaveIfError( iDirUpdateQueue.Append( aDirectory ) ); 
    if( !IsActive() )
        {
        iXdmProtocol->InitTransferMedium( KDefaultIdleTimeout, iStatus );
        SetActive();
        }
    }
    
// ---------------------------------------------------------
// CXdmEngine::RunL
// 
// ---------------------------------------------------------
//
void CXdmEngine::RunL()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::RunL() - Status: %d" ), iStatus.Int() );
    #endif
    if( iStatus == KErrCancel )
        {
        #ifdef _DEBUG
            WriteToLog( _L8( " Update cancelled, complete client request" ) );
        #endif
        if( iClientStatus )
            {
            iDocUpdateQueue.Reset();
            iDirUpdateQueue.Reset();
            User::RequestComplete( iClientStatus, KErrCancel );
            #ifdef _DEBUG
                WriteToLog( _L8( "  Request complete" ) );
            #endif
            }
        }  
    else
        {
        #ifdef _DEBUG
            WriteToLog( _L8( " Transfer media initialised, start update" ) );
        #endif
        TInt dirCount( iDirUpdateQueue.Count() );
        TInt docCount( iDocUpdateQueue.Count() );
        #ifdef _DEBUG
            WriteToLog( _L8( "  Document queue count:  %d" ), docCount );
            WriteToLog( _L8( "  Directory queue count: %d" ), dirCount );
        #endif
        //First check the document queue
        for( TInt i = 0;i < docCount;i++ )
            {
            iDocUpdateQueue[0]->StartUpdateL();
            iDocUpdateQueue.Remove( 0 );
            }
        //Then the directory queue
        for( TInt j = 0;j < dirCount;j++ )
            {
            iDirUpdateQueue[0]->StartUpdateL();
            iDirUpdateQueue.Remove( 0 );
            }
        }
    }

// ---------------------------------------------------------
// CXdmEngine::DoCancel
// 
// ---------------------------------------------------------
//
void CXdmEngine::DoCancel()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::DoCancel()" ) );
    #endif
    iXdmProtocol->CancelTransferMediumInit();
    if( iClientStatus )
        {
        #ifdef _DEBUG
            WriteToLog( _L8( "  Client request pending => Complete" ) );
        #endif
        iDocUpdateQueue.Reset();
        iDirUpdateQueue.Reset();
        User::RequestComplete( iClientStatus, KErrCancel );
        }
    }

// ---------------------------------------------------------
// CXdmEngine::XdmProtocol
// 
// ---------------------------------------------------------
//
CXdmProtocol* CXdmEngine::XdmProtocol() const
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::XdmProtocol()" ) );
    #endif
    return iXdmProtocol;
    }

// ---------------------------------------------------------
// CXdmEngine::ConvertToUTF8L
// 
// ---------------------------------------------------------
//
HBufC8* CXdmEngine::ConvertToUTF8L( const TDesC& aUnicodeDesc )
    {
    /*#ifdef _DEBUG
        WriteToLog( _L8( "CXdmEngine::ConvertToUTF8L()" ) );
    #endif*/
    return CnvUtfConverter::ConvertFromUnicodeToUtf8L( aUnicodeDesc );
    }
    




