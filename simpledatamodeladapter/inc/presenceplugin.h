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


#ifndef CPRESENCEPLUGIN_H
#define CPRESENCEPLUGIN_H

#include <e32base.h>
#include <ximpprotocolpluginbase.h>

#include "presencelogger.h"

class CPresencePluginConnection;
class MXIMPProtocolConnection;
class MXIMPServiceInfo;
class CPresenceConnectionInfo;

/**
 * CPresencePlugin
 *
 * Implementation of CXIMPProtocolPluginBase
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CPresencePlugin ) : public CXIMPProtocolPluginBase
    {
    public:
    
         /**
         * Two-phased constructor.
         *
         * @param none
         */
        static CPresencePlugin* NewL();
        
        /**
         * Two-phased constructor.
         *
		 * @param none
         */
        static CPresencePlugin* NewLC();

        /**
         * Standard C++ destructor
         */
        ~CPresencePlugin();
    
    private:
    
        /**
         * Standard C++ constructor
         */   
        CPresencePlugin();
        
        /**
         * Delete connection from arrays
         * @param TInt aIndex, array index
         * @return none
         */  
        void DeleteConnection( TInt aIndex );
                
    public: // from base class MXIMPProtocolPlugin

        /**
         * Defined in a base class
         */ 
        void PrimeHost( MXIMPProtocolPluginHost& aHost );
        
        /**
         * Defined in a base class
         */ 
        MXIMPProtocolConnection& AcquireConnectionL(
            const MXIMPServiceInfo& aService,
            const MXIMPContextClientInfo& aContextClient );
                     
        /**
         * Defined in a base class
         */                 
        void ReleaseConnection( MXIMPProtocolConnection& aConnection );
        
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
        
    private: // Data
        
        /**
         * Prime host
         * Own.
         */
        MXIMPProtocolPluginHost* iHost;
        
        /**
         * connection array
         * Own.
         */
        RPointerArray< CPresencePluginConnection > iConnections;
        
        /**
         * connection data array
         * Own.
         */
        RPointerArray< CPresenceConnectionInfo > iConnectionArray;
        
        SIMPLE_UNIT_TEST( T_CPresencePlugin )
        
    };
#endif // CPRESENCEPLUGIN_H