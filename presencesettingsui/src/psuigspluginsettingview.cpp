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
* Description:    Presence Settings UI setting view implementation.
*
*/




// INCLUDE FILES
#include <bautils.h>
#include <psuigspluginrsc.rsg>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <aknViewAppUi.h>
#include <featmgr.h>
#include <hlplch.h>
#include "psuigsplugin.h"
#include "psuigspluginsettingview.h"
#include "psuigspluginmodel.h"
#include "psuigspluginsettingviewcontainer.h"

// CONSTANTS

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::CPSUIGSPluginSettingView()
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CPSUIGSPluginSettingView::CPSUIGSPluginSettingView( CPSUIGSPlugin* aMainView )
    : iMainView ( aMainView )
    {
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::~CPSUIGSPluginSettingView()
// Destructor
// ---------------------------------------------------------------------------
//
CPSUIGSPluginSettingView::~CPSUIGSPluginSettingView()
    {
    // iAppUi, iMainView deleted elsewhere
    if ( iPSUIContainer && iAppUi )
        {
        iAppUi->RemoveFromViewStack( *this, iPSUIContainer );
        delete iPSUIContainer;
        iPSUIContainer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginSettingView::ConstructL()
    {
    // base construct
    iAppUi = AppUi();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::NewL()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPSUIGSPluginSettingView* CPSUIGSPluginSettingView::NewL( CPSUIGSPlugin* aMainView )
    {
    CPSUIGSPluginSettingView* self = new( ELeave ) CPSUIGSPluginSettingView( aMainView );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::Id()
// See header for details.
// ---------------------------------------------------------------------------
//
TUid CPSUIGSPluginSettingView::Id() const
    {
    return KPreSettingViewId;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::HandleViewRectChange()
// See header for details.
// ---------------------------------------------------------------------------
//  
void CPSUIGSPluginSettingView::HandleViewRectChange()
    {
    if ( iPSUIContainer && iPSUIContainer->ListBox() )
        {
        iPSUIContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::DoActivateL()
// See header for details.
// ---------------------------------------------------------------------------
//  
void CPSUIGSPluginSettingView::DoActivateL( const TVwsViewId& aPrevViewId,
                                  TUid /*aCustomMessageId*/,
                                  const TDesC8& /*aCustomMessage*/ )
    {
    iPrevViewId = aPrevViewId;

    CreateContainerL();
    iAppUi->AddToViewStackL( *this, iPSUIContainer );
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::DoDeactivate()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginSettingView::DoDeactivate()
    {
    if ( iPSUIContainer )
        {
        iAppUi->RemoveFromViewStack( *this, iPSUIContainer );
        delete iPSUIContainer;
        iPSUIContainer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::HandleCommandL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginSettingView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            HandleBackKeyL();
            break;
            }
        case EPSUICmdChange:
            {
            iPSUIContainer->EditCurrentItemL();
            break;
            }
        case EAknCmdHelp:
            {
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), iAppUi->AppHelpContextL() );
                }
            break;
            }              
        default:
            {
            if ( EEikCmdExit == aCommand )
                {
                HandleMenuExitL();
                }            
            
            iAppUi->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::CreateContainerL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginSettingView::CreateContainerL()
    {
    // delete container if present
    if( iPSUIContainer )
        {
        iAppUi->RemoveFromViewStack( *this, iPSUIContainer );
        delete iPSUIContainer;
        iPSUIContainer = NULL;
        }
    
    // check the data    
    iMainView->PSModel()->ClearInvalidSIPAndXDMSettingsL();  
    
    // create container
    iPSUIContainer = new ( ELeave ) CPSUIGSPluginSettingViewContainer( this );
    iPSUIContainer->SetMopParent( this );
    iPSUIContainer->ConstructL( ClientRect() );
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::HandleBackKeyL()
// See header for details.
// ---------------------------------------------------------------------------
//    
void CPSUIGSPluginSettingView::HandleBackKeyL()
    {
    CPSUIGSPluginModel* psModel = iMainView->PSModel();// not owned
    
    TBool okToGoBack = ETrue;
    TBool okToSave = ETrue;
    psModel->ClearInvalidSIPAndXDMSettingsL();
    const TInt id = psModel->SettingsId();
    
    // compulsory-field handling
    if ( !psModel->AllCompulsoryFieldsFilled())
        {// empty compulsory fields found
        okToSave = EFalse;
        }
        
    // same name checking        
    if ( okToSave && HandleDuplicatePSNamesL() )
        {// same name found
        okToGoBack = EFalse;
        okToSave = EFalse;
        iPSUIContainer->EditItemL( EPSUIServerNameId, ETrue );
        }
     
    if ( okToSave )
        {// save settings
        psModel->SaveSettingsL();
        }
    else if ( !okToSave && okToGoBack && id )
        {// old settings, delete and go back
        psModel->DeleteSettingsL( id, EFalse );
        }
    if ( okToGoBack )
        {// go back, no save or delete
        iAppUi->ActivateLocalViewL( iPrevViewId.iViewUid );
        }
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::HandleMenuExitL()
// See header for details.
// ---------------------------------------------------------------------------
//  
void CPSUIGSPluginSettingView::HandleMenuExitL()
    {
    CPSUIGSPluginModel* psModel = iMainView->PSModel();// not owned
    if ( !psModel )
        {// no model, can't do anything
        // (leaving would stop exiting, so just return)
        return;
        }
    
    if ( psModel->AllCompulsoryFieldsFilled() )
        {// needed fields filled, proceed saving
        if ( psModel->IsItDuplicatePSName() )
            {// a dupe name -> change name
            HBufC* newName = psModel->CreateSettingNameLC( 
                psModel->SettingsSet().iSetName.AllocL() );
            psModel->SettingsSet().iSetName.Copy( newName->Des() );
            CleanupStack::PopAndDestroy( newName );            
            }
        // save settings
        psModel->SaveSettingsL();            
        }
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::HandleDuplicatePSNamesL()
// See header for details.
// ---------------------------------------------------------------------------
// 
TBool CPSUIGSPluginSettingView::HandleDuplicatePSNamesL()
    {
    TBool found = iMainView->PSModel()->IsItDuplicatePSName();
    return found;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingView::MainView()
// See header for details.
// ---------------------------------------------------------------------------
//    
CPSUIGSPlugin* CPSUIGSPluginSettingView::MainView()
    {
    return iMainView;
    }

// End of file
