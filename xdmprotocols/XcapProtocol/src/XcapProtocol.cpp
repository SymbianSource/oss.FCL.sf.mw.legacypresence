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
* Description: CXcapProtocol
*
*/



#include <hal.h>
#include <f32file.h>
#include <flogger.h>
#include <XdmSettingsApi.h>
#include <XdmSettingsProperty.h>
#include <XdmSettingsCollection.h>
#include <implementationproxy.h>
#include <msgconnmanagerapi.h>
#include "XcapAppUsage.h"
#include "XcapEarlyIms.h"
#include "XcapProtocol.h"
#include "XcapDocument.h"
#include "xdmlogwriter.h"
#include "XcapDocumentNode.h"
#include "XcapDirectory.h"
#include "XdmXmlParser.h"
#include "XdmCredentials.h"
#include "XcapHttpReqGet.h"
#include "XdmProtocolInfo.h"
#include "XdmShutdownTimer.h"
#include "XcapHttpTransport.h"
#include "XdmOperationFactory.h"

// ----------------------------------------------------------
// CXcapProtocol::CXcapProtocol
// 
// ----------------------------------------------------------
//
CXcapProtocol::CXcapProtocol() : iSuspend( EFalse ),
                                 iCacheUsage( EFalse ),
                                 iTransferMediaOpen( EFalse ),
                                 iSimRequestPending( EFalse ),
                                 iAuthType( EXcapAuthHttpDigest )
    {   
    }
        
// ----------------------------------------------------------
// CXcapProtocol::DeleteLogFileL
// 
// ----------------------------------------------------------
//
CXcapProtocol* CXcapProtocol::NewL( const TXdmProtocolParams& aProtocolParams )
    {
    CXcapProtocol* self = new ( ELeave ) CXcapProtocol();
    CleanupStack::PushL( self );
    self->ConstructL( aProtocolParams );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXcapProtocol::DeleteLogFileL
// 
// ----------------------------------------------------------
//
CXcapProtocol::~CXcapProtocol()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapProtocol::~CXcapProtocol()" ) );  
    #endif
    delete iRootUri;
    delete iPublicId;
    delete iXmlParser;
    delete iCapabilities;
    delete iImsResolver;
    delete iHttpTransport;
    delete iIdleTimer;
    if( iCacheUsage )
        {
        iCacheClient.Flush();
        #ifdef _DEBUG
            WriteToLog( _L8( "  Cache flushed" ) );
        #endif
        iCacheClient.Close();
        #ifdef _DEBUG
            WriteToLog( _L8( "  Cache closed" ) );
        #endif
        }
    if( iConnectionManager != NULL )
        iConnectionManager->Destroy();
    #ifdef _DEBUG
        WriteToLog( _L8( "  ConnMan destroyed" ) );
    #endif
    #ifdef _DEBUG
        WriteToLog( _L8( "  All finished, about to delete file logger" ) );
    #endif
    delete iLogWriter;
    iNotifyeeQueue.Close();
    }
 
// ----------------------------------------------------------
// CXcapProtocol::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapProtocol::ConstructL( const TXdmProtocolParams& aProtocolParams )
    {
    #ifdef _DEBUG
        iLogWriter = CXdmLogWriter::NewL( KXcapEngLogFile );
        WriteToLog( _L8( "CXcapProtocol::ConstructL()" ) );  
    #endif
    iXdmEngine = CONST_CAST( CXdmEngine*, &aProtocolParams.iXdmEngine );
    iProtocolInfo = &aProtocolParams.iProtocolInfo;
    TInt error = SaveSettingsL();
    if( error == KErrNone )
        {
        iXmlParser = CXdmXmlParser::NewL();
        iIdleTimer = new ( ELeave ) CXdmShutdownTimer( *this, this );
        iIdleTimer->ConstructL();
        if( iProtocolInfo->IsCacheEnabled() )
            {
            iCacheUsage = ETrue;
            TInt error = iCacheClient.Connect();
            #ifdef _DEBUG
                WriteToLog( _L8( "  Cache start: %d" ), error );  
            #endif
            __ASSERT_DEBUG( ( error == KErrNone || error == KErrAlreadyExists ),
                              User::Panic( _L( "CXcapProtocol" ), 1 ) );
            }
        iUserName.Copy( iCredentials.iUserName );
        iConnectionManager = NewMsgConnManagerL( iAccessPoint );
        __ASSERT_DEBUG( iRootUri->Des().Length() > 0, User::Panic( _L( "CXcapProtocol" ), 1 ) );
        TBool secure = iProtocolInfo->IsSecure() || iRootUri->Des().FindF( KHttps ) == 0;
        HBufC* root = ParseRootLocationLC( secure, iRootUri->Des() );
        __ASSERT_ALWAYS( root != NULL, User::Panic( _L( "CXcapProtocol" ), 1 ) );
        iHttpTransport = CXcapHttpTransport::NewL( root->Des(), *iConnectionManager, iCredentials );
        CleanupStack::PopAndDestroy();  //root   
        }
    else User::Leave( error );
    }

