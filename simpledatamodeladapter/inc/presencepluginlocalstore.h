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


#ifndef C_CPRESENCEPLUGINLOCALSTORE_H
#define C_CPRESENCEPLUGINLOCALSTORE_H


#include <e32base.h>
#include <d32dbms.h>
#include <s32file.h>


/**
 * Service contacts local store reader
 *
 * @lib presenceplugin.dll
 * @since S60 v5.0
 */
class CPresencePluginLocalstore : public CBase
    {

public:

    /**
     * Two-phased constructor.
     * @param aServiceName Service name
     */
    static CPresencePluginLocalstore* NewL( const TDesC& aServiceName );

    /**
     * Two-phased constructor.
     * @param aServiceName Service name
     */
    static CPresencePluginLocalstore* NewLC( const TDesC& aServiceName );

    /**
    * Destructor.
    */
    virtual ~CPresencePluginLocalstore();

    /**
     * Seeks given id at contact column
     *
     * @since S60 v5.0
     * @param aIdentifier Id. to search
     * @return ETrue if found
     */
    TBool SeekRowAtContactColL( TInt32& aIdentifier );

private:
	  
    CPresencePluginLocalstore();
	  
    void ConstructL( const TDesC& aServiceName );

    TBool DbExists();

    void OpenDbL();

private: // data

    /**
     * File stream
     */
    RFs iFs;
    
    /**
     * Store database
     */
    RDbStoreDatabase iDb;
    
    /**
     * Table
     */
    RDbTable iTable;
    
    /**
     * Column definitions
     * Own.
     */
    CDbColSet* iColset;

    /**
     * DB Store
     * Own.
     */
    CPermanentFileStore* iFileStore;

    /**
     * Local database name
     * Own.
     */
    HBufC* iLocalDBName;

    };


#endif // C_CPRESENCEPLUGINLOCALSTORE_H
