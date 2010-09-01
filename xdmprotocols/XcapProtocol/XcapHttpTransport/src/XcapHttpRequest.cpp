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
* Description:   CXcapHttpRequest
*
*/




// INCLUDE FILES
#include <hal.h>
#include <e32math.h>
#include "XdmCredentials.h"
#include "XcapHttpRequest.h"
#include "XcapHttpResponse.h"
#include "XcapHttpTransport.h"
#include "XcapHttpHeaderModel.h"
#include "XcapHttpAuthManager.h"
#include "XcapHttpRequestTimer.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CXcapHttpRequest::CXcapHttpRequest
// 
// ----------------------------------------------------------
//
CXcapHttpRequest::CXcapHttpRequest( RHTTPSession& aHttpSession,
                                    CXcapHttpAuthManager& aAuthManager,
                                    CXcapHttpTransport& aTransportMain ) :
                                    iHttpSession( aHttpSession ),
                                    iTransportMain( aTransportMain ),
                                    iSent( EFalse ),
                                    iActive( EFalse ),
                                    iCancelled( EFalse ),
                                    iConstructed( EFalse ),
                                    iAuthManager( aAuthManager )
                                        
    {
    }

// ----------------------------------------------------------
// CXcapHttpRequest::BaseConstructL
// 
// ----------------------------------------------------------
//
void CXcapHttpRequest::BaseConstructL( const TDesC& aRequestUri )
    {
    SetRequestUriL( aRequestUri );
    iHttpResponse = CXcapHttpResponse::NewL( this, iTransportMain );
    iResponseBuffer = CBufSeg::NewL( 250 );
    }

// ----------------------------------------------------------
// CXcapHttpRequest::~CXcapHttpRequest
// 
// ----------------------------------------------------------
//
CXcapHttpRequest::~CXcapHttpRequest()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::~CXcapHttpRequest()" ) );    
    #endif
    delete iExpiryTimer;
    delete iHttpResponse;
    delete iRequestUriBuf;
    ReleaseResponseData();
    delete iResponseBuffer;
    delete iWholeUri;
    iHeaderCollection.Close();
    }

// ----------------------------------------------------------
// CXcapHttpRequest::DispatchRequestL
// 
// ----------------------------------------------------------
//
EXPORT_C void CXcapHttpRequest::DispatchRequestL( TRequestStatus& aClientStatus )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::DispatchMessageL()" ) );
        iSendTime = TimeL();
    #endif
    ConstructRequestL();
    DoSetHeadersL();
    iCurrentDataLength = 0;
    aClientStatus = KRequestPending;
    iClientStatus = &aClientStatus;
    iHttpTransaction.SubmitL();
    SetStatus( ETrue );
    iActive = ETrue;
    }

// ----------------------------------------------------------
// CXcapHttpRequest::ConstructRequest
// 
// ----------------------------------------------------------
//
void CXcapHttpRequest::ConstructRequestL()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::ConstructRequestL() - Constructed: %d" ), iConstructed );
    #endif
    if( !iConstructed )
        {    
        RStringF method = ConstructMethodStringL();
        CleanupClosePushL( method );
        iHttpTransaction = iHttpSession.OpenTransactionL( iRequestUri, *iHttpResponse, method );
        CleanupStack::PopAndDestroy();  //method
        RHTTPHeaders hdrs = iHttpTransaction.Request().GetHeaderCollection();
        iConstructed = ETrue;
        }
    }

// ----------------------------------------------------------
// CXcapHttpRequest::UpdateRequestUriL
// 
// ----------------------------------------------------------
//
EXPORT_C void CXcapHttpRequest::UpdateRequestUriL( const TDesC8& aUpdatedUri )
    {
    HBufC* temp = HBufC::NewLC( aUpdatedUri.Length() );
    temp->Des().Copy( aUpdatedUri );
    SetRequestUriL( temp->Des() );
    CleanupStack::PopAndDestroy();
    }
    
// ----------------------------------------------------------
// CXcapHttpRequest::ResetUriL
// 
// ----------------------------------------------------------
//
EXPORT_C void CXcapHttpRequest::ResetUriL( const TDesC& aNewUri )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::ResetUriL()" ) );
    #endif
    delete iRequestUriBuf;
    iRequestUriBuf = NULL;
    delete iWholeUri;
    iWholeUri = NULL;
    iWholeUri = HBufC8::NewL( aNewUri.Length() );
    iWholeUri->Des().Copy( aNewUri );
    iRequestUri.Parse( iWholeUri->Des() );
    #ifdef _DEBUG
        TPtrC8 address( iRequestUri.UriDes() );
        iTransportMain.WriteToLog( _L8( "  New URI %S"), &address );
    #endif
    }
    
