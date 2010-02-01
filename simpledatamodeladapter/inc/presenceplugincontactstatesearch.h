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


#ifndef C_CPRESENCEPLUGINCONTACTSTATESEARCH_H
#define C_CPRESENCEPLUGINCONTACTSTATESEARCH_H


#include "presenceplugincontactstate.h"

/**
 *  CPresencePluginContactStateSearch
 * 
 */
class CPresencePluginContactStateSearch :
    public CPresencepluginContactState
    {

public:

    /**
     * Constructor
     */
    CPresencePluginContactStateSearch( MPresenceContactsContextBase& aContext,
            CPresencePluginContactStateHandler& aStateHandler );

    virtual ~CPresencePluginContactStateSearch();

    void HandleL();

    };


#endif // C_CPRESENCEPLUGINCONTACTSTATESEARCH_H
