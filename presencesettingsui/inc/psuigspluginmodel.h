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
* Description:    Presence settings UI model header.
*
*/




#ifndef PSUIGSPLUGINMODEL_H
#define PSUIGSPLUGINMODEL_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <sipprofileregistryobserver.h>
#include <pressettingsapi.h>
#include "psuigsplugin.hrh"

// FORWARD DECLARATIONS

// CLASS DECLARATION
class RPresenceSettingsArray;
class CSIPManagedProfileRegistry;
class CSIPManagedProfile;

// Constants
_LIT( KPSUIGSPluginPanicCategory, "PSUIGSPlugin" );

/**
* Presence settings UI model
*
* @lib PSUIGSPlugin.lib
* @since Series60_3.2
*/
class CPSUIGSPluginModel : 
    public CBase, 
    public MDesCArray,
    public MSIPProfileRegistryObserver
    {

    public: // Constructors and destructor
            
        /**
        * Symbian OS two-phased constructor
        */    
        static CPSUIGSPluginModel* NewL();

        /**
        * Destructor.
        */
        ~CPSUIGSPluginModel();

    public: // New
    
        //
        //
        // Presence settings related
    
        /**
        * Retrieves current presence settings from system
        */
        void RetrieveSettingsL();

        /**
        * Stores current presence settings to system
        */
        void SaveSettingsL();
        
        /**
        * Checks are the compulsory fields filled.
        *
        * @return boolean about filled fields
        */        
        TBool AllCompulsoryFieldsFilled();
        
        /**
        * Prepares iSettingSet for editing
        *
        * @param aIndex of presence settings to be edited
        */
        void EditSettingsL( TInt aIndex );
        
        /**
        * Refreshes the presence settings related arrays
        */
        void RefreshPSArraysL();

        /**
        * Prepares the iSettingSet and iSettingId
        * with default values
        */
        void CreateDefaultSettingsL();

        /**
        * Prepares the iSettingSet and iSettingId
        * with values from indexed presence setting
        *
        * @param aIndex is the presence settings to be duplicated
        */
        void DuplicateExistingSettingsL( TInt aIndex );

        /**
        * Creates new name for to be created settings. If aOriginal
        * provided, then will use that as a base. If not, then will
        * create the name based on default naming. Adds numbering to
        * end of name if needed.
        * Note : aOriginal will be stored to cleanupstack if provided
        *
        * @param aOriginal is optional base name
        *
        */
        HBufC* CreateSettingNameLC( HBufC* aOriginal = NULL );

        /**
        * Deletes indexed settings through presence settings API.
        *
        * @param aToBeDeleted to be deleted settings
        * @param aIsIndex tells is aToBeDeleted index or id
        */
        void DeleteSettingsL( TInt aToBeDeleted, TBool aIsIndex );
        
        /**
        * Returns name of the indexed settings.
        *
        * @param aIndex of the wanted name
        * @return HBufC* containing the name
        */
        HBufC* SettingNameLC( TInt aIndex );

        /**
        * Checks if there already is settings with the same name.
        */
        TBool IsItDuplicatePSName();

        /**
        * Returns iPSNameArray, which contains names of the
        * different setting sets. Ownership of array stays in
        * CPSUIGSPluginModel.
        *
        * @return CDesCArray* containing the names of the settings
        */
        CDesCArray* SettingNames();

        /**
        * Returns a reference to iSettingSet.
        *
        * @return TPresSettingsSet& containing one set of the settings
        */        
        TPresSettingsSet& SettingsSet();

        /**
        * Checks the validity of SIP- and XDM-fields, 
        * if not valid empties the fields.
        */     
        void ClearInvalidSIPAndXDMSettingsL();
        
        /**
        * Returns a reference to iSettingSetId.
        *
        * @return TInt& containing setting id
        */        
        TInt& SettingsId();

        /**
        * Returns a reference to iPSIdArray.
        *
        * @return RArray<TInt>& containing iPSIdArray
        */        
        RArray<TInt>& SettingsIDArray();

        //
        //
        // SIP related
        
        /**
        * Fetches names of defined SIP-profiles
        * Refreshes also SIP related arrays.
        *
        * @return descriptor array of names
        */
        CDesCArray* SipProfilesLC();
        
        /**
        * Fetches the ID of indexed SIP profile
        *
        * @param aIndex is the index in the array
        * @return ID of the SIP profile
        */
        TInt SipProfileId( TInt aIndex );
        
        /**
        * Fetches the index of ID'ed SIP profile
        *
        * @param aId is ID of the SIP profile
        * @return index in the array
        */        
        TInt SipProfileIndex( TInt aId );
        
        /**
        * Fetches the name of ID'ed SIP profile
        * Refreshes also SIP related arrays.        
        *
        * @param aId is ID of the SIP profile
        * @return descriptor containing the name
        */        
        HBufC* SipProfileNameL( TInt aId );
        
        //
        //
        // XDM related

        /**
        * Fetches names of defined XDM settings.
        * Refreshes also XDM related arrays.
        *
        * @return descriptor array of names
        */        
        CDesCArray* XdmCollectionNamesL();
        
        /**
        * Fetches the name of ID'ed XDM setting
        *
        * @param aId is ID of the SIP profile
        * @return descriptor containing the name
        */ 
        HBufC* XdmCollectionNameL( TInt aId );
        
        /**
        * Fetches the index of ID'ed XDM setting
        *
        * @param aId is ID of the SIP profile
        * @return index in the array
        */ 
        TInt XdmSettingIndex( TInt aId );
        
        /**
        * Fetches the ID of indexed XDM setting
        *
        * @param aIndex is the index in the array
        * @return ID of the SIP profile
        */        
        TInt XdmSettingId( TInt aIndex );        

    private: // New
    
        /**
        * Destroys given array. Used with TCleanupItem.
        *
        * @param pointerarray to be deleted
        */      
        static void ResetAndDestroy( TAny* aPointerArray );

        /**
        * Refreshes the SIP profile array. Needs to be
        * called before calling most of the SIP-related methods.
        */
        void ReadSipArrayFromEngineL();
        
        /** 
        * Checks if there is ordinal numbering at the end of
        * descriptor and rips it off if there is.
        *
        * @return HBufC* containing the name ready for the numbering
        */
        HBufC* GetPrefixL( HBufC* aName );

        /** 
        * Re-arranges presence settings (name and id arrays) 
        * to alphabetical order.
        */
        void ReArrangePresenceSettingsL();            

    private: // Functions from base classes

        /** 
        * from MSIPProfileRegistryObserver
        * Refreshes in most cases SIP related arrays.
        *
        * See base class.
        */
        void ProfileRegistryEventOccurred(
            TUint32 aSIPProfileId, 
            TEvent aEvent );

        /**
        * from MSIPProfileRegistryObserver
        * Refreshes SIP related arrays.
        *
        * See base class.
        */
        void ProfileRegistryErrorOccurred(                  
            TUint32 aSIPProfileId,
            TInt aError);

        /**
        * from MDesCArray
        * See base class.
        */
        TInt MdcaCount() const;
        
        /**
        * from MDesCArray
        * See base class.
        */        
        TPtrC16 MdcaPoint( TInt aIndex ) const;

    private: // Constructors
        
        /**
        * C++ default constructor.
        */
        CPSUIGSPluginModel();
        
        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

    private: // Data

        // Currently edited presence settings
        TPresSettingsSet iSettingSet;
        // Setting identifier
        TInt iSettingId;
        // SIP profile registry
        CSIPManagedProfileRegistry* iSIPEngine;
        // SIP profile array
        CArrayPtrFlat<CSIPManagedProfile>* iSIPProfiles;
        // XDM ID array
        RArray<TInt> iXdmIdArray;
        // XDM name array
        CDesCArray* iXdmNameArray;
        // Presence settings id array
        RArray<TInt> iPSIdArray;
        // Presence settings name array
        CDesCArray* iPSNameArray;
        // Texts for the MDesCArray
        HBufC* iVisibleSettingName;
        
    };

/**
* Small utility class to store presence settings
* name and id. 
* Used in CPSUIGSPluginModel::ReArrangePresenceSettingsL().
*
* @lib PSUIGSPlugin.lib
* @since Series60_3.2
*/
class CPSUINameIDPair :
    public CBase
    {
    public :
    inline ~CPSUINameIDPair() { delete iName; };
    
    public :
    TInt iID;
    HBufC* iName;
    };

#endif // PSUIGSPLUGINMODEL_H
// End of File
