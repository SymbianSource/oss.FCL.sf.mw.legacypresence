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
* Description:   XcapCache common defines
*
*/




#ifndef __COMMONDEFINES_H__
#define __COMMONDEFINES_H__

#include <e32base.h>

//Information about the data in the cache
class TCacheEntryInfo
    {
    public:    
        
        TTime                         iLastUpdate;
        TTime                         iLastAccess;
        TInt32                        iDataLength;
        const TDesC*                  iDocumentUri;
        const TDesC8*                 iEtag;
        const TDesC8*                 iRootUri;
        const TDesC8*                 iRespData;
    };
    
// server name
_LIT( KXcapCacheServerName,                          "!CXcapCacheServer" );
_LIT( KXcapCacheServerLib,                           "XcapCacheServer" );
_LIT( KXcapCacheServerExe,                           "XcapCache.exe");

// A version must be specifyed when creating a session with the server
const TUint KCacheServerMajorVersionNumber           = 0;
const TUint KCacheServerMinorVersionNumber           = 1;
const TUint KCacheServerBuildVersionNumber           = 1;

// Server panic codes
enum TXcapCacheServerPanic
    {
    ECacheCreateServer = 0,
    ECacheStartServer,
    ECacheFileServerConnect,
    ECacheIndexFileOpen,
    ECachePageFileOpen,
    EMainSchedulerError,
    EBadRequest
    };
    
//Opcodes used in message passing between client and server
enum TTransportServerRequest
	{
	EXcapCacheFetchInfo = 0,
	EXcapCacheFetchData,
    EXcapCacheStore,
    EXcapCacheDelete,
    EXcapCacheCheckValidity,
    EXcapCacheFlush
	};	

//Number of asynchronous requests
const TUint KTTMaxAsyncRequests                     = 4;

//Number of data transfer requests
const TUint KTTDataTransferRequests                 = 3;

//Default number of message slots per session
const TUint KTTDefaultMessageSlots	                = KTTMaxAsyncRequests + 2;

const TInt KServerLogBufferMaxSize                  = 2000;

const TInt KMaxCacheSize                            = 30000;

#endif // #ifndef __COMMONDEFINES_H__

// End of File
