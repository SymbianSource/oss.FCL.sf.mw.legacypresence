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


#ifndef M_MPRESENCEPLUGINCONTACTSOBS_H
#define M_MPRESENCEPLUGINCONTACTSOBS_H


#include <e32std.h>
#include "presenceplugincontacts.h"


/**
 *  Callback from virtual phonebook contacts handler
 *
 *  @lib presenceplugin.dll
 *  @since S60 v5.0
 */
class MPresencePluginContactsObs
    {

public:

    /**
     * Informs the observer that the operation is completed
     *
     * @param aResult Result data returned,ownership is
     *                transferred to the client.
     *                In operation EOperationIsPresenceStoredToContacts
     *                return type is TBool*.
     * @param aOperation Operation type
     * @param aError Completion error code
     */
    virtual void RequestComplete( TAny* aResult,
        TPresenceContactsOperation aOperation, TInt aError ) = 0;
    
    };


#endif // M_PRESENCEPLUGINCONTACTSOBS_H

// End of File
