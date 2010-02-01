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
* Description:    SIMPLE Protocol implementation for Presence Framework
*
*/




#ifndef C_SIMPLEPLUGINDATA_H
#define C_SIMPLEPLUGINDATA_H


// Includes
#include <e32std.h>

class MXIMPObjectFactory;
class MXIMPIdentity;

class MPresenceObjectFactory;
class MSimpleDocument;
class MPresenceInfo;
class MSimplePresenceList;
class MPresenceInfoFieldCollection;
class MPersonPresenceInfo;
class MPresenceInfoFieldValueEnum;
class MSimpleElement;
class MPresenceInfoFieldValueText;


/**
 *  CSimplePluginData
 *
 *  This provides data conversion between SIMPLE Engine and PrFw data strcutres.
 *
 *  @lib simplexmlutils
 *  @since s60 v5.0
 */
NONSHARABLE_CLASS(CSimplePluginData) : public CBase
    {
public:

    static CSimplePluginData* NewL(  );

    static CSimplePluginData* NewLC();

    virtual ~CSimplePluginData();

    /**
     * Convert to engine notification to MPresenceInfo
     * @param aPresenceFactory PrFw Presence Factory
     * @param aDocument notification
     * @param aPrInfo PrFW info structure [out]
     */
    static void NotifyToPrInfoL(
        MPresenceObjectFactory& aPresenceFactory,        
        MSimpleDocument& aDocument,
        MPresenceInfo& aPrInfo );

    /**
     * Convert to engine list notification to MPresenceInfo elements
     * @param aFactory PrFw Factory
     * @param aPresenceFactory PrFw Presence Factory
     * @param aList notification
     * @param aEntities array of PrFW info structure [out]
     * @param aTerminated array of PrFW info structure for terminated  entities [out]
     * @param aFullList ETrue if full state list [out]
     */
    static void NotifyListToPrInfoL(
        MXIMPObjectFactory& aFactory,
        MPresenceObjectFactory& aPresenceFactory,        
        MSimplePresenceList& aList,
        RPointerArray<MPresenceInfo>& aEntities,
        RPointerArray<MXIMPIdentity>& aActives,
        RPointerArray<MXIMPIdentity>& aTerminated,
        TBool& aFullList );

    /**
     * Converts MPersonPresenceInfo to MSimpleDocument elements
     * @param aInfo Person's presence info [in]
     * @param aDocument PIDF document containing Person's presence info [out] 
     * @param aSipId Person's SIP indetity
     */
    static void AddPrPersToSimpleDocumentL(
        const MPersonPresenceInfo* aInfo,
        MSimpleDocument& aDocument,
        const TDesC8& aSipId );


private:

    CSimplePluginData();

    void ConstructL( );

    static void UserOverridingWillingnessToPrFwL(
        MPresenceObjectFactory& aPresenceFactory,        
        MSimpleElement* aElement,
        MPresenceInfoFieldCollection& aCollection );

    static void UserStatusIconToPrFwL(
        MPresenceObjectFactory& aPresenceFactory,        
        MSimpleElement* aElement,
        MPresenceInfoFieldCollection& aCollection,
        MSimpleDocument& aDocument );

    static void UserNoteToPrFwL(
        MPresenceObjectFactory& aPresenceFactory,        
        MSimpleElement* aElement,
        MPresenceInfoFieldCollection& aCollection );
        
    static void CollectAllPresentitiesL( 
        RPointerArray<MPresenceInfo>& aEntities,
        RPointerArray<MXIMPIdentity>& aActives,        
        RPointerArray<MSimpleDocument>& aDocs,         
        MXIMPObjectFactory& aFactory,
        MPresenceObjectFactory& aPresenceFactory );
        
    /**
     * Add terminated instances into array
     * @param aTerminated list of PrFw indetities [OUT]
     * @param aElems RLS meta data list entry elements
     * @param aFactory PrFw Factory
     */     
    static void CollectTerminatedPresentitiesL( 
        RPointerArray<MXIMPIdentity>& aTerminated,
        RPointerArray<MSimpleElement>& aElems,    
        MXIMPObjectFactory& aFactory );
        
    /** 
     * Search the latest timestamp in the elements
     * @param aElems element array     
     * @retun position of the latest element in the element array. KErrNotFound if not found.    
     */
    static TInt SearchLatestTimestampL( 
        RPointerArray<MSimpleElement>& aElems  );
        
    /**
     * Add availability info into SIMPLE document
     * @param aText new value [IN]
     * @param aPersElem personal data to be modified [OUT]     
     */    
    static void AddPrPersAvailabilityToDocL( 
        const MPresenceInfoFieldValueText* aText, 
        MSimpleElement* aPersElem );
    
    /**
     * Check if the current element is terminated instance.
     * If yes then add the aUri into aTerminated
     * @param aTerminated list of PrFw indetities [OUT]
     * @param aElem current element
     * @param aUri8 corresponding sip entity URI
     * @param aFactory PrFw Factory
     */
    static void SearchTerminatedInstanceL( 
            RPointerArray<MXIMPIdentity>& aTerminated,
            MSimpleElement* aElem,    
            const TDesC8* aUri8,
            MXIMPObjectFactory& aFactory );
        
      

};



#endif // C_SIMPLEPLUGINDATA_H
