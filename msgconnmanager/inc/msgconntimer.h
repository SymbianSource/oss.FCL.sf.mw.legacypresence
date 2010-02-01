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
* Description:   Connection manager timer
*
*/




#ifndef __MSGCONNTIMER__
#define __MSGCONNTIMER__

#include <e32base.h>

class MMsgConnTimerCallback;

// CLASS DECLARATION
class CMsgConnTimer : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
		* @return CMsgConnTimer* 
        */
        static CMsgConnTimer* NewL();

        /**
        * Two-phased constructor.
		* @param MMsgConnectionTimerCallback* aClient
        * @return CMsgConnTimer*
        */
        static CMsgConnTimer* NewL( MMsgConnTimerCallback* aClient );

        /**
        * Destructor.
        */
        virtual ~CMsgConnTimer();

    public: // New functions
        
        /**
        * Request to activate timer
        * @param TRequestStatus& aStatus
        * @param TTimeIntervalMicroSeconds32 aDelay
		* @param TBool aCompleteIfCancel = EFalse
        * @return void
        */
        void ActivateTimer( TRequestStatus& aStatus,
                            TTimeIntervalMicroSeconds32 aDelay );

        /**
        * Request to activate timer
        * @param TTimeIntervalMicroSeconds32 aDelay
		* @param MMsgConnectionTimerCallback* aClient
        * @return void
        */
        void ActivateTimer( MMsgConnTimerCallback* aClient,
                            TTimeIntervalMicroSeconds32 aDelay );
        
    private:
        
        /**
        * From CActive.
        * @return void
        */
		void RunL();

        /**
        * From CActive.
        * @return void
        */
		void DoCancel();

        /**
        * By default Symbian OS constructor is private.
        * @return void
        */
        void ConstructL();

        /**
        * C++ constructor.
        */
		CMsgConnTimer();

        /**
        * C++ constructor.
		* @param MMsgConnectionTimerCallback* aClient
        */
		CMsgConnTimer( MMsgConnTimerCallback* aClient );

    private:    // Data
        
		RTimer							        iTimer;
		TRequestStatus*	                        iClientStatus;
        TBool                                   iCompleteIfCancel;
        MMsgConnTimerCallback*                  iClient;
    };

#endif      //MSGCONNTIMER_H
            
// End of File
