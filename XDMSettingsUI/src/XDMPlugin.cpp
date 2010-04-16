/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    XDMPlugin Implementation
*
*/




// Includes
#include "XDMExternalInterface.h"

#include <aknViewAppUi.h>
#include <StringLoader.h>
#include <eikmenub.h>
#include <aknlists.h>
#include <aknPopup.h>
#include <XdmSettingsApi.h>
#include <AknQueryDialog.h>
#include <featmgr.h>
#include <hlplch.h>
#include <csxhelp/xdm.hlp.hrh>
#include <gsxdmplugin.mbg>
#include <bautils.h>

#include "XDMPlugin.h"
#include "XDMPluginContainer.h"
#include "XDMPluginSLContainer.h"
#include "XDMPlugin.hrh"


// Constants

#ifdef __SCALABLE_ICONS
// bitmap
#else //__SCALABLE_ICONS
// svg file
_LIT( KGSXDMPluginIconFileName, "\\resource\\apps\\GSXDMplugin.mbm");
#endif //__SCALABLE_ICONS


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CXDMPlugin::CXDMPlugin()
// Constructor
//
// ---------------------------------------------------------------------------
//
CXDMPlugin::CXDMPlugin( )
    : iResources( *CCoeEnv::Static() )
    {
    
    }

// ---------------------------------------------------------------------------
// CXDMPlugin::~CXDMPlugin()
// Destructor
// ---------------------------------------------------------------------------
//
CXDMPlugin::~CXDMPlugin()
    {
    if (iCurrentContainer)
        {
        }

    if( iMainListContainer )
        {
        delete iMainListContainer;
        iMainListContainer=NULL;
        }
        
    if (iSettingListContainer)
        {
        delete iSettingListContainer;
        iSettingListContainer=NULL;
        }

    iResources.Close();
    /** Nice to know when the plugin is cleaned up */
    #ifdef _DEBUG
    RDebug::Print( _L( "[CXDMPlugin] ~CXDMPlugin()" ) );
    #endif
    }

// ---------------------------------------------------------------------------
// CXDMPlugin::ConstructL()
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
void CXDMPlugin::ConstructL()
    {
    // To know if the plugin is loaded (for debugging)
    #ifdef _DEBUG
    RDebug::Print(_L("[CXDMPlugin] ConstructL()" ));
    RDebug::Print( _L( "[CXDMPlugin] Loading resource from :" ) );
    #endif
    }

