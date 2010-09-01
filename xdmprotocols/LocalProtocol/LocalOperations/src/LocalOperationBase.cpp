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
* Description: CLocalOperationBase
*
*/





// INCLUDES
#include "XdmXmlParser.h"
#include "LocalProtocol.h"
#include "LocalDocument.h"
#include "LocalOperationBase.h"

// ---------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CLocalOperationBase::CLocalOperationBase( const CLocalDocument& aTargetDoc,
                                          CLocalOperationFactory& aOperationFactory ) :
                                          iTargetDoc( CONST_CAST( CLocalDocument&, aTargetDoc ) ),
                                          iOperationFactory( aOperationFactory ) 
    {
    }

// ---------------------------------------------------------
// Destructor
//
// ---------------------------------------------------------
//
CLocalOperationBase::~CLocalOperationBase()
    {
    iXmlFile.Close();
    delete iXmlParser;
    delete iFileManager;
    }

// ---------------------------------------------------------
// CLocalOperationBase::BaseConstructL
//
// ---------------------------------------------------------
//
void CLocalOperationBase::BaseConstructL()
    {
    iXmlParser = CXdmXmlParser::NewL();
    iFileManager = CFileMan::NewL( CLocalProtocol::FileSession() );
    }
    
// ---------------------------------------------------------
// CLocalOperationBase::WriteFileL
//
// ---------------------------------------------------------
//
void CLocalOperationBase::WriteFileL( const CXdmDocumentNode* aRootNode )
    {
    __ASSERT_DEBUG( aRootNode != NULL, User::Panic( _L( "CLocalOperationBase" ), 1 ) );
    HBufC8* data = iXmlParser->FormatToXmlLC( ETrue, &iTargetDoc, aRootNode );
    User::LeaveIfError( iXmlFile.Write( data->Des() ) );
    CleanupStack::PopAndDestroy();  //data
    GenerateTimeStampL();
    }

// ---------------------------------------------------------
// CLocalOperationBase::GenerateTimeStampL
//
// ---------------------------------------------------------
//
void CLocalOperationBase::GenerateTimeStampL()
    {
    RFile timeStamp;
    HBufC8* time = TimeLC();
    HBufC* timeStampName = ConstructTimestampNameL();
    CleanupStack::PushL( timeStampName );
    User::LeaveIfError( timeStamp.Replace( CLocalProtocol::FileSession(), timeStampName->Des(), EFileWrite ) );
    User::LeaveIfError( timeStamp.Write( time->Des() ) );
    timeStamp.Close();
    CleanupStack::PopAndDestroy( 2 );  //timeStampName, time
    }

// ---------------------------------------------------------
// CLocalOperationBase::ConstructTimestampNameL
//
// ---------------------------------------------------------
//
HBufC* CLocalOperationBase::ConstructTimestampNameL()
    {
    const TChar backSlash = 92;
    const TChar idSeparator = 46;
    TInt index = iFullPath.LocateReverse( backSlash );
    TPtrC path = iFullPath.Left( index + 1 );
    HBufC* fileName = HBufC::NewLC( iTargetDoc.Name().Length() + KTimeStampFileExt().Length() );
    HBufC* fullPath = HBufC::NewLC( path.Length() + fileName->Des().MaxLength() );
    TPtr ptr( fileName->Des() );
    ptr.Copy( iTargetDoc.Name() );
    index = ptr.LocateReverse( idSeparator );
    TPtrC tmspName = index > 0 ? ptr.Left( index ) : ptr;
    fullPath->Des().Copy( path );
    fullPath->Des().Append( tmspName );
    fullPath->Des().Append( KTimeStampFileExt );
    CleanupStack::Pop();  //fullPath
    CleanupStack::PopAndDestroy();  //fileName
    return fullPath;
    }

