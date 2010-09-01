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


#ifndef MPRESENCEPLUGINCONNECTIONOBS_H
#define MPRESENCEPLUGINCONNECTIONOBS_H

#include <e32std.h>
#include <ximpbase.h>
#include <ximpprotocolpluginhostifids.hrh>

class TXIMPRequestId;
class MXIMPObjectFactory;
class CPresencePluginWinfo;
class MSimpleWinfo;
class MXIMPProtocolConnectionHost;
class CPresencePluginXdmUtils;
class CPresencePluginWatcher;
class MPresenceObjectFactory;
class MProtocolPresenceDataHost;
class MProtocolPresenceAuthorization;
class CPresencePluginVirtualGroup;
class CPresencePluginAuthorization;
class CPresencePluginGroups;

/**
 *  MPresencePluginConnectionObs
 *
 *  presence engine connection
 *
 *  @lib presenceplugin.dll
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( MPresencePluginConnectionObs )
    {

public:

    enum TReqType
        {
        /** undefined */
        ENoReq = 0,
        /** Connection initializing */
        EOpenSess,
        /** Connection close */
        ECloseSess,
        /** Publish own data */
        EPublish,
        /** Subscribe Own data */
        ESubscribeOwn,
        /** Subscribe other user */
        ESubscribeOther
        };
    
    /**
     * CPresencePluginWatcher accessor
     * @return CPresencePluginWatcher
     */    
    virtual CPresencePluginWatcher* WatcherHandlerL() = 0;
    
    /**
     * Complete the request
     *
     * @param aType request type
     * @param aStatus status
     */
    virtual void CompleteReq(TReqType aType, TInt aStatus ) = 0;

    /**
     * Complete the request
     *
     * @param aReqId ximp request id
     * @param aStatus status
     */
    virtual void CompleteReq(TXIMPRequestId aReqId, TInt aStatus ) = 0;

    /**
     * XIMP Object Factory accessor
     * @return ximp Object Factory
     */
    virtual MXIMPObjectFactory& ObjectFactory()=0;
       
    /**
     * XIMP Object Factory accessor
     * @return PrFw Object Factory
     */
    virtual MPresenceObjectFactory& PresenceObjectFactoryOwn()=0;
    
    /**
     * XIMP presence authorization accessor
     * @return PrFw Protocol Presence Authorization
     */
    virtual MProtocolPresenceAuthorization& PresenceAuthorization() = 0;
    
    /**
     * XIMP presence authorization accessor as own internal type
     * @return Presence Authorization
     */
    virtual CPresencePluginAuthorization& InternalPresenceAuthorization() = 0;    
    
    /**
     * Gets reference to
     * MProtocolPresenceDataHost interface.
     *
     * Presence protocol connection implementation uses returned
     * data host interface to manage the Presence connection's
     * presence auhtorization data.
     *
     * @return MProtocolPresenceAuthorizationDataHost interface.
     * Interface ownership is not returned caller.
     * Returned interface is guaranteed to remain valid (usable)
     * as long as this source interface.
     */
    virtual MProtocolPresenceDataHost& ProtocolPresenceHost() = 0;
    
    /**
     * CPresencePluginWinfo accessor
     * @return CPresencePluginWinfo, ownership is not transferred.
     */
    virtual CPresencePluginWinfo* WinfoHandlerL()=0;  
    
    /**
     * CPresencePluginWinfoObserver is terminated
     * @param aReason reason code
     */
    virtual void WinfoTerminatedL( TInt aReason )=0;  
    
    /**
     * Watcher info subscription notification is received
     * @aWinfo notification content
     */
    virtual void WinfoNotification( MSimpleWinfo& aWinfo ) = 0;  
      
    /**
     * Current registered SIP entity
     */
    virtual TPtrC8 CurrentSipPresentity8() = 0;             
    
    /**
     * Access connection host
     * @return MximpProtocolConnectionHost
     */
    virtual MXIMPProtocolConnectionHost* Host() = 0;
    
    /**
     * Current domain syntax accessor
     */
    virtual TPtrC16 CurrentDomain() = 0;
    
    /**
     * Access CPresencePluginXdmUtils
     * @return CPresencePluginXdmUtils entity
     */
    virtual CPresencePluginXdmUtils* XdmUtilsL() = 0;
    
    /**
     * Get ETag
     * @return TDesC8, etag value
     */
    virtual const TDesC8& GetETag() const = 0;
    
    /**
     * Set ETag
     * @return none
     */
    virtual void SetETag( const TDesC8& aETag ) = 0;
    
    /**
     * Get client session time tupleId
     * @return TInt, tuple value
     */
    virtual TInt GetTupleId() const = 0;
    
    /**
     * Set session time tupleId
     * @return none
     */
    virtual void SetTupleId( const TInt aTupleId ) =0;
    
    /**
     * Get Stop publish state
     * @return TBool, stop publish state
     */
    virtual TBool GetStopPublishState() = 0;
    
    /**
     * Is Stop publish called
     * @return TBool, stop publish called state
     */
    virtual TBool IsStopPublishCalled() = 0;
    
    /**
     * Get Stop publish state
     * @return TBool, stop publish state
     */
    virtual void SetStopPublishState( TBool aState ) = 0;
    
    /**
     * Get Stop publish state
     * @return TBool, session status
     */
    virtual TBool GetSessionStatus( ) = 0; 
    
    /**
     * Return instace of subcribed contact
     * @return TBool, session status
     */
    virtual CPresencePluginVirtualGroup* SubscribedContacts( ) = 0;
    
    /** 
     * Get grand request list subscribe state
     */
    virtual TBool GrandListState() = 0;
    
    /** 
     * Terminate connection
     */
    virtual void TerminateConnectionL( ) = 0;
    
    /** 
     * Return Groups API implemention
     */
    virtual CPresencePluginGroups& GroupsL( ) = 0;  
    
    /** 
     * Return service id
     */
    virtual TInt& ServiceId( ) = 0;        
    };

#endif

// End of File