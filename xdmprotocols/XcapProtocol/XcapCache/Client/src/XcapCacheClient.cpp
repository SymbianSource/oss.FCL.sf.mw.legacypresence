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
* Description:   CXcapCacheClient
*
*/



#include <e32std.h>
#include "XcapCache.h"
#include "CommonDefines.h"
#include "XcapCacheClient.h"

//CLASS DECLARATION
CXcapCacheClient::CXcapCacheClient( RXcapCache& aCacheMain ) :
                                    CActive( EPriorityStandard ),
                                    iClientMain( aCacheMain )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------
// CXcapCacheClient::Store
// 
// ----------------------------------------------------
//
void CXcapCacheClient::Reset()
    {
    #ifdef _DEBUG
        RXcapCache::WriteToLog( _L8( "CXcapCacheClient::Reset()" ) );
    #endif
    }

// ----------------------------------------------------
// CXcapCacheClient::SendMessageL
// 
// ----------------------------------------------------
//
void CXcapCacheClient::ReceiveAsync( TRequestStatus& aStatus )   
    {
    iClientStatus = &aStatus;
    aStatus = KRequestPending;
    SetActive();
    }

// ----------------------------------------------------
// CXcapCacheClient::RunL
// 
// ----------------------------------------------------
//
void CXcapCacheClient::RunL()
    {
    
    }

// ----------------------------------------------------
// CXcapCacheClient::DoCancel
// 
// ----------------------------------------------------
//
void CXcapCacheClient::DoCancel()
    {
    
    }

// End of File


