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
* Description: CLocalDocument
*
*/




// INCLUDE FILES
#include <f32file.h>
#include "XdmProtocol.h"
#include "XdmNamespace.h"
#include "XdmOperation.h"
#include "XdmXmlParser.h"
#include "LocalProtocol.h"
#include "LocalDocument.h"
#include "LocalDocumentNode.h"
#include "XdmOperationFactory.h"

// ----------------------------------------------------
// CLocalDocument::CLocalDocument
// 
// ----------------------------------------------------
//
CLocalDocument::CLocalDocument( CXdmEngine& aXdmEngine,
                                CLocalProtocol& aLocalProtocol ) :
                                CXdmDocument( aXdmEngine ),
                                iLocalProtocol( aLocalProtocol )
    {
    }


// ----------------------------------------------------------
// CLocalDocument::NewL
// 
// ----------------------------------------------------------
//
CLocalDocument* CLocalDocument::NewL( CXdmEngine& aXdmEngine,
                                      const TDesC& aDocumentName,
                                      CLocalProtocol& aLocalProtocol )
    {   
    CLocalDocument* self = new ( ELeave ) CLocalDocument( aXdmEngine, aLocalProtocol );
    CleanupStack::PushL( self );
    self->BaseConstructL( KLocalOperationFactory, aDocumentName );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
// ----------------------------------------------------------
// CLocalDocument::ConstructL
// 
// ----------------------------------------------------------
//
void CLocalDocument::ConstructL()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocument::ConstructL()" ) );
    #endif
    TPtrC name = iDocumentName->Des();
    TPtrC root = iLocalProtocol.RootFolder();
    iFullPath = HBufC::NewL( name.Length() + root.Length() );
    iFullPath->Des().Copy( root );
    iFullPath->Des().Append( name );
    FetchTimeStampL();
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------
// CLocalDocument::ResetContents
// 
// ----------------------------------------------------------
//
void CLocalDocument::ResetContents()
    {
    delete iDocumentRoot;
    iDocumentRoot = NULL;
    iNamespaces.ResetAndDestroy();
    }

// ----------------------------------------------------------
// CLocalDocument::FetchTimeStampL
// 
// ----------------------------------------------------------
//
void CLocalDocument::FetchTimeStampL()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocument::FetchTimeStampL()" ) );
    #endif
    RFile timeStamp;
    TBuf8<KDateTimeMaxSize> buf;
    TBuf<KDateTimeMaxSize> buf16;
    const TChar idSeparator = 46;
    TPtrC fullPath( iFullPath->Des() );
    HBufC* path = HBufC::NewLC( fullPath.Length() + KTimeStampFileExt().Length() );
    TInt index = fullPath.LocateReverse( idSeparator );
    TPtrC tmspName( index > 0 ? fullPath.Left( index ) : fullPath );
    path->Des().Copy( tmspName );
    path->Des().Append( KTimeStampFileExt );
    TInt error = timeStamp.Open( CLocalProtocol::FileSession(), path->Des(), EFileRead );
    if( error == KErrNone )
        {
        CleanupClosePushL( timeStamp );
        User::LeaveIfError( timeStamp.Read( buf ) );
        buf16.Copy( buf );
        buf.Zero();
        User::LeaveIfError( iLastModification.Parse( buf16 ) );
        CleanupStack::PopAndDestroy();  //timeStamp
        }
    CleanupStack::PopAndDestroy();  //path
    }
// ----------------------------------------------------
// CLocalDocument::~CLocalDocument
// 
// ----------------------------------------------------
//
CLocalDocument::~CLocalDocument()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocument::~CLocalDocument()" ) );
    #endif
    Cancel();
    delete iFullPath;
    delete iDocumentRoot;
    iNamespaces.ResetAndDestroy();
    }

// ----------------------------------------------------
// CLocalDocument::CreateRootL
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CLocalDocument::CreateRootL()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocument::DocumentSubsetL()" ) );
    #endif
    iDocumentRoot = CLocalDocumentNode::NewL( iXdmEngine, iLocalProtocol );
    return iDocumentRoot;
    }

// ----------------------------------------------------
// CLocalDocument::TempCopyL
// 
// ----------------------------------------------------
//
EXPORT_C CLocalDocument* CLocalDocument::TempCopyL()
    {
    return CLocalDocument::NewL( iXdmEngine, Name(), iLocalProtocol );
    }
        
// ----------------------------------------------------
// CLocalDocument::DocumentRoot
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CLocalDocument::DocumentRoot() const
    {
    return iDocumentRoot;
    }    

