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
* Description:  IETF SIMPLE Protocol implementation for XIMP Framework
*
*/


#ifndef DATAMODELADAPTERLOGGER_H
#define DATAMODELADAPTERLOGGER_H

#define DP_TP_PRINT(s) L##s
#define DP_STRA_PRINT(s) DP_TP_PRINT(s)
#define DP_STR_PRINT(t) DP_STRA_PRINT("[PrwAdapter]") L##t
#define DP_LIT_PRINT(s) TPtrC((const TText *) DP_STR_PRINT(s))
#ifdef _DEBUG     
    #include <e32svr.h>
    #define _DEBUG_SIMPLE_DATAMODEL_ADAPTER
#endif

#ifdef _DEBUG_SIMPLE_DATAMODEL_ADAPTER
    #define DP_SDA(a) {RDebug::Print(DP_LIT_PRINT(a));}
    #define DP_SDA2(a, b) {RDebug::Print(DP_LIT_PRINT(a),b);}
    #define DP_SDA3(a, b, c) RDebug::Print(DP_LIT_PRINT(a), b, c);
#else
    #define DP_SDA(a);
    #define DP_SDA2(a, b);
    #define DP_SDA3(a, b, c);
#endif

#ifdef SIMPLE_EUNIT
#define SIMPLE_UNIT_TEST(ClassName) friend class ClassName;
#else 
#define SIMPLE_UNIT_TEST(ClassName)
#endif // SIMPLE_EUNIT

#endif //DATAMODELADAPTERLOGGER_H

