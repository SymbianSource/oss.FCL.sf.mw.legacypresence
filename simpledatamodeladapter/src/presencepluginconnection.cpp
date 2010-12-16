/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <badesca.h>
#include <ximpbase.h>
#include <ximpserviceinfo.h>
#include <protocolpresencefeatures.h>
#include <cprotocolimfeatures.h>
#include <protocolpresencedatahost.h>
#include <presenceobjectfactory.h>  

#include <ximpcontextclientinfo.h>		
#include <protocolpresenceauthorization.h>  
#include <ximpprotocolconnectionhost.h>			   
#include <ximpfeatureinfo.h> //for etc. KPublish
#include <msimpleconnection.h> //Part of Registration API from SIMPLE engine
#include <msimpleconnectionobserver.h>//Connection callback methods
#include <presenceinfo.h>

#include "presencepluginconnection.h"
#include "presencepluginsession.h" 
#include "mpresencepluginconnectionobs.h"   
#include "presencepluginwatcher.h"
#include "presencepluginauthorization.h"
#include "presencepluginwinfo.h"
#include "presencepluginpublisher.h"
#include "presenceplugingroup.h"
#include "presencepluginxdmutils.h"
#include "presencepluginvirtualgroup.h"
#include "presenceconnectioninfo.h"
#include "presenceplugindata.h"