// ----------------------------------------------------------
// CXcapHttpRequest::RequestUriL
// 
// ----------------------------------------------------------
//
EXPORT_C TPtrC8 CXcapHttpRequest::RequestUriL() const
    {
    return iRequestUriBuf != NULL ? iRequestUriBuf->Des() : TPtrC8();
    }

// ----------------------------------------------------------
// CXcapHttpRequest::ResendL
// 
// ----------------------------------------------------------
//
CXcapHttpAuthManager& CXcapHttpRequest::AuthManager()
    {
    return iAuthManager;
    }
   
// ----------------------------------------------------------
// CXcapHttpRequest::ResendWithAuthL
// 
// ----------------------------------------------------------
//
void CXcapHttpRequest::ResendWithAuthL( TInt aAuthType )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::ResendWithAuthL() - Auth type: %d" ), aAuthType );
    #endif
    iHttpTransaction.Cancel();
    if( iExpiryTimer != NULL && iExpiryTimer->IsActive() )
    	iExpiryTimer->Cancel();
    RHTTPHeaders hdrs = iHttpTransaction.Request().GetHeaderCollection();
    AppendAuthorizationHeaderL( hdrs, aAuthType );
    iHttpTransaction.SubmitL();
    SetStatus( ETrue );
    iActive = ETrue;
    }
        
// ----------------------------------------------------------
// CXcapHttpRequest::Transaction
// 
// ----------------------------------------------------------
//
RHTTPTransaction& CXcapHttpRequest::Transaction()
    {
    return iHttpTransaction;
    }

// ----------------------------------------------------------
// CXcapHttpRequest::RelativeUri
// 
// ----------------------------------------------------------
//
TPtrC8 CXcapHttpRequest::RelativeUri() const
    {
    TPtrC8 absolute( iRequestUri.UriDes() );
    TPtrC8 temp( absolute.Mid( absolute.FindF( _L8( "//" ) ) + 2 ) );
    return temp.Mid( temp.LocateF( '/' ) );
    }
    
// ----------------------------------------------------------
// CXcapHttpRequest::SetStatus
// 
// ----------------------------------------------------------
//
void CXcapHttpRequest::SetRequestUriL( const TDesC& aRequestUri )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::SetRequestUriL()") );
    #endif
    delete iRequestUriBuf;
    iRequestUriBuf = NULL;
    iRequestUriBuf = HBufC8::NewL( aRequestUri.Length() );
    iRequestUriBuf->Des().Copy( aRequestUri );
    TPtrC8 document = iRequestUriBuf->Des();
    TPtrC8 root = iTransportMain.RootUri();
    delete iWholeUri;
    iWholeUri = NULL;
    iWholeUri = HBufC8::NewL( root.Length() + document.Length() );
    iWholeUri->Des().Copy( root );
    iWholeUri->Des().Append( document );
    iRequestUri.Parse( iWholeUri->Des() );
    #ifdef _DEBUG
        TPtrC8 address( iRequestUri.UriDes() );
        iTransportMain.WriteToLog( _L8( "  URI %S"), &address );
    #endif
    }

// ----------------------------------------------------------
// CXcapHttpRequest::SetStatus
// 
// ----------------------------------------------------------
//
void CXcapHttpRequest::SetStatus( const TBool aSent )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::SetStatus()" ) );
    #endif
    iSent = aSent;
    if( iSent )
        {
        if( iExpiryTimer != NULL && !iExpiryTimer->IsActive() )
            iExpiryTimer->ActivateTimer( iExpiryTime );
        }
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::SetStatus(): Status %d"), iSent );
    #endif
    }

// ----------------------------------------------------------
// CXcapHttpRequest::SetHeaderL
// 
// ----------------------------------------------------------
//
EXPORT_C void CXcapHttpRequest::SetHeaderL( const TDesC8& aHeaderName,
                                            const TDesC8& aHeaderValue )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::SetHeaderL()" ) );
        iTransportMain.WriteToLog( _L8( "  Header name:    %S" ), &aHeaderName );
        iTransportMain.WriteToLog( _L8( "  Header value:   %S" ), &aHeaderValue );
    #endif
    CXcapHttpHeaderModel* model = CXcapHttpHeaderModel::NewL( aHeaderName, aHeaderValue, iTransportMain );
    CleanupStack::PushL( model );
    User::LeaveIfError( iHeaderCollection.Append( model ) );
    CleanupStack::Pop();  //model
    }

