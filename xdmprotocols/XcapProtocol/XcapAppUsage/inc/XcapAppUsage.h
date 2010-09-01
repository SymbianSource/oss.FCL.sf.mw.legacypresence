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
* Description:  CXcapAppUsage
*
*/




#ifndef __XCAPAPPUSAGE__
#define __XCAPAPPUSAGE__

// INCLUDES
#include <ecom.h>

#include <XdmEngine.h>
#include <XdmProtocolUidList.h>
#include "XcapAppUsageDef.h"

// CONSTANTS
#ifdef _DEBUG
_LIT( KXdmAppUsageLogDir,                          "Xdm" );
_LIT( KXdmAppUsageLogFile,                         "XcapAppUsage.txt" );
const TInt KAppUsageLogBufferMaxSize                = 2000;
#endif

_LIT( KXdmDefaultNsAttrName, "xmlns");
_LIT( KXdmNonDefaultNsAttrName, "xmlns:");
const TInt KXdmMaxPrefixLength = 10;

// FORWARD DECLARATIONS
class CXdmDocument;
class MXcapUriInterface;

// NOTE
// If _NAMESPACES_TO_NODE_SELECTOR flag is defined, used namespaces will be added to the node selector
// when updating partial documents.
// The default namespace of the usage will not be added.
#define _NAMESPACES_TO_NODE_SELECTOR

// If _ADD_NAMESPACE_ATTRIBUTES flag is defined, namespace information is
// added as an node atttribute when updating partial documents.
// The default namespace of the usage will not be added.
#define _ADD_NAMESPACE_ATTRIBUTES

/**
* A helper class for  parameter passing from
* the XCAP protocol to the application usage
*
* NOTE: There will most probably be more than one parameter!
*
*/
class TXcapAppUsageParams
    {
    public:
        
        /**
        * @param CXdmEngine& XDM Engine
        * @return TXcapAppUsageParams
        */
        TXcapAppUsageParams( const CXdmEngine& aXdmEngine ) :
                             iXdmEngine( aXdmEngine ){}
    
    public:
    
        const CXdmEngine&    iXdmEngine;
    };

