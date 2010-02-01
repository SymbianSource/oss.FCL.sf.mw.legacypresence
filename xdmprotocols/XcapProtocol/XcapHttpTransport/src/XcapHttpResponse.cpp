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
* Description:   CXcapHttpResponse
*
*/




// INCLUDE FILES
#include <XdmErrors.h>
#include "XdmCredentials.h"
#include "XcapHttpRequest.h"
#include "XcapHttpResponse.h"
#include "XcapHttpTransport.h"
#include "XcapHttpAuthManager.h"

const TUint KMaxRetryCount = 3;

// ================= MEMBER FUNCTIONS =======================
//


// ----------------------------------------------------------
// CXcapHttpResponse::CXcapHttpResponse
// 
// ----------------------------------------------------------
//
CXcapHttpResponse::CXcapHttpResponse( CXcapHttpRequest* aHttpRequest,
                                      CXcapHttpTransport& aTransportMain ) :
                                      iAuthPending( EFalse ),
                                      iHttpRequest( aHttpRequest ),
                                      iTransportMain( aTransportMain ),
                                      iAuthManager( iHttpRequest->AuthManager() )                                       
    {
    }

// ----------------------------------------------------
// CXcapHttpResponse::NewL
// 
// ----------------------------------------------------
//
CXcapHttpResponse* CXcapHttpResponse::NewL( CXcapHttpRequest* aHttpRequest,
                                            CXcapHttpTransport& aTransportMain )
    {
    CXcapHttpResponse* self = new ( ELeave ) CXcapHttpResponse( aHttpRequest, aTransportMain );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();  //self
    return self;
    }
    
// ----------------------------------------------------
// CXcapHttpResponse::ConstructL
// 
// ----------------------------------------------------
//
void CXcapHttpResponse::ConstructL()
    {
    }
    
// ----------------------------------------------------
// CXcapHttpResponse::~CXcapHttpResponse()
// Destructor
// ----------------------------------------------------
//
CXcapHttpResponse::~CXcapHttpResponse()
    {
    }

