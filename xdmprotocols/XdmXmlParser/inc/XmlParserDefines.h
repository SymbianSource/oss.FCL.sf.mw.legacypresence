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
* Description:   XmlParserDefines
*
*/




#ifndef __XMLPARSERDEFINES__
#define __XMLPARSERDEFINES__

//  INCLUDES
#include <e32base.h>
        
_LIT8( KEquelsSignString,                               "=" );
_LIT8( KQuotationSignString,                            "\"" );
_LIT8( KSlashString,                                    "/" );
_LIT8( KCommentString,                                  "<!--" );
_LIT8( KEmptyElementString,                             "/>" );
_LIT8( KStartBracketString,                             "<" );
_LIT8( KEndBracketString,                               ">" );
_LIT8( KXmlNamespaceString,                             " xmlns" );
_LIT8( KPrefixSeparatorString,                          ":" );
_LIT8( KNewlineString,                                  "\r\n" );

const TInt KStartBracket                                = 60;
const TInt KEndBracket                                  = 62;
const TInt KSpace                                       = 32;
const TInt KEquality                                    = 61;
const TInt KQuotation                                   = 34;

#endif      //__XMLPARSERDEFINES__  
            
// End of File
