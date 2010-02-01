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


#include "presenceplugincontactstate.h"
#include "PresencePluginContactStateHandler.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencepluginContactState::CPresencepluginContactState
// ---------------------------------------------------------------------------
//
CPresencepluginContactState::CPresencepluginContactState(
    MPresenceContactsContextBase& aContext,
    CPresencePluginContactStateHandler& aStateHandler )
    : iContext( &aContext ), iStateHandler( &aStateHandler )
    {
    }


// ---------------------------------------------------------------------------
// CPresencepluginContactState::~CPresencepluginContactState
// ---------------------------------------------------------------------------
//
CPresencepluginContactState::~CPresencepluginContactState()
    {
    }


// ---------------------------------------------------------------------------
// CPresencepluginContactState::Complete
// ---------------------------------------------------------------------------
//
void CPresencepluginContactState::Complete()
    {
    iStateHandler->ProgressToNextState();
    }


// ---------------------------------------------------------------------------
// CPresencepluginContactState::Complete
// ---------------------------------------------------------------------------
//
void CPresencepluginContactState::Error( TInt aError )
    {
    iStateHandler->Complete( aError );
    }
