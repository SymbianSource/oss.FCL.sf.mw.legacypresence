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
* Description:    Header for Presence Settings UI GS plugin container.
*
*/




#ifndef PSUIGSPLUGINCONTAINER_H
#define PSUIGSPLUGINCONTAINER_H

// INCLUDES

// FORWARD DECLARATIONS
class CPSUIGSPlugin;

// CLASS DECLARATION

/**
* CPSUIGSPluginContainer container class
* @since Series60_3.2
*/
class CPSUIGSPluginContainer
    {
    public: // Constructors and destructor
        
        /**
        * Default constructor.
        */        
        CPSUIGSPluginContainer( CPSUIGSPlugin* aView );

        /**
        * Destructor.
        */
        ~CPSUIGSPluginContainer();
    
    private: // Functions from base classes 

        /**
        * From CCoeControl
        * See base class.        
        */ 
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
    private: // New

        /**
        * Sets empty texts to listbox
        *
        * @param none
        */
        void SetEmptyTextsToListboxL();

    private: // Constructor
    
        /**
        * Symbian OS constructor.
        *
        * @param aRect Listbox's rect.
        */
        void ConstructL( const TRect& aRect );
        
    private: // Data
    
        // Handle to view        
        CPSUIGSPlugin* iView; // not owned
    };

#endif //PSUIGSPLUGINCONTAINER_H   
// End of File
