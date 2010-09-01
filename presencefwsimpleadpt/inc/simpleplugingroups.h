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




#ifndef CSIMPLEPLUGINGROUPS_H
#define CSIMPLEPLUGINGROUPS_H

#include <e32std.h>

#include <ximpbase.h>
#include <protocolpresentitygroups.h>

#include <mrlspresxdmasynchandler.h>

#include "simpleplugincommon.h"
#include "msimplepluginconnectionobs.h"


class TXIMPRequestId;
class MXIMPIdentity;
class MPresenceInfoFilter;
class MProtocolPresentityGroupsDataHost;
class MXIMPObjectCollection;

class MSimpleWinfo;
class CPresenceXDM;
class CRLSXDM;
class CXdmEngine;
class CXdmDocument;


/**
 * CSimplePluginGroups
 *
 * Simple Engine Connection
 *
 * @lib ?library
 * @since s60 v5.0
 */
NONSHARABLE_CLASS( CSimplePluginGroups ) : public CActive,
    public MProtocolPresentityGroups
    {
private:


    /**
     * Current operation
     */
    enum TPluginGroupsOperation
        {
        ENoOperation,
        ECreatePresentityGroup,
        EDeletePresentityGroup,
        EAddPresentityGroupMember,
        ERemovePresentityGroupMember,
        EGetListOfLists,
        EGetListContent
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
        EPluginCommitXdm,
        ERemoveGroupFromGranted        
        };

public:
    /**
     * Constructor.
     * @param aObs callback for complete requests
     */
    static CSimplePluginGroups* NewL(
        MSimplePluginConnectionObs& aObs );

    virtual ~CSimplePluginGroups();



private:

    CSimplePluginGroups(
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


// from base class MProtocolPresentityGroups

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
     * Save presentity id
     * @param aPresentityId presentity id
     */
    void SetPresIdentity2L(
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
        TXIMPRequestId aReqId,
        TPluginGroupsOperation aOperation );

    /**
     * Start XDM operation
     */
    void StartXdmOperationL(
        const MXIMPIdentity& aId,
        TXIMPRequestId aReqId,
        TPluginGroupsOperation aOperation );

    /**
     * Start XDM operation
     */
    void StartXdmOperationL(
        const MXIMPIdentity& aId,
        const TDesC16& aDisplayName,
        TXIMPRequestId aReqId,
        TPluginGroupsOperation aOperation );

    /**
     * Start XDM operation
     */
    void StartXdmOperationL(
        const MXIMPIdentity& aId,
        const MXIMPIdentity& aMemberId,
        TXIMPRequestId aReqId,
        TPluginGroupsOperation aOperation );

    /**
     * Start XDM operation
     */
    void StartXdmOperationL(
        const MXIMPIdentity& aId,
        const MXIMPIdentity& aMemberId,
        const TDesC16& aDisplayName,
        TXIMPRequestId aReqId,
        TPluginGroupsOperation aOperation );              

    /**
     * Start XDM operation
     */
    void StartXdmOperationL();
    
    /**
     * Start XDM operation, StartXdmOperationL calls this.
     */
    void DoStartXdmOperationL( );      

    /**
     * Call CSimpleXdmUtils operation
     */
    void CallActualXdmOperationL();

    /**
     * Get list of the entity lists
     */
    void GetListOfListsL();

    /**
     * Get content of the entity list
     */
    void GetListContentL();

    /**
     * Yields to active scheduler and runs next RunL loop
     * @param aStatus error status
     */
    void CompleteMe( TInt aStatus );
    
    /**
     * Copy data from aLists and from aNames into aCollection
     * @param aNames group names [IN]
     * @param aNames group display names [IN]
     * @param aCollection collection of MPresentityGroupInfo [OUT]
     */         
    void CopyGroupArraysToCollectionL( 
        CDesCArrayFlat& aLists, CDesCArrayFlat& aNames, MXIMPObjectCollection& aCollection );  
        
    /**
     * Copy data from aIds and from aNames into aCollection
     * @param aNames group member names [IN]
     * @param aNames group member display names [IN]
     * @param aCollection collection of MPresentityGroupMemberInfo [OUT]
     */        
    void CopyGroupMembersToCollectionL( 
        CPtrCArray& aIds, CPtrCArray& aNames, MXIMPObjectCollection& aCollection);
        
        
    /**
     * Add presentity group
     * @param aGroupId group id
     * @param adispName display name
     */        
    void HandleCreatePresentityGroupL( 
        const TDesC& aGroupId, const TDesC& aDispName );
            
    /**
     * Delete presentity group
     * @param aGroupId group id
     */    
    void HandleDeletePresentityGroupL( const TDesC& aGroupId );
    
    /**
     * Add presentity group member
     */    
    void HandleAddPresentityGroupMemberL();
    
    /**
     * Remove group member
     */
    void HandleRemovePresentityGroupMemberL();



private: // Data

    /**
     * PrFw Plugin connection observer
     */
    MSimplePluginConnectionObs& iConnObs;

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
     * Subscribed items. Bitmask.
     */
    TUint iSubscribed;

    /**
     * Current operation
     */
    TPluginGroupsOperation iOperation;

    /**
     * Current presentity in progress
     * Own.
     */
    HBufC16* iPresIdentity;

    /**
     * Current presentity in progress
     * Own.
     */
    HBufC16* iPresIdentity2;

    /**
     * XDM Utils
     * Not own.
     */
    CSimplePluginXdmUtils* iXdmUtils;

    /**
     * Display name
     * Own
     */
    HBufC* iDisplayName;

    /**
     * Active object state
     */
    TPluginGroupsState iState;

    TBool iCompleted;

    };

#endif // CSimplePluginGroups_H