// ----------------------------------------------------------
// CXcapProtocol::PublicID
// 
// ----------------------------------------------------------
//
EXPORT_C TPtrC8 CXcapProtocol::PublicID()
    {
    return iPublicId != NULL ? iPublicId->Des() : TPtrC8();  
    }

// ----------------------------------------------------------
// CXcapProtocol::Cache
// 
// ----------------------------------------------------------
//
EXPORT_C RXcapCache* CXcapProtocol::Cache()
    {
    return iCacheUsage ? &iCacheClient : NULL;  
    }

// ---------------------------------------------------------
// CXcapProtocol::Parser
// 
// ---------------------------------------------------------
//
EXPORT_C CXdmXmlParser& CXcapProtocol::Parser()
    {
    return *iXmlParser;
    }

// ---------------------------------------------------------
// CXcapProtocol::Transport
// 
// ---------------------------------------------------------
//
EXPORT_C CXcapHttpTransport& CXcapProtocol::Transport()
    {
    return *iHttpTransport;
    }
   
// ----------------------------------------------------------
// CXcapProtocol::IsNetworkAvailable
// 
// ----------------------------------------------------------
//
EXPORT_C TBool CXcapProtocol::IsNetworkAvailable()
    {
    return iConnectionManager->Status();
    } 
       
// ----------------------------------------------------------
// CXcapProtocol::SaveSettingsL
// 
// ----------------------------------------------------------
//
TInt CXcapProtocol::SaveSettingsL()
    {
    TInt settingsId = iProtocolInfo->SettingsID();
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapProtocol::SaveSettingsL() - ID: %d" ), settingsId );
    #endif
    CXdmSettingsCollection* settings = NULL;
    TRAPD( error, settings = TXdmSettingsApi::SettingsCollectionL( settingsId ) );
    if( error == KErrNone && settings != NULL )
        {
        CleanupStack::PushL( settings );
        SaveFromCollectionL( *settings );
        CleanupStack::PopAndDestroy();  //settings
        }
    else   //Default to HTTP-DIGEST
        {
        iAuthType = EXcapAuthHttpDigest;
        iAccessPoint = iProtocolInfo->AccessPoint();
        iCredentials = iProtocolInfo->Credentials();
        iRootUri = iProtocolInfo->Root().AllocL();
        error = iRootUri->Des().Length() > 0 &&
                iCredentials.iUserName.Length() > 0 &&
                iCredentials.iPassword.Length() > 0 ?
                KErrNone : KErrArgument;
        }
    return error;
    }

