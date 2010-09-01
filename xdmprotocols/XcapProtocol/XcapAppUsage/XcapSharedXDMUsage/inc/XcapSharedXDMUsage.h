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
* Description:   CXcapSharedXDMUsage
*
*/




#ifndef __XCAPSHAREDXDMUSAGE__
#define __XCAPSHAREDXDMUSAGE__

#include "XcapAppUsage.h"


// CLASS DECLARATION
class CXcapSharedXDMUsage : public CXcapAppUsage
    {
    public:
        
        /**
        * Symbian OS constructor
        * @param TXcapAppUsageParams Parameters
        * @return CXcapSharedXDMUsage*
        */
        static CXcapSharedXDMUsage* NewL( const TXcapAppUsageParams& aParameters );
        
        /**
        * Destructor
        */
        virtual ~CXcapSharedXDMUsage();
        
    public:  // From CXcapAppUsage
       
        /**
        * AUID for this usage
        * @return AUID
        */
        TPtrC8 AUID() const;
        
        /**
        * MIME type for this usage
        * @return MIME type
        */
        TPtrC8 ContentType() const;

        /**
        * Default namespace for this usage
        * @return Default namespace definition
        */
        TPtrC8 DefaultNamespace() const;
                        
        /**
        * Validate the node in the parameter
        * Leaves with system wide error codes or
        * with error defined in XdmErrors.h
        * @param CXdmDocumentNode& reference to the node
        * @return ETrue, if the node belongs to the appusage's namespace
        */
        TBool ValidateNodeL( CXdmDocumentNode& aXdmNode );
                        
        /**
        * Validate the attribute in the parameter
        * Leaves with system wide error codes or
        * with error defined in XdmErrors.h
        * @param CXdmNodeAttribute& reference to the attribute
        */
        void ValidateAttributeL( const CXdmNodeAttribute& aXdmNodeAttr );
        
        /**
        * Add namespaces and namespace prefixes used in document
        * @param CXdmDocument& reference to the document        
        */
        void AddNamespaceInformationL( CXdmDocument& aXdmDocument );

    private:
        
        /**
        * Second phase construction
        */
        void ConstructL();
        
        /**
        * Prohibit copy costructor
        */      
        CXcapSharedXDMUsage( const CXdmEngine& aXdmEngine );  
 
    private:    // Data
           
        CXcapAppUsage*      iResListUsage;  

    };

#endif    //__XCAPSHAREDXDMUSAGE__
            
// End of File
