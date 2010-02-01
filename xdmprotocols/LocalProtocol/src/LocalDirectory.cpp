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
* Description: CLocalDirectory
*
*/




// INCLUDE FILES
#include <utf.h>
#include <f32file.h>
#include "LocalProtocol.h"
#include "LocalDocument.h"
#include "LocalDirectory.h"
#include "XdmDocumentNode.h"
#include "XdmNodeAttribute.h"
#include "LocalDirectoryEntry.h"


// ----------------------------------------------------------
// CLocalDirectory::CLocalDirectory
// 
// ----------------------------------------------------------
//
CLocalDirectory::CLocalDirectory( CXdmEngine& aXdmEngine,
                                  CLocalProtocol& aLocalProtocol ) : 
                                  CXdmDirectory( aXdmEngine ),
                                  iLocalProtocol( aLocalProtocol )                                              
    { 
    }

// ----------------------------------------------------------
// CLocalDirectory::NewL
// 
// ----------------------------------------------------------
//
CLocalDirectory* CLocalDirectory::NewL( const TDesC& aFilePath,
                                        CXdmEngine& aXdmEngine,
                                        CLocalProtocol& aLocalProtocol )
    {
    CLocalDirectory* self = new ( ELeave ) CLocalDirectory( aXdmEngine, aLocalProtocol );
    CleanupStack::PushL( self );
    self->BaseConstructL( aFilePath );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CLocalDirectory::~CLocalDirectory
// 
// ----------------------------------------------------
//
CLocalDirectory::~CLocalDirectory()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDirectory::~CLocalDirectory()" ) );
    #endif
    Cancel();
    iEntryArray.ResetAndDestroy();
    iEntryArray.Close();
    }
    
// ----------------------------------------------------------
// CLocalDirectory::ConstructL
// 
// ----------------------------------------------------------
//
void CLocalDirectory::ConstructL()
    {
    RefreshEntryArrayL();
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------
// CLocalDirectory::RefreshEntryArrayL
// 
// ----------------------------------------------------------
//
void CLocalDirectory::RefreshEntryArrayL()
    {
    CDir* directory = NULL;
    iEntryArray.ResetAndDestroy();
    TPtrC path = iDirectoryPath->Des();
    RFs& session = CLocalProtocol::FileSession();
    User::LeaveIfError( session.GetDir( path, KEntryAttNormal, ESortByExt, directory ) );
    CleanupStack::PushL( directory );
    TInt count = directory->Count();
    FindTimeStampFilesL( directory );
    MatchWithStampFilesL( directory );
    CleanupStack::PopAndDestroy();  //directory
    }

// ----------------------------------------------------------
// CLocalDirectory::FindTimeStampFilesL
// 
// ----------------------------------------------------------
//
void CLocalDirectory::FindTimeStampFilesL( const CDir* aDirectory )
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDirectory::FindTimeStampFilesL()" ) );
    #endif
    TBool ready = EFalse;
    TInt count = aDirectory->Count();
    for( TInt i = 0;!ready && i < count;i++ )
        {
        TPtrC name = ( *aDirectory )[i].iName;
        TInt index = name.Find( KTimeStampFileExt );
        if( index > 0 )
            {
            TInt stampIndex = i;
            CLocalDirectoryEntry* entry = NULL;
            while( !ready )
                {
                entry = CLocalDirectoryEntry::NewL( name );
                CleanupStack::PushL( entry );
                User::LeaveIfError( iEntryArray.Append( entry ) );
                CleanupStack::Pop();  //entry
                stampIndex++;
                if( stampIndex < count )
                    {
                    name.Set( ( *aDirectory )[stampIndex].iName );
                    index = name.Find( KTimeStampFileExt );
                    if( index <= 0 )
                        ready = ETrue;
                    }
                else ready = ETrue;
                }
            }
        }
    }