// ----------------------------------------------------------
// CXcapHttpRequest::HeaderValue
// 
// ----------------------------------------------------------
//
EXPORT_C TPtrC8 CXcapHttpRequest::HeaderValue( const HTTP::TStrings aHeaderName ) const
    {
    THTTPHdrVal value;
    RStringPool stringPool = iHttpSession.StringPool();
    RHTTPHeaders headerCollection = iHttpTransaction.Response().GetHeaderCollection();
    RStringF hdrName = stringPool.StringF( aHeaderName, RHTTPSession::GetTable() );
    headerCollection.GetField( hdrName, 0, value );
    hdrName.Close();
    if( value.Type() == THTTPHdrVal::KStrVal )
        return value.Str().DesC();
    else if( value.Type() == THTTPHdrVal::KStrFVal )
        return value.StrF().DesC();
    else return TPtrC8();
    }

// ----------------------------------------------------------
// CXcapHttpRequest::DoSetHeadersL
// 
// ----------------------------------------------------------
//
void CXcapHttpRequest::DoSetHeadersL()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::DoSetHeadersL()" ) );
    #endif
    TInt count = iHeaderCollection.Count();
    RHTTPHeaders hdrs = iHttpTransaction.Request().GetHeaderCollection();
    if( iAuthManager.IsAuthorized() )
        AppendAuthorizationHeaderL( hdrs, KAuthTypeNormal );
    if( iConstructed && count > 0 )
        {
        #ifdef _DEBUG
            iTransportMain.WriteToLog( _L8( "--------------------" ) );
            iTransportMain.WriteToLog( _L8( "User-set headers:" ) );
        #endif
        for( TInt i = 0;i < count;i++ )
            {
            TPtrC8 name = iHeaderCollection[i]->HeaderName();
            TPtrC8 value = iHeaderCollection[i]->HeaderValue();
            RStringF nameString = iHttpSession.StringPool().OpenFStringL( name );
            CleanupClosePushL( nameString );
            RStringF valueString = iHttpSession.StringPool().OpenFStringL( value );
            CleanupClosePushL( valueString );
            hdrs.SetFieldL( nameString, valueString );
            #ifdef _DEBUG
                iTransportMain.WriteToLog( _L8( "* %S: %S" ), &name, &value );
            #endif
            CleanupStack::PopAndDestroy( 2 );  //valueString, nameString
            }
        #ifdef _DEBUG
            iTransportMain.WriteToLog( _L8( "--------------------" ) );
        #endif
        }
    }

// ----------------------------------------------------------
// CXcapHttpRequest::AppendAuthorizationHeaderL
// 
// ----------------------------------------------------------
//
void CXcapHttpRequest::AppendAuthorizationHeaderL( RHTTPHeaders& aHeaderCollection, TInt aAuthType )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::AppendAuthorizationHeaderL()" ) );
    #endif
    HBufC8* authorization = iAuthManager.AuthorizationL( *this );
    if( authorization != NULL )
        {
        RemoveDuplicateAuth();
        CleanupStack::PushL( authorization );
        RStringF valStr = iHttpSession.StringPool().OpenFStringL( *authorization );
        CleanupClosePushL( valStr );
        switch( aAuthType )
            {
            case KAuthTypeNormal:
                aHeaderCollection.SetFieldL( iHttpSession.StringPool().StringF( 
                    HTTP::EAuthorization, RHTTPSession::GetTable() ), valStr );
                break;
            case KAuthTypeProxy:
                aHeaderCollection.SetFieldL( iHttpSession.StringPool().StringF( 
                    HTTP::EProxyAuthorization, RHTTPSession::GetTable() ), valStr );
                break;
            default:
                #ifdef _DEBUG
                    iTransportMain.WriteToLog( _L8( "  Default case: %d" ), aAuthType );
                #endif
                break;
            }
        CleanupStack::PopAndDestroy( 2 );  //valStr, authorization
        }
    }

// ----------------------------------------------------------
// CXcapHttpRequest::RemoveDuplicateAuth
// 
// ----------------------------------------------------------
//
void CXcapHttpRequest::RemoveDuplicateAuth()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::RemoveDuplicateAuth()" ) );
    #endif
    RStringPool spool = iHttpSession.StringPool();								  
    RHTTPHeaders requestHeaders( iHttpTransaction.Request().GetHeaderCollection() );
    //Check for existence of the header is not needed, remove both if they're there
    requestHeaders.RemoveField( spool.StringF( HTTP::EAuthorization, RHTTPSession::GetTable() ) );
    requestHeaders.RemoveField( spool.StringF( HTTP::EProxyAuthorization, RHTTPSession::GetTable() ) );
    }
               
