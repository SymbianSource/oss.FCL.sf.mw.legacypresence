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
* Description:   CXcapHttpRequestTimer
*
*/




// INCLUDES
#include "XcapHttpTransport.h"
#include "XcapHttpRequestTimer.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CXcapHttpRequestTimer::CXcapHttpRequestTimer( CXcapHttpTransport& aTransportMain,
                                              MXcapHttpRequestTimerCallback* aCallback ) :
                                              CActive( 0 ),
                                              iTransportMain( aTransportMain ),
                                              iCallback( aCallback )
                                              
                                          
    {
    }

// ---------------------------------------------------------
// CXcapHttpRequestTimer::ConstructL()
//
// ---------------------------------------------------------
//
void CXcapHttpRequestTimer::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CXcapHttpRequestTimer::NewL
//
// ---------------------------------------------------------
//
CXcapHttpRequestTimer* CXcapHttpRequestTimer::NewL( CXcapHttpTransport& aTransportMain,
                                                    MXcapHttpRequestTimerCallback* aCallback )
    {
    CXcapHttpRequestTimer* self = new ( ELeave ) CXcapHttpRequestTimer( aTransportMain, aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CXcapHttpRequestTimer::~CXcapHttpRequestTimer()
//
// ---------------------------------------------------------
//
CXcapHttpRequestTimer::~CXcapHttpRequestTimer()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequestTimer::~CXcapHttpRequestTimer()" ) );  
    #endif
    Cancel();
    iTimer.Close();
    }


// ---------------------------------------------------------
// CXcapHttpRequestTimer::ActivateTimer
//
// ---------------------------------------------------------
//
void CXcapHttpRequestTimer::ActivateTimer( TTimeIntervalMicroSeconds32 aDelay )
    {
    __ASSERT_ALWAYS( !IsActive(), User::Panic( _L( "CXcapHttpRequestTimer" ), 1 ) );
    iTimer.After( iStatus, aDelay );
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequestTimer activated." ) );
    #endif
    SetActive();
    }

// ---------------------------------------------------------
// CXcapHttpRequestTimer::RunL
//
// ---------------------------------------------------------
//
void CXcapHttpRequestTimer::RunL()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequestTimer::RunL()." ) );
    #endif
    iCallback->HandleTimerEventL();
    }

// ---------------------------------------------------------
// CXcapHttpRequestTimer::DoCancel
//
// ---------------------------------------------------------
//
void CXcapHttpRequestTimer::DoCancel()
    {
    iTimer.Cancel();
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequestTimer cancelled." ) );
    #endif
    }

//  End of File  

