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
* Description:    SIMPLE Protocol implementation for Presence Framework
*
*/




#include <e32std.h>

#include <ximpstatus.h>
#include <ximpserviceinfo.h>
#include <ximpcontextclientinfo.h>
#include <ximpprotocolconnectionhost.h>
#include <ximpfeatureinfo.h>
#include <ximpidentity.h>
#include <ximperrors.hrh>
#include <presenceerrors.hrh>

#include <protocolpresencedatahost.h>

#include "simplepluginconnection.h"
#include "simplepluginsession.h"
#include "simpleplugindebugutils.h"
#include "simplepluginauthorization.h"
#include "simplepluginwinfo.h"
#include "simplepluginpublisher.h"
#include "simplepluginwatcher.h"
#include "simpleplugingroups.h"
#include "simplepluginxdmutils.h"
#include "simplepluginvariation.h"
#include "msimplepluginconnectionobs.h"
#include "msimplepluginsettings.h"
#include "simpleerrors.h"

class MXIMPObjectFactory;

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSimplePluginConnection::CSimplePluginConnection
// ---------------------------------------------------------------------------
//
CSimplePluginConnection::CSimplePluginConnection()
: iSipPresentity(NULL)
    {
    }

// ---------------------------------------------------------------------------
// CSimplePluginConnection::NewL
// ---------------------------------------------------------------------------
//
CSimplePluginConnection* CSimplePluginConnection::NewL( 
    const MXIMPServiceInfo& aServiceInfo,
    const MXIMPContextClientInfo& aClientCtxInfo )
    {
    CSimplePluginConnection* self = new( ELeave ) CSimplePluginConnection(  );
    CleanupStack::PushL( self );
    self->ConstructL( aServiceInfo, aClientCtxInfo );
    CleanupStack::Pop( self );    
    return self;
    }
    
// ---------------------------------------------------------------------------
// CSimplePluginConnection::ConstructL
// ---------------------------------------------------------------------------
//
void CSimplePluginConnection::ConstructL( 
    const MXIMPServiceInfo& aServiceInfo,
    const MXIMPContextClientInfo& /* aClientCtxInfo */ )
    {                  
    iSession = CSimplePluginSession::NewL( aServiceInfo, *this ); 
   
    iAuth = CSimplePluginAuthorization::NewL( *this );   
        
    iPublisher = CSimplePluginPublisher::NewL(
        *this, *this,        
        *iSession->SimpleConnection() ); 
        
    iWatcher = CSimplePluginWatcher::NewL(
        *this, *this,
        *iSession->SimpleConnection() ); 
        
    iGroups = CSimplePluginGroups::NewL( 
        *this );
    
    iPluginWinfo = CSimplePluginWinfo::NewL( 
        *this, *(iSession->SimpleConnection()));  
        
    iVariation = CSimplePluginVariation::NewL();    
    iVariation->InitL();      
    }    

// ---------------------------------------------------------------------------
// CSimplePluginConnection::~CSimplePluginConnection
// ---------------------------------------------------------------------------
//
CSimplePluginConnection::~CSimplePluginConnection()
    {     
    
    delete iSipPresentity;
    
    delete iVariation;                   
    delete iPluginWinfo;    
    delete iWatcher;
    delete iPublisher;
    delete iAuth;   
    delete iGroups; 
    delete iSession;       
    
    delete iXdmUtils;    
    }

// ---------------------------------------------------------------------------
// CSimplePluginConnection::PrimeHost
// ---------------------------------------------------------------------------
//
void CSimplePluginConnection::PrimeHost( MXIMPProtocolConnectionHost& aHost )
    {
    iConnectionHost = &aHost;
    iAuth->SetDataHost( iConnectionHost->ProtocolPresenceDataHost().AuthorizationDataHost() );
    }
    
    
// ---------------------------------------------------------------------------
// CSimplePluginConnection::ProtocolPresenceFeatures
// ---------------------------------------------------------------------------
//  
MProtocolPresenceFeatures& CSimplePluginConnection::ProtocolPresenceFeatures()
    {
    return *this;
    }
    
