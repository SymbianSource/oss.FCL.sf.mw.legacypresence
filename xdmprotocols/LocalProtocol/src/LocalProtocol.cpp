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
* Description: CLocalProtocol
*
*/



#include <hal.h>
#include <f32file.h>
#include <xdmlogwriter.h>
#include <implementationproxy.h>
#include "XdmXmlParser.h"
#include "LocalProtocol.h"
#include "LocalDocument.h"
#include "LocalDocumentNode.h"
#include "LocalDirectory.h"
#include "XdmCredentials.h"
#include "XdmProtocolInfo.h"
#include "XdmOperationFactory.h"

// ----------------------------------------------------------
// CLocalProtocol::CLocalProtocol
// 
// ----------------------------------------------------------
//
CLocalProtocol::CLocalProtocol( const CXdmEngine& aXdmEngine,
                                const CXdmProtocolInfo& aProtocolInfo ) :
                                iTransferMediaOpen( EFalse ),
                                iXdmEngine( CONST_CAST( CXdmEngine&, aXdmEngine ) ),
                                iProtocolInfo( aProtocolInfo )
    {   
    }
    
// ----------------------------------------------------------
// CLocalProtocol::DeleteLogFileL
// 
// ----------------------------------------------------------
//
CLocalProtocol* CLocalProtocol::NewL( const TXdmProtocolParams& aProtocolParams )
    {
    CLocalProtocol* self = new ( ELeave ) CLocalProtocol( aProtocolParams.iXdmEngine,
                                                          aProtocolParams.iProtocolInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CLocalProtocol::DeleteLogFileL
// 
// ----------------------------------------------------------
//
CLocalProtocol::~CLocalProtocol()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CLocalProtocol::~CLocalProtocol()" ) );  
    #endif
    delete iRootFolder;
    delete iXmlParser;
    delete iLogWriter;
    iFileSession.Close();
    Dll::FreeTls();
    }
        
// ----------------------------------------------------------
// CLocalProtocol::ConstructL
// 
// ----------------------------------------------------------
//
void CLocalProtocol::ConstructL()
    {
    #ifdef _DEBUG
        iLogWriter = CXdmLogWriter::NewL( KLocalEngLogFile );
        WriteToLog( _L8( "CLocalProtocol::ConstructL() " ) );  
    #endif
    iXmlParser = CXdmXmlParser::NewL();
    SetRootDirectoryL( iProtocolInfo.Root() ); 
    User::LeaveIfError( iFileSession.Connect() );
    Dll::SetTls( this );                                                                               
    }

// ----------------------------------------------------------
// CLocalProtocol::SetRootDirectoryL
// 
// ----------------------------------------------------------
//
void CLocalProtocol::SetRootDirectoryL( const TDesC& aRootPath )
    {
    TUid id = RProcess().Identity();
    TBuf<32> processId;
    processId.Zero();
    processId.Format( _L( "%x" ), id );
    #ifdef _DEBUG
        WriteToLog( _L8( "CLocalProtocol::SetRootDirectoryL()" ) );  
    #endif
    if( aRootPath.Length() > 0 )
        {
        iRootFolder = HBufC::NewL( aRootPath.Length() +
                                   processId.Length() + 
                                   TPtrC( KLocalEngPrivateRoot ).Length() + 4 );
        iRootFolder->Des().Copy( KLocalEngPrivateRoot );
        iRootFolder->Des().Append( processId );
        iRootFolder->Des().Append( _L( "\\" ) );
        iRootFolder->Des().Append( aRootPath );
        iRootFolder->Des().Append( _L( "\\" ) );
        }
    else
        {
        iRootFolder = HBufC::NewL( processId.Length() + 
                                   KLocalEngPrivateRoot().Length() + 2 );
        iRootFolder->Des().Copy( KLocalEngPrivateRoot );
        iRootFolder->Des().Append( processId );
        iRootFolder->Des().Append( _L( "\\" ) );
        }
    #ifdef _DEBUG
        TBuf8<256> eight;
        eight.Copy( iRootFolder->Des() );
        WriteToLog( _L8( "  Path: %S" ), &eight );  
    #endif
    }

// ----------------------------------------------------------
// CLocalProtocol::.WriteToLog
// 
// ----------------------------------------------------------
//
void CLocalProtocol::WriteToLog( TRefByValue<const TDesC8> aFmt,... ) const                                
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    iLogWriter->WriteToLog( buf );
    }
    
