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




#ifndef CSIMPLEPLUGINPUBLISHER_H
#define CSIMPLEPLUGINPUBLISHER_H

#include <e32std.h>

#include <ximpbase.h>
#include <protocolpresencepublishing.h>

#include <msimplepublishobserver.h>
#include <msimplewatcherobserver.h>

#include "simpleplugincommon.h"
#include "msimplepluginconnectionobs.h"
#include "msimplepluginsettings.h"

class TXIMPRequestId;
class MXIMPIdentity;
class MXIMPProtocolConnectionHost;

class MPersonPresenceInfo;
class MServicePresenceInfo;
class MDevicePresenceInfo;

class MSimplePublisher;
class MSimpleDocument;
class MSimpleWinfo;
class MSimpleWatcher;
class MSimpleElement;


/**
 * CSimplePluginWatcherInfo
 *
 * watcher info.
 *
 * @lib ?library
 * @since s60 v5.0
 */
NONSHARABLE_CLASS( CSimplePluginWatcherInfo ): public CBase
    {
public:

    /**
     * Constructor.
     * @param aId watcher id
     * @param aSipId watcher SIP identity
     */
    static CSimplePluginWatcherInfo* NewL(
        const TDesC8& aId,
        const TDesC& aSipId );

    virtual ~CSimplePluginWatcherInfo();
            
    /**
     * Destructor
     */
    void Destroy();
    
    /** 
     * Match to given info
     * @param aId watcher id
     * @param aSipId watcher SIP identity     
     * @return ETrue if current entity matches to the parameters
     */
    TBool Match( const TDesC8& aId,
        const TDesC& aSipId ); 
        
    TPtrC SipId();    
    
    inline static TInt LinkOffset(); 
    
private:

    CSimplePluginWatcherInfo( );

    /** 
     * Constructor
     * @param aId watcher id
     * @param aSipId watcher SIP identity     
     */
    void ConstructL( 
        const TDesC8& aId,
        const TDesC& aSipId ); 
   
public: // data        

    TDblQueLink iLink;
    
private: // data
    
    HBufC8* iId;
    HBufC* iSipId;

    };

/**
 * CSimplePluginPublisher
 *
 * publisher.
 *
 * @lib ?library
 * @since s60 v5.0
 */
