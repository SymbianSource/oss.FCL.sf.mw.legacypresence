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
#include <gsprivatepluginproviderids.h>
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
_LIT( KGSXDMPluginResourceFileName, "z:\\resource\\XDMPluginRsc.rsc" );

#ifdef __SCALABLE_ICONS
// bitmap
_LIT( KGSXDMPluginIconFileName, "\\resource\\apps\\GSXDMplugin.mif");
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
    : iAppUi( CAknView::AppUi() ), iResources( *CCoeEnv::Static() )
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
        if(iAppUi)
        iAppUi->RemoveFromViewStack( *this, iCurrentContainer );
        iCurrentContainer = NULL;
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
    RDebug::Print( KGSXDMPluginResourceFileName );
    #endif
    
    TFileName fileName( KGSXDMPluginResourceFileName );
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    iResources.OpenL( fileName );
    BaseConstructL( R_GS_XDM_MAIN_VIEW );
        
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
        iMainListContainer->SetRect( ClientRect() );
        }
        
    if ( iSettingListContainer->IsVisible() )
        {
        iSettingListContainer->SetRect( ClientRect() );
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
        iAppUi->RemoveFromViewStack( *this, iCurrentContainer );
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

    iMainListContainer = new( ELeave ) CXDMPluginContainer(this);
    iMainListContainer->SetMopParent(this);
    TRAPD( error, iMainListContainer->ConstructL( ClientRect() ) );
    if (error)
        {
        delete iMainListContainer;
        iMainListContainer = NULL;
        User::Leave( error );
        }
        
    iSettingListContainer = new( ELeave ) CXDMPluginSLContainer(this);
    iSettingListContainer->SetMopParent(this);
    TRAPD( error1, iSettingListContainer->ConstructL( ClientRect()));
    if (error1)
        {
        delete iSettingListContainer;
        iSettingListContainer = NULL;
        User::Leave( error1 );
        }

    // switching control
    iSettingListContainer->MakeVisible(EFalse);    
    iCurrentContainer = iMainListContainer;
    iAppUi->AddToViewStackL( *this, iCurrentContainer );
    UpdateMskL();
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
    if (iCurrentContainer)
        iAppUi->RemoveFromViewStack( *this, iCurrentContainer );
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
            UpdateMskL();
            break;
        case EAknCmdHelp:
            HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), AppUi()->AppHelpContextL ());
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
// CXDMPlugin::GetCaptionL()
// ---------------------------------------------------------------------------
//
void CXDMPlugin::GetCaptionL( TDes& aCaption ) const
    {
    StringLoader::Load( aCaption, R_STR_XDM_SETTINGS );
    }

// ---------------------------------------------------------------------------
// CXDMPlugin::PluginProviderCategory()
// ---------------------------------------------------------------------------
//
TInt CXDMPlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
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
    if (iCurrentContainer)
        iAppUi->RemoveFromViewStack( *this, iCurrentContainer );
    iCurrentContainer = iSettingListContainer;
    iAppUi->AddToViewStackL( *this, iCurrentContainer );
    iMainListContainer->MakeVisible(EFalse);
    iSettingListContainer->MakeVisible(ETrue);
    UpdateMskL();
    }

// ---------------------------------------------------------------------------
// CXDMPlugin::LoadMainViewL()
// ---------------------------------------------------------------------------
//
void CXDMPlugin::LoadMainViewL()
    {
    TInt err(KErrNone);
    TRAP(err,iMainListContainer->LoadSettingsListArrayL()); // update main container
    if (iCurrentContainer)
        iAppUi->RemoveFromViewStack( *this, iCurrentContainer );
    iCurrentContainer = iMainListContainer;
    iAppUi->AddToViewStackL( *this, iCurrentContainer );
    
    iMainListContainer->SetFocusIfExist(iSettingListContainer->GetCurrentSetName());
    
    iSettingListContainer->MakeVisible(EFalse);  
    iMainListContainer->MakeVisible(ETrue);
    UpdateMskL();
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
// CXDMPlugin::CreateIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CXDMPlugin::CreateIconL( const TUid aIconType )
    {
    CGulIcon* icon;
       
    if( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnPropSetConnXdm, 
        KGSXDMPluginIconFileName,
        EMbmGsxdmpluginQgn_prop_set_conn_xdm,
        EMbmGsxdmpluginQgn_prop_set_conn_xdm_mask);
        }
     else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }

    return icon;
    }
    
// ---------------------------------------------------------------------------
// CXDMPlugin::UpdateMskL()
// ---------------------------------------------------------------------------
//
void CXDMPlugin::UpdateMskL()
    {
    if ( !Cba() )
        {
        return;
        }
    
    CEikCba* cba = static_cast< CEikCba* >( Cba()->ButtonGroup() );
    if(iCurrentContainer==iMainListContainer)
        {
        TBool showEdit = !(iMainListContainer->IsListEmpty());
        cba->SetCommandSetL(R_XDMUI_MAINVIEW_SOFTKEYS);
        cba->MakeCommandVisible( EGSXDMPluginCmdEdit, showEdit );
        }
    else if(iCurrentContainer==iSettingListContainer)
        {
        cba->SetCommandSetL(R_XDMUI_MAINVIEW_SOFTKEYS_SL);
        cba->MakeCommandVisible( EGSXDMPluginCmdChange, ETrue);
        if ( iSettingListContainer->IsVisible() )
            {
            iCurrentContainer->DrawNow();
            }
        }
    cba->DrawNow();
    }
    
// End of file


