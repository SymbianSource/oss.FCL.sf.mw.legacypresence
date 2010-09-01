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




#include "XdmNamespace.h"

// ----------------------------------------------------
// CXdmNamespace::CXdmNamespace
// 
// ----------------------------------------------------
//
CXdmNamespace::CXdmNamespace()
    {   
    }

// ----------------------------------------------------
// CXdmNamespace::NewL
// 
// ----------------------------------------------------
//
EXPORT_C CXdmNamespace* CXdmNamespace::NewL( const TDesC8& aUri, const TDesC8& aPrefix )
    {
    CXdmNamespace* self = new ( ELeave ) CXdmNamespace();
    CleanupStack::PushL( self );
    self->ConstructL( aUri, aPrefix );
    CleanupStack::Pop();
    return self;
    }
    
// ----------------------------------------------------
// CXdmNamespace::ConstructL
// 
// ----------------------------------------------------
//        
void CXdmNamespace::ConstructL( const TDesC8& aUri, const TDesC8& aPrefix )
    {
    if( aUri.Length() > 0 )
        {
        iUri = aUri.AllocL();
        iPrefix = aPrefix.AllocL();
        }
    else User::Leave( KErrArgument );
    }

// ----------------------------------------------------
// CXdmNamespace::~CXdmNamespace
// 
// ----------------------------------------------------
//	    
EXPORT_C CXdmNamespace::~CXdmNamespace()
    {
    delete iUri;
    delete iPrefix;
    }
    
// ----------------------------------------------------
// CXdmNamespace::Uri
// 
// ----------------------------------------------------
//	
EXPORT_C TPtrC8 CXdmNamespace::Uri() const
    {
    return iUri != NULL ? iUri->Des() : TPtrC8();
    }

// ----------------------------------------------------
// CXdmNamespace::Prefix
// 
// ----------------------------------------------------
//	
EXPORT_C TPtrC8 CXdmNamespace::Prefix() const
    {
    return iPrefix != NULL ? iPrefix->Des() : TPtrC8();
    }

	
