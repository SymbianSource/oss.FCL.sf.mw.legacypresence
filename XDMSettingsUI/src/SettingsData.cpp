/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    XDM GS plugin data holding class for XDM setting list
*
*/




// INCLUDE FILES
#include "SettingsData.h"

// -----------------------------------------------------------------------------
// CSettingsData::NewL
// -----------------------------------------------------------------------------
//
CSettingsData *CSettingsData::NewL()
    {
    CSettingsData *self = CSettingsData::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSettingsData::NewLC()
// -----------------------------------------------------------------------------
//
CSettingsData *CSettingsData::NewLC()
    {
    CSettingsData *self = new (ELeave) CSettingsData();
    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// CSettingsData::~CSettingsData()
// -----------------------------------------------------------------------------
//
CSettingsData::~CSettingsData()
    {
    }

// -----------------------------------------------------------------------------
// CSettingsData::CSettingsData()
// -----------------------------------------------------------------------------
//
CSettingsData::CSettingsData()
    {
    // initialise local data
    Reset();
    }

// -----------------------------------------------------------------------------
// CSettingsData::ConstructL() 
// -----------------------------------------------------------------------------
//
void CSettingsData::ConstructL() 
    {
    }

// -----------------------------------------------------------------------------
// CSettingsData::Reset()
// -----------------------------------------------------------------------------
//
void CSettingsData::Reset()
    {
    iSettingName = KNullDesC;
    iSettingNameDisp = KNullDesC;
    iServerAddress = KNullDesC;
    iAccessPoint = -1;
    iAccessPointDes = KNullDesC;
    iSipURL = KNullDesC;
    iUserID= KNullDesC;
    iPassword = KNullDesC;
    iAccessPointName = KNullDesC;
    iSettingId = -1;    
    }

//  End of File

