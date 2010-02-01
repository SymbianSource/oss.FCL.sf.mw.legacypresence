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
* Description:   Connection manager timer
*
*/




// INCLUDES
#include "msgconntimer.h"
#include "msgconnmanager.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgConnTimer::CMsgConnTimer() : CActive( EPriorityStandard ),
                                 iCompleteIfCancel( EFalse )
                                          
    {
    }

// ---------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgConnTimer::CMsgConnTimer( MMsgConnTimerCallback* aClient ) :
                              CActive( EPriorityStandard ),
                              iClient( aClient )
                                               
                                          
    {
    }

// ---------------------------------------------------------
// Symbian OS default constructor may leave.
//
// ---------------------------------------------------------
//
void CMsgConnTimer::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// Two-phased constructor.
//
// ---------------------------------------------------------
//
CMsgConnTimer* CMsgConnTimer::NewL()
    {
    CMsgConnTimer* self = new ( ELeave ) CMsgConnTimer();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// Two-phased constructor.
//
// ---------------------------------------------------------
//
CMsgConnTimer* CMsgConnTimer::NewL( MMsgConnTimerCallback* aClient )
    {
    CMsgConnTimer* self = new ( ELeave ) CMsgConnTimer( aClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// Destructor
//
// ---------------------------------------------------------
//
CMsgConnTimer::~CMsgConnTimer()
    {
    #ifdef _DEBUG
        CMsgConnManager::WriteToLog( _L8( "CMsgConnTimer::~CMsgConnTimer(), destructor called." ) );  
    #endif
    Cancel();
    iTimer.Close();
    }

// ---------------------------------------------------------
// CMsgConnTimer::ActivateTimer
//
// ---------------------------------------------------------
//
void CMsgConnTimer::ActivateTimer( TRequestStatus& aStatus,
                                   TTimeIntervalMicroSeconds32 aDelay )
    {
    #ifdef _DEBUG
        CMsgConnManager::WriteToLog( _L8( "CMsgConnTimer::ActivateTimer()." ) );
    #endif
    __ASSERT_ALWAYS( !IsActive(), User::Panic( _L("ConnectionTimer"), 1 ) );
    iClient = NULL;
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iTimer.After( iStatus, aDelay );
    #ifdef _DEBUG
        CMsgConnManager::WriteToLog( _L8( "  Timer activated." ) );
    #endif
    SetActive();
    }

// ---------------------------------------------------------
// CMsgConnTimer::ActivateTimer
//
// ---------------------------------------------------------
//
void CMsgConnTimer::ActivateTimer( MMsgConnTimerCallback* aClient,
                                   TTimeIntervalMicroSeconds32 aDelay )
    {
    #ifdef _DEBUG
        CMsgConnManager::WriteToLog( _L8( "CMsgConnTimer::ActivateTimer()" ) );
    #endif
    __ASSERT_ALWAYS( !IsActive(), User::Panic( _L("ConnectionTimer"), 1 ) );
    iClient = aClient;
    iClientStatus = NULL;
    iTimer.After( iStatus, aDelay );
    #ifdef _DEBUG
        CMsgConnManager::WriteToLog( _L8( "  Timer activated (with client)" ) );
    #endif
    SetActive();
    }

// ---------------------------------------------------------
// CMsgConnTimer::RunL
//
// ---------------------------------------------------------
//
void CMsgConnTimer::RunL()
    {
    #ifdef _DEBUG
        CMsgConnManager::WriteToLog( _L8( "CMsgConnTimer::RunL()." ) );
    #endif
    if( iClient != NULL )
        {
        #ifdef _DEBUG
            CMsgConnManager::WriteToLog( _L8( "  iClient != NULL, Callback interface in use." ) );
        #endif
        iClient->HandleTimerEvent( iStatus.Int() );
        iClient = NULL;
        }
    else
        {
        #ifdef _DEBUG
            CMsgConnManager::WriteToLog( _L8( "  iClient == NULL, completing request." ) );
        #endif
        User::RequestComplete( iClientStatus, iStatus.Int() );
        }
    }

// ---------------------------------------------------------
// CMsgConnTimer::DoCancel
//
// ---------------------------------------------------------
//
void CMsgConnTimer::DoCancel()
    {
    #ifdef _DEBUG
        CMsgConnManager::WriteToLog( _L8( "CMsgConnTimer::DoCancel()" ) );
    #endif
    iTimer.Cancel();
    if( iClientStatus != NULL )
        User::RequestComplete( iClientStatus, KErrCancel );
    #ifdef _DEBUG
        CMsgConnManager::WriteToLog( _L8( "  DoCancel() ends" ) );
    #endif
    }

//  End of File  