NONSHARABLE_CLASS( CSimplePluginPublisher ): public CActive,
    public MSimplePublishObserver,
    public MSimpleWatcherObserver,
    public MProtocolPresencePublishing
    {
public:

    /**
     * Current operation
     */
    enum TPluginPublishOperation
        {
        ENoOperation,
        EPublishOwn,
        ESubscribeOwn,
        EUnsubscribeOwn,
        ESubscribeWinfo,
        EUnsubscribeWinfo
        };

    /**
     * Constructor.
     * @param aConnSetts current settings
     * @param aObs callback for complete requests
     * @param aConn Simple Engine connection
     */
    static CSimplePluginPublisher* NewL(
        MSimplePluginSettings& aConnSetts,
        MSimplePluginConnectionObs& aObs,
        MSimpleConnection& aConn );

    virtual ~CSimplePluginPublisher();

    /**
     * WinfoTerminatedL
     *
     * SIMPLE Winfo watcher sunscription is terminated
     * @param aReason reason code
     */
     void WinfoTerminatedL( TInt aReason );

     /**
      * WINFO received from SIMPLE
      * @param aWinfo WINFO
      */
     void WinfoNotificationL( MSimpleWinfo& aWinfo );

private:

    CSimplePluginPublisher(
        MSimplePluginSettings& aConnSetts,    
        MSimplePluginConnectionObs& aObs,
        MSimpleConnection& aConn );

    void ConstructL( );


public:

// from base class MSimplePublishObserver

    /**
     * Defined in a base class
     */
    void PublishReqCompleteL( TInt aOpid, TInt aStatus);

    /**
     * Defined in a base class
     */
    void PublishTerminatedL( TInt aOpid );

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


// from base class MXIMPBase
public:

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


// from base class MProtocolPresencePublishing
public:

    /**
     * Defined in a base class
     */
    void DoPublishOwnPresenceL(
        const MPresenceInfo& aOwnPresence,
        TXIMPRequestId aReqId );

    /**
     * Defined in a base class
     */
    void DoSubscribeOwnPresenceL(
        const MPresenceInfoFilter& aPif,
        TXIMPRequestId aReqId );

    /**
     * Defined in a base class
     */
    void DoUpdateOwnPresenceSubscriptionPifL(
        const MPresenceInfoFilter& aPif,
        TXIMPRequestId aReqId );

    /**
     * Defined in a base class
     */
    void DoUnsubscribeOwnPresenceL(
        TXIMPRequestId aReqId );

    /**
     * Defined in a base class
     */
    void DoSubscribePresenceWatcherListL(
        TXIMPRequestId aReqId );

    /**
     * Defined in a base class
     */
    void DoUnsubscribePresenceWatcherListL(
        TXIMPRequestId aReqId );


protected:

// from base class CActive

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

private:


    /**
     * Calls MXIMPProtocolConnectionHost callback
     * @param aStatus error status
     */
    void CompletePrFwReq( TInt aStatus );

    /**
     * Initialize a document
     */
    void InitializeSimpleDocumentL( );

    /**
     * Add person information to the document.
     * @param aPersInfo personal information
     */
    void AddSimpleDocumentPersL(
        const MPersonPresenceInfo* aInfo );

    void AddSimpleDocumentServiceL(
        const MServicePresenceInfo& aInfo );

    void AddSimpleDocumentDeviceL(
        const MDevicePresenceInfo& aInfo );

    /**
     * Make a publish request
     */
    void MakePublishReqL();

    /**
     * Start XDM initialization
     */
    void StartXdmOperationL( TXIMPRequestId aReqId );
    
    /**
     * Start XDM initialization. StartXdmOperationL calls this.
     */
    void DoStartXdmOperationL( TXIMPRequestId aReqId );    
    
    /**
     * Delete watcher information
     */
    void DeleteWatchers();  
    
    void AddWatcherIfNotExistsL( const TDesC8& aId, const TDesC& aSipId );
        
    void RemoveWatcherIfExistsL( const TDesC8& aId, const TDesC& aSipId ); 
    
    CDesCArrayFlat* MakeCurrentWatcherListLC();  
    
    
    /**
     * Update private data member of active watchers list
     * @param aElems SIMPLE elements [IN]
     */
    void UpdateActiveWatchersListL( 
        RPointerArray<MSimpleElement>& aElems );     


private: // Data

    /**
     * PrFw Plugin connection observer
     */
    MSimplePluginConnectionObs& iConnObs;
    
    /**
     * Current settings
     */
    MSimplePluginSettings& iConnSets;

    /**
     * SIMPLE engine connection
     */
    MSimpleConnection& iConnection;

    /**
     * SIMPLE engine publisher
     * Own.
     */
    MSimplePublisher* iPublisher;

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
     * Subscribed winfo
     */
    TBool iSubscribed;

    /**
     * Subscribed own data
     */
    TBool iSubscribedOwn;

    /**
     * Published own data
     */
    TBool iPublished;

    /**
     * Current operation
     */
    TPluginPublishOperation iOperation;

    /**
     * Simple publish document
     * Own.
     */
    MSimpleDocument* iDocument;

    /**
     * XDM Utils
     * Not own.
     */
    CSimplePluginXdmUtils* iXdmUtils;
    
    /**
     * WINFO watchers
     * Own.
     */
    TDblQue<CSimplePluginWatcherInfo> iWatcherList;

    };
    
    TInt CSimplePluginWatcherInfo::LinkOffset()
        {
        return _FOFF(CSimplePluginWatcherInfo, iLink);
        }     

#endif // CSimplePluginPublisher_H
