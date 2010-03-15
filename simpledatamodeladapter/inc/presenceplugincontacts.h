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


#ifndef C_CPRESENCEPLUGINCONTACTS_H
#define C_CPRESENCEPLUGINCONTACTS_H


#include <e32base.h>
#include <MVPbkContactFindObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactStoreListObserver.h>
#include "presencecontactscontextbase.h"
#include "presencelogger.h"

class CVPbkContactManager;
class MVPbkContactOperationBase;
class MVPbkContactLinkArray;
class MVPbkStoreContact;
class CVPbkFieldTypeRefsList;
class CPresencePluginContactStateHandler;
class MPresencePluginContactsObs;


/**
 * CPrecensePluginContacts Operation states
 */
enum TPresenceContactsOperation
    {
    EOperationIsPresenceStoredToContacts
    };


/**
 * Access to virtual phonebook contacts database
 *
 * @lib presenceplugin.dll
 * @since S60 v5.0
 */
NONSHARABLE_CLASS( CPresencePluginContacts ) :
    public CActive,
    public MVPbkContactStoreListObserver,
    public MVPbkContactFindObserver,
    public MVPbkSingleContactOperationObserver,
    public MPresenceContactsContextBase
    {

public:

    /**
     * Two-phased constructor.
     * @param aServiceId Service ID.
     * @param aServiceName Service name
     * @param aObserver Callback results
     */
    static CPresencePluginContacts* NewL(
        TInt aServiceId,
        const TDesC& aServiceName,
        MPresencePluginContactsObs& aObserver );
    
    /**
     * Two-phased constructor.
     * @param aServiceId Service ID.
     * @param aServiceName Service name
     * @param aObserver Callback results
     */
    static CPresencePluginContacts* NewLC(
        TInt aServiceId,
        const TDesC& aServiceName,
        MPresencePluginContactsObs& aObserver );
    
    /**
    * Destructor.
    */
    virtual ~CPresencePluginContacts();
    
    /**
     * Query to virtual bhonebook contacts database
     * to ensure is the presence service stored to database
     *
     * Calls MPresencePluginContactsObs::RequestComplete() when comlete
     *
     * @since S60 v5.0
     * @param aPresenceId Presence ID.
     * @param aStatus Client status
     */
    void IsPresenceIdStoredL( const TDesC16& aPresenceId,
	    TRequestStatus& aStatus );
    
protected:

// from base class CActive
    
    void RunL();
    
    void DoCancel();

    TInt RunError( TInt aError );
    
// from base class MVPbkContactStoreListObserver
     
    /**
     * Called when the opening process is complete. 
     *
     * Before this all stores have sent StoreReady or StoreUnavailable 
     * event. The client can not trust that all stores are available
     * for use when this is called.
     */
    void OpenComplete();
    
// from base class MVPbkContactStoreObserver ( from MVPbkContactStoreListObserver )
     
    /**
     * Called when a contact store is ready to use.
     *
     * @param aContactStore The store that is ready.
     */
    void StoreReady( MVPbkContactStore& aContactStore );

    /**
     * Called when a contact store becomes unavailable.
     *
     * Client may inspect the reason of the unavailability and decide
     * whether or not it will keep the store opened (ie. listen to 
     * the store events).
     *
     * @param aContactStore The store that became unavailable.
     * @param aReason The reason why the store is unavailable.
     *                This is one of the system wide error codes.
     */
    void StoreUnavailable( MVPbkContactStore& aContactStore, 
            TInt aReason );

    /**
     * Called when changes occur in the contact store.
     *
     * @see TVPbkContactStoreEvent
     * @param aContactStore A store whose event it is.
     * @param aStoreEvent The event that has occurred.
     */
    void HandleStoreEventL(
             MVPbkContactStore& aContactStore, 
             TVPbkContactStoreEvent aStoreEvent );
    
// from base class MVPbkContactFindObserver

    /**
     * Called when find is complete. Caller takes ownership of the results
     * In case of an error during find, the aResults may contain only 
     * partial results of the find
     *
     * @param aResults Array of contact links that matched the find
     *                 Callee must take ownership of this object in
     *                 the start of the function, ie. in case the 
     *                 function leaves the results must be destroyed. 
     *                 The find operation can be destroyed at the end 
     *                  of this callback.
     */
    void FindCompleteL( MVPbkContactLinkArray* aResults );
    
    /**
    * Called in case the find fails for some reason. The find operation
    * can be destroyed at the end of this callback.
    * 
    * @param aError One of the system wide error codes.
    *        KErrNotReady if store is not ready (not open or unavailable)
    */
    void FindFailed( TInt aError );
    
// from base class MVPbkSingleContactOperationObserver

    /**
     * Called when the operation is completed.
     *
     * A client has the operation as a member and it can delete the operation
     * instance in this function call. If the implementation of the store
     * calls the function from the operation instance it must not handle
     * any member data after calling it.
     *
     * @param aOperation The completed operation.
     * @param aContact The contact returned by the operation.
     *                 A client must take the ownership immediately.
     *
     *                  NOTE:
     *                  If you use CleanupStack for MVPbkStoreContact
     *                  Use MVPbkStoreContact::PushL or
     *                  CleanupDeletePushL from e32base.h.
     *                  (Do Not Use CleanupStack::PushL(TAny*) because
     *                  then the virtual destructor of the M-class
     *                  won't be called when the object is deleted).
     */
    void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );

    /**
     * Called if the operation fails.
     *
     * A client has the operation as a member and it can delete the operation
     * instance in this function call. If the implementation of the store
     * calls the function from the operation instance it must not handle
     * any member data after calling it.
     *
     * @param aOperation The failed operation.
     * @param aError An error code of the failure.
     */
    void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation, 
            TInt aError );
    
