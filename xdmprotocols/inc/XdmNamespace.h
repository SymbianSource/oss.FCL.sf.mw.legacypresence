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
* Description: CXdmNamespace
*
*/




#ifndef __XDMNAMESPACE__
#define __XDMNAMESPACE__

#include <e32base.h>

NONSHARABLE_CLASS ( CXdmNamespace ) : public CBase
	{	
    public:
    
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
	    IMPORT_C static CXdmNamespace* NewL( const TDesC8& aUri, const TDesC8& aPrefix );
	
	    /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
	    IMPORT_C TPtrC8 Uri() const;
	
	    /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
	    IMPORT_C TPtrC8 Prefix() const;
	    
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        IMPORT_C virtual ~CXdmNamespace();
	
    private:
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        CXdmNamespace();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
	    void ConstructL( const TDesC8& aUri, const TDesC8& aPrefix );
	    
	private:
	
		HBufC8*                 iUri;
		HBufC8*                 iPrefix;
	};
	
#endif	
