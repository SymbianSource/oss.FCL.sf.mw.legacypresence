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


#ifndef CPRESENCEPLUGINWATCHERINFO_H
#define CPRESENCEPLUGINWATCHERINFO_H

#include <e32std.h>
#include <ximpbase.h>

#include "presencelogger.h"

/**
 * CPresencePluginWatcherInfo
 *
 * watcher info.
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CPresencePluginWatcherInfo ): public CBase
    {
    public:

        /**
         * Constructor.
         * @param aId watcher id
         * @param aSipId watcher SIP identity
         */
        static CPresencePluginWatcherInfo* NewL(
            const TDesC8& aId,
            const TDesC& aSipId );

        /**
         * Standard C++ destructor
         */
        virtual ~CPresencePluginWatcherInfo();
                
        /**
         * Destructor
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void Destroy();
        
        /** 
         * Match to given info
         *
         * @since S60 3.2
         * @param aId watcher id
         * @param aSipId watcher SIP identity     
         * @return ETrue if current entity matches to the parameters
         */
        TBool Match( const TDesC8& aId,
            const TDesC& aSipId ); 
        /**
         * SipId
         *
         * @since S60 3.2
         * @param none
         * @return TPtrC, sip Uri
         */   
        TPtrC SipId();     
        
        static TInt LinkOffset();   
        
    private:

        CPresencePluginWatcherInfo( );

        /** 
         * Constructor
         *
         * @since S60 3.2
         * @param aId watcher id
         * @param aSipId watcher SIP identity     
         */
        void ConstructL( 
            const TDesC8& aId,
            const TDesC& aSipId ); 
       
    public: // data        

        /**
         * Linked list
         * own
         */
        TDblQueLink iLink;
        
    private: // data
        
        /**
         * ident id
         * own
         */
        HBufC8* iId;
        
        /**
         * sip id
         * own
         */
        HBufC* iSipId;

        SIMPLE_UNIT_TEST( T_SIMPLEDataModelAdapter )

    };
    
#endif // CPRESENCEPLUGINWATCHERINFO_H