// ----------------------------------------------------------
// CXcapHttpRequest::AppendDataL
// 
// ----------------------------------------------------------
//
void CXcapHttpRequest::AppendDataL( const TPtrC8& aBodyPart )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::AppendDataL() - Current: %d" ),
                                        iCurrentDataLength );
    #endif
    iResponseBuffer->ResizeL( iCurrentDataLength + aBodyPart.Length() );
    iResponseBuffer->Write( iCurrentDataLength, aBodyPart );
    iCurrentDataLength = iCurrentDataLength + aBodyPart.Length();
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "  Current: %d" ), iCurrentDataLength );
        iTransportMain.WriteToLog( _L8( "  Buffer size: %d" ), iResponseBuffer->Size() );
    #endif
    }

// ----------------------------------------------------------
// CXcapHttpRequest::SetStatus
// 
// ----------------------------------------------------------
//
RHTTPSession& CXcapHttpRequest::Session()
    {
    return iHttpSession;
    }

// ----------------------------------------------------------
// CXcapHttpRequest::SetStatus
// 
// ----------------------------------------------------------
//
EXPORT_C TXdmCompletionData* CXcapHttpRequest::ResponseData()
    {
    return &iRespData;
    }

// ----------------------------------------------------------
// CXcapHttpRequest::ReleaseResponseData
// 
// ----------------------------------------------------------
//
EXPORT_C void CXcapHttpRequest::ReleaseResponseData()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::ReleaseResponseData()" ) );
    #endif
    if( iRespData.iETag != NULL )
        {
        delete iRespData.iETag;
        iRespData.iETag = NULL;
        }
    if( iRespData.iStatusText != NULL )
        {
        delete iRespData.iStatusText;
        iRespData.iStatusText = NULL;
        }
    if( iRespData.iResponseData != NULL )
        {
        delete iRespData.iResponseData;
        iRespData.iResponseData = NULL;
        }
    if( iConstructed )
        {
        iConstructed = EFalse;
        iHttpTransaction.Close();
        }
    iHeaderCollection.ResetAndDestroy();
    }
    
// ----------------------------------------------------------
// CXcapHttpRequest::SetExpiryTimeL
// 
// ----------------------------------------------------------
//
EXPORT_C void CXcapHttpRequest::SetExpiryTimeL( MXcapHttpRequestTimerCallback* /*aCallback*/,
                                                const TTimeIntervalMicroSeconds32 aExpiryTime )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::SetExpiryTimeL()" ) );
    #endif
    if( iExpiryTimer == NULL )
        iExpiryTimer = CXcapHttpRequestTimer::NewL( iTransportMain, this );
    iExpiryTime = aExpiryTime;
    }

// ----------------------------------------------------------
// CXcapHttpRequest::FinaliseRequestL
// 
// ----------------------------------------------------------
//
void CXcapHttpRequest::FinaliseRequestL( TInt aErrorCode )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::FinaliseRequestL()" ) );
        iTransportMain.WriteToLog( _L8( "  Error:  %d" ), aErrorCode );
        iTransportMain.WriteToLog( _L8( "  Active: %d" ), iActive );
    #endif
    if( iActive )
        {
        iActive = EFalse;
        if( iExpiryTimer )
            iExpiryTimer->Cancel();
        PrepareResponseBodyL();
        CompileResponseDataL( aErrorCode );
        User::RequestComplete( iClientStatus, aErrorCode );
        delete iWholeUri;
        iWholeUri = NULL;
        }
    }

// ----------------------------------------------------------
// CXcapHttpRequest::CancelRequest
// 
// ----------------------------------------------------------
//
EXPORT_C void CXcapHttpRequest::CancelRequest()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::CancelRequest()" ) );
    #endif
    if( iActive )
        {
        #ifdef _DEBUG
            iTransportMain.WriteToLog( _L8( " Request is active, cancelling..." ) );
        #endif
        if( iExpiryTimer )
            iExpiryTimer->Cancel();
        iActive = EFalse;
        iConstructed = EFalse;
        iHttpTransaction.Close();
        iRespData.iETag = NULL;
        iRespData.iStatusText = NULL;
        iRespData.iResponseData = NULL;
        iRespData.iCompletion = KErrCancel;
        iRespData.iHttpStatus = KErrCancel;
        User::RequestComplete( iClientStatus, KErrCancel );
        #ifdef _DEBUG
            iTransportMain.WriteToLog( _L8( " Request completed with KErrCancel" ) );
        #endif
        }
    }

