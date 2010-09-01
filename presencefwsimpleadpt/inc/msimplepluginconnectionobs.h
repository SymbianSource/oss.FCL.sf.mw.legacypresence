/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Presence FW SIMPLE plugin engine connection observer
*
*/




#ifndef MSIMPLEPLUGINCONNECTIONOBSERVER_H
#define MSIMPLEPLUGINCONNECTIONOBSERVER_H

#include <e32std.h>

class TXIMPRequestId;
class MXIMPObjectFactory;
class MPresenceObjectFactory;
class MXIMPProtocolConnectionHost;

class CSimplePluginWinfo;
class MSimpleWinfo;
class CSimplePluginXdmUtils;


/**
 *  MSimplePluginConnectionObs
 *
 *  SIMPLE engine connection
 *
 *  @lib simpleengine
 *  @since s60 v5.0
 */

class MSimplePluginConnectionObs
    {

public:

enum TReqType
    {
    /** undefined */
    ENoReq = 0,
    /** Connection initializing */
    EOpenSess
    };

    /**
     * Coplete the request
     *
     * @since S60 3.2
     * @param aType request type
     * @param aStatus status
     */
    virtual void CompleteReq( TReqType aType, TInt aStatus ) = 0;

    /**
     * Coplete the request
     *
     * @since S60 3.2
     * @param aReqId PrFw request id
     * @param aStatus status
     */
    virtual void CompleteReq( TXIMPRequestId aReqId, TInt aStatus ) = 0;
    
    /**
     * Coplete the SIP watcher info request
     *
     * @since S60 3.2
     * @param aReqId PrFw request id
     * @param aStatus status
     */
    virtual void CompleteWinfoReq( TXIMPRequestId aReqId, TInt aStatus ) = 0;    

    /**
     * PrFw Object Factory accessor
     * @since S60 3.2
     * @return PrFw Object Factory
     */
    virtual MXIMPObjectFactory& ObjectFactory()=0;
    
    /**
     * PrFw Object Factory accessor
     * @since S60 3.2
     * @return PrFw Object Factory
     */
    virtual MPresenceObjectFactory& PresenceObjectFactory()=0;

    /**
     * CSimplePluginWinfo accessor
     * @since S60 3.2
     * @return CSimplePluginWinfo, ownership is not transferred.
     */
    virtual CSimplePluginWinfo* WinfoHandlerL()=0;

    /**
     * CSimplePluginWinfoObserver is terminated
     * @since S60 3.2
     * @param aReason reason code
     */
    virtual void WinfoTerminatedL( TInt aReason )=0;

    /**
     * Watcher info subscription notification is received
     * @aWinfo notification content
     */
    virtual void WinfoNotification( MSimpleWinfo& aWinfo ) = 0;

    /**
     * Access CSimplePluginXdmUtils
     * @return CSimplePluginXdmUtils entity
     */
    virtual CSimplePluginXdmUtils* XdmUtilsL() = 0;

    /**
     * Access connection host
     * @return MXIMPProtocolConnectionHost
     */
    virtual MXIMPProtocolConnectionHost* Host() = 0;    

    };

#endif

// End of File