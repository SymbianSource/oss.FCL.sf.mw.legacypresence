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
* Description: CXcapNodeSelector
*
*/




// INCLUDES
//This is only for logging
#include "XcapProtocol.h"
#include "XcapNodeSelector.h"

// ---------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CXcapNodeSelector::CXcapNodeSelector()                                    
    {
    }

// ---------------------------------------------------------
// Two-phased constructor.
//
// ---------------------------------------------------------
//
CXcapNodeSelector* CXcapNodeSelector::NewL()
    {
    CXcapNodeSelector* self = new ( ELeave ) CXcapNodeSelector();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// Symbian OS default constructor may leave.
//
// ---------------------------------------------------------
//
void CXcapNodeSelector::ConstructL()
    {
    
    }

// ---------------------------------------------------------
// Destructor
//
// ---------------------------------------------------------
//
CXcapNodeSelector::~CXcapNodeSelector()
    {
    #ifdef _DEBUG
        CXcapProtocol::WriteToLog( _L8( "CXcapNodeSelector::~CXcapNodeSelector()" ) );  
    #endif
    delete iSelectorString;
    }

//  End of File  

