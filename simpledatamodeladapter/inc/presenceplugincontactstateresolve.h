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


#ifndef C_CPRESENCEPLUGINCONTACTSTATERESOLVE_H
#define C_CPRESENCEPLUGINCONTACTSTATERESOLVE_H


#include "presenceplugincontactstate.h"

/**
 *  CPresencePluginContactStateResolve
 * 
 */
class CPresencePluginContactStateResolve :
    public CPresencepluginContactState
    {

public:

    /**
     * Constructor
     */
    CPresencePluginContactStateResolve( MPresenceContactsContextBase& aContext,
            CPresencePluginContactStateHandler& aStateHandler );

    virtual ~CPresencePluginContactStateResolve();

    void HandleL();
    
    void Complete();
    
private: // Data
    
    TInt iContactLinkIndex;

    TInt iContactLinkCount;

    };


#endif // C_CPRESENCEPLUGINCONTACTSTATERESOLVE_H
