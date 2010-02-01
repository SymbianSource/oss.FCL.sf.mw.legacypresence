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




#ifndef __XCAPSTATICUTILS__
#define __XCAPSTATICUTILS__

#include <e32base.h>

class RFs;

// CLASS DECLARATION
class XcapStaticUtils

    {
    public:
        
        /**
        * Check whether a file already exists.
        *
        * @param TDesC& The file name to check
        * @return TBool Is the file already there
        */     
        IMPORT_C static TBool CheckFileExistsL( RFs& aFileSession, const TDesC& aFileName );
        
        /**
        * Generate a new file name unique in the context
        * of the folder in which it resides
        *
        * @param TDesC& The file name to check
        * @return TBool Is the file already there
        */     
        IMPORT_C static HBufC* GenerateFileNameLC( RFs& aFileSession, const TDesC& aFileName,
                                                   const TDesC& aExtension );
    
    };

#endif

// End of File