// from base class MPresenceContactsContextBase

    /**
     * Open virtual phonebook contact stores
     *
     * @since S60 v5.0
     */
    void OpenL();

    /**
     * Start virtual phonebook contacts search
     *
     * @since S60 v5.0
     */
    void SearchL();

    /**
     * Return virtual phonebook contact links
     *
     * @since S60 v5.0
     * @return virtual phonebook contact links
     */
    const MVPbkContactLinkArray& ContactLinkArrayL();

    /**
    * Resolve virtual phonebook contact from contact link
    *
    * @since S60 v5.0
    * @param aContactLink contact link to resolve
    */
    void RetrieveContactL(
        const MVPbkContactLink& aContactLink );

private:

    CPresencePluginContacts( TInt aServiceId, MPresencePluginContactsObs& aObserver );

    void ConstructL( const TDesC& aServiceName );

    HBufC* ContactStoreNameL();
    
private: //data
    
    /**
     * Service id.
     */
    TInt iServiceId;
    
    /**
     * Service name.
     * Own.
     */
    HBufC* iServiceName;

    /**
     * Text used in search
     * Own.
     */
    HBufC* iSearchText;
    
    /**
     * Observer.
     * Not own.
     */
    MPresencePluginContactsObs* iObserver;
    
    /**
     * Presence Plugin Contacts Operation
     */
    TPresenceContactsOperation iOperation;
    
    /**
     * Virtual Phonebook field types
     * Own.
     */
    CVPbkFieldTypeRefsList* iFieldTypeRefList;

    /**
     * Virtual Phonebook contact manager
     * Own.
     */
    CVPbkContactManager* iContactManager;

    /**
     * VPbk operation
     * Own.
     */
    MVPbkContactOperationBase* iContactOperation;
    
    /**
     * VPbk contact links
     * Own.
     */
    MVPbkContactLinkArray* iContactLinkArray;

    /**
     * VPbk Store contact array
     * Own.
     */
    RPointerArray<MVPbkStoreContact> iStoreContactArray;
    
    /**
     * state handler
     * Own.
     */
    CPresencePluginContactStateHandler* iStateHandler;
    
    /**
     * Client status
     * Not own.
     */
    TRequestStatus* iClientStatus;

    
    SIMPLE_UNIT_TEST( UT_CPresencePluginContacts )

    };


#endif // C_CPRESENCEPLUGINCONTACTS_H
