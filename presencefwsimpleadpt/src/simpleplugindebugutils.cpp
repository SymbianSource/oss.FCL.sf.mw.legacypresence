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




// INCLUDE FILES
#include <e32std.h>

#ifdef _DEBUG
#include <flogger.h>
#include <e32debug.h>
#include <apparc.h>
#include <bautils.h>

#include "simpleplugindebugutils.h"

//**********************************
// PluginLogger
//**********************************
const TInt KLogBufferLength = 256;

// ---------------------------------------------------------
// PluginLogger::Log
// ---------------------------------------------------------
//
void PluginLogger::Log(TRefByValue<const TDesC> aFmt,...)
    {
    VA_LIST list;
    VA_START(list, aFmt);

    // Print to log file
    TBuf<KLogBufferLength> buf;
    buf.FormatList(aFmt, list);

    _LIT(KLogDir, "simple");
    _LIT(KLogFile, "simple.txt");
    // Write to log file
    RFileLogger::Write(KLogDir, KLogFile, EFileLoggingModeAppend, buf);
    }

#endif  // _DEBUG



//  End of File