/*<branchInfo originator="gronoff" since="08-06-06" category="new_feature" error=”none”>
<pf_5250_OFF_CYCLE_SW_INT>
</branchInfo>*/
// BRANCH 08-06-17 : gronoff
// ---------------------------------------------------------------------------
// CPresencePluginConnection::GetProtocolInterface()
// ---------------------------------------------------------------------------
// 
TAny* CSimplePluginConnection::GetProtocolInterface( TInt /*aInterfaceId*/ )
	{
	return NULL;
	}
// BRANCH_END 08-06-17 : gronoff   

// ---------------------------------------------------------------------------
// CSimplePluginConnection::PresenceWatching
// ---------------------------------------------------------------------------
//
MProtocolPresenceWatching& CSimplePluginConnection::PresenceWatching()
    {   
    return *iWatcher;
    }
// ---------------------------------------------------------------------------
// CSimplePluginConnection::PresencePublishing
// ---------------------------------------------------------------------------
//
MProtocolPresencePublishing& CSimplePluginConnection::PresencePublishing()
    {
    return *iPublisher;
    }
// ---------------------------------------------------------------------------
// CSimplePluginConnection::PresentityGroups
// ---------------------------------------------------------------------------
//
MProtocolPresentityGroups& CSimplePluginConnection::PresentityGroups()
    {
    return *iGroups;
    }
// ---------------------------------------------------------------------------
// CSimplePluginConnection::PresenceAuthorization
// ---------------------------------------------------------------------------
//
MProtocolPresenceAuthorization& CSimplePluginConnection::PresenceAuthorization()
    {      
    return *iAuth;
    }
    
    
// ---------------------------------------------------------------------------
// CSimplePluginConnection::OpenSessionL
// ---------------------------------------------------------------------------
//
void CSimplePluginConnection::OpenSessionL(
    const TInt& aSettingsId,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginConnection: OpenSessionL"));
#endif             
    iPrFwId = aReqId;
    TRAPD( err, iSession->OpenSessionL( aSettingsId )); 
    if ( err )
        {
        User::Leave( CSimplePluginConnection::HarmonizeErrorCode( err ));
        }     
    }  

// ---------------------------------------------------------------------------
// CSimplePluginConnection::OpenSessionL
// ---------------------------------------------------------------------------
//
void CSimplePluginConnection::OpenSessionL( 
    const MXIMPContextClientInfo& /*aClientCtxInfo*/,
    TXIMPRequestId /*aReqId*/ )
    {   
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginConnection: OpenSessionL - error: not supported"));
#endif    
    User::Leave( KErrNotSupported );           
    }

// ---------------------------------------------------------------------------
// CSimplePluginConnection::CloseSession
// ---------------------------------------------------------------------------
//
void CSimplePluginConnection::CloseSession( 
    const MXIMPContextClientInfo& /*aClientCtxInfo*/,
    TXIMPRequestId aReqId )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginConnection: CloseSession"));
#endif          
    iPrFwId = aReqId;
    
    delete iPluginWinfo;
    iPluginWinfo = NULL;    
    delete iWatcher;
    iWatcher = NULL;
    delete iPublisher;
    iPublisher = NULL;
    delete iAuth;    
    iAuth = NULL;
    delete iSession;    
    iSession = NULL;
    delete iGroups;
    iGroups = NULL; 
    delete iXdmUtils;
    iXdmUtils = NULL; 
    
    CompleteReq( iPrFwId, KErrNone );          
    }
// ---------------------------------------------------------------------------
// CSimplePluginConnection::GetSupportedFeaturesL
// ---------------------------------------------------------------------------
//
void CSimplePluginConnection::GetSupportedFeaturesL( CDesC8Array& aFeatures ) const
    {
    // first empty the whole array
    aFeatures.Reset();   
    using namespace NXIMPFeature::Presence;    
    aFeatures.AppendL( KPublish );
    aFeatures.AppendL( KFetch );    
    aFeatures.AppendL( KSubscribe );    
    aFeatures.AppendL( KUnsubscribe  );    
    }
    
