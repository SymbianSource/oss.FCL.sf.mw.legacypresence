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
* Description:   MXmlNodePathInterface
*
*/




#ifndef __XMLNODEPATHINTERFACE__
#define __XMLNODEPATHINTERFACE__

// INCLUDES
#include <e32base.h>

class CXdmNodeAttribute;

// CLASS DECLARATION
NONSHARABLE_CLASS( MXmlNodePathInterface )
    {
    public: 
        
        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        virtual CXdmNodeAttribute* TargetAttribute() const = 0;
    };

#endif  //__XMLPARSERNODEPATH__


// End of File