// CLASS DECLARATION
/**
* Application usage base class
*/
class CXcapAppUsage : public CBase
    {
    public:

        /**
        * Symbian OS constructor
        * @param aXdmEngine XdmEngine reference
        * @param aDocumentType Type of the document
        * @return CXcapAppUsage*
        * REMOVE THIS WHEN USAGE IN ENGINE CHANGED!!!
        */
        static CXcapAppUsage* NewL( const CXdmEngine& aXdmEngine,
                                    const TXdmDocType aDocumentType );
                                    
        /**
        * Symbian OS constructor
        * @param aXdmEngine XdmEngine reference
        * @param aDocumentType Type of the document
        * @return CXcapAppUsage*
        */
        IMPORT_C CXcapAppUsage* NewL( const CXdmEngine& aXdmEngine,
                                    const TInt aDocumentType );
        
        /**
        * Destructor
        */
        virtual ~CXcapAppUsage();
    
        /**
        * Validate the document in the parameter.
        * Calls ValidateNodeL( aXdmNode ) method from derived class
        * for each node in document and ValidateAttributeL( aXdmNodeAttr )
        * for each attribute in document. If the document is outgoing, 
        * calls also AddNamespaceInformationL method from derived class.
        * @param aXdmDocument document reference
        * @param aAddNamespaces if true, the namespaces will be added
        * @return KErrNone or error code.
        */
        IMPORT_C TInt Validate( CXdmDocument& aXdmDocument, TBool aAddNamespaces ) ;
                                 
        /**
        * Validate the node in the parameter.
        * Calls ValidateNodeL( aXdmNode ) method from derived class for 
        * current node and it's descendants and ValidateAttributeL( aXdmNodeAttr )
        * for each attribute. Stores the uri of the node.
        * @param aXdmNode node reference
        * @param TBool aIsGetOrDelete indication of http get or delete
        * @return KErrNone or error code.
        */
        IMPORT_C TInt Validate( CXdmDocumentNode& aXdmNode, 
                                MXcapUriInterface* aUriInterface,
                                TBool aIsGetOrDelete ) ;
                                                     
    
    
    public:  // Application Usage API
        
        /**
        * AUID for this usage
        * @return AUID
        */
        virtual TPtrC8 AUID() const = 0;
           
        /**
        * MIME type for this usage
        * @return MIME type
        */
        virtual TPtrC8 ContentType() const = 0;
        
        /**
        * Default namespace for this usage
        * @return Default namespace definition
        */
        virtual TPtrC8 DefaultNamespace() const = 0;
        
        /**
        * Validate the node in the parameter
        * This method is called by base class for each node
        * in document, here we have to check element datatype,
        * restrictions for values and also do all checking that concerns  
        * the structure of the element. If the datatype is some of
        * the common datatypes defined in xcapappusage.h, the node
        * can pe passed to the base class for value validation.
        * @param CXdmDocumentNode& node reference
        * @return True, if the node belongs to the current namespace
        */
        virtual TBool ValidateNodeL( CXdmDocumentNode& aXdmNode ) = 0;
        
        /**
        * Validate the attribute in the parameter
        * This method is called by base class for each attribute
        * in document, here we have to define data types
        * for attributes, and pass them to the base class
        * for the actual data checking.
        * @param aXdmNodeAttr attribute reference
        */
        virtual void ValidateAttributeL( const CXdmNodeAttribute& aXdmNodeAttr ) = 0;
          
        /**
        * Add namespaces and namespace prefixes used in document
        * @param aXdmDocument document reference
        */
        virtual void AddNamespaceInformationL( CXdmDocument& aXdmDocument ) = 0;
                              
                              
    protected:
        
        /**
        * C++ constructor
        */
        CXcapAppUsage( const CXdmEngine& aXdmEngine );
    
        /**
        * Validates basic data types
        * @param aDataType type of the data
        * @param aXdmNode node reference
        */
        void ValidateDataL ( TDataType aDataType, CXdmDocumentNode& aXdmNode );
       
       /**
        * Validates basic data types
        * @param aDataType type of the data
        * @param aXdmNodeAttr attribute reference
        */
        void ValidateDataL ( TDataType aDataType, const CXdmNodeAttribute& aXdmNodeAttr );
        
        /**
        * If outgoing document, adds namespacePrefix 
        * to the node in parameter
        * @param aXdmNode node reference
        * @param aPrefix namespace prefix
        */
        void SetPrefixL( CXdmDocumentNode& aXdmNode, const TDesC8& aPrefix );
        
        /**
        * Helper method for string comparison
        * @param aLeft first string
        * @param aRight second string
        * @return ETrue, if the strings are equal
        */
        TBool Match( const TDesC& aLeft, const TDesC& aRight );
        
        /**
        * Helper method for string comparison
        * @param aLeft first string
        * @param aRight second string
        * @return ETrue, if the strings are equal
        */
        TBool Match( const TDesC8& aLeft, const TDesC8& aRight );
        
        /**
        * To be used when an error occurs, however if the node is
        * for http get or delete request, this do not leave
        * @param aErrorCode function will leave with this error code
        */
        void LeaveWithErrorL( TInt aErrorCode );
        
#ifdef _DEBUG        
        /**
        * Writes logs to file.
        */
        void WriteToLog( TRefByValue<const TDesC8> aFmt,... );
#endif
    
    private:
        
        /**
        * Validate the document in the parameter.
        * @param aXdmDocument document reference
        */
        void DoValidateL( CXdmDocument& aXdmDocument ) ;
        
        /**
        * Validate the node in the parameter. 
        * @param aXdmNode node reference
        */
        void DoValidateL( CXdmDocumentNode& aXdmNode ) ;
        
        /**
        * Validates basic data types
        * @param aDataType type of the data
        * @param aData validated data
        */
        void ValidateDataL ( TDataType aDataType, TDesC8& aData );
        
        /**
        * Updates member variables for used namespaces
        * @param aPrefix prefix of the namespace
        */        
        void AddUsedNamespace( const TDesC8& aPrefix ) ;

        /**
        * Resets member variables for used namespaces
        */
        void ResetUsedNamespaces( ); 
        
        /**
        * Appends namespace information to the node's uri interface. 
        * Called if _NAMESPACES_TO_NODE_SELECTOR defined.
        * @param aUriInterface interface for storing the uri
        */
        void AppendUsedNamespacesL( MXcapUriInterface* aUriInterface ); 
        
        /**
        * Creates namespace attribute for the node. 
        * Called if _ADD_NAMESPACE_ATTRIBUTES is defined
        * and validating partial document.
        * @param aXdmNode node for storing the uri
        */
        void CreateNamespaceAttributesL( CXdmDocumentNode& aXdmNode, MXcapUriInterface* aUriInterface );
           
        /**
        * Removes namespace attributes from the node
        * and it's child nodes
        * @param aXdmNode 
        */
        void RemoveNamespaceAttributesL( CXdmDocumentNode& aXdmNode ); 
        
        /**
        * Adds namespace attributes that are used in 
        * partial document
        * @param aXdmNode 
        */
        void AppendNamespaceAttributesL( CXdmDocumentNode& aXdmNode ) ;
        
        /**
        * Creates namespace attribute for node
        * @param aXdmNode 
        * @param aPrefix prefix of the namespace
        * @param aNamespace namespace definition
        */
        void CreateNamespaceAttributeL( CXdmDocumentNode& aXdmNode, const TDesC8& aPrefix, const TDesC8& aNamespace );
        
        
        /**
        * Goes through all nodes and checks if 
        * default namespace is used in document fragment
        * the namespace given in parameter
        * @param aXdmNode 
        * @return ETrue or EFalse
        */
        TBool DefaultNamespaceUsed( CXdmDocumentNode& aXdmNode );
        
    protected:  // Data
    
        CXdmEngine&                 iXdmEngine;
       
    private:  // Data
    
        TBool                       iAddNamespaces;
        TBool                       iNsToNodeSelector;
        TBool                       iAddNsAttributes;
        TBool                       iIsGetOrDelete;
        TUid                        iDestructorKey;
		// Variables for used namespaces
		// Add new one here if needed
        TBool iRL;
        TBool iCR;
        TBool iCP ;
        TBool iPR;
        TBool iLS;
        TBool iPOC;
        TBool iOPR;
        TBool iRLS;
        TBool iOU;
    // add new prefix definitions here 


    };


