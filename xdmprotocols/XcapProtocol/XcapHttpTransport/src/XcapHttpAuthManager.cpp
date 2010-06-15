/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CXcapHttpAuthManager
*
*/




// INCLUDE FILES
#include <hash.h>
#include <e32math.h>
#include "XcapHttpRequest.h"
#include "XcapHttpTransport.h"
#include "XcapHttpAuthManager.h"

// ----------------------------------------------------------
// CXcapHttpAuthManager::CXcapHttpAuthManager
// 
// ----------------------------------------------------------
//
CXcapHttpAuthManager::CXcapHttpAuthManager( RHTTPSession& aHttpSession,
                                            CXcapHttpTransport& aTransportMain ) :
                                            iNonceCount( 1 ),
                                            iAuthorized( EFalse ),
                                            iHttpSession( aHttpSession ),
                                            iTransportMain( aTransportMain ),
                                            iStringPool( aHttpSession.StringPool() )
    { 
    }

// ----------------------------------------------------------
// CXcapHttpAuthManager::NewL
// 
// ----------------------------------------------------------
//
CXcapHttpAuthManager* CXcapHttpAuthManager::NewL( RHTTPSession& aHttpSession,
                                                  CXcapHttpTransport& aTransportMain,
                                                  const TXdmCredentials& aDigestCredentials )
    {
    CXcapHttpAuthManager* self = new ( ELeave ) CXcapHttpAuthManager( aHttpSession, aTransportMain );
    CleanupStack::PushL( self );
    self->ConstructL( aDigestCredentials );
    CleanupStack::Pop();  //self
    return self;
    }

// ----------------------------------------------------
// CXcapHttpAuthManager::~CXcapHttpAuthManager
// 
// ----------------------------------------------------
//
CXcapHttpAuthManager::~CXcapHttpAuthManager()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpAuthManager::~CXcapHttpAuthManager()" ) );
    #endif
    delete iMD5;
    delete iRealm;
    delete iOpaque;
    delete iDomain;
    delete iQopString;
    delete iAlgorithm;
    delete iServerNonce;
    }

// ----------------------------------------------------------
// CXcapHttpAuthManager::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapHttpAuthManager::ConstructL( const TXdmCredentials& aDigestCredentials )
    {
    iMD5 = CMD5::NewL();
    iUserName.Copy( aDigestCredentials.iUserName );
    iPassword.Copy( aDigestCredentials.iPassword );
    } 

// ----------------------------------------------------------
// CXcapHttpAuthManager::SetAuthorized
// 
// ----------------------------------------------------------
//
void CXcapHttpAuthManager::SetAuthorized( TBool aAuthorized )
    {
    iAuthorized = aAuthorized;
    } 

// ----------------------------------------------------------
// CXcapHttpAuthManager::IsAuthorized
// 
// ----------------------------------------------------------
//
TBool CXcapHttpAuthManager::IsAuthorized() const
    {
    return iAuthorized;
    }

