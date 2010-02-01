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
* Description:  
*
*/


#include <ximpobjectfactory.h>
#include <imobjectfactory.h>
#include <simplefactory.h>
#include <imconversationinfo.h>
#include <utf.h>
#include <ximpprotocolconnectionhost.h>
#include <protocolimdatahost.h>
#include <protocolimconversationdatahost.h>
#include <spsettings.h>
#include <cch.h>
#include <cimcachefactory.h>
#include <mimcacheupdater.h>

#include "simpleimconversation.h"
#include "presenceplugincommon.h"
#include "mpresencepluginconnectionobs.h"
#include "simpleimplugindefs.h"
#include "simpleimlogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginGroups::CPresencePluginGroups
// ---------------------------------------------------------------------------
//
CSimpleImConversation::CSimpleImConversation( 
    MPresencePluginConnectionObs& aObs, MSimpleConnection& aConn,
    TUint aServiceId ):
  	iConnectionObs( &aObs ),
  	iConnection( &aConn ),
  	iServiceId( aServiceId )  	
    {
    }


// ---------------------------------------------------------------------------
// CSimpleImConversation::NewL
// ---------------------------------------------------------------------------
//
CSimpleImConversation* CSimpleImConversation::NewL(
    MPresencePluginConnectionObs& aObs,
    MSimpleConnection& aConn, TUint aServiceId)
    { 
    CSimpleImConversation* self = 
        new( ELeave ) CSimpleImConversation ( aObs, aConn, aServiceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSimpleImConversation::ConstructL()
// ---------------------------------------------------------------------------
//
void CSimpleImConversation::ConstructL()
    {
    DP_IMP("CSimpleImConversation::ConstructL")
    
    iImHandler = TSimpleFactory::NewInstantMessageL( 
        *iConnection, *this );
    
    iImCacheFactory = CIMCacheFactory::InstanceL();
    }


// ---------------------------------------------------------------------------
// CSimpleImConversation::~CSimpleImConversation
// ---------------------------------------------------------------------------
//
CSimpleImConversation::~CSimpleImConversation()
    {
    DP_IMP("CSimpleImConversation::~CSimpleImConversation")
    
    if ( iImHandler )
        {
        DP_IMP("Close the msg handler")
        iImHandler->Close();
        }
    
    // Release the imcache, all the conversation will lost here
    if ( iImCacheFactory )
        {
        DP_IMP("Release IM Cache")
        CIMCacheFactory::Release();
        iImCacheFactory = NULL;
        }

    iImCacheUpdater = NULL;
    iHost = NULL;
    }


// ---------------------------------------------------------------------------
// CSimpleImConversation::SetHost
// ---------------------------------------------------------------------------
//
void CSimpleImConversation::SetHost( MProtocolImDataHost& aHost )
    {
    DP_IMP("CSimpleImConversation::SetHost")
    
    iHost = &aHost;
    }


// ---------------------------------------------------------------------------
// CSimpleImConversation::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CSimpleImConversation::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions )
    {
    DP_IMP("CSimpleImConversation::GetInterface")
    
    if ( MProtocolImConversation::KInterfaceId == aInterfaceId )
        {
        return this;
        }
    
    if ( MXIMPBase::EPanicIfUnknown == aOptions )
        {
        User::Panic( KSimpleImPluginName, KErrExtensionNotSupported );
        }
    
    return NULL;
    }


// ---------------------------------------------------------------------------
// CSimpleImConversation::GetInterface() const
// ---------------------------------------------------------------------------
//
const TAny* CSimpleImConversation::GetInterface(
        TInt32 aInterfaceId, TIfGetOps aOptions ) const
    {
    DP_IMP("CSimpleImConversation::GetInterface")
    
    if ( MProtocolImConversation::KInterfaceId == aInterfaceId )
        {
        // caller wants this interface
        return const_cast<CSimpleImConversation*>( this );
        }
    
    if ( MXIMPBase::EPanicIfUnknown == aOptions )
        {
        User::Panic( KSimpleImPluginName, KErrExtensionNotSupported );
        }
    
    return NULL;
    }


// ---------------------------------------------------------------------------
// CSimpleImConversation::GetInterfaceId() const
// ---------------------------------------------------------------------------
//
TInt32 CSimpleImConversation::GetInterfaceId() const
    {
    DP_IMP("CSimpleImConversation::GetInterfaceId")
    
    return MProtocolImConversation::KInterfaceId;
    }


// ---------------------------------------------------------------------------
// From MProtocolImConversation.
// CSimpleImConversation::DoSendMessageL()
// ---------------------------------------------------------------------------
//
void CSimpleImConversation::DoSendMessageL(
    const MImConversationInfo& aImMessage,
    TXIMPRequestId aReqId )
    {
    DP_IMP( "CSimpleImConversation::DoSendMessageL" )
    
    iCompleted = EFalse;    
    
    TPtrC16 msgText16 = aImMessage.TextMessage();
    DP_IMP2( "CSimpleImConversation::DoSendMessageL, CONTENT: %S",
        &msgText16 )
    HBufC8* contentData = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
        msgText16 );
    CleanupStack::PushL( contentData );
    
    const MDesCArray& recipients = aImMessage.RecipientL();
    for ( TInt i = 0; i < recipients.MdcaCount(); i++ )
        {
        TPtrC16 recipientsPtr16 = recipients.MdcaPoint( i );
        HBufC8* recipientsData = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
                    recipientsPtr16 );
        CleanupStack::PushL( recipientsData );
        DP_IMP2( "CSimpleImConversation::DoSendMessageL, RECIPIENT: %S",
            &recipientsPtr16 )
        
        TInt result = iImHandler->SendInstantMessage( *contentData,
            *recipientsData );
        CleanupStack::PopAndDestroy( recipientsData );
        
        if ( KErrInUse == result )
            {
            User::Leave( KErrInUse );
            }
        }
    
    CleanupStack::PopAndDestroy( contentData );
    iXimpId = aReqId;
    }


