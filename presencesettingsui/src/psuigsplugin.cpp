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
#include <psuigspluginrsc.rsg>
#include <psuigsplugin.mbg>
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

#ifdef __SCALABLE_ICONS
// bitmap
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
// CPSUIGSPlugin::DoActivateL()
// See header for details.
// ---------------------------------------------------------------------------
//  
void CPSUIGSPlugin::DoActivateL( const TVwsViewId& aPrevViewId,
                                  TUid /*aCustomMessageId*/,
                                  const TDesC8& /*aCustomMessage*/ )
    {
    if ( KPreSettingViewId != aPrevViewId.iViewUid )
        {        
        iPSUIGSPrevViewId = aPrevViewId;
        }
    
    // model & observer constructed only when needed
    if ( !iPSModel )
        {
        iPSModel = CPSUIGSPluginModel::NewL();   
        }

    const TInt id = iPSModel->SettingsId();
    if ( id )
        {// readjust the focus
        }

    iClosing = EFalse;
    UpdateMSK();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::DoDeactivate()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUIGSPlugin::DoDeactivate()
    {
    if ( iClosing )
        {// PSUI plugin is going to be closed so it is safe to free some memory
        delete iPSModel;
        iPSModel = NULL;
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
            break;
            }
        case EPSUICmdEdit:
            {
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
                }
            break;
            }                       
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::SettingCountAndIndex()
// See header for details.
// ---------------------------------------------------------------------------
//
TInt CPSUIGSPlugin::SettingCountAndIndex( TInt& /*aIndex*/ )
    {
    return KErrNone;        
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
    if ( aCount == 1 )
        {
        }
    else
        {
        }

    CAknQueryDialog* query = CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );        
    TInt ok = KErrNone;
    
    CleanupStack::PopAndDestroy( 2, currentName );//currentName, string       
        
    return ( ok );    
    }

// ---------------------------------------------------------------------------
// CPSUIGSPlugin::DynInitMenuPaneL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUIGSPlugin::DynInitMenuPaneL( TInt /*aResourceId*/, CEikMenuPane */*aMenuPane*/ )
    {
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
    }
    
// ---------------------------------------------------------------------------
// CPSUIGSPlugin::RemoveSettingView()
// See header for details.
// ---------------------------------------------------------------------------
//    
void CPSUIGSPlugin::RemoveSettingView()
    {
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
    }

// End of file
