/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IETF SIMPLE Protocol implementation for XIMP Framework
*
*/


#include <vpbkcontactstoreuris.h>
#include <spsettings.h>
#include <spproperty.h>
#include <cvpbkcontactmanager.h>
#include <cvpbkcontactstoreuriarray.h>
#include <tvpbkcontactstoreuriptr.h>
#include <mvpbkcontactoperationbase.h>
#include <mvpbkcontactstorelist.h>
#include <mvpbkcontactlinkarray.h>
#include <mvpbkcontactlink.h>
#include <mvpbkstorecontact.h>
#include <mvpbkcontactfielddata.h>
#include <mvpbkcontactfieldtextdata.h>
#include <mvpbkcontactfielduridata.h>
#include <cvpbkcontactidconverter.h>
#include <mvpbkstorecontactfieldcollection.h>
#include <cvpbkfieldtyperefslist.h>
#include <vpbkeng.rsg>

#include "presenceplugincontacts.h" 
#include "presencelogger.h"
#include "presenceplugincontactsobs.h"
#include "presenceplugincontactstatehandler.h"
#include "presencepluginlocalstore.h"
// States
#include "presenceplugincontactstate.h"
#include "presenceplugincontactstateopen.h"
#include "presenceplugincontactstateresolve.h"
#include "presenceplugincontactstatesearch.h"
#include "presenceplugincontactstateend.h"

