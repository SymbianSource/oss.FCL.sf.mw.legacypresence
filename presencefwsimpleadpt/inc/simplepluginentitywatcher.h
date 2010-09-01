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




#ifndef CSIMPLEPLUGINENTITYWATCHER_H
#define CSIMPLEPLUGINENTITYWATCHER_H

#include <e32std.h>

#include <protocolpresencewatching.h>

#include <msimplewatcherobserver.h>

#include "simpleplugincommon.h"
#include "msimplepluginconnectionobs.h"


class TXIMPRequestId;
class MXIMPIdentity;
class MXIMPProtocolConnectionHost;
class MPersonPresenceInfo;

class MSimpleDocument;
class MSimpleWatcher;

class CSimplePluginWatcher;

/**
 * CSimplePluginEntityWatcher
 *
 * Simple Engine Connection
 *
 * @lib ?library
 * @since s60 v5.0
 */
NONSHARABLE_CLASS( CSimplePluginEntityWatcher ): public CBase,
    public MSimpleWatcherObserver
    {

private:
    /**
     * Current operation
     */
    enum TPluginEntityWatcherOperation
        {
        EPluginIdle,
        EPluginStop,
        EPluginStart
        };

public:

    /**
     * Constructor.
     * @param aObs callback for complete requests
     * @param aConn Simple Engine connection
     * @param aHost PrFw connection host
     */
    static CSimplePluginEntityWatcher* NewL(
        MSimplePluginConnectionObs& aObs,
        MSimpleConnection& aConn,
        CSimplePluginWatcher& aWatcher );

    virtual ~CSimplePluginEntityWatcher();


    /**
     * Access Request id
     * @return PrFw request id
     */
    TXIMPRequestId PrFwRequestId();

    /**
     * Access Presentity id
     * @return Presentity id
     */
    TPtrC8 PresentityId();

    /**
     * Start subscribe
     * @param aPresentityId presentity id
     */
    void StartSubscribeL(
        const TDesC8& aPresentityId );

    /**
     * Start subscribe
     * @param aPresentityId presentity id
     */
    void StartSubscribeListL(
        const TDesC8& aPresentityId );

    /**
     * Set simple name
     * @param aPresentityId presentity id
     */
    void SetSimpleNameL(
        const TDesC8& aPresentityId );

    void StopSubscribeL( );

private:

    CSimplePluginEntityWatcher(
        MSimplePluginConnectionObs& aObs,
        MSimpleConnection& aConn,
        CSimplePluginWatcher& aWatcher );

    void ConstructL( );

public:

// from base class MSimpleWatcherObserver

    /**
     * Defined in a base class
     */
    void WatcherReqCompleteL( TInt aOpId, TInt aStatus );

    /**
     * Defined in a base class
     */
    void WatcherNotificationL( MSimpleDocument& aDocument );

    /**
     * Defined in a base class
     */
    void WatcherListNotificationL( MSimplePresenceList& aList );

    /**
     * Defined in a base class
     */
    void WatcherTerminatedL(
        TInt aOpId, TInt aReason );

private:

    /**
     * Calls caller active object
     * @param aStatus error status
     */
    void CompleteClientReq( TInt aStatus );



private: // Data

    /**
     * host class
     */
    CSimplePluginWatcher& iPluginWatcher;

    /**
     * PrFw Plugin connection observer
     */
    MSimplePluginConnectionObs& iConnObs;

    /**
     * SIMPLE engine connection
     */
    MSimpleConnection& iConnection;

    /**
     * SIMPLE engine watcher
     * Own.
     */
    MSimpleWatcher* iWatcher;

    /**
     * PrFW request id
     */
    TXIMPRequestId iPrFwId;

    /**
     * Simple Engine request id
     */
    TInt iSimpleId;

    /**
     * client statutus.
     * Not own.
     */
    TRequestStatus* iClientStatus;

    /**
     * Subscribed entity id
     * Own.
     */
    HBufC8* iEntityId;

    /**
     * Subscribed entity id, pure group name
     * Own.
     */
    HBufC8* iSimpleEntityId;

    /**
     * current pending operation
     */
    TPluginEntityWatcherOperation iOperation;

    /**
     * Whether subscription of list is pending
     */
    TBool iListSubsActive;

    };

#endif // CSimplePluginEntityWatcher_H
