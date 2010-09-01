/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides IM sending and receiving services.
*
*/


#ifndef C_CSIMPLEIMCONVERSATION_H
#define C_CSIMPLEIMCONVERSATION_H

#include <e32base.h>
#include <ximpbase.h>
#include <ximpcontextobserver.h>
#include <ximprequestcompleteevent.h>
#include <protocolimconversation.h>
#include <simpleinstantmsgobserver.h>
#include <simpleinstantmessage.h>

class CIMCacheFactory;
class MIMCacheUpdater;
class MPresencePluginConnectionObs;
class MSimpleConnection;
class MProtocolImDataHost;
class MXIMPIdentity;

/**
 *  This class is used for sending and receiving instant messages.
 *  A client sends the message and waits for notification of send process 
 *  success. Client is notified with 
 *  MPresencePluginConnectionObs::CompleteReq().
 *  
 *  There can be only one message in the sending que at the time.
 *  Otherwise DoSendMessageL() method leaves with KErrInUse.
 *  
 *  Messages received from the other end are handled in this class and
 *  delivered to the client. Client gets messages through
 *  MProtocolImConversationDataHost::HandleNewTextMessageL().
 *
 *  @lib simpleimplugin.dll
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CSimpleImConversation ) : public CBase,
                                             public MProtocolImConversation,
                                             public MSimpleInstantMsgObserver
    {

public:
    
    /**
     * Two-phased constructor.
     * 
     * @param   aObs    Callback for complete requests.
     * @param   aConn   Simple engine connection.
     */
    static CSimpleImConversation* NewL(
        MPresencePluginConnectionObs& aObs, MSimpleConnection& aConn,
        TUint aServiceId );

    /**
     * Destructor.
     */ 
    virtual ~CSimpleImConversation();

public:
    
    /**
     * Sets current IM data host.
     * 
     * @since   S60 v5.1
     * @param   aHost   IM data host.
     */
    void SetHost( MProtocolImDataHost& aHost );
    
public:  //from MXIMPBase

    /**
     * From MXIMPBase.
     * @see MXIMPBase.
     */
    TAny* GetInterface( 
        TInt32 aInterfaceId,
        TIfGetOps  aOptions );
    
    /**
     * From MXIMPBase.
     * @see MXIMPBase.
     */
    const TAny* GetInterface( 
        TInt32 aInterfaceId,
        TIfGetOps aOptions ) const;
    
    /**
     * From MXIMPBase.
     * @see MXIMPBase.
     */
    TInt32 GetInterfaceId() const;

public: // From MProtocolImConversation
    
    /**
     * From MProtocolImConversation.
     * @see MProtocolImConversation.
     */
    void DoSendMessageL( 
        const MImConversationInfo& aImMessage,
        TXIMPRequestId aReqId );
    
    /**
     * From MProtocolImConversation.
     * @see MProtocolImConversation.
     */
    void DoForwardMessageL(
        const MImConversationInfo& aImMessage,
        TXIMPRequestId aReqId);
    
    /**
     * From MProtocolImConversation.
     * @see MProtocolImConversation.
     */
    void DoSendMessageToGroupL( 
        const MImConversationInfo& aImMessage,
        const TDesC* aGroupId,TXIMPRequestId aReqId );
    
    /**
     * From MProtocolImConversation.
     * @see MProtocolImConversation.
     */
    TXIMPRequestId DoBlockUsersL(
        const MDesCArray* aUserIds,
        TXIMPRequestId aReqId);
    
    /**
     * From MProtocolImConversation.
     * @see MProtocolImConversation.
     */
    TXIMPRequestId DoUnBlockUsersL(
        const MDesCArray* aUserIds,
        TXIMPRequestId aReqId );
    
    /**
     * From MProtocolImConversation.
     * @see MProtocolImConversation.
     */
    void DoGetBlockedListRequestL( 
        const MXIMPIdentity& aImMessageId,
        TXIMPRequestId aReqId );            
    
    /**
     * From MProtocolImConversation.
     * @see MProtocolImConversation.
     */
    void DoGetMessageListL( 
        const MXIMPIdentity& aImMessageId,
        const TDesC* aGroupId,
        const TInt aMessageCount,
        TBool aDeliveryReportWanted,
        TXIMPRequestId aReqId);
    
    /**
     * From MProtocolImConversation.
     * @see MProtocolImConversation.
     */
    void DoRejectMessageL( 
        const MXIMPIdentity& aImMessageId,
        const TDesC& aMessageID,
        TXIMPRequestId aReqId );
    
public: // From MSimpleInstantMsgObserver
    
    /**
     * From MSimpleInstantMsgObserver.
     * @see MSimpleInstantMsgObserver.
     */
    void SendInstantMsgReqComplete( TInt aOpid, TInt aStatus );
    
    /**
     * From MSimpleInstantMsgObserver.
     * @see MSimpleInstantMsgObserver.
     */
    void HandleReceiveTextMessage( TPtrC8 aFrom, TPtrC8 aContent );

private: // constructor and the Symbian second-phase constructor

    CSimpleImConversation( MPresencePluginConnectionObs& aObs, 
                           MSimpleConnection& aConn,
                           TUint aServiceId );
    
    void ConstructL( );

private:

    /**
     * Calls MXIMPProtocolConnectionHost callback.
     *
     * @since   S60 v5.1
     * @param   aStatus     Error status.
     */
    void CompleteXimpReq( TInt aStatus );
    
    /**
     * Leaveable version of HandleReceiveTextMessage.
     * 
     * @see MSimpleInstantMsgObserver 
     */
    void DoHandleReceiveTextMessageL( TPtrC8 aFrom, TPtrC8 aContent );
    
    /**
     * Resolves username to be used.
     * 
     * @since   S60 v5.1
     * @param   aServiceId     Service identifier.
     * @param   aUserName      On return contains resolved user name.
     */
    void ResolveUsernameL( TUint aServiceId, RBuf& aUserName ) const;
    
    /**
     * Creates IM cache updater if not yet created.
     * 
     * @since   S60 v5.1
     */
    void CreateImCacheUpdaterIfNeededL();
    
private:  // Data
    
    /**
     * XIMP Plugin connection observer.
     * Not own.
     */
    MPresencePluginConnectionObs* iConnectionObs;
    
    /**
     * SIMPLE engine connection.
     * Not Own.
     */
    MSimpleConnection* iConnection;
    
    /**
     * Instant message handler instance.
     * Own.
     */
    MSimpleInstantMessage* iImHandler;
    
    /**
     * Request identifier.
     */
    TXIMPRequestId iXimpId;

    /**
     * Current request status; completed or not.
     */
    TBool iCompleted;
    
    /**
     * Protocol IM data host.
     * Not own.
     */
    MProtocolImDataHost* iHost;
    
    /**
     * IM cache factory.
     * Own.
     */
    CIMCacheFactory* iImCacheFactory;

    /**
     * IM cache updater.
     * Owned by factory.
     */
    MIMCacheUpdater* iImCacheUpdater;
    
    /**
     * ServiceId of current 
     * service
     */
    TUint iServiceId;
    
    };

#endif // C_CSIMPLEIMCONVERSATION_H
