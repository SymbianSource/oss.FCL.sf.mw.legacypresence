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


#include <ximpidentity.h>
#include "presencepluginutility.h"
#include "presencelogger.h"

// -----------------------------------------------------------------------------
// TPresencePluginUtility::ResetAndDestroyIdentities
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TPresencePluginUtility::ResetAndDestroyIdentities( TAny* anArray )
	{
	DP_SDA("TPresencePluginUtility::ResetAndDestroyIdentities"); 	 
	
    RPointerArray<MXIMPIdentity>* array = 
        reinterpret_cast<RPointerArray<MXIMPIdentity>*>( anArray );
        
    if ( array )
        {
        DP_SDA("  ->  reset and destroy array items"); 	 
        array->ResetAndDestroy();
        DP_SDA("  ->  close array"); 	 
        array->Close();
        }
	DP_SDA("TPresencePluginUtility::ResetAndDestroyIdentities out"); 	 
	}
    
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
