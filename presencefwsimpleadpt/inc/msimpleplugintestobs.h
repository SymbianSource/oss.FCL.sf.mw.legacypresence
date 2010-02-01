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
* Description:    Presence FW SIMPLE plugin test suite
*
*/




#ifndef MSIMPLEPLUGINTESTOBS_H
#define MSIMPLEPLUGINTESTOBS_H

#include <e32std.h>


/**
 * MSimplePluginTestObs
 *
 *  SIMPLE plugin tester observer for active object testing
 *
 *  @lib simpleplugin
 *  @since s60 v5.0
 */

class MSimplePluginTestObs
    {

public:

    /**
     * Test callback in the begin of RunL or callback
     */
    virtual void TestCallbackInRunL( 
        TInt aStatus, TInt aState, TInt aAuxInt, const TDesC& aAuxStr) = 0;        
        
    /**
     * Test callback before new asynch request
     */ 
    virtual void TestCallbackAsynchL( 
        TInt aStatus, TInt aState, TInt aAuxInt, const TDesC& aAuxStr) = 0;
                   

    };

#endif

// End of File