// ----------------------------------------------------------
// CXcapHttpRequest::CompileResponseData
// 
// ----------------------------------------------------------
//
void CXcapHttpRequest::CompileResponseDataL( TInt aErrorCode )
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L( "CXcapHttpRequest::CompileResponseData()" ) );
    #endif
    RHTTPResponse response = iHttpTransaction.Response();
    TInt statusInt = response.StatusCode();
    RStringF statusStr = response.StatusText();
    HBufC8* statusBuf = statusStr.DesC().AllocLC();
    TPtrC8 etagDesc( HeaderValue( HTTP::EETag ) );
    HBufC8* eTag = etagDesc.Length() > 0 ? etagDesc.AllocL() : NULL;
    iRespData.iETag = eTag;
    iRespData.iHttpStatus = statusInt;
    iRespData.iStatusText = statusBuf;
    iRespData.iCompletion = aErrorCode;
    iRespData.iResponseData = iFlatResponse;
    CleanupStack::Pop();  //statusBuf
    #ifdef _DEBUG
        if( iRespData.iResponseData != NULL )
            {
            TBuf<32> response( _L( "response" ) );
            response.AppendNum( iHttpTransaction.Id() );
            response.Append( _L( ".xml" ) );
            DumpResponseL( *iRespData.iResponseData, response );
            }
    #endif
    }

#ifdef _DEBUG
// ---------------------------------------------------------
// CXcapHttpRequest::DumpResponseL
//
// ---------------------------------------------------------
//
void CXcapHttpRequest::DumpResponseL( const TDesC8& aRespData, const TDesC& aDumpName ) 
    {
    RFile file;
    RFs session;
    TBuf<512> path( _L( "C:\\logs\\XDM\\"  ) ); 
    path.Append( aDumpName );
    User::LeaveIfError( session.Connect() );
    TInt error( file.Replace( session, path, EFileWrite ) );
    if( error == KErrNone )
        {
        file.Write( aRespData );
        file.Close();
        }
    session.Close();
    }
#endif
    
// ----------------------------------------------------------
// CXcapHttpRequest::PrepareResponseBodyL
// 
// ----------------------------------------------------------
//
void CXcapHttpRequest::PrepareResponseBodyL()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::PrepareResponseBodyL()" ) );
    #endif
    iResponseBuffer->Compress();
    TInt dataLength = iResponseBuffer->Size();
    #ifdef _DEBUG
        TInt contentLength = iHttpResponse->ContentLengthL();
        if( contentLength >= 0 && contentLength < KMaxTInt )
            {
            TPtrC8 result;
            contentLength == dataLength ? result.Set( _L8( "Correct" ) ) :
                                          result.Set( _L8( "Values do not match!" ) );
            iTransportMain.WriteToLog( _L8( "  Actual length: %d *** Content-Length: %d => %S" ),
                                            dataLength, contentLength, &result );
            }
    #endif
    if( dataLength > 0 )
        {
        iFlatResponse = HBufC8::NewL( dataLength );
        TPtr8 pointer( iFlatResponse->Des() );
        iResponseBuffer->Read( 0, pointer, dataLength );
        //pointer.Copy( iResponseBuffer->Ptr( 0 ) );
        iResponseBuffer->Reset();
        iCurrentDataLength = 0;
        }
    }

// ----------------------------------------------------------
// CXcapHttpRequest::HandleTimerEventL
// 
// ----------------------------------------------------------
//
void CXcapHttpRequest::HandleTimerEventL()
    {
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( "CXcapHttpRequest::HandleTimerEventL()" ) );
    #endif
    iHttpTransaction.Close();
    iActive = EFalse;
    iConstructed = EFalse;
    iRespData.iETag = NULL;
    iRespData.iStatusText = NULL;
    iRespData.iResponseData = NULL;
    iRespData.iCompletion = KErrTimedOut;
    iRespData.iHttpStatus = KErrTimedOut;
    User::RequestComplete( iClientStatus, KErrTimedOut );
    #ifdef _DEBUG
        iTransportMain.WriteToLog( _L8( " Request completed with KErrTimedOut" ) );
    #endif
    }

// ---------------------------------------------------------
// CXcapHttpRequest::TimeL
// 
// ---------------------------------------------------------
//
TInt CXcapHttpRequest::TimeL() const
    {
    TInt period = 0;
    User::LeaveIfError( HAL::Get( HALData::ESystemTickPeriod, period ) );
    TInt millisecsPerTick = period / 1000;
    return User::TickCount() * millisecsPerTick;
    }

// End of file

