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


#ifndef PRESENCEPLUGINCONNECTION_H
#define PRESENCEPLUGINCONNECTION_H

#include <e32base.h>
#include <ximpprotocolconnection.h>
#include <protocolpresencefeatures.h>
//Connection obs
#include "mpresencepluginconnectionobs.h"
#include "simpleimfeatures.h"
#include "presencelogger.h"

//FORWARD
class MXIMPServiceInfo;
class MXIMPObjectFactory; 
class MXIMPProtocolConnectionHost;
class MProtocolPresenceWatching;
class MProtocolPresencePublishing;
class MProtocolPresentityGroups;
class MProtocolPresenceAuthorization;
class MProtocolPresenceFeatures;
class MProtocolPresenceDataHost;
class CPresencePluginSession;
class CPresencePluginWinfo;
class CPresencePluginWatcher;
class CPresencePluginAuthorization;
class CPresencePluginPublisher;
class CPresencePluginXdmUtils;
class CPresencePluginGroups;
class CPresencePluginVirtualGroup;
class CPresencePluginData;
class CPresenceConnectionInfo;
class MProtocolImFeatures;


/**
 * CPrecensePluginSession
 *
 * presence Engine Connection
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS(CPresencePluginConnection) : public CActive,
    public MXIMPProtocolConnection,
    public MProtocolPresenceFeatures,
    public MPresencePluginConnectionObs
    {
    public:
    
    	enum TConnectionStatus
            {
            /** undefined */
            ENotActive = 0,
            /** Connection initializing */
            EActive,
            /** Connection close */
            ETerminated
            };

        /**
         * Two-phased constructor.
         *
         * @param aServiceInfo, XIMP service info
         * @param aClientCtxInfo, XIMP contex client info
         */
        static CPresencePluginConnection* NewL( 
            const MXIMPServiceInfo& aServiceInfo,
            const MXIMPContextClientInfo& aClientCtxInfo );
        
        /**
         * Two-phased constructor loaded.
         *
         * @param aServiceInfo, XIMP service info
         * @param aClientCtxInfo, XIMP contex client info
         */
        static CPresencePluginConnection* NewL( 
            const MXIMPServiceInfo& aServiceInfo,
            const MXIMPContextClientInfo& aClientCtxInfo,
            const TDesC8& aETag );
        
        /**
         * Standard C++ destructor
         */ 
        ~CPresencePluginConnection();

        /**
         * Grand list subscribe state
         *
         * @since S60 3.2
         * @param none
         * @return TBool grand list subscribe state
         */
        TBool GrandListState( );

        /**
         * Set connection info array
         *
         * @since S60 3.2
         * @param connection info
         * @return none
         */
        void SetConnectionArray( CPresenceConnectionInfo* aArray );

        /**
         * Get connection info array
         *
         * @since S60 3.2
         * @param none
         * @return return info 
         */
        CPresenceConnectionInfo* GetConnectionArray( );


        /**
         * Gets protocol interface. In error cases returns NULL
         *
         * @since S60 5.0
         * @param none
         * @return protocol interface 
         */        
        TAny* GetProtocolInterface( TInt aInterfaceId );
        
    private:
    
        /**
         * Standard C++ constructor
         */         
        CPresencePluginConnection( );

        /**
         * Performs the 2nd phase of construction.
         *
         * @param aService, XIMP service info
         * @param aClientCtxInfo, XIMP contex client info
         */  
        void ConstructL(
            const MXIMPServiceInfo& aService,
            const MXIMPContextClientInfo& aClientCtxInfo );
        
        /**
         * Performs the 2nd phase of construction. Overload
         *
         * @param aService, XIMP service info
         * @param aClientCtxInfo, XIMP contex client info
         */  
        void ConstructL(
            const MXIMPServiceInfo& aService,
            const MXIMPContextClientInfo& aClientCtxInfo,
            const TDesC8& aETag );
        
    private: //Own private functions
    	
    	/**
         * DeRegister
         *
         * @since S60 3.2
         * @param none
         * @return none
         */
    	void DeRegister( );
    	
        /**
          * CreateImFeaturesPlugin
          *
          * @since S60 5.0
          * @param none
          * @return none
          */
    	void CreateImFeaturesPluginL();
    	
        /**
          * DeleteImFeaturesPlugin
          *
          * @since S60 5.0
          * @param none
          * @return none
          */
    	void DeleteImFeaturesPlugin();
    	
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
    
    public: // from base class MPrFwBase
        
        TInt32 GetPresenceSetId() const;
        
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
        
    public:// from base class MPresencePluginConnectionObs
    
        /**
         * Return subscribed buddy virtual group
         *
         * @since S60 3.2
         * @param none
         * @return CPresencePluginVirtualGroup, virtual group
         */
        CPresencePluginVirtualGroup* SubscribedContacts();
        
        /**
         * Get Stop Publish state
         *
         * @since S60 3.2
         * @param none
         * @return TBOOL, Stop Publish state
         */
        TBool GetStopPublishState();
        
        /**
         * Is stop publish called
         *
         * @since S60 3.2
         * @param none
         * @return TBOOL, Stop Publish
         */
        TBool IsStopPublishCalled();
        
        /**
         * Set Stop Publish state
         *
         * @since S60 3.2
         * @param aState, stop publish state TRUE/FALSE
         * @return none
         */
        void SetStopPublishState( TBool aState );
    
        /**
         * Get Tuple id
         *
         * @since S60 3.2
         * @param none
         * @return TInt, tupleid
         */
        TInt GetTupleId() const;
        
        /**
         * Set Tuple id
         *
         * @since S60 3.2
         * @param aTupleId, generated tupleid
         * @return none
         */
        void SetTupleId( const TInt aTupleId );
        
        /**
         * Get ETag
         *
         * @since S60 3.2
         * @param none
         * @return TDesC8, Etag value
         */
        const TDesC8& GetETag() const;
   
        /**
         * Set ETag
         *
         * @since S60 3.2
         * @param aEtag, Etag value
         * @return none
         */
        void SetETag( const TDesC8& aETag );
        
        /**
         * Defined in a base class
         */
        void CompleteReq( TReqType aType, TInt aStatus );

        /**
         * Defined in a base class
         */
        void CompleteReq( TXIMPRequestId aReqId, TInt aStatus );

        /**
         * Defined in a base class
         */
        MXIMPObjectFactory& ObjectFactory();
    
        /**
         * Defined in a base class
         */    
        MPresenceObjectFactory& PresenceObjectFactoryOwn();
            
        /**
         * Defined in a base class
         */
        CPresencePluginWinfo* WinfoHandlerL();   
    
        /**
         * Defined in a base class
         */
        CPresencePluginWatcher* WatcherHandlerL();
        
        /**
         * Defined in a base class
         */    
        void WinfoTerminatedL( TInt aReason ); 
    
        /**
         * Defined in a base class
         */     
        void WinfoNotification( MSimpleWinfo& aWinfo );      
 
        /**
         * Defined in a base class
         */ 
        TPtrC8 CurrentSipPresentity8();
        
        /**
         * Defined in a base class
         */
        CPresencePluginXdmUtils* XdmUtilsL();
        
        /**
         * Defined in a base class
         */
        TBool GetSessionStatus( );
        
        /** 
         * Termiante connection
         */
        void TerminateConnectionL( );
        
        /** 
         * Returns groups api implementation. Leaves if 
         * pointer not available.
         */        
        CPresencePluginGroups& GroupsL();
        
        /** 
         * Returns service id
         */                
        TInt& ServiceId();                
                         
    public: // from MXIMPProtocolConnection
               
        /**
         * Defined in a base class
         */
        MProtocolPresenceDataHost& ProtocolPresenceHost();
        
        /**
         * Defined in a base class
         */
        MProtocolPresenceFeatures& ProtocolPresenceFeatures();
        
        /**
         * PrimeHost
         *
         * @since S60 3.2
         * @param [in] aHost
         * Reference to host interface, what this MXIMPProtocolConnection
         * instance must use to communicate back to Presence Framework
         * direction.
         */
        void PrimeHost( MXIMPProtocolConnectionHost& aHost );

        /**
         * OpenSessionL
         *
         * @since S60 3.2 
         * @param [in] aReqId
         * Request ID identifying the request. Presence protocol
         * connection implementation must cache here given ID and
         * use it when completing the request later through the
         * MXIMPProtocolConnectionHost::HandleRequestCompleted().
         */
        void OpenSessionL( const MXIMPContextClientInfo& aClientCtxInfo,
            TXIMPRequestId aReqId );
        
        /**
         * OpenSessionL
         *
         * @since S60 3.2 
         * @param [in] aReqId
         * Request ID identifying the request. Presence protocol
         * connection implementation must cache here given ID and
         * use it when completing the request later through the
         * MXIMPProtocolConnectionHost::HandleRequestCompleted().
         */
        void OpenSessionL( const TInt& aSettingsId,
            TXIMPRequestId aReqId );
         
        /**
         * CloseSession
         *
         * @since S60 3.2
         * @param [in] aReqId
         * Request ID identifying the request. Presence protocol
         * connection implementation must cache here given ID and
         * use it when completing the request later through the
         * MXIMPProtocolConnectionHost::HandleRequestCompleted().
         */               
        void CloseSession( const MXIMPContextClientInfo& aContextClient,
            TXIMPRequestId aReqId );
            
        /**
         * GetSupportedFeaturesL
         * 
         * @since S60 3.2
         * @param [out] aFeatures
         * Return parameter where to append supported
         * presence protocol connection features.
         * Feature IDs are defined in NXIMPFeature namespace.
         */               
        void GetSupportedFeaturesL( CDesC8Array& aFeatures ) const;

        /**
         * PresenceWatching
         *
         * @since S60 3.2 
         * @return Requested interface.
         * interface ownership is not returned caller.
         */
        MProtocolPresenceWatching& PresenceWatching();

        /**
         * PresencePublishing
         *
         * @since S60 3.2 
         * @return Requested interface.
         * interface ownership is not returned caller.
         */
        MProtocolPresencePublishing& PresencePublishing(); 

        /**
         * PresentityGroups
         * 
         * @since S60 3.2 
         * @return Requested interface.
         * interface ownership is not returned caller.
         */
        MProtocolPresentityGroups& PresentityGroups();

        /**
         * PresenceAuthorization
         * 
         * @since S60 3.2
         * @return Requested interface.
         * interface ownership is not returned caller.
         */
        MProtocolPresenceAuthorization& PresenceAuthorization();
        
        /**
         * PresenceAuthorization
         * 
         * @since S60 3.2
         * @return Requested interface as own type
         * interface ownership is not returned caller.
         */        
        CPresencePluginAuthorization& InternalPresenceAuthorization();

        /**
         * Defined in a base class
         */
        MXIMPProtocolConnectionHost* Host();

        /**
         * Defined in a base class
         */
        TPtrC16 CurrentDomain();
          
        /**
         * Defined in a base class
         */
        MXIMPServiceInfo& ServiceInfo();
                        
    private:  //data
     
        /**
         * Presence Engine session
         * Own.
         */
        CPresencePluginSession* iSession;
        
        /**
         * CPresencePluginWinfo
         * Own.
         */    
        CPresencePluginWinfo* iPluginWinfo;
        
        /**
         * CPresencePluginWatcher
         * Own.
         */
        CPresencePluginWatcher* iWatcher;
        
        /**
         * current request id
         * Own
         */
        TXIMPRequestId iXIMPId;  
        
        /**
         * Host for connection.
         * Not own.
         */
        MXIMPProtocolConnectionHost*  iConnectionHost;  
        
        /**
         * MXIMPProtocolPresenceauthorization implementation.
         * Own.
         */
        CPresencePluginAuthorization* iAuth;
        
        /**
         * CSimplePluginPublisher
         * Own.
         */
        CPresencePluginPublisher* iPublisher;
        
        /**
         * CSimplePluginGroups
         * Own.
         */
        CPresencePluginGroups* iGroups;
        
        /**
         * CPresencePluginImFeatures
         * Own.
         */
        CSimpleImFeatures* iImFeatures;
        
        /**
         * Current user SIP identity
         * Own.
         */
        HBufC16* iSipPresentity; 
        
        /**
         * XDM utils
         * Own.
         */
        CPresencePluginXdmUtils* iXdmUtils;
        
        /**
         * TupleId
         * Own.
         */
        TInt iOwnTupleId;
         
        /**
         * Document Etag
         * Own.
         */
        HBufC8* iETag;

        /**
         * Virtual groups
         * Own
         */
        CPresencePluginVirtualGroup* iSubscribedBuddys;   

        /**
         * Plugin Data
         * Own.
         */
        CPresencePluginData* iPresenceData;
        
        /**
         * Stop publish state
         * Own
         */
        TBool iStopPublishState;
        
        /**
         * Grant list subscribe state
         * Own
         */
        TBool iGrantListSubs;
        
        /**
         * Is stop publish called
         * Own
         */
        TBool iStopPublishCall;
        
        /**
         * Terminated state called
         * Own.
         */
        TBool iTerminatedCall;
        
        /**
         * Connection Array instance
         * Own.
         */
        CPresenceConnectionInfo* iConnectionArray;
        
        SIMPLE_UNIT_TEST( T_CPresencePluginConnection )
        SIMPLE_UNIT_TEST( T_SIMPLEDataModelAdapter )
    };

#endif // PRESENCEPLUGINCONNECTION_H