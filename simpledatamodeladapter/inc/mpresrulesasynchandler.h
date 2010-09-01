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


#ifndef __MPRESRULESASYNCHANDLER_H__
#define __MPRESRULESASYNCHANDLER_H__

class CXdmDocument;
// Class declaration

NONSHARABLE_CLASS( MPresRulesAsyncHandler )
    {
    public:

       /**
        * This method handles the async. 
        * call to function CPresenceXDM::UpdateToServerL
        * and CPresenceXDM::UpdateAllFromServerL. 
        * Clients should implement this method to get a callback.
        *
        * @param aErrorCode, XDMengine error codes
        */
       virtual void HandlePresUpdateDocumentL(
           TInt aErrorCode ) = 0;

    };
    
#endif //__MPRESRULESASYNCHANDLER_H__