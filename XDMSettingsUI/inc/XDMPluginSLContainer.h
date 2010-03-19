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
* Description:    XDM GS plugin Settings list container.
*
*/




#ifndef GS_XDMPLUGIN_SL_CONTAINER_H
#define GS_XDMPLUGIN_SL_CONTAINER_H

// INCLUDES
#include    <bldvariant.hrh>
#include    <coeccntx.h>
#include    <eikclb.h>

#include    "XDMPlugin.hrh"

// FORWARD DECLARATION
class CGSListBoxItemTextArray;
class CEikTextListBox;
class CSettingsData;
class CXDMPluginSettinglist;
class CAknColumnListBox;
class CAknViewAppUi;

/**
*  CXDMPluginSLContainer Settings list container class
*/
class CXDMPluginSLContainer : public CCoeControl, MEikListBoxObserver
    {
    public:
    
        /**
        * C++ Constructor
        */
        CXDMPluginSLContainer(CAknView* aView);
        
        /**
        * Symbian OS constructor.
        * @param aRect Listbox's rect.
        */
        void ConstructL( const TRect& aRect);

        /**
        * Destructor.
        */
        ~CXDMPluginSLContainer();
    
    public: // From CCoeControl
    
        /**
        * See CCoeControl.
        */
        TInt CountComponentControls() const;
        
        /**
        * See CCoeControl.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
        /**
        * See CCoeControl.
        */
        TKeyResponse OfferKeyEventL( 
            const TKeyEvent& aKeyEvent, TEventCode aType );
            
        /**
        * See CCoeControl.
        */
        void SizeChanged();
        
        /**
        * See CCoeControl.
        */
        void Draw(const TRect& aRect) const;
        
        /**
        * See CCoeControl
        */
        void HandleResourceChange( TInt aType );

        /**
         * Gets help context
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /**
         * See CCoeControl
         */
        void FocusChanged(TDrawNow aDrawNow);
        
    public:
    
        /**
        * Edit currently focused item on settings list. called from UI
        */
        void EditCurrentItemL();
        
        /**
        * See MEikListBoxObserver
        */
        void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
        
        /**
        * Prepare the XDM set with given name for editing
        * @param aXDMSetName reference to the XDM set name to be edited
        */
        void PrepareXDMSetForEditingL(TDesC& aXDMSetName);
        
        /**
        * Prepare the a new XDM set with default values
        * @param none
        */
        void PrepareXDMSetNewDefaultL();
        
        /**
        * Prepare the a new XDM set with values from an existing set
        * @param aXDMSetName reference to the new XDM set name
        */
        void PrepareNewXDMSetFromExistingL(TDesC& aXDMSetName);
        
        /**
        * Display the dialog to the user to select the creation of new set
        * from existing sets. Saves the selected name to iData
        * @return ETrue if user has choosen a set, return EFalse if user cancels
        */
        TBool DisplayNewXDMSetOptionsL();
        
        /**
        * Called by the UI when back button is pressed, to perform needed steps
        * @return ETrue if settings list is allowed to close
        */
        TBool IsExitProcessingOKL();
        
        /**
        * Handle manual resource change with its type
        * @param aType, type of resource
        */
        void HandleResourceChangeManual(TInt aType);

        /**
        * Save settings if possible, without any user interaction.
        * this is useful for force exit, e.g. when mmc taken out
        */
        void SaveSettingsIfPossibleL();
        
        /**
        * Return the name of current set in iData
        * @return TDes reference to current set
        */
        TDes& GetCurrentSetName();
        
        
    private: // Most of these methods perform operations using iData
    
        /**
        * Sets the title pane text with given discriptor
        * @param aTitleText text to be shown on title pane
        */
        void SetTitlePaneTextL( const TDesC& aTitleText ) const;
        
        /**
        * Finds out whether compulsory items are filled
        * @return ETrue if compulsory items are filled
        */
        TBool AreCompulsoryItemsFilled();
        
        /**
        * Display a query dialog to user that compulsory settings are not
        * filled, and if user wants to delete the settings
        * @return ETrue if user wants to delete the settings
        */
        TBool DisplayDeleteOrDontSaveDialogL();
        
        /**
        * Update an XDM set if it already exist or create it if it doesnt exist
        * @param aShowDialog whether dialog will be displayed or not
        * @return ETrue if save procedure goes ok
        */
        TBool SaveOrCreateAndSaveXDMSetL(TBool aShowDialog);
        
        /**
        * Deletes the XDM set if it exist in the XDM settings API
        */
        void DeleteXDMSetIfExistL();
        
        /**
        * Gets the setting id of the given set name
        * @param aXDMSetName reference to XDM set name
        * @return settingid of the given xdm set
        */
        TInt GetSettingIdL(TDesC& aXDMSetName);
        
        /**
        * If the given XDM set name with given ID is exist or not
        * @param aXDMSetName reference to XDM set name
        * aSettingId settings id of the XDM
        * @return ETrue of XDM set exist with given name
        */
        TBool IsXDMSetNameExistL(TDesC& aXDMSetName, TInt32& aSettingId);
        
        /**
        * Load XDM Collection names with trap, useful when list doesnt have anything
        * @param Reference to setting ids
        * @return CDesCArray collection names
        */
        CDesCArray* LoadCollectionNamesL(RArray<TInt>& aSettingIDs, TBool aDisp);
        
        /**
        * Check the given name and returns the possible altered name. NULL 
        * pointer is returned no change is needed in aName, so aName can be
        * used if NULL pointer is returned. If some valid pointer is returned
        * then that pointer is owned by client, and needed to be deleted after
        * usage. 
        * @param aName, the name from which to start suggestion.
        * @param aChanged, returns ETrue if an alternate name is returned.
        * @return HBufC* pointer to the new name, can be NULL if not needed.
        */
        HBufC* DoMakeValidNameL(HBufC* aName, TBool& aChanged);
        
        /**
        * Ensures that the passed name is valid, length > 0 & length < max.
        * If length == 0, leaves with KErrInvalidName
        * If name is only whitespace, leaves with KErrInvalidName
        * If name is longer than max, name is truncated
        * It copies the name, pushes it onto the CleanupStack and passes
        * ownership.

        * @param aName The name
        * @param aChanged A reference to a boolean to hold whether the name 
        *   had been changed or not.
        * @return The valid-length name
        */
        HBufC* EnsureMaxLengthLC( const TDesC* aName, TBool& aChanged);
        
        /**
        * Given aName in the format <prefix> or
        * <prefix><brace><integer><brace>, return a
        * pointer to the leading part. That is, if there is
        * trailing <space><integer>, then that is excluded;
        * if there is no trailing part, then the original
        * decriptor is returned.
        * Examples:
        *   - "Foo" returns "Foo";
        *   - "Foo 12" returns "Foo 12";
        *   - "Foo(12)" returns "Foo";
        *   - "Foo 12 (34)" returns "Foo 12 ";
        *   - "Foo bar" returns "Foo bar";
        *   - "Foo " returns "Foo ".
        * @param aName  The name to get the prefix from
        * @return The prefix
        */        
        TPtrC GetPrefix( const TDesC& aName );
        
        
        /**
        * If aName is constructed from aPrefix with a postfix, get the numeric
        * value of the postfix, e.g:
        *   - GetPostfix( "Foo (3)", "Foo" ) == 3
        *   - GetPostfix( "Foo 23 (45)", "Foo 23" ) == 45
        * If aName is the same as aPrefix, return 0, e.g.:
        *   - GetPostfix( "Foo", "Foo" ) == 0
        * If aName is not constructed from aPrefix, return -1, e.g.:
        *   - GetPostfix( "Foobar", "Foo" ) == -1
        *   - GetPostfix( "Fo 23 45", "Foo" ) == -1
        * @param aName  The name to get the postfix from
        * @param aPrefix The prefix
        * @return The postfix
        */
        TInt GetPostfix( const TDesC& aName, const TDesC& aPrefix );

        
   private: // data
        
        // CXDMPluginSettinglist owned
      CXDMPluginSettinglist* iSettingList;        
  
      // Pointer to settings data owned
      CSettingsData* iData;

      // Pointer to the application view, not owned
      CAknView* iView; // not owned

    };

#endif //GS_XDMPLUGIN_SL_CONTAINER_H   
