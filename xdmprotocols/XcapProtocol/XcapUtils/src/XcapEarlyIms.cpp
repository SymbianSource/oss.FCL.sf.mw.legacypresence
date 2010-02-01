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
* Description:   CXcapEarlyIms
*
*/




// INCLUDE FILES
#ifdef _DEBUG
    #include <f32file.h>  
    #include <flogger.h>
    #include <e32debug.h>
#endif
#include <cdblen.h>
#include <commsdat.h>
#include <metadatabase.h>
#include <commsdattypesv1_1.h>
#include <mmretrieve.h>
#include "XcapEarlyIms.h"
#include "XcapUtilsInterface.h"

// ----------------------------------------------------------
// CXcapEarlyIms::CXcapEarlyIms
// 
// ----------------------------------------------------------
//
CXcapEarlyIms::CXcapEarlyIms() : CActive( EPriorityNormal ),
                                 iRequestComplete( EFalse ),
                                 iSimType( ESimTypeUnknown )
                                 
    { 
    }

// ----------------------------------------------------------
// CXcapEarlyIms::NewL
// 
// ----------------------------------------------------------
//
EXPORT_C CXcapEarlyIms* CXcapEarlyIms::NewL()
    {
    CXcapEarlyIms* self = new ( ELeave ) CXcapEarlyIms();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();  //self
    return self;
    }

// ----------------------------------------------------
// CXcapEarlyIms::~CXcapEarlyIms
// 
// ----------------------------------------------------
//
CXcapEarlyIms::~CXcapEarlyIms()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapEarlyIms::~CXcapEarlyIms()" ) );
    #endif
    delete iImpu;
    delete iImpi;
    delete iAuthData;
    delete iRetriever;
    iPhone.Close();
    iServer.Close();
    }

// ----------------------------------------------------------
// CXcapEarlyIms::ConstructL
// 
// ----------------------------------------------------------
//
void CXcapEarlyIms::ConstructL()
    {
    #ifdef _DEBUG
        DeleteLogFileL();
        WriteToLog( _L8( "CXcapEarlyIms::ConstructL()" ) );
    #endif
    #ifndef __FAKE_IMPU__
        TName tsyName;
        TInt numPhone = 0;
        TBool found = EFalse;
        RTelServer::TPhoneInfo phoneInfo;
        User::LeaveIfError( iServer.Connect() );
        TBuf<KCommsDbSvrMaxFieldLength> tsyModuleName;
	    ModuleNameL( tsyModuleName );
        User::LeaveIfError( iServer.LoadPhoneModule( tsyModuleName ) );
        User::LeaveIfError( iServer.EnumeratePhones( numPhone ) );
        for( TInt i = 0;!found && i < numPhone;i++ )
            {
            User::LeaveIfError( iServer.GetTsyName( i, tsyName ) );
            User::LeaveIfError( iServer.GetPhoneInfo( i, phoneInfo ) );
            if( tsyName.CompareF( tsyModuleName ) == 0 )
                found = ETrue;
            }
        __ASSERT_ALWAYS( found, User::Leave( KErrNotFound ) );
        User::LeaveIfError( iPhone.Open( iServer, phoneInfo.iName ) ); 
        SelectSimTypeL();
    #endif
    CActiveScheduler::Add( this );    
    } 

// ----------------------------------------------------------
// CXcapEarlyIms::RequestSimDataL
// 
// ----------------------------------------------------------
//
EXPORT_C void CXcapEarlyIms::RequestSimDataL( MXcapEarlyImsObs* aObserver )
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapEarlyIms::RequestSimDataL()" ) );
    #endif
    #ifdef __FAKE_IMPU__
        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
    #else
        if( !IsActive() )
		    {
		    TUint32 caps( 0 );    
            User::LeaveIfError( iPhone.GetIdentityCaps( caps ) );
            if( !( caps & RMobilePhone::KCapsGetSubscriberId ) )
	  	        User::Leave( KErrNotSupported );
            iPhone.GetSubscriberId( iStatus, iImsi );
            iRequestPhase = TRequestAuth;
            iRequestComplete = EFalse;
		    SetActive(); 
    	    }
    #endif 
    iObserver = aObserver;   
    }

// ----------------------------------------------------------
// CXcapEarlyIms::PublicIDL
// 
// ----------------------------------------------------------
//
EXPORT_C TPtrC8 CXcapEarlyIms::PublicIDL()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapEarlyIms::PublicIDL()" ) );
    #endif
    #ifdef __FAKE_IMPU__
        return KXcapFakeImpu();
    #else
        if( iRequestComplete && iRequestPhase == TRequestIdle )
            {
            switch( iSimType )
                {
                case ESimTypeWcdma:
                    return iAuthData->iIMPUArray[0]; 
                case ESimTypeGsm:
                    return DeriveL();
                default:
                    return TPtrC8();
                }
            }
        else return TPtrC8(); 
    #endif
    }

