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
* Description:    Header for Presence Settings UI setting view container.
*
*/




#ifndef PSUIGSPLUGINSETTINGVIEWCONTAINER_H
#define PSUIGSPLUGINSETTINGVIEWCONTAINER_H

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CAknColumnListBox;
class CPSUIGSPluginSettingView;

// CLASS DECLARATION

/**
* CPSUIGSPluginSettingViewContainer container class
* @since Series60_3.2
*/
class CPSUIGSPluginSettingViewContainer : 
    public CAknSettingItemList
    {
    public: // Constructors and destructor
        
        /**
        * Default constructor.
        */        
        CPSUIGSPluginSettingViewContainer( CPSUIGSPluginSettingView* aView );

        /**
        * Destructor.
        */
        ~CPSUIGSPluginSettingViewContainer();
   
    private: // Functions from base classes
        
        /**
        * From CAknSettingItemList
        * See base class.        
        */    
        CAknSettingItem* CreateSettingItemL( TInt aSettingId );

        /**
        * From CCoeControl
        * See base class.        
        */ 
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /**
        * From CCoeControl
        * See base class.        
        */         
        void HandleResourceChange(TInt aType);

        /**
        * From CCoeControl
        * See base class.        
        */         
        void SizeChanged();      

    private: // New
    
        /**
        * Update titlepane with setting name
        *
        * @param none
        */
        void UpdateTitleL();

    public: // Functions from base classes
           
        /**
        * From CAknSettingItemList
        * See base class.        
        */    
        void EditItemL( TInt aIndex, TBool aCalledFromMenu );

    public: // New
    
        /**
        * Edit setting which is currently under focus.
        *
        * @param none
        */
        void EditCurrentItemL();
        
    public: // Constructor
    
        /**
        * Symbian OS constructor.
        *
        * @param aRect Listbox's rect.
        */
        void ConstructL( const TRect& aRect );
        
    private: // Data        

        // Pointer to view
        CPSUIGSPluginSettingView* iView;
    };

#endif //PSUIGSPLUGINSETTINGVIEWCONTAINER_H   
// End of File
