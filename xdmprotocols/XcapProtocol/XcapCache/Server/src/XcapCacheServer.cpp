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
* Description:   CXcapCacheServer
*
*/




// INCLUDE FILES
#include <e32std.h>
#include <e32math.h>
#include <flogger.h>
#include <centralrepository.h>
#include <XdmEngineDomainCRKeys.h>
#include "XcapCacheIndex.h"
#include "XcapCacheServer.h"
#include "XcapCacheSession.h"
#include "XcapCacheIndexAdmin.h"
#include "XcapCacheServerMain.h"

GLDEF_C void PanicServer( TXcapCacheServerPanic aPanic )
    {
    _LIT( KCacheServerPanic, "XcapCacheServer");
    User::Panic( KCacheServerPanic, aPanic );
    }

// ----------------------------------------------------------
// CXcapCacheServer::New
// 
// ----------------------------------------------------------
//
CServer2* CXcapCacheServer::NewLC()
    {
    CXcapCacheServer* self = new CXcapCacheServer( EPriorityHigh );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------
// CXcapCacheServer::New
// 
// ----------------------------------------------------------
//
void CXcapCacheServer::ConstructL()
    {
    ReadMaxCacheSizeL();
    User::LeaveIfError( iFileSession.Connect() );
    StartL( KXcapCacheServerLib );
    iShutdownTimer.ConstructL();
    iShutdownTimer.Start();
    }
    
// ----------------------------------------------------------
// CXcapCacheServer::NewSessionL
// 
// ----------------------------------------------------------
//
CSession2* CXcapCacheServer::NewSessionL( const TVersion& aVersion,
        								  const RMessage2& /*aMessage*/ ) const
    {
    // Check version number of API against our known version number.
    TVersion thisVersion( KCacheServerMajorVersionNumber,
                          KCacheServerMinorVersionNumber,
                          KCacheServerBuildVersionNumber );
    if( !User::QueryVersionSupported( thisVersion, aVersion ) ) 
        User::Leave( KErrNotSupported );
    return CXcapCacheSession::NewL( ( CXcapCacheServer* )this );
    }

// ----------------------------------------------------------
// CXcapCacheServer::CXcapCacheServer
// 
// ----------------------------------------------------------
//
CXcapCacheServer::~CXcapCacheServer()
    {
    iFileSession.Close();
    }

// ----------------------------------------------------------
// CXcapCacheServer::ReadMaxCacheSizeL
// 
// ----------------------------------------------------------
//
void CXcapCacheServer::ReadMaxCacheSizeL()
    {
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheServer::ReadMaxCacheSizeL()" ) );
    #endif
    CRepository* repository = CRepository::NewL( KCRUIDXDMEngineXcapLocal );
    CleanupStack::PushL( repository );
    TInt error = repository->Get( KXDMXcapCacheSize, iMaxCacheSize );
    //If something went wrong, just use 30 KB as a default value
    if( error != KErrNone )
        {
        #ifdef _DEBUG
            CXcapCacheServer::WriteToLog( _L8( "Reading from CenRep failed: %d => Default to 30 KB" ), error );
        #endif
        iMaxCacheSize = 30000;
        }
    else
        {
        #ifdef _DEBUG
            CXcapCacheServer::WriteToLog( _L8( "  Max cache size is %d KB" ), iMaxCacheSize );
        #endif  
        iMaxCacheSize = iMaxCacheSize * 1000;
        }
    CleanupStack::PopAndDestroy();  //repository
    }

// ----------------------------------------------------------
// CXcapCacheServer::CXcapCacheServer
// 
// ----------------------------------------------------------
//
CXcapCacheServer::CXcapCacheServer( TInt aPriority ) : CServer2( aPriority )
    {
    }

// ----------------------------------------------------------
// CXcapCacheServer::AddSession
//
// ----------------------------------------------------------
//
void CXcapCacheServer::AddSession()
    {
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheServer::AddSession()" ) );
    #endif
	++iSessionCount;
	iShutdownTimer.Cancel();
    }
        
// ----------------------------------------------------------
// CXcapCacheServer::DropSession
// 
// ----------------------------------------------------------
//
void CXcapCacheServer::DropSession()
    {
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapCacheServer::DropSession()" ) );
    #endif
	if( --iSessionCount == 0 )
		CActiveScheduler::Stop();
    }
    
// ----------------------------------------------------------
// CXcapCacheServer::ConvertDesc
// 
// ----------------------------------------------------------
//
TInt CXcapCacheServer::ConvertDesc( const TDesC8& aNumberDesc )
    {
    TInt ret = 0;
    TLex8 lex( aNumberDesc );
    TInt error = lex.Val( ret );
    return error == KErrNone ? ret : error;
    }

// ---------------------------------------------------------
// CXcapCacheServer::DateL
// 
// ---------------------------------------------------------
//
HBufC* CXcapCacheServer::DateL()
    {
    TTime time;
    TBuf<KDateMaxSize> dateBuffer;
    dateBuffer.SetLength( 0 );
    time.HomeTime();
    time.FormatL( dateBuffer, KCacheDateFormat );
    HBufC* heapDate = dateBuffer.AllocL();
    return heapDate;
    }

// ---------------------------------------------------------
// CXcapCacheServer::DateTimeL
// 
// ---------------------------------------------------------
//
HBufC* CXcapCacheServer::DateTimeL()
    {
    TTime time;
    TBuf<KDateMaxSize> dateBuffer;
    time.HomeTime();
    time.FormatL( dateBuffer, KDateFormatFileName );
    TBuf<KDateMaxSize> timeBuffer;
    time.HomeTime();
    time.FormatL( timeBuffer, KTimeFormatFileName );
    TBuf<KDateTimeMaxSize> buffer;
    buffer.SetLength( 0 );
    buffer.Copy( dateBuffer );
    buffer.Append( timeBuffer );
    HBufC* ret = HBufC::NewL( buffer.Length() );
    ret->Des().Copy( buffer );
    return ret;
    }

// ---------------------------------------------------------
// CXcapCacheServer::DateTimeL
// 
// ---------------------------------------------------------
//
HBufC* CXcapCacheServer::DateTimeL( const TTime& aTime )
    {
    TBuf<KDateTimeMaxSize> dateTimeBuffer;
    aTime.FormatL( dateTimeBuffer, KDateTimeFormat );
    HBufC* heapDateTime = dateTimeBuffer.AllocL();
    return heapDateTime;
    }
    
// ----------------------------------------------------------
// CXcapCacheServer::MaxCacheSize
// 
// ----------------------------------------------------------
//
TInt CXcapCacheServer::MaxCacheSize() const
    {
    return iMaxCacheSize;
    }
    
// ---------------------------------------------------------
// CXcapCacheServer::CacheSize
// 
// ---------------------------------------------------------
//
TInt CXcapCacheServer::CacheSize( TInt& aEntryCount )
    {
    #ifdef _DEBUG
	    CXcapCacheServer::WriteToLog( _L8( "CXcapCacheServer::CacheSize()" ) );
	#endif
    TInt total = 0;
    TInt entryCount = 0;
    CDir* directory = NULL;
    User::LeaveIfError( iFileSession.GetDir( KCacheServerRoot, KEntryAttNormal,
                                             ESortNone, directory ) );
    CleanupStack::PushL( directory );
    entryCount = directory->Count();
    for( TInt i = 0;i < entryCount;i++ )
        total = total + ( *directory )[i].iSize;
    #ifdef _DEBUG
	    CXcapCacheServer::WriteToLog( _L8( " Entries:    %d " ), entryCount );
	    CXcapCacheServer::WriteToLog( _L8( " Total size: %d bytes" ), total );
	#endif
	CleanupStack::PopAndDestroy();
	aEntryCount = entryCount;
	return total;
    }

// ---------------------------------------------------------
// CXcapCacheServer::RandomStringL
// 
// ---------------------------------------------------------
//
HBufC* CXcapCacheServer::RandomStringL()
    {
    const TInt charCount( sizeof( KRandomStringCharArray ) / sizeof( TInt ) );
    TBuf<KRandStringLength> buffer;
    buffer.Zero();
    for( TInt i = 0; i < KRandStringLength;i++ )
        {
        TInt index = Math::Random() % charCount;
        buffer.Append( ( TChar )KRandomStringCharArray[index] );
        }
    HBufC* randomHeapBuffer = buffer.AllocL();
    return randomHeapBuffer;
    }

// ---------------------------------------------------------
// CXcapCacheServer::TimeL
// 
// ---------------------------------------------------------
//
HBufC* CXcapCacheServer::TimeL()
    {
    TTime time;
    TBuf<KDateMaxSize> timeBuffer;
    timeBuffer.SetLength( 0 );
    time.HomeTime();
    time.FormatL( timeBuffer, KCacheTimeFormat );
    HBufC* heapTime = timeBuffer.AllocL();
    return heapTime;
    }

// ---------------------------------------------------------
// CXcapCacheServer::DescriptorCast
//
// ---------------------------------------------------------
//
TPtr8 CXcapCacheServer::DescriptorCast( const TDesC8& aConstData )
	{
    TInt length = aConstData.Length();
    TPtr8 constCast( CONST_CAST( TUint8*, aConstData.Ptr() ), length, length );
    constCast.TrimAll();
    return constCast;
	}

// ----------------------------------------------------
// CXcapCacheServer::FileSession
// 
// ----------------------------------------------------
//
RFs& CXcapCacheServer::FileSession()
    {
    return FsSession;
    }

// ----------------------------------------------------
// CXcapCacheServer::FileSession
// 
// ----------------------------------------------------
//
CXcapCacheIndex* CXcapCacheServer::Index()
    {
    return CacheIndex;
    }

// ----------------------------------------------------
// CXcapCacheServer::FileSession
// 
// ----------------------------------------------------
//
CXcapCacheIndexAdmin* CXcapCacheServer::IndexAdmin()
    {
    return CacheIndexAdmin;
    }

// ----------------------------------------------------
// CXcapCacheServer::StartThreadL
// 
// ----------------------------------------------------
//
EXPORT_C TInt CXcapCacheServer::StartThreadL()
    {
    User::LeaveIfError( User::RenameThread( KXcapCacheServerName ) );
    CActiveScheduler* scheduler = new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );
    User::LeaveIfError( FsSession.Connect() );
    CleanupClosePushL( FsSession );
    CXcapCacheServer::NewLC();
    CacheIndex = CXcapCacheIndex::NewL();
    CleanupStack::PushL( CacheIndex );
    CacheIndexAdmin = CXcapCacheIndexAdmin::NewL();
    CleanupStack::PushL( CacheIndexAdmin );
    RProcess::Rendezvous( KErrNone );
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "XcapCacheServer fully running" ) );
    #endif
    CActiveScheduler::Start();
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "XcapCacheServer closing..." ) );
    #endif
    CleanupStack::PopAndDestroy( 5 );  //CacheIndexAdmin, CacheIndex, server, FsSession, scheduler
    return KErrNone;
    }

// ----------------------------------------------------
// CXcapCacheServer::WriteToLog
// 
// ----------------------------------------------------
//
EXPORT_C void CXcapCacheServer::WriteToLog( TRefByValue<const TDesC8> aFmt,... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KServerLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    RFileLogger::Write( KCacheServerLogDir, KCacheServerLogFile, EFileLoggingModeAppend, buf );
    RDebug::RawPrint( buf );
    }
    