// ---------------------------------------------------------------------------
// CSimplePluginConnection::CompleteReq
// ---------------------------------------------------------------------------
//
void CSimplePluginConnection::CompleteReq( TReqType aType, TInt aStatus )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginConnection: CompleteReq stat=%d"), aStatus);
#endif         

    TInt retVal = HarmonizeErrorCode( aStatus );
    
    // Return immediately if PrimeHost() is not called. This is for testing purposes. 
    if ( !iConnectionHost )
        {
        iPrFwId = TXIMPRequestId(); 
        return;
        }
                      
    switch ( aType )
        {
        case EOpenSess:
#ifdef _DEBUG
    PluginLogger::Log(
        _L("PluginConnection: callback HandleRequestCompleted stat=%d"), retVal);
    PluginLogger::Log(
        _L("PluginConnection: ------------------------------------------------1"));        
#endif          
            iConnectionHost->HandleRequestCompleted( iPrFwId, retVal );
            iPrFwId = TXIMPRequestId();             
            break;
        default: 
            break;   
        };
    }
        
// ---------------------------------------------------------------------------
// CSimplePluginConnection::CompleteReq
// ---------------------------------------------------------------------------
//
void CSimplePluginConnection::CompleteReq( TXIMPRequestId aReqId, TInt aStatus )
    {    
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginConnection: CompleteReq stat=%d"), aStatus);
#endif

    TInt retVal = HarmonizeErrorCode( aStatus );
                   
    // Return immediately if PrimeHost() is not called. This is for testing purposes.
    if ( !iConnectionHost )
        {
        return;
        }  
#ifdef _DEBUG
    PluginLogger::Log(
        _L("PluginConnection: callback HandleRequestCompleted stat=%d"), retVal);
    PluginLogger::Log(
        _L("PluginConnection: ------------------------------------------------2"));        
#endif              
    iConnectionHost->HandleRequestCompleted( aReqId, retVal );
    } 
    
// ---------------------------------------------------------------------------
// CSimplePluginConnection::CompleteWinfoReq
// ---------------------------------------------------------------------------
//
void CSimplePluginConnection::CompleteWinfoReq( TXIMPRequestId aReqId, TInt aStatus )
    {    
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginConnection: CompleteWinfoReq stat=%d"), aStatus);
#endif

    if ( iAuth && iAuth->HandleIfMine( aReqId, aStatus ))
        {
        // Nothing to do, the request in not complete yet
#ifdef _DEBUG
        PluginLogger::Log(_L("PluginConnection: CompleteWinfoReq waits ***"), aStatus);
#endif         
        }
    else
        {
#ifdef _DEBUG
        PluginLogger::Log(_L("PluginConnection: CompleteWinfoReq completes ***"), aStatus);
#endif        
        CompleteReq( aReqId, aStatus );
        }
    }                  
     
// ---------------------------------------------------------------------------
// CSimplePluginConnection::ObjectFactory
// ---------------------------------------------------------------------------
// 
MXIMPObjectFactory& CSimplePluginConnection::ObjectFactory()
    {
    return iConnectionHost->ObjectFactory();
    }
    
// ---------------------------------------------------------------------------
// CSimplePluginConnection::PresenceObjectFactory
// ---------------------------------------------------------------------------
// 
MPresenceObjectFactory& CSimplePluginConnection::PresenceObjectFactory()
    {
    return iConnectionHost->ProtocolPresenceDataHost().PresenceObjectFactory();    
    }     
  
// ---------------------------------------------------------------------------
// CSimplePluginConnection::Host
// ---------------------------------------------------------------------------
//     
MXIMPProtocolConnectionHost* CSimplePluginConnection::Host()
    {
    return iConnectionHost;
    }
    
// ---------------------------------------------------------------------------
// CSimplePluginConnection::CurrentDomain
// ---------------------------------------------------------------------------
//     
TPtrC16 CSimplePluginConnection::CurrentDomain()
    {
    return iSession->CurrentDomain();    
    } 
    
// ---------------------------------------------------------------------------
// CSimplePluginConnection::Variation
// ---------------------------------------------------------------------------
//     
CSimplePluginVariation& CSimplePluginConnection::Variation()
    {
    return *iVariation; 
    }        
           
// ---------------------------------------------------------------------------
// CSimplePluginConnection::CurrentSipPresentity
// ---------------------------------------------------------------------------
// 
TPtrC16 CSimplePluginConnection::CurrentSipPresentity()
    {       
    delete iSipPresentity;
    iSipPresentity = NULL;    
    TRAPD( err, iSipPresentity = HBufC16::NewL( iSession->CurrentSipPresentity().Length() ));
        
    if ( !err )
        {                
        iSipPresentity->Des().Copy( iSession->CurrentSipPresentity() );
        return iSipPresentity->Des();
        }
    else
        {
        return TPtrC16();
        }        
    }     
    
