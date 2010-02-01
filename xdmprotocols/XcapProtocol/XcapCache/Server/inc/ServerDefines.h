/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   XCapServerDefines
*
*/




#ifndef __SERVERDEFINES_H__
#define __SERVERDEFINES_H__

#include <e32base.h>

const TInt KPageFilePosSeparator            = 32;
_LIT8( KPageFileSeparator,                  " " );
_LIT8( KIndexFileEndOfLine,                 "\r\n" );
_LIT8( KIndexFileEndOfEntry,                "\r\n\r\n" );
_LIT8( KValuePropertySeparator,             "=" );
_LIT8( KValueFieldSeparator,                ";" );

//Array of characters available for random string creation
const TInt KRandomStringCharArray[]     = { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
                                            0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
                                            0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
                                            0x59, 0x5A, 0x61, 0x62, 0x63, 0x64, 0x30, 0x31,
                                            0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
                                            0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C,
                                            0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
                                            0x75, 0x76, 0x77, 0x78, 0x79, 0x7A };
                                            
const TReal KDefaultCompression             = 0.75;                                 
const TInt KRandStringLength                = 20;
const TInt KDateMaxSize                     = 100;
const TInt KDateTimeMaxSize                 = 200;
_LIT( KDateFormatFileName,                  "%1%2%3");
_LIT( KTimeFormatFileName,                  "%H%T%S");
_LIT( KCacheDateFormat,                     "%1-%2-%3");
_LIT( KCacheTimeFormat,                     "%H:%T:%S");
_LIT( KDateTimeFormat,                      "%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S");



#endif // #ifndef __SERVERDEFINES_H__

// End of File
