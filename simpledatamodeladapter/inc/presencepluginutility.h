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


#ifndef T_PRESENCEPLUGINUTILITY_H
#define T_PRESENCEPLUGINUTILITY_H

/**
*  Static utility functions for presence adapter.
*
*  @lib presenceplugin.dll
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( TPresencePluginUtility )
    {

    public:
                              
       /**
        * Implementation for cleanup item.
        * Resets and destroys array of the MXIMPIdentity
        * entries and closes array.
        * @param anArray RPointerArray pointer.
        */
        static void ResetAndDestroyIdentities( TAny* anArray );

    };

#endif      // T_PRESENCEPLUGINUTILITY_H   
            
// End of File
