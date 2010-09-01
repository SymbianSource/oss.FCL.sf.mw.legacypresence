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

#ifndef C_CPRESENCEPLUGINCONTACTSTATE_H
#define C_CPRESENCEPLUGINCONTACTSTATE_H

#include <e32base.h>

class CPresencePluginContactStateHandler;
class MPresenceContactsContextBase;

/**
 *  Base class for presenceplugin contacts states
 * 
 *  @lib presenceplugin.dll
 *  @since S60 V5.0
 */
class CPresencepluginContactState : public CBase
    {

public:

    CPresencepluginContactState( MPresenceContactsContextBase& aContext,
        CPresencePluginContactStateHandler& aStateHandler );
    
    virtual ~CPresencepluginContactState();

    virtual void HandleL() = 0;
    
    virtual void Complete();
    
    virtual void Error( TInt aError );
    
protected: // data
    
    /**
     * Context
     * Not own.
     */
    MPresenceContactsContextBase* iContext;
    
    /**
     * State handler
     * Not own.
     */
    CPresencePluginContactStateHandler* iStateHandler;
    
    };


#endif // C_CPRESENCEPLUGINCONTACTSTATE_H
