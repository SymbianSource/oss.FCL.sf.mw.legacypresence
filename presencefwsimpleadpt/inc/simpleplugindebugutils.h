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
* Description:    DEBUG utilities, for DEBUG version only
*
*/




#ifndef SIMPLEPLUGINDEBUGUTILS_H
#define SIMPLEPLUGINDEBUGUTILS_H

#ifdef _DEBUG

#include <e32std.h>
#include <s32file.h>

// FORWARD DECLARATION
class RFs;
class RFileReadStream;


//**********************************
// PluginLogger
//**********************************
NONSHARABLE_CLASS( PluginLogger )
    {
public:
    static void Log(TRefByValue<const TDesC> aFmt,...);
    };

#endif // _DEBUG

#endif      // simpledebugutils_H

// End of File
