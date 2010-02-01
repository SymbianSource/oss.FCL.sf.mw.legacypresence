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
* Description: CLocalDirectoryEntry
*
*/




#ifndef __LOCALDIRECTORYENTRY__
#define __LOCALDIRECTORYENTRY__

#include <e32base.h>

//CLASS DECLARATION
NONSHARABLE_CLASS( CLocalDirectoryEntry ) : public CBase
    {
    public:
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        static CLocalDirectoryEntry* NewL( const TDesC& aFileName );

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        TPtrC EntryName() const;
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        TBool OfferEntryL( const TDesC& aEntryName );

        /**
        * Destructor.
        */      
        virtual ~CLocalDirectoryEntry();
        
    private:  //Methods
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        CLocalDirectoryEntry();
        
        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CXdmDocOperation*
        */     
        void ConstructL( const TDesC& aFilePath );
        
    private:  //Data
        
        HBufC*                      iXmlEntryName;
        HBufC*                      iTimeStampEntryName;
    };

#endif      //__LOCALDIRECTORYENTRY__
            
// End of File
