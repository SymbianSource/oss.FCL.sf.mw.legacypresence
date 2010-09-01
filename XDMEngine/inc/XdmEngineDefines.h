/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   XDM Engine defines
*
*/




#ifndef __XdmENGINEDEFINES__
#define __XdmENGINEDEFINES__

#include <e32base.h>

//General defines
_LIT8( KGeneralEqualsAndQuotes,                        "=\""  );
_LIT8( KGeneralQuotes,                                 "\""  );

//Xdm specific defines 
const TInt KXdmPathSeparator                           = 47;

//XCAP specific defines
_LIT8( KXCAPEquality,                                  "="  );
_LIT8( KXCAPNodeSeparator,                             "~~" );
_LIT8( KXCAPPathSeparator,                             "/"  );
_LIT8( KXCAPAttributeIdentifier,                       "@"  );

//HTTP specific defines
_LIT8( KHTTPAngleBracketOpen,                          "%5b");
_LIT8( KHTTPAngleBracketClose,                         "%5d");
_LIT8( KHTTPAngleQuotationMark,                        "%22");

//XML specific defines
_LIT8( KXmlSpace,                                      " ");
#endif    //__XdmENGINEDEFINES__
            
// End of File
