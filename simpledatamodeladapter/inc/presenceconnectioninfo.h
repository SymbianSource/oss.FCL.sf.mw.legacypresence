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


#ifndef CPRESENCECONNECTIONARRAY_H
#define CPRESENCECONNECTIONARRAY_H

#include <e32base.h>
#include "presencelogger.h"
#include "presenceplugincommon.h"
#include "presencepluginconnection.h"

class MXIMPProtocolConnection;

/**
 * CPresencePlugin
 *
 * Implementation of CXIMPProtocolPluginBase
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */


NONSHARABLE_CLASS( CPresenceConnectionInfo ) : public CBase
    {
    public:
        
        /**
        * Two-phased constructor.
        *
        * @param none
        */
        static CPresenceConnectionInfo* NewL( 
        		MXIMPProtocolConnection* aConnection );
    
        /**
         * SetConnectionStatus()
         *
         * @since S60 3.2 
         * @param TConnectionStatus
         * @return none
         */
        void SetConnectionStatus( 
        		CPresencePluginConnection::TConnectionStatus aStatus );
        
        /**
         * SetConnectionETag()
         *
         * @since S60 3.2 
         * @param TDesC8 aETag
         * @return none
         */
        void SetConnectionETag( const TDesC8& aETag );
        
        /**
         * GetConnectionStatus()
         *
         * @since S60 3.2 
         * @param none
         * @return TConnectionStatus connection status
         */
        CPresencePluginConnection::TConnectionStatus GetConnectionStatus();
        
        /**
         * GetConnectionEtag()
         *
         * @since S60 3.2 
         * @param none
         * @return TDesC8 ETag
         */
        TDesC8* GetConnectionEtag();
        
        /**
         * GetConnection()
         *
         * @since S60 3.2 
         * @param none
         * @return MXIMPProtocolConnection connection instance
         */
        MXIMPProtocolConnection* GetConnection();
        
        /**
         * IncreaseClientCount()
         *
         * @since S60 3.2 
         * @param none
         * @return none
         */
        void IncreaseClientCount();
        
        /**
         * DecreaseClientCount()
         *
         * @since S60 3.2 
         * @param none
         * @return none
         */        
        void DecreaseClientCount();
        
        /**
         * GenerateClientId()
         *
         * @since S60 3.2 
         * @param none
         * @return TInt uniqueclient id
         */ 
        TInt GenerateClientId();
        
        /**
         * GenerateClientId()
         *
         * @since S60 3.2 
         * @param none
         * @return TInt uniqueclient id
         */ 
        TInt GetClientId();
        
        /**
         * GenerateClientId()
         *
         * @since S60 3.2 
         * @param none
         * @return TInt client count
         */
        TInt ClientCount();
        
         /**
          * Standard C++ destructor
          */
         ~CPresenceConnectionInfo();
               
     private:
     
         /**
          * Standard C++ constructor
          */   
         CPresenceConnectionInfo( MXIMPProtocolConnection* aConnection );
         
    public: // data
        
        /**
         * Connection ETag
         * Own.
         */
        HBufC8* iETag;
        
        /**
         * Connection
         * Own.
         */
        MXIMPProtocolConnection* iConnection;
        
        /**
         * Connection status
         * Own.
         */
        CPresencePluginConnection::TConnectionStatus iConnectStatus;
        
        /**
         * Connection status
         * Own.
         */
        TInt iClientCount;
        
        /**
         * Client id
         * Own.
         */
        TUint iClientId;
                  
        SIMPLE_UNIT_TEST( T_CPresenceConnectionInfo )
        
    };

#endif // CPRESENCEPLUGIN_H
