/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   XDM Engine static utils
*
*/




#include <f32file.h>
#include "XdmStaticUtils.h"
#include "XdmLogWriter.h"

// ----------------------------------------------------------
// XdmStaticUtils::CheckFileExistsL
// 
// ----------------------------------------------------------
//
EXPORT_C TBool XdmStaticUtils::CheckFileExistsL( RFs& aFileSession, const TDesC& aFileName )
    {
    TBool found = EFalse;
    CDir* directory = NULL;
    TInt index = aFileName.LocateReverse( KBackSlash );
    TPtrC dir( aFileName.Left( index + 1 ) );
    TPtrC name( aFileName.Right( aFileName.Length() - index - 1 ) );
    User::LeaveIfError( aFileSession.GetDir( dir, KEntryAttNormal, ESortNone, directory ) );
    CleanupStack::PushL( directory );
    const TInt count = directory->Count();
    for( TInt i = 0;!found && i < count;i++ )
        {
        if( name.CompareF( ( *directory )[i].iName ) == 0 )
            found = ETrue;
        }
	CleanupStack::PopAndDestroy();  //directory
    return found;
    }

// ----------------------------------------------------------
// XdmStaticUtils::GenerateFileNameLC
// 
// ----------------------------------------------------------
//
EXPORT_C HBufC* XdmStaticUtils::GenerateFileNameL( RFs& aFileSession, const TDesC& aFileName,
                                                   const TDesC& aExtension )
    {
    TInt i = 1;
    TBuf<KMaxFileName> name;
    name.Copy( aFileName );
    name.Append( aExtension );
    TBool exists = XdmStaticUtils::CheckFileExistsL( aFileSession, name );
    for( ;exists && i <= KMaxLogFiles;i++ )
        {
        name.Zero();
        name.Copy( aFileName );
        name.AppendNum( i );
        name.Append( aExtension );
        exists = XdmStaticUtils::CheckFileExistsL( aFileSession, name );
        }
    if( i >= KMaxLogFiles )
        {
        TPtrC dir( aFileName.Left( aFileName.LocateReverse( KBackSlash ) ) );
        CleanUpDirectoryL( aFileSession, dir );
        name.Zero();
        name.Copy( aFileName );
        name.Append( aExtension );
        }
    return name.AllocL();
    }

// ----------------------------------------------------------
// XdmStaticUtils::CleanUpDirectoryL
// 
// ----------------------------------------------------------
//
EXPORT_C void XdmStaticUtils::CleanUpDirectoryL( RFs& aFileSession, const TDesC& aDirectory )
    {
    TBuf<KMaxFileName> allFiles;
    allFiles.Copy( aDirectory );
    allFiles.Append( _L( "\\*.*") );
    CFileMan* manager = CFileMan::NewL( aFileSession );
    manager->Delete( allFiles );
    delete manager;
    manager = NULL;
    }
         
// End of File  