// ----------------------------------------------------
// CXcapHttpResponse::MHFRunL
// 
// ----------------------------------------------------
//
void CXcapHttpResponse::MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent )
    {
    TInt status = aTransaction.Response().StatusCode();
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpResponse::MHFRunL()" ) );
        iTransportMain.WriteToLog( _L8( "  HTTP status:  %d" ), status );
		iTransportMain.WriteToLog( _L8( "  Event status: %d" ), aEvent.iStatus );
        iTransportMain.WriteToLog( _L8( "  TransID:      %d " ), aTransaction.Id() );
    #endif
    switch( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            #ifdef _DEBUG
                DumpHeadersL( aTransaction.Response().GetHeaderCollection(), aTransaction.Id() );
                //DumpHeadersL( aTransaction.Request().GetHeaderCollection(), aTransaction.Id() );
            #endif
            TInt authType;
            if( IsUnauthRequest( aTransaction, authType ) )
                {
                // If server keeps on sending 401 resposenses. Will stop
                // sending new requests after 3 trys.
                if ( KMaxRetryCount == iUnauthRequestCounter )
                    {
                    #ifdef _DEBUG
                        iTransportMain.WriteToLog(
                        _L8( "  Max retry count. Stop sending auth requests to server." ) );
                    #endif   
                    iHttpRequest->FinaliseRequestL( KErrCancel );
                    aTransaction.Cancel();                   
                    }                
                //We only support Digest => If Basic is defined, stop.
                else if( iAuthManager.ParseHeaderL( aTransaction, authType ) )
                    {
                    iHttpRequest->ResendWithAuthL( authType );
                    iAuthPending = ETrue;
                    }
                else
                    {
                    #ifdef _DEBUG
                        iTransportMain.WriteToLog( _L8( " Basic authentication not supported" ) );
                    #endif
                    iHttpRequest->FinaliseRequestL( KXcapErrorAuthentication );
                    aTransaction.Cancel();
                    }
                }
            else if( iAuthPending && ( status != 401 && status != 403 ) )
                {
                iAuthPending = EFalse;
                iAuthManager.SetAuthorized( ETrue );
                }
            else if( !CheckAuthInfoHeaderL( aTransaction.Response().GetHeaderCollection() ) )
                {
                //This means that something was wrong with the Authentication-Info header
                //the server returned. Do not take any chances here, but stop.
                #ifdef _DEBUG
                    iTransportMain.WriteToLog( _L8( " Something's wrong with Authentication-Info => STOP" ) );
                #endif
                iHttpRequest->FinaliseRequestL( KXcapErrorAuthentication );
                aTransaction.Cancel();
                }
            break;
        case THTTPEvent::EGotResponseBodyData:
            #ifdef _DEBUG
                iTransportMain.WriteToLog( _L8( "MHFRunL() - A body part received" ) );
            #endif
            HandleReceivedBodyDataL( aTransaction );
            break;
        case THTTPEvent::EResponseComplete:
            #ifdef _DEBUG
                iTransportMain.WriteToLog( _L8( " * Transaction complete") );
            #endif
            break;
        case THTTPEvent::ESucceeded:
            #ifdef _DEBUG
                iTransportMain.WriteToLog( _L8( " * Transaction succeeded") );
            #endif
            iHttpRequest->FinaliseRequestL( KErrNone );
            break;
        case THTTPEvent::EFailed:
            {
            TInt completion = aEvent.iStatus < 0 ? aEvent.iStatus : KErrNone;
            #ifdef _DEBUG
                iTransportMain.WriteToLog( _L8( " * Transaction failed - Completion: %d"), completion );
            #endif
            iHttpRequest->FinaliseRequestL( completion );
            }
            break;
        case THTTPEvent::ERedirectedPermanently:
            break;
        case THTTPEvent::ERedirectedTemporarily:
            break;
        default:
            {
            if( aEvent.iStatus < KErrNone )
                {
                #ifdef _DEBUG
                    iTransportMain.WriteToLog( _L8( "  * Transaction failed, stop" ) );
                #endif
                iHttpRequest->FinaliseRequestL( aEvent.iStatus );
                }
            else
                {
                #ifdef _DEBUG
                    iTransportMain.WriteToLog( _L8( "  * Unknown status, stop" ) );
                #endif
                iHttpRequest->FinaliseRequestL( KErrUnknown );
                }
            }
            break;
        }  
    }

// ----------------------------------------------------------
// CXcapHttpAuthManager::CheckAuthInfoHeaderL
// 
// ----------------------------------------------------------
//
TBool CXcapHttpResponse::CheckAuthInfoHeaderL( RHTTPHeaders aHeaders )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpResponse::CheckAuthInfoHeaderL()" ) );
    #endif
    THTTPHdrVal fieldVal;
    TBool authOk = ETrue;
    TPtrC8 rawData( _L8( "" ) );
    RStringPool spool = iHttpRequest->Session().StringPool();
    RStringF authInfo = spool.OpenFStringL( TPtrC8( KAuthInfoParamArray[ENfoAuthInfo] ) );
    CleanupClosePushL( authInfo );
    RStringF prxAuthInfo = spool.OpenFStringL( TPtrC8( KAuthInfoParamArray[ENfoPrxAuthInfo] ) );
    CleanupClosePushL( prxAuthInfo );
    if( aHeaders.GetRawField( authInfo, rawData ) == KErrNone ||
        aHeaders.GetRawField( prxAuthInfo, rawData ) == KErrNone )
        {
        TInt length = 0;
        HBufC8* tempCopy = rawData.AllocLC();
        TPtr8 modDesc( tempCopy->Des() );
        while( authOk && modDesc.Length() > 0 )
            {
            TPtrC8 value;
            TAuthInfoParam name = ( TAuthInfoParam )-1;
            if( ParseAuthInfoParam( length, name, value, modDesc ) )
                authOk = iAuthManager.ConsumeAuthInfoParamL( name, value );
            modDesc.Delete( 0, length );
            modDesc.TrimLeft();
            }
        CleanupStack::PopAndDestroy();  //tempCopy
        }
    CleanupStack::PopAndDestroy( 2 );  //prxAuthInfo, authInfo
    return authOk;
    }

