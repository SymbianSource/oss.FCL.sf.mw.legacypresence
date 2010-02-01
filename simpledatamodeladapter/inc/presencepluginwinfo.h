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


#ifndef CPRESENCEPLUGINWINFO_H
#define CPRESENCEPLUGINWINFO_H

#include <e32base.h>
#include <ximpbase.h>
#include <msimplewinfoobserver.h>

#include "mpresencepluginconnectionobs.h"
#include "presencelogger.h"

class MSimpleWinfo;
class TximpRequestId;
class MSimpleWinfoWatcher;
class MSimpleConnection;
class MximpProtocolConnectionHost;
class MPresencePluginConnectionObs;

/**
 * CPresencePluginWinfo
 *
 * Simple Engine Connection
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CPresencePluginWinfo ): public CBase,
    public MSimpleWinfoObserver
    {
    public:

        /**
         * Constructor.
         * @param aObs callback for complete requests
         * @param aConn Simple Engine connection
         */ 
        static CPresencePluginWinfo* NewL(
            MPresencePluginConnectionObs& aObs,    
            MSimpleConnection& aConn );
            
        /**
         * Standard C++ destructor
         */
        virtual ~CPresencePluginWinfo();

        /**
         * SetHost
         * Set ximp Host
         *
         * @since S60 3.2         
         * @param aHost ximp Host
         */
        void SetHost( MXIMPProtocolConnectionHost* aHost );
        
        /**
         * SubscribeWinfoListL
         *
         * @since S60 3.2         
         * @param aReqId ximp req id
         */
        void SubscribeWinfoListL( TXIMPRequestId aReqId ); 
        
        /**
         * UnsubscribeWinfoListL
         *
         * @since S60 3.2
         * @param aReqId ximp req id
         */
        void UnsubscribeWinfoListL( TXIMPRequestId aReqId );           
       
    private:
    
        /**
         * Standard C++ constructor
         *
         * @param aObs, plugin connection observer
         * @param aConn, SIMPLE connection 
         */  
        CPresencePluginWinfo(
            MPresencePluginConnectionObs& aObs,    
            MSimpleConnection& aConn );
            
        /**
         * Performs the 2nd phase of construction.
         */ 
        void ConstructL( );

    public: // from base class MSimpleWinfoObserver

        // Subscribe presence grant request list

        /**
         * Defined in a base class
         */
        void WinfoReqCompleteL( TInt aOpid, TInt aStatus );

        /**
         * Defined in a base class
         */
        void WinfoTerminatedL(
            TInt aOpid, TInt aReason );

        /**
         * Defined in a base class
         */
        void WinfoNotificationL( MSimpleWinfo& aWinfo );

    private: // Data

        /**
         * ximp Plugin connection observer
         * Own.
         */
        MPresencePluginConnectionObs& iConnObs;
        
        /**
         * ximp Host.     
         * Not Own.
         */    
        MXIMPProtocolConnectionHost* iHost;

        /**
         * SIMPLE engine connection
         */
        MSimpleConnection& iConnection;

        /**
         * SIMPLE engine winfo watcher.
         * Own.
         */
        MSimpleWinfoWatcher* iWinfoWatcher;
        
        /**
         * ximp request id
         * Own.
         */
        TXIMPRequestId iXIMPId;
        
        /**
         * Simple Engine request id
         * Own.
         */
        TInt iSimpleId;
        
        /**
         * Subscribed
         * Own.
         */
        TBool iSubscribed;  
        
        /**
         * Winfo subscribe completed
         * Own.
         */
        TBool iWinfoCompleted;  
        
        SIMPLE_UNIT_TEST( T_CPresencePluginAuthorization )
        SIMPLE_UNIT_TEST( T_CPresencePluginWinfo )
        SIMPLE_UNIT_TEST( CSimpleDataModelAdapterTestObserver )        
    };

#endif // CPRESENCEPLUGINWINFO_H
