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
* Description:    Presence Settings UI view container implementation
*
*/




// INCLUDE FILES
#include <psuigspluginrsc.rsg>
#include <aknlists.h>
#include <aknnavi.h>
#include <akntitle.h>
#include <AknUtils.h>
#include "psuigspluginsettingviewcontainer.h"
#include "psuigspluginsettingview.h"
#include "psuigspluginmodel.h"
#include "psuisipxdmsettingitem.h"
#include "psuiintegersettingitem.h"
#include "psuigsplugin.h"
#include "psuigspluginids.hrh"
#include <csxhelp/pre.hlp.hrh>
#include <akntextsettingpage.h>


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingViewContainer::CPSUIGSPluginSettingViewContainer()
// Default constructor.
// ---------------------------------------------------------------------------
//
CPSUIGSPluginSettingViewContainer::CPSUIGSPluginSettingViewContainer( 
    CPSUIGSPluginSettingView* aView )
    : iView ( aView )
    {
    }
    
// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingViewContainer::ConstructL(const TRect& aRect)
// Symbian OS phase 2 constructor
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginSettingViewContainer::ConstructL( const TRect& aRect )
    {
    UpdateTitleL();
    CreateWindowL();
    ConstructFromResourceL( R_PSUI_SETTING_ITEMS );
    SetRect( aRect );
    ActivateL();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingViewContainer::~CPSUIGSPluginSettingViewContainer()
// Destructor
// ---------------------------------------------------------------------------
//
CPSUIGSPluginSettingViewContainer::~CPSUIGSPluginSettingViewContainer()
    {
    // iView is deleted elsewhere
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingViewContainer::CreateSettingItemL()
// See header for details. 
// ---------------------------------------------------------------------------
//
CAknSettingItem* CPSUIGSPluginSettingViewContainer::CreateSettingItemL( 
    TInt aSettingId )
    {
    CAknSettingItem* settingItem = NULL;
    TPresSettingsSet& settingSet = iView->MainView()->PSModel()->SettingsSet();

    switch ( aSettingId )
        {
        case EPSUIServerNameId:
            settingItem = new (ELeave) CAknTextSettingItem( 
                aSettingId, settingSet.iSetName );
            break;
        case EPSUISIPProfileId:
            settingItem = new (ELeave) CPSUISipXdmSettingItem( 
                aSettingId, settingSet.iSipProfile, iView->MainView()->PSModel() );
            break;
        case EPSUIXDMSettingsId:
            settingItem = new (ELeave) CPSUISipXdmSettingItem( 
                aSettingId, settingSet.iXDMSetting, iView->MainView()->PSModel() );
            break;
        case EPSUIObjectSizeId:
            settingItem = new (ELeave) CPSUIIntegerSettingItem( 
                aSettingId, settingSet.iObjectSize );
            break;
/* server address disabled             
        case EPSUIServerAddrId:
            settingItem = new (ELeave) CAknTextSettingItem( 
                aSettingId, settingSet.iContSrvrAdd );
            break;*/
        case EPSUIPublishingIntervalId:
            settingItem = new (ELeave) CPSUIIntegerSettingItem(
                aSettingId, settingSet.iPublicationInt );
            break;
        case EPSUIMaxSubscriptionsId:
            settingItem = new (ELeave) CPSUIIntegerSettingItem(
                aSettingId, settingSet.iMaxSubscriptions );
            settingItem->SetSettingPageFlags(
                    CAknIntegerSettingPage::EEmptyValueAllowed);// Empty input allowed      
            break;
        case EPSUIMaxContactsInListId:
            settingItem = new (ELeave) CPSUIIntegerSettingItem( 
                aSettingId, settingSet.iMaxContactsInList );
            settingItem->SetSettingPageFlags(
                    CAknIntegerSettingPage::EEmptyValueAllowed);// Empty input allowed      
            break;
        case EPSUIDomainSyntaxId:
            settingItem = new (ELeave) CAknTextSettingItem( 
                aSettingId, settingSet.iDomainSyntax );
            break;            
        default:
            __ASSERT_DEBUG( 0, User::Panic( KPSUIGSPluginPanicCategory, KErrNotFound ));
            break;
        }

    return settingItem;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingViewContainer::EditItemL()
// See header for details. 
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginSettingViewContainer::EditItemL( 
    TInt aIndex, TBool aCalledFromMenu )
    {
    CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );

    // update setting to setting-object
    (*SettingItemArray())[aIndex]->StoreL();
    (*SettingItemArray())[aIndex]->UpdateListBoxTextL();
    ListBox()->DrawNow();
    
    if ( EPSUIServerNameId == aIndex )
        {
        UpdateTitleL();
        }
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingViewContainer::EditCurrentItemL()
// See header for details. 
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginSettingViewContainer::EditCurrentItemL()
    {
    EditItemL( ListBox()->CurrentItemIndex(), ETrue );
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginSettingViewContainer::UpdateTitleL()
// See header for details. 
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginSettingViewContainer::UpdateTitleL()
    {
    // title construction
    static_cast<CAknTitlePane*>( 
        iAvkonAppUi->StatusPane()->ControlL( TUid::Uid( 
        EEikStatusPaneUidTitle )))->SetTextL( 
        iView->MainView()->PSModel()->SettingsSet().iSetName );
    }
    
// -----------------------------------------------------------------------------
// CPSUIGSPluginSettingViewContainer::GetHelpContext()
// See header for details.
// -----------------------------------------------------------------------------
//
void CPSUIGSPluginSettingViewContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = TUid::Uid( PSUIGSPluginDllUid );
    aContext.iContext = KPRE_HLP_EDIT_SET;
    }

// -----------------------------------------------------------------------------
// CPSUIGSPluginSettingViewContainer::HandleResourceChange()
// See header for details.
// -----------------------------------------------------------------------------
//    
void CPSUIGSPluginSettingViewContainer::HandleResourceChange(TInt aType)
    {
    CAknSettingItemList::HandleResourceChange( aType );
	if ( aType == KAknsMessageSkinChange ||
         aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                           mainPaneRect);
        SetRect( mainPaneRect );
        }        
    }

// -----------------------------------------------------------------------------
// CPSUIGSPluginSettingViewContainer::SizeChanged()
// See header for details.
// -----------------------------------------------------------------------------
//   
void CPSUIGSPluginSettingViewContainer::SizeChanged()
    {
    ListBox()->SetRect( Rect() );
    }

// End of File
