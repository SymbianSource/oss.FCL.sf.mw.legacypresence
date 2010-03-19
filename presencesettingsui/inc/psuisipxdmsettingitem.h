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
* Description:    Setting item for SIP / XDM settings
*
*/




#ifndef PSUISIPXDMSETTINGITEM_H
#define PSUISIPXDMSETTINGITEM_H

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* CPSUISipXdmSettingItem setting class
* @since Series60_3.2
*/
class CPSUISipXdmSettingItem : public CAknSettingItem
    {
    
    public: // Constructors and destructor

        /*
        * C++ default constructor.
        */
        CPSUISipXdmSettingItem( TInt aIdentifier, TInt& aId, CPSUIGSPluginModel* iPSModel );

        /**
        * Destructor
        */ 
        ~CPSUISipXdmSettingItem();

    private: // New
    
        /**
        * Like in CAknSettingItem, but SIP specific.
        */   
        void EditSipItemL();
        
        /**
        * Like in CAknSettingItem, but XDM specific.
        */           
        void EditXdmItemL();
        
        /**
        * Launches the actual radio button setting dialog with data given.
        *
        * @param aIndex is currently selected setting
        * @param aArray contains the settings to choose from
        */           
        void LaunchSettingDlgL( TInt& aIndex, CDesCArray* aArray );

    private: // Functions from base classes

        /**
        * From CAknSettingItem
        * See base class.
        */
        virtual void StoreL();

        /**
        * From CAknSettingItem
        * See base class.
        */
        virtual void LoadL();

        /**
        * From CAknSettingItem
        * See base class.
        */
        virtual const TDesC& SettingTextL();

        /**
        * From CAknSettingItem
        * See base class.
        */
        virtual void EditItemL( TBool aCalledFromMenu );

    protected: // Data
        
        // reference to external value
        TInt& iExtChosenSettingID;
        // internal value
        TInt iIntChosenSettingID;
        // pointer to PSUI model
        CPSUIGSPluginModel* iPSModel;// owned elsewhere
        // internal text having the listbox text
        HBufC* iPSUIInternalText;// owned
};

#endif //PSUISIPXDMSETTINGITEM_H   
// End of File
