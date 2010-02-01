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




#ifndef CSIMPLEPLUGINAUTHORIZATION_H
#define CSIMPLEPLUGINAUTHORIZATION_H

#include <e32std.h>

#include <ximpbase.h>
#include <protocolpresenceauthorization.h>

#include "simpleplugincommon.h"
#include "msimplepluginconnectionobs.h"

class TXIMPRequestId;
class MXIMPIdentity;
class MPresenceInfoFilter;
class MProtocolPresenceAuthorizationDataHost;

class MSimpleWinfo;
class CPresenceXDM;
class CRLSXDM;
class CXdmEngine;
class CXdmDocument;


/**
 * CSimplePluginAuthorization
 *
 * Simple Engine Connection
 *
 * @lib ?library
 * @since s60 v5.0
 */
NONSHARABLE_CLASS( CSimplePluginAuthorization ) : public CActive,
    public MProtocolPresenceAuthorization
    {
public:


    /**
     * Current operation
     */
    enum TPluginAuthOperation
        {
        ENoOperation,
        // XDM operations
        EGrantPresenceForPresentity,
        EGrantPresenceForPresentityGroupMembers,
        EWithdrawFromPresentity,
        EWithdrawFromGroupMembers,
        EGrantForEveryone,
        EWithdrawFromEveryone,
        ESubscribeBlockList,
        EUnsubscribeBlockList,
        EBlockPresentity,
        EUnblockPresentity,
        // SIP and XDM  operations
        ESubscribePresenceGrantRequestList,
        EUnsubscribePresenceGrantRequestList        
        };
        
    /**
     * Current default rule
     */
    enum TPluginAuthDefRule
        {
        ERuleDef,
        ERuleConfirm,
        ERuleAllow
        };     

    /**
     * Constructor.
     * @param aObs callback for complete requests
     */
    static CSimplePluginAuthorization* NewL(
        MSimplePluginConnectionObs& aObs );

    virtual ~CSimplePluginAuthorization();

    /**
     * SetDataHost
     *
     * Set Data Host
     *
     * @lib ?library
     * @since s60 v5.0
     */
    void SetDataHost( MProtocolPresenceAuthorizationDataHost& aDataHost );

    /**
     * WinfoTerminatedL
     *
     * SIMPLE Winfo watcher subscription is terminated
     * @param aReason reason code
     */
    void WinfoTerminatedL( TInt aReason );

    /**
     * WINFO received from SIMPLE
     * @param aWinfo WINFO
     */
    void WinfoNotificationL( MSimpleWinfo& aWinfo );
    
    /**
     * This is to ensure that WINFO watchers responses are detected.
     * @param aPrFwId framework request id to be completed
     * @param sStatus error code
     */
    TBool HandleIfMine( TXIMPRequestId aPrFwId, TInt aStatus );


private:

    CSimplePluginAuthorization(
        MSimplePluginConnectionObs& aObs );

    void ConstructL( );


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


// from base class MProtocolPresenceAuthorization

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
     * Save presentity id
     * @param aPresentityId presentity id
     */
    void SetPresIdentityL(
        const MXIMPIdentity& aPresentityId );

    /**
     * Calls MXIMPProtocolConnectionHost callback
     * @param aStatus error status
     */
    void CompletePrFwReq( TInt aStatus );


    /**
     * Start XDM operation
     */
    void StartXdmOperationL(
        const MXIMPIdentity& aId,
        TXIMPRequestId aReqId,
        TPluginAuthOperation aOperation );
        
    /**
     * Start XDM operation. StartXdmOperationL calls this.
     */
    void DoStartXdmOperationL(
        const MXIMPIdentity& aId,
        TXIMPRequestId aReqId,
        TPluginAuthOperation aOperation ); 
        
    /**
     * Start XDM operation. HandleIfMine calls this.
     */
    void DoStartXdmOperationL(
        TXIMPRequestId aReqId,
        TPluginAuthOperation aOperation );                

    /**
     * Call CSimpleXdmUtils operation
     */
    void CallActualXdmOperationL();
    
    /**
     * Copy data from aBlockers to aBlockedSouls
     * @param aBlockers user ids
     * @param aBlockedSouls user PrFwIdentities
     *
     */
    void CopyBlockersToArrayL( 
        CPtrCArray& aBlockers, RPointerArray<MXIMPIdentity>& aBlockedSouls ); 
        
    /**
     * Handle full Winfo notification
     * @param aWinfo WINFO
     */    
    void HandleFullWinfoNotificationL( MSimpleWinfo& aWinfo ); 
    
    /**
     * Handle partial Winfo notification
     * @param aWinfo WINFO     
     */    
    void HandlePartialWinfoNotificationL( MSimpleWinfo& aWinfo );
    
    /**
     * Call HandlePresenceGrantRequestReceivedL
     * @param aUserId user id
     */
    void CallHandlePresenceGrantRequestReceivedL( const TDesC& aUserId );

    /**
     * Call HandlePresenceGrantRequestObsoletedL(
     * @param aUserId user id
     */
    void CallHandlePresenceGrantRequestObsoletedL( const TDesC& aUserId ); 
    
    /**
     * Handle block list subscription routines
     */
    void HandleSubscribeBlockListL();        
    

private: // Data

    /**
     * PrFw Plugin connection observer
     */
    MSimplePluginConnectionObs& iConnObs;

    /**
     * PrFw Plugin Authorization Data Host
     * Not own.
     */
    MProtocolPresenceAuthorizationDataHost* iDataHost;

    /**
     * XDM Settings id
     */
    TInt iSettingsId;

    /**
     * PrFW request id
     */
    TXIMPRequestId iPrFwId;

    /**
     * PrFW request id for own requests
     */
    TXIMPRequestId iPrFwIdOwn;

    /**
     * Simple Engine request id
     */
    TInt iSimpleId;

    /**
     * Current operation
     */
    TPluginAuthOperation iOperation;

    /**
     * Current presentity in progress
     * Own.
     */
    HBufC16* iPresIdentity;

    /**
     * Whether XDM utility is initialized
     */
    TBool iXdmOk;

    /**
     * XDM Utils
     * Not own.
     */
    CSimplePluginXdmUtils* iXdmUtils;
    
    /**
     * Current default rule
     */
    TPluginAuthDefRule iDefRule; 
    
    /**
     * Active WINFO subscription
     */
    TBool iWinfoSubs;   


    };

#endif // CSimplePluginAuthorization_H
