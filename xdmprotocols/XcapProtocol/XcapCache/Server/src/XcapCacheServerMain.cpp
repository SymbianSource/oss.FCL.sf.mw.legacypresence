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
* Description:   XcapCacheServerMain.cpp
*
*/



#include "XcapCacheServer.h"

// ----------------------------------------------------------
// DeleteLogFiles
// 
// ----------------------------------------------------------
//
TInt DeleteLogFileL()
    {
    RFs session;
    TInt error = session.Connect();
    if( error == KErrNone )
        {
        CFileMan* manager = CFileMan::NewL( session );
        error = manager->Delete( _L( "C:\\logs\\XDM\\CacheServer.txt" ) );
        session.Close();
        delete manager;
        manager = NULL;
        }
    return error;
    }

// ----------------------------------------------------------
// E32Main
// 
// ----------------------------------------------------------
//     
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
    TInt error = KErrNoMemory;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    #ifdef _DEBUG
        TRAPD( logErr, DeleteLogFileL() );
        TBuf8<KMaxFileName> exe;
        TFileName name( RProcess().FileName() );
        exe.Copy( name );
        CXcapCacheServer::WriteToLog( _L8( "*** E32Main(): Exe loaded from %S - Log deletion: %d"),
                                      &exe, logErr );
    #endif
	if( cleanup )
		{
		TRAP( error, CXcapCacheServer::StartThread() );
		delete cleanup;
		}
	__UHEAP_MARKEND; 
    #ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "*** E32Main() completes: %d" ), error );
    #endif  
	return error;
	}

//  End of File  

