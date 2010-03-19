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
* Description:    XDM GS plugin, UI(CAknView) interface header
*
*/




#ifndef GS_XDMPLUGIN_H
#define GS_XDMPLUGIN_H

// INCLUDES
#include <aknview.h>
#include <eikclb.h>
#include <ConeResLoader.h>
#include <gsplugininterface.h>

#include "XDMPlugin.hrh"

// FORWARD DECLARATIONS
class CXDMPluginContainer;
class CXDMPluginSLContainer;
class CAknViewAppUi;
class CSettingsData;

/** 
* This UID is used for both the view UID and the ECOM plugin implementation 
* UID.
*/
//const TUid KGSXDMPluginUid = { 0x10207429 };
  
// CLASS DECLARATION

/**
*  CXDMPlugin view class (CAknView).
*
* This is XDM GS plugin.
*/
class CXDMPlugin : public CGSPluginInterface
    {
    
    public: // Constructors and destructor
        
        /**
        * Symbian OS two-phased constructor
        * @return 
        */
        static CXDMPlugin* NewL( TAny* aInitParams );
    
        /**
        * Destructor.
        */
        ~CXDMPlugin();
        
        /**
        * Load the SettingsView with specified XDM set and type of loading
        * The types are EEditExisting, ENewDefault, ENewFromExisting
        */
        void LoadSettingsViewL(TXDMSettingsViewType aType, TDesC& aXDMSetName);

        /**
        * Loads the Main View where list of XDM sets are visible
        */
        void LoadMainViewL();

         /**
         * Manual resource change method
         * @param aType ype of resource
         */
         void HandleResourceChangeManual(TInt aType);

        /**
        * Hides MSK if not needed
        */
        void UpdateMSK();

    public: // From CAknView
        
        /**
        * This function is used for identifying the plugin
        */
        TUid Id() const;
        
        /**
        * See CAknView
        */
        //void HandleClientRectChange();
        void HandleViewRectChange();
        
        /**
        * See CAknView
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        /**
        * See CAknView
        */
        void DoDeactivate();

        /**
        * See CAknView
        */
        void HandleCommandL( TInt aCommand );
        
 

    protected:

        /**
        * C++ default constructor.
        */
        CXDMPlugin( );

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();
        
    private:
    
        /**
        * See base classes
        */
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
        
    public: // From CGSPluginInterface
    
        /**
        * See CGSPluginInterface
        */
        void GetCaptionL( TDes& aCaption ) const;

        /**
        * See CGSPluginInterface
        */
        TInt PluginProviderCategory() const;
        
        /**
        * @see CGSPluginInterface
        */
        CGulIcon* CreateIconL( const TUid aIconType );
        
        
    protected: //Data        
        
        // Reference to application UI - not owned.
        CAknViewAppUi* iAppUi;
        
        // RConeResourceLoader
        RConeResourceLoader iResources;
        
        // Previous View ID
        TVwsViewId iPrevViewId; // Previous view.
        
    private:
    
        // Pointer to the main list container - owned
        CXDMPluginContainer* iMainListContainer;
        
        // Pointer to the setting list container - owned.
        CXDMPluginSLContainer* iSettingListContainer;
        
        // Pointer to the current container - not owned.
        CCoeControl* iCurrentContainer;     
    };

#endif // GS_XDMPLUGIN_H