// ---------------------------------------------------------------------------
// CSimplePluginConnection::CurrentSipPresentit8
// ---------------------------------------------------------------------------
// 
TPtrC8 CSimplePluginConnection::CurrentSipPresentity8()
    {
    return iSession->CurrentSipPresentity();
    } 
            
// ---------------------------------------------------------------------------
// CSimplePluginConnection::XdmUtilsL
// ---------------------------------------------------------------------------
//
CSimplePluginXdmUtils* CSimplePluginConnection::XdmUtilsL()
    {
    if ( !iXdmUtils )
        {                
        iXdmUtils = CSimplePluginXdmUtils::NewL( 
            *this, iSession->XdmSettingsId() );
        }
    return iXdmUtils;
    }
            
// ---------------------------------------------------------------------------
// CSimplePluginConnection::WinfoHandlerL
// ---------------------------------------------------------------------------
//
CSimplePluginWinfo* CSimplePluginConnection::WinfoHandlerL()
    {
    if ( !iPluginWinfo )
        {
        iPluginWinfo = CSimplePluginWinfo::NewL( 
            *this, *(iSession->SimpleConnection() ));
        }
    iPluginWinfo->SetHost( iConnectionHost );
    return iPluginWinfo;
    }
    
// ---------------------------------------------------------------------------
// CSimplePluginConnection::WinfoTerminatedL
// ---------------------------------------------------------------------------
//
void CSimplePluginConnection::WinfoTerminatedL( TInt aReason )
    {
    // Pass the information to iAuth and iPublisher,
    // so that they can call PrFw Plugin Data Host callbacks.
    iAuth->WinfoTerminatedL( aReason );
    iPublisher->WinfoTerminatedL( aReason );
    }  
    
// ---------------------------------------------------------------------------
// CSimplePluginConnection::WinfoNotification
// ---------------------------------------------------------------------------
//
void CSimplePluginConnection::WinfoNotification( MSimpleWinfo& aWinfo )
    {
    TRAP_IGNORE( iPublisher->WinfoNotificationL( aWinfo ));    
    TRAP_IGNORE( iAuth->WinfoNotificationL( aWinfo ));
    }      
            
// ---------------------------------------------------------------------------
// CSimplePluginConnection::GetInterface
// ---------------------------------------------------------------------------
//
TAny* CSimplePluginConnection::GetInterface(
        TInt32 aInterfaceId,
        TIfGetOps aOptions )
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        MXIMPProtocolConnection* myIf = this;
        return myIf;
        }
    if( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( _L("CSimplePlugin"), KErrExtensionNotSupported );
        }  
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSimplePluginConnection::GetInterface
// ---------------------------------------------------------------------------
//
const TAny* CSimplePluginConnection::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions ) const
    {
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        const MXIMPProtocolConnection* myIf = this;
        return myIf;
        }
    if( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( _L("CSimplePlugin"), KErrExtensionNotSupported );
        }  
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSimplePluginConnection::GetInterfaceId
// ---------------------------------------------------------------------------
//
TInt32 CSimplePluginConnection::GetInterfaceId() const
    {
    return MXIMPProtocolConnection::KInterfaceId;
    }
    
// ---------------------------------------------------------------------------
// CSimplePluginConnection::HarmonizeErrorCode
// ---------------------------------------------------------------------------
//
TInt CSimplePluginConnection::HarmonizeErrorCode( TInt aStatus )
    {    

    TInt retVal = aStatus;

    // Convert error codes outsise e32err.h error range into PrFw errors.

    switch ( aStatus )
        {
        case KSimpleErrAuthorization:
            retVal = KPresenceErrNotEnoughCredits;
            break;
        case KSimpleErrTimeout:
        case KErrTimedOut:
            retVal = KXIMPErrServicRequestTimeouted;
            break;            
        default:
            {
            if ( aStatus < KSimplePluginGeneralErrorLow )
                {
                retVal = KXIMPErrServiceGeneralError;
                }
            }
            break;                                
        };
        
    return retVal;    
    }


// End of file

