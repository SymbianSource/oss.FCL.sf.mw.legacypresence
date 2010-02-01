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
* Description: CXdmShutdownTimer
*
*/




#ifndef __XDMSHUTDOWNTIMER__
#define __XDMSHUTDOWNTIMER__

//FORWARD DECLARATION
class CXcapProtocol;
class MXdmShutdownSwitch;

NONSHARABLE_CLASS ( CXdmShutdownTimer ) : public CTimer
	{	
    public:
    
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
	    CXdmShutdownTimer( CXcapProtocol& aXcapProtocol,
	                       MXdmShutdownSwitch* aShutdownSwitch );
	
	    /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
	    void ConstructL();
	
	    /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
	    void Start( const TTimeIntervalMicroSeconds32 aShutdownDelay );
	
    private:
    
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
	    void RunL();
	    
	private:
	    
	    CXcapProtocol&                  iXcapProtocol;
		MXdmShutdownSwitch*             iShutdownSwitch;
	};
	
#endif	
