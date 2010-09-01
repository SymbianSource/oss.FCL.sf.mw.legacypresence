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
* Description:  CXdmShutdownTimer
*
*/



#include <e32base.h>
#include "XcapProtocol.h"
#include "XdmShutdownTimer.h"
#include "XdmShutdownSwitch.h"

// ----------------------------------------------------
// CXdmShutdownTimer::CXdmShutdownTimer
// 
// ----------------------------------------------------
//
CXdmShutdownTimer::CXdmShutdownTimer( CXcapProtocol& aXcapProtocol,
                                      MXdmShutdownSwitch* aShutdownSwitch ) :
                                      CTimer( -1 ),
                                      iXcapProtocol( aXcapProtocol ),
                                      iShutdownSwitch( aShutdownSwitch )
	{
	}

// ----------------------------------------------------
// CXdmShutdownTimer::ConstructL
// 
// ----------------------------------------------------
//	
void CXdmShutdownTimer::ConstructL()
	{
	#ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXdmShutdownTimer::ConstructL()" ) );
    #endif
	CTimer::ConstructL();
	CActiveScheduler::Add( this );
	}

// ----------------------------------------------------
// CXdmShutdownTimer::Start
// 
// ----------------------------------------------------
//
void CXdmShutdownTimer::Start( const TTimeIntervalMicroSeconds32 aShutdownDelay )
	{
	#ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXdmShutdownTimer::Start()" ) );
    #endif
	After( aShutdownDelay );
	}

// ----------------------------------------------------
// CXdmShutdownTimer::RunL
// 
// ----------------------------------------------------
//	
void CXdmShutdownTimer::RunL()
	{
	#ifdef _DEBUG
        iXcapProtocol.WriteToLog( _L8( "CXdmShutdownTimer::RunL() - Close transfer media" ) );
    #endif
	iShutdownSwitch->SwitchOff();
	}


