/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef SIMPLEIMLOGGER_H
#define SIMPLEIMLOGGER_H

#ifdef _DEBUG
    #include <e32svr.h>
    #define _DEBUG_SIMPLE_IM_PLUGIN
#endif

#ifdef _DEBUG_SIMPLE_IM_PLUGIN
    #define DP_IMP( a ) \
        { _LIT( KFormatString, a ); RDebug::Print( KFormatString ); }
    #define DP_IMP2( a, b ) \
        { _LIT( KFormatString, a ); RDebug::Print( KFormatString, b ); }
    #define DP_IMP3( a, b, c ) \
        { _LIT( KFormatString, a ); RDebug::Print( KFormatString, b, c ); }
#else
    #define DP_IMP( a );
    #define DP_IMP2( a, b );
    #define DP_IMP3( a, b, c );
#endif

#endif // SIMPLEIMLOGGER_H
