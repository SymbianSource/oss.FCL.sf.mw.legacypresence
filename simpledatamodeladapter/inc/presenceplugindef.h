/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PRESENCEPLUGINDEF_H
#define PRESENCEPLUGINDEF_H

#include <e32std.h>


/**
 * C++ compatible definitions for XIMP PRESENCE protocol.
 *
 * This must be kept in-sync with definitions in
 * "simpleplugin_resource.hrh"
 */

//XIMP primary test protocol
const TInt K_PRESENCEPLUGIN_1_VERSION_NO          = 1;
_LIT( K_PRESENCEPLUGIN_1_DISPLAY_NAME,            "XIMP PRESENCE protocol" );
_LIT( K_PRESENCEPLUGIN_1_DEFAULT_DATA,            "PRESENCE" );
_LIT( K_PRESENCEPLUGIN_1_OPAQUE_DATA ,             "<p>Nokia<f>feat/pres/pub<f>feat/pres/fetch<f>feat/pres/subs<s>sip" );


#endif // PRESENCEPLUGINDEF_H