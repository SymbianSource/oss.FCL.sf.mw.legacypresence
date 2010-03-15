/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <MVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include "presenceplugincontactstateresolve.h"
#include "presencecontactscontextbase.h"
#include "presenceplugincontactstatehandler.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginContactStateResolve::PresencePluginContactStateResolve
// ---------------------------------------------------------------------------
//
CPresencePluginContactStateResolve::CPresencePluginContactStateResolve(
    MPresenceContactsContextBase& aContext,
    CPresencePluginContactStateHandler& aStateHandler )
    : CPresencepluginContactState( aContext, aStateHandler ),
      iContactLinkIndex( NULL ),
    iContactLinkCount( NULL )
    {
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateResolve::~CPresencePluginContactStateResolve
// ---------------------------------------------------------------------------
//
CPresencePluginContactStateResolve::~CPresencePluginContactStateResolve()
    {
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateResolve::HandleL
// ---------------------------------------------------------------------------
//
void CPresencePluginContactStateResolve::HandleL()
    {
    iContactLinkCount = iContext->ContactLinkArrayL().Count();
    if ( NULL == iContactLinkCount )
        {
        Error( KErrNotFound );
        }
    else
        {
        iContext->RetrieveContactL(
            iContext->ContactLinkArrayL().At( iContactLinkIndex ) );
        iContactLinkIndex++;
        }
    }


// ---------------------------------------------------------------------------
// CPresencePluginContactStateResolve::Complete
// ---------------------------------------------------------------------------
//
void CPresencePluginContactStateResolve::Complete()
    {
    if ( iContactLinkIndex < iContactLinkCount )
        {
        TRAPD( error, HandleL() );
        if ( KErrNone != error )
            {
            Error( error );
            }
        }
    else
        {
        iStateHandler->ProgressToNextState();
        }
    }
