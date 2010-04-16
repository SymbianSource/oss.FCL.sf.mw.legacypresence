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




// INCLUDE FILES
#include <psuigspluginrsc.rsg>
#include <aknradiobuttonsettingpage.h>
#include <akntextsettingpage.h>
#include "psuigspluginmodel.h"
#include "psuisipxdmsettingitem.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CPSUISipXdmSettingItem::CPSUISipXdmSettingItem()
// Default constructor.
// ---------------------------------------------------------------------------
//
CPSUISipXdmSettingItem::CPSUISipXdmSettingItem( 
    TInt aIdentifier, TInt& aId, CPSUIGSPluginModel* aPSModel ): 
        CAknSettingItem( aIdentifier ),
        iExtChosenSettingID ( aId ),
        iPSModel ( aPSModel )
    {
    }        

// ---------------------------------------------------------------------------
// CPSUISipXdmSettingItem::~CPSUISipXdmSettingItem()
// Destructor
// ---------------------------------------------------------------------------
//
CPSUISipXdmSettingItem::~CPSUISipXdmSettingItem()
    {
    // iPSModel deleted elsewhere
    delete iPSUIInternalText;
    }

// ---------------------------------------------------------------------------
// CPSUISipXdmSettingItem::LoadL()
// See header for details. 
// ---------------------------------------------------------------------------
//    
void CPSUISipXdmSettingItem::LoadL()
    {
    delete iPSUIInternalText;
    iPSUIInternalText = 0;
    
    TInt id = Identifier();
    if ( id == EPSUISIPProfileId )
        {// SIP
        iPSUIInternalText = iPSModel->SipProfileNameL( iExtChosenSettingID );
        }
    else if ( id ==  EPSUIXDMSettingsId )
        {// XDM
        iPSUIInternalText = iPSModel->XdmCollectionNameL( iExtChosenSettingID );
        }
    else
        {
        __ASSERT_DEBUG( 0,User::Panic( KPSUIGSPluginPanicCategory, KErrNotFound ));
        }        

    iIntChosenSettingID = iExtChosenSettingID;
    }    

// ---------------------------------------------------------------------------
// CPSUISipXdmSettingItem::StoreL()
// See header for details. 
// ---------------------------------------------------------------------------
// 
void CPSUISipXdmSettingItem::StoreL()
    {
    iExtChosenSettingID = iIntChosenSettingID;
    }

// ---------------------------------------------------------------------------
// CPSUISipXdmSettingItem::SettingTextL()
// See header for details. 
// ---------------------------------------------------------------------------
// 
const TDesC& CPSUISipXdmSettingItem::SettingTextL()
    {
    if ( iPSUIInternalText->Length() > 0 )
        return *iPSUIInternalText;
    else
        return EmptyItemText();
    }    

// ---------------------------------------------------------------------------
// CPSUISipXdmSettingItem::EditItemL()
// See header for details. 
// ---------------------------------------------------------------------------
//     
void CPSUISipXdmSettingItem::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    TInt id = Identifier();
    if ( id == EPSUISIPProfileId )
        {// SIP
        EditSipItemL();
        }
    else if ( id ==  EPSUIXDMSettingsId )
        {// XDM
        EditXdmItemL();
        }
    else
        {
        __ASSERT_DEBUG( 0,User::Panic( KPSUIGSPluginPanicCategory, KErrNotFound ));
        }
    }    

// ---------------------------------------------------------------------------
// CPSUISipXdmSettingItem::EditSipItemL()
// See header for details.
// ---------------------------------------------------------------------------
//  
void CPSUISipXdmSettingItem::EditSipItemL()
    {
    CDesCArray* array = iPSModel->SipProfilesLC();
    TInt index = iPSModel->SipProfileIndex( iIntChosenSettingID );
    LaunchSettingDlgL( index, array );
    CleanupStack::PopAndDestroy( array );  // array
    
    if ( index != KErrNotFound )
        {// update only if index available
        iIntChosenSettingID = iPSModel->SipProfileId( index );

        delete iPSUIInternalText;
        iPSUIInternalText = 0;
        iPSUIInternalText = iPSModel->SipProfileNameL( iIntChosenSettingID );
        
        UpdateListBoxTextL();
        }
    }

// ---------------------------------------------------------------------------
// CPSUISipXdmSettingItem::EditXdmItemL()
// See header for details.
// ---------------------------------------------------------------------------
//  
void CPSUISipXdmSettingItem::EditXdmItemL()
    {
    CDesCArray* array = iPSModel->XdmCollectionNamesL();// array not owned
    TInt index = iPSModel->XdmSettingIndex( iIntChosenSettingID );
    LaunchSettingDlgL( index, array );

    if ( index != KErrNotFound )
        {// update only if index available
        iIntChosenSettingID = iPSModel->XdmSettingId( index );

        delete iPSUIInternalText;
        iPSUIInternalText = 0;
        iPSUIInternalText = iPSModel->XdmCollectionNameL( iIntChosenSettingID );
        
        UpdateListBoxTextL();
        }
    } 

// ---------------------------------------------------------------------------
// CPSUISipXdmSettingItem::LaunchSettingDlgL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUISipXdmSettingItem::LaunchSettingDlgL( TInt& /*aIndex*/, CDesCArray* /*aArray*/ )
    {
    }

// End of File
