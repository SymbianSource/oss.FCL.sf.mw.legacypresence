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


#include "presenceplugincontactstateopen.h"
#include "presencecontactscontextbase.h"
#include "presenceplugincontactstatehandler.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginContactStateOpen::CPresencePluginContactStateOpen
// ---------------------------------------------------------------------------
//
CPresencePluginContactStateOpen::CPresencePluginContactStateOpen(
    MPresenceContactsContextBase& aContext,
    CPresencePluginContactStateHandler& aStateHandler )
    : CPresencepluginContactState( aContext, aStateHandler )
    {
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateOpen::~CPresencePluginContactStateOpen
// ---------------------------------------------------------------------------
//
CPresencePluginContactStateOpen::~CPresencePluginContactStateOpen()
    {
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateOpen::HandleL
// ---------------------------------------------------------------------------
//
void CPresencePluginContactStateOpen::HandleL()
    {
    iContext->OpenL();
    }
