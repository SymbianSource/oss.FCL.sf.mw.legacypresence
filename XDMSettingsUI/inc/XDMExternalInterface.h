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
* Description:    Header file with UID definitions
 *
*/



#ifndef XDMEXTERNALINTERFACE_H
#define XDMEXTERNALINTERFACE_H

#include <e32std.h>
#include <bldvariant.hrh>
#include <XDMPluginRsc.rsg>
#include "XDMPlugin.h"
#define XDM_PLUGIN

/** 
* This UID is used for both the view UID and the ECOM plugin implementation 
* UID.
*/
const TUid KGSXDMPluginUid = { 0x10207429 };

// View UIDs
const TUid KViewId= {0x1028236B};

#endif // XDMEXTERNALINTERFACE_H

// End of File
