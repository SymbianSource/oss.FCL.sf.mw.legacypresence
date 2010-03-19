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


#ifndef CPRESENCEPLUGINXDMPRESRULES_H
#define CPRESENCEPLUGINXDMPRESRULES_H

#include <e32base.h>
#include <ximpbase.h>

#include "presenceplugincommon.h"
#include "presencelogger.h"

class CXdmEngine;
class CXdmDocument;
class CXdmDocumentNode;
class TRequestStatus;
class MPresRulesAsyncHandler;

/**
 * CPresencePluginXdmPresRules
 *
 * Simple Engine Connection
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CPresencePluginXdmPresRules ): public CActive
    {
    public: // Constructor and destructor
         
        /**
         * XDM presrules state
         */
        enum TPluginPresRulesXdmState
            {
 			EStateIdle,
            EGetXdmRules,
            EUpdateXdmRules,
            ECreateXdmPresrules
            };
            
        /**
         * Operation state for rules update
         */     
        enum TPluginPresRulesUpdateState
            {
            EStateNoOperation,
            EStateAddToWhiteList,
            EStateAddToBlockList,
            EStateRemoveFromWhiteList,
            EStateRemoveFromBlackList,
            EStateInitializeRules
            };
            
        /**
         * Current operation
         */
        enum TPluginPresRulesXdmOperation
            {
            ENoOperation,
            ECancelDocument,
            EXdmInit,   
            EXdmDeleteAll,
            EXdmCancel,
            EXdmGetRulesFromServer,
            EXdmInitialPresDoc,
            EXdmUpdateRules,
            EXdmUpdateRulesFromServer
            };
        
        /**
         * Two-phased constructor.
         *         
         * @param aXmdId xdm settings id
         * @param aLocalMode, local or server mode TRUE/FALSE
         */       
        static CPresencePluginXdmPresRules* NewL( const TInt aXmdId, TBool  );

        /**
         * Two-phased constructor.
         *         
         * @param aXmdId xdm settings id
         * @param aLocalMode, local or server mode TRUE/FALSE
         */ 
        static CPresencePluginXdmPresRules* NewLC( 
        		const TInt aSettingId,
        		TBool aLocalMode );

        /**
         * Standard C++ destructor
         */ 
        virtual ~CPresencePluginXdmPresRules( );
    
    public: // Own functions
        
        /**
         * Updates XDM rules into server
         *
         * @since S60 3.2
         * @param aHandler, MPresRulesAssyncHandler
         * @return TInt, Error code
         */
        TInt UpdateXdmRulesL( MPresRulesAsyncHandler* const aHandler );
        
        /** 
         * Add white list tag to presrules document
         *
         * @since S60 3.2
         * @param aRoot, xml document root
         * @return none
         */
        void AddWhiteListL( CXdmDocumentNode* aRoot );
         
        /** 
         * Add black list tag to presrules document
         *
         * @since S60 3.2
         * @param aRoot, xml document root
         * @return none
         */
        void AddBlackListL( CXdmDocumentNode* aRoot );
         
        /**
         * Get presence rules from server
         *
         * @since S60 3.2
         * @param aHandler, PresRules assync handler
         * @return TInt, error code
         */
        TInt GetXdmRulesL( MPresRulesAsyncHandler* const aHandler );
        
        /**
         * Making initial presence rules document
         *
         * @since S60 3.2
         * @param aHandler, PresRules assync handler
         * @return none
         */
        void MakeInitialRulesDocumentL( 
        		MPresRulesAsyncHandler* const aHandler );
            
        /**
         * Remove entiry from blocked rules
         *
         * @since S60 3.2
         * @param aUri, entity url
         * @aHandler, PresRules assync handler 
         * @return none
         */   
        void RemoveEntityBlockedRulesL( 
            const TDesC&  aUri,
            MPresRulesAsyncHandler* const aHandler );
            
        /**
         * Remove entity from presrules white list
         *
         * @since S60 3.2
         * @param aUri, entity url
         * @aHandler, PresRules assync handler 
         * @return none
         */
        void RemoveEntityFromWhiteListL( 
            const TDesC&  aUri,
            MPresRulesAsyncHandler* const aHandler );
                
        /**
         * Add entity to presrules white list
         *
         * @since S60 3.2
         * @param aUri, entity url
         * @aHandler, PresRules assync handler 
         * @return none
         */
        void AddEntityToWhiteListL(
            const TDesC&  aUri,
            MPresRulesAsyncHandler* const aHandler );

        /**
         * Add entity to blocked rules
         *
         * @since S60 3.2
         * @param aUri, entity url
         * @aHandler, PresRules assync handler 
         * @return none
         */
        void AddEntityToBlockedRulesL( 
            const TDesC&  aUri,
            MPresRulesAsyncHandler* const aHandler );
                
        /**
         * Remove one from rule list
         *
         * @since S60 3.2
         * @param aRule, presence rule name
         * @param aUri, entity url 
         * @return none
         */ 
        void RemoveOneFromRuleL(
            const TDesC& aRule,
            const TDesC&  aUri );
        
        /**
         * Add one to rule list
         *
         * @since S60 3.2
         * @param aRule, presence rule name
         * @param aUri, entity url 
         * @return none
         */ 
        void DoOneAddUserL(
            const TDesC& aList,
            const TDesC&  aUri );
          
        /**
         * Update presence document from server
         *
         * @since S60 3.2
         * @param none
         * @return TInt, error code
         */
        TInt UpdateFromServerL( MPresRulesAsyncHandler* const aHandler );
  
    private:
        
        /**
         * Standard C++ constructor
         * @param aXmdId, XDM settings id
         */  
        CPresencePluginXdmPresRules( const TInt aXmdId );

        /**
         * Performs the 2nd phase of construction.
         *
         * @param aXmdId, xdm settings id
         * @param aLocalMode, clinet localmode state
         */   
        void ConstructL( const TInt aXmdId, TBool aLocalMode );
    
    private: // own functions
    
        /**
         * GetRulesL
         *
         * @since S60 3.2
         * @param aRuleArray, rule array
         * @return TBool, rules found
         */        
        CXdmDocumentNode* GetRulesL( const TDesC& aRule,
            RPointerArray<CXdmDocumentNode>& aRuleArray );
    
        /**
         * Remove rule group from pres-rules document
         *
         * @since S60 3.2
         * @param aRule, presence rule name
         * @return none
         */
        void RemoveRuleGroupL( const TDesC& aRule );
        
        /**
         * Search list under parent
         *
         * @since S60 3.2        
         * @param aParent. document node
         * @param aName, name of search
         * @return none
         */        
        CXdmDocumentNode* SearchListUnderParentL(
            CXdmDocumentNode* aParent,
            const TDesC& aName );
    
        /**
         * Create root if need
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
        void CreateRootIfNeededL();
       
        static void ResetAndDestroy1( TAny* aPointerArray );
        static void ResetAndDestroy2( TAny* aPointerArray );
        
        /**
         * Find rule from presrules xml
         *
         * @since S60 3.2
         * @param aRule, rule name
         * @return TBool
         */
        TBool FindRuleL( const TDesC& aRule, TBool aCreateRule );
               
        /**
         * Check if one already exist
         *
         * @since S60 3.2
         * @param aList, listname
         * @param aUri, entity url
         * @return TBool
         */
        TBool CheckIfOneExistL(
            const TDesC& aRule,
            const TDesC& aUri );
                
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

    private: // Data
        
        /**
         * XDM Engine
         * Own.
         */
        CXdmEngine* iXdmEngine;
        
        /**
         * XDM Settings id
         * Own.        
         */
        TInt iSettingsId;
        
        /**
         * Presence rules document
         * Own
         */
        CXdmDocument* iPresRulesDoc;
        
        /**
         * Current state
         * Own.
         */
        TPluginPresRulesXdmState iXdmState;
        
        /**
         * TPluginPresRulesUpdateState
         * Own.
         */
        TPluginPresRulesUpdateState iRulesUpdateState;
         
        /**
         * client statutus.
         * Not own.
         */
        TRequestStatus* iClientStatus;
        
        /**
         * Whether Presence XDM is up-to-date
         * Own.
         */
        TBool iPresXdmOk;
       
        /**
         * Entity uri
         * own
         */
        HBufC*  iEntityUri;
        
        /**
         * Async request
         * Own. 
         */
        TPluginPresRulesXdmOperation iAsyncReq;
        
        /**
         * Async handler
         * Own.
         */
        MPresRulesAsyncHandler* iAsyncHandler;
        
        SIMPLE_UNIT_TEST( T_CPresencePluginXdmPresRules )
    };

#endif // CPRESENCEPLUGINXDMPRESRULES_H
