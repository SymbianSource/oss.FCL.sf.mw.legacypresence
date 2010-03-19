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


#ifndef CPRESENCEPLUGINXDMUTILS_H
#define CPRESENCEPLUGINXDMUTILS_H

#include <e32base.h>
#include <ximpbase.h>
#include <protocolpresenceauthorization.h>
#include "presenceplugincommon.h"
#include "mpresrulesasynchandler.h"
#include "presencelogger.h"

class CPresencePluginXdmPresRules;
class TXIMPRequestId;
class MXIMPIdentity;
class MXIMPObjectCollection;
class CXdmEngine;
class CXdmDocument;
class CXdmDocumentNode;
class TRequestStatus;
class MPresencePluginConnectionObs;

/**
 * CPresencePluginXdmUtils
 *
 * Simple Engine Connection
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CPresencePluginXdmUtils ) : public CActive,
    public MPresRulesAsyncHandler
    {
    public: // Constructor and destructor

        /**
         * Active object asynch states
         */
        enum TPluginXdmState
            {
            EStateIdle,
            /** Get XDM lists 1*/
            EGetXdmLists,
            /** Create XDM lists */
            ECreateXdmLists,
            /** Get XDM lists only, no rules later */
            EGetXdmListsOnly,
            /** Create XDM lists only, no rules later */
            ECreateXdmListsOnly,        
            /** Get RLS service */
            EGetRlsServices,
            /** Create RLS service */
            ECreateRlsServices,
            /** Update RLS service */
            EUpdateRlsServices,
            /** Get rules from server for GrantPresenceForPresentity */
            EGetXdmRules,
            /** Update rules to server for GrantPresenceForPresentity */
            EUpdateXdmRules,
            /** Update XDM List */
            EUpdateXdmList,
            /** Cancel XDM document operation */
            ECancelDocument,
            /** Get Block list */
            EGetXdmBlockList,
            /** Create presence rules */
            ECreateXdmRules,
            /** Subscripe block list */
            ESubsBlockList,
            EGetResourceListFromServer
            };

        /**
         * Current operation
         */
        enum TPluginXdmOperation
            {
            ENoOperation,
            EXdmInit,   
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
            EXdmCommitXdm,
            EXdmCommitRls,
            EXdmGetBuddyList,
            EXdmAddUserToRules,
            EXdmRemoveUserFromRules
            };

        /**
         * Operation state for rules update
         */
        enum TPluginPresRulesState
            {
            EStateNoOperation,
            EStateAddToWhiteList,
            EStateAddToBlockList,
            EStateRemoveFromWhiteList,
            EStateRemoveFromBlackList,
            EStateInitializeRules
            };
            
        /**
         * NewL
         * @param aObs callback for complete requests
         * @param aXdmId XDm settings id
         */
        static CPresencePluginXdmUtils* NewL(
            MPresencePluginConnectionObs& aObs,
            TInt aXmdId,
            TBool aLocalMode );

        virtual ~CPresencePluginXdmUtils();

        /**
         * Initialize XDMS and Presence XDM structures in network
         *
         * @since S60 3.2
         * @param aStatus Request status of the caller
         */
        void InitializeXdmL( TRequestStatus& aStatus );
        
        /**
         * Initialize XDMS in network
         *
         * @since S60 3.2
         * @param aStatus Request status of the caller
         */
        void InitializeXdmsOnlyL( TRequestStatus& aStatus );    

        /**
         * Initialize Presence rules document only
         *
         * @since S60 3.2        
         * @param aStatus Request status of the caller
         */
        void InitializePresRulesL( );

        /**
         * Add entity to granted list
         *
         * @since S60 3.2        
         * @param aUri, entity URL
         * @param aStatus, request status
         * @return none
         */
        void AddEntityToGrantedL( 
            const TDesC&  aUri,
            TRequestStatus& aStatus );
         
        /**
         * Remove entity from granted list
         *
         * @since S60 3.2        
         * @param aUri, entity URL
         * @param aStatus, request status
         * @return none
         */
        void RemoveEntityFromGrantedL( 
            const TDesC&  aUri,
            TRequestStatus& aStatus );

        /**
         * Remove entity from blocked list
         *
         * @since S60 3.2        
         * @param aName, entity URL
         * @param aStatus, request status
         * @return none
         */
        void RemoveEntityFromBlockedL( 
            const TDesC&  aName,
            TRequestStatus& aStatus );
        
        /**
         * Add entity to blocked list
         *
         * @since S60 3.2        
         * @param aUri, entity URL
         * @param aStatus, request status
         * @return none
         */
        void AddEntityToBlockedL( 
            const TDesC&  aUri,
            TRequestStatus& aStatus );
    
        /**
         * Get (subscribe) Block List
         *
         * @since S60 3.2        
         * @param aMembers, member list array
         * @return none
         */
        void SubscribeBlockListL( RPointerArray<MXIMPIdentity>& aMembers );

        /**
         * Get (subscribe) Buddy List
         *
         * @since S60 3.2        
         * @param aMembers, member list array
         * @return none
         */
        void SubscribeBuddyListL( RPointerArray<MXIMPIdentity>& aMembers );
        
        /**
         * GetUserListsL
         *
         * @since S60 3.2
         * @param aList, List name
         * @param aColl, XIMP object collection        
         */
        void GetEntitiesInListL(
            const TDesC&  aList, MXIMPObjectCollection& aColl );
        
        /**
         * Add identity to virtual list
         *
         * @since S60 3.2
         * @param aList, List name
         */
        void AddIdentityToVirtualListL(
            const TDesC&  aList );
        
       /**
        * CheckXdmDoc
        *
        * @since S60 3.2
        * @param none
        * @return TBool, state of xdm documents        
        */    
       TBool CheckXdmDoc();
    
    public: //From assync
        
        /**
         * Defined in a base class
         */
        void HandlePresUpdateDocumentL( TInt aErrorCode );
        
        /**
         * Defined in a base class
         */
        void HandlePresUpdateCancelL( TInt aErrorCode );
   
    private:
    
        /**
         * Standard C++ constructor
         *
         * @param aObs, plugin connection observer
         * @param aXmdId, XDM settings id 
         */ 
        CPresencePluginXdmUtils( 
        		MPresencePluginConnectionObs& aObs,
        		TInt aXmdId,
        		TBool aLocalmode );

        /**
         * Performs the 2nd phase of construction.
         */ 
        void ConstructL( );

    protected:// from base class CActive

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
         * DoUpdateXdmLists
         *
         * @since S60 3.2
         * @param aMyStatus, RunL status
         * @return aOrigState, xdm state
         */    
        void DoUpdateXdmListsL( TInt aMyStatus,
            TPluginXdmState aOrigState );
                
        /**
         * DoGetXdmLists
         *
         * @since S60 3.2
         * @param aMyStatus, RunL status
         * @return aOrigState, xdm state
         */    
        void DoGetXdmListsL( TInt aMyStatus,
            TPluginXdmState aOrigState );

        /**
         * DoCreateXdmLists
         *
         * @since S60 3.2
         * @param aMyStatus, RunL status
         * @return aOrigState, xdm state
         */    
        void DoCreateXdmListsL( TInt aMyStatus,
            TPluginXdmState aOrigState,
            TBool aOnlyResourceList );
               
        /**
         * Search list under parent
         *
         * @since S60 3.2        
         * @param none
         * @return none
         */
        CXdmDocumentNode* SearchListUnderParentL(
            CXdmDocumentNode* aParent,
            const TDesC& aName );

        /**
         * Complete client request
         *
         * @since S60 3.2
         * @param aStatus, client status
         * @return none
         */    
        void CompleteClientReq( TInt aStatus );

        /**
         * Get XDM lists
         *
         * @since S60 3.2
         * @param aCreateRules, create new rule document
         * @param aLocalMode, use localmode
         * @return none
         */
        void GetXdmListsL( TBool aCreateRules, TBool aLocalMode );

        /**
         * Create XDM list document model
         *
         * @since S60 3.2
         * @param aCreateRules, create new rule document
         * @return none
         */
        void CreateXdmListsL( TBool aCreateRules );

        /**
         * Update document to server
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void UpdateXdmToServerL();
          
        /**
         * Updates XDMS reource lists into server
         *
         * @since S60 3.2
         * @param none
         * @return none
         */  
        void UpdateXdmsL();
        
        /**
         * Updates XDM rules
         *
         * @since S60 3.2
         * @param none
         * @return none
         */        
        void UpdateXdmRulesL();
        
        /**
         * Get XDM rules
         *
         * @since S60 3.2
         * @param none
         * @return none
         */         
        void GetXdmRulesL();
                 
        /**
         * Handle presence document 
         *
         * @since S60 3.2        
         * @param aList, list name
         * @param aUser, entity name
         * @return none
         */
        void DoHandlePresUpdateDocumentL( TInt aErrorCode );

        /**
         * Add User to resource list
         *
         * @since S60 3.2
         * @param aList, list name
         * @param aName, url         
         */
        void DoAddUserToListL(
            const TDesC& aList,
            const TDesC&  aName );
        
        /**
         * Remove user from list
         *
         * @since S60 3.2
         * @param aList, list name
         * @param aUser, entity name
         * @return none
         */
        void DoRemoveUserFromListL(
            const TDesC& aList,
            const TDesC&  aName );

        /**
         * Get member list
         *
         * @since S60 3.2
         * @param aList, list name
         * @param aMembers, member array
         * @return none
         */
        void DoGetListMembersL(
            const TDesC& aList,
            RPointerArray<MXIMPIdentity>& aMembers  );

        /**
         * Get buddy list
         *
         * @since S60 3.2
         * @param aList, list name
         * @return XDM document
         */
        CXdmDocumentNode* DoGetBuddyListL( const TDesC& aParent);

        /**
         * Get buddy list
         *
         * @since S60 3.2
         * @param aList, list name
         * @parma aBuddyList, xdm document node
         * @return XDM document
         */
        CXdmDocumentNode* DoGetUserListL(
            const TDesC& aList,
            CXdmDocumentNode* aBuddyList );
        
        /**
         * Check XDM errors
         *
         * @since S60 3.2
         * @param aMyStatus, XDM error code
         * @return errorcode
         */    
        TInt CheckIfErrorL( TInt aMyStatus );

        /**
         * Check if entity already exist
         *
         * @since S60 3.2
         * @param aList, listname
         * @param aUri, entity url
         * @return TBool
         */
        TBool CheckIfEnityExistL( const TDesC& aList, const TDesC&  aUri );
        
        /**
         * Update document from server
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void UpdateFromServerL();
        
    private: // Data

        /**
         * ximp Plugin connection observer
         * Own.
         */
        MPresencePluginConnectionObs& iConnObs;

        /**
         * XDM Settings id
         * Own.
         */
        TInt iSettingsId;

        /**
         * ximp request id
         * Own.         
         */
        TXIMPRequestId iximpId;

        /**
         * ximp request id for own requests
         * Own.         
         */
        TXIMPRequestId iximpIdOwn;

        /**
         * Simple Engine request id
         * Own.         
         */
        TInt iSimpleId;

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
         * Own.         
         */
        TPluginXdmState iXdmState;
         
        /**
         * Current operation
         * Own.         
         */
        TPluginXdmOperation iOperation;

        /**
         * Whether Presence XDM is up-to-date
         * Own.         
         */
        TBool iPresXdmOk;

        /**
         * Whether XDM is up-to-date
         * Own.         
         */
        TBool iXdmOk;

        /**
         * client statutus.
         * Not own.
         */
        TRequestStatus* iClientStatus;
           
        /**
         * XDM presence rules handling
         * own.
         */
        CPresencePluginXdmPresRules*  iXdmPresRules;
        
        /**
         * Entity uri
         * Own.         
         */
        HBufC* iEntityUri;
        
        /**
         * TPluginPresRulesUpdateState
         * Own.         
         */
        TPluginPresRulesState iRulesUpdateState;
        
        /**
         * XDM localmode
         * Own.         
         */
        TBool iLocalMode;
        
        SIMPLE_UNIT_TEST( T_CPresencePluginGroups )
        SIMPLE_UNIT_TEST( T_CPresencePluginWatcher )
        SIMPLE_UNIT_TEST( T_CPresencePluginAuthorization )
        SIMPLE_UNIT_TEST( T_CPresencePluginXdmUtils )
    };

#endif // CPRESENCEPLUGINXDMUTILS_H
