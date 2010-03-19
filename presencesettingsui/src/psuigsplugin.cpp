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
* Description:    GSFW implementation for Presence Settings UI GS plugin.
*
*/




// INCLUDE FILES
#include <bautils.h>
#include <gsprivatepluginproviderids.h>
#include <GSFWViewUIDs.h>
#include <psuigspluginrsc.rsg>
#include <psuigsplugin.mbg>
#include <GSBaseView.h>
#include <eiktxlbx.h>
#include <eikmenup.h>
#include <eiktxlbm.h>
#include <featmgr.h>
#include <hlplch.h>
#include <coeaui.h>
#include <aknlists.h>
#include <aknPopup.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>

#include "psuigsplugin.h"
#include "psuigsplugin.hrh"
#include "psuigsplugincontainer.h"
#include "psuigspluginids.hrh"
#include "psuigspluginmodel.h"
#include "psuigspluginsettingview.h"

// CONSTANTS
_LIT( KPSUIGSPluginResourceFileName, "z:\\resource\\psuigspluginrsc.rsc" );

#ifdef __SCALABLE_ICONS
// bitmap
_LIT( KPSUIGSPluginIconFileName, "\\resource\\apps\\psuigsplugin.mif");
#else //__SCALABLE_ICONS
// svg file
_LIT( KPSUIGSPluginIconFileName, "\\resource\\apps\\psuigsplugin.mbm");
#endif //__SCALABLE_ICONS

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::CPSUIGSPlugin()
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CPSUIGSPlugin::CPSUIGSPlugin()
    : iClosing ( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::~CPSUIGSPlugin()
// Destructor
// ---------------------------------------------------------------------------
//
CPSUIGSPlugin::~CPSUIGSPlugin()
    {
    delete iPSModel;
    FeatureManager::UnInitializeLib();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CPSUIGSPlugin::ConstructL()
    {
    // preparing resources for use
    TFileName fileName( KPSUIGSPluginResourceFileName );
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    iResourceLoader.OpenL( fileName );
    // base construct (base needs the resources to be loaded 1st)
    BaseConstructL( R_PSUI_MAIN_VIEW );
    FeatureManager::InitializeLibL();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::NewL()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPSUIGSPlugin* CPSUIGSPlugin::NewL( TAny* /*aAppUi*/ )
    {
    CPSUIGSPlugin* self = new( ELeave ) CPSUIGSPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::Id()
// See header for details.
// ---------------------------------------------------------------------------
//
TUid CPSUIGSPlugin::Id() const
    {
    return TUid::Uid( PSUIGSPluginImplUid );
    }
   
// ---------------------------------------------------------------------------
// CPSUIGSPlugin::NewContainerL()
// See header for details.
// ---------------------------------------------------------------------------
//  
void CPSUIGSPlugin::NewContainerL()
    {
    iContainer = new ( ELeave ) CPSUIGSPluginContainer( this );
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::DoActivateL()
// See header for details.
// ---------------------------------------------------------------------------
//  
void CPSUIGSPlugin::DoActivateL( const TVwsViewId& aPrevViewId,
                                  TUid aCustomMessageId,
                                  const TDesC8& aCustomMessage )
    {
    if ( KPreSettingViewId != aPrevViewId.iViewUid )
        {        
        iPSUIGSPrevViewId = aPrevViewId;
        }
    
    // model & observer constructed only when needed
    if ( !iPSModel )
        {
        iPSModel = CPSUIGSPluginModel::NewL();
        iAppUi->AddViewDeactivationObserverL( this );        
        }

    const TInt id = iPSModel->SettingsId();
    if ( id )
        {// readjust the focus
        SetCurrentItem( iPSModel->SettingsIDArray().Find( id ));
        }

    iClosing = EFalse;
    CGSBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
    UpdateMSK();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::DoDeactivate()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUIGSPlugin::DoDeactivate()
    {
    CGSBaseView::DoDeactivate();
    if ( iClosing )
        {// PSUI plugin is going to be closed so it is safe to free some memory
        delete iPSModel;
        iPSModel = NULL;
        delete iSettingView;
        iSettingView = NULL;        
        }    
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::HandleViewDeactivation()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUIGSPlugin::HandleViewDeactivation(
    const TVwsViewId& aViewIdToBeDeactivated,
    const TVwsViewId& aNewlyActivatedViewId)
    {
    if ( TUid::Uid( PSUIGSPluginImplUid ) == aViewIdToBeDeactivated.iViewUid && 
        iPSUIGSPrevViewId.iViewUid == aNewlyActivatedViewId.iViewUid )
        {
        iClosing = ETrue;
        iAppUi->RemoveViewDeactivationObserver( this );
        }
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::HandleCommandL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUIGSPlugin::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            RemoveSettingView();
            iAppUi->ActivateLocalViewL( iPSUIGSPrevViewId.iViewUid );
            break;
            }
        case EPSUICmdEdit:
            {
            HandleListBoxSelectionL();
            break;
            }
        case EPSUICmdDelete:
            {
            DeleteSettingsL();
            break;
            }
        case EPSUICmdNewSettingsDefault:
            {
            CreateNewDefaultSettingsL();
            break;
            }
        case EPSUICmdNewSettingsExisting:
            {
            CreateCopyOfExistingSettingsL();
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
            iAppUi->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::SettingCountAndIndex()
// See header for details.
// ---------------------------------------------------------------------------
//
TInt CPSUIGSPlugin::SettingCountAndIndex( TInt& aIndex )
    {
    CEikTextListBox* listbox = Container()->iListBox;// not owned
    TInt count = listbox->Model()->NumberOfItems();
    aIndex = count ? listbox->CurrentItemIndex() : KErrNotFound;
        
    return count;        
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::HandleListBoxSelectionL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUIGSPlugin::HandleListBoxSelectionL()
    {
    TInt index = KErrNotFound;
    if ( SettingCountAndIndex( index ))
        {
        iPSModel->EditSettingsL( index );

        ConstructSettingViewL();    
        iAppUi->ActivateLocalViewL( KPreSettingViewId );
        }
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::DeleteSettingsL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUIGSPlugin::DeleteSettingsL()
    {
    TInt index = KErrNotFound;
    TInt count = SettingCountAndIndex( index );
    if ( count )
        {
        if ( AcceptDeletionL( count, index ))
            {
            iPSModel->DeleteSettingsL( index, ETrue );
            CEikTextListBox* listbox = Container()->iListBox;// not owned
            AknListBoxUtils::HandleItemRemovalAndPositionHighlightL(
                listbox, index, ETrue );
            listbox->DrawNow();
            UpdateMSK();    
            }
        }
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::AcceptDeletionL()
// See header for details.
// ---------------------------------------------------------------------------
//
TBool CPSUIGSPlugin::AcceptDeletionL( TInt aCount, TInt aIndex )
    {
    HBufC* currentName = iPSModel->SettingNameLC( aIndex );
    HBufC* string = NULL;
    if ( aCount == 1 )
        {
        string = StringLoader::LoadLC( R_QTN_PRESENCE_SETTINGS_DELETE_LAST, *currentName );
        }
    else
        {
        string = StringLoader::LoadLC( R_QTN_QUERY_COMMON_CONF_DELETE, *currentName );
        }

    CAknQueryDialog* query = CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );        
    TInt ok = query->ExecuteLD( R_PSUI_CONF_QUERY,  *string );
    
    CleanupStack::PopAndDestroy( 2, currentName );//currentName, string       
        
    return ( ok );    
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::GetCaptionL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUIGSPlugin::GetCaptionL( TDes& aCaption ) const
    {
    StringLoader::Load( aCaption, R_QTN_SET_FOLDER_PRESENCE );
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::PluginProviderCategory()
// See header for details.
// ---------------------------------------------------------------------------
//
TInt CPSUIGSPlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::CreateIconL()
// See header for details.
// ---------------------------------------------------------------------------
//
CGulIcon* CPSUIGSPlugin::CreateIconL( const TUid aIconType )
    {
    CGulIcon* icon = NULL;
    if( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL(
            AknsUtils::SkinInstance(), 
            KAknsIIDQgnPropSetConnPresence,
            KPSUIGSPluginIconFileName,
            EMbmPsuigspluginQgn_prop_set_conn_presence,
            EMbmPsuigspluginQgn_prop_set_conn_presence_mask );
        }
     else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }
    
    return icon;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::DynInitMenuPaneL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUIGSPlugin::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane *aMenuPane )
    {
    // remove some of the items if no settings available
    const TInt count = Container()->iListBox->Model()->NumberOfItems();
    if ( aResourceId == R_PSUI_MAINVIEW_MENUPANE && 
        0 == count )
        {
        aMenuPane->DeleteMenuItem( EPSUICmdEdit );
        aMenuPane->DeleteMenuItem( EPSUICmdDelete );
        }
    if ( aResourceId == R_PSUI_NEWSETTINGS_MENU && 
        0 == count )
        {
        aMenuPane->DeleteMenuItem( EPSUICmdNewSettingsExisting );
        }
    
    // help-item handling
    if ( aResourceId == R_PSUI_MAINVIEW_MENUPANE && 
        !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        aMenuPane->DeleteMenuItem( EAknCmdHelp );
        }
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::PSModel()
// See header for details.
// ---------------------------------------------------------------------------
//    
CPSUIGSPluginModel* CPSUIGSPlugin::PSModel()
    {
    __ASSERT_DEBUG( iPSModel,User::Panic( KPSUIGSPluginPanicCategory, KErrNotFound ));
    return iPSModel;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::ConstructSettingViewL()
// See header for details.
// ---------------------------------------------------------------------------
//    
void CPSUIGSPlugin::ConstructSettingViewL()
    {
    if ( !iSettingView )
        {// no view yet, construct it
        CAknView* view = CPSUIGSPluginSettingView::NewL( this );
        CleanupStack::PushL( view );
        iAppUi->AddViewL( view );
        CleanupStack::Pop( view );
        iSettingView = view;
        }
    }
    
// ---------------------------------------------------------------------------
// CPSUIGSPlugin::RemoveSettingView()
// See header for details.
// ---------------------------------------------------------------------------
//    
void CPSUIGSPlugin::RemoveSettingView()
    {
    // Remove view from iAppUi -> View is deleted by iAppUi automatically.
    if ( iSettingView )
        {
        iAppUi->RemoveView( KPreSettingViewId );
        iSettingView = NULL;
        }
    }
    
// ---------------------------------------------------------------------------
// CPSUIGSPlugin::CreateNewDefaultSettingsL()
// See header for details.
// ---------------------------------------------------------------------------
// 
void CPSUIGSPlugin::CreateNewDefaultSettingsL()
    {
    iPSModel->CreateDefaultSettingsL();
    ConstructSettingViewL();
    iAppUi->ActivateLocalViewL( KPreSettingViewId );
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::CreateCopyOfExistingSettingsL()
// See header for details.
// ---------------------------------------------------------------------------
// 
void CPSUIGSPlugin::CreateCopyOfExistingSettingsL()
    {
    CAknSinglePopupMenuStyleListBox* list = 
        new ( ELeave ) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( list );

    CAknPopupList* popupList = 
        CAknPopupList::NewL( list, R_AVKON_SOFTKEYS_OK_CANCEL );
    CleanupStack::PushL( popupList );    
    
    HBufC* title = StringLoader::LoadLC( R_QTN_PRESENCE_PROPMT_USE_EXIST );
    popupList->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );

    list->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL( 
        CEikScrollBarFrame::EOff, 
        CEikScrollBarFrame::EAuto );

    CTextListBoxModel* model = list->Model();
    model->SetItemTextArray( iPSModel->SettingNames() );// setting names not owned
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );
    list->HandleItemAdditionL();

    CleanupStack::Pop( popupList );
    TInt keyPress = popupList->ExecuteLD();
    
    if ( keyPress )
        {
        iPSModel->DuplicateExistingSettingsL( list->CurrentItemIndex() );
        ConstructSettingViewL();
        iAppUi->ActivateLocalViewL( KPreSettingViewId );        
        }
    
    CleanupStack::PopAndDestroy( list );
    }   

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::UpdateMSK()
// See header for details.
// ---------------------------------------------------------------------------
// 
void CPSUIGSPlugin::UpdateMSK()
    {
    TBool showEdit = ( 0 < Container()->iListBox->Model()->NumberOfItems());
    CEikCba* cba = static_cast< CEikCba* >( Cba()->ButtonGroup() );
    cba->MakeCommandVisible( EPSUICmdEdit, showEdit );
    cba->DrawNow();
    }

// End of file
