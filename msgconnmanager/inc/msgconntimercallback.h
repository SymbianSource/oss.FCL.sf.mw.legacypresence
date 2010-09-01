/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Connection manager timer callback
*
*/




#ifndef __MSGCONNTIMERCALLBACK__
#define __MSGCONNTIMERCALLBACK__

#include <e32base.h>

// CLASS DECLARATION

class MMsgConnTimerCallback
    {
    public:

        /**
        * Pure virtual callback function to be implemented in the
        * derived class. This function is called when timer has
        * compeleted.
        * @param aStatus The integer value of iStatus of the timer class
        * @return void
        */
        virtual void HandleTimerEvent( TInt aStatus ) = 0;
    };

#endif      //__MSGCONNTIMERCALLBACK__
            
// End of File
