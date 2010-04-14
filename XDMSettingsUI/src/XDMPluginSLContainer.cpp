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
* Description:    XDM GS plugin Settins list container
*
*/




// INCLUDE FILES
#include "XDMExternalInterface.h"

#include    <aknappui.h> 
#include    <akntitle.h>
#include    <eikspane.h>
#include    <barsread.h>
#include    <eiktxlbm.h>
#include    <aknlists.h>
#include    <AknUtils.h>
#include    <eiksbfrm.h>
#include    <XdmSettingsApi.h>
#include    <aknPopup.h>
#include    <StringLoader.h>
#include    <XdmSettingsCollection.h>
#include    <AknQueryDialog.h>
#include    <aknnotewrappers.h>
#include    <aknnotedialog.h>
#include    <escapeutils.h>

#include    "XDMPluginSLContainer.h"
#include    "XDMPluginSettinglist.h"
#include    "SettingsData.h"

#include    <csxhelp/xdm.hlp.hrh>

// LOCAL CONSTANTS AND MACROS
_LIT(KFormatPostfix, "%S(%02d)");
_LIT(KFormatNoPostfix, "%S");
_LIT(KFormatLargePostfix, "%S(%d)");
const TInt KMaxPostfixLength = 5;

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::CXDMPluginSLContainer()
// ---------------------------------------------------------------------------
//
CXDMPluginSLContainer::CXDMPluginSLContainer(CAknView* aView) : iView(aView)
    {
    SetComponentsToInheritVisibility( ETrue );
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::ConstructL(const TRect& aRect)
// ---------------------------------------------------------------------------
//
void CXDMPluginSLContainer::ConstructL( const TRect& aRect)
    {
    #ifdef _DEBUG
    RDebug::Print( _L( "[CXDMPluginSLContainer] ConstructL()" ) );
	#endif

    CEikStatusPane* sp = static_cast<CAknAppUi*> 
        ( CEikonEnv::Static()->EikAppUi() )->StatusPane();
    CAknTitlePane* title = static_cast<CAknTitlePane*> 
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    // Set the proper title of this list
    TResourceReader rReader;
    iCoeEnv->CreateResourceReaderLC( rReader, R_GS_XDM_MAIN_VIEW_TITLE );
    title->SetFromResourceL( rReader );
    CleanupStack::PopAndDestroy(); //rReader

    CreateWindowL(); // Makes the control a window-owning control

    // construct the data object the settings list will use
    iData = CSettingsData::NewL();
    // construct control and set parent
    iSettingList = CXDMPluginSettinglist::NewL(*iData);
    iSettingList->SetContainerWindowL(*this);

    // CreateResourceReaderLC will allocate a buffer to be used by
    // the TResourceReader. This buffer is pushed onto the cleanup
    // stack - not the TResourceReader itself
    CEikonEnv::Static()->CreateResourceReaderLC(rReader, R_SETTINGS);
    iSettingList->ConstructFromResourceL(rReader);

    // Clean up the buffer allocated above, NOT the reader itself.
    // Cannot use expected item overload of PopAndDestroy() as buffer 
    // is inaccessible. 
    CleanupStack::PopAndDestroy();
    iSettingList->ActivateL();

    SetRect( aRect );
    ActivateL();
    
    #ifdef _DEBUG        
    RDebug::Print( _L( "[CXDMPluginSLContainer] Construct done" ) );
	#endif
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::~CXDMPluginSLContainer()
// ---------------------------------------------------------------------------
//
CXDMPluginSLContainer::~CXDMPluginSLContainer()
    {
    if (iData)
        delete iData;
    if(iSettingList)  // if setting list has been created
        {
        delete iSettingList;
        iSettingList = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::SizeChanged()
// ---------------------------------------------------------------------------
//
void CXDMPluginSLContainer::SizeChanged()
    {
    iSettingList->SetRect(Rect());
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::CountComponentControls() const
// ---------------------------------------------------------------------------
//
TInt CXDMPluginSLContainer::CountComponentControls() const
    {
    return 1;
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::ComponentControl( TInt aIndex ) const
// ---------------------------------------------------------------------------
//
CCoeControl* CXDMPluginSLContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iSettingList;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::EditCurrentItemL()
// ---------------------------------------------------------------------------
//
void CXDMPluginSLContainer::EditCurrentItemL()
    {
    iSettingList->EditCurrentItemL();    
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
TKeyResponse CXDMPluginSLContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    if (iSettingList)
        return iSettingList->OfferKeyEventL(aKeyEvent, aType);
    else
        return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------------------------
//
void CXDMPluginSLContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    gc.Clear(aRect);
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::HandleListBoxEventL()
// ---------------------------------------------------------------------------
//
void CXDMPluginSLContainer::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aListBoxEvent)
    {
    // if the Select Key has been pressed
    if ((aListBoxEvent == MEikListBoxObserver::EEventEnterKeyPressed) ||
    (aListBoxEvent == MEikListBoxObserver::EEventItemClicked))
        {
        iSettingList->EditCurrentItemL();
        }
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::PrepareXDMSetForEditingL(TDesC& aXDMSetName)
// ---------------------------------------------------------------------------
//
void CXDMPluginSLContainer::PrepareXDMSetForEditingL(TDesC& aXDMSetName)
    {
    TLex16 myLex;
    iData->Reset();
    
    iData->iSettingName = aXDMSetName;
    AknTextUtils::ConvertDigitsTo(iData->iSettingName, EDigitTypeWestern );    
    iData->iSettingId = GetSettingIdL(iData->iSettingName);
    
    CXdmSettingsCollection* xDMSet = NULL;
    
    // read the source setting
    xDMSet = TXdmSettingsApi::SettingsCollectionL(iData->iSettingId);
    CleanupStack::PushL(xDMSet);
    
    iData->iSettingNameDisp = aXDMSetName;
    iData->iServerAddress = xDMSet->Property(EXdmPropUri);
    
    // AccessPoint handling
    iData->iAccessPointDes = xDMSet->Property(EXdmPropToNapId);
    myLex.Assign(iData->iAccessPointDes);
    myLex.Val(iData->iAccessPoint);
    
    iSettingList->GetAccessPointNameL(iData->iAccessPoint, iData->iAccessPointName);
    if ( (iData->iAccessPointName) == KNullDesC) // if name is not found
        {
        iData->iAccessPoint = -1;
        }
    
    TBuf<KMaxUserIDLength> username = xDMSet->Property( EXdmPropAuthName );
    
    // to show special chars in UI correctly
    _LIT( KProcent, "%" );
    if ( username.Find( KProcent ) != KErrNotFound )
        {
        // convert to 8 bit
        HBufC8* tmp = HBufC8::NewLC( username.Length() );
        tmp->Des().Copy( username );
        
        // Decode encoded username
        HBufC8* decodedUsername = EscapeUtils::EscapeDecodeL( *tmp );
        CleanupStack::PopAndDestroy( tmp );
        CleanupStack::PushL( decodedUsername );
        
        // convert to unicode
        HBufC* userName16 =
             EscapeUtils::ConvertToUnicodeFromUtf8L( decodedUsername->Des() );
        CleanupStack::PopAndDestroy( decodedUsername );        
        username = userName16->Des();
        delete userName16;
        }
    
    iData->iUserID = username;
    iData->iPassword = xDMSet->Property(EXdmPropAuthSecret);
    
    CleanupStack::PopAndDestroy(1); //xDMSet
    iSettingList->LoadSettingsL();
    SetTitlePaneTextL(iData->iSettingName);
    iSettingList->ResetItemIndex();
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::PrepareXDMSetNewDefaultL()
// ---------------------------------------------------------------------------
//
void CXDMPluginSLContainer::PrepareXDMSetNewDefaultL()
    {
    iData->Reset();
    
    HBufC* defName = StringLoader::LoadL( R_STR_XDM_NEW_SET );
    CleanupStack::PushL( defName );
    
    TBool changed;

    HBufC* newName = DoMakeValidNameL(defName, changed);
        
    if ( newName )
        // name converted to unique
        {
        (iData->iSettingName).Copy(newName->Des());
        delete newName;
        }
    else
        {
        (iData->iSettingName).Copy(defName->Des());
        }
    CleanupStack::PopAndDestroy( defName );
    
    iData->iSettingNameDisp = iData->iSettingName;
    AknTextUtils::LanguageSpecificNumberConversion(iData->iSettingNameDisp);
    
    iSettingList->LoadSettingsL();
    iSettingList->ResetItemIndex();
    iSettingList->DrawNow();
    SetTitlePaneTextL(iData->iSettingName);
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::PrepareNewXDMSetFromExistingL(TDesC& aXDMSetName)
// ---------------------------------------------------------------------------
//
void CXDMPluginSLContainer::PrepareNewXDMSetFromExistingL(TDesC& aXDMSetName)
    {
    TLex16 myLex;
    
    // iData is already resetted in this case and iSettingId has id of 
    // existing list from which data to be copied
    // we need to find the new name, from given name
    HBufC* originalName = aXDMSetName.AllocL();
    CleanupStack::PushL( originalName );
    
    // aXDMSetName is in disp format, need to convert to western format
    TPtr corrFormat(originalName->Des());
    AknTextUtils::ConvertDigitsTo(corrFormat, EDigitTypeWestern );
    
    TBool changed;

    HBufC* newName = DoMakeValidNameL(originalName, changed);
        
    if ( newName )
        // name converted to unique
        {
        (iData->iSettingName).Copy(newName->Des());
        delete newName;
        }
    else
        {
        (iData->iSettingName).Copy(originalName->Des());
        }

    CleanupStack::PopAndDestroy( originalName );
    
    
    CXdmSettingsCollection* xDMSet = NULL;
    
    // Displayable name
    iData->iSettingNameDisp = iData->iSettingName;
    AknTextUtils::LanguageSpecificNumberConversion(iData->iSettingNameDisp);
    
    // read the source setting
    xDMSet = TXdmSettingsApi::SettingsCollectionL(iData->iSettingId);
    CleanupStack::PushL(xDMSet);
    
    iData->iServerAddress = xDMSet->Property(EXdmPropUri);
    
    // AccessPoint handling
    iData->iAccessPointDes = xDMSet->Property(EXdmPropToNapId);
    myLex.Assign(iData->iAccessPointDes);
    myLex.Val(iData->iAccessPoint);
    
    iSettingList->GetAccessPointNameL(iData->iAccessPoint, iData->iAccessPointName);
    if ( (iData->iAccessPointName) == KNullDesC) // if name is not found
        {
        iData->iAccessPoint = -1;
        }

    iData->iUserID = xDMSet->Property(EXdmPropAuthName);
    iData->iPassword = xDMSet->Property(EXdmPropAuthSecret);
    
    // also overwrite the setting id now since we dont need it anymore
    iData->iSettingId = -1;
    CleanupStack::PopAndDestroy(1); //xDMSet
    iSettingList->LoadSettingsL();
    SetTitlePaneTextL(iData->iSettingNameDisp);
    iSettingList->ResetItemIndex();
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::DisplayNewXDMSetOptions()
// ---------------------------------------------------------------------------
//
TBool CXDMPluginSLContainer::DisplayNewXDMSetOptionsL()
    {
    // First phase construction of menu list
    CAknSinglePopupMenuStyleListBox* newXDMsetMenuList =
    new (ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(newXDMsetMenuList);

    // Create a popuplist to show the menu list in
    CAknPopupList* popupList = CAknPopupList::NewL(
                  newXDMsetMenuList, 
                  R_AVKON_SOFTKEYS_OK_CANCEL);

    CleanupStack::PushL(popupList);

    // Second phase construction of menulist
    newXDMsetMenuList->ConstructL(popupList,EAknListBoxMenuList);
    // Set up scroll bars
    newXDMsetMenuList->CreateScrollBarFrameL(ETrue);
    newXDMsetMenuList->ScrollBarFrame()->SetScrollBarVisibilityL(
    CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    // Set up menu items
    CTextListBoxModel* model = newXDMsetMenuList->Model();  // not taking ownership
    model->SetOwnershipType (ELbmOwnsItemArray); // list'll delete with listbox
    RArray<TInt> settingIDs;
    CleanupClosePushL(settingIDs);
    CDesCArray* newXDMsetMenuListArray = NULL;

    TRAPD(err, newXDMsetMenuListArray = LoadCollectionNamesL(settingIDs, ETrue));
    if(err!=KErrNone)    
        {
        CleanupStack::PopAndDestroy(3); //newXDMsetMenuList, popupList, settingIDs
        return EFalse;
        }
    CleanupStack::PushL(newXDMsetMenuListArray);

    newXDMsetMenuListArray->Sort(ECmpCollated);
    model->SetItemTextArray(newXDMsetMenuListArray);
    CleanupStack::Pop(1); // newXDMsetMenuListArray owned by model
    CleanupStack::PopAndDestroy(1); //settingIDs

    // Set title
    HBufC* title;
    title = StringLoader::LoadLC(R_STR_XDM_USE_EXISTING);
    popupList->SetTitleL(*title);
    CleanupStack::PopAndDestroy(1); //title

    // Show the menu in the popup list
    TInt popupOk = popupList->ExecuteLD();

    CleanupStack::Pop(popupList);

    // if the user selected some set
    if (popupOk)
        {
        iData->Reset();
        TPtrC setName = newXDMsetMenuListArray->MdcaPoint
                                    (newXDMsetMenuList->CurrentItemIndex());
        
        iData->iSettingId = GetSettingIdL(setName);
        (iData->iSettingNameDisp).Copy(setName);
        (iData->iSettingName).Copy(setName);
        AknTextUtils::ConvertDigitsTo(iData->iSettingName, EDigitTypeWestern );
        CleanupStack::PopAndDestroy(newXDMsetMenuList);
        return ETrue;
        }
    // otherwise return false
    CleanupStack::PopAndDestroy(1); //newXDMsetMenuList
    return EFalse;      
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::SetTitlePaneTextL( const TDesC& aTitleText ) const
// ---------------------------------------------------------------------------
//
void CXDMPluginSLContainer::SetTitlePaneTextL( const TDesC& aTitleText ) const
    {
    CAknTitlePane* title = static_cast< CAknTitlePane* >
        ( CEikonEnv::Static()->AppUiFactory()->StatusPane()->ControlL(
                                        TUid::Uid( EEikStatusPaneUidTitle ) ) );
    if ( !title )
        {
        User::Leave( KErrNotSupported );
        }
    title->SetTextL( aTitleText );
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::AreCompulsoryItemsFilled()
// ---------------------------------------------------------------------------
//
TBool CXDMPluginSLContainer::AreCompulsoryItemsFilled()
    {
    if ( (iData->iAccessPoint > -1) && ((iData->iServerAddress)!=KNullDesC) && 
        ((iData->iUserID)!=KNullDesC) )
        {
        return ETrue;
        }
    return EFalse;   
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::DisplayDeleteOrDontSaveDialog()
// ---------------------------------------------------------------------------
//
TBool CXDMPluginSLContainer::DisplayDeleteOrDontSaveDialogL()
    {
    TBuf<KMaxMessageToDisplayLength> myDisplayMessage;
    CAknQueryDialog* query = CAknQueryDialog::NewL
                                        (CAknQueryDialog::EConfirmationTone);
    StringLoader::Load ( myDisplayMessage, R_STR_XDM_CONF_COMPULSORY );
    return (query->ExecuteLD( R_CONFIRMATION_QUERY, myDisplayMessage ));
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::DeleteXDMSetIfExistL()
// ---------------------------------------------------------------------------
//
void CXDMPluginSLContainer::DeleteXDMSetIfExistL()
    {
    TRAPD(err, TXdmSettingsApi::RemoveCollectionL(iData->iSettingId));
    if ( (err!=KErrNone) && (err!=KErrNotFound) )
        {
        User::LeaveIfError(err);        
        }
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::IsExitProcessingOKL()
// ---------------------------------------------------------------------------
//
TBool CXDMPluginSLContainer::IsExitProcessingOKL()
    {
    if (AreCompulsoryItemsFilled())
        {
        return SaveOrCreateAndSaveXDMSetL(ETrue);
        }
    else
        {
        if (DisplayDeleteOrDontSaveDialogL())
            {
            DeleteXDMSetIfExistL();
            }
        else
            {
             return EFalse;
            }
        }
    return ETrue;
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::SaveSettingsIfPossibleL()
// ---------------------------------------------------------------------------
//
void CXDMPluginSLContainer::SaveSettingsIfPossibleL()
    {
    if (AreCompulsoryItemsFilled())
        SaveOrCreateAndSaveXDMSetL(EFalse);    
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::SaveOrCreateAndSaveXDMSetL()
// ---------------------------------------------------------------------------
//
TBool CXDMPluginSLContainer::SaveOrCreateAndSaveXDMSetL(TBool aShowDialog)
    {
    TBool ret = ETrue;
    CXdmSettingsCollection* myXDMSet(NULL);
    TInt err(KErrNotFound); // Initialize such that id not found in settings API
    
    // take the saveable name
    iData->iSettingName = iData->iSettingNameDisp;
    AknTextUtils::ConvertDigitsTo(iData->iSettingName, EDigitTypeWestern ); 
    
    // check whether this is a new set or an existing set was edited
    if ((iData->iSettingId) > NULL) // is there is valid id
        {
        TRAP(err, myXDMSet = TXdmSettingsApi::SettingsCollectionL
                                                (iData->iSettingId));
        }
    if(myXDMSet)
        {
         delete myXDMSet; // we dont need it
        }
        
    // check if the setting name is already exist
    // ask user to change name
    if (IsXDMSetNameExistL(iData->iSettingName, iData->iSettingId))
        {
        if (!aShowDialog) // if we cant display dialog than return.
            return EFalse;
        HBufC* myDisplayMessage = NULL;
        CAknInformationNote* note = new (ELeave) CAknInformationNote( ETrue );
        note->SetTimeout( CAknNoteDialog::ELongTimeout  );
        myDisplayMessage = StringLoader::LoadLC(R_STR_XDM_NAME_ALREADY_EXIST, iData->iSettingNameDisp);
        note->ExecuteLD( *myDisplayMessage );
        CleanupStack::PopAndDestroy(1); //myDisplayMessage
        iSettingList->EditItemL(ESettingNameIndex,EFalse);
        return EFalse;                
        }
        
    if(err == KErrNone) // if set exists update it
        {
        TXdmSettingsApi::UpdatePropertyL(iData->iSettingId, 
                                        iData->iSettingName, EXdmPropName);
        TXdmSettingsApi::UpdatePropertyL(iData->iSettingId, 
                                        iData->iAccessPointDes, EXdmPropToNapId);
        TXdmSettingsApi::UpdatePropertyL(iData->iSettingId, 
                                        iData->iServerAddress, EXdmPropUri);
        TXdmSettingsApi::UpdatePropertyL(iData->iSettingId, 
                                        iData->iUserID, EXdmPropAuthName);
        TXdmSettingsApi::UpdatePropertyL(iData->iSettingId, 
                                        iData->iPassword, EXdmPropAuthSecret);
        ret = ETrue;    
        }

    else if (err == KErrNotFound) // if set doenst exists create and save it
        {
        myXDMSet = new (ELeave) CXdmSettingsCollection();
        CleanupStack::PushL(myXDMSet);
        myXDMSet->AppendL(iData->iSettingName, EXdmPropName);
        myXDMSet->AppendL(iData->iServerAddress, EXdmPropUri);
        myXDMSet->AppendL(iData->iAccessPointDes, EXdmPropToNapId);
        myXDMSet->AppendL(iData->iUserID, EXdmPropAuthName);
        myXDMSet->AppendL(iData->iPassword, EXdmPropAuthSecret);
        TRAPD(err,TXdmSettingsApi::CreateCollectionL(*myXDMSet));
        err = err; //just to satisfy compiler
        CleanupStack::PopAndDestroy(1); // myXDMSet
        ret = ETrue;        
        }
        
    else
        {
        User::LeaveIfError(err);            
        }
    
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::GetSettingIdL(TDesC& aXDMSetName)
// ---------------------------------------------------------------------------
//
TInt CXDMPluginSLContainer::GetSettingIdL(TDesC& aXDMSetName)
    {
    TInt myId(-1);
    RArray<TInt> settingIds;
    CleanupClosePushL(settingIds);
    CDesCArray* xdmSetList =NULL;

    TBuf<KMaxSettingSetNameLength> setName = aXDMSetName;
    AknTextUtils::ConvertDigitsTo(setName, EDigitTypeWestern );  
    
    TRAPD(err, xdmSetList = LoadCollectionNamesL(settingIds, EFalse));
    if(err!=KErrNone)    
        {
        CleanupStack::PopAndDestroy(1); //settingIds
        return myId;
        }
    CleanupStack::PushL(xdmSetList);

    TInt xdmSetListCount = xdmSetList->Count();
    for (TInt i = 0; i< xdmSetListCount; i++)
        {
        if (!((xdmSetList->MdcaPoint(i)).Compare(setName))) // if equal
            {
            myId = settingIds[i];
            break;
            }
        }
    CleanupStack::PopAndDestroy(2); //xdmSetList,settingIds
    return myId;
    }
        
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::IsXDMSetNameExistL(TDesC& aXDMSetName, TInt32& aSettingId)
// ---------------------------------------------------------------------------
//
TBool CXDMPluginSLContainer::IsXDMSetNameExistL(TDesC& aXDMSetName, TInt32& aSettingId)
    {
    TBool ret(EFalse);
    RArray<TInt> settingIds;
    CleanupClosePushL(settingIds);
    CDesCArray* xdmSetList = NULL;

    TBuf<KMaxSettingSetNameLength> setName = aXDMSetName;
    AknTextUtils::ConvertDigitsTo(setName, EDigitTypeWestern );  

    TRAPD(err, xdmSetList = LoadCollectionNamesL(settingIds, EFalse));
    if(err!=KErrNone)    
        {
        CleanupStack::PopAndDestroy(1); //settingIds
        return EFalse;
        }
    CleanupStack::PushL(xdmSetList);

    TInt xdmSetListCount = xdmSetList->Count();
    for (TInt i = 0; i< xdmSetListCount; i++)
        {
        if (!((xdmSetList->MdcaPoint(i)).Compare(setName))) // if equal
            {
            if (settingIds[i] != aSettingId) // if the set is not itself
                {
                 ret = ETrue;
                }
            }
        }
    CleanupStack::PopAndDestroy(2); //xdmSetList,settingIds
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::LoadCollectionNamesL
// ---------------------------------------------------------------------------
//
CDesCArray* CXDMPluginSLContainer::LoadCollectionNamesL(RArray<TInt>& aSettingIDs
                                                                    , TBool aDisp)
    {
    CDesCArray* myArray = TXdmSettingsApi::CollectionNamesLC(aSettingIDs);
    TBuf<KMaxSettingSetNameLength> name;
    if(aDisp) // convert all names to disp names
        {
        TInt count = myArray->Count();
        CDesCArray* dispArray = new CDesCArrayFlat(count+1);
        CleanupStack::PushL(dispArray);
        for(TInt i=0;i<count;i++)
            {
            name.Copy(myArray->MdcaPoint(i));
            AknTextUtils::LanguageSpecificNumberConversion(name);
            TPtrC16 ptr = name;
            dispArray->AppendL(ptr);                
            }
        CleanupStack::Pop(2);
        return dispArray;        
        }
    CleanupStack::Pop(1); // myArray
    return myArray;
    }

// ---------------------------------------------------------
// CXDMPluginSLContainer::GetHelpContext
// This function is called when Help application is launched
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CXDMPluginSLContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KGSXDMPluginUid;
    aContext.iContext = KXDM_HLP_EDIT_SET;
    }   

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::HandleResourceChange()
// ---------------------------------------------------------------------------
void CXDMPluginSLContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KAknsMessageSkinChange ||
         aType == KEikDynamicLayoutVariantSwitch )
        {
        CXDMPlugin* iTempView = static_cast<CXDMPlugin*> (iView);    
        iTempView->HandleResourceChangeManual(aType);      
        }
    }


// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::HandleResourceChangeManual()
// ---------------------------------------------------------------------------    
void CXDMPluginSLContainer::HandleResourceChangeManual(TInt aType)
    {
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                       mainPaneRect);
    SetRect( mainPaneRect );
	DrawDeferred();
	
	iSettingList->HandleResourceChange(aType);
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::FocusChanged
// Set focus on the selected listbox. For animated skins feature.
// ---------------------------------------------------------------------------
void CXDMPluginSLContainer::FocusChanged(TDrawNow aDrawNow)
    {
    if(iSettingList)
        {
        iSettingList->SetFocus( IsFocused(), aDrawNow );
        }
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::GetCurrentSetName()
// See header for details.
// ---------------------------------------------------------------------------    
// 
TDes& CXDMPluginSLContainer::GetCurrentSetName()
    {
    return iData->iSettingName;    
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::DoMakeValidNameL()
// See header for details.
// ---------------------------------------------------------------------------
// 
HBufC* CXDMPluginSLContainer::DoMakeValidNameL(HBufC* aName, TBool& aChanged)
    {
    aChanged = EFalse;
    HBufC* temp = HBufC::NewLC( KMaxSettingSetNameLength );
    HBufC* temp2 = HBufC::NewLC( KMaxSettingSetNameLength );
    
    HBufC* corrname = EnsureMaxLengthLC( aName, aChanged);
    *temp = *corrname;
    TInt postfix( 0 );
    TInt pf( 0 );
    TInt i( 0 );
    
    
    // Load the names
    RArray<TInt> settingIDs;
    CleanupClosePushL(settingIDs);
    CDesCArray* xdmSetList = NULL;

    TRAPD(err, xdmSetList = LoadCollectionNamesL(settingIDs, EFalse));
    if(err!=KErrNone)    
        {
        CleanupStack::PopAndDestroy(4); //settingIds
        return corrname; 
        }
    CleanupStack::PushL(xdmSetList);
    
    // do the actual checking of name
    TInt found(0);
    TInt pos(0);
    
    TPtrC prefix = GetPrefix( *corrname );
    
    postfix = GetPostfix( *temp, prefix );
    postfix = -1;
    do
        {       
        found = xdmSetList->Find( *temp, pos );
        if ( found == KErrNone )
            {
            aChanged = ETrue;
            postfix++;
            // check the length of postfix, check text length accordingly
            pf = postfix;
            for (i=1; i<10; i++)
                {
                pf /= 10;
                if ( !pf )
                    break;
                }
            TPtr sgdptr( temp->Des() );
            TPtr sgdptr2( temp2->Des() );
            if ( postfix )
                {
                if ( postfix < 10 )
        {
                    sgdptr2 = prefix.Left( KMaxSettingSetNameLength - i - 3 );
                    }
                else
            {
                    sgdptr2 = prefix.Left( KMaxSettingSetNameLength - i - 2 );
                    }
                }
            else
                {
                sgdptr2 = prefix.Left( KMaxSettingSetNameLength );
                }
            if ( postfix )
                {
                if ( postfix > 9 )
                    {
                    sgdptr.Format( KFormatLargePostfix, &sgdptr2,
                                       postfix );
                    }
                else
                    {
                    sgdptr.Format( KFormatPostfix, &sgdptr2,
                                       postfix );
                }
            }
            else
                {
                sgdptr.Format( KFormatNoPostfix, &sgdptr2 );
        }
            }
        } while (found == KErrNone);
        
    CleanupStack::PopAndDestroy(xdmSetList);
    CleanupStack::PopAndDestroy(1); //settingIds
    CleanupStack::PopAndDestroy( corrname );
    CleanupStack::PopAndDestroy( temp2 );

    if ( aChanged )
        {
        CleanupStack::Pop( temp );
        }
    else
        {
        CleanupStack::PopAndDestroy( temp );
        temp = NULL;
        }
    
    return temp;
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::EnsureMaxLengthLC()
// See header for details.
// ---------------------------------------------------------------------------
HBufC* CXDMPluginSLContainer::EnsureMaxLengthLC( const TDesC* aName, TBool& aChanged)
    {
    TInt length = aName->Length();
    
    aChanged = EFalse;
    
    if ( !length )
        {
        // Name is required.
        User::Leave(KErrArgument);
        }

    HBufC* corrname;
    if ( KMaxSettingSetNameLength < length )
        { // name too long, truncate.
        corrname = aName->Left( KMaxSettingSetNameLength ).AllocLC();
        aChanged = ETrue;
        }
    else
        {
        corrname = aName->AllocLC();
        corrname->Des().Trim();
        if ( corrname->Length() == 0 )
            {
            User::Leave(KErrArgument);
            }
        // comes here only if name is valid
        if ( corrname->Length() != aName->Length() )
    {
            aChanged = ETrue;
            }
        }

    return corrname;
    }
    
// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::GetPrefix()
// See header for details.
// ---------------------------------------------------------------------------
TPtrC CXDMPluginSLContainer::GetPrefix( const TDesC& aName )
    {
    TPtrC prefix = aName;
    TInt lastBrace = aName.LocateReverse('(');
    if ( lastBrace != KErrNotFound )
        {
        // aName looks like "<prefix><brace><something>".
        // See if <something> is an integer number.
        TPtrC num = aName.Right( aName.Length() - lastBrace - 1 );
        TInt val;
        TLex lex( num );
        if ( lex.Val( val ) == KErrNone )
            {
            // Yes, the trailer is an integer.
            prefix.Set( aName.Left( lastBrace ) );
            }
        }
    return prefix;
    }

// ---------------------------------------------------------------------------
// CXDMPluginSLContainer::GetPostfix()
// See header for details.
// ---------------------------------------------------------------------------
TInt CXDMPluginSLContainer::GetPostfix( const TDesC& aName, const TDesC& aPrefix )
    {
    TInt postfix( KErrNotFound );
    TInt nameLength = aName.Length();
    TInt prefixLength = aPrefix.Length();
    if ( nameLength >= prefixLength && aName.FindF( aPrefix ) == 0 )
        {
        // aName is longer or equal length, and
        // aPrefix can be found in the beginning of aName.
        if ( nameLength == prefixLength )
            {
            // They have the same length; they equal.
            postfix = 0;
            }
        else
            {
            if ( aName[ prefixLength ] == '(' )
                {
                // (Now we know that aName is longer than aPrefix.)
                // aName looks like "aPrefix<brace><something>".
                // See if <something> is an integer number.
                TPtrC num = aName.Right( nameLength - prefixLength - 1 );
                TBuf< KMaxPostfixLength > pf;
                if (num.Length() <= pf.Length())
                    pf = num;
                TInt val;
                TLex lex( pf );
                if ( lex.Val( val ) == KErrNone )
                    {
                    // Yes, the trailer is an integer.
                    if ( val > 0 )
                        {
                        postfix = val;
                        }
                    else
                        {
                        // signal that it is invalid...
                        postfix = -1;
                        }
                    }
                }
            }
        }
    return postfix;
    }
// end of file
