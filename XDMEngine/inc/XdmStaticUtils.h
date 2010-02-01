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




#ifndef __XDMSTATICUTILS__
#define __XDMSTATICUTILS__

#include <e32base.h>

const TInt KBackSlash           = 92;

class RFs;

// CLASS DECLARATION
class XdmStaticUtils

    {
    public:
        
        /**
        * Check whether a file already exists.
        *
        * @param RFs& An opened file session
        * @param TDesC& The file name to check
        * @return TBool Is the file already there
        */     
        IMPORT_C static TBool CheckFileExistsL( RFs& aFileSession, const TDesC& aFileName );
        
        /**
        * Generate a new file name unique in the context
        * of the folder in which it resides
        *
        * @param RFs& An opened file session
        * @param TDesC& The file name to check
        * @param TDesC& The extension in the filename
        * @return HBufC* The generated filename
        */     
        IMPORT_C static HBufC* GenerateFileNameL( RFs& aFileSession, const TDesC& aFileName,
                                                  const TDesC& aExtension );
        
        /**
        * Clean up a directory
        *
        * @param RFs& An opened file session
        * @param TDesC& The directory to clean up
        * @return void
        */                                           
        IMPORT_C static void CleanUpDirectoryL( RFs& aFileSession, const TDesC& aDirectory );
    
    };

#endif      //__XDMCREDENTIALS__
            
// End of File