// ----------------------------------------------------------
// CLocalDirectory::MatchWithStampFilesL
// 
// ----------------------------------------------------------
//
void CLocalDirectory::MatchWithStampFilesL( const CDir* aDirectory )
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDirectory::MatchWithStampFilesL()" ) );
    #endif
    TInt count = aDirectory->Count();
    for( TInt i = 0;i < count;i++ )
        {
        TPtrC name = ( *aDirectory )[i].iName;
        TInt index = name.Find( KTimeStampFileExt );
        if( index > 0 )
            continue;
        else 
            {
            TBool ready = EFalse;
            TInt count = iEntryArray.Count();
            for( TInt i = 0;!ready && i < count;i++ )
                ready = iEntryArray[i]->OfferEntryL( name );
            }
        }
    }

// ----------------------------------------------------------
// CLocalDirectory::DocumentCount
// 
// ----------------------------------------------------------
//
TInt CLocalDirectory::DocumentCount()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDirectory::DocumentCount()" ) );
    #endif
    RefreshEntryArrayL();
    return iEntryArray.Count();
    }

// ----------------------------------------------------
// CLocalDocument::DocumentTypeL
// 
// ----------------------------------------------------
//
TXdmDocType CLocalDirectory::DocumentTypeL( TInt /*aIndex*/ ) const
    {
    return EXdmDocGeneral;
    }
       
// ----------------------------------------------------------
// CLocalDirectory::Document
// 
// ----------------------------------------------------------
//
TPtrC CLocalDirectory::Document( TInt aIndex ) const
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDirectory::Document()" ) );
    #endif
    return aIndex < 0 || aIndex > iEntryArray.Count() ? TPtrC() :
           iEntryArray[aIndex]->EntryName();
    }

// ----------------------------------------------------------
// CLocalDirectory::ConstructL
// 
// ----------------------------------------------------------
//
TPtrC CLocalDirectory::DirectoryPath() const
    {
    return iDirectoryPath != NULL ? iDirectoryPath->Des() : TPtrC();
    }

// ----------------------------------------------------------
// CXcapDirectory::SaveRequestData
// 
// ----------------------------------------------------------
//
void CLocalDirectory::SaveRequestData( TDirUpdatePhase aUpdatePhase,
                                       TRequestStatus& aClientStatus )
    {
    iUpdatePhase = aUpdatePhase;
    iClientStatus = &aClientStatus;
    }
               
// ----------------------------------------------------------
// CLocalDirectory::StartUpdateL
// 
// ----------------------------------------------------------
//
void CLocalDirectory::StartUpdateL()
    
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDirectory::StartUpdateL()" ) );
    #endif
    switch( iUpdatePhase )
        {
        case EUpdateDocumentList:
            RefreshEntryArrayL();
            User::RequestComplete( iClientStatus, KErrNone );
            break;
        case ERefreshDocuments:
            User::RequestComplete( iClientStatus, KErrNone );
        default:
            break;
        } 
    }

// ---------------------------------------------------------
// CLocalDirectory::RunL()
// 
// ---------------------------------------------------------
//
void CLocalDirectory::RunL()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDirectory::RunL() - Request: %d" ), iStatus.Int() );
    #endif
    }

// ----------------------------------------------------
// CLocalDirectory::CancelUpdate
// 
// ----------------------------------------------------
//
void CLocalDirectory::CancelUpdate()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDirectory::CancelUpdate()" ) );
    #endif
    Cancel();
    }
    
// ---------------------------------------------------------
// CLocalDirectory::DoCancel
// 
// ---------------------------------------------------------
//
void CLocalDirectory::DoCancel()
    {
    #ifdef _DEBUG
        iLocalProtocol.WriteToLog( _L8( "CLocalDirectory::DoCancel()" ) );
    #endif
    switch( iUpdatePhase )
        {
        case EDirPhaseIdle:
            break;
        case EUpdateDocumentList:
            break;
        case ERefreshDocuments:
            break;
        default:
            break;
        }
    User::RequestComplete( iClientStatus, KErrCancel );
    }