// ----------------------------------------------------------
// CXcapHttpAuthManager::AuthorizationL
// 
// ----------------------------------------------------------
//
HBufC8* CXcapHttpAuthManager::AuthorizationL( CXcapHttpRequest& aHttpRequest )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpAuthManager::AuthorizationL()" ) );
    #endif
    if( !iUnauthReceived )
        return NULL;
    TInt bufPtr = 0;
    HBufC8* ret = NULL;
    THTTPHdrVal requestUri;
    //All these strings should have been defined - except for
    //"opaque", maybe - in the WWW-Authentication header that
    //the server sent. In case some of them were not, let's just
    //keep going as if they were there, but were empty. The request
    //will then fail & be completed in the next MHFRunL() call. 
    TPtrC8 realm( iRealm ? iRealm->Des() : TPtrC8() );
    TPtrC8 qop( iQopString ? iQopString->Des() : TPtrC8() );
    TPtrC8 opaque( iOpaque ? iOpaque->Des() : TPtrC8() );
    TPtrC8 nonce( iServerNonce ? iServerNonce->Des() : TPtrC8() );
    TPtrC8 uri( aHttpRequest.RelativeUri() );
    TBuf8<8> nonceCount;
    nonceCount.AppendFormat( _L8( "%08x" ), iNonceCount );   
    CBufFlat* buffer = CBufFlat::NewL( 128 );
    CleanupStack::PushL( buffer ); 
    buffer->InsertL( bufPtr, KAuthHeaderStart );
    bufPtr = bufPtr + KAuthHeaderStart().Length();
    buffer->InsertL( bufPtr, iUserName );
    bufPtr = bufPtr + iUserName.Length();    
    buffer->InsertL( bufPtr, KAuthHeaderParamEndQ );
    bufPtr = bufPtr + KAuthHeaderParamEndQ().Length();    
    buffer->InsertL( bufPtr, KAuthHeaderRealm );
    bufPtr = bufPtr + KAuthHeaderRealm().Length();
    buffer->InsertL( bufPtr, realm );
    bufPtr = bufPtr + realm.Length();
    buffer->InsertL( bufPtr, KAuthHeaderParamEndQ );
    bufPtr = bufPtr + KAuthHeaderParamEndQ().Length();   
    buffer->InsertL( bufPtr, KAuthHeaderNonce );
    bufPtr = bufPtr + KAuthHeaderNonce().Length();
    buffer->InsertL( bufPtr, nonce );
    bufPtr = bufPtr + nonce.Length();
    buffer->InsertL( bufPtr, KAuthHeaderParamEndQ );
    bufPtr = bufPtr + KAuthHeaderParamEndQ().Length(); 
    buffer->InsertL( bufPtr, KAuthHeaderUri );
    bufPtr = bufPtr + KAuthHeaderUri().Length();
    buffer->InsertL( bufPtr, uri );
    bufPtr = bufPtr + uri.Length();
    buffer->InsertL( bufPtr, KAuthHeaderParamEndQ );
    bufPtr = bufPtr + KAuthHeaderParamEndQ().Length();
    buffer->InsertL( bufPtr, KAuthHeaderQop );
    bufPtr = bufPtr + KAuthHeaderQop().Length();
    buffer->InsertL( bufPtr, qop );
    bufPtr = bufPtr + qop.Length();
    buffer->InsertL( bufPtr, KAuthHeaderParamEnd );
    bufPtr = bufPtr + KAuthHeaderParamEnd().Length();
    buffer->InsertL( bufPtr, KAuthHeaderNonceCount );
    bufPtr = bufPtr + KAuthHeaderNonceCount().Length();
    buffer->InsertL( bufPtr, nonceCount );
    bufPtr = bufPtr + nonceCount.Length();
    buffer->InsertL( bufPtr, KAuthHeaderParamEnd );
    bufPtr = bufPtr + KAuthHeaderParamEnd().Length();
    buffer->InsertL( bufPtr, KAuthHeaderCNonce );
    bufPtr = bufPtr + KAuthHeaderCNonce().Length();
    buffer->InsertL( bufPtr, iClientNonce );
    bufPtr = bufPtr + iClientNonce.Length();
    buffer->InsertL( bufPtr, KAuthHeaderParamEndQ );
    bufPtr = bufPtr + KAuthHeaderParamEndQ().Length();    
    buffer->InsertL( bufPtr, KAuthHeaderResponse );
    bufPtr = bufPtr + KAuthHeaderResponse().Length();  
    TPtrC8 respDigest( RequestDigestLC( aHttpRequest )->Des() );
    buffer->InsertL( bufPtr, respDigest );
    bufPtr = bufPtr + respDigest.Length(); 
    CleanupStack::PopAndDestroy();  //RequestDigestLC()
    buffer->InsertL( bufPtr, KAuthHeaderParamEndQ );
    bufPtr = bufPtr + KAuthHeaderParamEndQ().Length();
    if( opaque.Length() > 0 )
        {
        buffer->InsertL( bufPtr, KAuthHeaderOpaque );
        bufPtr = bufPtr + KAuthHeaderOpaque().Length();
        buffer->InsertL( bufPtr, opaque );
        bufPtr = bufPtr + opaque.Length();
        buffer->InsertL( bufPtr, KAuthHeaderParamQuote );
        }
    ret = HBufC8::NewL( buffer->Size() );
    ret->Des().Copy( buffer->Ptr( 0 ) );
    buffer->Reset();
    iNonceCount++;
    CleanupStack::PopAndDestroy();  //buffer
    return ret;
    }