// ----------------------------------------------------------
// CXcapProtocol::SelectAuthType
// 
// ----------------------------------------------------------
//
void CXcapProtocol::SelectAuthTypeL( const CXdmSettingsCollection& aSettings )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapProtocol::SelectAuthTypeL()" ) );  
    #endif
    TPtrC authType( aSettings.Property( EXdmPropAuthType ) );
    if( authType.Compare( KXdmAuthGaa ) == 0 ) 
        {
        iAuthType = EXcapAuthGaa;
        #ifdef _DEBUG
            CXcapProtocol::WriteToLog( _L8( " GAA" ) );
        #endif
        }
    else if( authType.Compare( KXdmAuthEarlyIms ) == 0 )
        {
        #ifndef __WINSCW__ 
            iAuthType = EXcapAuthEarlyIms;
            ResolveIdentityL();
            #ifdef _DEBUG
                CXcapProtocol::WriteToLog( _L8( " EARLY-IMS" ) );
            #endif
        #endif
        #ifdef __FAKE_IMPU__
            iAuthType = EXcapAuthEarlyIms;
            ResolveIdentityL();
            #ifdef _DEBUG
                CXcapProtocol::WriteToLog( _L8( " EARLY-IMS" ) );
            #endif
        #else
            #ifdef _DEBUG
                CXcapProtocol::WriteToLog( _L8( " WINS, cannot authenticate without SIM card => ignore" ) );
            #endif
            iAuthType = EXcapAuthHttpDigest;
        #endif   
        }
    else
        {
        iAuthType = EXcapAuthHttpDigest;
        #ifdef _DEBUG
            CXcapProtocol::WriteToLog( _L8( " HTTP-DIGEST" ) );
        #endif
        } 
    //Fallback position: in case the server challenges us even though EXcapAuthEarlyIms
    //has been defined, use the credentials the client supplied regardless of what they are.
    TPtrC authName( aSettings.Property( EXdmPropAuthName ) );
    TPtrC authSecret( aSettings.Property( EXdmPropAuthSecret ) );
    //But they must not exceed the stipulated length
    __ASSERT_ALWAYS( authName.Length() <= KXdmMaxUserNameLength, User::Leave( KErrCorrupt ) );
    __ASSERT_ALWAYS( authSecret.Length() <= KXdmMaxPasswordLength, User::Leave( KErrCorrupt ) );
    iCredentials = TXdmCredentials( authName, authSecret );
    }

// ----------------------------------------------------------
// CXcapProtocol::SaveFromCollectionL
// 
// ----------------------------------------------------------
//
void CXcapProtocol::SaveFromCollectionL( const CXdmSettingsCollection& aSettings )
    {
    #ifdef _DEBUG
        for( TInt i = 0;i < aSettings.Count();i++ )
            {
            const CXdmSettingsProperty& prop = aSettings.Property( i );
            HBufC8* value = HBufC8::NewLC( prop.PropertyValue().Length() );
            value->Des().Copy( prop.PropertyValue() );
            TPtrC8 valueDes( value->Des() );
            WriteToLog( _L8( "  Property %d - Name: %d  Value: %S" ), i, prop.PropertyName(), &valueDes );
            CleanupStack::PopAndDestroy();  //value
            }
    #endif
    SelectAuthTypeL( aSettings );
    __ASSERT_DEBUG( aSettings.Property( EXdmPropToNapId ).Length() > 0, User::Leave( KErrCorrupt ) );
    __ASSERT_DEBUG( aSettings.Property( EXdmPropUri ).Length() > 0, User::Leave( KErrCorrupt ) );
    TLex accessPoint( aSettings.Property( EXdmPropToNapId ) );
    TInt error = accessPoint.Val( iAccessPoint );
    __ASSERT_DEBUG( error == KErrNone, User::Leave( KErrCorrupt ) );
    iRootUri = aSettings.Property( EXdmPropUri ).AllocL();
    #ifdef _DEBUG
        TBuf8<KXdmMaxUserNameLength> name( iCredentials.iUserName );
        TBuf8<KXdmMaxPasswordLength> password( iCredentials.iPassword );
        HBufC8* rootUri = HBufC8::NewLC( iRootUri->Des().Length() );
        rootUri->Des().Copy( iRootUri->Des() );
        TPtrC8 uriDes( rootUri->Des() );
        WriteToLog( _L8( " Finished - Error:    %d" ), error );
        WriteToLog( _L8( "  Access point:       %d" ), iAccessPoint );
        WriteToLog( _L8( "  URI:                %S" ), &uriDes );
        WriteToLog( _L8( "  User name:          %S" ), &name );
        WriteToLog( _L8( "  Password:           %S" ), &password );
        CleanupStack::PopAndDestroy();  //rootUri
    #endif
    iUserName.Copy( iCredentials.iUserName );
    }
    
