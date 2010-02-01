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
* Description:    CSimplePluginConnection implementation.
*
*/




#ifndef CSIMPLEPLUGINCONNECTION_H
#define CSIMPLEPLUGINCONNECTION_H


#include <ximpprotocolconnection.h>
#include <protocolpresencefeatures.h>
#include "msimplepluginconnectionobs.h"
#include "msimplepluginsettings.h"

class MXIMPObjectFactory;
class MXIMPServiceInfo;
class CSimplePluginAuthorization;
class CSimplePluginSession;
class CSimplePluginPublisher;
class CSimplePluginWatcher;
class CSimplePluginGroups;
class CSimplePluginVariation;

/**
 * CSimplePluginConnection
 *
 * Implementation of MXIMPProtocolConnection
 *
 * @lib ?library
 * @since s60 v5.0
 */
NONSHARABLE_CLASS(CSimplePluginConnection) : public CBase,
    public MXIMPProtocolConnection,
    public MProtocolPresenceFeatures,
    public MSimplePluginConnectionObs,
    public MSimplePluginSettings  
    {

public:

    static CSimplePluginConnection* NewL(
        const MXIMPServiceInfo& aServiceInfo,
        const MXIMPContextClientInfo& aClientCtxInfo );

    ~CSimplePluginConnection();

private:

    CSimplePluginConnection();


    void ConstructL( const MXIMPServiceInfo& aServiceInfo,
                     const MXIMPContextClientInfo& aClientCtxInfo );
                     
public:

    /**
     * Convert error code into PrFW errors
     * @param aStatus error code [IN]
     * @return harmonized error code
     */
    static TInt HarmonizeErrorCode( TInt aStatus );

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


// from base class MXIMPProtocolConnection
public:

    /**
     * Defined in a base class
     */
    void PrimeHost( MXIMPProtocolConnectionHost& aHost );

    /**
     * Defined in a base class
     */
    void OpenSessionL(
        const MXIMPContextClientInfo& aContextClient,
        TXIMPRequestId aReqId );

    /**
     * Defined in a base class
     */
    void OpenSessionL(
        const TInt& aSettingsId,
        TXIMPRequestId aReqId );

    /**
     * Defined in a base class
     */
    void CloseSession( 
        const MXIMPContextClientInfo& aContextClient,
        TXIMPRequestId aOpId );

    /**
     * Defined in a base class
     */
    void GetSupportedFeaturesL( CDesC8Array& aFeatures ) const;
    
    /**
     * Defined in a base class
     */    
    MProtocolPresenceFeatures& ProtocolPresenceFeatures();
    
/*<branchInfo originator="gronoff" since="08-06-06" category="new_feature" error=”none”>
<pf_5250_OFF_CYCLE_SW_INT>
</branchInfo>*/
// BRANCH 08-06-06 : gronoff
    TAny* GetProtocolInterface( TInt aInterfaceId );
// BRANCH_END 08-06-06 : gronoff        
    
    
// from base class MProtocolPresenceFeatures

    MProtocolPresenceWatching& PresenceWatching();


    MProtocolPresencePublishing& PresencePublishing();


    MProtocolPresentityGroups& PresentityGroups();


    MProtocolPresenceAuthorization& PresenceAuthorization();  
    
    

// from base class MSimplePluginConnectionObs

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
    void CompleteWinfoReq( TXIMPRequestId aReqId, TInt aStatus );      

    /**
     * Defined in a base class
     */
    MXIMPObjectFactory& ObjectFactory();
    
    /**
     * Defined in a base class
     */    
    MPresenceObjectFactory& PresenceObjectFactory();    

    /**
     * Defined in a base class
     */
    CSimplePluginWinfo* WinfoHandlerL();

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
    CSimplePluginXdmUtils* XdmUtilsL();

    /**
     * Defined in a base class
     */
    MXIMPProtocolConnectionHost* Host();

    
// from base class MSimplePluginSettings 

    /**
     * Defined in a base class
     */
    TPtrC16 CurrentSipPresentity();

    /**
     * Defined in a base class
     */
    TPtrC8 CurrentSipPresentity8();

    /**
     * Defined in a base class
     */
    TPtrC16 CurrentDomain();
    
    /**
     * Variation
     */
    CSimplePluginVariation& Variation();  
    
private:

    

private:    // data members

    /**
     * Host for connection.
     * Not own.
     */
    MXIMPProtocolConnectionHost* iConnectionHost;

    /**
     * Simple Engine session
     * Own.
     */
    CSimplePluginSession* iSession;

    /**
     * current request id
     */
    TXIMPRequestId iPrFwId;

    /**
     * MProtocolPresenceAuthorization implementation.
     * Own.
     */
    CSimplePluginAuthorization* iAuth;

    /**
     * CSimplePluginWinfo
     * Own.
     */
    CSimplePluginWinfo* iPluginWinfo;

    /**
     * CSimplePluginPublisher
     * Own.
     */
    CSimplePluginPublisher* iPublisher;

    /**
     * CSimplePluginWatcher
     * Own.
     */
    CSimplePluginWatcher* iWatcher;

    /**
     * CSimplePluginGroups
     * Own.
     */
    CSimplePluginGroups* iGroups;

    /**
     * Current user SIP identity
     * Own.
     */
    HBufC16* iSipPresentity;

    /**
     * XDM utils
     * Own.
     */
    CSimplePluginXdmUtils* iXdmUtils;
    
    
    /**
     * Run time variation
     * Own
     */
    CSimplePluginVariation* iVariation;    

    };


#endif // CSIMPLEPLUGIN_H
