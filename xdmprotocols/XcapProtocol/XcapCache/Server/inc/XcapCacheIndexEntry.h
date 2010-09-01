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
* Description:   CXcapCacheIndexEntry
*
*/




#ifndef __XCAPCACHEINDEXENTRY_H__
#define __XCAPCACHEINDEXENTRY_H__

// INCLUDES
#include <e32std.h>
#include <s32file.h>
//#include "XcapCacheClient.h"

//CONSTANTS
const TUint KCacheEntryETag                               = 0;
const TUint KCacheXmlSize                                 = 1;
const TUint KCacheXmlFile                                 = 2;
const TUint KCacheLastAccess                              = 3;
const TUint KCacheLastModified                            = 4;
 
//ENUMS
enum TCacheIndexEntry
    {
    EEntryETag                                            = KCacheEntryETag,
    EEntryXmlSize                                         = KCacheXmlSize,
    EEntryXmlFile                                         = KCacheXmlFile,
    EEntryLastAccess                                      = KCacheLastAccess,
    EEntryLastModified                                    = KCacheLastModified
    };

//Specifies the cache file "protocol"
const TText8* const KCacheEntryArray[] = {                _S8( "etag" ),
                                                          _S8( "size" ),
                                                          _S8( "file" ),
                                                          _S8( "access" ),
                                                          _S8( "modify" )
                                         };

//FORWARD DECLARATIONs
class CXcapCacheEntryProperty;

// CLASS DECLARATION
NONSHARABLE_CLASS( CXcapCacheIndexEntry ) : public CBase                         
    {
    public:             // Constructors and destructor
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        static CXcapCacheIndexEntry* NewL( const TCacheEntryInfo* aHeader );

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        static CXcapCacheIndexEntry* NewL( TPtr8& aEntryData );
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void UpdateEntryL( const TCacheEntryInfo* aHeader );

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TTime LastAccess() const;

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TTime LastModified() const;

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TInt XmlSize() const;
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TPtrC8 ETag() const;

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TPtrC8 FileNameL() const;
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TPtrC FileName16L() const;

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        HBufC8* PrintLC() const;
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        ~CXcapCacheIndexEntry();

    private:
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void ConstructL( const TCacheEntryInfo* aHeader );

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void ConstructL( TPtr8& aEntryData );
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        CXcapCacheIndexEntry();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        CXcapCacheEntryProperty* CreatePropertyL( const TInt aPropertyName,
                                                  const TDesC8& aPropertyValue );
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void CreateFromHeaderL( const TCacheEntryInfo* aHeader );
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        const CXcapCacheEntryProperty& FindProperty( const TInt aPropId ) const;

        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        TInt FindPropNameId( const TDesC8& aNameString ) const;
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        HBufC8* HourStringLC();
        
        /**
        * Returns the MIME type of the messages
        * @return TPtrC8 The MIME type of the messages
        */
        void DeleteFileL( const TDesC8& aFileName );

        /**
        * Second-phase constructor.
        */ 
        HBufC8* StoreXmlFileLC( const TDesC8& aXmlData );

    private:            // Data
        
        RPointerArray<CXcapCacheEntryProperty>          iPropertyList;
    };

#endif

// End of File
