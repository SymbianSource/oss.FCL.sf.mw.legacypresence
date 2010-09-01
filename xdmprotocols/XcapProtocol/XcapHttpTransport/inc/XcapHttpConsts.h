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
* Description:   XcapHttpConsts
*
*/




#ifndef __XCAPHTTPCONSTS__
#define __XCAPHTTPCONSTS__

#include <e32base.h>

const TInt KXcapHashLength                                  = 32;
const TInt KXcapRawHashLength                               = 16;

_LIT8( KAuthHeaderParamEnd,                                 "," );
_LIT8( KAuthHeaderParamEndQ,                                "\"," );
_LIT8( KAuthHeaderParamQuote,                               "\"" );
_LIT8( KAuthHeaderStart,                                    "Digest username=\"" );
_LIT8( KAuthHeaderRealm,                                    "realm=\"" );
_LIT8( KAuthHeaderNonce,                                    "nonce=\"" );
_LIT8( KAuthHeaderUri,                                      "uri=\"" );
_LIT8( KAuthHeaderResponse,                                 "response=\"" );
_LIT8( KAuthHeaderCNonce,                                   "cnonce=\"" );
_LIT8( KAuthHeaderOpaque,                                   "opaque=\"" );
_LIT8( KAuthHeaderNonceCount,                               "nc=" );
_LIT8( KAuthHeaderQop,                                      "qop=" );

const TText8* const KAuthInfoParamArray[]                   = 
                                                            {
                                                            _S8( "Authentication-Info" ),
                                                            _S8( "Proxy-Authentication-Info" ),
                                                            _S8( "nextnonce" ),
                                                            _S8( "rspauth" ),
                                                            _S8( "cnonce" ),
                                                            _S8( "nc" ),
                                                            _S8( "qop" ),
                                                            };
                                                            
enum TAuthInfoParam                                         
                                                            {
                                                            ENfoAuthInfo = 0,
                                                            ENfoPrxAuthInfo,
                                                            ENfoNextnonce,
                                                            ENfoRspauth,
                                                            ENfoCnonce,
                                                            ENfoNc,
                                                            ENfoQop,
                                                            };
enum TXcapAuthQop
    {
    EXcapAuth = 0,
    EXcapAuthInt,
    EXcapAuthNone
    };
    
#endif      //__XCAPHTTPCONSTS__
            
// End of File