// ---------------------------------------------------------------------------
// CXDMPlugin::NewL()
// Static constructor
// ---------------------------------------------------------------------------
//
CXDMPlugin* CXDMPlugin::NewL( TAny* /*aInitParams*/ )
    {
    CXDMPlugin* self = new(ELeave) CXDMPlugin( );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CXDMPlugin::Id() const
// ---------------------------------------------------------------------------
//
TUid CXDMPlugin::Id() const
    {
    return KGSXDMPluginUid;
    }

// ---------------------------------------------------------------------------
// CXDMPlugin::HandleClientRectChange()
// ---------------------------------------------------------------------------
void CXDMPlugin::HandleViewRectChange()
    {
    if ( iMainListContainer->IsVisible() )
        {
        }
        
    if ( iSettingListContainer->IsVisible() )
        {
        }
    }

// ---------------------------------------------------------------------------
// CXDMPlugin::DoActivateL()
// ---------------------------------------------------------------------------
//
void CXDMPlugin::DoActivateL( const TVwsViewId& aPrevViewId,
                                  TUid /*aCustomMessageId*/,
                                  const TDesC8& /*aCustomMessage*/ )
    {
    iPrevViewId = aPrevViewId;
    if (iCurrentContainer)
        {
        iCurrentContainer = NULL;
        }
        

    if( iMainListContainer )
        {
        delete iMainListContainer;
        iMainListContainer=NULL;
        }

    if( iSettingListContainer )
        {
        delete iSettingListContainer;
        iSettingListContainer = NULL;
        }

    // switching control
    iSettingListContainer->MakeVisible(EFalse);    
    iCurrentContainer = iMainListContainer;
    UpdateMSK();
    }

// ---------------------------------------------------------------------------
// CXDMPlugin::DoDeactivate()
// ---------------------------------------------------------------------------
//
void CXDMPlugin::DoDeactivate()
    {
    // try to save settings if in settings list container
    TInt err;
    if (iCurrentContainer == iSettingListContainer)
        TRAP(err, iSettingListContainer->SaveSettingsIfPossibleL());
    iCurrentContainer = NULL;

    if( iMainListContainer )
        {
        delete iMainListContainer;
        iMainListContainer = NULL;
        }
        
    if( iSettingListContainer )
        {
        delete iSettingListContainer;
        iSettingListContainer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CXDMPlugin::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CXDMPlugin::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            if (iCurrentContainer == iSettingListContainer)
                {
                if (iSettingListContainer->IsExitProcessingOKL())
                    LoadMainViewL();    
                }
            else 
                iAppUi->ActivateLocalViewL( iPrevViewId.iViewUid );
            break;
        case EGSXDMPluginCmdChange:
            iSettingListContainer->EditCurrentItemL();                       
            break;
        case EGSXDMPluginCmdEdit:
            iMainListContainer->EditCurrentItemL();
            break;
        case EGSXDMPluginCmdNewExisting:
            if(iSettingListContainer->DisplayNewXDMSetOptionsL()) // if user ok
                LoadSettingsViewL(ENewFromExisting, iSettingListContainer->GetCurrentSetName());
            break;
        case EGSXDMPluginCmdNewDefault:
            LoadSettingsViewL(ENewDefault, iSettingListContainer->GetCurrentSetName());
            break;
        case EGSXDMPluginCmdDelete:
            iMainListContainer->DeleteSetProcedureL();
            UpdateMSK();
            break;
        case EAknCmdHelp:
            break;
        case EGSXDMPluginCmdExitFromSL:
            if (iCurrentContainer == iSettingListContainer)
                {
                if (iSettingListContainer->IsExitProcessingOKL())
                    iAppUi->HandleCommandL( EAknCmdExit ); 
                }
            break;
        case EAknCmdExit:
        case EEikCmdExit:
            if (iCurrentContainer == iSettingListContainer)
                iSettingListContainer->SaveSettingsIfPossibleL();
            iAppUi->HandleCommandL( EAknCmdExit );
            break;    
        default:
            iAppUi->HandleCommandL( aCommand );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CXDMPlugin::LoadSettingsViewL()
// ---------------------------------------------------------------------------
//
void CXDMPlugin::LoadSettingsViewL(TXDMSettingsViewType aType, TDesC& aXDMSetName)
    {
    switch(aType)
        {
        case EEditExisting:
            iSettingListContainer->PrepareXDMSetForEditingL(aXDMSetName);
            break;
        case ENewDefault:
            iSettingListContainer->PrepareXDMSetNewDefaultL();
            break;
        case ENewFromExisting:
            iSettingListContainer->PrepareNewXDMSetFromExistingL(aXDMSetName);
            break;
        default:
            break;
        }
    iCurrentContainer = iSettingListContainer;
    iMainListContainer->MakeVisible(EFalse);
    iSettingListContainer->MakeVisible(ETrue);
    UpdateMSK();
    }

// ---------------------------------------------------------------------------
// CXDMPlugin::LoadMainViewL()
// ---------------------------------------------------------------------------
//
void CXDMPlugin::LoadMainViewL()
    {
    TInt err(KErrNone);
    TRAP(err,iMainListContainer->LoadSettingsListArrayL()); // update main container
    iCurrentContainer = iMainListContainer;
    
    iMainListContainer->SetFocusIfExist(iSettingListContainer->GetCurrentSetName());
    
    iSettingListContainer->MakeVisible(EFalse);  
    iMainListContainer->MakeVisible(ETrue);
    UpdateMSK();
    }
    
// ---------------------------------------------------------------------------
// CXDMPlugin::DynInitMenuPaneL()
// ---------------------------------------------------------------------------
//
void CXDMPlugin::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
    {
    if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
    	{
    	aMenuPane->SetItemDimmed(EAknCmdHelp, ETrue);
    	}
    
    if (aResourceId == R_GS_XDM_SETTINGS_MAIN_MENU)
        {
        if (iCurrentContainer == iMainListContainer)
            {
            aMenuPane->SetItemDimmed(EGSXDMPluginCmdChange, ETrue);
            aMenuPane->SetItemDimmed(EGSXDMPluginCmdExitFromSL, ETrue);
            if (iMainListContainer->IsListEmpty())
                {
                aMenuPane->SetItemDimmed(EGSXDMPluginCmdEdit, ETrue);
                aMenuPane->SetItemDimmed(EGSXDMPluginCmdDelete, ETrue);
                }
            }
        else
            {
            aMenuPane->SetItemDimmed(EGSXDMPluginCmdEdit, ETrue);
            aMenuPane->SetItemDimmed(EGSXDMPluginCmdDelete, ETrue);
            aMenuPane->SetItemDimmed(EGSXDMPluginCmdNewSettings, ETrue);
            aMenuPane->SetItemDimmed(EAknCmdExit, ETrue);
            }
        }
    
    if (aResourceId == R_GS_XDM_NEW_SETTINGS_MENU)
        {
        if (iMainListContainer->IsListEmpty())
            {
            aMenuPane->SetItemDimmed(EGSXDMPluginCmdNewExisting, ETrue);
            }
        }
    }

// ---------------------------------------------------------
// CXDMPlugin::HandleResourceChangeManual()
// ---------------------------------------------------------
//
void CXDMPlugin::HandleResourceChangeManual(TInt aType)
    {
    if ( iSettingListContainer )
        iSettingListContainer->HandleResourceChangeManual(aType);    
    if ( iMainListContainer )
        iMainListContainer->HandleResourceChangeManual(aType);
    }
    
    
// ---------------------------------------------------------------------------
// CXDMPlugin::UpdateMSK()
// ---------------------------------------------------------------------------
//
void CXDMPlugin::UpdateMSK()
    {
    }
    
// End of file


