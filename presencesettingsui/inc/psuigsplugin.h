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
* Description:    GSFW plugin header for Presence Settings UI GS plugin.
*
*/




#ifndef PSUIGSPLUGIN_H
#define PSUIGSPLUGIN_H

//  INCLUDES
#include <ConeResLoader.h>
#include <coeview.h>

// FORWARD DECLARATIONS
class CPSUIGSPluginContainer;
class CPSUIGSPluginModel;

// CLASS DECLARATION
class CEikMenuPane;

/**
* Header of CPSUIGSPlugin which implements the CGSPluginInterface.
*
* @lib PSUIGSPlugin.lib
* @since Series60_3.2
*/
class CPSUIGSPlugin :
    public MCoeViewDeactivationObserver
    {
    
    public: // Constructors and destructor
            
        /**
        * Symbian OS two-phased constructor
        */
        static CPSUIGSPlugin* NewL( TAny* aAppUi );
    
        /**
        * Destructor.
        */
        ~CPSUIGSPlugin();

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
        * From MEikMenuObserver
        * See base class.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane *aMenuPane );

        /**
        * From MCoeViewDeactivationObserver
        * Used to be able to free memory when plugin get's closed.
        * See base class.
        */        
        void HandleViewDeactivation ( 
            const TVwsViewId& aViewIdToBeDeactivated,
            const TVwsViewId& aNewlyActivatedViewId );
        
    public: // New

        /**
        * Gives a handle to PS Model. Ownership is not transferred.
        *
        * @return handle to PS Model
        */       
        CPSUIGSPluginModel* PSModel();

        /**
        * Takes care of settings deletion.
        */        
        void DeleteSettingsL();        
    
    private: // New
    
        /**
        * Returns count and index of current item in listbox.
        *
        * @param aIndex returns the index. If no items, KErrNotFound.
        * @return TInt containing the count.
        */
        TInt SettingCountAndIndex( TInt& aIndex );

        /**
        * Shows the delete certain setting set confirmation note.
        *
        * @param aCount is the amount of all settings
        * @param aIndex is the to be deleted setting set
        * @return TBool about proceed with deletion.
        */        
        TBool AcceptDeletionL( TInt aCount, TInt aIndex );
    
        /**
        * Constructs the setting view
        */        
        void ConstructSettingViewL();

        /**
        * Removes the setting view
        */
        void RemoveSettingView();

        /**
        * Create new default settings and launch setting view
        */        
        void CreateNewDefaultSettingsL();

        /**
        * Create new a copy of existing settings and launch setting view
        */        
        void CreateCopyOfExistingSettingsL();
        
        /**
        * Toggles visibility of MSK depending on the amount of setting sets
        */        
        void UpdateMSK();
            
    private: // Constructors

        /**
        * C++ default constructor.
        */
        CPSUIGSPlugin();

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

    private: // Data
        
        // Presence UI model
        CPSUIGSPluginModel* iPSModel;

        // Indicates when PSUIGSPlugin's views are closed
        TBool iClosing;
        // Previous view
        TVwsViewId iPSUIGSPrevViewId; 
    };

#endif // PSUIGSPLUGIN_H
// End of File
