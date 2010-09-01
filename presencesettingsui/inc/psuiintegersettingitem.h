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
* Description:    Setting item for integer values
*
*/




#ifndef PSUIINTEGERSETTINGITEM_H
#define PSUIINTEGERSETTINGITEM_H

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* CPSUIIntegerSettingItem setting class. Needed to be able to
* keep default values invisible.
* @since Series60_3.2
*/
class CPSUIIntegerSettingItem : public CAknIntegerEdwinSettingItem
    {
    public : // Constructors and destructor
    
        /*
        * C++ default constructor.
        */
        CPSUIIntegerSettingItem( TInt aIdentifier, TInt& aValue );

        /**
        * Destructor
        */         
        ~CPSUIIntegerSettingItem();
    
    private: // Functions from base classes
    
        /**
        * From CAknSettingItem
        * See base class.
        */
        virtual const TDesC& SettingTextL();

        /**
        * From CAknSettingItem
        * See base class.
        */
        virtual void CompleteConstructionL();

    private: // Data
        
        // since no access to base classes descriptors,
        // here are similar ones        
        HBufC* iPSUIInternalText;
        TPtr iPSUIInternalTextPtr;
    };

#endif //PSUIINTEGERSETTINGITEM_H   
// End of File
