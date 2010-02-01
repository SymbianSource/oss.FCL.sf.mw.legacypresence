/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MXcapHttpRequestTimerCallback
*
*/




#ifndef __XCAPHTTPREQUESTTIMERCALLBACK__
#define __XCAPHTTPREQUESTTIMERCALLBACK__

// CLASS DECLARATION

/**
*  A callback class for the timer class of IMPS Transport layer
*/
NONSHARABLE_CLASS( MXcapHttpRequestTimerCallback )
    {
    public:

        /**
        * Pure virtual callback function to be implemented in the
        * derived class. This function is called when timer has
        * compeleted.
        * @param aStatus The integer value of iStatus of the timer class
        */
        virtual void HandleTimerEventL() = 0;
    };

#endif      //__XCAPHTTPREQUESTTIMERCALLBACK__
            
// End of File
