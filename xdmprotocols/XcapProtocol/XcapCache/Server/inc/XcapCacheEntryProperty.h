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
* Description:   CXcapCacheEntryProperty
*
*/




#ifndef __XCAPCACHEINDEXPROPERTY_H__
#define __XCAPCACHEINDEXPROPERTY_H__

// INCLUDES
#include <e32std.h>
#include "CommonDefines.h"

//FORWARD DECLARATIONs

// CLASS DECLARATION
NONSHARABLE_CLASS( CXcapCacheEntryProperty ) : public CBase                         
    {
    public:             // Constructors and destructor
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        static CXcapCacheEntryProperty* NewL( TInt aPropertyName,
                                              const TDesC8& aPropertyValue );
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TInt IntValue() const;
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TTime TimeValue() const;
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TPtrC DesValue16() const;
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TPtrC8 DesValue8() const;

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TInt Name() const;

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        ~CXcapCacheEntryProperty();

    private:
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void ConstructL( const TDesC8& aPropertyValue );

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        CXcapCacheEntryProperty( TInt aPropertyName );

    private:            // Data
        
        HBufC*                             	iPropertyValue16;
        HBufC8*                             iPropertyValue8;
        const TInt                          iPropertyName;
    };

#endif

// End of File
