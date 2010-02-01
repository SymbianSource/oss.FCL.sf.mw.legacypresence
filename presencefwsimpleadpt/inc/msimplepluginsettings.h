/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Presence FW SIMPLE plugin engine settings
*
*/




#ifndef MSIMPLEPLUGINCONNECTIONSETTINGS_H
#define MSIMPLEPLUGINCONNECTIONSETTINGS_H

#include <e32std.h>

class CSimplePluginVariation;

/**
 *  MSimplePluginConnectionObs
 *
 *  SIMPLE engine connection
 *
 *  @lib simpleengine
 *  @since s60 v5.0
 */

class MSimplePluginSettings
    {

public:

    /**
     * Current registered SIP entity
     */
    virtual TPtrC16 CurrentSipPresentity() = 0;

    /**
     * Current registered SIP entity
     */
    virtual TPtrC8 CurrentSipPresentity8() = 0;

    /**
     * Current domain syntax accessor
     */
    virtual TPtrC16 CurrentDomain() = 0;
    
    /**
     * Variation
     */
    virtual CSimplePluginVariation& Variation() = 0; 

    };

#endif

// End of File