/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IETF SIMPLE Protocol implementation for XIMP Framework
*
*/


#ifndef C_PRESENCEPLUGINDATA_H
#define C_PRESENCEPLUGINDATA_H

#include <e32std.h>
#include <e32base.h>
#include <presenceinfo.h>
#include <mpresencebuddyinfo2.h>

#include "mpresencepluginconnectionobs.h"
#include "presencelogger.h"

class MXIMPObjectFactory;
class MPresenceObjectFactory;
class MSimpleDocument;
class MPresenceInfo;
class MSimplePresenceList;
class MPresenceInfoFieldCollection;
class MXIMPObjectCollection;
class MPersonPresenceInfo;
class MPresenceInfoField;
class MSimpleElement;
class MPresenceCacheWriter2;

/**
 * CPresencePluginData
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CPresencePluginData ): public CBase
    {
    public:
    
        /**
         * Two-phased constructor.
         *
         * @param none
         * @param none
         */
        static CPresencePluginData* NewL( 
			MPresencePluginConnectionObs& aObs,
			TInt aServiceId );

        /**
         * Two-phased constructor.
         *
         * @param none
         * @param none
         */
        static CPresencePluginData* NewLC( 
            MPresencePluginConnectionObs& aObs,
            TInt aServiceId );

        /**
         * Standard C++ destructor
         */
        virtual ~CPresencePluginData();

        /**
         * Convert to engine notification to MXIMPPresenceInfo
         *
         * @since S60 5.0
         * @param aPresenceFactory object factory
         * @param aPrInfo ximp info structure [out]
         * @param aUnicodeNoteContent note field content [out]
         */
        void NotifyToBlockedToXIMPL(
            MPresenceObjectFactory& aPresenceFactory,
            MPresenceInfo& aPrInfo,
            const TDesC& aUnicodeNoteContent = KNullDesC );

        /**
         * Convert to engine notification to MXIMPPresenceInfo
         *
         * @since S60 3.2
         * @param aFactory ximp Factory
         * @param aPrInfo ximp info structure [out]
         * @param aUnicodeNoteContent note field content [out]
         */
        void NotifyToPendingToXIMPL(
            MPresenceObjectFactory& aPresenceFactory,
            MPresenceInfo& aPrInfo,
            const TDesC& aUnicodeNoteContent = KNullDesC );
            
        /**
         * Convert to engine notification to MXIMPPresenceInfo
         *
         * @since S60 3.2
         * @param aFactory ximp Factory
         * @param aPrInfo ximp info structure [out]
         * @param aUnicodeNoteContent note field content [out]
         */
        void NotifyTerminatedToXIMPL(
            MPresenceObjectFactory& aPresenceFactory,
            MPresenceInfo& aPrInfo,
            const TDesC& aUnicodeNoteContent = KNullDesC );
            
        /**
         * Convert to engine notification to MXIMPPresenceInfo
         *
         * @since S60 3.2
         * @param aFactory ximp Factory
         * @param aPrInfo ximp info structure [out]
         * @param aUnicodeNoteContent note field content [out]
         * @param aAvailability availability value for cache entry [out]
         */
        void NotifyToActiveToXIMPL(
            MPresenceObjectFactory& aPresenceFactory,
            MPresenceInfo& aPrInfo,
            const TDesC& aUnicodeNoteContent = KNullDesC,
            NPresenceInfo::TAvailabilityValues aAvailability = 
                NPresenceInfo::EAvailable );
            
        /**
         * Convert to engine notification to MximpPresenceInfo
         *
         * @since S60 3.2
         * @param aFactory ximp Factory
         * @param aDocument notification
         * @param aPrInfo ximp info structure [out]
         */
        void NotifyToPrInfoL(
            MXIMPObjectFactory& aFactory,
            MPresenceObjectFactory& aPresenceFactory,
            MSimpleDocument& aDocument,
            MPresenceInfo& aPrInfo );
           
        /**
         * Extracts cache data entries from presence info object.
         *
         * @since S60 5.0
         * @param aPrInfo ximp info structure
         * @param aAvailability will store cache availability
         * @param aExtendedAvailability will store extended availability
         * @param aStatusMessage will store custom status message
         */            
        void CacheEntriesFromPrInfo(
            MPresenceInfo& aPrInfo,
            MPresenceBuddyInfo2::TAvailabilityValues& aAvailability,
            TPtr& aExtendedAvailability,
            TPtr& aStatusMessage );

        /**
         * Convert to engine list notification to MximpPresenceInfo elements
         *
         * @since S60 3.2         
         * @param aFactory ximp Factory
         * @param aList notification
         * @param aEntities array of ximp info structure [out]
         * @param aTerminated array of ximp info structure for
         *        terminated  entities [out]
         */
        void NotifyListToPrInfoL(
            MXIMPObjectFactory& aFactory,
            MPresenceObjectFactory& aPresenceFactory,
            MSimplePresenceList& aList,
            RPointerArray<MPresenceInfo>& aEntities,
            RPointerArray<MXIMPIdentity>& aActives,
            RPointerArray<MXIMPIdentity>& aTerminated );

        /**
         * AddPrPersToSimpleDocumentL
         *
         * @since S60 3.2         
         * Add subscribed or blocked person to document
         * @param aInfo, presence info
         * @param aDocument, xml document
         * @param aSipId, sip identity
         * @param aTupleId, client tupleId
         */
        void AddPrPersToSimpleDocumentL(
            const MPersonPresenceInfo* aInfo,
            MSimpleDocument& aDocument,
            const TDesC8& aSipId,
            TInt aTupleId );
        
        /**
         * Creates document.
         *
         * @since S60 3.2         
         * @param aDocument XDM document
         * @param aSipId SIP profile id
         * @param aStatusField, status enum field 
         * @param aMessageField, message field 
         */    
        void DoCreateDocumentL(
            MSimpleDocument& aDocument,
            const TDesC8& aSipId,
            const MPresenceInfoField* aStatusField,
            const MPresenceInfoField* aMessageField ); 
        
        /**
         * DoCheckBasicStatusValueL
         *
         * @since S60 3.2         
         * @param aField 
         * @return TPtrC basic status value
         */ 
        TPtrC DoCheckBasicStatusValueL(
        	const MPresenceInfoField& aField );
        
        /**
         * DoCheckActivitiesValueL
         *
         * @since S60 3.2         
         * @param aField 
         * @return TPtrC activities value
         */ 
        TPtrC8 DoCheckActivitiesValueL(
        	const MPresenceInfoField& aField );
        
        /**
         * DoCheckNoteValueL
         *
         * @since S60 3.2         
         * @param aField 
         * @return TPtrC activities value
         */
        TPtrC DoCheckNoteValueL(
            const MPresenceInfoField& aField );
        
        /**
         * CreatePresenceUri8LC
         *
         * @since S60 3.2
         * @param aPresentityUri
         * @return presence URI
         */ 
        HBufC8* CreatePresenceUri8LC( 
            const TDesC8& aPresentityUri );          

        /**
         * GenerateTupleId()
         * Generate Tuple id for status document
         *
         * @since S60 3.2         
         * @param none
         * @return TInt, tupleid
         */
        TInt GenerateTupleId();
        
           
        /**
         * Removes prefix from uri.
         *
         * @since S60 3.2
         * @param aPresentityUri, presentity url
         * @return HBufC16, URL without prefix
         */            
        HBufC16* RemovePrefixLC( 
            const TDesC& aPresentityUri );
            
        /**
         * Returns service id.
         *
         * @since S60 5.0
         * @return TInt, service id related to this data instance
         */            
        TInt ServiceId();            
        
        /**
         * Writes status to presence cache
         *
         * @since S60 5.0
         * @param aPresentityId, identity of status
         * @param aAvailability, availability value to set
         * @param aAvailabilityText, text for extended states
         * @param aStatusMessage, status message from remote party
         * @return None
         */                    
        void WriteStatusToCacheL(
            const TDesC& aPresentityId, 
            MPresenceBuddyInfo2::TAvailabilityValues aAvailability,
            const TDesC& aAvailabilityText,
            const TDesC& aStatusMessage );
            
        /**
         * Removes cache entries for this service.
         *
         * @since S60 5.0
         * @return None
         */                             
        void RemoveCacheL();
        
        /**
         * Removes cache entries for certain person.
         *
         * @since S60 5.0
         * @param aIdentity, identity of presence entry
         * @return None
         */                                     
        void DeletePresenceL( const TDesC& aIdentity );
        
        /**
         * Stores own presence values to permanent storage.
         *
         * @since S60 5.0
         * @param aServiceId, service id
         * @param aAvailability, availability enumeration
         * @param aStatusMessage, custom status message
         * @return None
         */                                             
        void StorePresenceOwnPresenceL(
            const TUint aServiceId, 
            NPresenceInfo::TAvailabilityValues aAvailability,
            const TDesC& aStatusMessage );

        /**
         * Stores own presence document id to permanent storage.
         *
         * @since S60 5.0
         * @param aServiceId, service id
         * @param aAvailability, availability enumeration
         * @param aDocumentId, document id
         * @return None
         */                                            
        void StoreDocumentIdL(
            const TUint aServiceId, 
            const TDesC8& aDocumentId );
            
        /**
         * Reads document id from permanent storage.
         *
         * @since S60 5.0
         * @param aServiceId, service id
         * @param aDocumentId, document id
         * @return None
         */                                   
        void ReadDocumentIdL(
            const TInt aServiceId, 
            TDes8& aDocumentId );
            
        /**
         * Deletes permanent data from storage
         *
         * @since S60 5.0
         * @param aServiceId, service id
         * @return None
         */                                               
        void DeletePresenceVariablesL( const TInt aServiceId );
        
    private:

        /**
         * Standard C++ constructor
         */ 
        CPresencePluginData( 
            MPresencePluginConnectionObs& aObs,
            TInt aServiceId );
            
        /**
         * Standard C++ constructor
         */             
        void ConstructL();
        
        /**
         * UserNoteToXIMPL
         *
         * @since S60 3.2 
         * This method not yet used
         * @param aPresenceFactory ximp Factory
         * @param aElement Simple elemnt
         * @param aCollection Field Collection
         */ 
        void UserNoteToXIMPL(
            MPresenceObjectFactory& aPresenceFactory,
            MSimpleElement* aElement,
            MPresenceInfoFieldCollection& aCollection );
        
        /**
         * SingleUserStatusToXIMPL
         *
         * @since S60 3.2          
         * @param aPresenceFactory ximp Factory
         * @param aElement Simple element
         * @param aActivitiesElement Simple element
         * @param aNoteElement Simple element for note elems (can be null)
         * @param aCollection Field Collection
         */     
        void SingleUserStatusToXIMPL(
            MPresenceObjectFactory& aPresenceFactory,
            MSimpleElement* aBasicElement,
            MSimpleElement* aActivitiesElement,
            MSimpleElement* aNoteElement,
            MPresenceInfoFieldCollection& aCollection );

    private: // data

        /**
         * Generated tuple/person id
         * Own.
         */
        TInt  iNumBuf;

        /**
         * ximp Plugin connection observer
         * Not Own.
         */
        MPresencePluginConnectionObs& iConnObs;
        
        /**
         * Stores service id.
         */        
        TInt iServiceId;
        
        /**
         * Writes service status to presence cache.
         * Own.
         */        
        MPresenceCacheWriter2* iPresenceCacheWriter;          
        
        SIMPLE_UNIT_TEST( T_CPresencePluginData )
    };

#endif //C_PRESENCEPLUGINDATA_H
