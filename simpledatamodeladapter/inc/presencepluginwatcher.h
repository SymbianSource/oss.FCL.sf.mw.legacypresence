/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CPRESENCEPLUGINWATCHER_H
#define CPRESENCEPLUGINWATCHER_H

#include <e32base.h>
#include <s32strm.h>
#include <ximpbase.h>
#include <protocolpresencewatching.h>

#include "mpresencepluginconnectionobs.h"
#include "presencelogger.h"

class TXIMPRequestId;
class MXIMPIdentity;
class MXIMPProtocolConnectionHost;
class MSimpleConnection;
class CPresencePluginEntityWatcher;
class CPresencePluginData;

/**
 * CPresencePluginWatcher
 *
 * Simple Engine Connection
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CPresencePluginWatcher ): public CActive,
    public MProtocolPresenceWatching
    {
    public:

        /**
         * Current operation
         */
        enum TPluginWatcherOperation
            {
            EPluginUndef,
            EPluginSubscribeGroup,
            EPluginUnsubscribeGroup,
            EPluginSubscribeSingle,
            EPluginSubscribeByAdding,
            EPluginUnSubscribeByRemoving
            };
            
        /**
         * Current actibe object state
         */
        enum TPluginXdmState
            {
            EPluginIdle,
            EPluginInitXdm,
            EPluginFetchRls,
            EPluginAddGroupMember,
            EPluginRemoveGroupMember
            };        

        /**
         * Constructor.
         * @param aObs callback for complete requests
         * @param aConn Simple Engine connection
         */ 
        static CPresencePluginWatcher* NewL(
            MPresencePluginConnectionObs& aObs,
            MSimpleConnection& aConn,
            CPresencePluginData* aPresenceData );
        
        /**
         * Standard C++ destructor
         */ 
        virtual ~CPresencePluginWatcher();

        /**
         * SIP Watcher is complets it's task.
         * @param aStatus error status
         * @return none         
         */
        void CompleteWatcher( TInt aStatus );

        /**
         * Delete a single entity watcher
         * @param aPresentityid presentity id
         * @return none         
         */
        void DeleteWatcher(
            const TDesC8& aPresentityid );
        
        /**
         * SubscribeL()
         *
         * @since S60 5.1
         * @param aIdentity presentity id to subscribe
         * @return none
         */
        void SubscribeL( const TDesC& aIdentity );
        
        /**
         * SubscribeAllL()
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void SubscribeAllL();
        
        /**
         * UnSubscribeAllL()
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void UnSubscribeAllL(); 
        
        /**
         * Subscribes presentity presence.
         *
         * @since S60 5.0
         * @param aPresentityId, presentity id
         * @param aStatus, client status to complete
         * @return none
         */            
        void DoPerformSubscribePresentityPresenceL(
            const MXIMPIdentity& aPresentityId,
            TRequestStatus& aStatus );    
            
        /**
         * Unsubscribes presentity presence.
         *
         * @since S60 5.0
         * @param aPresentityId, presentity id
         * @param aStatus, client status to complete
         * @return none
         */            
        void DoPerformUnsubscribePresentityPresenceL(
            const MXIMPIdentity& aPresentityId,
            TRequestStatus& aStatus );
              
        /**
         * Accept watcher to watcher list
         *
         * @since S60 5.1
         * @param aWatcher, watcher
         * @return none
         */
        void AcceptL( const CPresencePluginEntityWatcher* aWatcher );
        
    private:

        /**
         * Standard C++ constructor
         * @param aObs, Presence plugin connection
         * @param aConn, SIMPLE connection             
         */ 
        CPresencePluginWatcher(
            MPresencePluginConnectionObs& aObs,
            MSimpleConnection& aConn,
            CPresencePluginData* aPresenceData );
            
    public: // from base class MXIMPBase

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
        
    public: // from base class MXIMPProtocolPresenceWatching

        /**
         * Defined in a base class
         */
        void DoSubscribePresentityPresenceL(
            const MXIMPIdentity& aPresentityId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUpdatePresentityPresenceSubscriptionPifL(
            const MXIMPIdentity& aPresentityId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUnsubscribePresentityPresenceL(
            const MXIMPIdentity& aPresentityId,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoSubscribePresentityGroupMembersPresenceL(
            const MXIMPIdentity& aGroupId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUpdatePresentityGroupMembersPresenceSubscriptionPifL(
            const MXIMPIdentity& aGroupId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUnsubscribePresentityGroupMembersPresenceL(
            const MXIMPIdentity& aGroupId,
            TXIMPRequestId aReqId );

    public:

         /**
         * Save presentity id
         *
         * @since S60 3.2
         * @param aPresentityId presentity id
         * @param aFormatUri, attempt to format uri
         * @return none         
         */
        void SetPresIdentityL(
            const MXIMPIdentity& aPresentityId,
            TBool aFormatUri );
            
        /**
         * Calls MXIMPProtocolConnectionHost callback
         *
         * @since S60 3.2
         * @param aStatus error status
         * @return none     
         */
        void CompleteXIMPReq( TInt aStatus );
        
        /**
         * Set XIMP request id
         *
         * @since S60 3.2
         * @param aId, request id
         * @return none    
         */
        void SetXIMPId( TXIMPRequestId aId );

        /**
         * Check if ID matches with existing watcher
         *
         * @since S60 5.1
         * @param aPresentityId presentity ID
         * @return ETrue if matching watcher found
         */
        TBool MatchWatcherId( const TDesC8& aPresentityId ) const;

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
         * Match existing watcher
         *
         * @since S60 3.2         
         * @param aPresentityId presentity id
         * @return watcher or NULL when not found
         */
        CPresencePluginEntityWatcher* MatchWatcherL(
            const TDesC8& aPresentityId, TBool aCreate );   
        
        /**
         * StartXdmOperationL
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void StartXdmOperationL();
                                      
    private: // Data

        /**
         * XIMP Plugin connection observer
         * Own.
         */
        MPresencePluginConnectionObs& iConnObs;
        
        /**
         * XIMP Host.     
         * Not Own.
         */    
        MXIMPProtocolConnectionHost* iHost;

        /**
         * SIMPLE engine connection
         * Not Own.
         */
        MSimpleConnection& iConnection;

        /**
         * SIMPLE engine watchers
         * Own.
         */
        RPointerArray<CPresencePluginEntityWatcher> iWatchers;    
        
        /**
         * Watcher candidates
         * Own.
         */
        RPointerArray<CPresencePluginEntityWatcher> iWatcherCandidates;
        
        /**
         * Current PrFW request id
         * Own.
         */
        TXIMPRequestId iXIMPId;
        
        /**
         * Simple Engine request id
         * Own.         
         */
        TInt iSimpleId;
        
         /**
         * XDM Utils
         * Not own.
         */
        CPresencePluginXdmUtils* iXdmUtils;
         
        /**
         * Current presentity in progress
         * Own.
         */
        HBufC16* iPresIdentity;
        
        /**
         * Presence data
         * Own
         */
        CPresencePluginData* iPresenceData;
        
        /**
         * XDM state
         */
        TPluginXdmState iXdmState;
        
        /**
         * Whether the request is completed
         * Own.
         */
        TBool iCompleted;
        
        /**
         * Current operation
         * Own.
         */
        TPluginWatcherOperation iOperation;    
        
        SIMPLE_UNIT_TEST( T_CPresencePluginWatcher )
        SIMPLE_UNIT_TEST( T_CPresencePluginXdmUtils )
    };

#endif // CPRESENCEPLUGINWATCHER_H
