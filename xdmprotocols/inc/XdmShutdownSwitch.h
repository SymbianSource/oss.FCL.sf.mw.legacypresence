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
* Description: MXdmShutdownSwitch
*
*/




#ifndef __XDMSHUTDOWNSWITCH__
#define __XDMSHUTDOWNSWITCH__

// CLASS DECLARATION

NONSHARABLE_CLASS( MXdmShutdownSwitch )
    {
    public:

        /**
        * Pure virtual callback function to be implemented in the
        * derived class. This function is called when timer has
        * compeleted.
        * @param aStatus The integer value of iStatus of the timer class
        */
        virtual void SwitchOff() = 0;
    };

#endif      // __XDMSHUTDOWNSWITCH__
            
// End of File
