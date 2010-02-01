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


#ifndef CPRESENCEPLUGINENTITYWATCHER_H
#define CPRESENCEPLUGINENTITYWATCHER_H

#include <e32base.h>
#include <ximpbase.h>
#include <protocolpresencewatching.h>
#include <msimplewatcherobserver.h>
#include "presenceplugincommon.h"
#include "mpresencepluginconnectionobs.h"
#include "presencelogger.h"

class TXIMPRequestId;
class MSimpleDocument;
class MSimpleWatcher;
class CPresencePluginWatcher;
class MSimpleConnection;
class CPresencePluginData;
 
/**
 * CPresencePluginEntityWatcher
 *
 * Simple Engine Connection
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CPresencePluginEntityWatcher ): public CBase,
    public MSimpleWatcherObserver
    {
    private:
        
    /**
    * Current operation
    */
	enum TPluginEntityWatcherOperation
        {
        EPluginIdle,
        EPluginStop,
        EPluginStart,
        EPluginModify,
        EPluginStartNotifyClient
        };
            
    public:
       
        /**
         * Constructor.
         * @param aObs callback for complete requests
         * @param aConn Simple Engine connection
         * @param aHost XIMP connection host 
         */ 
        static CPresencePluginEntityWatcher* NewL(
            MPresencePluginConnectionObs& aObs,
            MSimpleConnection& aConn,
       		CPresencePluginWatcher& aWatcher,
        	CPresencePluginData* aPresenceData );
        	
        /**
         * Standard C++ destructor
         */
        virtual ~CPresencePluginEntityWatcher();
      
        /**
         * Access Request id
         *
         * @since S60 3.2 
         * @return XIMP request id
         */
        TXIMPRequestId XIMPRequestId();
        
        /**
         * Access Presentity id
         *
         * @since S60 3.2 
         * @return Presentity id
         */    
         TPtrC8 PresentityId(); 
        
        /**
         * Start subscribe
         *
         * @since S60 3.2 
         * @param aPresentityId presentity id
         */
        void StartSubscribeL( const TDesC8& aPresentityId );  
        
        /**
         * Start subscribe
         *
         * @since S60 5.0
         * @param aPresentityId presentity id
         * @param aClientRequst client status to complete
         */        
        void StartSubscribeL(
            const TDesC8& aPresentityId, 
            TRequestStatus& aClientRequst );
        
        /**
         * Stop Subscrib
         *
         * @since S60 3.2 
		 * @param none
         * @return none         
         */
        void StopSubscribeL();
        
        /**
         * Stop subscribe
         *
         * @since S60 5.0
         * @param aClientRequst client status to complete
         */                
        void StopSubscribeL( 
            TRequestStatus& aClientRequst );
        
        /**
         * Set active status to XIMP
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void SetActiveToXIMPL( const TDesC& aIdentity );
        
        /**
         * Set terminated status to XIMP
         *
         * @since S60 3.2
         * @param aIdentity identity
         * @return none
         */
        void SetTerminatedToXIMPL( const TDesC& aIdentity );
        
    private:
    
        /**
         * Standard C++ constructor
         * 
         * @param aObs, connection observer
         * @param aConn, SIMPLE connection
         * @param aWatcher, plugin watcher
         */ 
        CPresencePluginEntityWatcher(
            MPresencePluginConnectionObs& aObs,
            MSimpleConnection& aConn,
            CPresencePluginWatcher& aWatcher );

        /**
         * Performs the 2nd phase of construction.
         *
         * @since S60 3.2 
         * @param none
         * @param none
         */ 
        void ConstructL( CPresencePluginData* aPresenceData );

    public: // from base class MSimpleWatcherObserver

        /**
         * Defined in a base class
         */
        void WatcherReqCompleteL( TInt aOpId, TInt aStatus );

        /**
         * Defined in a base class
         */
        void WatcherNotificationL( MSimpleDocument& aDocument );

        /**
         * Defined in a base class
         */
        void WatcherListNotificationL( MSimplePresenceList& aList );

        /**
         * Defined in a base class
         */
        void WatcherTerminatedL(
            TInt aOpId, TInt aReason );
                
    private:
        
        /**
         * Set active status to XIMP
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void SetActiveToXIMPL( MSimpleDocument& aDocument );
        
        /**
         * Set terminated status to XIMP
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void SetTerminatedToXIMPL( );
        
        /**
         * Set pending status to XIMP
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void SetPendingToXIMPL( );

        /**
         * Completes client request
         *
         * @since S60 5.0
         * @param aStatus, complete code
         * @return none
         */
        void CompleteClientReq( TInt aStatus );
           
    private: // Data

        /**
         * XIMP Plugin connection observer
         */
        MPresencePluginConnectionObs& iConnObs;
        
        /**
         * SIMPLE engine connection
         * Not Own.
         */
        MSimpleConnection& iConnection;
        
        /**
         * SIMPLE engine watcher
         * Own.
         */
        MSimpleWatcher* iWatcher;    
        
        /**
         * PrFW request id
         * Own.
         */
        TXIMPRequestId iXIMPId;
        
        /**
         * Subscribed entity Id.
         * Own.
         */
        HBufC8* iEntityId; 
             
        /**
         * Simple Engine request id
         * Own.
         */
        TInt iSimpleId;
                          
        /**
         * host class
         * Own.
         */
        CPresencePluginWatcher& iPluginWatcher;
        
        /**
         * current pending operation
         * Own.
         */
        TPluginEntityWatcherOperation iOperation;
        
        /**
         * instance to Presence data
         * Own.
         */
        CPresencePluginData* iPresenceData;

        /**
         * Client status.
         * Not own.
         */
        TRequestStatus* iClientStatus;                

        SIMPLE_UNIT_TEST( T_CPresencePluginEntityWatcher )
        SIMPLE_UNIT_TEST( T_CPresencePluginWatcher )        
    };

#endif // CPRESENCEPLUGINENTITYWATCHER_H
