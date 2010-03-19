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
* Description:   CXdmXmlParser
*
*/




// INCLUDE FILES
#include <hal.h>
#include <parser.h>
#include <f32file.h>
#include <flogger.h>
#include <parserfeature.h>
#include <XdmNodeAttribute.h>
#include <XdmDocument.h>
#include <xdmlogwriter.h>
#include <XdmDocumentNode.h>
#include "XdmXmlParser.h"
#include "XmlFormatter.h"
#include "XmlParserDefines.h"
#include "XdmXmlContentHandler.h"

// ----------------------------------------------------------
// CXdmXmlParser::CXdmXmlParser
// 
// ----------------------------------------------------------
//
CXdmXmlParser::CXdmXmlParser() : iDumpIndex( 1 ),
                                 iXmlDocument( ( TText8* )"", 0, 0 )                                           
    {
    }

// ----------------------------------------------------------
// CXdmXmlParser::NewL
// 
// ----------------------------------------------------------
//
EXPORT_C CXdmXmlParser* CXdmXmlParser::NewL()
    {
    CXdmXmlParser* self = new ( ELeave ) CXdmXmlParser();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CXdmXmlParser::ConstructL
// 
// ----------------------------------------------------------
//
void CXdmXmlParser::ConstructL()
    {
    #ifdef _DEBUG
        iLogWriter = CXdmLogWriter::NewL( KParserLogFile );
        WriteToLog( _L8( "CXdmXmlParser::ConstructL()" ) );
    #endif
    iXmlFormatter = CXmlFormatter::NewL( *this );
    iContentHandler = CXdmXmlContentHandler::NewL( *this );
    iXmlParser = CParser::NewL( KDefaultMimeType, *iContentHandler );               
    }

// ----------------------------------------------------
// CXdmXmlParser::~CXdmXmlParser
// 
// ----------------------------------------------------
//
CXdmXmlParser::~CXdmXmlParser()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmXmlParser::~CXdmXmlParser()" ) );
    #endif
    delete iXmlParser;
    delete iLogWriter;
    delete iXmlFormatter;
    delete iContentHandler;
    }
     
// ----------------------------------------------------------
// CXdmXmlParser::ParseDocumentL
// 
// ----------------------------------------------------------
//    
EXPORT_C void CXdmXmlParser::ParseDocumentL( CXdmDocument* aDocument,
                                             const TDesC8& aXmlDocument )  
    {
    #ifdef _DEBUG
        TInt start = CXdmXmlParser::TimeL();
    #endif
    CXdmDocumentNode* root = aDocument->DocumentRoot();
    if( root == NULL )
        aDocument->CreateRootL();
    User::LeaveIfError( iXmlParser->EnableFeature( EReportNamespaceMapping ) );
    iContentHandler->SetTarget( *aDocument );
    Xml::ParseL( *iXmlParser, aXmlDocument );
    iContentHandler->Reset();
    //self->ParseL( aDocument, aXmlDocument );
    #ifdef _DEBUG
        TInt finish = CXdmXmlParser::TimeL();
        WriteToLog( _L8( "CXdmXmlParser::ParseDocumentL() - Parsing took %d milliseconds" ), finish - start ); 
    #endif
    }

// ----------------------------------------------------------
// CXdmXmlParser::ParseDocumentL
// 
// ----------------------------------------------------------
// 
EXPORT_C void CXdmXmlParser::ParseDocumentL( const TDesC8& aXmlDocument,
                                             CXdmDocumentNode* aDocumentRoot )  
    {
    #ifdef _DEBUG
        TInt start = CXdmXmlParser::TimeL();
    #endif
    User::LeaveIfError( iXmlParser->EnableFeature( EReportNamespaceMapping ) );
    iContentHandler->SetTarget( *aDocumentRoot );
    Xml::ParseL( *iXmlParser, aXmlDocument );
    iContentHandler->Reset();
    //self->ParseL( aXmlDocument, aDocumentRoot );
    #ifdef _DEBUG
        TInt finish = CXdmXmlParser::TimeL();
        WriteToLog( _L8( "CXdmXmlParser::ParseDocumentL() - Parsing took %d milliseconds" ), finish - start ); 
    #endif
    }
    