// ----------------------------------------------------------
// CXcapHttpAuthManager::ParseAuthInfoParam
// 
// ----------------------------------------------------------
//
TBool CXcapHttpResponse::ParseAuthInfoParam( TInt& aLength, TAuthInfoParam& aName, TPtrC8& aValue, TPtr8& aParam )
    {
    TBool found = EFalse;
    TInt delim = aParam.Locate( ',' );
    TPtrC8 param( delim > 0 ? aParam.Left( delim ) : aParam );
    TInt index = param.Locate( '=' );
    if( index > 0 )
        {
        TPtrC8 name( param.Left( index ) );
        TPtrC8 value( param.Mid( index + 1 ) );
        const TInt count = sizeof( KAuthInfoParamArray ) / sizeof( KAuthInfoParamArray[0] );
        for( TInt i = 0;!found && i < count;i++ )
            {
            if( name.CompareF( TPtrC8( KAuthInfoParamArray[i] ) ) == 0 )
                {
                aValue.Set( value );
                aName = ( TAuthInfoParam )i;
                found = ETrue;
                }
            }
        }
    aLength = delim > 0 ? delim + 1 : aParam.Length();
    return found;
    }

               
// ----------------------------------------------------------
// CXcapHttpResponse::IsUnauthRequest
// 
// ----------------------------------------------------------
//
TBool CXcapHttpResponse::IsUnauthRequest( RHTTPTransaction aTransaction, TInt& aAuthType )
    {
    iUnauthRequestCounter++;
    TInt status = aTransaction.Response().StatusCode();
    switch( status )
        {
        case 401:  //Normal authentication
            aAuthType = 401;
            return ETrue;
        case 407:  //Proxy authentication
            aAuthType = 407;
            return ETrue;
        default:
            return EFalse;
        }
    }
       
// ----------------------------------------------------------
// CXcapHttpResponse::HandleReceivedBodyDataL
// 
// ----------------------------------------------------------
//
TBool CXcapHttpResponse::HandleReceivedBodyDataL( const RHTTPTransaction aTransaction )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpResponse::HandleReceivedBodyDataL()") );
    #endif
    TPtrC8 bodyData;
    MHTTPDataSupplier* respBody = aTransaction.Response().Body();
    TBool lastChunk = respBody->GetNextDataPart( bodyData );
    iHttpRequest->AppendDataL( bodyData );
    respBody->ReleaseData();
    return lastChunk;
    }

// ----------------------------------------------------------
// CImpsHttpTransactionSender::ContentLengthL
// 
// ----------------------------------------------------------
//
TInt CXcapHttpResponse::ContentLengthL() const
    {
    TInt retVal = KMaxTInt;
    THTTPHdrVal fieldValue;
    _LIT8( KContentLength, "Content-Length" );
    RHTTPHeaders headers = ResponseHeaderCollection();
    RStringPool stringPool = iHttpRequest->Session().StringPool();
    RStringF lengthString = stringPool.OpenFStringL( KContentLength );
    headers.GetField( lengthString, 0, fieldValue );
    lengthString.Close();
    if( fieldValue.Type() == THTTPHdrVal::KTIntVal )
        retVal = fieldValue.Int();
    return retVal;
    }
    
// ----------------------------------------------------
// CXcapHttpResponse::HeaderCollection
// 
// ----------------------------------------------------
//
RHTTPHeaders CXcapHttpResponse::ResponseHeaderCollection() const
    {
    return iHttpRequest->Transaction().Response().GetHeaderCollection();
    }

// ----------------------------------------------------
// CXcapHttpResponse::HeaderCollection
// 
// ----------------------------------------------------
//
RHTTPHeaders CXcapHttpResponse::RequestHeaderCollection() const
    {
    return iHttpRequest->Transaction().Request().GetHeaderCollection();
    }

