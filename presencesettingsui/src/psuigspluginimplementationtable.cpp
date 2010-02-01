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
* Description:   
*        ECOM proxy table for this plugin
*
*/




// System includes
#include <e32std.h>
#include <implementationproxy.h>
#include "psuigspluginids.hrh"
#include "psuigsplugin.h"

// Constants
const TImplementationProxy PSUIGSPluginImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( PSUIGSPluginImplUid, CPSUIGSPlugin::NewL )
    };

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// Gate/factory function
//
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof( PSUIGSPluginImplementationTable ) 
        / sizeof( TImplementationProxy );
    return PSUIGSPluginImplementationTable;
    }

// End of File
