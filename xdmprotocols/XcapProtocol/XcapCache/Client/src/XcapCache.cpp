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
* Description:   RXcapCache
*
*/



#include "XcapCache.h"
#include "CommonDefines.h"


//CLASS DECLARATION
EXPORT_C RXcapCache::RXcapCache() : iConnected( EFalse )
    {
    }

// ----------------------------------------------------
// RXcapCache::Store
// 
// ----------------------------------------------------
//
EXPORT_C void RXcapCache::Store( const TDesC8& aETag, 
                                 const TDesC& aDocumentName,
                                 const TDesC8& aRootLocation,
                                 const TDesC8& aResponseData )
    {
    SendReceive( EXcapCacheStore, TIpcArgs( &aDocumentName, &aRootLocation, &aETag, &aResponseData ) );
    }

// ----------------------------------------------------
// RXcapCache::Delete
// 
// ----------------------------------------------------
//
EXPORT_C TInt RXcapCache::Delete( const TDesC& aDocumentName,
                                  const TDesC8& aRootLocation )
    {
    TCacheEntryInfo entryInfo;
    entryInfo.iRootUri = CONST_CAST( TDesC8*, &aRootLocation );
    entryInfo.iDocumentUri = CONST_CAST( TDesC*, &aDocumentName );
    TPckgC<TCacheEntryInfo> package( entryInfo );
    return SendReceive( EXcapCacheDelete, TIpcArgs( &package ) );
    }
                                  
// ----------------------------------------------------
// RXcapCache::FetchDocumentInfo
// 
// ----------------------------------------------------
//
EXPORT_C TInt RXcapCache::FetchDocumentInfo( TDes8& aETagDescriptor,
                                             const TDesC& aDocumentName,
                                             const TDesC8& aRootLocation,
                                             TCacheEntryInfo& aEntryInfo )
    {
    TPckg<TCacheEntryInfo> package( aEntryInfo );
    return SendReceive( EXcapCacheFetchInfo, TIpcArgs( &package, &aETagDescriptor,
                                                       &aDocumentName, &aRootLocation ) );
    }

// ----------------------------------------------------
// RXcapCache::FetchDocumentContent
// 
// ----------------------------------------------------
//
EXPORT_C void RXcapCache::FetchDocumentContent( TDes8& aContentDescriptor,
                                                const TDesC& aDocumentName,
                                                const TDesC8& aRootLocation )
    {
    SendReceive( EXcapCacheFetchData, TIpcArgs(
                 &aDocumentName, &aRootLocation, &aContentDescriptor ) );
    }

// ----------------------------------------------------
// RXcapCache::IsCurrent
// 
// ----------------------------------------------------
//
EXPORT_C TInt RXcapCache::IsCurrent( const TDesC8& aETag, 
                                     const TDesC& aDocumentName,
                                     const TDesC8& aRootLocation )
    {
    return SendReceive( EXcapCacheCheckValidity, TIpcArgs(
                        &aETag, &aDocumentName, &aRootLocation ) );
    }
                                 
// ----------------------------------------------------
// RXcapCache::Flush
// 
// ----------------------------------------------------
//
EXPORT_C void RXcapCache::Flush()
    {
    SendReceive( EXcapCacheFlush );
    }
        
// ----------------------------------------------------
// RXcapCache::Version
// 
// ----------------------------------------------------
//
EXPORT_C TVersion RXcapCache::Version() const
	{
	return TVersion( KCacheServerMajorVersionNumber,
					 KCacheServerMinorVersionNumber,
					 KCacheServerBuildVersionNumber );
	}

// ----------------------------------------------------
// RXcapCache::~RXcapCache
// 
// ----------------------------------------------------
//
EXPORT_C RXcapCache::~RXcapCache()
    {
    }

// ----------------------------------------------------
// RXcapCache::Connect()
// 
// ----------------------------------------------------
//
EXPORT_C TInt RXcapCache::Connect()
    {
    TInt ret = KErrNone;
    if( !iConnected )
        {
        ret = StartServer();
        if( ret == KErrNone || ret == KErrAlreadyExists )
            {
            ret = CreateSession( KXcapCacheServerLib, Version(), KTTDefaultMessageSlots );
            iConnected = ( ret == KErrNone );
            }
        }
    return ret; 
    }

// ----------------------------------------------------
// RXcapCache::StartServer
//
// ----------------------------------------------------
//
TInt RXcapCache::StartServer()
    {
    TInt ret = KErrNone;
    TRequestStatus status = KRequestPending; 
    if ( !IsRunning() )
        {
        RProcess server;
        ret = server.Create( KXcapCacheServerExe, KNullDesC );       
        if( ret != KErrNone )
            return ret;     
        server.Rendezvous( status );    	
        status != KRequestPending ? server.Kill( 0 ) : server.Resume();
        //Wait for start or death 
        User::WaitForRequest( status );	
        ret = server.ExitType() == EExitPanic ? KErrGeneral : status.Int();
        server.Close();	
        }
    return ret;
    }

// ----------------------------------------------------
// RXcapCache::IsRunning
//
// ----------------------------------------------------
//
TBool RXcapCache::IsRunning()
    {
    TFindServer findServer( KXcapCacheServerName );
    TFullName name;
    return ( findServer.Next( name ) == KErrNone );
    } 
  
// End of File