// ---------------------------------------------------------------------------
// From MProtocolImConversation.
// CSimpleImConversation::DoForwardMessageL()
// ---------------------------------------------------------------------------
//
void CSimpleImConversation::DoForwardMessageL(
    const MImConversationInfo& /*aImMessage*/,
    TXIMPRequestId /*aReqId*/ )
    {
    DP_IMP( "CSimpleImConversation::DoForwardMessageL, NOT SUPPORTED" )
    
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------------------------
// From MProtocolImConversation.
// CSimpleImConversation::DoSendMessageToGroupL()
// ---------------------------------------------------------------------------
//
void  CSimpleImConversation::DoSendMessageToGroupL(
    const MImConversationInfo& /*aImMessage*/,
    const TDesC* /*aGroupId*/, TXIMPRequestId /*aReqId*/ )
    {
    DP_IMP( "CSimpleImConversation::DoSendMessageToGroupL, NOT SUPPORTED" )
    
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------------------------
// From MProtocolImConversation.
// CSimpleImConversation::DoBlockUsersL()
// ---------------------------------------------------------------------------
//
TXIMPRequestId CSimpleImConversation::DoBlockUsersL(
    const MDesCArray* /*aUserIds*/,
    TXIMPRequestId /*aReqId*/ )
    {
    DP_IMP( "CSimpleImConversation::DoBlockUsersL, NOT SUPPORTED" )
    
    User::Leave( KErrNotSupported );
    return TXIMPRequestId();
    }


// ---------------------------------------------------------------------------
// From MProtocolImConversation.
// CSimpleImConversation::DoUnBlockUsersL()
// ---------------------------------------------------------------------------
//
TXIMPRequestId CSimpleImConversation::DoUnBlockUsersL(
    const MDesCArray* /*aUserIds*/,
    TXIMPRequestId /*aReqId*/ )
    {
    DP_IMP( "CSimpleImConversation::DoUnBlockUsersL, NOT SUPPORTED" )
    
    User::Leave( KErrNotSupported );
    return TXIMPRequestId();
    }


// ---------------------------------------------------------------------------
// From MProtocolImConversation.
// CSimpleImConversation::DoGetBlockedListRequestL()
// ---------------------------------------------------------------------------
//
void CSimpleImConversation::DoGetBlockedListRequestL(
    const MXIMPIdentity& /*aImMessageId*/,
    TXIMPRequestId /*aReqId*/ )
    {
    DP_IMP( "CSimpleImConversation::DoGetBlockedListRequestL, NOT SUPPORTED" )
    
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------------------------
// From MProtocolImConversation.
// CSimpleImConversation::DoGetMessageListL()
// ---------------------------------------------------------------------------
//
void CSimpleImConversation::DoGetMessageListL(
    const MXIMPIdentity& /*aImMessageId*/,
    const TDesC* /*aGroupId*/,
    const TInt /*aMessageCount*/,
    TBool /*aDeliveryReportWanted*/,
    TXIMPRequestId /*aReqId*/ )
    {
    DP_IMP( "CSimpleImConversation::DoGetMessageListL, NOT SUPPORTED" )
    
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------------------------
// From MProtocolImConversation.
// CSimpleImConversation::DoRejectMessageL()
// ---------------------------------------------------------------------------
//
void CSimpleImConversation::DoRejectMessageL(
    const MXIMPIdentity& /*aImMessageId*/,
    const TDesC& /*aMessageID*/,
    TXIMPRequestId /*aReqId*/ )
    {
    DP_IMP( "CSimpleImConversation::DoRejectMessageL, NOT SUPPORTED" )
    
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------------------------
// From MSimpleInstantMsgObserver.
// CSimpleImConversation::SendInstantMsgReqComplete()
// ---------------------------------------------------------------------------
//
void CSimpleImConversation::SendInstantMsgReqComplete( 
    TInt /*aOpid*/, TInt aStatus)
    {
    DP_IMP("CSimpleImConversation::SendInstantMsgReqComplete")
    
    CompleteXimpReq( aStatus );
    }


// ---------------------------------------------------------------------------
// From MSimpleInstantMsgObserver.
// CSimpleImConversation::HandleReceiveTextMessage()
// ---------------------------------------------------------------------------
//
void CSimpleImConversation::HandleReceiveTextMessage( 
    TPtrC8 aFrom, TPtrC8 aContent )
    {
    DP_IMP( "CSimpleImConversation::HandleReceiveTextMessage" )
    
    TRAPD( err, DoHandleReceiveTextMessageL( aFrom, aContent ) )
    
    if ( KErrNone != err )
        {
        // Do nothing
        DP_IMP2( "CSimpleImConversation::HandleReceiveTextMessage, ERROR: %i",
            err )
        }
    }


// ---------------------------------------------------------------------------
// CSimpleImConversation::CompleteXimpReq()
// ---------------------------------------------------------------------------
//
void CSimpleImConversation::CompleteXimpReq( TInt aStatus )
    {
    DP_IMP( "CSimpleImConversation::CompleteXimpReq" )
    
    if ( iCompleted )
        {
        DP_IMP("CSimpleImConversation::CompleteXimpReq return")
        return;
        }
    
    iCompleted = ETrue;
    iConnectionObs->CompleteReq( iXimpId, aStatus );
    iXimpId = TXIMPRequestId();
    }


// ---------------------------------------------------------------------------
// CSimpleImConversation::DoHandleReceiveTextMessageL()
// ---------------------------------------------------------------------------
//
void CSimpleImConversation::DoHandleReceiveTextMessageL( TPtrC8 aFrom,
    TPtrC8 aContent )
    {
    DP_IMP( "CSimpleImConversation::DoHandleReceiveTextMessageL" )
    
    CreateImCacheUpdaterIfNeededL();
    
    if ( iHost )
        {
        MXIMPObjectFactory& objFact = iConnectionObs->Host()->ObjectFactory();
        MImObjectFactory& imObjFact = iHost->ImObjectFactory();
        
        MXIMPIdentity* identity = objFact.NewIdentityLC(); // CS:1
        MImConversationInfo* convinfo = imObjFact.
            NewImConversationInfoLC(); // CS:2
        
        HBufC16* sendername = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
            aFrom );
        CleanupStack::PushL( sendername ); // CS:3
        identity->SetIdentityL( *sendername );
        CleanupStack::PopAndDestroy( sendername ); // CS:2
        convinfo->SetMessageIdL( identity );
        
        HBufC16* messagecontent = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
            aContent );
        CleanupStack::PushL( messagecontent ); // CS:3
        convinfo->SetTextMessageL( *messagecontent );
        CleanupStack::PopAndDestroy( messagecontent ); // CS:2
        
        iHost->ConversationDataHost().HandleNewTextMessageL( convinfo );
        TRAP_IGNORE( iImCacheUpdater->AppendReceiveMessageL(
            identity->Identity(), convinfo->TextMessage() ) );
        
        CleanupStack::Pop(); // CS:1
        CleanupStack::Pop(); // CS:0
        }
    else
        {
        DP_IMP( " --> PROTOCOL IM DATA HOST IS NULL!" )
        }
    }

// ---------------------------------------------------------------------------
// CSimpleImConversation::ResolveUsernameL
// ---------------------------------------------------------------------------
//
void CSimpleImConversation::ResolveUsernameL( 
        TUint aServiceId, RBuf& aUserName ) const
    {
    DP_IMP( "CSimpleImConversation::ResolveUsernameL" )
    
    CCch* cchApi = CCch::NewLC();
    CCchService* service = cchApi->GetService( aServiceId );

    User::LeaveIfError( service->GetConnectionParameter( 
        ECCHUnknown, ECchUsername, aUserName ) );
    
    DP_IMP2( "CSimpleImConversation::ResolveUsernameL, USERNAME: %S",
        &aUserName )
    
    CleanupStack::PopAndDestroy( cchApi );
    }


// ---------------------------------------------------------------------------
// CSimpleImConversation::CreateImCacheUpdaterIfNeededL()
// ---------------------------------------------------------------------------
//
void CSimpleImConversation::CreateImCacheUpdaterIfNeededL()
    {
    if ( !iImCacheUpdater )
        {
        DP_IMP( "CSimpleImConversation::CreateImCacheUpdaterIfNeededL," )
        DP_IMP( "CREATING UPDATER" )
        
        RBuf username;
        CleanupClosePushL( username );
        const TInt KMaxUserNameLength = 255;
        username.CreateL( KMaxUserNameLength );
        ResolveUsernameL( iServiceId, username );
        
        iImCacheUpdater =
            iImCacheFactory->CreateUpdaterL( iServiceId, username, EFalse );
        CleanupStack::PopAndDestroy( &username );
        }
    }

// End of file
