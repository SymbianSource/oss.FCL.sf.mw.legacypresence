/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   XDM Engine log writer
*
*/




#ifndef __XDMLOGWRITER__
#define __XDMLOGWRITER__

#include <e32base.h>

_LIT( KXdmLogDir,                                  "XDM" );
_LIT( KXdmLogFileExt,                              ".txt" );
_LIT( KXdmLogRoot,                                 "C:\\logs\\" );

//Determines how many log file instances are
//retained until the directory is cleaned up
/* E.g. XdmEngine1.txt, XdmEngine2.txt ... XdmEngine10.txt */
const TInt KMaxLogFiles                            = 10;

/* Default size of the log buffer */
const TInt KLogBufferMaxSize		               = 2000;

// CLASS DECLARATION
class CXdmLogWriter : public CBase
    {
    public:
        
        /**
        * Symbian OS constructor
        *
        * @param TDesC& The log file name
        * @return CXdmLogWriter* A log writer instance
        */ 
        IMPORT_C static CXdmLogWriter* NewL( const TDesC& aLogName );
        
        /**
        * Write a log string.
        *
        * @param TDesC& Log string in 16-bit
        * @return void
        */     
        IMPORT_C void WriteToLog( const TDesC& aLogLine ) const;
        
        /**
        * Write a log string.
        *
        * @param TDesC8& Log string in 8-bit
        * @return void
        */     
        IMPORT_C void WriteToLog( const TDesC8& aLogLine  ) const;
        
        /**
        * Destructor
        */ 
        virtual ~CXdmLogWriter();

    private:
        
        /**
        * C++ constructor is private
        */ 
        CXdmLogWriter();
        
        /**
        * Symbian OS second-phase constructor
        * @param TDesC& Name of the log file to write to
        * @return void
        */ 
        void ConstructL( const TDesC& aLogName );
        
    private:  //Data
        
        HBufC*              iLogFileName;
    
    };

#endif      //__XDMLOGWRITER__
            
// End of File