// ----------------------------------------------------------
// CXcapProtocol::ParseRootLocationLC
// 
// ----------------------------------------------------------
//
HBufC* CXcapProtocol::ParseRootLocationLC( const TBool aSecurity,
                                           const TDesC& aRootLocation )
    {
    #ifdef _DEBUG
        TBuf8<1024> root;
        root.Copy( aRootLocation );
        WriteToLog( _L8( "CXcapProtocol::ParseRootLocationLC()" ) );
        WriteToLog( _L8( "  Security:     %d" ), aSecurity );
        WriteToLog( _L8( "  Provided URI: %S" ), &root );
    #endif
    HBufC* ret = NULL;
    HBufC* temp = CheckProtocolPrefixL( aRootLocation );
    if( temp != NULL )
        {
        CleanupStack::PushL( temp );
        ret = aSecurity ? ConstructSecureUriL( temp->Des() ) :
                          ConstructStandardUriL( temp->Des() );
        CleanupStack::PopAndDestroy();  //temp                       
        }
    else
        ret = aSecurity ? ConstructSecureUriL( aRootLocation ) :
                          ConstructStandardUriL( aRootLocation );
	TPtr desc( ret->Des() );
	if( desc[desc.Length() - 1] == '/' )
    	desc.Delete( desc.Length() - 1, 1 );
    CleanupStack::PushL( ret );
    return ret;
    }

// ----------------------------------------------------------
// CXcapProtocol::CheckProtocolPrefixLC
// 
// ----------------------------------------------------------
//
HBufC* CXcapProtocol::CheckProtocolPrefixL( const TDesC& aRootLocation )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapProtocol::CheckProtocolPrefixL()" ) );  
    #endif
    HBufC* buffer = NULL;
    if( aRootLocation.FindF( KHttp ) == 0 || aRootLocation.FindF( KHttps ) == 0 )
        {
        TInt length = aRootLocation.FindF( KHttp ) == 0 ?
                      KHttp().Length() : KHttps().Length();
        buffer = HBufC::NewL( aRootLocation.Length() - length );
        TPtrC desc( aRootLocation );
        TPtrC temp = desc.Right( aRootLocation.Length() - length );
        buffer->Des().Copy( temp );
        }  
    return buffer;
    }
    
// ----------------------------------------------------------
// CXcapProtocol::ConstructStandardUriL
// 
// ----------------------------------------------------------
//
HBufC* CXcapProtocol::ConstructStandardUriL( const TDesC& aBasicUri )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapProtocol::ConstructStandardUriLC()" ) );  
    #endif
    HBufC* ret = HBufC::NewL( aBasicUri.Length() +
                              TPtrC( KHttp ).Length() );
	TPtr desc( ret->Des() );
    desc.Copy( KHttp );
    desc.Append( aBasicUri );
    return ret;
    }
    
// ----------------------------------------------------------
// CXcapProtocol::ConstructSecureUriL
// 
// ----------------------------------------------------------
//
HBufC* CXcapProtocol::ConstructSecureUriL( const TDesC& aBasicUri )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapProtocol::ConstructSecureUriLC()" ) );  
    #endif
    TInt index = aBasicUri.FindF( KHostSeparator );
    HBufC* ret = HBufC::NewL( aBasicUri.Length() +
                        TPtrC( KHttps ).Length() );
    TPtr uriDesc( ret->Des() );
    if( index > 0 )
        {
        TPtrC basic( aBasicUri.Left( index ) );
        TPtrC theRest( aBasicUri.Mid( index ) );
        uriDesc.Copy( KHttps );
        uriDesc.Append( basic );
        uriDesc.Append( theRest );
        }
    else
        {
        uriDesc.Copy( KHttps );
        uriDesc.Append( aBasicUri );
        }
    return ret;
    }

// ----------------------------------------------------------
// CXcapProtocol::ResolveIdentityL
// 
// ----------------------------------------------------------
//
void CXcapProtocol::ResolveIdentityL()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapProtocol::ResolveIdentityL()" ) );  
    #endif
    iImsResolver = CXcapEarlyIms::NewL();
    iImsResolver->RequestSimDataL( this );
    iSimRequestPending = ETrue;
    }

