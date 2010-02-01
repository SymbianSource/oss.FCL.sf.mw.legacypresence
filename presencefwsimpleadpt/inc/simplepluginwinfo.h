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




#ifndef CSIMPLEPLUGINWINFO_H
#define CSIMPLEPLUGINWINFO_H

#include <e32std.h>

#include <msimplewinfoobserver.h>

#include "simpleplugincommon.h"
#include "msimplepluginconnectionobs.h"

#include <ximpbase.h>

class MSimpleWinfo;
class MXIMPIdentity;
class MPresenceInfoFilter;
class MSimpleWinfoWatcher;
class MXIMPProtocolConnectionHost;
class MSimpleConnection;




/**
 * CSimplePluginWinfo
 *
 * Simple Engine Connection
 *
 * @lib ?library
 * @since s60 v5.0
 */
NONSHARABLE_CLASS( CSimplePluginWinfo ) : public CBase,
    public MSimpleWinfoObserver
    {
public:

    /**
     * Constructor.
     * @param aObs callback for complete requests
     * @param aConn Simple Engine connection
     */
    static CSimplePluginWinfo* NewL(
        MSimplePluginConnectionObs& aObs,
        MSimpleConnection& aConn );

    virtual ~CSimplePluginWinfo();

    /**
     * SetHost
     *
     * Set PrFw Host
     *
     * @lib ?library
     * @since s60 v5.0
     * @patam aHost PrFW Host
     */
    void SetHost( MXIMPProtocolConnectionHost* aHost );

    /**
     * SubscribeWinfoListL
     *
     *
     * @lib ?library
     * @since s60 v5.0
     * @param aReqId PrFw req id
     */
    void SubscribeWinfoListL( TXIMPRequestId aReqId );

    /**
     * UnsubscribeWinfoListL
     *
     *
     * @lib ?library
     * @since s60 v5.0
     * @param aReqId PrFw req id
     */
    void UnsubscribeWinfoListL( TXIMPRequestId aReqId );

private:

    CSimplePluginWinfo(
        MSimplePluginConnectionObs& aObs,
        MSimpleConnection& aConn );

    void ConstructL( );


public:

// from base class MSimpleWinfoObserver

// Subscribe presence grant request list

    /**
     * Defined in a base class
     */
    void WinfoReqCompleteL( TInt aOpid, TInt aStatus );

    /**
     * Defined in a base class
     */
    void WinfoTerminatedL(
        TInt aOpid, TInt aReason );

    /**
     * Defined in a base class
     */
    void WinfoNotificationL( MSimpleWinfo& aWinfo );


private: // Data

    /**
     * PrFw Plugin connection observer
     */
    MSimplePluginConnectionObs& iConnObs;

    /**
     * PrFw Host.
     * Not Own.
     */
    MXIMPProtocolConnectionHost* iHost;

    /**
     * SIMPLE engine connection
     */
    MSimpleConnection& iConnection;

    /**
     * SIMPLE engine winfo watcher.
     * Own.
     */
    MSimpleWinfoWatcher* iWinfoWatcher;

    /**
     * PrFW request id
     */
    TXIMPRequestId iPrFwId;

    /**
     * Simple Engine request id
     */
    TInt iSimpleId;

    /**
     * Subscribed
     */
    TInt iSubscribed;

    /**
     * Winfo subscribe completed
     */
    TBool iWinfoCompleted;


    };

#endif // CSimplePluginWinfo_H
