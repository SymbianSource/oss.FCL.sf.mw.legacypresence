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
* Description:   CXcapHttpTransport
*
*/




// INCLUDE FILES
#include <in_iface.h>
#include <xdmlogwriter.h>
#include <deflatefilterinterface.h>
#include <msgconnmanagerapi.h>
#include "XdmCredentials.h"
#include "XcapHttpReqGet.h"
#include "XcapHttpReqPut.h"
#include "XcapHttpReqMkcol.h"
#include "XcapHttpReqHead.h"
#include "XcapHttpRequest.h"
#include "XcapHttpReqDelete.h"
#include "XcapHttpTransport.h"
#include "XcapHttpAuthManager.h"

// ================= MEMBER FUNCTIONS =======================
//


// ----------------------------------------------------------
// CXcapHttpTransport::CXcapHttpTransport
// 
// ----------------------------------------------------------
//
CXcapHttpTransport::CXcapHttpTransport( MMsgConnManager& aConnManager) :
                                        iCredValidity( EFalse ),
                                        iSessionClosed( ETrue ),
                                        iConnManager( aConnManager )
    { 
    }

// ----------------------------------------------------------
// CXcapHttpTransport::NewL
// 
// ----------------------------------------------------------
//
EXPORT_C CXcapHttpTransport* CXcapHttpTransport::NewL( const TDesC& aRootUri,
                                                       MMsgConnManager& aConnManager,
                                                       const TXdmCredentials& aDigestCredentials )
    {
    CXcapHttpTransport* self = new ( ELeave ) CXcapHttpTransport( aConnManager );
    CleanupStack::PushL( self );
    self->ConstructL( aRootUri, aDigestCredentials );
    CleanupStack::Pop();  //self
    return self;
    }

// ----------------------------------------------------
// CXcapHttpTransport::~CXcapHttpTransport
// 
// ----------------------------------------------------
//
CXcapHttpTransport::~CXcapHttpTransport()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapHttpTransport::~CXcapHttpTransport()" ) );
    #endif
    delete iRootUri;
    delete iAuthManager;
    iHttpSession.Close();
    iSessionClosed = ETrue;
    delete iProxyBuffer;
    #ifdef _DEBUG
        WriteToLog( _L8( "  Destructor ends." ) );
    #endif
    delete iLogWriter;
    }

// ----------------------------------------------------------
// CXcapHttpTransport::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapHttpTransport::ConstructL( const TDesC& aRootUri, const TXdmCredentials& aDigestCredentials )
    {
    #ifdef _DEBUG
        iLogWriter = CXdmLogWriter::NewL( KTransportLogFile );
        WriteToLog( _L8( "CXcapHttpTransport::ConstructL" ) );
    #endif
    SetRootUriL( aRootUri );
    InitialiseSessionL();
    iAuthManager = CXcapHttpAuthManager::NewL( iHttpSession, *this, aDigestCredentials );
    } 

// ---------------------------------------------------------
// CXcapHttpTransport::SetRootUriL
// 
// ---------------------------------------------------------
//
EXPORT_C void CXcapHttpTransport::SetRootUriL( const TDesC& aRootUri )
    {
    delete iRootUri;
    iRootUri = NULL;
    iRootUri = HBufC8::NewL( aRootUri.Length() );
    iRootUri->Des().Copy( aRootUri );
    }
         
// ----------------------------------------------------
// CXcapHttpTransport::GetL
// 
// ----------------------------------------------------
//
EXPORT_C CXcapHttpReqGet* CXcapHttpTransport::GetL( const TDesC& aRequestUri )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapHttpTransport::GetRequestL()" ) );
    #endif
    if( !iSessionClosed )
        return CXcapHttpReqGet::NewL( aRequestUri, iHttpSession, *iAuthManager, *this );
    else
        {
        User::Leave( KErrNotReady );
        return NULL;
        }    
    }

// ----------------------------------------------------
// CXcapHttpTransport::PutL
// 
// ----------------------------------------------------
//
EXPORT_C CXcapHttpReqPut* CXcapHttpTransport::PutL( const TDesC& aRequestUri )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapHttpTransport::PutRequestL()" ) );
    #endif
    if( !iSessionClosed )
        return CXcapHttpReqPut::NewL( aRequestUri, iHttpSession, *iAuthManager, *this );
    else
        {
        User::Leave( KErrNotReady );
        return NULL;
        }    
    }