// ---------------------------------------------------------
// CLocalOperationBase::Time
//
// ---------------------------------------------------------
//
HBufC8* CLocalOperationBase::TimeLC()
    {
    TTime time;
    TBuf<KDateBufferMaxSize> dateBuffer;
	time.HomeTime();
    time.FormatL( dateBuffer, KDateFormat );
    TBuf<KDateBufferMaxSize> timeBuffer;
	time.HomeTime();
	time.FormatL( timeBuffer, KTimeFormat );
    TBuf<KDateTimeMaxSize> buffer;
    buffer.SetLength( 0 );
    buffer.Copy( dateBuffer );
    buffer.Append( _L( " " ) );
    buffer.Append( timeBuffer );
    HBufC8* ret = HBufC8::NewLC( buffer.Length() );
    ret->Des().Copy( buffer );
    return ret;        
    }

// ---------------------------------------------------------
// CLocalOperationBase::FetchXmlDataL
//
// ---------------------------------------------------------
//
HBufC8* CLocalOperationBase::FetchXmlDataL( TInt aFileSize )
    {
    HBufC8* data = HBufC8::NewLC( aFileSize );
    TPtr8 desc( data->Des() );
    User::LeaveIfError( iXmlFile.Read( desc ) );
    CleanupStack::Pop();  //data
    return data;
    }

// ---------------------------------------------------------
// CLocalOperationBase::FetchXmlDataL
//
// ---------------------------------------------------------
//
TInt CLocalOperationBase::OpenDataFileL( const TDesC& aFilePath )
    {
    TInt length = 0;
    iFullPath.Set( aFilePath );
    TInt error = iXmlFile.Open( CLocalProtocol::FileSession(), iFullPath,
                                EFileRead | EFileWrite | EFileShareExclusive );
    if( error != KErrNone )
        {
        if( error == KErrPathNotFound )
            User::LeaveIfError( CLocalProtocol::FileSession().MkDirAll( CLocalProtocol::RootFolder() ) );
        error = iXmlFile.Create( CLocalProtocol::FileSession(), iFullPath,
                                 EFileRead | EFileWrite | EFileShareExclusive );                                 
        }
    error = error == KErrNone ? iXmlFile.Size( length ) : KErrUnknown;
    return error == KErrNone ? length : error;
    }

// ---------------------------------------------------------
// CLocalOperationBase::ReplaceDataFileL
//
// ---------------------------------------------------------
//
void CLocalOperationBase::ReplaceDataFileL()
    {
    User::LeaveIfError( iXmlFile.Replace( CLocalProtocol::FileSession(), iFullPath,
                                          EFileRead | EFileWrite | EFileShareExclusive ) );
    }

// ---------------------------------------------------------
// CLocalOperationBase::DeleteDataFileL
//
// ---------------------------------------------------------
//
void CLocalOperationBase::DeleteDataFileL()
    {
    User::LeaveIfError( CLocalProtocol::FileSession().Delete( iFullPath ) );
    HBufC* timeStampName = ConstructTimestampNameL();
    CleanupStack::PushL( timeStampName );
    User::LeaveIfError( CLocalProtocol::FileSession().Delete( timeStampName->Des() ) );
    CleanupStack::PopAndDestroy();  //timeStampName
    }      

// ---------------------------------------------------------
// CLocalOperationBase::ExecuteL
//
// ---------------------------------------------------------
//
void CLocalOperationBase::ExecuteL( TRequestStatus& /*aStatus*/, TUint /*aOptions*/ )
    {
    }
        
// ---------------------------------------------------------
// CLocalOperationBase::CancelOperation
//
// ---------------------------------------------------------
//
void CLocalOperationBase::CancelOperation()
    {
    }

// ---------------------------------------------------------
// CLocalOperationBase::CompletionData
//
// ---------------------------------------------------------
//     
const TXdmCompletionData& CLocalOperationBase::CompletionData() const
    {
    return iCompletionData;
    }
        
// ---------------------------------------------------------
// CLocalOperationBase::IsCompleted
//
// ---------------------------------------------------------
//        
TBool CLocalOperationBase::IsCompleted() const
    {
    return ETrue;
    }
        
// ---------------------------------------------------------
// CLocalOperationBase::TargetNode
//
// ---------------------------------------------------------
//      
CXdmDocumentNode* CLocalOperationBase::TargetNode() const
    {
    return NULL;
    }                      

// ---------------------------------------------------------
// CLocalOperationBase::Result
//
// ---------------------------------------------------------
//        
TInt CLocalOperationBase::Result() const
    {
    return KErrNone;
    }   
     
//  End of File  

