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


#include "simpleimfeatures.h"
#include "simpleimconversation.h"
#include "mpresencepluginconnectionobs.h"
#include "simpleimplugindefs.h"
#include "simpleimlogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSimpleImFeatures::CSimpleImFeatures()
// ---------------------------------------------------------------------------
//
CSimpleImFeatures::CSimpleImFeatures() : CProtocolImFeatures()
    {
    }


// ---------------------------------------------------------------------------
// CSimpleImFeatures::NewL()
// ---------------------------------------------------------------------------
//
CSimpleImFeatures* CSimpleImFeatures::NewL( TAny* aInitParams )
    {
    DP_IMP( "CSimpleImFeatures::NewL()" )
    
    CSimpleImFeatures* self = new( ELeave ) CSimpleImFeatures();
    CleanupStack::PushL( self );
    self->ConstructL( aInitParams );
    CleanupStack::Pop( self );
    
    return self;
    }


// ---------------------------------------------------------------------------
// CSimpleImFeatures::ConstructL
// ---------------------------------------------------------------------------
//
void CSimpleImFeatures::ConstructL( TAny* aInitParams )
    {    
    DP_IMP( "CSimpleImFeatures::ConstructL()" )
    __ASSERT_ALWAYS( aInitParams, User::Leave( KErrArgument ) );
    
    TSimpleImPluginInitParams* initParams =
        static_cast<TSimpleImPluginInitParams*>( aInitParams );
    
    iImConversation = CSimpleImConversation::NewL( initParams->iObserver,
        initParams->iConnection, initParams->iServiceId );
    }


// ---------------------------------------------------------------------------
// CSimpleImFeatures::~CSimpleImFeatures
// ---------------------------------------------------------------------------
//
CSimpleImFeatures::~CSimpleImFeatures()
    {
    DP_IMP( "CSimpleImFeatures::~CSimpleImFeatures()" )
    
    delete iImConversation;
    }


// ---------------------------------------------------------------------------
// From MXIMPBase.
// CSimpleImFeatures::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CSimpleImFeatures::GetInterface( TInt32 aInterfaceId,
        TIfGetOps aOptions )
    {
    DP_IMP( "CSimpleImFeatures::GetInterface()" )
    
    if ( CProtocolImFeatures::KInterfaceId == aInterfaceId )
        {
        DP_IMP( "CSimpleImFeatures::GetInterface() - Found" )
        CProtocolImFeatures* self = this;
        return self;
        }
    
    if ( MXIMPBase::EPanicIfUnknown == aOptions )
        {
        User::Panic( KSimpleImPluginName, KErrExtensionNotSupported );
        }
    
    return NULL;
    }


// ---------------------------------------------------------------------------
// From MXIMPBase.
// CSimpleImFeatures::GetInterface() const
// ---------------------------------------------------------------------------
//
const TAny* CSimpleImFeatures::GetInterface( TInt32 aInterfaceId,
        TIfGetOps aOptions ) const
    {
	DP_IMP( "CSimpleImFeatures::GetInterface() const" )
	
    if ( CProtocolImFeatures::KInterfaceId == aInterfaceId )
        {
        DP_IMP( "CSimpleImFeatures::GetInterface() const - Found" )
        const CProtocolImFeatures* self = this;
        return self;
        }

    if ( MXIMPBase::EPanicIfUnknown == aOptions )
        {
        User::Panic( KSimpleImPluginName, KErrExtensionNotSupported );
        }
    
    return NULL;
    }


// ---------------------------------------------------------------------------
// From MXIMPBase.
// CSimpleImFeatures::GetInterfaceId() const
// ---------------------------------------------------------------------------
//
TInt32 CSimpleImFeatures::GetInterfaceId() const
    {
    return CProtocolImFeatures::KInterfaceId;
    }


// ---------------------------------------------------------------------------
// From CProtocolImFeatures.
// CSimpleImFeatures::ImGroup()
// ---------------------------------------------------------------------------
//
MProtocolImGroup& CSimpleImFeatures::ImGroup()
    {
    DP_IMP( "CSimpleImFeatures::ImGroup(), NOT SUPPORTED" )
    
    MProtocolImGroup* dummyGroup = NULL;
    return *dummyGroup;
    }


// ---------------------------------------------------------------------------
// From CProtocolImFeatures.
// CSimpleImFeatures::ImConversation()
// ---------------------------------------------------------------------------
//
MProtocolImConversation& CSimpleImFeatures::ImConversation()
    {
    DP_IMP( "CSimpleImFeatures::ImConversation()" )
    
    return *iImConversation;
    }


// ---------------------------------------------------------------------------
// From CProtocolImFeatures.
// CSimpleImFeatures::ImInvitation()
// ---------------------------------------------------------------------------
//
MProtocolImInvitation& CSimpleImFeatures::ImInvitation()
    {
    DP_IMP( "CSimpleImFeatures::ImInvitation(), NOT SUPPORTED" )
    
    MProtocolImInvitation* dummyInvitation = NULL;
    return *dummyInvitation;
    }


// ---------------------------------------------------------------------------
// From CProtocolImFeatures.
// CSimpleImFeatures::ImSearch()
// ---------------------------------------------------------------------------
//
MProtocolImSearch& CSimpleImFeatures::ImSearch()
    {
    DP_IMP( "CSimpleImFeatures::ImSearch(), NOT SUPPORTED" )
    
    MProtocolImSearch* dummySearch = NULL;
    return *dummySearch;
    }


// ---------------------------------------------------------------------------
// From CProtocolImFeatures.
// CSimpleImFeatures::SetHost()
// ---------------------------------------------------------------------------
//
void CSimpleImFeatures::SetHost( MProtocolImDataHost& aHost )
    {
    DP_IMP( "CSimpleImFeatures::SetHost()" )
    
    iHost = &aHost ;
    iImConversation->SetHost( *iHost );
    }

// ---------------------------------------------------------------------------
// From CProtocolImFeatures.
// CSimpleImFeatures::SupportedSubInterfaces()
// ---------------------------------------------------------------------------
//
TInt CSimpleImFeatures::SupportedSubInterfaces() const
    {
    return EProtocolImConversation;
    }

// End of file
