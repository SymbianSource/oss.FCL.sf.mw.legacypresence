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




#ifndef GS_XDMPLUGIN_CONTAINER_H
#define GS_XDMPLUGIN_CONTAINER_H

// INCLUDES
#include    <bldvariant.hrh>
#include    <coeccntx.h>
#include    <eikclb.h>
#include    <eikmobs.h>
#include    <akntitle.h>
#include    <aknview.h>

#include    "XDMPlugin.hrh"


// FORWARD DECLARATION
class CGSListBoxItemTextArray;
class CEikTextListBox;
class CAknSingleStyleListBox;
class CXDMPluginMainList;
class CAknColumnListBox;
class CAknViewAppUi;
class CXDMPlugin;
class CMainView;

// CLASS DECLARATION

/**
*  CXDMPluginContainer main list container class
*/
class CXDMPluginContainer : public CCoeControl, 
                            public MEikListBoxObserver,
                            public MEikMenuObserver
    {
    public: // Constructors and destructor
        
        /**
        * Constructor
        */
        CXDMPluginContainer(CAknView* aView);

        /**
        * Symbian OS constructor.
        *
        * @param aRect Listbox's rect.
        */
        void ConstructL( const TRect& aRect );

        /**
        * Destructor.
        */
        ~CXDMPluginContainer();
    
    public: // From CCoeControl
        
        /**
        * See CCoeControl
        */
        TInt CountComponentControls() const;
        
        /**
        * See CCoeControl
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * See CCoeControl
        */
        TKeyResponse OfferKeyEventL( 
            const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * See CCoeControl
        */
        void SizeChanged();

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

 
    public: // own methods
        
        /**
        * Invokes editing on current item, in response to UI Edit command
        */
        void EditCurrentItemL();
        
        /**
        * See MEikListBoxObserver
        */
        void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
        
        /**
         * From MEikMenuObserver
         */
        void SetEmphasis( CCoeControl* /*aMenuControl*/, TBool /*aEmphasis*/ ){};
      
        
        /**
         * From MEikMenuObserver
         */
        void ProcessCommandL( TInt /*aCommandId*/ ){};
        
        /**
        * Finds whether main list is empty
        * @return ETrue if main list is empty.
        */
        TBool IsListEmpty();
        
        /**
        * Finds whether the current item is the last item in main list
        * @return ETrue if current item is last in main list.
        */
        TBool IsLastItem();
        
        /**
        * Delete the currently focused set. Also deletes it using XDM API
        */
        void DeleteCurrentSetL();
        
        /**
        * Load the main list using XDM API, and perform other necessary tasks
        */
        void LoadSettingsListArrayL();
        
        /**
        * Get the name of currently focused set. Usually called by UI.
        * The pointer to buffer remains on heap, which is deleted by caller.
        * @return HBufC* pointer to currently focused set name
        */
        HBufC* GetCurrentSetNameLC();
        
        /**
        * This function starts the deleting current XDM set. All sub functions
        * are called through this function.
        */
        void DeleteSetProcedureL();

        /**
        * This function does necessary action on layout change, it is called from
        * actual resource change function.
        * @param aType type of resource
        */
        void HandleResourceChangeManual(TInt aType);

        /**
        * Set the focus to the given set name if it exist
        * @param aSetName Set name to set focus
        */
        void SetFocusIfExist(TDes& aSetName);

        
    private: // own methods
    
        /**
        * Perform the initial setup of the main list. Called by Constructor
        */
      void SetupListL();
      
        /**
        * Get the Setting ID of the current set
        * @return TInt setting ID of the current set
        */
      TInt CXDMPluginContainer::GetCurrentSetIdL();
      
        /**
        * Load XDM Collection names with trap, useful when list doesnt have anything
        * @param Reference to setting ids
        * @return CDesCArray collection names
        */
      CDesCArray* LoadCollectionNamesL(RArray<TInt>& aSettingIDs);
      
      
      
    private: // data
      
        // Pointer to the main list, owned
      CAknColumnListBox* iMainList;
  
      // Pointer to the application view, not owned
      CAknView* iView; // not owned
      
      // The XDM set names list array, not owned
      CDesCArray* iSettingListArray;
      
      // Buffer for holding new XDM set name
      TBuf<KMaxSettingSetNameLength>  iSettingNewName;
      
      // Title for menu pane
      TBuf<KMaxSettingSetNameLength>  iTitle;
      
      // Pointer to title pane, not owned
      CAknTitlePane* iTitlePane;
      
      // Menubar, owned
      CEikMenuBar* iEikMenuBar;
    };

#endif //GS_XDMPLUGIN_CONTAINER_H   