// ----------------------------------------------------------
// CXdmXmlParser::ParseDocumentL
// 
// ----------------------------------------------------------
// 
EXPORT_C void CXdmXmlParser::ParseDocumentL( CXdmDocument* aDocument,
                                             const TDesC8& aXmlDocument,
                                             CXdmDocumentNode* aDocumentSubset )  
    {
    #ifdef _DEBUG
        TInt start = CXdmXmlParser::TimeL();
    #endif
    User::LeaveIfError( iXmlParser->EnableFeature( EReportNamespaces ) );
    User::LeaveIfError( iXmlParser->EnableFeature( EReportNamespaceMapping ) );
    User::LeaveIfError( iXmlParser->EnableFeature( EReportNamespacePrefixes ) );
    iContentHandler->SetTargetL( *aDocument, *aDocumentSubset );
    Xml::ParseL( *iXmlParser, aXmlDocument );
    iContentHandler->Reset();
    //self->ParseL( aDocument, aXmlDocument, aDocumentSubset );
    #ifdef _DEBUG
        TInt finish = CXdmXmlParser::TimeL();
        CXdmXmlParser::WriteToLog( _L8(
            "CXdmXmlParser::ParseDocumentL() - Parsing took %d milliseconds" ), finish - start ); 
    #endif
    }
    
// ----------------------------------------------------------
// CXdmXmlParser::FormatToXmlLC
// 
// ----------------------------------------------------------
//       
EXPORT_C HBufC8* CXdmXmlParser::FormatToXmlLC( TBool aIsWholeDocument,
                                               const CXdmDocument* aDocument, 
                                               const CXdmDocumentNode* aRootNode )
    {
    #ifdef _DEBUG
        TInt start = CXdmXmlParser::TimeL();
    #endif
    HBufC8* ret = iXmlFormatter->FormatLC( aIsWholeDocument, aDocument, aRootNode );
    #ifdef _DEBUG
        DumpDocumentL( ret );
        TInt finish = CXdmXmlParser::TimeL();
        CXdmXmlParser::WriteToLog( _L8(
            "CXdmXmlParser::FormatToXmlLC() - Formatting took %d milliseconds" ), finish - start ); 
    #endif
    return ret;  
    }

// ----------------------------------------------------------
// CXdmXmlParser::FormatToXmlLC
// 
// ----------------------------------------------------------
//      
EXPORT_C HBufC8* CXdmXmlParser::FormatToXmlLC( const TDesC8& aXmlFragment,
                                               const CXdmDocument* aDocument,
                                               const CXdmDocumentNode* aTargetNode )
    {
    #ifdef _DEBUG
        TInt start = CXdmXmlParser::TimeL();
    #endif
    //HBufC8* ret = self->FormatLC( aXmlFragment, aDocument, aTargetNode );
    HBufC8* ret = iXmlFormatter->FormatLC( aXmlFragment, aDocument, aTargetNode );
    #ifdef _DEBUG
        DumpDocumentL( ret );
        TInt finish = CXdmXmlParser::TimeL();
        CXdmXmlParser::WriteToLog( _L8(
            "CXdmXmlParser::FormatToXmlLC() - Formatting took %d milliseconds" ), finish - start ); 
    #endif
    return ret;  
    }
    
// ---------------------------------------------------------
// CXdmXmlParser::FinishParsing
//
// ---------------------------------------------------------
//
void CXdmXmlParser::FinishParsingL()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CXdmXmlParser::FinishParsing()" ) );
    #endif
    iXmlParser->ParseEndL();
    }

#ifdef _DEBUG 

// ---------------------------------------------------------
// CXcapPartDocOperation::DumpDocumentL
//
// ---------------------------------------------------------
//
void CXdmXmlParser::DumpDocumentL( HBufC8* aDocData ) 
    {
    if( aDocData )
        {
        RFile file;
        RFs session;
        TPtr8 pointer( aDocData->Des() );
        _LIT( KXmlFileExtension, ".xml" );
        TBuf<128> nameBuf( _L( "C:\\logs\\XDM\\request" ) );
        nameBuf.AppendNum( iDumpIndex );
        nameBuf.Append( KXmlFileExtension );
        User::LeaveIfError( session.Connect() );
        TInt error( file.Replace( session, nameBuf, EFileWrite ) );
        if( error == KErrNone )
            {
            file.Write( pointer );
            file.Close();
            iDumpIndex++;
            }
        session.Close();
        }
    }
    
// ----------------------------------------------------------
// CXdmXmlParser::TimeL
// 
// ----------------------------------------------------------
//
TInt CXdmXmlParser::TimeL()
    {
    TInt period = 0;
    User::LeaveIfError( HAL::Get( HALData::ESystemTickPeriod, period ) );
    TInt millisecsPerTick = period / 1000;
    return User::TickCount() * millisecsPerTick;
    }

// ----------------------------------------------------------
// CXdmXmlParser::WriteToLog
// 
// ----------------------------------------------------------
//
void CXdmXmlParser::WriteToLog( TRefByValue<const TDesC8> aFmt,... ) const                                
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    iLogWriter->WriteToLog( buf );
    }

#endif




