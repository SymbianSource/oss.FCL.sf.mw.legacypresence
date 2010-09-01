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
* Description:    C++ compatible definitions for PrFw SIMPLE protocol
*
*/



#ifndef SIMPLEPLUGINDEF_H
#define SIMPLEPLUGINDEF_H

#include <e32std.h>
#include "prfwsimpleplugin.hrh" 


/**
 * C++ compatible definitions for PrFw SIMPLE protocol.
 *
 * This must be kept in-sync with definitions in
 * "simpleplugin_resource.hrh"
 */

//PrFw primary test protocol
const TInt K_SIMPLEPLUGIN_1_IMPLEMENTATION_UID  = SIMPLEPLUGIN_1_IMPLEMENTATION_UID;
const TInt K_SIMPLEPLUGIN_1_VERSION_NO          = 1;
_LIT( K_SIMPLEPLUGIN_1_DISPLAY_NAME,            "PrFw SIMPLE protocol" );
_LIT( K_SIMPLEPLUGIN_1_DEFAULT_DATA,            "SIMPLE" );
_LIT( K_SIMPLEPLUGIN_1_OPAQUE_DATA ,            " " );

#endif // SIMPLEPLUGINDEF_H

