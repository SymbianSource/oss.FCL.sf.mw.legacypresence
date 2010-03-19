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
* Description:    XDM GS plugin main list container.
*
*/




// INCLUDE FILES
#include "XDMExternalInterface.h"

#include <aknappui.h> 
#include <akntitle.h>
#include <eikspane.h>
#include <barsread.h>
#include <eiktxlbm.h>
#include <StringLoader.h>
#include <aknlists.h>
#include <AknUtils.h>
#include <eiksbfrm.h>
#include <AknQueryDialog.h>
#include <XdmSettingsApi.h>

#include "SettingsData.h"
#include "XDMPluginContainer.h"

#include <csxhelp/xdm.hlp.hrh>



   
// ---------------------------------------------------------------------------
// CXDMPluginContainer::CXDMPluginContainer()
// ---------------------------------------------------------------------------
//
CXDMPluginContainer::CXDMPluginContainer(CAknView* aView) : iView(aView)
    {
    
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginContainer::ConstructL(const TRect& aRect)
// Symbian OS two phased constructor
// 
// ---------------------------------------------------------------------------
//
void CXDMPluginContainer::ConstructL( const TRect& aRect )
    {
    #ifdef _DEBUG  
    RDebug::Print( _L( "[CXDMPluginContainer] ConstructL()" ) );
	#endif

    CEikStatusPane* sp = static_cast<CAknAppUi*> 
        ( CEikonEnv::Static()->EikAppUi() )->StatusPane();
    iTitlePane = static_cast<CAknTitlePane*> 
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    // Set title
    
    StringLoader::Load ( iTitle, R_STR_XDM_TITLE_PANE_TEXT );
    iTitlePane->SetTextL(iTitle);// FromResourceL( rReader );
    CreateWindowL(); // Makes the control a window-owning control

    // Main List creation and initialization
    iMainList = new(ELeave) CAknSingleStyleListBox();
    iMainList->SetContainerWindowL(*this);
    iMainList->ConstructL(this, EAknListBoxLoopScrolling);

    // Main list scroll bar issues
    iMainList->CreateScrollBarFrameL(EFalse);
    iMainList->ScrollBarFrame()->SetScrollBarVisibilityL(
    CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    
    // Empty text processing
    _LIT (KStringHeader, "%S\n%S");
    HBufC* emptyText = iEikonEnv->AllocReadResourceLC( R_STR_XDM_EMPTY_TEXT );
    HBufC* emptyText2 = iEikonEnv->AllocReadResourceLC( R_STR_XDM_EMPTY_TEXT_SEC);   
    HBufC* emptyFinal = HBufC::NewLC( 
        emptyText->Length() + emptyText2->Length() + 2 );// 2 is length of "\n" 
    emptyFinal->Des().Format(KStringHeader, emptyText, emptyText2);
    
    iMainList->SetListBoxObserver(this);
    iMainList->View()->SetListEmptyTextL(*emptyFinal);
    CleanupStack::PopAndDestroy(3, emptyText);//emptyText, emptyText2, emptyFinal

    // This call fails if Problems in XDM API, so leave if error
    TRAPD(err,SetupListL());
    User::LeaveIfError(err);

    SetRect( aRect );
    ActivateL();
    
    #ifdef _DEBUG           
    RDebug::Print( _L( "[CXDMPluginContainer] Construct done" ) );
	#endif

    }

// ---------------------------------------------------------------------------
// CXDMPluginContainer::~CXDMPluginContainer()
// Destructor
//  
// ---------------------------------------------------------------------------
//
CXDMPluginContainer::~CXDMPluginContainer()
    {
    if (iMainList)
        {
         delete iMainList;
        }
    }

// ---------------------------------------------------------------------------
// CXDMPluginContainer::SizeChanged()
// ---------------------------------------------------------------------------
//
void CXDMPluginContainer::SizeChanged()
    {
    iMainList->SetRect(Rect());
    }

// ---------------------------------------------------------------------------
// CXDMPluginContainer::CountComponentControls() const
// ---------------------------------------------------------------------------
//
TInt CXDMPluginContainer::CountComponentControls() const
    {
    return 1;
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginContainer::ComponentControl() const
// ---------------------------------------------------------------------------
//
CCoeControl* CXDMPluginContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iMainList;
        default:
            return NULL;
        }
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginContainer::EditCurrentItemL()
// ---------------------------------------------------------------------------
//
void CXDMPluginContainer::EditCurrentItemL()
    {
    TPtrC16 xDMSetName;
    TPtrC16 xDMSetNameClipped;
    xDMSetName.Set(iSettingListArray->MdcaPoint(iMainList->CurrentItemIndex()));
    xDMSetNameClipped.Set(xDMSetName.Mid(1)); // remove '/t' from the begining
        CXDMPlugin* iTempView = static_cast<CXDMPlugin*> (iView);    
    iTempView->LoadSettingsViewL(EEditExisting, xDMSetNameClipped);      
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginContainer::GetCurrentSetNameLC()
// ---------------------------------------------------------------------------
//
HBufC* CXDMPluginContainer::GetCurrentSetNameLC()
    {
    TPtrC16 xDMSetName;
    TPtrC16 xDMSetNameClipped;
    xDMSetName.Set(iSettingListArray->MdcaPoint(iMainList->CurrentItemIndex()));
    xDMSetNameClipped.Set(xDMSetName.Mid(1)); // remove '/t' from the begining
    
    HBufC* name = xDMSetNameClipped.AllocLC();
    TPtr ptr(name->Des());
    AknTextUtils::ConvertDigitsTo(ptr, EDigitTypeWestern );
    
    return name;    
    }
    
   
// ---------------------------------------------------------------------------
// CXDMPluginContainer::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
TKeyResponse CXDMPluginContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                            TEventCode aType )
    {
    if (iMainList)
        {
        // if cancel key is pressed and list is not empty, invoke deletion
        if ((aKeyEvent.iCode == EKeyBackspace ) && (aType == EEventKey) )
            {
            if(!IsListEmpty())
                DeleteSetProcedureL();
            CXDMPlugin* iTempView = static_cast<CXDMPlugin*> (iView);
            iTempView->UpdateMSK();
            return EKeyWasConsumed;
            }
        else
            return iMainList->OfferKeyEventL (aKeyEvent, aType);
        }
    return EKeyWasNotConsumed;
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginContainer::HandleListBoxEventL()
// ---------------------------------------------------------------------------
//
void CXDMPluginContainer::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aListBoxEvent)
    {
    // if the Select Key has been pressed
    if ((aListBoxEvent == MEikListBoxObserver::EEventEnterKeyPressed) ||
    (aListBoxEvent == MEikListBoxObserver::EEventItemDoubleClicked))
        {
        EditCurrentItemL();           
        }
    }
  
// ---------------------------------------------------------------------------
// CXDMPluginContainer::SetupListL()
// ---------------------------------------------------------------------------
//
void CXDMPluginContainer::SetupListL()
    {
    CTextListBoxModel* model = iMainList->Model();
    model->SetOwnershipType(ELbmOwnsItemArray);
    
    iSettingListArray = STATIC_CAST(CDesCArray*,model->ItemTextArray());
    LoadSettingsListArrayL();
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginContainer::LoadSettingsListArrayL()
// ---------------------------------------------------------------------------
//
void CXDMPluginContainer::LoadSettingsListArrayL()
    {
    iTitlePane->SetTextL(iTitle);
    _LIT (KStringHeader, "\t%S");
    TBuf <KMaxSettingSetNameLength+2> myString; // maximum name + \t
    RArray<TInt> settingIds;
    CleanupClosePushL(settingIds);
    CDesCArray* xdmSetList = NULL;
    TRAPD(err, xdmSetList = LoadCollectionNamesL(settingIds));
    if(err!=KErrNone)    
        {
        CleanupStack::PopAndDestroy(1); //settingIds
        return;
        }
    CleanupStack::PushL(xdmSetList);
    TInt xdmSetListCount = xdmSetList->Count();
    iSettingListArray->Reset();
    for (TInt i = 0; i< xdmSetListCount; i++)
        {
        myString.Format(KStringHeader, &(xdmSetList->MdcaPoint(i)));
        AknTextUtils::LanguageSpecificNumberConversion(myString);
        iSettingListArray->AppendL (myString);
        }
    iSettingListArray->Sort(ECmpCollated);
    CleanupStack::PopAndDestroy(2); //xdmSetList,settingIds
    iMainList->HandleItemRemovalL();
    iMainList->HandleItemAdditionL();
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginContainer::IsListEmpty()
// ---------------------------------------------------------------------------
//
TBool CXDMPluginContainer::IsListEmpty()
    {
    if (iSettingListArray->Count())
        return EFalse;
    return ETrue;    
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginContainer::IsLastItem()
// ---------------------------------------------------------------------------
//
TBool CXDMPluginContainer::IsLastItem()
    {
    if (iSettingListArray->Count() == 1)
        return ETrue;
    return EFalse;    
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginContainer::DeleteCurrentSetL()
// ---------------------------------------------------------------------------
//
void CXDMPluginContainer::DeleteCurrentSetL()
    {
    TXdmSettingsApi::RemoveCollectionL( GetCurrentSetIdL() );    
    TInt deletedItemIndex = iMainList->CurrentItemIndex();
    iSettingListArray->Delete( deletedItemIndex );
    
    AknListBoxUtils::HandleItemRemovalAndPositionHighlightL(
        iMainList, deletedItemIndex, ETrue );
    iMainList->DrawNow();
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginContainer::GetCurrentSetIdL()
// ---------------------------------------------------------------------------
//
TInt CXDMPluginContainer::GetCurrentSetIdL()
    {
    TInt myId(-1);
    RArray<TInt> settingIds;
    CleanupClosePushL(settingIds);
    CDesCArray* xdmSetList = NULL;

    TRAPD(err, xdmSetList = LoadCollectionNamesL(settingIds));
    if(err!=KErrNone)    
        {
        CleanupStack::PopAndDestroy(1); //settingIds
        return myId;
        }
    CleanupStack::PushL(xdmSetList);

    TInt xdmSetListCount = xdmSetList->Count();
    HBufC* currentSetName = GetCurrentSetNameLC();
    for (TInt i = 0; i< xdmSetListCount; i++)
        {
        if (!((xdmSetList->MdcaPoint(i)).Compare(*currentSetName))) // if equal
            {
            myId = settingIds[i];
            break;
            }
        }
    CleanupStack::PopAndDestroy(3); //xdmSetList,settingIds,currentSetName
    return myId;
    }

// ---------------------------------------------------------------------------
// CXDMPluginContainer::LoadCollectionNamesL()
// ---------------------------------------------------------------------------
//
CDesCArray* CXDMPluginContainer::LoadCollectionNamesL(RArray<TInt>& aSettingIDs)
    {
    CDesCArray* myArray = TXdmSettingsApi::CollectionNamesLC(aSettingIDs);
    CleanupStack::Pop(1); // will be pop if above doesnt leave
    return myArray;
    }

// ---------------------------------------------------------
// CXDMPluginContainer::GetHelpContext
// This function is called when Help application is launched
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CXDMPluginContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KGSXDMPluginUid;
    aContext.iContext = KXDM_HLP_LIST_VIEW;
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginContainer::DeleteSetProcedureL()
// ---------------------------------------------------------------------------
//
void CXDMPluginContainer::DeleteSetProcedureL()
    {
    HBufC* myDisplayMessage = NULL;
    HBufC* currentSetName = GetCurrentSetNameLC();
    TPtr myname(currentSetName->Des());
    AknTextUtils::LanguageSpecificNumberConversion(myname); 
    
    CAknQueryDialog* query = CAknQueryDialog::NewL(CAknQueryDialog::EConfirmationTone);
    if (IsLastItem())
        myDisplayMessage = StringLoader::LoadLC(R_STR_XDM_CONF_DELETE_LAST, *currentSetName );
    else
        myDisplayMessage = StringLoader::LoadLC(R_STR_XDM_CONF_DELETE, *currentSetName );
    
    if ( query->ExecuteLD( R_CONFIRMATION_QUERY, *myDisplayMessage ) )
        {
        DeleteCurrentSetL();
        }
    CleanupStack::PopAndDestroy(2); //currentSetName, myDisplayMessage
    }
  
// ---------------------------------------------------------------------------
// CXDMPluginContainer::HandleResourceChange()
// ---------------------------------------------------------------------------
void CXDMPluginContainer::HandleResourceChange( TInt aType )
    {
    if ( aType == KAknsMessageSkinChange ||
         aType == KEikDynamicLayoutVariantSwitch )
        {
        CXDMPlugin* iTempView = static_cast<CXDMPlugin*> (iView);    
        iTempView->HandleResourceChangeManual(aType);      
        }
    CCoeControl::HandleResourceChange( aType );
    }
  
// ---------------------------------------------------------------------------
// CXDMPluginContainer::HandleResourceChangeManual()
// ---------------------------------------------------------------------------
void CXDMPluginContainer::HandleResourceChangeManual(TInt aType)
    {
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                       mainPaneRect);
    SetRect( mainPaneRect );
    //DrawNow();
    
    iMainList->HandleResourceChange(aType);
    }

// ---------------------------------------------------------------------------
// CXDMPluginContainer::FocusChanged
// Set focus on the selected listbox. For animated skins feature.
// ---------------------------------------------------------------------------
void CXDMPluginContainer::FocusChanged(TDrawNow aDrawNow)
    {
    if(iMainList)
        {
        iMainList->SetFocus( IsFocused(),aDrawNow );
        }
    }

// ---------------------------------------------------------------------------
// CXDMPluginContainer::SetFocusIfExist
// Set focus on the given set name
// ---------------------------------------------------------------------------
void CXDMPluginContainer::SetFocusIfExist(TDes& aSetName)
    {
    _LIT (KStringHeader, "\t%S");
    TBuf <KMaxSettingSetNameLength+2> myString; // maximum name + \t
    myString.Format(KStringHeader, &aSetName);
    AknTextUtils::LanguageSpecificNumberConversion(myString);

    TInt xdmSetListCount = iSettingListArray->Count();
    for (TInt i = 0; i< xdmSetListCount; i++)
        {
        if (!((iSettingListArray->MdcaPoint(i)).Compare(myString))) // if equal
            {
            iMainList->SetCurrentItemIndex(i);
            break;
            }
        }
    }
    
// End of File
