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
* Description:    Presence Settings UI's setting view.
*
*/




#ifndef PSUIGSPLUGINSETTINGVIEW_H
#define PSUIGSPLUGINSETTINGVIEW_H

//  INCLUDES
#include <aknview.h>

// FORWARD DECLARATIONS
class CPSUIGSPluginContainer;
class CPSUIGSPluginSettingViewContainer;
class CPSUIGSPlugin;

const TUid KPreSettingViewId = {313};

// CLASS DECLARATION

/**
* Header of CPSUIGSPluginSettingView.
*
* @lib PSUIGSPlugin.lib
* @since Series60_3.2
*/
class CPSUIGSPluginSettingView : 
    public CAknView
    {
    
    public: // Constructors and destructor
            
        /**
        * Symbian OS two-phased constructor
        */
        static CPSUIGSPluginSettingView* NewL( CPSUIGSPlugin* aMainView );
    
        /**
        * Destructor.
        */
        ~CPSUIGSPluginSettingView();

    private: // Functions from base classes
                
        /**
        * From CAknView
        * See base class.
        */
        TUid Id() const;

        /**
        * From CAknView
        * See base class.
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        
        /**
        * From CAknView
        * See base class.
        */
        void DoDeactivate();
        
        /**
        * From CAknView
        * See base class.
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * From CAknView
        * See base class.
        */
        void HandleViewRectChange();
    
    public : // New

        /**
        * Gives a handle to plugin main view. Ownership is not transferred.
        *
        * @return handle to plugin main view
        */          
        CPSUIGSPlugin* MainView();
        
        /**
        * Uses model to check if name is a duplicate and if it
        * is shows a note.
        *
        * @return TBool about if duplicates were found
        */  
        TBool HandleDuplicatePSNamesL();
    
    private: // New
    
        /**
        * Creates the setting view container
        *
        * @param none
        */
        void CreateContainerL();

        /**
        * Handles back-key presses. Checks if all compulsory
        * fields are filled and if there is duplicates. Based on
        * this saves and switches view back to main view.
        */  
        void HandleBackKeyL();
        
        /**
        * Handles saving funtionality when Exit is chosen from Menu. 
        * Checks if all compulsory fields are filled and in case of 
        * duplicate changes name, after this saves settings.
        */
        void HandleMenuExitL();
        
    private: // Constructors

        /**
        * C++ default constructor.
        */
        CPSUIGSPluginSettingView( CPSUIGSPlugin* aMainView );

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

    private: // Data
    
        // Previous view ID
        TVwsViewId iPrevViewId;  
        // Pointer to application UI.
        CAknViewAppUi* iAppUi;        
        // Presence setting view container
        CPSUIGSPluginSettingViewContainer* iPSUIContainer;
        // Handle to main view        
        CPSUIGSPlugin* iMainView; // not owned
    };

#endif // PSUIGSPLUGINSETTINGVIEW_H
// End of File
