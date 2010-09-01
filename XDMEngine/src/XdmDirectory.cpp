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
* Description:   XDM Engine directory
*
*/




// INCLUDE FILES
#include "XdmEngine.h"

// ----------------------------------------------------------
// CXdmDirectory::CXdmDirectory
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmDirectory::CXdmDirectory( CXdmEngine& aXdmEngine ) :
                                       CActive( EPriorityStandard ),
                                       iFileCount( KErrNotReady ),
                                       iXdmEngine( aXdmEngine )
    {   
    }

// ----------------------------------------------------------
// CXdmDirectory::BaseConstructL
// 
// ----------------------------------------------------------
//
EXPORT_C void CXdmDirectory::BaseConstructL( const TDesC& aDirectoryPath )
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDirectory::ConstructL()" ) );
    #endif
    iDirectoryPath = aDirectoryPath.AllocL();
    iDirectoryPath->Des().Copy( aDirectoryPath );
    }
       
// ----------------------------------------------------------
// CXdmDirectory::CXdmDirectory
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmDirectory::~CXdmDirectory()
    {
    #ifdef _DEBUG
        iXdmEngine.WriteToLog( _L8( "CXdmDirectory::~CXdmDirectory()" ) );
    #endif
    delete iDirectoryPath;
    }