const TInt KSDMASPSMaxPropertyLength = 512;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginContacts::CPresencePluginContacts()
// ---------------------------------------------------------------------------
//
CPresencePluginContacts::CPresencePluginContacts( TInt aServiceId,
    MPresencePluginContactsObs& aObserver )
    : CActive( CActive::EPriorityStandard ),
    iServiceId( aServiceId ),
    iObserver( &aObserver )
    {
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// CPresencePluginContacts::ConstructL()
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::ConstructL( const TDesC& aServiceName )
    {
    DP_SDA( "CPresencePluginContacts::ConstructL()" );
    
    iServiceName = aServiceName.AllocL();
    CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewLC();
    HBufC* storeName = ContactStoreNameL();

    if ( NULL != storeName )
        {
        DP_SDA2( "CPresencePluginContacts::ConstructL() %S", storeName );
        CleanupStack::PushL( storeName );
        uriArray->AppendL( TVPbkContactStoreUriPtr( *storeName ) );
        CleanupStack::PopAndDestroy( storeName );
        }
    else
        {
        DP_SDA( "CPresencePluginContacts::ConstructL() Use default contact store uri" );
        uriArray->AppendL( TVPbkContactStoreUriPtr(
            VPbkContactStoreUris::DefaultCntDbUri() ) );
        }
    iContactManager = CVPbkContactManager::NewL( *uriArray );
    CleanupStack::PopAndDestroy( uriArray );
    
    DP_SDA( "CPresencePluginContacts::ConstructL() -exit" );
    }
    
    
// ---------------------------------------------------------------------------
// CPresencePluginContacts::NewL()
// ---------------------------------------------------------------------------
//
CPresencePluginContacts* CPresencePluginContacts::NewL( TInt aServiceId,
    const TDesC& aServiceName, MPresencePluginContactsObs& aObserver )
    {
    CPresencePluginContacts* self =
        CPresencePluginContacts::NewLC( aServiceId, aServiceName, aObserver );
    CleanupStack::Pop( self );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// CPresencePluginContacts::NewLC()
// ---------------------------------------------------------------------------
//
CPresencePluginContacts* CPresencePluginContacts::NewLC( TInt aServiceId,
        const TDesC& aServiceName, MPresencePluginContactsObs& aObserver )
    {
    CPresencePluginContacts* self =
        new( ELeave ) CPresencePluginContacts( aServiceId, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aServiceName );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// CPresencePluginContacts::~CPresencePluginContacts()
// ---------------------------------------------------------------------------
//
CPresencePluginContacts::~CPresencePluginContacts()
    {
    DP_SDA("CPresencePluginContacts::~CPresencePluginContacts");
    
    delete iServiceName;
    delete iSearchText;
    delete iContactOperation;
    iStoreContactArray.ResetAndDestroy();
    delete iStateHandler;
    delete iFieldTypeRefList;
    delete iContactLinkArray;
    if( iContactManager )
        {
        TRAP_IGNORE( iContactManager->ContactStoresL().CloseAll( *this ) );
        }
    delete iContactManager;
    iClientStatus = NULL;
    }


// ---------------------------------------------------------------------------
// CPresencePluginContacts::PresenceIdStoredL
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::IsPresenceIdStoredL(
    const TDesC16& aPresenceId, TRequestStatus& aStatus )
    {
    DP_SDA( "CPresencePluginContacts::IsPresenceIdStoredL -Enter" );
    
    iOperation = EOperationIsPresenceStoredToContacts;
    
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    
    iSearchText = HBufC::NewL(
        iServiceName->Length() + aPresenceId.Length() + 1 );
    TPtr searchTextPtr = iSearchText->Des();
    searchTextPtr.Copy( *iServiceName );
    searchTextPtr.Append( ':' );
    searchTextPtr.Append( aPresenceId );
    
    iFieldTypeRefList = CVPbkFieldTypeRefsList::NewL();
    iFieldTypeRefList->AppendL(
        *iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP  ) );
    
    iStateHandler = CPresencePluginContactStateHandler::NewL();
    
    iStateHandler->AddStateL( new( ELeave ) CPresencePluginContactStateOpen(
        *this, *iStateHandler ) );
    iStateHandler->AddStateL( new( ELeave ) CPresencePluginContactStateSearch(
        *this, *iStateHandler ) );
    iStateHandler->AddStateL( new( ELeave ) CPresencePluginContactStateResolve(
        *this, *iStateHandler ) );
    iStateHandler->AddStateL( new( ELeave ) CPresencePluginContactStateEnd(
        *this, *iStateHandler ) );
    
    iStateHandler->Start( &iStatus );
    SetActive();
    
    DP_SDA( "CPresencePluginContacts::IsPresenceIdStoredL - exit" );
    }


// ---------------------------------------------------------------------------
// CPresencePluginContacts::ContactStoreNameL
// ---------------------------------------------------------------------------
//
HBufC* CPresencePluginContacts::ContactStoreNameL()
    {
    DP_SDA( "CPresencePluginContacts::ContactStoreNameL()" );
    
    HBufC* storeName = NULL;
    CSPSettings* spSettings = CSPSettings::NewLC();
    CSPProperty* property = CSPProperty::NewLC();
    
    TInt err = spSettings->FindPropertyL( iServiceId,
                                EPropertyContactStoreId,      
                                *property );
    
    if ( KErrNone == err )
        {
        storeName = HBufC::NewL( KSDMASPSMaxPropertyLength );
        TPtr16 storeNamePtr = storeName->Des();
        err = property->GetValue( storeNamePtr );
        }
    
    CleanupStack::PopAndDestroy( property );
    CleanupStack::PopAndDestroy( spSettings );
    
    return storeName;
    }


// ---------------------------------------------------------------------------
// From class MPresenceContactsContextBase. 
// CPresencePluginContacts::Open
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::OpenL()
    {
    DP_SDA( "CPresencePluginContacts::Open()" );
    iContactManager->ContactStoresL().OpenAllL( *this );
    }


// ---------------------------------------------------------------------------
// From class MPresenceContactsContextBase.
// CPresencePluginContacts::Search
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::SearchL()
    {
    DP_SDA( "CPresencePluginContacts::Search()" );
    iContactOperation = iContactManager->FindL(
        *iSearchText, *iFieldTypeRefList, *this );
    }


// ---------------------------------------------------------------------------
// From class MPresenceContactsContextBase. 
// CPresencePluginContacts::ContactLinkArray
// ---------------------------------------------------------------------------
//
const MVPbkContactLinkArray& CPresencePluginContacts::ContactLinkArrayL()
    {
    if ( NULL == iContactLinkArray )
        {
        User::Leave( KErrNotReady );
        }
    return *iContactLinkArray;
    }


// ---------------------------------------------------------------------------
// From class MPresenceContactsContextBase.
//CPresencePluginContacts::RetrieveContactL
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::RetrieveContactL(
    const MVPbkContactLink& aContactLink )
    {
    DP_SDA( "CPresencePluginContacts::RetrieveContactL()" );
    iContactOperation =
        iContactManager->RetrieveContactL( aContactLink, *this );
    }


// ---------------------------------------------------------------------------
// From class MVPbkContactStoreListObserver.
// CPresencePluginContacts::OpenComplete
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::OpenComplete()
    {
    DP_SDA( "CPresencePluginContacts::OpenComplete()" );
    iStateHandler->State()->Complete();
    }


// ---------------------------------------------------------------------------
// From class MVPbkContactStoreObserver.
// CPresencePluginContacts::StoreReady
// ---------------------------------------------------------------------------
// 
void CPresencePluginContacts::StoreReady(
    MVPbkContactStore& /*aContactStore*/ )
     {
     DP_SDA( "CPresencePluginContacts::StoreReady()" );
     }


// ---------------------------------------------------------------------------
// From class MVPbkContactStoreObserver.
// CPresencePluginContacts::StoreUnavailable
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::StoreUnavailable(
    MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/ )
    {
    DP_SDA( "CPresencePluginContacts::StoreUnavailable()" );
    }


// ---------------------------------------------------------------------------
// From class MVPbkContactStoreObserver.
// CPresencePluginContacts::HandleStoreEventL
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::HandleStoreEventL(
         MVPbkContactStore& /*aContactStore*/, 
         TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    DP_SDA( "CPresencePluginContacts::HandleStoreEventL()" );
    }


// ---------------------------------------------------------------------------
// From class MVPbkContactFindObserver.
// CPresencePluginContacts::FindCompleteL
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::FindCompleteL( MVPbkContactLinkArray* aResults )
    {
    DP_SDA( "CPresencePluginContacts::FindCompleteL()" );
    
    delete iContactLinkArray;
    iContactLinkArray = NULL;
    iContactLinkArray = aResults;
    
    delete iContactOperation;
    iContactOperation = NULL;

    iStateHandler->State()->Complete();
    }


// ---------------------------------------------------------------------------
// From class MVPbkContactFindObserver.
// CPresencePluginContacts::FindFailed
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::FindFailed( TInt aError )
    {
    DP_SDA( "CPresencePluginContacts::FindFailed()" );
    
    delete iContactOperation;
    iContactOperation = NULL;
    
    iStateHandler->State()->Error( aError );
    }


// ---------------------------------------------------------------------------
// From class MVPbkSingleContactOperationObserver.
// CPresencePluginContacts::VPbkSingleContactOperationComplete
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact )
    {
    DP_SDA( "CPresencePluginContacts::VPbkSingleContactOperationComplete()" );
    
    if ( iContactOperation == &aOperation )
        {
        delete iContactOperation;
        iContactOperation = NULL;
        }
    
    TInt error = iStoreContactArray.Append( aContact );
    if ( KErrNone != error )
        {
        delete aContact;
        aContact = NULL;
        iStateHandler->State()->Error( error );
        }
    else
        {
        iStateHandler->State()->Complete();
        }
    }


// ---------------------------------------------------------------------------
// From class MVPbkSingleContactOperationObserver.
// CPresencePluginContacts::VPbkSingleContactOperationFailed
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation, 
        TInt aError )
    {
    DP_SDA( "CPresencePluginContacts::VPbkSingleContactOperationFailed()" );
    
    if ( iContactOperation == &aOperation )
        {
        delete iContactOperation;
        iContactOperation = NULL;
        }
    
    iStateHandler->State()->Error( aError);
    }


