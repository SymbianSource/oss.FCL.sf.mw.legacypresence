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
* Description:    XDM GS plugin, Setting List class
*
*/




#ifndef CXDMPLUGINSETTINGLIST_H
#define CXDMPLUGINSETTINGLIST_H

// INCLUDE FILES
#include <coecntrl.h>
#include <aknsettingitemlist.h>
#include <cmmanagerext.h>

// FORWARD DECLARATIONS
class CSettingsData;

/**
*  CXDMPluginSettinglist derived from CAknSettingItemList
*/
class CXDMPluginSettinglist : public CAknSettingItemList
    {
    
    public:
        
        /**
        * Symbian OS two-phased constructor
        * @param reference to CSettingsData from which data to be displayed
        */
        static CXDMPluginSettinglist* NewL(CSettingsData &aData);
        
        /**
        * Symbian OS two-phased constructor, leave pointer to stack
        * @param reference to CSettingsData from which data to be displayed
        */
        static CXDMPluginSettinglist* NewLC(CSettingsData &aData);

        /**
        * Destructor.
        */
        virtual ~CXDMPluginSettinglist();

        /**
        * Inherited from base classes. See CCoeControl
        */
        void SizeChanged();
      
        /**
        * Edit the indexed item. This probably came as a result of UI command
        * @param aIndex index to the item
        * aCalledFromMenu ETrue of this call resulted from UI command
        */
        void EditItemL (TInt aIndex, TBool aCalledFromMenu);
      
        /**
        * Actual editing of item done here, from whereever the call is made from
        */
        void EditCurrentItemL();
      
        /**
        * Gets the Access point name
        * @param aAP Access point number as integer
        * aAccessPoint reference to TDes, doesnt change if AP not found
        * otherwise access point name returns here
        */
        void GetAccessPointNameL(TInt32 aAP, TDes& aAccessPoint);
      
        /**
        * Sets the focus to the first item of the settings list
        */
        void ResetItemIndex();

    private:
        
        
        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();
        
        /**
        * C++ constructor
        * @param reference to CSettingsData
        */
        CXDMPluginSettinglist(CSettingsData &aData);

        /**
        * See CAknSettingItemList
        */
        CAknSettingItem* CreateSettingItemL (TInt aSettingId);
      
        /**
        * Edit the AccessPoint, We edit the access point differently than other
        * settings. We bypass the list's text editor and open the S60 standard
        * access point editor 
        */
        void EditAccessPointL();
      
        /**
        * Saves the changed settings to the list's data
        * @param aIndex index of the setting to be saved
        */
        void SaveSettingL(TInt aIndex);
      
        /**
        * Sets the title pane text with given discriptor
        * @param aTitleText text to be shown on title pane
        */
        void SetTitlePaneTextL( const TDesC& aTitleText ) const;

    private:
    
        /**
         *  Reference to the data owned by container
         */
        CSettingsData& iSettingsData;
        
        /**
         * Handle to connection method manager.
         * Own.
         */
        RCmManagerExt iCmManagerExt;
      
    };

#endif // CXDMPLUGINSETTINGLIST_H