// ----------------------------------------------------
// CLocalDocument::ErrorRoot
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CLocalDocument::ErrorRoot()
    {
    return NULL;
    }

// ----------------------------------------------------
// CLocalDocument::ErrorRoot
// 
// ----------------------------------------------------
//
TXdmDocType CLocalDocument::DocumentType() const
    {
    return EXdmDocGeneral;
    }
              
// ----------------------------------------------------
// CLocalDocument::IsSubset
// 
// ----------------------------------------------------
//
EXPORT_C TBool CLocalDocument::IsSubset() const
    {
    return iDocSubset; 
    }

// ----------------------------------------------------
// CLocalDocument::ResetSubset
// 
// ----------------------------------------------------
//
EXPORT_C void CLocalDocument::ResetSubset()
    {
    CXdmDocument::ResetSubset();
    delete iDocumentRoot;
    iDocumentRoot = NULL;
    }

// ----------------------------------------------------
// CLocalDocument::XmlFilePath
// 
// ------------------------------ ----------------------
//
EXPORT_C TPtrC CLocalDocument::XmlFilePath() const
    {
    return iFullPath != NULL ? iFullPath->Des() : TPtrC();
    }
    
// ----------------------------------------------------
// CLocalDocument::RemoveData
// 
// ----------------------------------------------------
//
EXPORT_C void CLocalDocument::RemoveData( CLocalDocumentNode* /*aDocumentNode*/ )
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocument::RemoveData()" ) );
    #endif
    
    }
           
// ----------------------------------------------------
// CLocalDocument::StartUpdateL
// 
// ----------------------------------------------------
//
void CLocalDocument::StartUpdateL()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocument::StartUpdateL()" ) );
    #endif
    TBool keepGoing = ETrue;
    TInt completion = KErrNone;
    TInt count = iChangeRequests.Count();
    for( TInt i = 0;keepGoing && i < count;i++ )
        {
        //"First in, first served"
        TRAPD( error, iChangeRequests[0]->ExecuteL() );
        if( error == KErrNone )
            {
            #ifdef _DEBUG
                iLocalProtocol.WriteToLog( _L8( " Execution of the operation no. %d was successful" ), i );
            #endif
            }
        else
            {
            #ifdef _DEBUG
                iLocalProtocol.WriteToLog( _L8( " Execution of the operation no. %d failed with %d" ), i, error );
            #endif
            completion = error;
            keepGoing = EFalse;
            }
        FinaliseOperation( 0 );
        }
    User::RequestComplete( iClientStatus, completion );
    }

// ----------------------------------------------------
// CLocalProtocol::AppendPathPartL
// 
// ----------------------------------------------------
//
void CLocalDocument::AppendPathPartL( const TDesC& aString )
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocument::PathPartL()" ) );
    #endif
    if( iDocumentRoot != NULL )
        {
        CXdmDocumentNode* node = NULL;
        CXdmDocumentNode* parent = iDocumentRoot;
        while( parent->NextNode() != NULL )
            parent = parent->NextNode();
        node = CLocalDocumentNode::NewL( iXdmEngine, aString, parent, iLocalProtocol );
        parent->SetNextNode( node );
        }
    else
        iDocumentRoot = CLocalDocumentNode::NewL( iXdmEngine, aString, iLocalProtocol );
    }

// ----------------------------------------------------
// CLocalProtocol::CurrentExtent
// 
// ----------------------------------------------------
//
CXdmDocumentNode* CLocalDocument::CurrentExtent() const
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocument::CurrentExtent()" ) );
    #endif
    CXdmDocumentNode* ret = NULL;
    if( iDocumentRoot != NULL )
        {
        CXdmDocumentNode* node = iDocumentRoot;
        while( node->NextNode() != NULL )
            node = node->NextNode();
        ret = node;
        }
    else
        {
        #ifdef _DEBUG
            iLocalProtocol.WriteToLog( _L8( " This document does not yet have a root, leave with KErrGeneral" ) );
        #endif
        User::Leave( KErrGeneral );
        }
    return ret;
    }
    
// ----------------------------------------------------
// CLocalDocument::StartUpdateL
// 
// ----------------------------------------------------
//
void CLocalDocument::StartInternalL( TRequestStatus& aStatus )
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocument::StartInternalL()" ) );
    #endif
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    StartUpdateL();
    }
    
// ----------------------------------------------------
// CLocalDocument::CancelUpdate
// 
// ----------------------------------------------------
//
void CLocalDocument::CancelUpdate()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocument::CancelUpdate()" ) );
    #endif
    User::RequestComplete( iClientStatus, KErrCancel );
    }