// ---------------------------------------------------------------------------
// From class CActive.
// CPresencePluginContacts::RunL
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::RunL()
    {
    DP_SDA2( "CPresencePluginContacts::RunL - status %d", iStatus.Int() );
    
    TBool result( EFalse );
    TBool found( EFalse );
    CPresencePluginLocalstore* localStore = NULL;
    TInt error = iStatus.Int();
    
    switch( iOperation )
        {
        case EOperationIsPresenceStoredToContacts:
            DP_SDA( "CPresencePluginContacts::RunL -EOperationIsPresenceStoredToContacts" );
            
            if ( KErrNone == error )
                {
                localStore = CPresencePluginLocalstore::NewLC( *iServiceName );
                
                for ( TInt i( 0 ); ( i < iStoreContactArray.Count() ) && !found; i++ )
                    {
                    MVPbkContactLink* link = iStoreContactArray[ i ]->CreateLinkLC();
                    CVPbkContactIdConverter* converter =
                        CVPbkContactIdConverter::NewL( link->ContactStore() );
                    CleanupStack::PushL( converter );
    
                    TInt32 id = converter->LinkToIdentifier( *link );
                    if ( localStore->SeekRowAtContactColL( id ) )
                        {
                        found = ETrue;
                        result = ETrue;
                        }
                    CleanupStack::PopAndDestroy( converter );
                    CleanupStack::PopAndDestroy(); // link
                    }
                 
                CleanupStack::PopAndDestroy( localStore );
                }
            if ( KErrNotFound == error )
                {
                error = KErrNone;
                }
            iObserver->RequestComplete( &result, iOperation, error );
            break;
        default:
            break;
        }
    // Operation completed.
    // State handler not needet anymore
    delete iStateHandler;
    iStateHandler = NULL;
    // Close contact stores
    iContactManager->ContactStoresL().CloseAll( *this );
    
    User::RequestComplete( iClientStatus, error );
    }


// ---------------------------------------------------------------------------
// From class CActive.
// CPresencePluginContacts::DoCancel
// ---------------------------------------------------------------------------
//
void CPresencePluginContacts::DoCancel()
    {
    if ( iClientStatus && *iClientStatus == KRequestPending )
        {
        User::RequestComplete( iClientStatus, KErrCancel );
        }
    iStateHandler->Complete( KErrCancel );
    }


// ---------------------------------------------------------------------------
// From class CActive.
// CPresencePluginContacts::RunError
// ---------------------------------------------------------------------------
//
TInt CPresencePluginContacts::RunError( TInt aError )
    {
    DP_SDA2( "CPresencePluginContacts::RunError - status %d", aError );
    User::RequestComplete( iClientStatus, aError );
    return KErrNone;
    }

// End of file
