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
* Description: XcapEngine literals
*
*/




#ifndef __XCAPENGINEDEFINES__
#define __XCAPENGINEDEFINES__

#include <e32base.h>

//Xdm specific defines 
const TInt KXcapPathSeparatorChar                      = 47;

//XCAP specific defines
_LIT8( KXCAPEquality,                                  "="  );
_LIT8( KXCAPNodeSeparator,                             "~~" );
_LIT8( KXCAPPathSeparator,                             "/"  );
_LIT8( KXCAPAttributeIdentifier,                       "@"  );
_LIT8( KHTTPBracketOpen,                               "[");
_LIT8( KHTTPBracketClose,                              "]");
_LIT8( KHTTPQuotationMark,                             "\"");

//Escape coded HTTP specific defines
_LIT8( KHTTPBracketOpenEsc,                            "%5b");
_LIT8( KHTTPBracketCloseEsc,                           "%5d");
_LIT8( KHTTPQuotationMarkEsc,                          "%22");

//XML specific defines
_LIT8( KXmlSpace,                                      " ");
#endif    //__XCAPENGINEDEFINES__
            
// End of File