// ----------------------------------------------------
// CXcapHttpTransport::Mkcol
// 
// ----------------------------------------------------
//
EXPORT_C CXcapHttpReqMkcol* CXcapHttpTransport::MkcolL( const TDesC& aRequestUri )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapHttpTransport::MkcolRequestL()" ) );
    #endif
    if( !iSessionClosed )
        return CXcapHttpReqMkcol::NewL( aRequestUri, iHttpSession, *iAuthManager, *this );
    else
        {
        User::Leave( KErrNotReady );
        return NULL;
        }    
    }

// ----------------------------------------------------
// CXcapHttpTransport::Head
// 
// ----------------------------------------------------
//
EXPORT_C CXcapHttpReqHead* CXcapHttpTransport::HeadL( const TDesC& aRequestUri )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapHttpTransport::HeadRequestL()" ) );
    #endif
    if( !iSessionClosed )
        return CXcapHttpReqHead::NewL( aRequestUri, iHttpSession, *iAuthManager, *this );
    else
        {
        User::Leave( KErrNotReady );
        return NULL;
        }    
    }

// ----------------------------------------------------
// CXcapHttpTransport::DeleteL
// 
// ----------------------------------------------------
//
EXPORT_C CXcapHttpReqDelete* CXcapHttpTransport::DeleteL( const TDesC& aRequestUri )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapHttpTransport::DeleteRequestL()" ) );
    #endif
    if( !iSessionClosed )
        return CXcapHttpReqDelete::NewL( aRequestUri, iHttpSession, *iAuthManager, *this );
    else
        {
        User::Leave( KErrNotReady );
        return NULL;
        }    
    }

// ----------------------------------------------------------
// CXcapHttpTransport::RootUri
// 
// ----------------------------------------------------------
//
EXPORT_C TPtrC8 CXcapHttpTransport::RootUri()
    {
    return iRootUri != NULL ? iRootUri->Des() : TPtrC8();
    }

// ----------------------------------------------------
// CXcapHttpTransport::InitialiseSessionL
// 
// ----------------------------------------------------
//
void CXcapHttpTransport::InitialiseSessionL()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapHttpTransport::InitialiseSessionL()" ) );
    #endif
    iHttpSession.OpenL();
    RSocketServ& session = iConnManager.SocketSession();
    RConnection& connection = iConnManager.Connection();
    RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();
    RStringPool stringPool = iHttpSession.StringPool();
    TInt conn = reinterpret_cast <TInt> ( &connection );
    connInfo.SetPropertyL( stringPool.StringF( HTTP::EHttpSocketServ, RHTTPSession::GetTable() ),
                           THTTPHdrVal( session.Handle() ) );
    connInfo.SetPropertyL( stringPool.StringF( HTTP::EHttpSocketConnection, RHTTPSession::GetTable() ),
                           THTTPHdrVal( conn ) );
    connInfo.SetPropertyL( stringPool.StringF( HTTP::EHTTPVersion, RHTTPSession::GetTable() ),
                           THTTPHdrVal( stringPool.StringF( HTTP::EHttp11, RHTTPSession::GetTable() ) ) );
    connInfo.SetPropertyL( stringPool.StringF( HTTP::ESecureDialog, RHTTPSession::GetTable() ),
                           THTTPHdrVal( stringPool.StringF( HTTP::EDialogPrompt, RHTTPSession::GetTable() ) ) );
    //CHttpDeflateFilter::InstallFilterL( iHttpSession );
    iSessionClosed = EFalse;
    #ifdef _DEBUG
        WriteToLog( _L8( "InitialiseSessionL() ends." ) );
    #endif
    }

#ifdef _DEBUG              
// ----------------------------------------------------
// CXcapHttpTransport::WriteToLog
// Writes to the log, 8-bit version
// ----------------------------------------------------
//
void CXcapHttpTransport::WriteToLog( TRefByValue<const TDesC8> aFmt,... ) const
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    iLogWriter->WriteToLog( buf );
    }
       
// ----------------------------------------------------
// CXcapHttpTransport::WriteToLog
// Writes to the log, 16-bit version
// ----------------------------------------------------
//
void CXcapHttpTransport::WriteToLog( TRefByValue<const TDesC> aFmt,... ) const
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf<KLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    iLogWriter->WriteToLog( buf );
    }
#endif  //_DEBUG
      



// End of File  