// ----------------------------------------------------
// CLocalProtocol::InitTransferMedia
// 
// ----------------------------------------------------
//
void CLocalProtocol::InitTransferMedium( TInt /*aIdleTimeout*/,
                                        TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNone );
    }

// ----------------------------------------------------
// CLocalProtocol::CancelTransferMediaInit
// 
// ----------------------------------------------------
//
void CLocalProtocol::CancelTransferMediumInit()
    {
    }

// ----------------------------------------------------
// CLocalProtocol::IsTransferAvailable
// 
// ----------------------------------------------------
//
TBool CLocalProtocol::IsTransferAvailable() const
    {
    return ETrue;
    }
            
// ----------------------------------------------------
// CLocalProtocol::RFSession
// 
// ----------------------------------------------------
//
RFs& CLocalProtocol::RFSession()
    {
    return iFileSession;
    }

// ----------------------------------------------------
// CLocalProtocol::Root
// 
// ----------------------------------------------------
//
TPtrC CLocalProtocol::Root()
    {
    return iRootFolder != NULL ? iRootFolder->Des() : TPtrC();
    }
    
// ----------------------------------------------------------
// void CXcapProtocol::DeleteLogFileL
// 
// ----------------------------------------------------------
//
EXPORT_C RFs& CLocalProtocol::FileSession()
    {
    TAny* ptr = Dll::Tls();
    CLocalProtocol* self = ( CLocalProtocol* )ptr;
    return self->RFSession();  
    }
    
// ----------------------------------------------------
// CLocalProtocol::RootFolder
// 
// ----------------------------------------------------
//
EXPORT_C TPtrC CLocalProtocol::RootFolder()
    {
    TAny* ptr = Dll::Tls();
    CLocalProtocol* self = ( CLocalProtocol* )ptr;
    return self->Root();
    }

// ----------------------------------------------------
// CLocalProtocol::CreateDocumentNodeL
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CLocalProtocol::CreateDocumentNodeL()
    {
    return CLocalDocumentNode::NewL( iXdmEngine, *this );
    }
    
// ----------------------------------------------------
// CLocalProtocol::CreateDocumentL
// 
// ----------------------------------------------------
//
CXdmDocument* CLocalProtocol::CreateDocumentL( const TDesC& aDocumentName,
                                               const TXdmDocType /*aDocumentType*/ )
    {
    return CLocalDocument::NewL( iXdmEngine, aDocumentName, *this );
    }

// ----------------------------------------------------
// CLocalProtocol::CreateDirectoryL
//
// ----------------------------------------------------
//
CXdmDirectory* CLocalProtocol::CreateDirectoryL( const TDesC& aDirectoryPath )
    {
    TPtrC root = iRootFolder->Des();
    HBufC* fullName = HBufC::NewLC( root.Length() + aDirectoryPath.Length() );
    fullName->Des().Copy( root );
    fullName->Des().Append( aDirectoryPath );
    CXdmDirectory* dir = CLocalDirectory::NewL( fullName->Des(), iXdmEngine, *this );
    CleanupStack::PopAndDestroy();  //fullName
    return dir;
    }

// ---------------------------------------------------------
// Map the interface UIDs to implementation factory functions
// 
// ---------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    {
    #ifdef __EABI__
        IMPLEMENTATION_PROXY_ENTRY( 0x10207424, CLocalProtocol::NewL )
    #else
        { { 0x10207424 }, CLocalProtocol::NewL }
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
    

// End of File
