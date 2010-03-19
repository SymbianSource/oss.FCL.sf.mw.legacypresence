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


#ifndef CPRESENCEPLUGINPUBLISHER_H
#define CPRESENCEPLUGINPUBLISHER_H

#include <e32base.h>

#include <ximpbase.h>
#include <protocolpresencepublishing.h>
#include <msimplepublishobserver.h>
#include <msimplewatcherobserver.h>
#include <mpresencebuddyinfo2.h>
#include <presenceinfo.h>

#include "presencepluginentitywatcher.h"
#include "presenceplugincommon.h"
#include "mpresencepluginconnectionobs.h"
#include "msimpleetagobserver.h"
#include "presencelogger.h"

class TXIMPRequestId;
class MXIMPProtocolConnectionHost;
class MPersonPresenceInfo;
class MServicePresenceInfo;
class MDevicePresenceInfo;
class MSimplePublisher;
class MSimpleDocument;
class MSimpleElement;
class MSimpleWatcher;
class CPresencePluginXdmUtils;
class CPresencePluginData;
class CPresencePluginWatcherInfo;

/**
 * CPresencePluginPublisher
 *
 * Simple Engine Connection
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CPresencePluginPublisher ): public CActive,
    public MSimplePublishObserver,
    public MSimpleWatcherObserver,
    public MProtocolPresencePublishing,
    public MSimpleETagObserver
    {
    public:

        /**
         * Current operation
         */
        enum TPluginPublishOperation
            {
            ENoOperation,
            EPublishOwn,
            ESubscribeOwn,
            EUnsubscribeOwn,
            ESubscribeWinfo,
            EUnsubscribeWinfo,
            EPublishReq
            };     

        /**
         * NewL
         * @param aObs callback for complete requests
         * @param aConn Simple Engine connection
         */ 
        static CPresencePluginPublisher* NewL(
            MPresencePluginConnectionObs& aObs,
            MSimpleConnection& aConn,
            CPresencePluginData* aPresenceData );
            
        /**
         * Standard C++ destructor
         */
        virtual ~CPresencePluginPublisher();
        
        /**
         * WinfoTerminatedL
         * 
         * SIMPLE Winfo watcher sunscription is terminated
         * @param aReason reason code
         */
        void WinfoTerminatedL( TInt aReason );
         
        /** 
         * WINFO received from SIMPLE
         * @param aWinfo WINFO
         */
        void WinfoNotificationL( MSimpleWinfo& aWinfo );
    
    private:
    
        /**
         * Standard C++ constructor
         * @param aObs, connection observer
         * @param aConn, SIMPLE connection 
         */ 
        CPresencePluginPublisher(
            MPresencePluginConnectionObs& aObs,
            MSimpleConnection& aConn );
            
        /**
         * Performs the 2nd phase of construction.
         */ 
        void ConstructL( CPresencePluginData* aPresenceData );
    
    public: //Own public methods

        /**
         * StopPublishL()
         *
         * @since S60 3.2 
         * @param none
         * @return none
         */
        void StopPublishL( TRequestStatus& aStatus ); 

    public:  // from MSimpleETagObserver

	    /**
	     * Defined in a base class
	     */
        void NewETagL( const TDesC8& aVal );

    public: // from base class MSimplePublishObserver

        /**
         * Defined in a base class
         */
        void PublishReqCompleteL( TInt aOpid, TInt aStatus);

        /**
         * Defined in a base class
         */
        void PublishTerminatedL( TInt aOpid );
    
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
       
    public: // from base class MximpBase

        /**
         * Defined in a base class
         */
        TAny* GetInterface(
            TInt32 aInterfaceId,
            TIfGetOps aOps );

        /**
         * Defined in a base class
         */
        const TAny* GetInterface(
            TInt32 aInterfaceId,
            TIfGetOps aOps ) const;

        /**
         * Defined in a base class
         */
        TInt32 GetInterfaceId() const;
    
    public:     // from base class MximpProtocolPresencePublishing

        /**
         * Defined in a base class
         */
        void DoPublishOwnPresenceL(
            const MPresenceInfo& aOwnPresence,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoSubscribeOwnPresenceL(
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUpdateOwnPresenceSubscriptionPifL(
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUnsubscribeOwnPresenceL(
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoSubscribePresenceWatcherListL(
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUnsubscribePresenceWatcherListL(
            TXIMPRequestId aReqId );

    protected: // from base class CActive

        /**
         * Defined in a base class
         */
        void RunL();

        /**
         * Defined in a base class
         */
        TInt RunError( TInt aError );

        /**
         * Defined in a base class
         */
        void DoCancel();

    private:
          
        /**
         * Calls MximpProtocolConnectionHost callback
         *
         * @since S60 3.2          
         * @param aStatus error status     
         */
        void CompleteXIMPReq( TInt aStatus );
        
        /**
         * Complete client request
         *
         * @since S60 3.2          
         * @param aStatus error status     
         */
        void CompleteClientReq( TInt aStatus );
        
        /**
         * Initialize a document
         */
        void InitializeSimpleDocumentL( );    
        
        /**
         * Add person information to the document.
         *
         * @since S60 3.2         
         * @param aPersInfo personal information
         */
        void AddSimpleDocumentPersL( 
            const MPersonPresenceInfo* aInfo );
                       
        /**
         * Make a publish request
         *
         * @since S60 3.2
         * @param none
         * @return none 
         */    
        void MakePublishReqL();            
        
        /**
         * Start XDM initialization
         *
         * @since S60 3.2
         * @param aReqId, XIMP request id           
         */
        void StartXdmOperationL( TXIMPRequestId aReqId );
        
        /**
         * Delete watcher information
         *
         * @since S60 3.2
         * @param none
         * @return none         
         */
        void DeleteWatchers();  
        
        /**
         * Add Watcher if not exist
         *
         * @since S60 3.2
         * @param aId, watcher identity
         * @param aSipId, watcher sip URL
         * @return none
         */ 
        void AddWatcherIfNotExistsL( 
            const TDesC8& aId, 
            const TDesC& aSipId );
         
        /**
         * Remove Watcher if not exist
         *
         * @since S60 3.2
         * @param aId, watcher identity
         * @param aSipId, watcher sip URL
         * @return none
         */    
        void RemoveWatcherIfExists( 
            const TDesC8& aId, 
            const TDesC& aSipId );
        
        /**
         * Make current watcher list
         *
         * @since S60 3.2
         * @param none
         * @return CDesCArrayFlat, watcher array
         */ 
        CDesCArrayFlat* MakeCurrentWatcherListLC(); 
        
        /**
         * Collect watcher from winfo
         *
         * @since S60 3.2
         * @param aElems element array
         * @return none
         */
        void CollectWatchersL(  
			RPointerArray<MSimpleElement>& aElems );
        
        /**
         * CreatePublisherL()
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void CreatePublisherL();
		
        /**
         * Stores own presence status to presence cache
         *
         * @since S60 5.0
         * @return none
         */			
        void StoreOwnStatusToCacheL();

        /**
         * Resolves presence cache values according to provided
         * unicode string content.
         *
         * @since S60 5.0
         * @param aUnicodeContent, string containing info data
         * @param aActivities, activities string
         * @param aCacheAvailability, will store enumerated status
         * @param aCacheAvailability, will store extended status string
         * @param aXimpApiAvailability, will store ximp api availabiltiy
         * @return TBool, was handled or not
         */
        TBool ResolveOwnCacheValues( 
            const TDesC& aUnicodeContent,
            const TDesC& aActivities, 
            MPresenceBuddyInfo2::TAvailabilityValues& aCacheAvailability,
            TPtr& aExtendedCacheAvailability,
            NPresenceInfo::TAvailabilityValues& aXimpApiAvailability );
      
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
         * Not Own.
         */
        MSimpleConnection& iConnection;

        /**
         * SIMPLE engine publisher
         * Own.
         */
        MSimplePublisher* iPublisher;
        
        /**
         * SIMPLE engine watcher
         * Own.
         */
        MSimpleWatcher* iWatcher;    
        
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
         * Subscribed winfo
         * Own.
         */
        TBool iSubscribed; 
        
        /**
         * Subscribed own data
         * Own.         
         */
        TBool iSubscribedOwn;     
        
        /**
         * Published own data
         * Own.
         */
        TBool iPublished;  

        /**
         * RePublish status
         * Own.
         */
        TBool iRePublish;
        
        /**
         * Current operation
         * Own.
         */
        TPluginPublishOperation iOperation;     
           
        /**
         * Simple publish document
         * Own.
         */
        MSimpleDocument* iDocument;
       
        /**
         * XDM Utils
         * Not own.
         */
        CPresencePluginXdmUtils* iXdmUtils; 
        
        /**
         * WINFO watchers
         * Own.
         */
        TDblQue<CPresencePluginWatcherInfo> iWatcherList;
        
        /**
         * Presence Data
         * Not Own
         */
        CPresencePluginData* iPresenceData;
        
        /**
         * client statutus.
         * Not own.
         */
        TRequestStatus* iClientStatus;
        
        SIMPLE_UNIT_TEST( T_CPresencePluginPublisher )

        
    };

#endif // CPRESENCEPLUGINPUBLISHER_H