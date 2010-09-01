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
* Description:    Simple Protocol implementation for Presence Framework
*
*/




#ifndef CSIMPLEPLUGINWATCHER_H
#define CSIMPLEPLUGINWATCHER_H

#include <e32std.h>

#include <protocolpresencewatching.h>

#include <msimplewatcherobserver.h>

#include "simpleplugincommon.h"
#include "msimplepluginconnectionobs.h"
#include "msimplepluginsettings.h"


class TXIMPRequestId;
class MXIMPIdentity;
class MXIMPProtocolConnectionHost;
class MPersonPresenceInfo;
class MXIMPObjectCollection;

class MSimpleDocument;
class MSimpleWatcher;

class CSimplePluginEntityWatcher;
class CSimplePluginXdmUtils;

/**
 * CSimplePluginWatcher
 *
 * Simple Engine Connection
 *
 * @lib ?library
 * @since s60 v5.0
 */
NONSHARABLE_CLASS( CSimplePluginWatcher ): public CActive,
    public MProtocolPresenceWatching
    {
    
private:

    /**
     * Current operation
     */
    enum TPluginWatcherOperation
        {
        EPluginUndef,
        EPluginSubscribeGroup,
        EPluginUnsubscribeGroup
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
        // EPluginCommitRls,
        // EPluginCommitXdm
        };        
            
public:

    /**
     * Constructor.
     * @param aconnSets current settings
     * @param aObs callback for complete requests
     * @param aConn Simple Engine connection
     */
    static CSimplePluginWatcher* NewL(
        MSimplePluginSettings& aConnSets,
        MSimplePluginConnectionObs& aObs,
        MSimpleConnection& aConn );

    virtual ~CSimplePluginWatcher();

    /**
     * SetHost
     *
     * Set PrFw Host
     *
     * @lib ?library
     * @since s60 v5.0
     * @patam aHost PrFW Host
     */
    void SetHost( MXIMPProtocolConnectionHost* aHost );

    /**
     * SIP Watcher is complets it's task.
     * @param aStatus error status
     */
    void CompleteWatcher( TInt aStatus );

    /**
     * Delete a single entity watcher
     * @param aPresentityid presentity id
     */
    void DeleteWatcher(
        const TDesC8& aPresentityid );

    /**
     * Get Block list
     * @param aList list name
     * @param aMembers blocked users [OUT]
     */
    void GetEntitiesInListL(
        const TDesC& aList,
        MXIMPObjectCollection& aMembers );


private:

    CSimplePluginWatcher(
        MSimplePluginSettings& aConnSets,
        MSimplePluginConnectionObs& aObs,
        MSimpleConnection& aConn );

    void ConstructL( );
    
    /**
     * Calls MXIMPProtocolConnectionHost callback
     * @param aStatus error status
     */
    void CompletePrFwReq( TInt aStatus ); 
    
    /**
     * Handles RunL routines for subscription
     */
    void DoRunForSubscriptionL();
    
    /**
     * Handles RunL routines for unsubscription
     */
    void DoRunForUnsubscriptionL();    


public:


// from base class MXIMPBase
public:

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


// from base class MProtocolPresenceWatching
public:

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

protected:

// from base class CActive

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
     * @param aPresentityId presentity id
     * @return watcher or NULL when not found
     */
    CSimplePluginEntityWatcher* MatchWatcher2L(
        const TDesC8& aPresentityId, TBool aCreate );

    /**
     * Save presentity id
     * @param aPresentityId presentity id
     */
    void SetPresIdentityL(
        const MXIMPIdentity& aPresentityId );

    /**
     *  Start watching service in RLS
     */
    void StartWatchingRlsL();

    void StartXdmOperationL();
    
    /**
     * StartXdmoperationL calls this
     */
    void DoStartXdmOperationL();    
    
    void GetListContentL();  
    
    /**
     * Copy data from aIds and from aNames into aCollection
     * @param aNames group member names [IN]
     * @param aNames group member display names [IN]
     * @param aCollection collection of MPresentityGroupMemberInfo [OUT]
     */        
    void CopyGroupMembersToCollectionL( 
        CPtrCArray& aIds, CPtrCArray& aNames, MXIMPObjectCollection& aCollection);
        
    /**
     * DoSubscribePresentityPresenceL calls this. 
     */
    void DoDoSubscribePresentityPresenceL(
            const MXIMPIdentity& aPresentityId,
            TXIMPRequestId aReqId );  
            
    /**
     * DoUnsubscribePresentityPresenceL calls this.
     */
    void DoDoUnsubscribePresentityPresenceL(
            const MXIMPIdentity& aPresentityId,
            TXIMPRequestId aReqId );  
          
    /**
     * DoDoUnsubscribePresentityGroupMembersPresenceL calls this
     */        
    void DoDoUnsubscribePresentityGroupMembersPresenceL(
            const MXIMPIdentity& aGroupId,
            TXIMPRequestId aReqId );                                  


private: // Data

    /**
     * Current settings
     */
    MSimplePluginSettings& iConnSets;

    /**
     * PrFw Plugin connection observer
     */
    MSimplePluginConnectionObs& iConnObs;

    /**
     * SIMPLE engine connection
     */
    MSimpleConnection& iConnection;

    /**
     * SIMPLE engine watchers
     */
    RPointerArray<CSimplePluginEntityWatcher> iWatchers;

    /**
     * Current PrFW request id
     */
    TXIMPRequestId iPrFwId;

    /**
     * Current presentity in progress
     * Own.
     */
    HBufC16* iPresIdentity;

    /**
     * XDM Utils
     * Not own.
     */
    CSimplePluginXdmUtils* iXdmUtils;

    /**
     * Whether the request is completed
     */
    TBool iCompleted;
    
    TPluginWatcherOperation iOperation;
    
    TPluginXdmState iXdmState;

    };

#endif // CSimplePluginWatcher_H
