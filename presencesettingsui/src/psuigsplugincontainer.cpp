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
* Description:    Presence Settings UI GS plugin container implementation
*
*/




// INCLUDE FILES
#include <psuigspluginrsc.rsg>
#include <aknlists.h>
#include "psuigsplugincontainer.h"
#include "psuigspluginmodel.h"
#include "psuigsplugin.h"
#include "psuigspluginids.hrh"
#include <csxhelp/pre.hlp.hrh>

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CPSUIGSPluginContainer::CPSUIGSPluginContainer()
// Default constructor.
// ---------------------------------------------------------------------------
//
CPSUIGSPluginContainer::CPSUIGSPluginContainer( CPSUIGSPlugin* aView )
    : iView ( aView )
    {
    }
    
// ---------------------------------------------------------------------------
// CPSUIGSPluginContainer::ConstructL(const TRect& aRect)
// Symbian OS phase 2 constructor
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginContainer::ConstructL( const TRect& /*aRect*/ )
    {
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginContainer::~CPSUIGSPluginContainer()
// Destructor
// ---------------------------------------------------------------------------
//
CPSUIGSPluginContainer::~CPSUIGSPluginContainer()
    {
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginContainer::SetEmptyTextsToListboxL()
// See header for details. 
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginContainer::SetEmptyTextsToListboxL()
    {
    }

// -----------------------------------------------------------------------------
// CPSUIGSPluginContainer::GetHelpContext()
// See header for details.
// -----------------------------------------------------------------------------
//
void CPSUIGSPluginContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = TUid::Uid( PSUIGSPluginDllUid );
    aContext.iContext = KPRE_HLP_LIST_VIEW;
    }
    
// End of File
