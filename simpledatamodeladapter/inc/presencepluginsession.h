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


#ifndef CPRESENCEPLUGINSESSION_H
#define CPRESENCEPLUGINSESSION_H


#include <e32base.h>
#include <msimpleconnectionobserver.h>
#include "mpresencepluginconnectionobs.h"
#include "presencelogger.h"

//FORWARD
class MXIMPServiceInfo;
class MPresencePluginConnectionObs;

/**
 * CPrecensePluginSession
 *
 * presence Engine Connection
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CPresencePluginSession ): public CActive,
    public MSimpleConnectionObserver                       
    {
    public:

    	/**
         * Current operation
         */
        enum TPluginSessionOperation
            {
            ENoOperation,
            EInitializeXdm,
            };  
        
        /**
         * Two-phased constructor.
         *
         * @param aService, XIMP Service info
         * @param aObs, Connection observer
         */
        static CPresencePluginSession* NewL( 
            const MXIMPServiceInfo& aService,
            MPresencePluginConnectionObs& aObs );

        /**
         * Standard C++ destructor
         */
        virtual ~CPresencePluginSession();

    public: //New function
    
         TInt32 GetPresenceSetId( ) const;
        
        /**
         * MSimpleConnection accessor
         *
         * @since S60 3.2
         * @return MSImpleConnection instance
         */
        MSimpleConnection* SimpleConnection();
         
        /**
         * OpenSessionL
         *
         * @since S60 3.2         
         * Opens the SIP connection (registers when needed)
         * @param none
         * @return none
         */
        void OpenSessionL();
        
        /**
         * XDMSettings accessor
         *
         * @since S60 3.2
         * @param none
         * @return XDM Settings ID
         */
        TInt XdmSettingsId();
         
        /**
         * Current registered SIP entity
         *
         * @since S60 3.2
         * @param none
         * @return TPrtC8, current sip presentity
         */ 
        TPtrC8 CurrentSipPresentity();
        
        /**
         * Domain syntax for current settings
         *
         * @since S60 3.2
         * @param none
         * @return TPrtC16, current domain
         */    
        TPtrC16 CurrentDomain();
        
        /**
         * Close Connection
         *
         * @since S60 3.2
         * @param none
         * @return none
         */ 
        void CloseConnection();
        
        /**
         * Connection status
         *
         * @since S60 3.2
         * @param none
         * @return TBool, connection TRUE/FALSE
         */
        TBool ConnectionStatus();
        
        /**
        * Connection status
        *
        * @since S60 3.2
        * @param none
        * @return TBool, connection TRUE/FALSE
        */
        TBool IsXdmLocalmode();
        
        /**
         * Check XDM settings valid
         *
         * @since S60 5.0
         * @param aServiceId, service if owning this session
         * @return none
         */
        void CheckXDMSettingsL( TUint aServiceId );
        
        /**
         * Accessor to valid XdmUtils
         *
         * @since S60 3.2
         * @param none
         * @return XdmUtils instance
         */
        CPresencePluginXdmUtils* XdmUtilsL();
        
        /**
         * Returns service id owning this session.
         *
         * @since S60 5.0
         * @param none
         * @return TInt, service id
         */        
        TInt& ServiceId();
 
    private:
    
        /**
         * Standard C++ constructor
         * @param aObs, connection observer.
         */ 
        CPresencePluginSession( 
            MPresencePluginConnectionObs& aObs );

        /**
         * Performs the 2nd phase of construction.
         *
         * @param aService, XIMP service info
         */ 
        void ConstructL( const MXIMPServiceInfo& aService );

    public:// from base class MSimpleConnectionObserver

        /**
         * Defined in a base class
         */
        void ConnectionStatusL( 
            MSimpleConnection::TSimpleState aState );

        /**
         * Defined in a base class
         */
        void RequestCompleteL( TInt aOpId, TInt aStatus );
        
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
         * XIMP Plugin connection observer
         * Own.         
         */
        MPresencePluginConnectionObs& iObs;
        
        /**
         * Simple Engine connection.
         * Own.
         */
        MSimpleConnection* iConnection;
        
        /**
         * Current Simple Engine operation id
         * Own.
         */
        TInt iOpId;
        
        /**
         * Request type
         * Own.
         */
        MPresencePluginConnectionObs::TReqType iType;

        /**
         * presence settings Id
         * Own.
         */
        TInt iPresSettingId;
        
        /**
         * XDM Settings id
         * Own.
         */
        TInt iXdmSetting;

        /**
         * Domain syntax for current settings
         * Own,
         */
        HBufC16* iDomain;
        
        /**
         * Current User ID, User's SIP identity
         * Own.
         */
        HBufC8* iUserId8;
        
        /**
         * Whether conncted to network
         * Own.
         */
        TBool iSipConnected;
        
        /**
         * Whether conncted to network
         * Own.
         */
        TBool iXdmConnected;
        
        /**
         * Xdm local mode
         * Own.
         */
        TBool iXdmLocalMode;
        
        /**
         * XDM utils
         * Own.
         */
        CPresencePluginXdmUtils* iXdmUtils;
        
        /**
         * Session operation.
         */        
        TPluginSessionOperation iOperation; 
        
        /**
         * Service id owning this session.
         */                
        TInt     iServiceId;

        SIMPLE_UNIT_TEST( T_CPresencePluginSession )
        SIMPLE_UNIT_TEST( T_CPresencePluginConnection )
    };

#endif // CPRESENCEPLUGINSESSION_H