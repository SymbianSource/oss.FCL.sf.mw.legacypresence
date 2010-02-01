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
* Description:   CXcapShutdownTimer
*
*/



#include "XcapCacheServer.h"
#include "XcapShutdownTimer.h"

// ----------------------------------------------------
// CXcapShutdownTimer::CXcapShutdownTimer
// 
// ----------------------------------------------------
//
CXcapShutdownTimer::CXcapShutdownTimer() : CTimer( -1 )
  {
  /*#ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapShutdownTimer::CXcapShutdownTimer()" ) );
    #endif*/
  CActiveScheduler::Add(this);
  }

// ----------------------------------------------------
// CXcapShutdownTimer::ConstructL
// 
// ----------------------------------------------------
//  
void CXcapShutdownTimer::ConstructL()
  {
  /*#ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapShutdownTimer::ConstructL()" ) );
    #endif*/
  CTimer::ConstructL();
  }

// ----------------------------------------------------
// CXcapShutdownTimer::Start
// 
// ----------------------------------------------------
//
void CXcapShutdownTimer::Start()
  {
  /*#ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapShutdownTimer::Start()" ) );
    #endif*/
  After( KXcapCacheShutdownDelay );
  }

// ----------------------------------------------------
// CXcapShutdownTimer::RunL
// 
// ----------------------------------------------------
//  
void CXcapShutdownTimer::RunL()
  {
  /*#ifdef _DEBUG
        CXcapCacheServer::WriteToLog( _L8( "CXcapShutdownTimer::RunL() - Closing down server" ) );
    #endif*/
  CActiveScheduler::Stop();
  }