// ----------------------------------------------------
// CXcapAppUsage::NewL
// Create a new application usage implementation
// REMOVE THIS WHEN USAGE IN ENGINE CHANGED!!!
// ----------------------------------------------------
//
inline CXcapAppUsage* CXcapAppUsage::NewL( const CXdmEngine& aXdmEngine,
                                           const TXdmDocType aDocumentType )
    {
    TAny* ptr = NULL;
    TXcapAppUsageParams params( aXdmEngine );
    switch( aDocumentType )
        {
        case EXdmCapabilities:
            {
            TUid KDefaultImplementationUid = { 0x10207418 };
            TRAPD( error, ptr = REComSession::CreateImplementationL( 
                   KDefaultImplementationUid,
                   _FOFF( CXcapAppUsage, iDestructorKey ),
                   ( TAny* )&params ) );
            }
            break;
        case EXdmDirectory:
            {
            TUid KDefaultImplementationUid = { 0x10207419 };
            TRAPD( error, ptr = REComSession::CreateImplementationL( 
                   KDefaultImplementationUid,
                   _FOFF( CXcapAppUsage, iDestructorKey ),
                   ( TAny* )&params ) );
            }
            break;
        case EXdmTestDocType:
            {
            TUid KDefaultImplementationUid = { 0x1020741A };
            TRAPD( error, ptr = REComSession::CreateImplementationL( 
                   KDefaultImplementationUid,
                   _FOFF( CXcapAppUsage, iDestructorKey ),
                   ( TAny* )&params ) );
            }
            break;
        case EXdmPoCGroup:
            {
            TUid KDefaultImplementationUid = { 0x1020741B };
            TRAPD( error, ptr = REComSession::CreateImplementationL( 
                   KDefaultImplementationUid,
                   _FOFF( CXcapAppUsage, iDestructorKey ),
                   ( TAny* )&params ) );
            }
            break;
        case EXdmPoCUserAccessPolicy:
            {
            TUid KDefaultImplementationUid = { 0x1020741C };
            TRAPD( error, ptr = REComSession::CreateImplementationL( 
                   KDefaultImplementationUid,
                   _FOFF( CXcapAppUsage, iDestructorKey ),
                   ( TAny* )&params ) );
            }
            break;
        case EXdmSharedXdm:
            {
            TUid KDefaultImplementationUid = { 0x1020741D };
            TRAPD( error, ptr = REComSession::CreateImplementationL( 
                   KDefaultImplementationUid,
                   _FOFF( CXcapAppUsage, iDestructorKey ),
                   ( TAny* )&params ) );
            }
            break;
       case EXdmRlsServices:
            {
            TUid KDefaultImplementationUid = { 0x1020741E };
            TRAPD( error, ptr = REComSession::CreateImplementationL( 
                   KDefaultImplementationUid,
                   _FOFF( CXcapAppUsage, iDestructorKey ),
                   ( TAny* )&params ) );
            }
            break;
      case EXdmResourceLists:
            {
            TUid KDefaultImplementationUid = { 0x10275080 };
            TRAPD( error, ptr = REComSession::CreateImplementationL( 
                   KDefaultImplementationUid,
                   _FOFF( CXcapAppUsage, iDestructorKey ),
                   ( TAny* )&params ) );
            }
            break;
      case EXdmIetfCommonPolicy:
            {
            TUid KDefaultImplementationUid = { 0x10275081 };
            TRAPD( error, ptr = REComSession::CreateImplementationL( 
                   KDefaultImplementationUid,
                   _FOFF( CXcapAppUsage, iDestructorKey ),
                   ( TAny* )&params ) );
            }
            break;
      case EXdmOmaCommonPolicy:
            {
            TUid KDefaultImplementationUid = { 0x10275082 };
            TRAPD( error, ptr = REComSession::CreateImplementationL( 
                   KDefaultImplementationUid,
                   _FOFF( CXcapAppUsage, iDestructorKey ),
                   ( TAny* )&params ) );
            }
            break;
      case EXdmIetfPresenceRules:
            {
            TUid KDefaultImplementationUid = { 0x10275083 };
            TRAPD( error, ptr = REComSession::CreateImplementationL( 
                   KDefaultImplementationUid,
                   _FOFF( CXcapAppUsage, iDestructorKey ),
                   ( TAny* )&params ) );
            }
            break;
      case EXdmOmaPresenceRules:
            {
            TUid KDefaultImplementationUid = { 0x102750AB };
            TRAPD( error, ptr = REComSession::CreateImplementationL( 
                   KDefaultImplementationUid,
                   _FOFF( CXcapAppUsage, iDestructorKey ),
                   ( TAny* )&params ) );
            }
            break;
// PRES-CONTENT STARTS
      case EXdmPresContent:
            {
            TUid KDefaultImplementationUid = { 0x20028701 };
            TRAPD( error, ptr = REComSession::CreateImplementationL( 
                KDefaultImplementationUid,
                _FOFF( CXcapAppUsage, iDestructorKey ),
               ( TAny* )&params ) );
            }
            break;
// PRES-CONTENT ENDS
        default:
            User::Leave( KErrNotSupported );
        }
    return reinterpret_cast<CXcapAppUsage*>( ptr );
    }
    
    
#endif          //__XCAPAPPUSAGE__


// End of File

