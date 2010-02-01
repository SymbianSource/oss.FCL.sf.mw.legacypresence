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


#include "presenceplugincontactstateend.h"
#include "presenceplugincontactstatehandler.h"
#include "presencecontactscontextbase.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginContactStateEnd::CPresencePluginContactStateEnd
// ---------------------------------------------------------------------------
//
CPresencePluginContactStateEnd::CPresencePluginContactStateEnd(
    MPresenceContactsContextBase& aContext,
    CPresencePluginContactStateHandler& aStateHandler )
    : CPresencepluginContactState( aContext, aStateHandler )
    {
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateEnd::~CPresencePluginContactStateEnd
// ---------------------------------------------------------------------------
//
CPresencePluginContactStateEnd::~CPresencePluginContactStateEnd()
    {
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateEnd::HandleL
// ---------------------------------------------------------------------------
//
void CPresencePluginContactStateEnd::HandleL()
    {
    iStateHandler->Complete( KErrNone );
    }