#include "simpleimpluginuids.hrh" //KImplUidSimpleImPlugin

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginConnection::CPresencePluginConnection()
// ---------------------------------------------------------------------------
//
CPresencePluginConnection::CPresencePluginConnection( ): 
	CActive( CActive::EPriorityHigh ), iSipPresentity(NULL),
    iStopPublishState( EFalse ), iGrantListSubs( EFalse ), iStopPublishCall( EFalse ),
    iTerminatedCall( EFalse )
    {
    DP_SDA("CPresencePluginConnection::CPresencePluginConnection");
    CActiveScheduler::Add(this);
    iOwnTupleId = 0;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::NewL()
// ---------------------------------------------------------------------------
//
CPresencePluginConnection* CPresencePluginConnection::NewL(
    const MXIMPServiceInfo& aServiceInfo,
    const MXIMPContextClientInfo& aClientCtxInfo )
    {
    DP_SDA("CPresencePluginConnection::NewL");
    CPresencePluginConnection* self = 
        new( ELeave ) CPresencePluginConnection( );
    CleanupStack::PushL( self );
    self->ConstructL( aServiceInfo, aClientCtxInfo );
    CleanupStack::Pop( self );    
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::NewL() loaded
// ---------------------------------------------------------------------------
//
CPresencePluginConnection* CPresencePluginConnection::NewL(
    const MXIMPServiceInfo& aServiceInfo,
    const MXIMPContextClientInfo& aClientCtxInfo,
    const TDesC8& aETag )
    {
    DP_SDA("CPresencePluginConnection::NewL loaded");
    CPresencePluginConnection* self = 
        new( ELeave ) CPresencePluginConnection( );
    CleanupStack::PushL( self );
    self->ConstructL( aServiceInfo, aClientCtxInfo, aETag );
    CleanupStack::Pop( self );    
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::ConstructL() loaded
// ---------------------------------------------------------------------------
//
void CPresencePluginConnection::ConstructL(
    const MXIMPServiceInfo& aServiceInfo,
    const MXIMPContextClientInfo& /*aClientCtxInfo*/,
    const TDesC8& aETag )
    {
    DP_SDA("CPresencePluginConnection::ConstructL loaded");
    
    iETag = aETag.AllocL();                  
    
    iSession = CPresencePluginSession::NewL( aServiceInfo, *this );
            
    // create instance from data class
    iPresenceData = CPresencePluginData::NewL( *this,
        aServiceInfo.IapId() ); 
    
    DP_SDA("ConstructL create subscribed buddy list loaded ");  
    // create another virtual group for all subscribed buddyes
    iSubscribedBuddys =
        CPresencePluginVirtualGroup::NewL( *this, 
        KPresenceBuddyList(),
        iPresenceData );

    iAuth = CPresencePluginAuthorization::NewL( *this, iPresenceData );
    
    iPublisher = CPresencePluginPublisher::NewL(
               *this,        
               *iSession->SimpleConnection(),
               iPresenceData );
    
    iWatcher = CPresencePluginWatcher::NewL(
               *this,
               *iSession->SimpleConnection(),
               iPresenceData ); 
    
    iGroups = CPresencePluginGroups::NewL( 
        *this, iSubscribedBuddys, iPresenceData );
    
    iPluginWinfo = CPresencePluginWinfo::NewL( 
                 *this, *(iSession->SimpleConnection() ));
    
    }    

// ---------------------------------------------------------------------------
// CPresencePluginConnection::ConstructL()
// ---------------------------------------------------------------------------
//
void CPresencePluginConnection::ConstructL(
    const MXIMPServiceInfo& aServiceInfo,
    const MXIMPContextClientInfo& /*aClientCtxInfo*/ )
    {
    DP_SDA("CPresencePluginConnection::ConstructL");
     
    iETag = KNullDesC8().AllocL();                  
    
    iSession = CPresencePluginSession::NewL( aServiceInfo, *this );
            
    // create instance from data class
    iPresenceData = CPresencePluginData::NewL( *this, 
        aServiceInfo.IapId() ); 

    DP_SDA("ConstructL create subscribed buddy list");	
    // create another virtual group for all subscribed buddyes
    iSubscribedBuddys =
    	CPresencePluginVirtualGroup::NewL( 
    	    *this, KPresenceBuddyList(), iPresenceData );

    iAuth = CPresencePluginAuthorization::NewL( *this, iPresenceData );
    
    iPublisher = CPresencePluginPublisher::NewL(
               *this,        
               *iSession->SimpleConnection(),
               iPresenceData );
    
    iWatcher = CPresencePluginWatcher::NewL(
               *this,
               *iSession->SimpleConnection(),
               iPresenceData ); 
      
    iGroups = CPresencePluginGroups::NewL( 
        *this, iSubscribedBuddys, iPresenceData );
    
    iPluginWinfo = CPresencePluginWinfo::NewL( 
                 *this, *(iSession->SimpleConnection() ));   
                                      
    }    

// ---------------------------------------------------------------------------
// CPresencePluginConnection::~CPresencePluginConnection()
// ---------------------------------------------------------------------------
//
CPresencePluginConnection::~CPresencePluginConnection()
    {
    DP_SDA("CPresencePluginConnection::~CPresencePluginConnection");
    
    Cancel();
    
    delete iSipPresentity;
    iSipPresentity = NULL;
    
    delete iPluginWinfo;
    iPluginWinfo = NULL;
    
    delete iWatcher;
    iWatcher = NULL;
    
    delete iPublisher;
    iPublisher = NULL;
    
    delete iAuth;
    iAuth = NULL;
    
    delete iGroups;
    iGroups = NULL;
    
    delete iImFeatures;
    
    delete iSession;
    iSession = NULL;
        
    REComSession::FinalClose();
    
    delete iSubscribedBuddys;
    delete iPresenceData;
    
    delete iETag;
    iETag = NULL;
    DP_SDA("CPresencePluginConnection::~CPresencePluginConnection end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::PrimeHost
// ---------------------------------------------------------------------------
//  
void CPresencePluginConnection::PrimeHost( 
    MXIMPProtocolConnectionHost& aHost )
    {
    DP_SDA("CPresencePluginConnection::PrimeHost");
    iConnectionHost = &aHost;
    iAuth->SetDataHost( 
        iConnectionHost->ProtocolPresenceDataHost().AuthorizationDataHost() );  
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::GetOnlineBuddyGroup()
// ---------------------------------------------------------------------------
//
CPresencePluginVirtualGroup* CPresencePluginConnection::SubscribedContacts( )
    {
    return iSubscribedBuddys;
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::GetSessionStatus()
// ---------------------------------------------------------------------------
//
TBool CPresencePluginConnection::GetSessionStatus( )
    {
    return iSession->ConnectionStatus();
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::TupleIdGet()
// ---------------------------------------------------------------------------
//
TInt CPresencePluginConnection::GetTupleId() const
    {
    DP_SDA("CPresencePluginConnection::TupleIdGet");
    return iOwnTupleId;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::GetEtag()
// ---------------------------------------------------------------------------
//    
const TDesC8& CPresencePluginConnection::GetETag() const
    {
    DP_SDA("CPresencePluginConnection::GetETag");
    
    if( !iETag )
        {
        DP_SDA2("CPresencePluginConnection::GetETag empty ??? %S", iConnectionArray->GetConnectionEtag() );
        return  KNullDesC8();
        }

    return *iETag;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::SetETag()
// ---------------------------------------------------------------------------
//    
void CPresencePluginConnection::SetETag( const TDesC8& aETag )
    {
    DP_SDA("CPresencePluginConnection::SetETag");
    delete iETag;
    iETag = NULL;
        
    TRAP_IGNORE( ( iETag = aETag.AllocL() ) );
    //Add etag also in connectionArray
    iConnectionArray->SetConnectionETag( *iETag );
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::SetConnectionArray()
// ---------------------------------------------------------------------------
//    
void CPresencePluginConnection::SetConnectionArray( 
		CPresenceConnectionInfo* aArray )
    {
    DP_SDA("CPresencePluginConnection::SetConnectionArray");
    iConnectionArray = aArray;
    iConnectionArray->SetConnectionStatus( EActive );
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::GetConnectionArray()
// ---------------------------------------------------------------------------
//    
CPresenceConnectionInfo* CPresencePluginConnection::GetConnectionArray( )
    {
    DP_SDA("CPresencePluginConnection::GetConnectionArray");
    return iConnectionArray;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::TupleIdGet()
// ---------------------------------------------------------------------------
//
TInt32 CPresencePluginConnection::GetPresenceSetId() const
    {
    DP_SDA("CPresencePluginConnection::GetPresenceSetId");
    return iSession->GetPresenceSetId();
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::TupleIdGet()
// ---------------------------------------------------------------------------
//
void CPresencePluginConnection::SetTupleId( const TInt aTupleId )
    {
    DP_SDA("CPresencePluginConnection::TupleIdSet");
    iOwnTupleId = aTupleId;
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::GetStopPublishState()
// ---------------------------------------------------------------------------
//    
TBool CPresencePluginConnection::GetStopPublishState()
    {
    DP_SDA2("::GetStopPublishState = %d", (TInt)iStopPublishState );
    return iStopPublishState;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::SetStopPublishState()
// ---------------------------------------------------------------------------
// 
void CPresencePluginConnection::SetStopPublishState( TBool aState )
    {
    DP_SDA2("SetStopPublishState : aState = %d ",  (TInt)aState );
    iStopPublishState = aState;
    }
            
// ---------------------------------------------------------------------------
// CPresencePluginConnection::PresenceWatching
// ---------------------------------------------------------------------------
//    
MProtocolPresenceDataHost& CPresencePluginConnection::ProtocolPresenceHost()
    {
    DP_SDA("CPresencePluginConnection::ProtocolPresenceHost");
    return iConnectionHost->ProtocolPresenceDataHost();
    }
     
// ---------------------------------------------------------------------------
// CPresencePluginConnection::PresenceWatching
// ---------------------------------------------------------------------------
//    
MProtocolPresenceWatching& CPresencePluginConnection::PresenceWatching()
    {
    DP_SDA("CPresencePluginConnection::PresenceWatching");
    return *iWatcher;
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::PresencePublishing
// ---------------------------------------------------------------------------
//   
 MProtocolPresencePublishing& CPresencePluginConnection::PresencePublishing()
    {
    return *iPublisher;
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::PresentityGroups
// ---------------------------------------------------------------------------
//   
MProtocolPresentityGroups& CPresencePluginConnection::PresentityGroups()
    {
    return *iGroups;
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::PresenceAuthorization
// ---------------------------------------------------------------------------
//   
MProtocolPresenceAuthorization& 
    CPresencePluginConnection::PresenceAuthorization()
    {
    DP_SDA2("CPresencePluginConnection::PresenceAuthorization %x", iAuth);
    return *iAuth;
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::InternalPresenceAuthorization
// ---------------------------------------------------------------------------
//       
CPresencePluginAuthorization& 
    CPresencePluginConnection::InternalPresenceAuthorization()
    {
    DP_SDA2("CPresencePluginConnection::InternalPresenceAuthorization %x", iAuth);
    return *iAuth;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::OpenSessionL
// ---------------------------------------------------------------------------
//     
void CPresencePluginConnection::OpenSessionL( 
    const MXIMPContextClientInfo& /*aClientCtxInfo*/,
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginConnection::OpenSessionL");
    iGrantListSubs = ETrue; 
    iTerminatedCall = EFalse;
    iXIMPId = aReqId;
    iSession->OpenSessionL();         
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::OpenSessionL
// ---------------------------------------------------------------------------
// 
void CPresencePluginConnection::OpenSessionL( const TInt& /*aSettingsId*/,
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginConnection::OpenSessionL 2 ");
    iXIMPId = aReqId;
    iGrantListSubs = ETrue; 
    iTerminatedCall = EFalse;
    iSession->OpenSessionL( ); 
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::CloseSession
// ---------------------------------------------------------------------------
//    
void CPresencePluginConnection::CloseSession( 
    const MXIMPContextClientInfo& /*aContextClient*/,
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginConnection::CloseSession begin");
    
    TBool completeXimpRequestNow( EFalse );
    iConnectionArray->DecreaseClientCount();
    iXIMPId = aReqId;
        
    // Client count can´t be negative. Increase if goes negative.
    // This happens for example in network lost case.
    // This method is called then because connection is terminated and
    // for all clients which have done unbind. But here we cannot tell
    // if this method is called by clients unbind or connection termination
    // therefore this workaround needed.
    if ( iConnectionArray->ClientCount() < 0 )
        {        
        completeXimpRequestNow = ETrue;
        iConnectionArray->IncreaseClientCount();
        }
    
    // In case there is no more clients we cannot complete Ximp request
    // right away. We have to stop publish first, Ximp request completion is
    // handled in RunL after that is done.
    if( KErrNone == iConnectionArray->ClientCount() && 
        (EActive == iConnectionArray->GetConnectionStatus()) )
        {       
        completeXimpRequestNow = EFalse; // request is completed later in RunL
        iGrantListSubs = EFalse;
        
        //Start stopPublish request
        DeRegister();
        }
        
    if ( !iStopPublishCall )
       {
       completeXimpRequestNow = ETrue;
       }
    
    if ( completeXimpRequestNow )
        {
        CompleteReq( iXIMPId, KErrNone );
        }

    DP_SDA("CPresencePluginConnection::CloseSession end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::GetSupportedFeaturesL
// ---------------------------------------------------------------------------
//     
void CPresencePluginConnection::GetSupportedFeaturesL( 
    CDesC8Array& aFeatures ) const
    {
    DP_SDA("CPresencePluginConnection::GetSupportedFeaturesL");
    // first empty the whole array
    aFeatures.Reset();   
    using namespace NXIMPFeature::Presence;    
    aFeatures.AppendL( KPublish );
    aFeatures.AppendL( KSubscribe );    
    aFeatures.AppendL( KUnsubscribe );  
    aFeatures.AppendL( KAddContact );
    aFeatures.AppendL( KDeleteContact );
    aFeatures.AppendL( KFetch );
    aFeatures.AppendL( KBlock );
    aFeatures.AppendL( KUnBlock );
    
    if( iImFeatures )
        {
        using namespace NXIMPFeature::InstantMessage;
        aFeatures.AppendL( KInstantMessage );
        }
    }

//*****************************************************************************
//*  FROM MPresencePluginConnectionObs
//*****************************************************************************  
// ---------------------------------------------------------------------------
// CPresencePluginConnection::CompleteReq()
// ---------------------------------------------------------------------------
//
void CPresencePluginConnection::CompleteReq( TReqType aType, TInt aStatus )
    {
    DP_SDA("CPresencePluginConnection::CompleteReq");
    // Return immediately if PrimeHost() is not called.
    if ( !iConnectionHost )
        {
        DP_SDA("CPresencePluginConnection::CompleteReq !iConnectionHost");
        iXIMPId = TXIMPRequestId(); 
        return;
        }
                      
    switch ( aType )
        {
        case EOpenSess:
            {
            DP_SDA("CPresencePluginConnection::CompleteReq EOpenSess");          
            iConnectionHost->HandleRequestCompleted( iXIMPId, aStatus );
            iXIMPId = TXIMPRequestId();
            //Set connection status to connection Array
            iConnectionArray->SetConnectionStatus( EActive );
            }
            break;
        case ECloseSess:
            {
            DP_SDA("CPresencePluginConnection::CompleteReq ECloseSess"); 
            iConnectionArray->DecreaseClientCount();
            CompleteReq( iXIMPId, aStatus );
            iConnectionArray->SetConnectionStatus( ENotActive );
            }
            break;
        default:
            DP_SDA("CPresencePluginConnection::CompleteReq DEFAULT");  
            break;   
        };
    DP_SDA("CPresencePluginConnection::CompleteReq end"); 
    }
        
// ---------------------------------------------------------------------------
// CPresencePluginConnection::CompleteReq()
// ---------------------------------------------------------------------------
//
void CPresencePluginConnection::CompleteReq( 
    TXIMPRequestId aReqId,
    TInt aStatus )
    { 
    DP_SDA2("CPresencePluginConnection::CompleteReq status %d", aStatus); 
    // Return immediately if PrimeHost() is not called.
    if ( !iConnectionHost )
        {
        DP_SDA("CPresencePluginConnection::CompleteReq return"); 
        return;
        }               
    DP_SDA("CPresencePluginConnection::CompleteReq complete");               
    iConnectionHost->HandleRequestCompleted( aReqId, aStatus );
    } 

// ---------------------------------------------------------------------------
// CPresencePluginConnection::ObjectFactory()
// ---------------------------------------------------------------------------
// 
MXIMPObjectFactory& CPresencePluginConnection::ObjectFactory()
    {
    DP_SDA("CPresencePluginConnection::ObjectFactory");
    return iConnectionHost->ObjectFactory();
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::Host
// ---------------------------------------------------------------------------
//     
MXIMPProtocolConnectionHost* CPresencePluginConnection::Host()
    {
    DP_SDA("CPresencePluginConnection::Host");
    return iConnectionHost;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection:CurrentDomain
// ---------------------------------------------------------------------------
//     
TPtrC16 CPresencePluginConnection::CurrentDomain()
    {
    return iSession->CurrentDomain();    
    } 
     
// ---------------------------------------------------------------------------
// CPresencePluginConnection::CurrentSipPresentit8()
// ---------------------------------------------------------------------------
// 
TPtrC8 CPresencePluginConnection::CurrentSipPresentity8()
    {
    return iSession->CurrentSipPresentity();
    } 
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection:XdmUtilsL
// ---------------------------------------------------------------------------
//
CPresencePluginXdmUtils* CPresencePluginConnection::XdmUtilsL()
    {
    DP_SDA("CPresencePluginConnection::XdmUtilsL");
    return iSession->XdmUtilsL();
    } 
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::WinfoHandlerL()
// ---------------------------------------------------------------------------
//
CPresencePluginWinfo* CPresencePluginConnection::WinfoHandlerL()
    {
    DP_SDA("CPresencePluginConnection::WinfoHandlerL");
    if ( !iPluginWinfo )
        {
        iPluginWinfo = CPresencePluginWinfo::NewL( 
            *this, *(iSession->SimpleConnection() ));
        }
    
    iPluginWinfo->SetHost( iConnectionHost );
    return iPluginWinfo;
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::WatcherHandlerL()
// ---------------------------------------------------------------------------
//    
 CPresencePluginWatcher* CPresencePluginConnection::WatcherHandlerL()
    {
    DP_SDA("CPresencePluginConnection::WatcherHandlerL");  
    if ( !iWatcher )
        {
        DP_SDA("CPresencePluginConnection::WatcherHandlerL: if ( !iWatcher )");  
        iWatcher = CPresencePluginWatcher::NewL(
            *this,
            *iSession->SimpleConnection(),
            iPresenceData );
        }
    return iWatcher;
    }
// ---------------------------------------------------------------------------
// CPresencePluginConnection::WinfoTerminatedL()
// ---------------------------------------------------------------------------
//
void CPresencePluginConnection::WinfoTerminatedL( TInt aReason )
    {
    DP_SDA("CPresencePluginConnection::WinfoTerminatedL");  
    // Pass the information to iAuth and iPublisher,
    // so that they can call XIMP Plugin Data Host callbacks.
    iAuth->WinfoTerminatedL( aReason );
    iPublisher->WinfoTerminatedL( aReason );
    }  
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::WinfoNotification()
// ---------------------------------------------------------------------------
//
void CPresencePluginConnection::WinfoNotification( MSimpleWinfo& aWinfo )
    {
    DP_SDA("CPresencePluginConnection::WinfoNotification");  
    TRAP_IGNORE( iPublisher->WinfoNotificationL( aWinfo  ));
    TRAP_IGNORE( iAuth->WinfoNotificationL( aWinfo ));
    }      
        
// ---------------------------------------------------------------------------
// CPresencePluginConnection::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CPresencePluginConnection::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions )
    { 
    DP_SDA("CPresencePluginConnection::GetInterface()");  
    if ( aInterfaceId == GetInterfaceId() )
        {
        DP_SDA("CPresencePluginConnection::GetInterface()if ");
        // caller wants this interface 
        return this;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        DP_SDA("CPresencePluginConnection::GetInterface() panic");  
        User::Panic( KPluginName, KErrExtensionNotSupported );
        }  
    return NULL;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CPresencePluginConnection::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions ) const
    {
    DP_SDA("CPresencePluginConnection::GetInterface() const ");  
    if ( aInterfaceId == GetInterfaceId() )
        {
        DP_SDA("CPresencePluginConnection::GetInterface()2 ");  
        // caller wants this interface
        return const_cast<CPresencePluginConnection*>(this);
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        DP_SDA("CPresencePluginConnection::GetInterface()panic 2");  
        User::Panic( KPluginName, KErrExtensionNotSupported );
        }  
    return NULL;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::GetInterfaceId()
// ---------------------------------------------------------------------------
//
TInt32 CPresencePluginConnection::GetInterfaceId() const
    {
    DP_SDA("CPresencePluginConnection::GetInterfaceId() const ");  
    return MXIMPProtocolConnection::KInterfaceId;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::ProtocolPresenceFeatures()
// ---------------------------------------------------------------------------
//    
MProtocolPresenceFeatures& CPresencePluginConnection::
    ProtocolPresenceFeatures()           
    {
    DP_SDA("CPresencePluginConnection::ProtocolPresenceFeatures() ");  
    return *this;
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::PresenceObjectFactory()
// ---------------------------------------------------------------------------
// 
MPresenceObjectFactory& CPresencePluginConnection::PresenceObjectFactoryOwn()
    {
    DP_SDA("CPresencePluginConnection::PresenceObjectFactoryOwn() ");  
    return iConnectionHost->
        ProtocolPresenceDataHost().PresenceObjectFactory();    
    }  

// ---------------------------------------------------------------------------
// CPresencePluginConnection::GrandListState()
// ---------------------------------------------------------------------------
// 
TBool CPresencePluginConnection::GrandListState( )
    {
    DP_SDA2("GrandListState : iGrantListSubs = %d", (TInt)iGrantListSubs );
    return iGrantListSubs;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::IsStopPublishCalled()
// ---------------------------------------------------------------------------
// 
TBool CPresencePluginConnection::IsStopPublishCalled( )
    {
    DP_SDA("CPresencePluginConnection::IsStopPublishCalled");
    return iStopPublishCall;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::TerminateConnectionL()
// ---------------------------------------------------------------------------
// 
void CPresencePluginConnection::TerminateConnectionL( )
    {
    DP_SDA("CPresencePluginConnection::TerminateConnectionL");
    iConnectionHost->HandleConnectionTerminated( NULL );
    iConnectionArray->SetConnectionStatus( ETerminated );
    DP_SDA("CPresencePluginConnection::TerminateConnectionL Done");
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::DeRegister()
// ---------------------------------------------------------------------------
// 
void CPresencePluginConnection::DeRegister( )
    {
    DP_SDA("CPresencePluginConnection::Deregister");
    
    DP_SDA(" -> CloseSession, delete permanent pres data");
    TRAP_IGNORE( iPresenceData->DeletePresenceVariablesL( ServiceId() ) );
    
    // Try to stop publish only if we have successfully published
    if ( iPublisher->Published() )
        {
        DP_SDA("CloseSession call stopPublish");
        TRAP_IGNORE( iPublisher->StopPublishL( iStatus ) );
        iStopPublishCall = ETrue;
        SetActive();
        }
    
    DP_SDA("CPresencePluginConnection::Deregister Done");
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::CreateImFeaturesPlugin()
// ---------------------------------------------------------------------------
//
void CPresencePluginConnection::CreateImFeaturesPluginL( )
    {
    if ( !iImFeatures )
        {
        DP_SDA("Creating SimpleImPlugin");
        
        CSimpleImFeatures::TSimpleImPluginInitParams imPluginInitParams( 
            *this,
            *iSession->SimpleConnection(),
            iSession->ServiceId() );
        
        iImFeatures = static_cast<CSimpleImFeatures*> ( 
            CProtocolImFeatures::NewL( TUid::Uid(KImplUidSimpleImPlugin),
            &imPluginInitParams ) );
        
        DP_SDA("Creating SimpleImPlugin done");
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::DeleteImFeaturesPlugin()
// ---------------------------------------------------------------------------
//
void CPresencePluginConnection::DeleteImFeaturesPlugin( )
    {
    if ( iImFeatures )
        {
        DP_SDA("Deleting SimpleImPlugin");
        
        delete iImFeatures;
        iImFeatures = NULL;      
        REComSession::FinalClose();
        
        DP_SDA("Deleting SimpleImPlugin done");
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::DoCancel()
// ---------------------------------------------------------------------------
//
void CPresencePluginConnection::DoCancel(  )
    {
    DP_SDA("CPresencePluginConnection::DoCancel CANCEL");
    iPublisher->Cancel();
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::RunL()
// ---------------------------------------------------------------------------
//
void CPresencePluginConnection::RunL(  )
    {
    DP_SDA("CPresencePluginConnection::RunL");
    TInt status = iStatus.Int();
    DP_SDA2("CPresencePluginConnection::RunL status %d", status );
    
    if ( iStopPublishCall )
        {
        DP_SDA("CPresencePluginConnection::RunL complete");     
        
        iStopPublishCall = EFalse;
        CompleteReq( iXIMPId, status );      
        
        DP_SDA("CPresencePluginConnection::RunL --> REMOVE CACHE");
        iPresenceData->RemoveCacheL();
        
        DP_SDA("CPresencePluginConnection::RunL --> Delete im plugin");
        DeleteImFeaturesPlugin();
        }
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::RunError
// ---------------------------------------------------------------------------
//
TInt CPresencePluginConnection::RunError( TInt /*aError*/ )
    {
    DP_SDA("CPresencePluginConnection::RunError"); 
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CPresencePluginConnection::GroupsL()
// ---------------------------------------------------------------------------
// 
CPresencePluginGroups& CPresencePluginConnection::GroupsL()
	{
	User::LeaveIfNull( iGroups );
	return *iGroups;
	}
    
// ---------------------------------------------------------------------------
// CPresencePluginConnection::ServiceId()
// ---------------------------------------------------------------------------
// 	
TInt& CPresencePluginConnection::ServiceId()
    {
    return iSession->ServiceId();
    }
	
// ---------------------------------------------------------------------------
// CPresencePluginConnection::GetProtocolInterface()
// ---------------------------------------------------------------------------
// 
TAny* CPresencePluginConnection::GetProtocolInterface( TInt aInterfaceId )
	{
	DP_SDA("CPresencePluginConnection::GetProtocolInterface");
	
	if( aInterfaceId == CProtocolImFeatures::KInterfaceId )
        {
        TInt err( KErrNone );
        
        if ( !iImFeatures )
            {
            DP_SDA("    --> Create im features plugin");           
            TRAP( err, CreateImFeaturesPluginL() );
            }
        
        if ( !err )
            {
            DP_SDA("    --> get interface");
            
            return iImFeatures->GetInterface( 
                aInterfaceId, MXIMPBase::EPanicIfUnknown );
            }
        else
            {
            DP_SDA("    --> Creating im features plugin failed");
            return NULL;
            }
        }
	else
        {
	    return NULL;
        }
	}
    
// End of file
