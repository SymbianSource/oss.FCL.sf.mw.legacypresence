/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CPRESENCEPLUGINAUTHORIZATION_H
#define CPRESENCEPLUGINAUTHORIZATION_H

#include <e32base.h>
#include <ximpbase.h>
#include <protocolpresenceauthorization.h>
#include "mpresencepluginconnectionobs.h"
#include "presenceplugincontactsobs.h"
#include "presencelogger.h"

class TXIMPRequestId;
class MXIMPIdentity;
class MPresenceInfoFilter;
class MProtocolPresenceAuthorizationDataHost;
class MSimpleWinfo;
class CPresencePluginData;
class CPresencePluginContacts;

/**
 * CPresencePluginAuthorization
 *
 * Simple Engine Connection
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CPresencePluginAuthorization ) :
    public CActive,
    public MProtocolPresenceAuthorization,
    public MPresencePluginContactsObs
    {
    public: // Constructor and destructor

        /**
         * Current operation
         */
        enum TPluginAuthOperation
            {
            ENoOperation,
            EGrantPresenceForPresentity,
            EGrantPresenceForPresentityGroupMembers,
            EWithdrawFromPresentity,
            EWithdrawFromGroupMembers,
            EGrantForEveryone,
            EWithdrawFromEveryone,
            ESubscribeBlockList,
            EUnsubscribeBlockList,
            EBlockPresentity,
            EUnblockPresentity
            };
        
        /**
         * Block state
         */
        enum TPluginAuthState
            {
            EStateIdle,
            EStateBlocked,
            EStateDoBlock,
            EStateIsContactBlockedBuddyRequest,
            EStateDoUnBlock,
            EStatePresenceGranted,
            EStateRemovePresentityFromGranted,
            EStateAcceptBuddyRequest,
            EStateSubscribe
            };
        
        /**
         * Constructor.
         * @param aObs callback for complete requests
         */ 
        static CPresencePluginAuthorization* NewL(
            MPresencePluginConnectionObs& aObs,
            CPresencePluginData* aPresenceData );
            
        /**
         * Standard C++ destructor
         */ 
        virtual ~CPresencePluginAuthorization();

        /**
         * SetDataHost
         *
         * @since S60 3.2         
         * @param aDataHost, XIMP Plugin Authorization Data Host
         */
        void SetDataHost( MProtocolPresenceAuthorizationDataHost& aDataHost );

        /**
         * WinfoTerminatedL
         *
         * @since S60 3.2          
         * SIMPLE Winfo watcher sunscription is terminated
         * @param aReason reason code
         */
        void WinfoTerminatedL( TInt aReason );
         
        /** 
         * WINFO received from SIMPLE
         * @since S60 3.2
         * @param aWinfo WINFO
         */
        void WinfoNotificationL( MSimpleWinfo& aWinfo ); 

        /** 
         * Return List subscribe state
         *
         * @since S60 3.2
         * @param none
         * @return TBool subscribe state
         */
        TBool ReturnListSubsState(); 
        
        /** 
         * Returns plugin data handler.
         *
         * @since S60 5.0
         * @param none
         * @return CPresencePluginData data handler reference
         */        
        CPresencePluginData& PluginData();
        
        /** 
         * Internal grant operation for group operation.
         *
         * @since S60 5.0
         * @param aPresentityId, identity to be subscribed
         * @param aClientStatus, client status to be completed
         * @return none
         */             
        void DoPerformGrantPresenceForPresentityL(
            const MXIMPIdentity& aPresentityId,
            TRequestStatus& aClientStatus );
        
        /** 
         * Internal withdraw grant operation for group operation.
         *
         * @since S60 5.0
         * @param aPresentityId, identity to be ungranted
         * @param aClientStatus, client status to be completed
         * @return none
         */                     
        void DoPerformWithdrawPresGrantFromPresentityL(
            const MXIMPIdentity& aPresentityId,
            TRequestStatus& aClientStatus );
        
        /** 
         * Internal block operation for group operation.
         *
         * @since S60 5.0
         * @param aPresentityId, identity to be blocked
         * @param aClientStatus, client status to be completed
         * @return none
         */                     
         void DoPerformBlockPresenceForPresentityL(
            const MXIMPIdentity& aPresentityId,
            TRequestStatus& aClientStatus );

        /** 
         * Internal unblock operation for group operation.
         *
         * @since S60 5.0
         * @param aPresentityId, identity to be unblocked
         * @param aClientStatus, client status to be completed
         * @return none
         */    
         void DoPerformCancelPresenceBlockFromPresentityL(
            const MXIMPIdentity& aPresentityId,
            TRequestStatus& aClientStatus );

         /**
          * Resolve is blocked contact a blocked friend request
          *
          * @since S60 5.0
          * @param aPresenceId, identity to be resolved
          * @param aObserver, Returns results
          * @param aStatus, client status
          */
         void IsBlockedContactFriendRequestL(
             const TDesC& aPresenceId,
             MPresencePluginContactsObs& aObserver,
             TRequestStatus& aStatus );

    private:

        /**
         * Standard C++ constructor
         * @param aObs callback for complete requests         
         */ 
        CPresencePluginAuthorization(
            MPresencePluginConnectionObs& aObs,
            CPresencePluginData* aPresenceData );
        
        void ConstructL();

    public:	 // from base class MXIMPBase

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
        

    public: // from base class MXIMPProtocolPresenceAuthorization

        /**
         * Defined in a base class
         */
        void DoSubscribePresenceGrantRequestListL(
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUnsubscribePresenceGrantRequestListL(
            TXIMPRequestId aReqId );

        // Granting presence information access to single presentities

        /**
         * Defined in a base class
         */
        void DoGrantPresenceForPresentityL(
            const MXIMPIdentity& aPresentityId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUpdatePresenceGrantPifForPresentityL(
            const MXIMPIdentity& aPresentityId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoWithdrawPresenceGrantFromPresentityL(
            const MXIMPIdentity& aPresentityId,
            TXIMPRequestId aReqId );

        // Granting presence information access to presentity group members

        /**
         * Defined in a base class
         */
        void DoGrantPresenceForPresentityGroupMembersL(
            const MXIMPIdentity& aGroupId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUpdatePresenceGrantPifForPresentityGroupMembersL(
            const MXIMPIdentity& aGroupId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoWithdrawPresenceGrantFromPresentityGroupMembersL(
            const MXIMPIdentity& aGroupId,
            TXIMPRequestId aReqId );

        // Granting presence information access to everyone

        /**
         * Defined in a base class
         */
        void DoGrantPresenceForEveryoneL(
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUpdatePresenceGrantPifForEveryoneL(
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoWithdrawPresenceGrantFromEveryoneL(
            TXIMPRequestId aReqId );

        // Subscribe presence block list

        /**
         * Defined in a base class
         */
        void DoSubscribePresenceBlockListL(
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUnsubscribePresenceBlockListL(
            TXIMPRequestId aReqId );

        // Blocking presence information access from single presentities

        /**
         * Defined in a base class
         */
        void DoBlockPresenceForPresentityL(
            const MXIMPIdentity& aPresentityId,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoCancelPresenceBlockFromPresentityL(
            const MXIMPIdentity& aPresentityId,
            TXIMPRequestId aReqId );

    public: // MPresencePluginContactsObs
	
        /**
         * Request Complete 
         *
         * @param aResult Result data returned, ownership changed to client.
         *                In operation EOperationIsPresenceStoredToContacts
         *                return type is TBool*.
         * @param aOperation Operation type
         * @param aError Completion error code
         */
        void RequestComplete( TAny* aResult,
                TPresenceContactsOperation aOperation,  TInt aError );
    
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
         * Save presentity id
         *
         * @since S60 3.2
         * @param aPresentityId presentity id
         * @param aFormatUri, attempt to format URI (set
         * ETrue is aPresentityId is uri identity)
         * @return none
         */
        void SetPresIdentityL(
            const MXIMPIdentity& aPresentityId,
            TBool aFormatUri );

        /**
         * Complete client request
         *
         * @since S60 3.2        
         * @param aStatus
         * @return none
         */
        void CompleteXIMPReq( TInt aStatus ); 
        
        /**
         * Start XDM operation
         *
         * @since S60 3.2
         * @param aId, XIMP identity
         * @param aReqId, request id
         * @param aOperation, plugin operation
         * @return none
         */
        void StartXdmOperationL(
            const MXIMPIdentity& aId,
            TXIMPRequestId aReqId,
            TPluginAuthOperation aOperation );
           
        /**
         * Start XDM operation
         *
         * @since S60 5.0
         * @param aId, XIMP identity
         * @param aOperation, plugin operation
         * @param aClientStatus, client status to complete
         * @return none
         */            
        void StartXdmOperationL(
            const MXIMPIdentity& aId,
            TPluginAuthOperation aOperation,
            TRequestStatus& aClientStatus );

        /**
         * Call actual xdm operation from xdm utils
         *
         * @since S60 3.2         
         * @param none
         * @return none
         */
        void CallActualXdmOperationL();
        
        /**
         * Subscribe block list
         *
         * @since S60 3.2
         * @param none
         * @return none         
         */
        void SubscribeBlockListL( );
        
        /**
         * Unsubscribe block list
         *
         * @since S60 3.2
         * @param none
         * @return none         
         */
        void UnSubscribeBlockListL();
        
        /**
         * Handle full winfo notification
         *
         * @since S60 3.2
         * @param MSimpleWinfo winfo notification
         * @return none         
         */
        void HandleFullWinfoNotificationL( MSimpleWinfo& aWinfo );
        
        /**
         * Handle partial winfo notification
         *
         * @since S60 3.2
         * @param MSimpleWinfo winfo notification
         * @return none         
         */
        void HandlePartialWinfoNotificationL( MSimpleWinfo& aWinfo );
        
        /**
         * Handle received grant request
         *
         * @since S60 3.2
         * @param TDesC userid
         * @return none         
         */
        void CallHandlePresenceGrantRequestReceivedL( const TDesC& aUserId );
        
        /**
         * Handle obsolated grantrequest
         *
         * @since S60 3.2
         * @param TDesC userid
         * @return none         
         */
        void CallHandlePresenceGrantRequestObsoletedL( const TDesC& aUserId );
        
        /**
         * Grant presence for presentity
         *
         * @since S60 3.2
         * @param none
         * @return none         
         */
        void GrantPresenceForPresentityL();
        
        /**
         * Withdraw from presentity
         *
         * @since S60 3.2
         * @param none
         * @return none         
         */
        void WithdrawFromPresentityL();
        
        /**
         * Set presentity blocked to XIMP
         *
         * @since S60 5.1
         * @param none
         * @return none
         */
        void SetPresentityBlockedToXIMPL();
        
        /**
         * Block presentity
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void BlockPresentityL();
        
        /**
         * Unblock presentity
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void UnblockPresentityL();
        
        /**
         * Unsubscribe presentity presence
         *
         * @since S60 3.2
         * @param none
         * @return none         
         */
        void UnsubscribePresentityPresenceL();
        
    private: // Data

        /**
         * XIMP Plugin connection observer
         * Own.
         */
        MPresencePluginConnectionObs& iConnObs;

        /**
         * XIMP Plugin Authorization Data Host
         * Not own.
         */
        MProtocolPresenceAuthorizationDataHost* iDataHost;

        /**
         * XDM Settings id
         * Own.         
         */
        TInt iSettingsId;

        /**
         * PrFW request id
         * Own.
         */
        TXIMPRequestId iXIMPId;
        
        /**
         * PrFW request id for own requests
         * Own.
         */
        TXIMPRequestId iXIMPIdOwn;    
        
        /**
         * Simple Engine request id
         * Own.
         */
        TInt iSimpleId;
        
        /**
         * Subscribed items.
         * Own.
         */
        TBool iSubscribed; 
        
        /**
         * Current operation
         * Own.
         */    
        TPluginAuthOperation iOperation;     
        
        /**
         * Current presentity in progress
         * Own.
         */
        HBufC16* iPresIdentity;   

        /**
         * Whether Presence XDM is up-to-date
         * Own.         
         */
        TBool iXdmOk;

        /**
         * XDM Utils
         * Not own.
         */
        CPresencePluginXdmUtils* iXdmUtils;
        
        /**
         * Authorization complete
         * Own.
         */
        TBool iComplete;
        
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
        
        /**
         * Block state.
         * Own.
         */
        TPluginAuthState iAuthState;
        
        /*
         * Virtual phone book contact database operations
         * Own.
         */
        CPresencePluginContacts* iContacts;
        
        /*
         * Data Returned by CPresencePluginContacts
        */
        TBool iContactIsStored;
        
        SIMPLE_UNIT_TEST( T_CPresencePluginAuthorization )
        SIMPLE_UNIT_TEST( T_CPresencePluginXdmUtils )
         
    };

#endif // CPresencePluginAuthorization_H