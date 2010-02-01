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


#ifndef C_CPRESENCEPLUGINCONTACTSTATEHANDLER_H
#define C_CPRESENCEPLUGINCONTACTSTATEHANDLER_H


#include <e32base.h>

class CPresencepluginContactState;

/**
 * States handler
 *
 * @lib presenceplugin.dll
 * @since S60 v5.0
 */
class CPresencePluginContactStateHandler : public CBase
    {

public:

    static CPresencePluginContactStateHandler* NewL();

    /**
     * Destructor.
     */
    virtual ~CPresencePluginContactStateHandler();
    
    /**
     * Add states to queue
     *
     * @since S60 v5.0
     * @param aState State to add, ownership is transferred.
     */
    void AddStateL( CPresencepluginContactState* aState );
    
    /**
     * Start State machine.
     * States must be added before
     *
     * @since S60 v5.0
     * @param aStatus Client status
     */
    void Start( TRequestStatus* aStatus );

    /**
     * States are progressed to end
     * or error occured
     *
     * @since S60 v5.0
     * @param aError Possible error
     */
    void Complete( TInt aError );
    
    /**
     * State getter
     *
     * @since S60 v5.0
     * @return State
     */
    CPresencepluginContactState* State();
    
    /**
     * Order statehandler to move next state
     *
     * @since S60 v5.0
     */
    void ProgressToNextState();
    
private:

    CPresencePluginContactStateHandler();

    void HandleNextState();

private: // data
    
    /**
     * Current state index
     * own.
     */
    TInt iCurrentStateIndex;
    
    /**
     * States to process
     * own.
     */
    RPointerArray< CPresencepluginContactState > iStateArray;
    
    /**
     * Client status
     * Not own.
     */
    TRequestStatus* iClientStatus;

    };


#endif // C_CPRESENCEPLUGINCONTACTSTATEHANDLER_H
