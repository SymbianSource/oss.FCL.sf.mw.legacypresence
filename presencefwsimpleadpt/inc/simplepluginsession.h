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




#ifndef CSIMPLEPLUGINSESSION_H
#define CSIMPLEPLUGINSESSION_H


#include <e32std.h>
#include <msimpleconnectionobserver.h>
#include "msimplepluginconnectionobs.h"

class MXIMPContextClientInfo;
class MXIMPProtocolConnection;
class MXIMPServiceInfo;


class MSimpleConnection;

/**
 * CSimplePluginSession
 *
 * Simple Engine Connection
 *
 * @lib ?library
 * @since s60 v5.0
 */
NONSHARABLE_CLASS( CSimplePluginSession ) : public CBase,
                             public MSimpleConnectionObserver
    {
public:

    static CSimplePluginSession* NewL(
        const MXIMPServiceInfo& aService,
        MSimplePluginConnectionObs& aObs );

    virtual ~CSimplePluginSession();

    /**
     * OpenSessionL
     *
     * Opens the SIP connection (registers when needed)
     * 
     */
    void OpenSessionL();
    
    /**
     * OpenSessionL
     *
     * Opens the SIP connection (registers when needed)
     * @param aSettingsId settings id
     *
     */
    void OpenSessionL( TInt aSettingsId );    

    /**
     * MSimpleConnection accessor
     * @return MSImpleConnection instance
     */
    MSimpleConnection* SimpleConnection();

    /**
     * XDMSettings accessor
     * @return XDM Settings ID
     */
    TInt XdmSettingsId();

    /**
     * Current registered SIP entity
     */
    TPtrC8 CurrentSipPresentity();

    /**
     * Domain syntax for current settings
     */
    TPtrC16 CurrentDomain();

private:

    CSimplePluginSession( MSimplePluginConnectionObs& aObs );

    void ConstructL(
        const MXIMPServiceInfo& aService );


public:

// from base class MSimpleConnectionObserver

    /**
     * Defined in a base class
     */
    void ConnectionStatusL( MSimpleConnection::TSimpleState aState );

    /**
     * Defined in a base class
     */
    void RequestCompleteL( TInt aOpId, TInt aStatus );

// from base class CActive

protected:


private: // Data

    /**
     * PrFw Plugin connection observer
     */
    MSimplePluginConnectionObs& iObs;

    /**
     * Simple Engine connection.
     * Own.
     */
    MSimpleConnection* iConnection;

    /**
     * Current Simple Engine operation id
     */
    TInt iOpId;

    /**
     * IAP Id
     */
    TInt32 iIap;

    /**
     * XDM Settings id
     */
    TInt iXdmSetting;

    /**
     * Request type
     */
    MSimplePluginConnectionObs::TReqType iType;

    /**
     * Current User ID, User's SIP identity
     * Own.
     */
    HBufC8* iUserId8;

    /**
     * Domain syntax for current settings
     * Own,
     */
    HBufC16* iDomain;
    
    /**
     * Whether conncted to network
     */
    TBool iConnected;

    };

#endif // CSIMPLEPLUGINSESSION_H
