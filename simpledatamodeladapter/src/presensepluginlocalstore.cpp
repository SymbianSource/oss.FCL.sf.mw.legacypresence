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


#include <aknutils.h>
#include "presencepluginlocalstore.h"
#include "presencelogger.h"

_LIT( KContactId, "ContactId" );
_LIT( KContactTable, "Contacts" );
_LIT( KStorageExtn, ".db" );
_LIT( KDbPath, "c:\\MeCo\\" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginLocalstore::CPresencePluginLocalstore
// ---------------------------------------------------------------------------
//
CPresencePluginLocalstore::CPresencePluginLocalstore()
    {
    }


// ---------------------------------------------------------------------------
// CPresencePluginLocalstore::ConstructL()
// ---------------------------------------------------------------------------
//
void CPresencePluginLocalstore::ConstructL( const TDesC& aServiceName )
    {
    DP_SDA( "CPresencePluginLocalstore::ConstructL Start" );

    iLocalDBName =
        HBufC::NewL( aServiceName.Length()+ KStorageExtn().Length() );
    TPtr localDBNamePtr = iLocalDBName->Des();
    localDBNamePtr.Append( aServiceName );

    _LIT( KSpecialChar, "/\\:*?<>\"" );
    AknTextUtils::StripCharacters( localDBNamePtr, KSpecialChar );
    localDBNamePtr.Append( KStorageExtn() );

    DP_SDA2( "CPresencePluginLocalstore::ConstructL - localDBName: %S" , &localDBNamePtr );

    User::LeaveIfError( iFs.Connect() );
    if ( DbExists() )
        {
        DP_SDA( "CPresencePluginLocalstore::ConstructL - DB exists" );
        OpenDbL();
        }
    else
        {
        DP_SDA( "CPresencePluginLocalstore::ConstructL - DB does not exist LEAVE!" );
        User::Leave( KErrNotReady );
        }

    User::LeaveIfError( iTable.Open( iDb, KContactTable ) );
    iColset = iDb.ColSetL( KContactTable );

    DP_SDA( "CPresencePluginLocalstore::ConstructL End" );
    }


// ---------------------------------------------------------------------------
// CPresencePluginLocalstore::NewL()
// ---------------------------------------------------------------------------
//
CPresencePluginLocalstore* CPresencePluginLocalstore::NewL(
    const TDesC& aServiceName )
    {
    CPresencePluginLocalstore* self =
        CPresencePluginLocalstore::NewLC( aServiceName );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CPresencePluginLocalstore::NewLC()
// ---------------------------------------------------------------------------
//
CPresencePluginLocalstore* CPresencePluginLocalstore::NewLC(
    const TDesC& aServiceName )
    {
    CPresencePluginLocalstore* self =
        new( ELeave ) CPresencePluginLocalstore();
    CleanupStack::PushL( self );
    self->ConstructL( aServiceName );
    return self;
    }


// ---------------------------------------------------------------------------
// CPresencePluginLocalstore::~CPresencePluginLocalstore()
// ---------------------------------------------------------------------------
//
CPresencePluginLocalstore::~CPresencePluginLocalstore()
    {
    DP_SDA( "CPresencePluginLocalstore::~CPresencePluginLocalstore Start" );

    delete iLocalDBName;
    delete iFileStore;
    delete iColset;

    iTable.Close();
    iDb.Close();
    iFs.Close();

    DP_SDA( "CPresencePluginLocalstore::~CPresencePluginLocalstore End" );
    }


// ---------------------------------------------------------------------------
// CPresencePluginLocalstore::OpenDbL
// ---------------------------------------------------------------------------
//
void CPresencePluginLocalstore::OpenDbL()
    {
    DP_SDA( "CPresencePluginLocalstore::OpenDbL() Start" );

    TBuf< KMaxPath > storagePath;
    storagePath.Append( KDbPath );
    storagePath.Append( *iLocalDBName );
    iFileStore = CPermanentFileStore::OpenL( iFs, storagePath,
        EFileShareReadersOrWriters|EFileWrite );
    iFileStore->SetTypeL( iFileStore->Layout() );
    iDb.OpenL( iFileStore, iFileStore->Root() );

    DP_SDA( "CPresencePluginLocalstore::OpenDbL() End" );
    }


// ---------------------------------------------------------------------------
// CPresencePluginLocalstore::DbExists
// ---------------------------------------------------------------------------
//
TBool CPresencePluginLocalstore::DbExists()
    {
    DP_SDA( "CPresencePluginLocalstore::DbExists() Start" );

    TBool ret( ETrue );
    RFile temp;
    TBuf< KMaxPath > storagePath;
    storagePath.Append( KDbPath );
    storagePath.Append( *iLocalDBName );

    TInt err( temp.Open( iFs, *( &storagePath ),
        EFileShareReadersOrWriters|EFileRead ) );
    TInt size( 0 );

    if ( KErrNone == err )
        {
        temp.Size( size );
        }

    temp.Close();

    if ( ( 0 == size ) || ( KErrNone != err ) )
        {
        ret = EFalse;
        }

    DP_SDA( "CPresencePluginLocalstore::DbExists() End" );
    return ret;
    }


// ---------------------------------------------------------------------------
// CPresencePluginLocalstore::SeekRowL()
// ---------------------------------------------------------------------------
//
TBool CPresencePluginLocalstore::SeekRowAtContactColL(
    TInt32& aIdentifier )
    {
    DP_SDA( "CPresencePluginLocalstore::SeekRowAtContactColL() Start" );

    TBool ret( EFalse );
    TDbColNo colNo = iColset->ColNo( KContactId );
    iTable.BeginningL();

    while ( iTable.NextL() )
        {
        DP_SDA( "CPresencePluginLocalstore::SeekRowAtContactColL() - next found" );
        iTable.GetL();
        if ( iTable.ColInt32( colNo ) == aIdentifier )
            {
            ret = ETrue;
            break;
            }
        }

    DP_SDA2( "CPresencePluginLocalstore::SeekRowAtContactColL() End - ret = %d", ret );
    return ret;
    }
