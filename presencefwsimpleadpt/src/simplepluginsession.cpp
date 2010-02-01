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

#include <presenceinfo.h>

#include <ximpserviceinfo.h>
#include <ximpbase.h>
#include <ximpidentity.h>
#include <ximpprotocolconnectionhost.h>

#include <simplefactory.h>
#include <msimpleconnection.h>
#include <msimpleconnectionobserver.h>
#include <pressettingsapi.h>

#include "simplepluginsession.h"
#include "simpleplugindebugutils.h"
#include "msimplepluginconnectionobs.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSimplePluginSession::CSimplePluginSession()
// ---------------------------------------------------------------------------
//
CSimplePluginSession::CSimplePluginSession( MSimplePluginConnectionObs& aObs)
: // CActive( CActive::EPriorityStandard ) ,
  iObs(aObs), iType( MSimplePluginConnectionObs::ENoReq ), iConnected( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CSimplePluginSession::ConstructL()
// ---------------------------------------------------------------------------
//
void CSimplePluginSession::ConstructL(
    const MXIMPServiceInfo& aService )
    {
    // iIap is needed only
    iIap = aService.IapId();

    iConnection = TSimpleFactory::NewConnectionL( *this );
    }

// ---------------------------------------------------------------------------
// CSimplePluginSession::NewL()
// ---------------------------------------------------------------------------
//
CSimplePluginSession* CSimplePluginSession::NewL(
    const MXIMPServiceInfo& aServiceInfo,
    MSimplePluginConnectionObs& aObs )
    {
    CSimplePluginSession* self =  new( ELeave ) CSimplePluginSession( aObs );
    CleanupStack::PushL( self );
    self->ConstructL( aServiceInfo );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSimplePluginSession::~CSimplePluginSession()
// ---------------------------------------------------------------------------
//
CSimplePluginSession::~CSimplePluginSession()
    {
    delete iUserId8;
    delete iDomain;
    if ( iConnection )
        {
        iConnection->Close();        
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginSession::SimpleConnection()
// ---------------------------------------------------------------------------
//
MSimpleConnection* CSimplePluginSession::SimpleConnection( )
    {
    return iConnection;
    }

// ---------------------------------------------------------------------------
// CSimplePluginSession::XdmSettingsId()
// ---------------------------------------------------------------------------
//
TInt CSimplePluginSession::XdmSettingsId( )
    {
    return iXdmSetting;
    }
    
// ---------------------------------------------------------------------------
// CSimplePluginSession::CurrentDomain()
// ---------------------------------------------------------------------------
//
TPtrC16 CSimplePluginSession::CurrentDomain( )
    {
    return iDomain ? iDomain->Des() : TPtrC16();
    }    

// ---------------------------------------------------------------------------
// CSimplePluginSession::OpenSessionL()
// Notice: MXIMPServiceInfo::IapId supported only
// ---------------------------------------------------------------------------
//
void CSimplePluginSession::OpenSessionL()
    {
    TPresSettingsSet mySet;

#ifdef _DEBUG

    PluginLogger::Log(_L("PluginSession: OpenSessionL") );
        
    RArray<TInt> setIds;
    CDesCArray* carr = PresSettingsApi::GetAllSetsNamesLC( setIds );
    TInt myCount = setIds.Count();
    for (TInt i=0; i<myCount; i++)
        {

        TBuf<500> buffer2; buffer2.Copy(carr->MdcaPoint(i));
        PluginLogger::Log(_L("PluginSession: ID:%d = %S"), setIds[i], &buffer2);
        }
    setIds.Reset();
    CleanupStack::PopAndDestroy( carr );
#endif

    iConnected = EFalse;

    User::LeaveIfError( PresSettingsApi::SettingsSetL( iIap, mySet ));
    iXdmSetting = mySet.iXDMSetting;
    
    iDomain = mySet.iDomainSyntax.AllocL();   

    // SIP register
    iOpId = iConnection->LoginL( iIap );
    iType = MSimplePluginConnectionObs::EOpenSess;       
    }
    
// ---------------------------------------------------------------------------
// CSimplePluginSession::OpenSessionL()
// Notice: Multiple connections support to be done.
// ---------------------------------------------------------------------------
//
void CSimplePluginSession::OpenSessionL( TInt aSettingsId )
    {
    iIap = aSettingsId;
    OpenSessionL();
    }

// ---------------------------------------------------------------------------
// CSimplePluginSession::ConnectionStatusL()
// ---------------------------------------------------------------------------
//
void CSimplePluginSession::ConnectionStatusL( MSimpleConnection::TSimpleState aState )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginSession: ConnectionStatusL %d"), aState );
#endif
        
    if ( (aState == MSimpleConnection::EInactive || aState == MSimpleConnection::EUnavailable ) &&
         iConnected )
        {
#ifdef _DEBUG
        PluginLogger::Log(
            _L("PluginSession: ConnectionStatusL callback HandleConnectionTerminated") );
#endif  
        iConnected = EFalse;                    
        iObs.Host()->HandleConnectionTerminated( NULL );         
        }        
    }

// ---------------------------------------------------------------------------
// CSimplePluginSession::RequestCompleteL()
// ---------------------------------------------------------------------------
//
void CSimplePluginSession::RequestCompleteL( TInt /*aOpId*/, TInt aStatus )
    {
    MSimplePluginConnectionObs::TReqType current = iType;
    iType = MSimplePluginConnectionObs::ENoReq;
    if ( !aStatus )
        {
        iConnected = ETrue;
        }
    iObs.CompleteReq( current, aStatus );
    }

// ---------------------------------------------------------------------------
// CSimplePluginSession::CurrentSipPresentity()
// ---------------------------------------------------------------------------
//
TPtrC8 CSimplePluginSession::CurrentSipPresentity()
    {        
    // Get from Simple engine
    delete iUserId8;
    iUserId8 = NULL;
    TRAP_IGNORE( iUserId8 = iConnection->CurrentSIPIdentityL().AllocL() );
    return iUserId8 ? iUserId8->Des() : TPtrC8();
    }



// End of file