// ----------------------------------------------------------
// CXcapHttpAuthManager::ConsumeAuthInfoParamL
// 
// ----------------------------------------------------------
//
TBool CXcapHttpAuthManager::ConsumeAuthInfoParamL( TAuthInfoParam aName, const TDesC8& aValue )
    {
    //For the time being, always return ETrue
    TBool ret = ETrue;
    TInt length = aValue.Length();
    TPtr8 value( CONST_CAST( TUint8*, aValue.Ptr() ), length, length );
    Unquote( value );
    #ifdef _DEBUG
        TPtrC8 name( KAuthInfoParamArray[aName] );
        iTransportMain.WriteToLog( _L8( "CXcapHttpAuthManager::ConsumeAuthInfoParamL()" ) );
        iTransportMain.WriteToLog( _L8( "  Name:   %S" ), &name );
        iTransportMain.WriteToLog( _L8( "  Value:  %S" ), &value );
    #endif
    switch( aName )
        {                                              
        case ENfoNextnonce:
            {
            SetNextNonceL( value );
            #ifdef _DEBUG
                TPtrC8 nonce( iServerNonce->Des() );
                iTransportMain.WriteToLog( _L8( "  -> Nonce set to %S" ), &nonce );
            #endif
            }
            break;
        case ENfoRspauth:
            /* TODO */
            break;
        case ENfoCnonce:
            #ifdef _DEBUG
                iTransportMain.WriteToLog( _L8( "  -> Current cnonce %S" ), &iClientNonce );
            #endif
            break;
            //ret = value.Compare( iClientNonce ) == 0;
        case ENfoNc:
            {
            TBuf8<8> nonceCount;
            nonceCount.AppendFormat( _L8( "%08x" ), iNonceCount );
            #ifdef _DEBUG
                iTransportMain.WriteToLog( _L8( "  -> Current nonce count %S" ), &nonceCount );
            #endif
            //ret = nonceCount.Compare( value ) == 0;
            break;
            }
        case ENfoQop:
            ret = ETrue;
            break;
        default:
            ret = ETrue;
            break;
        }
    return ret;
    }

// ----------------------------------------------------------
// CXcapHttpAuthManager::GenerateClientNonce
// 
// ----------------------------------------------------------
//
void CXcapHttpAuthManager::SetNextNonceL( const TDesC8& aNextnonce )
    {
    delete iServerNonce;
    iServerNonce = NULL;
    iServerNonce = aNextnonce.AllocL();
    }

// ----------------------------------------------------------
// CXcapHttpAuthManager::Unquote
// 
// ----------------------------------------------------------
//
void CXcapHttpAuthManager::Unquote( TPtr8& aParamValue )
    {
    TInt index = aParamValue.Locate( '"' );
    while( index >= 0 )
        {
        aParamValue.Delete( index, 1 );
        index = aParamValue.Locate( '\"');
        }
    }
       
// ----------------------------------------------------------
// CXcapHttpAuthManager::RequestDigestLC
// 
// ----------------------------------------------------------
//
HBufC8* CXcapHttpAuthManager::RequestDigestLC( CXcapHttpRequest& aHttpRequest )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpAuthManager::RequestDigestLC()" ) );
    #endif
    HBufC8* ret = NULL;
    TPtrC8 nonce( iServerNonce->Des() );
    if( iQop == EXcapAuth || iQop == EXcapAuthInt )
        {
        TBuf8<8> nonceCount;
        TBuf8<KXcapHashLength> hashBush;
        nonceCount.AppendFormat( _L8( "%08x" ), iNonceCount );
        TPtrC8 nonce( iServerNonce->Des() );
        TPtrC8 qop( iQop == EXcapAuth ? _L8( "auth" ) : _L8( "auth-int" ) );
        HBufC8* stringToHash = HBufC8::NewLC( nonce.Length() + qop.Length() +
                                              3 * KXcapHashLength + 8 + 5 );
        TPtr8 desc( stringToHash->Des() );
        ConstructHA1L( desc );
        desc.Append( ':' );
        desc.Append( nonce );
        desc.Append( ':' );
        desc.Append( nonceCount );
        desc.Append( ':' );
        desc.Append( iClientNonce );
        desc.Append(':');
        desc.Append( qop );
        desc.Append(':');
        ConstructHA2L( hashBush, aHttpRequest );
        desc.Append( hashBush ); 
        hashBush.Zero();		
        ret = HBufC8::NewL( KXcapHashLength );
        Hash( *stringToHash, hashBush );
        ret->Des().Copy( hashBush );
        CleanupStack::PopAndDestroy();  //stringToHash
        CleanupStack::PushL( ret );
        }
    else
        {
        /*
        * This case is for compatibility with RFC 2069:
        * request-digest = <"> < KD ( H(A1), unq(nonce-value) ":" H(A2) ) > <">
        */
        #ifdef _DEBUG
            iTransportMain.WriteToLog( _L8( "  Qop not defined, ignore" ) );
        #endif
        }
    return ret;
    }

