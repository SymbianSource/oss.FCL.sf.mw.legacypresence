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
* Description:   XDM Engine protocol info
*
*/



#include "XdmProtocol.h"
#include "XdmDocument.h"
#include "XdmSettingsApi.h"
#include "XdmProtocolInfo.h"
#include "XdmProtocolUidList.h"
#include "XdmSettingsProperty.h"
#include "XdmSettingsCollection.h"

// ----------------------------------------------------------
// CXdmProtocolInfo::CXdmProtocolInfo
// 
// ----------------------------------------------------------
//
CXdmProtocolInfo::CXdmProtocolInfo() : iSecure( EFalse ),
                                       iUseCache( ETrue )
    {   
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::CXdmProtocolInfo
// 
// ----------------------------------------------------------
//
CXdmProtocolInfo::CXdmProtocolInfo( TInt aProtocolUid ) :
                                    iProtocolUid( TUid::Uid( aProtocolUid ) ),
                                    iSecure( EFalse ),
                                    iUseCache( ETrue )
    {   
    }
       
// ----------------------------------------------------------
// CXdmProtocolInfo::CXdmProtocolInfo
// 
// ----------------------------------------------------------
//
CXdmProtocolInfo::CXdmProtocolInfo( TInt aAccessPoint,
                                    TInt aProtocolUid ) :
                                    iProtocolUid( TUid::Uid( aProtocolUid ) ),
                                    iAccessPoint( aAccessPoint ),
                                    iSecure( EFalse ),
                                    iUseCache( ETrue )
    {   
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::NewL
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmProtocolInfo* CXdmProtocolInfo::NewL( TInt aAccessPoint,
                                                   TInt aProtocolUid,
                                                   const TDesC& aRootLocation )
    {
    CXdmProtocolInfo* self = new ( ELeave ) CXdmProtocolInfo( aAccessPoint, aProtocolUid );
    CleanupStack::PushL( self );
    self->ConstructL( aRootLocation );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::NewL
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmProtocolInfo* CXdmProtocolInfo::NewL( TInt aSettingsId )
    {
    CXdmProtocolInfo* self = new ( ELeave ) CXdmProtocolInfo( KXcapProtocol );
    self->Construct( aSettingsId );
    return self;
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::ConstructL
// 
// ----------------------------------------------------------
//
void CXdmProtocolInfo::ConstructL( const TDesC& aRootLocation )
    {
    delete iRootLocation;
    iRootLocation = NULL;
    iRootLocation = HBufC::NewL( aRootLocation.Length() );
    iRootLocation->Des().Copy( aRootLocation );
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::Construct
// 
// ----------------------------------------------------------
//
void CXdmProtocolInfo::Construct( TInt aSettingsId )
    {
    iSettingsId = aSettingsId;
    }
        
// ----------------------------------------------------------
// CXdmProtocolInfo::~CXdmProtocolInfo
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmProtocolInfo::~CXdmProtocolInfo()
    {
    delete iRootLocation;
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::SetCredentials
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmProtocolInfo::SetCredentials( const TXdmCredentials aCredentials )
    {
    iCredentials = aCredentials;
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::SetCacheUsage
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmProtocolInfo::SetCacheUsage( const TBool aCacheUsage )
    {
    iUseCache = aCacheUsage;
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::CacheUsage
// 
// ----------------------------------------------------------
//
EXPORT_C TBool CXdmProtocolInfo::IsCacheEnabled() const
    {
    return iUseCache;
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::Credentials
// 
// ----------------------------------------------------------
//
EXPORT_C const TXdmCredentials& CXdmProtocolInfo::Credentials() const
    {
    return iCredentials;
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::SetCredentials
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmProtocolInfo::SetSecurity( const TBool aSecure )
    {
    iSecure = aSecure;
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::SetCredentials
// 
// ----------------------------------------------------------
//
EXPORT_C TBool CXdmProtocolInfo::IsSecure() const
    {
    return iSecure;
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::Root
// 
// ----------------------------------------------------------
//
EXPORT_C TUid CXdmProtocolInfo::ProtocolUid() const
    {
    return iProtocolUid;
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::AccessPoint
// 
// ----------------------------------------------------------
//
EXPORT_C TInt CXdmProtocolInfo::AccessPoint() const
    {
    return iAccessPoint;
    }

// ----------------------------------------------------------
// CXdmProtocolInfo::AccessPoint
// 
// ----------------------------------------------------------
//
EXPORT_C TInt CXdmProtocolInfo::SettingsID() const
    {
    return iSettingsId;
    }
    
// ----------------------------------------------------------
// CXdmProtocolInfo::Root
// 
// ----------------------------------------------------------
//
EXPORT_C TPtrC CXdmProtocolInfo::Root() const
    {
    return iRootLocation != NULL ? iRootLocation->Des() : TPtrC();
    }

              
// End of File


