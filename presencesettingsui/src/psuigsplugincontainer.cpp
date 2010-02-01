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
#include <gsbasecontainer.h>
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
void CPSUIGSPluginContainer::ConstructL( const TRect& aRect )
    {
    iListBox = new ( ELeave ) CAknSingleStyleListBox();// base needs the iListBox to be prepared 1st
    BaseConstructL( aRect, R_PSUI_MAINVIEW_TITLE, 0 );// last parameter 0, since dynamic listbox
    
    iEikMenuBar = new ( ELeave ) CEikMenuBar();
    iEikMenuBar->ConstructL( this, NULL, R_PSUI_MAINVIEW_MENUBAR );
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginContainer::~CPSUIGSPluginContainer()
// Destructor
// ---------------------------------------------------------------------------
//
CPSUIGSPluginContainer::~CPSUIGSPluginContainer()
    {
    delete iEikMenuBar;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginContainer::ConstructListBoxL()
// See header for details. 
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginContainer::ConstructListBoxL( TInt /*aResLbxId*/ )
    {
    // listbox creation and initialization
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this, EAknListBoxLoopScrolling );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    SetEmptyTextsToListboxL();
    
    CPSUIGSPluginModel* psuiModel = iView->PSModel();// not owned
    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray( psuiModel );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginContainer::SetEmptyTextsToListboxL()
// See header for details. 
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginContainer::SetEmptyTextsToListboxL()
    {
    // empty texts
    _LIT ( KPSUIEmptyStringHeader, "%S\n%S" );
    HBufC* emptyText = iEikonEnv->AllocReadResourceLC( 
        R_QTN_PRESENCE_SETTINGS_EMPTY_PRIMARY );
    HBufC* emptyText2 = iEikonEnv->AllocReadResourceLC( 
        R_QTN_PRESENCE_SETTINGS_EMPTY_SECONDARY );   
    HBufC* emptyFinal = HBufC::NewLC( 
        emptyText->Length() + emptyText2->Length() + 2 );// 2 is length of "\n" 
    emptyFinal->Des().Format( KPSUIEmptyStringHeader, emptyText, emptyText2 );
    iListBox->View()->SetListEmptyTextL( *emptyFinal );
    CleanupStack::PopAndDestroy( 3, emptyText );// emptyText, emptyText2, emptyFinal
    }
    
// ---------------------------------------------------------------------------
// CPSUIGSPluginContainer::OfferKeyEventL()
// See header for details. 
// ---------------------------------------------------------------------------
//
TKeyResponse CPSUIGSPluginContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    if ( iView && aType == EEventKey 
        && aKeyEvent.iCode == EKeyBackspace && 
        iEikMenuBar->ItemSpecificCommandsEnabled() )
        {
        iView->DeleteSettingsL();
        return EKeyWasConsumed;
        }
    else
        {
        return iListBox->OfferKeyEventL( aKeyEvent, aType );
        }
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
