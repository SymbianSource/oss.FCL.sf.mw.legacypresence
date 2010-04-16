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
* Description:    Presence settings UI model implementation.
*
*/




// INCLUDE FILES
#include <escapeutils.h>
#include <sipmanagedprofileregistry.h>
#include <sipprofileregistryobserver.h>
#include <sipmanagedprofile.h>
#include <sipprofile.h>
#include <StringLoader.h>
#include <psuigspluginrsc.rsg>
#include <XdmSettingsApi.h>
#include "psuigspluginmodel.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::CPSUIGSPluginModel()
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CPSUIGSPluginModel::CPSUIGSPluginModel()
    {
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::~CPSUIGSPluginModel()
// Destructor
// ---------------------------------------------------------------------------
//
CPSUIGSPluginModel::~CPSUIGSPluginModel()
    {
    delete iSIPEngine;
    if ( iSIPProfiles )
        {
        iSIPProfiles->ResetAndDestroy();
        delete iSIPProfiles;
        }     
        
    iXdmIdArray.Close();
    delete iXdmNameArray;
    
    iPSIdArray.Close();
    delete iPSNameArray;    
    delete iVisibleSettingName;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginModel::ConstructL()
    {
    RetrieveSettingsL();
    iVisibleSettingName = HBufC::NewL( 
        KPresSetNameMaxLength + 2 );// max chars + KColumnListSeparator
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::NewL()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPSUIGSPluginModel* CPSUIGSPluginModel::NewL()
    {
    CPSUIGSPluginModel* self = new ( ELeave ) CPSUIGSPluginModel();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    return self;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::RetrieveSettingsL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CPSUIGSPluginModel::RetrieveSettingsL()
    {
    RefreshPSArraysL();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::SaveSettingsL()
// See header for details.
// ---------------------------------------------------------------------------
//    
void CPSUIGSPluginModel::SaveSettingsL()
    {
    if ( iSettingId == 0 )
        {// new settings
        iSettingId = PresSettingsApi::CreateSetL( iSettingSet );
        }
    else
        {// old & edited settings
        PresSettingsApi::UpdateSetL( iSettingSet, iSettingId );
        }
    RefreshPSArraysL();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::CreateDefaultSettingsL()
// See header for details.
// ---------------------------------------------------------------------------
//    
void CPSUIGSPluginModel::CreateDefaultSettingsL()
    {
    iSettingId = 0; // new settings
    
    iSettingSet.iSetName.Zero();
    HBufC* name = CreateSettingNameLC();
    iSettingSet.iSetName.Append( *name );
    CleanupStack::PopAndDestroy( name );// name
    
    iSettingSet.iSipProfile = KErrNotFound;
    iSettingSet.iXDMSetting = KErrNotFound;
    iSettingSet.iObjectSize = KErrNotFound;
    iSettingSet.iContSrvrAdd.Zero();
    iSettingSet.iPublicationInt = KErrNotFound;
    iSettingSet.iMaxSubscriptions = KErrNotFound;
    iSettingSet.iMaxContactsInList = KErrNotFound;
    iSettingSet.iDomainSyntax.Zero();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::DuplicateExistingSettingsL()
// See header for details.
// ---------------------------------------------------------------------------
//    
void CPSUIGSPluginModel::DuplicateExistingSettingsL( TInt aIndex )
    {
    __ASSERT_DEBUG( aIndex < iPSIdArray.Count() ,User::Panic( KPSUIGSPluginPanicCategory, KErrOverflow ));
    
    iSettingId = 0; // new settings
    TPresSettingsSet originalSettings;
    PresSettingsApi::SettingsSetL( iPSIdArray[ aIndex ], originalSettings );
    
    iSettingSet.iSetName.Zero();
    HBufC* name = CreateSettingNameLC( originalSettings.iSetName.AllocL() );
    iSettingSet.iSetName.Append( *name );
    CleanupStack::PopAndDestroy( name );// name
    
    iSettingSet.iSipProfile = originalSettings.iSipProfile;
    iSettingSet.iXDMSetting = originalSettings.iXDMSetting;
    iSettingSet.iObjectSize = originalSettings.iObjectSize;
    iSettingSet.iContSrvrAdd = originalSettings.iContSrvrAdd;
    iSettingSet.iPublicationInt = originalSettings.iPublicationInt;
    iSettingSet.iMaxSubscriptions = originalSettings.iMaxSubscriptions;
    iSettingSet.iMaxContactsInList = originalSettings.iMaxContactsInList;
    iSettingSet.iDomainSyntax = originalSettings.iDomainSyntax;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::AllCompulsoryFieldsFilled()
// See header for details.
// ---------------------------------------------------------------------------
//    
TBool CPSUIGSPluginModel::AllCompulsoryFieldsFilled()
    {
    TBool returnValue = ETrue;
    for ( TInt i = EPSUIServerNameId ; i <= EPSUIDomainSyntaxId && returnValue ; i++ )
        {
        switch( i ) 
            {
            case EPSUIServerNameId:
                returnValue = ( iSettingSet.iSetName.Length() != 0 );
                break;
            case EPSUISIPProfileId:
                returnValue = ( iSettingSet.iSipProfile != KErrNotFound );
                break;
            case EPSUIObjectSizeId:
                returnValue = ( iSettingSet.iObjectSize != KErrNotFound );
                break;
            case EPSUIDomainSyntaxId:
                returnValue = ( iSettingSet.iDomainSyntax.Length() != 0 );
                break;
            default:
                // do nothing with non-compulsory ones
                break;
            }        
        }    
    return returnValue;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::IsItDuplicatePSName()
// See header for details.
// ---------------------------------------------------------------------------
// 
TBool CPSUIGSPluginModel::IsItDuplicatePSName()
    {
    TBool found = KErrNone;
    TInt pos = 0;
    found = ( iPSNameArray->Find( iSettingSet.iSetName, pos ) == KErrNone );
    if ( found )
        {// dupe found, let's check it's not the same setting
        __ASSERT_DEBUG( pos < iPSIdArray.Count() ,User::Panic( KPSUIGSPluginPanicCategory, KErrOverflow ));
        found = !( iSettingId == iPSIdArray[ pos ]);
        }
    return found;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::EditSettingsL()
// See header for details.
// ---------------------------------------------------------------------------
// 
void CPSUIGSPluginModel::EditSettingsL( TInt aIndex )
    {
    __ASSERT_DEBUG( aIndex < iPSIdArray.Count() ,User::Panic( KPSUIGSPluginPanicCategory, KErrOverflow ));
    TInt settingId = iPSIdArray[ aIndex ];
    PresSettingsApi::SettingsSetL( settingId, iSettingSet );
    iSettingId = settingId;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::DeleteSettingsL()
// See header for details.
// ---------------------------------------------------------------------------
// 
void CPSUIGSPluginModel::DeleteSettingsL( TInt aToBeDeleted, TBool aIsIndex )
    {
    if ( aIsIndex )
        {// is index
        __ASSERT_DEBUG( aToBeDeleted < iPSIdArray.Count() ,
            User::Panic( KPSUIGSPluginPanicCategory, KErrOverflow ));
        PresSettingsApi::RemoveSetL( iPSIdArray[ aToBeDeleted ]);
        }
    else
        {// is id
        PresSettingsApi::RemoveSetL( aToBeDeleted );
        }
    RefreshPSArraysL();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::RefreshPSArraysL()
// See header for details.
// ---------------------------------------------------------------------------
// 
void CPSUIGSPluginModel::RefreshPSArraysL()
    {
    delete iPSNameArray;
    iPSNameArray = NULL;
    iPSIdArray.Reset();
    
    iPSNameArray = PresSettingsApi::GetAllSetsNamesLC( iPSIdArray );
    CleanupStack::Pop( iPSNameArray );// iPSNameArray
    ReArrangePresenceSettingsL();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::ReArrangePresenceSettingsL()
// See header for details.
// ---------------------------------------------------------------------------
// 
void CPSUIGSPluginModel::ReArrangePresenceSettingsL()
    {
    const TInt settingsCount = iPSNameArray->Count();
    __ASSERT_DEBUG( 
        settingsCount == iPSIdArray.Count(), 
        User::Panic( KPSUIGSPluginPanicCategory, KErrArgument ));
   
    // make a tables of the original pairs
    RPointerArray<CPSUINameIDPair> pairArray;
    TCleanupItem clItem( ResetAndDestroy, &pairArray );
    CleanupStack::PushL( clItem );
    CPSUINameIDPair* pair = NULL;
    for( TInt i = 0;i < settingsCount;i++ )
        {
        pair = new ( ELeave ) CPSUINameIDPair;
        CleanupStack::PushL( pair );
        pair->iID = iPSIdArray[ i ];
        pair->iName = iPSNameArray->MdcaPoint( i ).AllocL();
        pairArray.AppendL( pair );
        CleanupStack::Pop( pair );// pair is in array's responsibility now
        }

    // sort the original
    iPSNameArray->Sort( ECmpCollated );
    
    // re-arrange the iPSIdArray based on order of iPSNameArray
    iPSIdArray.Reset();
    for( TInt i = 0;i < settingsCount;i++ )
        {
        for( TInt ii = 0;ii < pairArray.Count();ii++ )
            {
            if ( !iPSNameArray->MdcaPoint( i ).Compare(  
                *pairArray[ii]->iName ))
                {
                iPSIdArray.AppendL( pairArray[ii]->iID );
                delete pairArray[ii];
                pairArray.Remove(ii);
                break;// no need to go through if already found
                }
            }
        }
    CleanupStack::PopAndDestroy( &pairArray );
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::MdcaCount()
// See header for details.
// ---------------------------------------------------------------------------
// 
TInt CPSUIGSPluginModel::MdcaCount() const
    {
    return iPSNameArray->MdcaCount();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::MdcaPoint()
// See header for details.
// ---------------------------------------------------------------------------
// 
TPtrC16 CPSUIGSPluginModel::MdcaPoint( TInt aIndex ) const
    {
    TPtr tempText = iVisibleSettingName->Des();
    tempText.Zero();
    tempText.Append( KColumnListSeparator );
    tempText.Append( iPSNameArray->MdcaPoint( aIndex ));

    return tempText;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::CreateSettingNameLC()
// See header for details.
// ---------------------------------------------------------------------------
// 
HBufC* CPSUIGSPluginModel::CreateSettingNameLC( HBufC* aOriginalName )
    {
    _LIT( KPSUIGSPluginNumbering, "(%N)" );
    const TInt KPSUIFormattingExtraLength = 6;
    
    // prepare
    HBufC* settingName = aOriginalName; 

    if ( settingName->Length() + KPSUIFormattingExtraLength > KPresSetNameMaxLength )
        {//if there is no room for duplicate-numbering, let user take care of that when saving settings
        CleanupStack::PushL( settingName );
        return settingName;
        }
    
    settingName = settingName->ReAlloc( 
        settingName->Length() + KPSUIFormattingExtraLength );// enough for formatting
    CleanupStack::PushL( settingName );
    HBufC* tempSetName = NULL;
    TInt pos = 0;// used as dummy
    TInt found = 0;
    TInt index = 1;
    TPtr ptr = settingName->Des();
    
    // do the actual checking of name
    found = iPSNameArray->Find( *settingName, pos );
    while ( found == 0 )
        {
        if ( !tempSetName )
            {
            if ( aOriginalName )
                {// aOriginalName given
                tempSetName = GetPrefixL( settingName );
                tempSetName = tempSetName->ReAlloc( 
                    tempSetName->Length() + KPSUIFormattingExtraLength );// enough for formatting
                CleanupStack::PushL( tempSetName );
                tempSetName->Des().Append( KPSUIGSPluginNumbering );
                }
            else
                {// aOriginalName not given
                }
            }
        StringLoader::Format( ptr, *tempSetName, -1, index );
        found = iPSNameArray->Find( *settingName, pos );
        index++;
        }

    // cleanup
    if ( tempSetName )
        {
        CleanupStack::PopAndDestroy( tempSetName ); //tempSetName
        }

    return settingName;    
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::GetPrefixL()
// See header for details.
// ---------------------------------------------------------------------------
// 
HBufC* CPSUIGSPluginModel::GetPrefixL( HBufC* aName )
    {
    TPtrC prefix = aName->Des();
    TInt lastBrace = aName->LocateReverse('(');
    if ( lastBrace != KErrNotFound )
        {
        // aName looks like "<prefix><brace><something>".
        // See if <something> is an integer number and
        // <something> ends to a brace.
        TPtrC num = aName->Right( aName->Length() - lastBrace - 1 );
        TInt val;
        TLex lex( num );
        if ( lex.Val( val ) == KErrNone && num.Locate(')') == num.Length() - 1 )
            {
            // Yes, the trailer is an integer.
            prefix.Set( aName->Left( lastBrace ) );
            }
        }
    return prefix.AllocL();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::CurrentSettingName()
// See header for details.
// ---------------------------------------------------------------------------
// 
HBufC* CPSUIGSPluginModel::SettingNameLC( TInt aIndex )
    {
    return iPSNameArray->MdcaPoint( aIndex ).AllocLC();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::SettingNames()
// See header for details.
// ---------------------------------------------------------------------------
// 
CDesCArray* CPSUIGSPluginModel::SettingNames()
    {
    return iPSNameArray;// ownership stays in CPSUIGSPluginModel
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::SettingsSet()
// See header for details.
// ---------------------------------------------------------------------------
// 
TPresSettingsSet& CPSUIGSPluginModel::SettingsSet()
    {
    return iSettingSet;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::ClearInvalidSIPAndXDMSettingsL()
// See header for details.
// ---------------------------------------------------------------------------
// 
void CPSUIGSPluginModel::ClearInvalidSIPAndXDMSettingsL()
    {
    // SIP
    ReadSipArrayFromEngineL();
    if ( iSettingSet.iSipProfile != KErrNotFound && 
        KErrNotFound == SipProfileIndex( iSettingSet.iSipProfile ))
        {
        iSettingSet.iSipProfile = KErrNotFound;
        }
    // XDM
    XdmCollectionNamesL();
    if ( iSettingSet.iXDMSetting != KErrNotFound && 
        KErrNotFound == XdmSettingIndex( iSettingSet.iXDMSetting ))
        {
        iSettingSet.iXDMSetting = KErrNotFound;
        }
    }
    
// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::SettingsId()
// See header for details.
// ---------------------------------------------------------------------------
// 
TInt& CPSUIGSPluginModel::SettingsId()
    {
    return iSettingId;
    }    

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::SettingsIDArray()
// See header for details.
// ---------------------------------------------------------------------------
//
RArray<TInt>& CPSUIGSPluginModel::SettingsIDArray()
    {
    return iPSIdArray;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::SipProfilesLC()
// See header for details.
// ---------------------------------------------------------------------------
//   
CDesCArray* CPSUIGSPluginModel::SipProfilesLC()
    {
    ReadSipArrayFromEngineL();
    CDesCArray* array = new ( ELeave ) CDesCArrayFlat( 2 );
    CleanupStack::PushL( array );
    const TInt arrayCount = iSIPProfiles->Count();
    for ( TInt i = 0; i < arrayCount; i++ )
        {
        const TDesC8* profileName;
        iSIPProfiles->At( i )->GetParameter( KSIPProviderName, profileName );
        HBufC* profileName16 = 
            EscapeUtils::ConvertToUnicodeFromUtf8L( *profileName );
        CleanupStack::PushL( profileName16 );
        array->AppendL( profileName16->Des() );
        CleanupStack::PopAndDestroy( profileName16 );// profileName16
        }
    return array;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::SipProfileId()
// See header for details.
// ---------------------------------------------------------------------------
//   
TInt CPSUIGSPluginModel::SipProfileId( TInt aIndex )
    {
    __ASSERT_DEBUG( iSIPProfiles, User::Panic( KPSUIGSPluginPanicCategory, KErrNotFound ));

    TUint32 id = 0;
    if ( aIndex < iSIPProfiles->Count())
        {        
        iSIPProfiles->At( aIndex )->GetParameter( KSIPProfileId, id );
        }

    __ASSERT_DEBUG( id < KMaxTInt ,User::Panic( KPSUIGSPluginPanicCategory, KErrOverflow ));
    return id ? TInt( id ) : KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::SipProfileIndex()
// See header for details.
// ---------------------------------------------------------------------------
//   
TInt CPSUIGSPluginModel::SipProfileIndex( TInt aId )
    {
    __ASSERT_DEBUG( iSIPProfiles, User::Panic( KPSUIGSPluginPanicCategory, KErrNotFound ));    
    const TInt arrayCount = iSIPProfiles->Count();
    TUint32 idValue = aId;
    for ( TInt i = 0; i < arrayCount; i++ )
        {
        TUint32 tempValue;
        iSIPProfiles->At( i )->GetParameter( KSIPProfileId, tempValue );
        if ( tempValue == idValue )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::SipProfileNameL()
// See header for details.
// ---------------------------------------------------------------------------
//   
HBufC* CPSUIGSPluginModel::SipProfileNameL( TInt aId )
    {
    const TInt arrayCount = iSIPProfiles->Count();
    TUint32 idValue = aId;
    for ( TInt i = 0; i < arrayCount; i++ )
        {
        TUint32 tempValue;
        iSIPProfiles->At( i )->GetParameter( KSIPProfileId, tempValue );
        if ( tempValue == idValue )
            {
            const TDesC8* profileName;
            iSIPProfiles->At( i )->GetParameter( KSIPProviderName, profileName );
            return EscapeUtils::ConvertToUnicodeFromUtf8L( *profileName );// name found
            }
        }
    return HBufC::NewL( 0 );// name not found
    }
 
// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::ReadSipArrayFromEngineL()
// See header for details.
// ---------------------------------------------------------------------------
//       
void CPSUIGSPluginModel::ReadSipArrayFromEngineL()
    {
    RArray<TSIPProfileTypeInfo> array;
    CleanupClosePushL( array );
    
    if ( !iSIPEngine )
        {
        iSIPEngine = CSIPManagedProfileRegistry::NewL( *this );
        }
    
    // Get the supported profile types
    iSIPEngine->SupportedProfileTypesL( array );
    
    // check iSIPProfiles, if exists delete
    if ( iSIPProfiles )
        {
        iSIPProfiles->ResetAndDestroy();
        delete iSIPProfiles;
        }

    // Create the profile pointer array
    iSIPProfiles = 
        new ( ELeave ) CArrayPtrFlat<CSIPManagedProfile>( 2 );

    RPointerArray<CSIPProfile> profilePointerArray;
    TCleanupItem clItem( ResetAndDestroy, &profilePointerArray );
    CleanupStack::PushL( clItem );
    
    // Get all profiles based on profile types
    const TInt arrayCount = array.Count();
    for ( TInt i = 0; i < arrayCount; i++ )
        {        
        iSIPEngine->ProfilesL( array[i], profilePointerArray );
        while ( profilePointerArray.Count() > 0 )
            {
            CSIPManagedProfile* profile = 
                static_cast<CSIPManagedProfile*>( profilePointerArray[ 0 ]);
            iSIPProfiles->AppendL( profile );
            profilePointerArray.Remove( 0 ); 
            }                    
        profilePointerArray.Reset();    
        }

    CleanupStack::PopAndDestroy( 2, &array ); // array
    }    

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::ResetAndDestroy()
// See header for details.
// ---------------------------------------------------------------------------
//   
void CPSUIGSPluginModel::ResetAndDestroy( 
    TAny* aPointerArray )
    {
    RPointerArray<CSIPProfile>* array =
        static_cast<RPointerArray<CSIPProfile>*>( aPointerArray );
    array->ResetAndDestroy();
    array->Close();
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::ProfileRegistryEventOccurred()
// See header for details.
// ---------------------------------------------------------------------------
//   
void CPSUIGSPluginModel::ProfileRegistryEventOccurred( 
    TUint32 /*aSIPProfileId*/, TEvent aEvent )
    {
    switch( aEvent )    
        {
        case EProfileCreated:
        case EProfileUpdated:   
        case EProfileDestroyed:
            // update sip profile array
            TRAP_IGNORE( ReadSipArrayFromEngineL() );
            break;
        default:
            // do nothing.
            break;
        }
    }
    
// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::ProfileRegistryErrorOccurred()
// See header for details.
// ---------------------------------------------------------------------------
//       
void CPSUIGSPluginModel::ProfileRegistryErrorOccurred(                  
    TUint32 /*aSIPProfileId*/,
    TInt /*aError*/ )
    {        
    // update sip profile array
    TRAP_IGNORE( ReadSipArrayFromEngineL() );
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::XdmCollectionNamesL()
// See header for details.
// ---------------------------------------------------------------------------
//  
CDesCArray* CPSUIGSPluginModel::XdmCollectionNamesL()
    {
    delete iXdmNameArray;
    iXdmNameArray = NULL;
    iXdmIdArray.Reset();
    
    iXdmNameArray = TXdmSettingsApi::CollectionNamesLC( iXdmIdArray );
    CleanupStack::Pop( iXdmNameArray );// iXdmNameArray
    return iXdmNameArray;// pointer is given, but ownership stays here
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::XdmCollectionNameL()
// See header for details.
// ---------------------------------------------------------------------------
//  
HBufC* CPSUIGSPluginModel::XdmCollectionNameL( TInt aId )
    {
    HBufC* buf = NULL;
    TRAPD( err, buf = TXdmSettingsApi::PropertyL( aId, EXdmPropName ));
    return ( err == KErrNone ) ? buf : HBufC::NewL( 0 );
    }

// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::XdmSettingIndex()
// See header for details.
// ---------------------------------------------------------------------------
//  
TInt CPSUIGSPluginModel::XdmSettingIndex( TInt aId )
    {
    const TInt arrayCount = iXdmIdArray.Count();
    for ( TInt i = 0; i < arrayCount; i++ )
        {
        if ( aId == iXdmIdArray[ i ] )
            {
            return i;
            }
        }    
    return KErrNotFound;    
    }
    
// ---------------------------------------------------------------------------
// CPSUIGSPluginModel::XdmSettingId()
// See header for details.
// ---------------------------------------------------------------------------
//  
TInt CPSUIGSPluginModel::XdmSettingId( TInt aIndex )
    {
    TInt id = KErrNotFound;
    if ( aIndex+1 <= iXdmIdArray.Count())
        {        
        id = iXdmIdArray[ aIndex ];
        }
    return id;
    }

// End of file