// ----------------------------------------------------------
// CXcapHttpAuthManager::GenerateClientNonce
// 
// ----------------------------------------------------------
//
void CXcapHttpAuthManager::GenerateClientNonce()
    {
    TTime time;
    TBuf8<33> key;
    time.UniversalTime();
    TInt64 randomNumber = Math::Rand( iSeed );
    randomNumber <<= 32;
    randomNumber += Math::Rand( iSeed );
    key.Zero();
    key.AppendNum( time.Int64(), EHex );
    key.Append( _L8( ":" ) );
    key.AppendNum( randomNumber, EHex );
    Hash( key, iClientNonce );
    }

// ----------------------------------------------------------
// CXcapHttpAuthManager::Hash
// 
// ----------------------------------------------------------
//
void CXcapHttpAuthManager::Hash( const TDesC8& aMessage, TDes8& aHash )
    {
    TBuf8<2> buf;
    aHash.Zero();
    iMD5->Reset();
    TPtrC8 hash = iMD5->Hash( aMessage );
    _LIT8( formatStr, "%02x" );
    for( TInt i = 0;i < KXcapRawHashLength;i++ )
        {
        buf.Zero();
        buf.Format( formatStr, hash[i] );
        aHash.Append( buf );
        }
    }

// ----------------------------------------------------------
// CXcapHttpAuthManager::ParseQopL
// 
// ----------------------------------------------------------
//
void CXcapHttpAuthManager::ParseQopL( const TDesC8& aQopString )
    {
    _LIT8( KXcapAuthInt, "auth-int" );
    iQop = aQopString.FindF( KXcapAuthInt ) == 0 ? EXcapAuthInt : EXcapAuth;
    delete iQopString;
    iQopString = NULL;
    iQopString = HBufC8::NewL( 8 );
    iQopString->Des().Copy( iQop == EXcapAuth ? _L8( "auth" ) : _L8( "auth-int" )  );
    #ifdef _DEBUG
        TPtrC8 qop( iQopString->Des() );
        iTransportMain.WriteToLog( _L8( "  Qop:         %S" ), &qop );
    #endif
    GenerateClientNonce();
    iNonceCount = 1;    
    }
       
// ----------------------------------------------------------
// CXcapHttpAuthManager::ParseHeaderL
// 
// ----------------------------------------------------------
//
TBool CXcapHttpAuthManager::ParseHeaderL( RHTTPTransaction& aTransaction, TInt aAuthType )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpAuthManager::ParseHeaderL()" ) );
    #endif
    TBool ret = EFalse;
    THTTPHdrVal fieldVal;
    RHTTPHeaders headers = aTransaction.Response().GetHeaderCollection();
    RStringF auth = aAuthType == KAuthTypeNormal ? 
        iStringPool.StringF( HTTP::EWWWAuthenticate, RHTTPSession::GetTable() ) :
        iStringPool.StringF( HTTP::EProxyAuthenticate, RHTTPSession::GetTable() );
    headers.GetField( auth, 0, fieldVal );
    if( fieldVal.StrF() == iStringPool.StringF( HTTP::EDigest, RHTTPSession::GetTable() ) )
        {
        ret = ETrue;
        TInt error = headers.GetParam( auth, iStringPool.StringF( HTTP::ERealm, RHTTPSession::GetTable() ), fieldVal );
        if( KErrNone == error )
            {
            delete iRealm;
            iRealm = NULL;
            iRealm = fieldVal.Str().DesC().AllocL();
            #ifdef _DEBUG
                TPtrC8 realm( fieldVal.Str().DesC() );
                iTransportMain.WriteToLog( _L8( "  Realm:       %S" ), &realm );
            #endif
            }
        //In a proper WWW-Authentication request there SHOULD have been 
        //a qop value. In case there was not, call the whole thing off
        error = headers.GetParam( auth, iStringPool.StringF( HTTP::EQop, RHTTPSession::GetTable() ), fieldVal );
        if( KErrNone == error )
            ParseQopL( fieldVal.Str().DesC() );
        else
            {
            delete iQopString;
            iQopString = NULL;
            ret = EFalse;
            }
        error = headers.GetParam( auth, iStringPool.StringF( HTTP::ENonce, RHTTPSession::GetTable() ), fieldVal );
        if( ret && KErrNone == error )
            {
            SetNextNonceL( fieldVal.Str().DesC() );
            #ifdef _DEBUG
                TPtrC8 nonce( iServerNonce->Des() );
                iTransportMain.WriteToLog( _L8( "  Nonce:       %S" ), &nonce );
            #endif
            }
        error = headers.GetParam( auth, iStringPool.StringF( HTTP::EOpaque, RHTTPSession::GetTable() ), fieldVal );
        if( ret && KErrNone == error )
            {
            delete iOpaque;
            iOpaque = NULL;
            iOpaque = fieldVal.Str().DesC().AllocL();
            #ifdef _DEBUG
                TPtrC8 opaque( iOpaque->Des() );
                iTransportMain.WriteToLog( _L8( "  Opaque:      %S" ), &opaque );
            #endif
            }
        error = headers.GetParam( auth, iStringPool.StringF( HTTP::EStale, RHTTPSession::GetTable() ), fieldVal );
        if( ret && KErrNone == error )
            {
            #ifdef _DEBUG
                TPtrC8 stale( fieldVal.Str().DesC() );
                iTransportMain.WriteToLog( _L8( "  Stale:       %S" ), &stale );
            #endif
            }
        error = headers.GetParam( auth, iStringPool.StringF( HTTP::EDomain, RHTTPSession::GetTable() ), fieldVal );
        if( ret && KErrNone == error )
            {
            delete iDomain;
            iDomain = NULL;
            iDomain = fieldVal.Str().DesC().AllocL();
            #ifdef _DEBUG
                TPtrC8 domain( iDomain->Des() );
                iTransportMain.WriteToLog( _L8( "  Domain:       %S" ), &domain );
            #endif
            }
        error = headers.GetParam( auth, iStringPool.StringF( HTTP::EAlgorithm, RHTTPSession::GetTable() ), fieldVal );
        if( ret && KErrNone == error )
            {
            delete iAlgorithm;
            iAlgorithm = NULL;
            iAlgorithm = fieldVal.Str().DesC().AllocL();
            #ifdef _DEBUG
                TPtrC8 algorithm( iAlgorithm->Des() );
                iTransportMain.WriteToLog( _L8( "  Algorithm:   %S" ), &algorithm );
            #endif
            }
        }
    iUnauthReceived = ETrue;
    return ret;
    }