// ---------------------------------------------------------
// CLocalDocument::RunL()
// 
// ---------------------------------------------------------
//
void CLocalDocument::RunL()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocument::RunL()" ) );
    #endif
    }

// ----------------------------------------------------
// CLocalDocument::FinaliseOperation
// 
// ----------------------------------------------------
//
void CLocalDocument::FinaliseOperation( TInt aIndex )
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocument::FinaliseOperation()" ) );
    #endif
    TInt count = iChangeRequests.Count();
    if( count > 0 && ( aIndex >= 0 && aIndex < count ) )
        {
        MXdmOperation* operation = NULL;
        operation = iChangeRequests[aIndex];
        iChangeRequests.Remove( aIndex );
        operation->Destroy();
        operation = NULL;
        }
    }

// ---------------------------------------------------------
// CLocalDocument::AppendNamespaceL
// 
// ---------------------------------------------------------
//
void CLocalDocument::AppendNamespaceL( const TDesC8& aUri, const TDesC8& aPrefix )
    {
    CXdmNamespace* ns = CXdmNamespace::NewL( aUri, aPrefix );
    CleanupStack::PushL( ns );
    User::LeaveIfError( iNamespaces.Append( ns ) );
    CleanupStack::Pop();  //ns
    }

// ---------------------------------------------------------
// CXcapDocument::RemoveNamespace
// 
// ---------------------------------------------------------
//
void CLocalDocument::RemoveNamespace( const TDesC8& aUri )
    {
    TBool found = EFalse;
    CXdmNamespace* ns = NULL;
    TInt count = iNamespaces.Count();
    for( TInt i = 0;!found && i < count;i++ )
        {
        ns = iNamespaces[i];
        if( ns->Uri().Compare( aUri ) == 0 )
            {
            found = ETrue;
            iNamespaces.Remove( i );
            delete ns;
            ns = NULL;
            }
        }
    }
                
// ---------------------------------------------------------
// CLocalDocument::Uri
// 
// ---------------------------------------------------------
//
TPtrC8 CLocalDocument::Uri( const TDesC8& aPrefix ) const
    {
    TPtrC8 uri( _L8( "" ) );
    TBool found = EFalse;
    TInt count = iNamespaces.Count();
    for( TInt i = 0;i < count && !found;i++ )
        {
        if( iNamespaces[i]->Prefix().Compare( aPrefix ) == 0 )
            {
            uri.Set( iNamespaces[i]->Uri() );
            found = ETrue;
            }
        }
    return uri;
    }

// ---------------------------------------------------------
// CLocalDocument::Count
// 
// ---------------------------------------------------------
//
TInt CLocalDocument::Count() const
    {
    return iNamespaces.Count();
    }

// ---------------------------------------------------------
// CLocalDocument::Prefix
// 
// ---------------------------------------------------------
//
TPtrC8 CLocalDocument::Prefix( TInt aIndex ) const
    {
    TInt count = iNamespaces.Count();
    if( count > 0 && ( aIndex >= 0 && aIndex < count ) )
        return iNamespaces[aIndex]->Prefix();
    else return TPtrC8();
    }
    
// ---------------------------------------------------------
// CLocalDocument::Uri
// 
// ---------------------------------------------------------
//
TPtrC8 CLocalDocument::Uri( TInt aIndex ) const
    {
    TInt count = iNamespaces.Count();
    if( count > 0 && ( aIndex >= 0 && aIndex < count ) )
        return iNamespaces[aIndex]->Uri();
    else return TPtrC8();
    }
    
// ---------------------------------------------------------
// CLocalDocument::ResetNamespaces
// 
// ---------------------------------------------------------
//
void CLocalDocument::ResetNamespaces( ) 
    {   
    iNamespaces.ResetAndDestroy();
    }   
    
// ---------------------------------------------------------
// CLocalDocument::TimeStamp
// 
// ---------------------------------------------------------
//
TTime CLocalDocument::TimeStamp() const
    {
    return iLastModification;
    }

// ---------------------------------------------------------
// CLocalDocument::SaveClientStatus
// 
// ---------------------------------------------------------
//   
void CLocalDocument::SaveClientStatus( TRequestStatus& aStatus )
    {
    iClientStatus = &aStatus;
    }
    
// ---------------------------------------------------------
// CLocalDocument::DoCancel
// 
// ---------------------------------------------------------
//
void CLocalDocument::DoCancel()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDocument::DoCancel()" ) );
    #endif
    }




