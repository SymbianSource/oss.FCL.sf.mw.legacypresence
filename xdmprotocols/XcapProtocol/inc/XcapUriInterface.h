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
* Description: MXcapUriInterface
*
*/




#ifndef __XCAPURIINTERFACE__
#define __XCAPURIINTERFACE__

#include <e32base.h>

//CLASS DECLARATION
NONSHARABLE_CLASS( MXcapUriInterface )
    {
    public:
        
        /**
        * Add new namespace mapping
        * @param TDesC8& Namespace URI
        * @param TDesC8& Namespace prefix
        * @return void
        */      
        virtual void AddNamespaceMappingL( const TDesC8& aUri, const TDesC8& aPrefix ) = 0; 
    };
    
    

#endif      //__XCAPURIINTERFACE__
            
// End of File