// ----------------------------------------------------
// CXcapHttpResponse::MHFRunError
// HTTP Stack callback interface
// ----------------------------------------------------
//
TInt CXcapHttpResponse::MHFRunError( TInt /*aInt*/, RHTTPTransaction /*aTransaction*/,
                                    const THTTPEvent& /*aEvent*/ )
    {
    return KErrNone;
    }
    
#ifdef _DEBUG
// ----------------------------------------------------
// CXcapHttpResponse::DumpHeadersL
// 
// ----------------------------------------------------
//
void CXcapHttpResponse::DumpHeadersL( RHTTPHeaders aHeaders, TInt aId )
    {
    iTransportMain.WriteToLog( _L( "--------------------") );
    iTransportMain.WriteToLog( _L( "Headers of the transaction %d: "), aId );
    RStringPool strP = iHttpRequest->Session().StringPool();
    THTTPHdrFieldIter it = aHeaders.Fields();
    TBuf<KMaxHeaderNameLen>  fieldName16;
    TBuf<KMaxHeaderValueLen> fieldVal16;

    while( it.AtEnd() == EFalse )
        {
        RStringTokenF fieldName = it();
        RStringF fieldNameStr = strP.StringF( fieldName );
        THTTPHdrVal fieldVal;
        if( aHeaders.GetField( fieldNameStr, 0, fieldVal ) == KErrNone )
            {
            const TDesC8& fieldNameDesC = fieldNameStr.DesC();
            fieldName16.Copy( fieldNameDesC.Left( KMaxHeaderNameLen ) );
            switch( fieldVal.Type() )
                {
            case THTTPHdrVal::KTIntVal:
                iTransportMain.WriteToLog( _L("%S: %d"), &fieldName16, fieldVal.Int() );
                break;
            case THTTPHdrVal::KStrFVal:
                {
                RStringF fieldValStr = strP.StringF( fieldVal.StrF() );
                const TDesC8& fieldValDesC = fieldValStr.DesC();
                fieldVal16.Copy( fieldValDesC.Left( KMaxHeaderValueLen ) );
                iTransportMain.WriteToLog( _L( "%S: %S" ), &fieldName16, &fieldVal16 );
                }
                break;
            case THTTPHdrVal::KStrVal:
                {
                RString fieldValStr = strP.String(fieldVal.Str());
                const TDesC8& fieldValDesC = fieldValStr.DesC();
                fieldVal16.Copy( fieldValDesC.Left( KMaxHeaderValueLen ) );
                iTransportMain.WriteToLog( _L( "%S: %S" ), &fieldName16, &fieldVal16 );
                }
                break;
            case THTTPHdrVal::KDateVal:
                {
                TDateTime date = fieldVal.DateTime();
                TBuf<40> dateTimeString;
                TTime t(date);
                t.FormatL( dateTimeString,KTransportDateFormat );
                iTransportMain.WriteToLog( _L( "%S: %S" ), &fieldName16, &dateTimeString );
                } 
                break;
            default:
                iTransportMain.WriteToLog( _L( "%S: <unrecognised value type>" ), &fieldName16 );
                break;
                }
            // Display realm for WWW-Authenticate header
            RStringF wwwAuth = strP.StringF( HTTP::EWWWAuthenticate, RHTTPSession::GetTable() );
            if( fieldNameStr == wwwAuth )
                {
                // check the auth scheme is 'basic'
                RStringF basic = strP.StringF( HTTP::EBasic,RHTTPSession::GetTable() );
                RStringF realm = strP.StringF( HTTP::ERealm,RHTTPSession::GetTable() );
                THTTPHdrVal realmVal;
                if ( (fieldVal.StrF() == basic ) && 
                    ( !aHeaders.GetParam(wwwAuth, realm, realmVal) ) )
                    {
                    RStringF realmValStr = strP.StringF( realmVal.StrF() );
                    fieldVal16.Copy( realmValStr.DesC() );
                    iTransportMain.WriteToLog( _L( "Realm is: %S" ), &fieldVal16 );
                    }
                }
            }
        ++it;
        }
        iTransportMain.WriteToLog( _L( "--------------------") );
    }
    #endif

// End of File  

