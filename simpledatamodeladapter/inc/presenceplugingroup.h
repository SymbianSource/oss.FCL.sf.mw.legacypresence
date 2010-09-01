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


#ifndef CPRESENCEPLUGINGROUP_H
#define CPRESENCEPLUGINGROUP_H

#include <e32base.h>
#include <ximpbase.h>
#include <protocolpresentitygroups.h>

#include "presenceplugincommon.h"
#include "mpresencepluginconnectionobs.h"
#include "presencelogger.h"

class TXIMPRequestId;
class MXIMPIdentity;
class MXIMPProtocolPresentityGroupsDataHost;
class MXIMPObjectCollection;
class CPresencePluginVirtualGroup;
class CPresencePluginData;

/**
 * CPresencePluginGroups
 *
 * Simple Engine Connection
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CPresencePluginGroups ) : public CActive,
    public MProtocolPresentityGroups
    {
    public:

        /**
         * Current operation
         */
        enum TPluginGroupsOperation
            {
            ENoOperation,
            ECreatePresentityGroup,
            EDeletePresentityGroup,
            EAddPresentityGroupMember,
            EGrantPresentityGroupMember,
            ERemovePresentityGroupMember,
            EWithdrawGrantFromMember,
            EGetListOfLists,
            EGetListContent,
            EBlockPresentityGroupMember,
            EUnblockPresentityGroupMember,
            EUnblockInAddPresentityGroupMember
            };

        /**
         * Current actibe object state
         */
        enum TPluginGroupsState
            {
            EPluginIdle,
            EPluginInitXdm,
            EPluginFetchRls,
            EPluginAddGroupMember,
            EPluginRemoveGroupMember,
            EPluginCommitRls,
            EPluginCommitXdm
            };

        /**
         * Constructor.
         * @param aObs callback for complete requests
         * @param aConn Simple Engine connection
         */
        static CPresencePluginGroups* NewL(
            MPresencePluginConnectionObs& aObs,
            CPresencePluginVirtualGroup* aSubscribedBuddies,
            CPresencePluginData* aPresenceData );

        /**
         * Constructor.
         * @param aObs callback for complete requests
         * @param aConn Simple Engine connection
         */
        static CPresencePluginGroups* NewLC(
            MPresencePluginConnectionObs& aObs,
            CPresencePluginVirtualGroup* aSubscribedBuddies,
            CPresencePluginData* aPresenceData );

        /**
         * Standard C++ destructor
         */ 
        virtual ~CPresencePluginGroups();

    private:
    
        /**
         * Standard C++ constructor
         */
        CPresencePluginGroups(
            MPresencePluginConnectionObs& aObs,
            CPresencePluginVirtualGroup* aSubscribedBuddies,
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


    public: // from base class MXIMPProtocolPresentityGroups

        /**
         * Defined in a base class
         */
        void DoSubscribePresentityGroupListL(
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUnsubscribePresentityGroupListL(
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoCreatePresentityGroupL(
            const MXIMPIdentity& aGroupId,
            const TDesC16& aDisplayName,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoDeletePresentityGroupL(
            const MXIMPIdentity& aGroupId,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUpdatePresentityGroupDisplayNameL(
            const MXIMPIdentity& aGroupId,
            const TDesC16& aDisplayName,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoSubscribePresentityGroupContentL(
            const MXIMPIdentity& aGroupId,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUnsubscribePresentityGroupContentL(
            const MXIMPIdentity& aGroupId,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoAddPresentityGroupMemberL(
            const MXIMPIdentity& aGroupId,
            const MXIMPIdentity& aMemberId,
            const TDesC16& aMemberDisplayName,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoRemovePresentityGroupMemberL(
            const MXIMPIdentity& aGroupId,
            const MXIMPIdentity& aMemberId,
            TXIMPRequestId aReqId );

        /**
         * Defined in a base class
         */
        void DoUpdatePresentityGroupMemberDisplayNameL(
            const MXIMPIdentity& aGroupId,
            const MXIMPIdentity& aMemberId,
            const TDesC16& aMemberDisplayName,
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
         * Start XDM operation
         *
         * @since S60 3.2 
         * @param aReqId, request id
         * @param aOperation, groups operation
         * @return none
         */
        void StartXdmOperationL(
            TXIMPRequestId aReqId,
            TPluginGroupsOperation aOperation );

        /**
         * Start XDM operation
         *
         * @since S60 3.2 
         * @param aReqId, request id
         * @param aOperation, groups operation
         * @return none
         */
        void StartXdmOperationL(
            const MXIMPIdentity& aId,
            TXIMPRequestId aReqId,
            TPluginGroupsOperation aOperation );

        /**
         * Start XDM operation
         *
         * @since S60 3.2 
         * @param none
         * @return none
         */
        void StartXdmOperationL();

        /**
         * Call actual XDM operation
         *
         * @since S60 3.2 
         * @param aCompleteStatus, success or not
         * @return none
         */
        void CallActualXdmOperationL( TInt aCompleteStatus );

        /**
         * Get List of lists
         *
         * @since S60 3.2 
         * @param none
         * @return none
         */
        void GetListOfListsL();

        /**
         * Get List content
         *
         * @since S60 3.2 
         * @param none
         * @return none
         */
        void GetListContentL();

        /**
         * Do Get BuddyList form XDM list
         *
         * @since S60 3.2 
         * @param none
         * @return none
         */
        void DoGetBuddyListL();
        
        /**
         * Do Get subscribed BuddyList form virtual list
         *
         * @since S60 3.2 
         * @param none
         * @return none
         */
        void DoGetSubscribedBuddyListL();
        
        /**
         * Removes cache entries for certain person.
         *
         * @since S60 5.0
         * @param none
         * @return none
         */        
        void DeletePersonCacheL();

    private:  // Data

        /**
         * XIMP Plugin connection observer
         * Own.
         */
        MPresencePluginConnectionObs& iConnObs;

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
         * Current operation
         * Own.
         */
        TPluginGroupsOperation iOperation;

        /**
         * Current presentity in progress
         * Own.
         */
        HBufC16* iPresIdentity;

        /**
         * XDM Utils
         * Not own.
         */
        CPresencePluginXdmUtils* iXdmUtils;

        /**
         * Display name
         * Own
         */
        HBufC* iDisplayName;
        
        /**
         * Active object state
         * Own.         
         */
        TPluginGroupsState iState;
        
        /**
         * Group request complete
         * Own.         
         */
        TBool iCompleted;
        
        /**
         * Virtual groups
         * Not Own
         */
        CPresencePluginVirtualGroup* iSubscribedBuddies;
        
        /**
         * Plugin data.
         * Not Own
         */        
        CPresencePluginData*    iPresenceData;
        
        SIMPLE_UNIT_TEST( T_CPresencePluginGroups )
    };

#endif // CPRESENCEPLUGINGROUP_H