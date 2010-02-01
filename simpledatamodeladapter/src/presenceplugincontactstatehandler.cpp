/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IETF SIMPLE Protocol implementation for XIMP Framework
*
*/


#include "presenceplugincontactstatehandler.h"
#include "presenceplugincontactstate.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginContactStateHandler::CPresencePluginContactStateHandler
// ---------------------------------------------------------------------------
//
CPresencePluginContactStateHandler::CPresencePluginContactStateHandler()
    : iCurrentStateIndex( 0 )
    {
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateHandler::~CPresencePluginContactStateHandler
// ---------------------------------------------------------------------------
//
CPresencePluginContactStateHandler::~CPresencePluginContactStateHandler()
    {
    iStateArray.ResetAndDestroy();
    iClientStatus = NULL;
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateHandler::NewL
// ---------------------------------------------------------------------------
//
CPresencePluginContactStateHandler* CPresencePluginContactStateHandler::NewL()
    {
    CPresencePluginContactStateHandler* self =
            new (ELeave) CPresencePluginContactStateHandler();
    return self;
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateHandler::Start
// ---------------------------------------------------------------------------
//
void CPresencePluginContactStateHandler::Start( TRequestStatus* aStatus )
    {
    iClientStatus = aStatus;
    *iClientStatus = KRequestPending;
    iCurrentStateIndex = 0;
    HandleNextState();
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateHandler::Complete
// ---------------------------------------------------------------------------
//
void CPresencePluginContactStateHandler::Complete( TInt aError )
    {
    if( iClientStatus && *iClientStatus == KRequestPending )
        {
        User::RequestComplete( iClientStatus, aError );
        }
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateHandler::ProgressToNextState
// ---------------------------------------------------------------------------
//
void CPresencePluginContactStateHandler::ProgressToNextState()
    {
    iCurrentStateIndex++;
    HandleNextState();
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateHandler::HandleNextState
// ---------------------------------------------------------------------------
//
void CPresencePluginContactStateHandler::HandleNextState()
    {
    TRAPD( error, State()->HandleL() );
    if ( KErrNone != error )
        {
        Complete( error );
        }
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateHandler::AddStateL
// ---------------------------------------------------------------------------
//
void CPresencePluginContactStateHandler::AddStateL(
    CPresencepluginContactState* aState )
    {
    CleanupStack::PushL( aState );
    iStateArray.AppendL( aState );
    CleanupStack::Pop( aState );
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateHandler::State
// ---------------------------------------------------------------------------
//
CPresencepluginContactState* CPresencePluginContactStateHandler::State()
    {
    return iStateArray[ iCurrentStateIndex ];
    }