// ----------------------------------------------------------
// CXcapEarlyIms::DeriveL
// 
// ----------------------------------------------------------
//
TPtrC8 CXcapEarlyIms::DeriveL()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapEarlyIms::DeriveL()" ) );
    #endif
    _LIT8( KImsMnc,       "ims.mnc" );
    _LIT8( KMcc,          ".mcc" );
    _LIT8( K3gppnetwork,  ".3gppnetwork.org");
    _LIT8( KAt,           "@");
    _LIT8( KSip,          "sip:");
    _LIT8( KZero,         "0");
    TPtrC networkDesc( iNetworkInfoPckg().iNetworkId );
    TPtrC countryDesc( iNetworkInfoPckg().iCountryCode );
    TInt nwlength = networkDesc.Length() + countryDesc.Length();
    if( networkDesc.Length() == 2 )
        nwlength = nwlength + 1;
    //private user identity
    TInt impiLength = iImsi.Length() + KAt().Length() + KImsMnc().Length() +
                      nwlength + KMcc().Length() + K3gppnetwork().Length();   
    iImpi = HBufC8::NewL( impiLength );
	TPtr8 impiDesc( iImpi->Des() );
	impiDesc.Append( iImsi );
	impiDesc.Append( KAt );
	impiDesc.Append( KImsMnc );
	if( networkDesc.Length() == 2 )
    	impiDesc.Append( KZero );
	impiDesc.Append( networkDesc );	
	impiDesc.Append( KMcc );	
	impiDesc.Append( countryDesc );
	impiDesc.Append( K3gppnetwork );
    //public user identity
    TInt impuLength = KSip().Length() + impiDesc.Length();
    iImpu = HBufC8::NewL( impuLength );
    TPtr8 impuDesc( iImpu->Des() );
    impuDesc.Append( KSip );
	impuDesc.Append( impiDesc );
	#ifdef _DEBUG
        WriteToLog( _L8( "  IMPI: %S" ), &impiDesc );
        WriteToLog( _L8( "  IMPU: %S" ), &impuDesc );
    #endif
    return impuDesc;  
    }

// ----------------------------------------------------------
// CXcapEarlyIms::SelectSimTypeL
// 
// ----------------------------------------------------------
//
void CXcapEarlyIms::SelectSimTypeL()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapEarlyIms::SelectSimTypeL()" ) );
    #endif
	TUint32 caps;
	User::LeaveIfError( iPhone.GetIccAccessCaps( caps ) );
	if( caps & RMobilePhone::KCapsSimAccessSupported )
		{
		if( caps & RMobilePhone::KCapsUSimAccessSupported )
		    {			
		    #ifdef _DEBUG
                WriteToLog( _L8( "  USIM Card" ) );
            #endif
		    iSimType = ESimTypeWcdma;
		    }
		else
		    {
		    #ifdef _DEBUG
                WriteToLog( _L8( "  2G SIM Card" ) );
            #endif
		    iSimType = ESimTypeGsm;
		    }
		}
    else
        {
        #ifdef _DEBUG
            WriteToLog( _L8( "  No SIM Card" ) );
        #endif
        iSimType = ESimTypeUnknown;
        }
    }
		    
