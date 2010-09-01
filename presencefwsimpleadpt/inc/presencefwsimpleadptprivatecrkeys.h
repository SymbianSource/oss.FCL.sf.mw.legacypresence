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
* Description:  SIMPLE Protocol implementation for Presence Framework
*
*/



#ifndef PRESENCEFWSIMPLEADPTPRIVATECRKEYS_H
#define PRESENCEFWSIMPLEADPTPRIVATECRKEYS_H

#include <e32std.h>

const TUid KCRUIDPresencefwSimpleadptVariation = {0x10282C84};

// Id for rule to grant all elements
const TUint32 KPrFwSimpleGrantAllRuleName  = 0x00000001;
// Id for rule to grant own data
const TUint32 KPrFwSimpleGrantOwnRuleName  = 0x00000002;
// Id for default rule
const TUint32 KPrFwSimpleDefaultRuleName  = 0x00000003;
// Id for block rule
const TUint32 KPrFwSimpleBlockRuleName  = 0x00000004;
// Default rule action, allow/confirm/block
const TUint32 KPrFwSimpleDefaultRuleAction = 0x00000005;
// Block rule action, block/polit
const TUint32 KPrFwSimpleBlockRuleAction = 0x00000006;
// Template for RLS service URI for a user group
const TUint32 KPrFwSimpleRlsGroupUriTemplate = 0x00000007;
// Top level for lists in shared XDM index document
const TUint32 KPrFwSimpleSharedXdmTop = 0x00000008;
// Name of the Shared XDM list for granted users
const TUint32 KPrFwSimpleGrantListName = 0x00000009;

#endif      // PRESENCEFWSIMPLEADPTPRIVATECRKEYS_H

