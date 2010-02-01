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
* Description:   XcapStaticUtils
*
*/




#include <f32file.h>  
#include "XcapStaticUtils.h"

// ----------------------------------------------------------
// XcapStaticUtils::CheckFileExistsL
// 
// ----------------------------------------------------------
//
EXPORT_C TBool XcapStaticUtils::CheckFileExistsL( RFs& aFileSession, const TDesC& aFileName )
    {
    TBool found = EFalse;
    CDir* directory = NULL;
    const TInt KBackSlash = 92;
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
// XcapStaticUtils::GenerateFileNameLC
// 
// ----------------------------------------------------------
//
EXPORT_C HBufC* XcapStaticUtils::GenerateFileNameLC( RFs& aFileSession, const TDesC& aFileName,
                                                     const TDesC& aExtension )
    {
    TBuf<KMaxFileName> name;
    name.Copy( aFileName );
    name.Append( aExtension );
    TBool exists = XcapStaticUtils::CheckFileExistsL( aFileSession, name );
    for( TInt i = 1;exists;i++ )
        {
        name.Zero();
        name.Copy( aFileName );
        name.AppendNum( i );
        name.Append( aExtension );
        exists = XcapStaticUtils::CheckFileExistsL( aFileSession, name );
        }
    return name.AllocLC();
    }
      




// End of File  
