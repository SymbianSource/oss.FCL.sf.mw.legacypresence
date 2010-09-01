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




#ifndef CSIMPLEPLUGINXDMUTILS_H
#define CSIMPLEPLUGINXDMUTILS_H

#include <e32std.h>

#include <badesca.h>

#include <mrlspresxdmasynchandler.h>

#include "simpleplugincommon.h"

// Test suite
class MSimplePluginTestObs;

class MSimplePluginSettings;

class CPresenceXDM;
class CRLSXDM;
class CXdmEngine;
class CXdmDocument;
class CXdmDocumentNode;

class TPresenceActionXDM;
class TPresenceTransformXDM;
class TPresCondMisc;


// notice: class CPtrCArray;  after PrFw headers fixed.

class TRequestStatus;


/**
 * CSimplePluginXdmUtils
 *
 * Simple Engine Connection
 *
 * @lib ?library
 * @since s60 v5.0
 */
NONSHARABLE_CLASS( CSimplePluginXdmUtils ) : public CActive,
    public MRLSPresXDMAsyncHandler
    {
public:


    /**
     * Active object asynch states
     */
    enum TPluginXdmState
        {
        EStateIdle,
        /** Get OMA XDM lists */
        EGetXdmOMALists,
        /** Create OMA XDM lists */
        ECreateXdmOMALists,
        /** Get OMA XDM lists only, no rules later */
        EGetXdmOMAListsOnly,
        /** Create OMA XDM lists only, no rules later */
        ECreateXdmOMAListsOnly,
        /** Get RLS service */
        EGetRlsServices,
        /** Update RLS service */
        EUpdateRlsServices,
        /** Get rules from server for GrantPresenceForPresentity */
        EGetXdmRules,
        /** Update rules to server for GrantPresenceForPresentity */
        EUpdateXdmRules,
        /** Update XDM OMA List */
        EUpdateXdmOMAList,
        /** Cancel XDM document operation */
        ECancelDocument,
        /** Re-Update RLS service - 409 HTTP error handling*/
        EReUpdateRlsServices        
        };

    /**
     * Current operation
     */
    enum TPluginXdmOperation
        {
        ENoOperation,
        EXdmInit,
        EXdmInitXdmsOnly,
        EXdmDeleteAll,
        EXdmCancel,
        EXdmAddUserToGroup,
        EXdmAddGroupToGroup,
        EXdmRemoveUserFromGroup,
        EXdmRemoveGroupFromGroup,
        EXdmGrantForAll,
        EXdmWithdrawFromAll,
        EXdmGetBlockedList,
        EXdmCreateGroup,
        EXdmDeleteGroup,
        EXdmRlsFetch,
        EXdmRlsAdd,
        EXdmRlsRemove,
        EXdmGrantGroup,
        EXdmWithdrawGroup,
        EXdmSetReactiveAuth,
        EXdmSetProactiveAuth
        };
               

    /**
     * Constructor.
     * @param aConnSets connection settings
     * @param aXdmId XDm settings id
     */
    static CSimplePluginXdmUtils* NewL(
        MSimplePluginSettings& aConnSets,
        TInt aXdmId);

    virtual ~CSimplePluginXdmUtils();
    
    /**
     * Cancel non active wrappers
     */
    void CancelWrappers();
     
    /**
     * Initialize XDMS and Presence XDM structures in network.
     * @param aStatus Request status of the caller
     */
    void InitializeXdmL( TRequestStatus& aStatus );

    /**
     * Initialize XDMS in network
     * @param aStatus Request status of the caller
     */
    void InitializeXdmsOnlyL( TRequestStatus& aStatus );

    /**
     * Add resource-list into RLS document, overwrite old document
     * @param aStatus Request status of the caller
     */
    void FetchRlsL( TRequestStatus& aStatus );

    /**
     * Add resource-list into RLS document.
     * FetchRlsL must be called before this method.
     * @param aName list name
     */
    void AddRlsGroupL(
        const TDesC& aName );

    /**
     * Remove resource-list in RLS document
     * FetchRlsL must be called before this method.
     * Use RemoveRlsServiceByResourceListL instead when possible.      
     * @param aName list name
     */
    void RemoveRlsGroupL( const TDesC& aName );
    
    /**
     * Remove service URI in RLS document by corresponding shared XDM document entry.
     * The document in the server is updated when needed.
     * FetchRlsL must be called before this method.
     * This completes without error if the service was not found. 
     * @param aGroup user's group name, that's converted to resource-list value and used
     * to match the service resource-list in the rls-services document.
     * @param aStatus Request status of the caller.     
     */
    void RemoveRlsServiceByResourceListL( const TDesC& aGroup, TRequestStatus& aStatus  );    
    
    
    /**
     * Access service URI in RLS document by corresponding shared XDM document entry.
     * FetchRlsL must be called before this method.
     * Leaves with KErrNotFound if RLS service is not found.
     * @param aGroup user's group name, that's converted to resource-list value and used
     * to match the service resource-list in the rls-services document.
     * @return Access service URI in RLS document, ownership is transferred. May be NULL.
     */
    HBufC* RlsServiceByResourceListLC( const TDesC& aGroup );     

    /**
     * Add entity to granted list.
     * InitializeXdmL must be called before this method.
     * @param aUri entity URI to be granted
     * @param aStatus Request status of the caller
     */
    void AddEntityToGrantedL( const TDesC& aUri, TRequestStatus& aStatus );

    /**
     * Remove entity from granted list.
     * InitializeXdmL must be called before this method.
     * @param aUri entity URI to be removed
     * @param aStatus Request status of the caller
     */
    void RemoveEntityFromGrantedL(
        const TDesC&  aUri, TRequestStatus& aStatus );

    /**
     * Add entity to blocked list.
     * InitializeXdmL must be called before this method.
     * @param aUri entity URI to be blocked
     * @param aStatus Request status of the caller
     */
    void AddEntityToBlockedL( const TDesC& aUri, TRequestStatus& aStatus );

    /**
     * Remove entity from blocked list.
     * InitializeXdmL must be called before this method.
     * @param aUri entity URI to be removed
     * @param aStatus Request status of the caller
     */
    void RemoveEntityFromBlockedL(
        const TDesC&  aUri, TRequestStatus& aStatus );

    /**
     * Add group to granted.
     * InitializeXdmL must be called before this method.
     * @param aName a group to be granted
     * @param aStatus Request status of the caller
     */
    void AddGroupToGrantedL( const TDesC& aName, TRequestStatus& aStatus );

    /**
     * Remove group from granted list.
     * InitializeXdmL must be called before this method.
     * @param aName a group to be removed from granted list
     * @param aStatus Request status of the caller
     */
    void RemoveGroupFromGrantedL(
        const TDesC&  aName, TRequestStatus& aStatus );

    /**
     * Grant right for everyone, proactive authorization is set on.
     * InitializeXdmL must be called before this method.
     * @param aStatus Request status of the caller
     */
    void GrantForEveryoneL( TRequestStatus& aStatus );

    /**
     * Withdraw right from eveyone, reactive authorization is set on.
     * InitializeXdmL must be called before this method.
     * @param aStatus Request status of the caller
     */
    void WithdrawFromEveryoneL( TRequestStatus& aStatus );
    
    /**
     * Set reactive authorization policy.
     * Default rule is set to CONFIRM.
     */
    void SetReactiveAuthL( TRequestStatus& aStatus ); 
    
    /**
     * Set proactive authorization policy.
     * Default rule is set to ALLOW if it was CONFIRM.
     */
    void SetProactiveAuthL( TRequestStatus& aStatus );      

    /**
     * Get (subscribe) Block List.
     * InitializeXdmL must be called before this method.
     * @param aMembers blocked users [OUT].
     */
    void SubscribeBlockListL( CPtrCArray& aMembers );

    /**
     * Unsubscribe Block List.
     * @param aStatus Request status of the caller
     */
    void UnsubscribeBlockListL( TRequestStatus& aStatus );

    /**
     * Create entity's own group
     * InitializeXdmL or InitilaizeXdmsOnlyL must be called before this method.
     * @param aName group name
     * @param aDisplayName group's display name
     */
    void CreateEntityGroupL(
        const TDesC&  aName,
        const TDesC&  aDisplayName );

    /**
     * Delete entity's own group
     * InitializeXdmL or InitilaizeXdmsOnlyL must be called before this method.
     */
    void DeleteEntityGroupL( const TDesC&  aName );

    /**
     * Add group member
     * InitializeXdmL or InitilaizeXdmsOnlyL must be called before this method.
     * @param aGroup group name
     * @param aUser member
     * @param aDispName display name
     * @param aStatus Request status of the caller
     */
    void AddPresentityGroupMemberL(
        const TDesC&  aGroup,
        const TDesC&  aUser,
        const TDesC&  aDispName,
        TRequestStatus& aStatus );

    /**
     * Remove group member
     * InitializeXdmL or InitilaizeXdmsOnlyL must be called before this method.
     * @param aGroup group name
     * @param aUser member
     * @param aStatus Request status of the caller
     */
    void RemovePresentityGroupMemberL(
        const TDesC&  aGroup,
        const TDesC&  aUser,
        TRequestStatus& aStatus );

    /**
     * GetUserListsL
     * InitializeXdmL or InitilaizeXdmsOnlyL must be called before this method.
     * The size of the parameter arrays are same.
     * @param aIds sip id array [OUT]
     * @param aDispNames display name array [OUT]
     */
    void GetUserListsL( CDesCArrayFlat& aIds, CDesCArrayFlat& aDispNames );

    /**
     * GetEntitiesInListL
     * InitializeXdmL or InitilaizeXdmsOnlyL must be called before this method.
     * The size of the parameter arrays are same.
     * @param aList naem of the user's list [IN]
     * @param aIds sip id array [OUT]
     * @param aDispNames display name array [OUT]
     */
    void GetEntitiesInListL(
        const TDesC&  aList, CPtrCArray& aIds, CPtrCArray& aDispNames);

    /**
     * Updates XDM reource lists into server
     * @param aStatus Request status of the caller
     */
    void CommitXdmL( TRequestStatus& aStatus );

    /**
     * Updates RLS reource lists into server
     * @param aStatus Request status of the caller
     */
    void CommitRlsL( TRequestStatus& aStatus );

    /**
     * Test Suite setter
     * @param aObs test observer, may be NULL
     */
    void SetTestObserver( MSimplePluginTestObs* aObs );

    /**
     * Generate RLS document URI based on URI template located in CentRep variation definition.
     * @param aGroup grouup name
     * @param aPresentityUri presntity's URI, may be SIP or TEL URI. Domain part is stripped from SIP URI.
     * @param aDomain domain part of the URI,  
                      Leading '@' is stripped off and it must be hard coded in a template when needed.
     * @return RLS document URI     
     */
    HBufC16* CreateRlsDocumentUri16LC(
        const TDesC16& aGroup,
        const TDesC16& aPresentityUri,
        const TDesC16& aDomain );                

private:

    CSimplePluginXdmUtils(
        MSimplePluginSettings& aObs, TInt aXdmId );

    void ConstructL( );

  TBool DoesUserListExistsL( const TDesC& aName );


public:

// from base class MRLSPresXDMAsyncHandler

    /**
     * Defined in a base class
     */
    void HandleRLSUpdateDocumentL(TInt aErrorCode);

    /**
     * Defined in a base class
     */
    void HandleRLSUpdateCancelL(TInt aErrorCode);

    /**
     * Defined in a base class
     */
    void HandleRLSDeleteAllEmptyListsL(TInt aErrorCode);

    /**
     * Defined in a base class
     */
    void HandlePresUpdateDocumentL(TInt aErrorCode);

    /**
     * Defined in a base class
     */
    void HandlePresUpdateCancelL(TInt aErrorCode);



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
     * Complete client request
     * @param aStatus return code
     */
    void CompleteClientReq( TInt aStatus );

    /**
     * Get XDm OMA lists from a server
     */
    void GetXdmOMAListsL();

    /**
     * Create OMA Shared Xdm Lists when needed.
     * @return true if a list is created.
     */
    TBool CreateXdmOMAListsIfNeededL( );

    /**
     * Create a specified OMA Shared Xdm List when needed.
     *
     * @param aRoot parent for the new list
     * @param aName name for the new list
     * Leave with KErrAlreadyExists if all the lists already exist.
     * @return true is list was added into root node.
     */
    TBool CreateXdmOMAListIfNeededL( CXdmDocumentNode* aRoot, const TDesC& aName );

    /**
     * Create Resource List in shared xdm document
     * @param aParent parent for the new list
     * @param aName name for the new list
     */
    void CreateResourceListL(
        CXdmDocumentNode* aParent, const TDesC& aName );

    /**
     * Search List under a parent list, one level only
     * @param aParent parent list node
     * @param aName list to be searched for
     * @return child node found or NULL if not found.
     */
    CXdmDocumentNode* SearchListUnderParentL(
        CXdmDocumentNode* aParent, const TDesC& aName );

    void GetXdmRulesL();

    /**
     * Make Rls list URI
     * @param aXCapUri XCAP URI
     * @param aListName a list to be referred
     * @return URI, ownerhisp is transferred
     */
    HBufC* MakeRlsUriL(
        const TDesC& aXcapUri, const TDesC& aListName );

    /**
     * Make Rls list URI
     * @param aXCapUri XCAP URI
     * @param aListName a list to be referred
     * @return URI, ownerhisp is transferred
     */
    HBufC* MakeRlsBuddyUriL(
        const TDesC& aXcapUri, const TDesC& aListName );

    void MakeInitialXdmsDocumentL();

    /**
     * Updates XDM rules into server
     */
    void UpdateXdmRulesL();

    /**
     * Updates XDMS reource lists into server
     */
    void UpdateXdmsL();

    /**
     * Updates RLS document into server
     */
    void UpdateRlsL();

    /**
     * Handles pres auth rules document callback method
     * @param aErrorCode returned error code
     */
    void DoHandlePresUpdateDocumentL( TInt aErrorCode );
    
    /**
     * Handles RLS document callback method
     * @param aErrorCode returned error code
     */
    void DoHandleRLSUpdateDocumentL(TInt aErrorCode);    

    void DoAddUserToListL(
        const TDesC& aList, const TDesC&  aName );

    void DoAddUserToUserListL(
        const TDesC& aList, const TDesC&  aUser, const TDesC&  aDispName );

    void DoAddListIntoGrantRuleL( const TDesC& aListUri );

    void DoRemoveListFromGrantRuleL( const TDesC& aListUri );

    void DoRemoveUserFromListL(
        const TDesC& aList, const TDesC&  aName );

    void DoRemoveUserFromUserListL( const TDesC& aList, const TDesC&  aUser );

    void DoGrantForAllL();

    void DoWithdrawFromAllL();
    
    void DoSetReactiveAuthL(); 
    
    /**
     * Set Proactive authorization
     */
    void DoSetProactiveAuthL();        

    void DoGetListMembersL(
        const TDesC& aList, CPtrCArray& aMembers  );

    void DoCreateEntityGroupL( const TDesC& aList, const TDesC& aDisName  );

    void DoDeleteEntityGroupL( const TDesC& aList );

    /**
     * Search OMA Buddy List
     * Leaves with KErrNotFound if not found
     * @return list node
     */
    CXdmDocumentNode* DoGetBuddyListL();

    /**
     * Search a specific user XDM list
     * Leaves with KErrNotFound if not found
     * @param aList list name
     * @param aBuddyList parent buddy list
     * @return list node
     */
    CXdmDocumentNode* DoGetUserListL(
        const TDesC& aList, CXdmDocumentNode* aBuddyList );
        
    /**
     * Remove service URI in RLS document by corresponding shared XDM document entry.
     * FetchRlsL must be called before this method.
     * @param aResourceList resource-list entry value, a reference to Shared XDM document.
     * @return ETrue if service is found and removed.
     */
    TBool DoRemoveRlsServiceByResourceListL( const TDesC& aResourceList ); 
    
    /**
     * Access service URI in RLS document by corresponding shared XDM document entry.
     * FetchRlsL must be called before this method.
     * @param aResourceList resource-list entry value, a reference to Shared XDM document.
     * @return service URI, ownership is transferred.
     */
    HBufC* DoGetRlsServiceByResourceListL( const TDesC& aResourceList );  
    
    /**
     * Accesor for iAuxBuffer that contains last negotiated RLS service URI
     * @return buffer content
     */    
    TPtrC AuxBuffer(); 
    
    /**
     * Run next XDM routines
     * @param aOrigState original XDM state
     * @param aStatus error status
     */
    void DoXdmRunL( TPluginXdmState aOrigState, TInt aStatus ); 
    
    /**
     * Handle RLS service URI negatioation
     */
    void RlsServiceUriNegotiationL();
    
    /**
     * Make default auth rule
     * @param aRuleId Rule id
     * @param aAction action class initialized
     * @param aTransform transform class initialized
     */
    void MakeDefaultRuleL( 
        TDes& aRuleId, 
        TPresenceActionXDM& aAction, 
        TPresenceTransformXDM& aTransform,
        TPresCondMisc& aMisc );
      
    /**
     * Replace Group tag in URI
     * @param aBuf URI buffer [IN,OUT]
     * @param aGroup Group name
     */         
    static void ReplaceGroupTag( HBufC16* aBuf, const TDesC16& aGroup );
        
    /**
     * Replace Domain tag in URI
     * @param aBuf URI buffer [IN,OUT]
     * @param aDomain Domain value
     */          
    static void ReplaceDomainTag( HBufC16* aBuf, const TDesC16& aDomain );
        
    /**
     * Replace User tag in URI
     * @param aBuf URI buffer [IN,OUT]
     * @param aPresentityUri User URI value
     */    
    static void ReplaceUserTag( 
        HBufC16* aBuf, const TDesC16& aPresentityUri );


private: // Data

    /**
     * PrFw Plugin connection observer
     */
    MSimplePluginSettings& iConnSets;

    /**
     * XDM Settings id
     */
    TInt iSettingsId;

    /**
     * Simple Engine request id
     */
    TInt iSimpleId;

    /**
     * Subscribed items. Bitmask.
     */
    TUint iSubscribed;

    /**
     * Presence XDM Client
     * Own.
     */
    CPresenceXDM* iPresenceXdm;

    /**
     * RlsXDM Client
     * Own.
     */
    CRLSXDM* iRlsXdm;

    /**
     * XDM Engine
     * Own.
     */
    CXdmEngine* iXdmEngine;

    /**
     * XDM document for resource-lists
     * Own.
     */
    CXdmDocument* iXdmDoc;

    /**
     * Current state
     */
    TPluginXdmState iXdmState;

    /**
     * Current operation
     */
    TPluginXdmOperation iOperation;

    /**
     * Whether Presence XDM is up-to-date
     */
    TBool iPresXdmOk;

    /**
     * Whether XDM is up-to-date
     */
    TBool iXdmOk;

    /**
     * client statutus.
     * Not own.
     */
    TRequestStatus* iClientStatus;

    /**
     * Test suite observer
     * Not own.
     */
    MSimplePluginTestObs* iTestObs;
    
    /**
     * Auxiliary buffer for for Negotiated RLS service URI
     */
    HBufC* iAuxBuffer;
    
    /**
     * Auxiliary buffer for for Negotiated RLS service URI
     */
    HBufC* iAuxBuffer2;    
         
    };



#endif // CSimplePluginXdmUtils_H
