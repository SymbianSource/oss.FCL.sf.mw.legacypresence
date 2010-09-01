/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   XDM Engine log writer
*
*/



#include <f32file.h>
#include <flogger.h>
#include <e32debug.h>
#include "XdmLogWriter.h"
#include "XdmStaticUtils.h"


// ----------------------------------------------------------
// XdmStaticUtils::CheckFileExistsL
// 
// ----------------------------------------------------------
//
CXdmLogWriter::CXdmLogWriter()
    {         
    }

// ----------------------------------------------------------
// CXdmLogWriter::NewL
// 
// ----------------------------------------------------------
//      
EXPORT_C CXdmLogWriter* CXdmLogWriter::NewL( const TDesC& aLogName )
    {
    CXdmLogWriter* self = new ( ELeave ) CXdmLogWriter();
    CleanupStack::PushL( self );
    self->ConstructL( aLogName );
    CleanupStack::Pop();
    return self;        
    }

// ----------------------------------------------------------
// CXdmLogWriter::ConstructL
// 
// ----------------------------------------------------------
//
void CXdmLogWriter::ConstructL( const TDesC& aLogName )
    {
    RFs session;
    HBufC* full = NULL;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );
    HBufC* concat = HBufC::NewLC( KXdmLogRoot().Length() + 
                                  KXdmLogDir().Length()  +
                                  aLogName.Length() + 1 );
    TPtr path( concat->Des() );
    path.Copy( KXdmLogRoot ); 
    path.Append( KXdmLogDir );
    path.Append( _L( "\\") );
    path.Append( aLogName );
    TRAPD( error, full = XdmStaticUtils::GenerateFileNameL( session, path, KXdmLogFileExt ) );
    if( full && error == KErrNone )
    	{
    	CleanupStack::PushL( full );
    	TPtrC fullDes( full->Des() );
    	iLogFileName = fullDes.Mid( fullDes.LocateReverse( 92 ) + 1 ).AllocL();
    	CleanupStack::PopAndDestroy();  //full
    	}
   	else iLogFileName = NULL;
    CleanupStack::PopAndDestroy( 2 );  //concat, session 
    }

// ----------------------------------------------------------
// CXdmLogWriter::~CXdmLogWriter
// 
// ----------------------------------------------------------
//     
CXdmLogWriter::~CXdmLogWriter()
    {
    delete iLogFileName;
    }

// ----------------------------------------------------------
// CXdmLogWriter::WriteToLog
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmLogWriter::WriteToLog( const TDesC& aLogLine ) const
    {
    if( iLogFileName )
    	{
    	RFileLogger::Write( KXdmLogDir, *iLogFileName, EFileLoggingModeAppend, aLogLine ); 
    	}
	RDebug::Print( aLogLine );
    }
     
// ----------------------------------------------------------
// CXdmLogWriter::WriteToLog
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmLogWriter::WriteToLog( const TDesC8& aLogLine ) const
    {
    if( iLogFileName )
    	{
    	RFileLogger::Write( KXdmLogDir, *iLogFileName, EFileLoggingModeAppend, aLogLine );
    	}
	RDebug::RawPrint( aLogLine );
    }
    
// End of File