// ---------------------------------------------------------
// CXcapProtocol::CheckActivity
//
// ---------------------------------------------------------
//
void CXcapProtocol::CheckActivity()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapProtocol::CheckActivity()" ) );
    #endif
    TBool activity = EFalse;
    const RPointerArray<CXdmDocument>& documents = iXdmEngine->DocumentCollection();
    const RPointerArray<CXdmDirectory>& directories = iXdmEngine->DirectoryCollection();
    TInt docCount = documents.Count();
    TInt dirCount = directories.Count();
    if( docCount > 0 || dirCount > 0 )
        {
        TInt i = 0;
        for( ;!activity && i < docCount;i++ )
            activity = documents[i]->IsActive();
        for( i = 0 && !activity;!activity && i < dirCount;i++ )
            activity = directories[i]->IsActive();
        }
    if( !activity )
        {
        //Just in case...
        iIdleTimer->Cancel();
        iIdleTimer->Start( iIdleTimeout * 1000000 );
        }
    }

// ---------------------------------------------------------
// CXcapProtocol::SimRequestPending
// 
// ---------------------------------------------------------
//
TBool CXcapProtocol::IsSimRequestPending( TInt& aError ) const
    {
    aError = iSimRequestPending ? KErrNotReady : iImsResolverError;
    return iSimRequestPending; 
    }

// ---------------------------------------------------------
// CXcapProtocol::AppendNotifyeeL
// 
// ---------------------------------------------------------
//
void CXcapProtocol::AppendNotifyeeL( CXcapDocument* aPendingDoc )
    {
    User::LeaveIfError( iNotifyeeQueue.Append( aPendingDoc ) );
    }

// ---------------------------------------------------------
// CXcapProtocol::CancelImsResolver
// 
// ---------------------------------------------------------
//
void CXcapProtocol::CancelImsResolver()
    {
    #ifdef _DEBUG
        CXcapProtocol::WriteToLog( _L8( "CXcapDocument::CancelImsResolver()" ) );
    #endif
    if( iImsResolver )
        iImsResolver->Cancel();
    }
       
// ---------------------------------------------------------
// CXcapDocument::RequestComplete
// 
// ---------------------------------------------------------
//
void CXcapProtocol::RequestComplete( TInt aError )
    {
    #ifdef _DEBUG
        CXcapProtocol::WriteToLog( _L8( "CXcapDocument::RequestComplete()" ) );
    #endif
    if( aError == KErrNone )
        {
        #ifdef _DEBUG
            CXcapProtocol::WriteToLog( _L8( " Public ID retrieved successfully" ) );
        #endif
        iPublicId = iImsResolver->PublicIDL().AllocL();
        delete iImsResolver;
        iImsResolver = NULL;
        }
    else
        {
        #ifdef _DEBUG
            CXcapProtocol::WriteToLog( _L8( " Public ID retrieval failed - Status: %d" ), aError );
        #endif
        iImsResolverError = aError;
        }
    for( TInt i = 0;i < iNotifyeeQueue.Count();i++ )
        {
        CXcapDocument* document = iNotifyeeQueue[i];
        if( document )
            document->NotifyResolverCompleteL( aError );
        }
    iSimRequestPending = EFalse;
    iNotifyeeQueue.Reset();
    }
        
// ----------------------------------------------------------
// CXcapProtocol::WriteToLog
// 
// ----------------------------------------------------------
//
void CXcapProtocol::WriteToLog( TRefByValue<const TDesC8> aFmt,... ) const                                 
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    iLogWriter->WriteToLog( buf );
    }

// ----------------------------------------------------
// CXcapProtocol::InitTransferMedium
// 
// ----------------------------------------------------
//
void CXcapProtocol::InitTransferMedium( TInt aIdleTimeout, TRequestStatus& aStatus )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapProtocol::InitTransferMedium()" ) );  
    #endif
    iIdleTimer->Cancel();
    iIdleTimeout = aIdleTimeout;
    if( !IsNetworkAvailable() )
        iConnectionManager->StartConnection( aStatus );
    else  //Already open
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNone );
        }
    }

// ----------------------------------------------------
// CXcapProtocol::CancelTransferMediumInit
// 
// ----------------------------------------------------
//
void CXcapProtocol::CancelTransferMediumInit()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapProtocol::CancelTransferMediumInit()" ) );  
    #endif
    TInt error = KErrNone;
    TRAP( error, iConnectionManager->CancelStartL() );
    #ifdef _DEBUG
        WriteToLog( _L8( " Transfer media cancel completed - Error: %d" ), error );  
    #endif
    //Suppress build warning
    error = KErrNone;
    }

