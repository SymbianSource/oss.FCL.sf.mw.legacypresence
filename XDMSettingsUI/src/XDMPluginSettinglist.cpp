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
#include <cmconnectionmethodext.h>
#include <cmapplicationsettingsui.h>
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
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::ConstructL()
// -----------------------------------------------------------------------------
// 
void CXDMPluginSettinglist::ConstructL()
    {
    iCmManagerExt.OpenL();
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
  iCmManagerExt.Close();
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
    TCmSettingSelection selectionUid;
    CCmApplicationSettingsUi* settingsUi =  CCmApplicationSettingsUi::NewLC();
    // All Connection Methods will be listed if bearerFilter array has no element.
    TBearerFilterArray bearerFilter;
    CleanupClosePushL( bearerFilter );
    settingsUi->RunApplicationSettingsL( selectionUid ,
                                         CMManager::EShowConnectionMethods,
                                         bearerFilter );
    CleanupStack::PopAndDestroy( &bearerFilter );    
    CleanupStack::PopAndDestroy( settingsUi );

    if ( selectionUid.iResult == CMManager::EConnectionMethod )
        {
        iSettingsData.iAccessPoint = selectionUid.iId;
        }
    }

// -----------------------------------------------------------------------------
// CXDMPluginSettinglist::GetAccessPointNameL(TInt32 aAP, TDes& aAccessPoint)
// -----------------------------------------------------------------------------
// 
void CXDMPluginSettinglist::GetAccessPointNameL(TInt32 aAP, TDes& aAccessPoint)
    {
    #ifdef _DEBUG
    RDebug::Print( _L( "CXDMPluginSettinglist::GetAccessPointNameL - IN" ) );
    #endif
    if ( aAP > KErrNotFound )
        {
        RCmConnectionMethodExt connMethod = iCmManagerExt.ConnectionMethodL( aAP );
        CleanupClosePushL( connMethod );
        
        HBufC* connName = connMethod.GetStringAttributeL( CMManager::ECmName );
        CleanupStack::PushL( connName );
        
        if ( KMaxAccessPointNameLength >= connName->Des().Length() )
            {
            aAccessPoint.Copy( connName->Des() );
            }
        else
            {
            aAccessPoint.Copy( connName->Des().Left( KMaxAccessPointNameLength ) );
            }
        
        CleanupStack::PopAndDestroy( connName );
        CleanupStack::PopAndDestroy( &connMethod );
        #ifdef _DEBUG  
        RDebug::Print( _L( "CXDMPluginSettinglist::GetAccessPointNameL - Name: %S"),
                                &aAccessPoint );
        #endif
        }
    #ifdef _DEBUG
    RDebug::Print( _L( "CXDMPluginSettinglist::GetAccessPointNameL - OUT id: %d:" ),
                                aAP );
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
