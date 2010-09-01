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
* Description:   CXmlParserNodePath
*
*/




#ifndef __XMLPARSERNODEPATH__
#define __XMLPARSERNODEPATH__

// INCLUDES
//#include <parser.h>
#include <e32base.h>
#include <badesca.h>
#include <contenthandler.h>

class CXdmXmlParser;
class CXdmDocumentNode;
class MXmlNodePathInterface;

// CLASS DECLARATION
NONSHARABLE_CLASS( CXmlParserNodePath ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        static CXmlParserNodePath* NewL( CXdmXmlParser& aParserMain,
                                         TXdmElementType iElementType,
                                         const CXdmDocumentNode& aTargetNode,
                                         MXmlNodePathInterface* aPathInterface );
        
        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        TBool CheckNextNode( const CXdmDocumentNode& aNextNode );
        
        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        TBool CheckNextNodeL( const TDesC8& aNextElement,
                              const Xml::RAttributeArray& aAttributes );
                              
        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        virtual ~CXmlParserNodePath();
        
    private:
        
        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        CXmlParserNodePath( CXdmXmlParser& aParserMain,
                            TXdmElementType aElementType,
                            MXmlNodePathInterface* aPathInterface );
        
        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        void ConstructL( const CXdmDocumentNode& aTargetNode );
        
    private: //Data
        
        TInt                                iIndex;
        TBool                               iComplete;
        TXdmElementType                     iElementType;
        MXmlNodePathInterface*              iPathInterface;
        RPointerArray<CXdmDocumentNode>     iNodes;
        CXdmXmlParser&                      iParserMain;
    };

#endif  //__XMLPARSERNODEPATH__


// End of File

