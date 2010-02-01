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
* Description:    Setting item for integer settings
*
*/




// INCLUDE FILES
#include <aknradiobuttonsettingpage.h>
#include <akntextsettingpage.h>
#include "psuigspluginmodel.h"
#include "psuiintegersettingitem.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CPSUIIntegerSettingItem::CPSUIIntegerSettingItem()
// See header for details.
// ---------------------------------------------------------------------------
//
CPSUIIntegerSettingItem::CPSUIIntegerSettingItem( TInt aIdentifier, TInt& aValue )
    : CAknIntegerEdwinSettingItem ( aIdentifier, aValue ), 
    iPSUIInternalTextPtr( 0,0 )
    {
    }
    
// ---------------------------------------------------------------------------
// CPSUIIntegerSettingItem::~CPSUIIntegerSettingItem()
// See header for details.
// ---------------------------------------------------------------------------
//
CPSUIIntegerSettingItem::~CPSUIIntegerSettingItem()
    {
    delete iPSUIInternalText;
    }

// ---------------------------------------------------------------------------
// CPSUIIntegerSettingItem::CompleteConstructionL()
// See header for details.
// ---------------------------------------------------------------------------
//        
void CPSUIIntegerSettingItem::CompleteConstructionL()
    {
    const TInt KPSUIMaxIntegerDigits = 2 * EAknMaxIntegerDigits;// a bit longer for storing also empty texts
    iPSUIInternalText = HBufC::NewMaxL( KPSUIMaxIntegerDigits );
    }

// ---------------------------------------------------------------------------
// CPSUIIntegerSettingItem::SettingTextL()
// See header for details.
// ---------------------------------------------------------------------------
//    
const TDesC& CPSUIIntegerSettingItem::SettingTextL()
    {
    _LIT( KAknIntegerFormat, "%d" );
    iPSUIInternalTextPtr.Set( iPSUIInternalText->Des() );
    TInt& intValueRef = InternalValueRef();
    if ( intValueRef == KErrNotFound )
        {
        iPSUIInternalTextPtr.Zero();
        iPSUIInternalTextPtr.Append( EmptyItemText() );
        iPSUIInternalTextPtr.Append( KColumnListSeparator );
        }
    else
        {
        iPSUIInternalTextPtr.Format( KAknIntegerFormat, intValueRef );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
            iPSUIInternalTextPtr );
        }

    return iPSUIInternalTextPtr;
    }

// End of File
