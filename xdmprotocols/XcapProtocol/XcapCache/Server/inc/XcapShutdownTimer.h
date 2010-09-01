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
* Description:   CXcapShutdownTimer
*
*/




#ifndef __XCAPSHUTDOWNTIMER__
#define __XCAPSHUTDOWNTIMER__

NONSHARABLE_CLASS( CXcapShutdownTimer ) : public CTimer
	{	
    public:
    
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
	    CXcapShutdownTimer();
	
	    /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
	    void ConstructL();
	
	    /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
	    void Start();
	
    private:
    
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
	    void RunL();
	    
	private:
	
		enum { KXcapCacheShutdownDelay = 0x200000 };	
	};
	
#endif	