// ----------------------------------------------------------
// CXcapHttpAuthManager::ConstructHA1L
// 
// ----------------------------------------------------------
//
void CXcapHttpAuthManager::ConstructHA1L( TDes8& aResult )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpAuthManager::ConstructHA1L()" ) );
    #endif
    HBufC8* buffer = HBufC8::NewLC( iUserName.Length() +
                     iPassword.Length() + iRealm->Des().Length() + 2 ); 
    TPtr8 desc( buffer->Des() );
    desc.Copy( iUserName );
    desc.Append(':');
    desc.Append( iRealm->Des() );
    desc.Append(':');
    desc.Append( iPassword );
    Hash( *buffer, aResult );
    CleanupStack::PopAndDestroy();  //buffer	
    }

// ----------------------------------------------------------
// CXcapHttpAuthManager::ConstructHA2L
// 
// ----------------------------------------------------------
//
void CXcapHttpAuthManager::ConstructHA2L( TDes8& aResult, CXcapHttpRequest& aHttpRequest )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpAuthManager::ConstructHA2L()" ) );
    #endif
    TPtrC8 uri( aHttpRequest.RelativeUri() );
    TPtrC8 method( aHttpRequest.Transaction().Request().Method().DesC() );
    switch( iQop )
        {
        case EXcapAuth:
        case EXcapAuthNone:
            {
            HBufC8* buffer = HBufC8::NewLC( uri.Length() + method.Length() + 1 );
            TPtr8 desc( buffer->Des() );
            desc.Copy( method );
            desc.Append( ':' );
            desc.Append( uri );
            Hash( desc, aResult );
            CleanupStack::PopAndDestroy();  //buffer
            }
            break;
        case EXcapAuthInt:
            {
            TPtrC8 body( _L8( "" ) );
            TBuf8<KXcapHashLength> bodyHash;
            if( aHttpRequest.Transaction().Request().HasBody() )
                aHttpRequest.Transaction().Request().Body()->GetNextDataPart( body );
            bodyHash.Zero();
            Hash( body, bodyHash );
            HBufC8* buffer = HBufC8::NewLC( uri.Length() + method.Length() +
                                            KXcapHashLength + 2  );
            TPtr8 desc( buffer->Des() );
            desc.Copy( method );
            desc.Append( ':' );
            desc.Append( uri );
            desc.Append( ':' );
            desc.Append( bodyHash );
            Hash( desc, aResult );
            CleanupStack::PopAndDestroy();  //buffer
            }
            break;
        default:
            break;
        }
   
    }
     
// End of File  
