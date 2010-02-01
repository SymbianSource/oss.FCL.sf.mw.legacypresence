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
* Description:    XDM GS plugin, Setting List class implementation
*
*/




// INCLUDE FILES
#include "XDMExternalInterface.h"

#include <barsread.h>
#include <ApSettingsHandlerUI.h>
#include <ApUtils.h>
#include <commdb.h>
#include <avkon.loc>
#include <StringLoader.h>
#include <akntitle.h>
#include <eikspane.h>

#include "XDMPlugin.hrh"
#include "XDMPluginSettinglist.h"
#include "SettingsData.h"

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::NewL(CSettingsData &aData)
// -----------------------------------------------------------------------------
// 
CXDMPluginSettinglist *CXDMPluginSettinglist::NewL(CSettingsData &aData)
    {
    CXDMPluginSettinglist* self = CXDMPluginSettinglist::NewLC(aData);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::NewLC(CSettingsData &aData)
// -----------------------------------------------------------------------------
// 
CXDMPluginSettinglist *CXDMPluginSettinglist::NewLC(CSettingsData &aData)
    {
    CXDMPluginSettinglist* self = new (ELeave) CXDMPluginSettinglist(aData);
    CleanupStack::PushL(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::CXDMPluginSettinglist(CSettingsData &aData)
// -----------------------------------------------------------------------------
// 
CXDMPluginSettinglist::CXDMPluginSettinglist(CSettingsData &aData) : 
    CAknSettingItemList(),
    iSettingsData(aData)  
    {
    }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::~CXDMPluginSettinglist()
// -----------------------------------------------------------------------------
// 
CXDMPluginSettinglist::~CXDMPluginSettinglist()
  {
  // no specific destruction code required - no owned data
  }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::SizeChanged()
// -----------------------------------------------------------------------------
// 
void CXDMPluginSettinglist::SizeChanged()
    {
    // if size changes, make sure component takes whole available space
    if (ListBox()) 
        {
        ListBox()->SetRect(Rect());
        }
    }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::EditCurrentItemL()
// -----------------------------------------------------------------------------
// 
void CXDMPluginSettinglist::EditCurrentItemL()
    {
    // invoke EditItemL on the current item
    TInt index = ListBox()->CurrentItemIndex();

    //Cause SIP setting is hidden
    if(index >= 3) //ESipURLIndex == 3
      index++;
    
    // if access point need to be edit
    if (index == EAccessPointIndex)
        {
        EditAccessPointL();
        SaveSettingL(index);
        }
    else 
        EditItemL(index,ETrue); // invoked from menu
    }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::EditItemL (TInt aIndex, TBool aCalledFromMenu)
// -----------------------------------------------------------------------------
// 
void CXDMPluginSettinglist::EditItemL (TInt aIndex, TBool aCalledFromMenu)
    {
    if (aIndex == EAccessPointIndex)
      {
      EditAccessPointL();
      }
    else
        {
         CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);
        }
    (*SettingItemArray())[aIndex]->StoreL();
    SaveSettingL(aIndex);
    }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::SaveSettingL(TInt aIndex)
// -----------------------------------------------------------------------------
// 
void CXDMPluginSettinglist::SaveSettingL(TInt aIndex)
    {
    
    switch (aIndex)
        {
        case ESettingNameIndex:
            SetTitlePaneTextL(iSettingsData.iSettingNameDisp);         
            break;
 
         case EAccessPointIndex:
            if (iSettingsData.iAccessPoint > -1) // if Valid AP number
                {
                (iSettingsData.iAccessPointDes).Num(iSettingsData.iAccessPoint);
                GetAccessPointNameL(iSettingsData.iAccessPoint, 
                                                    iSettingsData.iAccessPointName);
                }
            LoadSettingsL();
            break;
 
        case EServerAddressIndex:
            break;
        
        
        case ESipURLIndex:
            break;
        case EUserIDIndex:
            break;
            
        case EPasswordIndex:
            break;
            
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::CreateSettingItemL (TInt aIdentifier) 
// -----------------------------------------------------------------------------
// 
CAknSettingItem * CXDMPluginSettinglist::CreateSettingItemL (TInt aIdentifier) 
    {
    // method is used to create specific setting item as required at run-time.
    // aIdentifier is used to determine what kind of setting item should be 
    // created

    CAknSettingItem* settingItem = NULL;

    switch (aIdentifier)
        {
        case ESettingItemSettingName:
            
            settingItem = new (ELeave) CAknTextSettingItem ( aIdentifier,
                                                   iSettingsData.iSettingNameDisp);
            break;

        case ESettingItemAccessPoint:
            GetAccessPointNameL(iSettingsData.iAccessPoint, iSettingsData.iAccessPointName);
            settingItem = new (ELeave) CAknTextSettingItem (
                          aIdentifier, iSettingsData.iAccessPointName);
            break;

        case ESettingItemServerAddress:
            settingItem = new (ELeave) CAknTextSettingItem (
                          aIdentifier, 
                          iSettingsData.iServerAddress);
            break;


        
        case ESettingItemSipURL:
            settingItem = new (ELeave) CAknTextSettingItem (
                          aIdentifier, 
                          iSettingsData.iSipURL);
                settingItem->SetHidden(ETrue);
            break;
        

        case ESettingItemUserID:
            settingItem = new (ELeave) CAknTextSettingItem (
                          aIdentifier, 
                          iSettingsData.iUserID);
            break;
        case ESettingItemPassword:
            settingItem = new (ELeave) CAknPasswordSettingItem (
                          aIdentifier, 
                          CAknPasswordSettingItem::EAlpha,
                          iSettingsData.iPassword);
            break;
        default:
            break;
        }
    return settingItem;
    }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::EditAccessPoint()
// -----------------------------------------------------------------------------
// 
void CXDMPluginSettinglist::EditAccessPointL()
    {
    // After dialog server for access point selection (RGenConAgentDialogServer)
    // has deprecated wins emulator mode needs a different technique to show
    // emulator-lan access point.
    
    CCommsDatabase* commsDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL(commsDb);
    CApUtils* aPUtils = CApUtils::NewLC( *commsDb );

    CApSettingsHandler *apUi = CApSettingsHandler::NewLC(
                                                        ETrue, 
                                                        EApSettingsSelListIsPopUp,
                                                        EApSettingsSelMenuSelectNormal,
                                                        KEApIspTypeAll,
                                                        EApBearerTypeAllBearers,
                                                        KEApSortNameAscending,
                                                        EIPv4 | EIPv6
                                                        );
    TUint32 id;
    
    TRAP_IGNORE(id = aPUtils->WapIdFromIapIdL(iSettingsData.iAccessPoint));
    
    //err can also be in case this is new set, iSettingsData.iAccessPoint = -1
    //so ignoring the error
    
    if ( apUi->RunSettingsL( id, id ) == KApUiEventSelected)
        {
    iSettingsData.iAccessPoint = aPUtils->IapIdFromWapIdL(id);
        }
    CleanupStack::PopAndDestroy(3, commsDb); //commsDb, aPUtils, apUi
    }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::GetAccessPointNameL(TInt32 aAP, TDes& aAccessPoint)
// -----------------------------------------------------------------------------
// 
void CXDMPluginSettinglist::GetAccessPointNameL(TInt32 aAP, TDes& aAccessPoint)
    {
#if defined __WINS__ && defined _DEBUG // handled differently in wins+debug
    if (aAP != KErrNotFound) // if access point is defined take a general name
        StringLoader::Load ( aAccessPoint, R_STR_XDM_AP_NAME_FOR_DEBUG_ONLY);    
#else
    CCommsDatabase* commsDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL(commsDb);
    CApUtils* aPUtils = CApUtils::NewLC( *commsDb );
    TInt err(KErrNone);
    // to remove id bug
    TRAP(err, aAP = aPUtils->WapIdFromIapIdL(aAP)); 
    TRAP(err, aPUtils->NameL(aAP, aAccessPoint));
    // dont do anything if name not found or if some error occur
    CleanupStack::PopAndDestroy(2); // commsDb, aPUtils
#endif 
    }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::SetTitlePaneTextL( const TDesC& aTitleText ) const
// -----------------------------------------------------------------------------
// 
void CXDMPluginSettinglist::SetTitlePaneTextL( const TDesC& aTitleText ) const
    {
    CAknTitlePane* title = static_cast< CAknTitlePane* >
        ( CEikonEnv::Static()->AppUiFactory()->StatusPane()->ControlL(
                                        TUid::Uid( EEikStatusPaneUidTitle ) ) );
    if ( !title )
        {
        User::Leave( KErrNotSupported );
        }

    title->SetTextL( aTitleText );
    }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::ResetItemIndex()
// -----------------------------------------------------------------------------
// 
void CXDMPluginSettinglist::ResetItemIndex()
    {
    ListBox()->SetCurrentItemIndexAndDraw(NULL);
    }

// End of File
