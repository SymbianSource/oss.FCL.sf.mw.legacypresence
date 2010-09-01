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
* Description: CXcapCacheEntryPropertyProperty from XcapCacheIndexProperty.cpp
*
*/




// INCLUDE FILES
#include "XcapCacheServer.h"
#include "XcapCacheEntryProperty.h"

// ----------------------------------------------------------
// CXcapCacheEntryProperty::CXcapCacheEntryProperty
// 
// ----------------------------------------------------------
//
CXcapCacheEntryProperty::CXcapCacheEntryProperty( TInt aPropertyName ) :
                                                  iPropertyName( aPropertyName )
    {
    }

// ----------------------------------------------------------
// CXcapCacheEntryProperty::NewL
// 
// ----------------------------------------------------------
//
CXcapCacheEntryProperty* CXcapCacheEntryProperty::NewL( TInt aPropertyName,
                                                        const TDesC8& aPropertyValue )
    {
    CXcapCacheEntryProperty* self = new( ELeave ) CXcapCacheEntryProperty( aPropertyName );
    CleanupStack::PushL( self );
    self->ConstructL( aPropertyValue );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXcapCacheEntryProperty::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapCacheEntryProperty::ConstructL( const TDesC8& aPropertyValue )
    {
    iPropertyValue8 = aPropertyValue.AllocL();
    iPropertyValue16 = HBufC::NewL( aPropertyValue.Length() );
    iPropertyValue16->Des().Copy( aPropertyValue );
    }

// ----------------------------------------------------------
// CXcapCacheEntryProperty::~CXcapCacheEntryProperty
// 
// ----------------------------------------------------------
//
CXcapCacheEntryProperty::~CXcapCacheEntryProperty()
    {
    delete iPropertyValue8;
    delete iPropertyValue16;
    }

// ----------------------------------------------------------
// CXcapCacheEntryProperty::ConstructL
// 
// ----------------------------------------------------------
//
TInt CXcapCacheEntryProperty::Name() const
    {
    return iPropertyName;
    }

// ----------------------------------------------------------
// CXcapCacheEntryProperty::ConstructL
// 
// ----------------------------------------------------------
//
TInt CXcapCacheEntryProperty::IntValue() const
    {
    return CXcapCacheServer::ConvertDesc( iPropertyValue8->Des() );
    }

// ----------------------------------------------------------
// CXcapCacheEntryProperty::ConstructL
// 
// ----------------------------------------------------------
//
TTime CXcapCacheEntryProperty::TimeValue() const
    {
    TTime ret;
    ret.Parse( iPropertyValue16->Des() );
    return ret;
    }

// ----------------------------------------------------------
// CXcapCacheEntryProperty::ConstructL
// 
// ----------------------------------------------------------
//
TPtrC CXcapCacheEntryProperty::DesValue16() const
    {
    return iPropertyValue16 != NULL ? iPropertyValue16->Des() : TPtrC();
    }

// ----------------------------------------------------------
// CXcapCacheEntryProperty::ConstructL
// 
// ----------------------------------------------------------
//
TPtrC8 CXcapCacheEntryProperty::DesValue8() const
    {
    return iPropertyValue8 != NULL ? iPropertyValue8->Des() : TPtrC8();
    }