// ----------------------------------------------------------
// CXcapEarlyIms::RunL
// 
// ----------------------------------------------------------
//
void CXcapEarlyIms::RunL()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapEarlyIms::RunL()" ) );
        WriteToLog( _L8( "  Status:        %d" ), iStatus.Int() );
        WriteToLog( _L8( "  Request phase: %d" ), iRequestPhase );
    #endif
    #ifdef __FAKE_IMPU__
        iObserver->RequestComplete( KErrNone );
    #else
        if( iStatus.Int() == KErrNone )
            {
            switch( iRequestPhase )
                {
                case TRequestIdle:
                    break; 
                case TRequestAuth:
                    {
                    #ifdef _DEBUG
                        HBufC8* buf = HBufC8::NewLC( iImsi.Length() );
                        TPtr8 desc( buf->Des() );
                        desc.Copy( iImsi );
                        WriteToLog( _L8( "  IMSI retrieved: %S" ), &desc );
                        CleanupStack::PopAndDestroy();  //buf
                    #endif
                    iPhone.GetHomeNetwork( iStatus, iNetworkInfoPckg );
                    iRequestPhase = TRequestNetwork;
                    SetActive(); 
                    }
                    break;
                case TRequestNetwork:
                    {
                    #ifdef _DEBUG
                        HBufC8* nwid = HBufC8::NewLC( iNetworkInfoPckg().iNetworkId.Length() );
                        HBufC8* country = HBufC8::NewLC( iNetworkInfoPckg().iCountryCode.Length() );
                        TPtr8 desc1( nwid->Des() );
                        TPtr8 desc2( country->Des() );
                        desc1.Copy( iNetworkInfoPckg().iNetworkId );
                        desc2.Copy( iNetworkInfoPckg().iCountryCode );
                        WriteToLog( _L8( "  Home network info retrieved" ) );
                        WriteToLog( _L8( "    Network ID:   %S" ), &desc1 );
                        WriteToLog( _L8( "    Country code: %S" ), &desc2  );
                        CleanupStack::PopAndDestroy( 2 );  //country, nwid
                    #endif
                    if( iSimType == ESimTypeWcdma )
                        {
    	                iAuthData = RMobilePhone::CImsAuthorizationInfoV5::NewL();
            	        iRetriever = CAsyncRetrieveAuthorizationInfo::NewL( iPhone, *iAuthData );
                        iRequestPhase = TRequestUsimAuth;
	    	            iRetriever->Start( iStatus );
	        	        SetActive();
                        }
                    else
                        {
                        #ifdef _DEBUG
                            WriteToLog( _L8( "  Request complete" ) );
                        #endif
                        iObserver->RequestComplete( KErrNone );
                        iRequestPhase = TRequestIdle;
                        iRequestComplete = ETrue;
                        }
                    }
                    break;
                case TRequestUsimAuth:
                    {
                    #ifdef _DEBUG
                        RMobilePhone::TAuthorizationDataSource source(
                            iAuthData->iAuthenticationDataSource );
                        TBuf8<4> sourceDesc( source == RMobilePhone::EFromUSIM
                            ? _L8( "USIM" ) : _L8( "ISIM" ) );  
                        WriteToLog( _L8( "  USIM auth retrieved" ) );
                        WriteToLog( _L8( "    IMPI:           %S" ), &iAuthData->iIMPI );
                        WriteToLog( _L8( "    IMPU:           %S" ), &iAuthData->iIMPUArray[0] );
                        WriteToLog( _L8( "    Domain:         %S" ), &iAuthData->iHomeNetworkDomainName );
                        WriteToLog( _L8( "    AuthDataSource: %S" ), &sourceDesc );
                    #endif
                    iObserver->RequestComplete( KErrNone );
                    iRequestPhase = TRequestIdle;
                    iRequestComplete = ETrue;
                    }
                    break;
                default:
                    break;
                }
            }
        else iObserver->RequestComplete( iStatus.Int() );
    #endif
    }
    
// ----------------------------------------------------------
// CXcapEarlyIms::DoCancel
// 
// ----------------------------------------------------------
//
void CXcapEarlyIms::DoCancel()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXcapEarlyIms::DoCancel() - State: %d" ), iRequestPhase );
    #endif
    switch( iRequestPhase )
        {
        case TRequestIdle:
            break; 
        case TRequestAuth:
            iPhone.CancelAsyncRequest( EMobilePhoneGetSubscriberId );
            break;
        case TRequestNetwork:
            iPhone.CancelAsyncRequest( EMobilePhoneGetHomeNetwork );
            break;
        case TRequestUsimAuth:
            delete iRetriever;
            iRetriever = NULL;
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------------
// CXcapEarlyIms::ModuleNameL
// 
// ----------------------------------------------------------
//
void CXcapEarlyIms::ModuleNameL( TDes& aModuleName ) const
    {
    __ASSERT_ALWAYS( aModuleName.MaxSize() >= KCommsDbSvrMaxFieldLength, User::Leave( KErrArgument ) );
    using namespace CommsDat;
    CMDBSession* db = CMDBSession::NewLC( KCDLatestVersion );
    CMDBField<TUint32>* field = new ( ELeave ) CMDBField<TUint32>( KCDTIdModemPhoneServicesSMS );
    CleanupStack::PushL( field );
    field->SetRecordId( 1 );
    field->LoadL( *db );
    TUint32 modemId = *field;
    CMDBField<TDesC>* tsyField = new ( ELeave ) CMDBField<TDesC>( KCDTIdTsyName );
    CleanupStack::PushL( tsyField );
    tsyField->SetRecordId( modemId );
    tsyField->LoadL( *db );
    aModuleName = *tsyField;
    CleanupStack::PopAndDestroy( 3 ); //tsyField, field, db
    }
 
#ifdef _DEBUG              
// ----------------------------------------------------
// CXcapEarlyIms::WriteToLog
//
// ----------------------------------------------------
//
void CXcapEarlyIms::WriteToLog( TRefByValue<const TDesC8> aFmt,... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KUtilsLogBufMaxSize> buf;
    buf.FormatList( aFmt, list );
    RFileLogger::Write( KUtilsLogDir, KUtilsLogFile, EFileLoggingModeAppend, buf );
    }

// ----------------------------------------------------------
// CXcapEarlyIms::DeleteLogFileL
// 
// ----------------------------------------------------------
//
void CXcapEarlyIms::DeleteLogFileL()
    {
    RFs session;
    TBuf<32> log( _L( "C:\\logs\\XDM\\" ) );
    User::LeaveIfError( session.Connect() );
    CFileMan* manager = CFileMan::NewL( session );
    log.Append( KUtilsLogFile );
    manager->Delete( log );
    session.Close();
    delete manager;
    manager = NULL;
    }
#endif  //_DEBUG
      




// End of File  