// ---------------------------------------------------------
// CXcapProtocol::IsTransferAvailable
// 
// ---------------------------------------------------------
//
TBool CXcapProtocol::IsTransferAvailable() const
    {
    return iConnectionManager->Status();
    }
    
// ---------------------------------------------------------
// CXcapProtocol::AuthType
// 
// ---------------------------------------------------------
//
TXcapAuthType CXcapProtocol::AuthType() const
    {
    return iAuthType;
    }
             
// ---------------------------------------------------------
// CXdmEngine::XdmProtocol
// 
// ---------------------------------------------------------
//
void CXcapProtocol::SwitchOff()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapProtocol::SwitchOff()" ) );
    #endif
    iConnectionManager->StopConnection();
    }

// ----------------------------------------------------
// CXcapProtocol::CreateDocumentL
// 
// ----------------------------------------------------
//
CXdmDocument* CXcapProtocol::CreateDocumentL( const TDesC& aDocumentName,
                                              const TXdmDocType aDocumentType )
    {
    return CXcapDocument::NewL( aDocumentType, aDocumentName, *iXdmEngine, *this );
    }
    
// ----------------------------------------------------
// CXcapProtocol::CreateDirectoryL
//
// ----------------------------------------------------
//
CXdmDirectory* CXcapProtocol::CreateDirectoryL( const TDesC& aDirectoryPath )
    {
    _LIT( KDirectoryDocName, "directory.xml" );
    CXdmDocument* document = iXdmEngine->CreateDocumentModelL( KDirectoryDocName, EXdmDirectory );
    CleanupStack::PushL( document );
    CXdmDirectory* directory = CXcapDirectory::NewL( aDirectoryPath, *iXdmEngine, document, *this );
    CleanupStack::Pop();               //document
    return directory;
    }

// ----------------------------------------------------
// CXcapProtocol::CreateDocumentNodeL
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CXcapProtocol::CreateDocumentNodeL()
    {
    return CXcapDocumentNode::NewL( *iXdmEngine, *this );
    }
        
// ---------------------------------------------------------
// CXcapProtocol::GenerateUniqueIdL
// 
// ---------------------------------------------------------
//
TInt CXcapProtocol::GenerateUniqueIdL()
    {
    TInt period = 0;
    User::LeaveIfError( HAL::Get( HALData::ESystemTickPeriod, period ) );
    TInt millisecsPerTick = period / 1000;
    return User::TickCount() * millisecsPerTick;
    }

// ----------------------------------------------------------
// CXdmProtocol::UserName
// 
// ----------------------------------------------------------
//
TPtrC8 CXcapProtocol::UserName() const
    {
    return iUserName;
    }
        
// ----------------------------------------------------------
// CXdmProtocol::HandleBearerEventL
// 
// ----------------------------------------------------------
//
void CXcapProtocol::HandleBearerEventL( TBool aAuthoritativeClose,
                                        TMsgBearerEvent aBearerEvent )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapProtocol::HandleBearerEventL(): %d" ), aBearerEvent );
    #endif
    switch( aBearerEvent )
        {
        case EMsgBearerSuspended:
            #ifdef _DEBUG
                WriteToLog( _L8( "  Bearer suspended" ) );
            #endif
            iSuspend = ETrue;
            break;
        case EMsgBearerActive:
            #ifdef _DEBUG
                WriteToLog( _L8( "  Bearer active" ) );
            #endif
            iSuspend = EFalse;
            break;
        case EMsgBearerLost:
            #ifdef _DEBUG
                WriteToLog( _L8( "  Bearer lost - Authoritative: %d" ),
                                    aAuthoritativeClose );
            #endif
            break;
        default:
        		//Suppress build warning
        		aAuthoritativeClose = EFalse;
            break;
        }
    }

// ---------------------------------------------------------
// Map the interface UIDs to implementation factory functions
// 
// ---------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
	{
#ifdef __EABI__
    IMPLEMENTATION_PROXY_ENTRY( 0x10207423,	CXcapProtocol::NewL )
#else
    { { 0x10207423 }, CXcapProtocol::NewL }
#endif
    };

// ---------------------------------------------------------
// Return the implementation table & number of implementations
// 
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }
    

  
// End of File
