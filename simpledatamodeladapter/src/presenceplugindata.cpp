/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32base.h>
#include <s32strm.h>
#include <e32def.h>
#include <e32svr.h>
#include <e32math.h> 
#include <utf.h>
#include <ximpbase.h>
#include <ximpobjectfactory.h>
#include <ximpobjectcollection.h>
#include <presenceobjectfactory.h>
#include <presenceinfo.h>
#include <personpresenceinfo.h>
#include <presenceinfofieldcollection.h>
#include <presenceinfofield.h>
#include <presenceinfofieldvaluetext.h>
#include <presenceinfofieldvalueenum.h>
#include <ximpidentity.h>
#include <msimpledocument.h>
#include <msimpleelement.h>
#include <msimplepresencelist.h>
#include <msimplemeta.h>
#include <simpleutils.h>
#include <presencecachewriter2.h>
#include <presencecachereader2.h>
#include <presencebuddyinfo.h>
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <spdefinitions.h>
#include <avabilitytext.h>
#include <cvimpstsettingsstore.h>
#include <escapeutils.h>

#include "presenceplugindata.h"
#include "presenceplugincommon.h"
#include "presencepluginvirtualgroup.h"
#include "presencepluginlanguagecodes.h"

// ---------------------------------------------------------------------------
// CPresencePluginData::CPresencePluginData
// ---------------------------------------------------------------------------
//
CPresencePluginData::CPresencePluginData( 
	MPresencePluginConnectionObs& aObs,
	TInt aServiceId ):
	iConnObs( aObs ), 
	iServiceId( aServiceId ),
	iPresenceCacheWriter( NULL ),
	iPresenceCacheReader( NULL )
    {
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::~CPresencePluginData
// ---------------------------------------------------------------------------
//
CPresencePluginData::~CPresencePluginData()
    {
    DP_SDA("CPresencePluginData::~CPresencePluginData");
    delete iPresenceCacheWriter;
    delete iPresenceCacheReader;
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::NewL
// ---------------------------------------------------------------------------
//
CPresencePluginData* CPresencePluginData::NewL( 
	MPresencePluginConnectionObs& aObs, TInt aServiceId )
    {
    CPresencePluginData* self = CPresencePluginData::NewLC( 
		aObs,
		aServiceId );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CPresencePluginData::ConstructL
// ---------------------------------------------------------------------------
//
void CPresencePluginData::ConstructL()
    {
    DP_SDA("CPresencePluginData::ConstructL");
    iPresenceCacheWriter =  MPresenceCacheWriter2::CreateWriterL();
    iPresenceCacheReader = MPresenceCacheReader2::CreateReaderL();
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::NewLC
// ---------------------------------------------------------------------------
//
CPresencePluginData* CPresencePluginData::NewLC(
    MPresencePluginConnectionObs& aObs, TInt aServiceId )
    {
    CPresencePluginData* self =
    	new( ELeave ) CPresencePluginData( aObs, aServiceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::NotifyToBlockedToXIMPL
// ---------------------------------------------------------------------------
//
void CPresencePluginData::NotifyToBlockedToXIMPL(
    MPresenceObjectFactory& aPresenceFactory,
    MPresenceInfo& aPrInfo,
    const TDesC& aUnicodeNoteContent )
    {
    DP_SDA("CPresencePluginData::NotifyToBlockedToXIMPL");
    MPersonPresenceInfo* persInfo =
        aPresenceFactory.NewPersonPresenceInfoLC();
    MPresenceInfoFieldCollection& coll = persInfo->Fields();
    
    //Set pending
    using namespace NPresenceInfo::NFieldType;
    using namespace NPresencePlugin::NPresenceStates;
    
    MPresenceInfoField* field = aPresenceFactory.NewInfoFieldLC();
    field->SetFieldTypeL( KAvailabilityEnum );
        
    // use mapped enums to pass status    
    MPresenceInfoFieldValueEnum* enumField = 
        aPresenceFactory.NewEnumInfoFieldLC();
    
    // HOX: BLOCKED ENUM NEEDED TO XIMP
    enumField->SetValueL( NPresenceInfo::ENotAvailable ); 
    field->SetFieldValue( enumField );
        
    coll.AddOrReplaceFieldL( field );
    CleanupStack::Pop( 2 );            // >> field, enumField      
    
    // Add status msg field
    MPresenceInfoField* noteField = aPresenceFactory.NewInfoFieldLC();
    noteField->SetFieldTypeL( KStatusMessage );
        
    MPresenceInfoFieldValueText* textField = 
        aPresenceFactory.NewTextInfoFieldLC();
    textField->SetTextValueL( aUnicodeNoteContent );
    
    noteField->SetFieldValue( textField );
        
    coll.AddOrReplaceFieldL( noteField );
    CleanupStack::Pop( 2 );            // >> noteField, enumField      
    
    aPrInfo.SetPersonPresenceL( persInfo );
    CleanupStack::Pop();   // >> persInfo
    DP_SDA("CPresencePluginData::NotifyToBlockedToXIMPL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::NotifyToPendingToXIMPL
// ---------------------------------------------------------------------------
//
void CPresencePluginData::NotifyToPendingToXIMPL(
    MPresenceObjectFactory& aPresenceFactory,
    MPresenceInfo& aPrInfo,
    const TDesC& aUnicodeNoteContent )
    {
    DP_SDA("CPresencePluginData::NotifyToPendingToXIMP");
    MPersonPresenceInfo* persInfo =
        aPresenceFactory.NewPersonPresenceInfoLC();
    MPresenceInfoFieldCollection& coll = persInfo->Fields();
    
    //Set pending
    using namespace NPresenceInfo::NFieldType;
    using namespace NPresencePlugin::NPresenceStates;
    
    MPresenceInfoField* field = aPresenceFactory.NewInfoFieldLC();
    field->SetFieldTypeL( KAvailabilityEnum );
        
    // use mapped enums to pass status    
    MPresenceInfoFieldValueEnum* enumField = 
        aPresenceFactory.NewEnumInfoFieldLC();
    enumField->SetValueL( NPresenceInfo::ERemotePending );
    field->SetFieldValue( enumField );
        
    coll.AddOrReplaceFieldL( field );
    CleanupStack::Pop( 2 );            // >> field, enumField      
    
    // Add status msg field
    MPresenceInfoField* noteField = aPresenceFactory.NewInfoFieldLC();
    noteField->SetFieldTypeL( KStatusMessage );
        
    MPresenceInfoFieldValueText* textField = 
        aPresenceFactory.NewTextInfoFieldLC();
    textField->SetTextValueL( aUnicodeNoteContent );
    
    noteField->SetFieldValue( textField );
        
    coll.AddOrReplaceFieldL( noteField );
    CleanupStack::Pop( 2 );            // >> noteField, enumField      
    
    aPrInfo.SetPersonPresenceL( persInfo );
    CleanupStack::Pop();   // >> persInfo
    DP_SDA("CPresencePluginData::NotifyToPendingToXIMP end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::NotifyActiveToPrInfoL
// ---------------------------------------------------------------------------
//
void CPresencePluginData::NotifyToActiveToXIMPL(
    MPresenceObjectFactory& aPresenceFactory,
    MPresenceInfo& aPrInfo,
    const TDesC& aUnicodeNoteContent,
    NPresenceInfo::TAvailabilityValues aAvailability )
    {
    DP_SDA("CPresencePluginData::NotifyToActiveToXIMPL");
    MPersonPresenceInfo* persInfo =
        aPresenceFactory.NewPersonPresenceInfoLC();
    MPresenceInfoFieldCollection& coll = persInfo->Fields();
    
    //Set pending
    using namespace NPresenceInfo::NFieldType;
    using namespace NPresencePlugin::NPresenceStates;
    
    MPresenceInfoField* field = aPresenceFactory.NewInfoFieldLC();
    field->SetFieldTypeL( KAvailabilityEnum );
        
    // use mapped enums to pass status    
    MPresenceInfoFieldValueEnum* enumField = 
        aPresenceFactory.NewEnumInfoFieldLC();
    enumField->SetValueL( aAvailability );
    field->SetFieldValue( enumField );
        
    coll.AddOrReplaceFieldL( field );
    CleanupStack::Pop( 2 );            // >> field, enumField      
    
    // Add status msg field
    MPresenceInfoField* noteField = aPresenceFactory.NewInfoFieldLC();
    noteField->SetFieldTypeL( KStatusMessage );
        
    MPresenceInfoFieldValueText* textField = 
        aPresenceFactory.NewTextInfoFieldLC();
    textField->SetTextValueL( aUnicodeNoteContent );
    
    noteField->SetFieldValue( textField );
        
    coll.AddOrReplaceFieldL( noteField );
    CleanupStack::Pop( 2 );            // >> noteField, enumField      
    
    aPrInfo.SetPersonPresenceL( persInfo );
    CleanupStack::Pop();   // >> persInfo
    DP_SDA("CPresencePluginData::NotifyToActiveToXIMPL end");
    }
        
// ---------------------------------------------------------------------------
// CPresencePluginData::NotifyTerminatedToPrInfoL
// ---------------------------------------------------------------------------
//
void CPresencePluginData::NotifyTerminatedToXIMPL(
    MPresenceObjectFactory& aPresenceFactory,
    MPresenceInfo& aPrInfo,
    const TDesC& aUnicodeNoteContent )
    {
    DP_SDA("CPresencePluginData::NotifyTerminatedToXIMPL");
    MPersonPresenceInfo* persInfo =
        aPresenceFactory.NewPersonPresenceInfoLC();
    MPresenceInfoFieldCollection& coll = persInfo->Fields();
    
    //Set pending
    using namespace NPresenceInfo::NFieldType;
    using namespace NPresencePlugin::NPresenceStates;
    
    MPresenceInfoField* field = aPresenceFactory.NewInfoFieldLC();
    field->SetFieldTypeL( KAvailabilityEnum );
        
    // use mapped enums to pass status    
    MPresenceInfoFieldValueEnum* enumField = 
        aPresenceFactory.NewEnumInfoFieldLC();
    enumField->SetValueL( NPresenceInfo::EOffline );
    field->SetFieldValue( enumField );
        
    coll.AddOrReplaceFieldL( field );
    CleanupStack::Pop( 2 );            // >> field, enumField      
    
    // Add status msg field
    MPresenceInfoField* noteField = aPresenceFactory.NewInfoFieldLC();
    noteField->SetFieldTypeL( KStatusMessage );
        
    MPresenceInfoFieldValueText* textField = 
        aPresenceFactory.NewTextInfoFieldLC();
    textField->SetTextValueL( aUnicodeNoteContent );
    
    noteField->SetFieldValue( textField );
        
    coll.AddOrReplaceFieldL( noteField );
    CleanupStack::Pop( 2 );            // >> noteField, enumField      
    
    aPrInfo.SetPersonPresenceL( persInfo );
    CleanupStack::Pop();   // >> persInfo
    DP_SDA("CPresencePluginData::NotifyTerminatedToXIMPL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::NotifyToPrInfoL
// ---------------------------------------------------------------------------
//
void CPresencePluginData::NotifyToPrInfoL(
    MXIMPObjectFactory& /*aFactory*/,
    MPresenceObjectFactory& aPresenceFactory,
    MSimpleDocument& aDocument,
    MPresenceInfo& aPrInfo )
    {
    DP_SDA("CPresencePluginData::NotifyToPrInfoL");
    MPersonPresenceInfo* persInfo =
        aPresenceFactory.NewPersonPresenceInfoLC();
    DP_SDA("CPresencePluginData::NotifyToPrInfoL persInfo");

    if( !aDocument.EntityURI() )
    	{
    	DP_SDA("CPresencePluginData::NotifyToPrInfoL entity URL not valid");
    	User::Leave( KErrCancel );
    	}
 
    MPresenceInfoFieldCollection& coll = persInfo->Fields();
    
    DP_SDA("CPresencePluginData::NotifyToPrInfoL collection");
    // Search own person info. Notice: Extend supported attributes later
    TBool basicElementFound = EFalse;
    TBool activitiesElementFound = EFalse;
    TBool noteElemFound = EFalse;
        
    DP_SDA("CPresencePluginData::NotifyToPrInfoL RPointerArray");
    RPointerArray<MSimpleElement> elems;
    DP_SDA("CPresencePluginData::NotifyToPrInfoL Push elems");
    CleanupClosePushL( elems );
    DP_SDA("CPresencePluginData::NotifyToPrInfoL aDocument");
    TInt err = aDocument.SimpleElementsL( elems );
    
    DP_SDA2("CPresencePluginData::NotifyToPrInfoL err %d", err);
    
    RPointerArray<MSimpleElement> basicElems;
	CleanupClosePushL( basicElems );
	MSimpleElement* basicSimpleElem(NULL);
	
	RPointerArray<MSimpleElement> activitiesElems;
	CleanupClosePushL( activitiesElems );
	MSimpleElement* activitiesSimpleElem(NULL);
	MSimpleElement* noteSimpleElem(NULL);
		
    if ( !err )
        {
        DP_SDA("CPresencePluginData::NotifyToPrInfoL, elements ok");       
        TInt count = elems.Count();
        DP_SDA2("CPresencePluginData::NotifyToPrInfoL, element count: %d", count );       
        
        using namespace NPresencePlugin::NPresence;

        MSimpleElement* elem = NULL;
        TPtrC8 p8;

        for ( TInt i = 0; i < count; i++ )
            {
            DP_SDA2("CPresencePluginData::NotifyToPrInfoL, handling elem[%d]", i );
            
            elem = elems[i];
            p8.Set( elem->LocalName());
            if (!( p8.CompareF( KPresencePerson8 )) ||
                !( p8.CompareF( KPresenceTuple8 )) )
                {
                // person element found
                RPointerArray<MSimpleElement> elems2;
                CleanupClosePushL( elems2 );
                elem->SimpleElementsL( elems2 );
                TInt count2 = elems2.Count();
                DP_SDA2("CPresencePluginData::NotifyToPrInfoL count2 %d", count2);
                for ( TInt j = 0; j < count2; j++ )
                    {
                    DP_SDA("CPresencePluginData::NotifyToPrInfoL 4");
                    //
                    MSimpleElement* elem2 = elems2[j];
                    
                    //FIND BASIC/ACTIVITIES ELEMENT
                    if ( !elem2->LocalName().CompareF( KPresenceStatus8 ) &&
                        !basicElementFound )
             	        {
             	        DP_SDA("NotifyToPrInfoL PresenceStatus");
             	        //Get elem2 childs
			            elem2->SimpleElementsL( basicElems );
			            TInt count3 = basicElems.Count();  //Child count
			            for ( TInt k = 0; k < count3; k++ )
			                {
			                MSimpleElement* tempElem = basicElems[k];
			                if ( !tempElem->LocalName().CompareF( 
			                		KPresenceBasic8 ))
			                    {
			                    DP_SDA("NotifyToPrInfoL basic elem true");
			                    basicSimpleElem = basicElems[k];
			                    basicElementFound = ETrue;
			                    }
			                else if( !tempElem->LocalName().CompareF( 
			                		KPresenceActivities8 ))
			                    {
			                    DP_SDA("NotifyToPrInfoL activities under basic");
			                    activitiesSimpleElem = basicElems[k];
			                    activitiesElementFound = ETrue;
			                    }
			                } 
             	        }
             	    
                    //FIND ACTIVITIES ELEMENT
             	    if ( basicElementFound && !activitiesElementFound &&
             	        !elem2->LocalName().CompareF( KPresenceActivities8 ) )
             	        {
             	        DP_SDA("NotifyToPrInfoL activities found outside basic");
			            elem2->SimpleElementsL( activitiesElems );
			            TInt count4 = activitiesElems.Count();
			            for ( TInt l = 0; l < count4; l++ )
			                {
			                activitiesSimpleElem = activitiesElems[l];
			                activitiesElementFound = ETrue;
			                DP_SDA("CPresencePluginData::NotifyToPrInfoL 6");
			                }
             	        }
                    }
                
                if ( basicElementFound && !noteSimpleElem )
                    {
                    noteSimpleElem = ResolveNoteElementL( elems2 );
                    }
                
                DP_SDA("NotifyToPrInfoL ALL DONE"); 
                CleanupStack::PopAndDestroy( &elems2 );
                
                if ( basicElementFound && activitiesElementFound && noteElemFound )
                    {
                    DP_SDA("NotifyToPrInfoL ALL DONE break out"); 
                    //Just handle first status information from document
                    // In future server should support <timestamp>
                    //Also client should be can handle timestaps if there are present
                    break;
                    }
                }
     	    // Check note field    
     	    else if ( basicElementFound && !( p8.CompareF( KPresenceNote8 )) &&
     	         !noteSimpleElem )
     	        {
     	        DP_SDA("NotifyToPrInfoL note field found outside tuple");
	            noteSimpleElem = elem;
     	        }             	    
            }                
        }
    
    if ( basicElementFound )
    	{
    	DP_SDA("NotifyToPrInfoL basic proceed to handling");
    	SingleUserStatusToXIMPL( 
            aPresenceFactory, basicSimpleElem, 
    		activitiesSimpleElem, noteSimpleElem, coll );  
    	}
    CleanupStack::PopAndDestroy( &activitiesElems );
    CleanupStack::PopAndDestroy( &basicElems );
    CleanupStack::PopAndDestroy( &elems );

    aPrInfo.SetPersonPresenceL( persInfo );
    CleanupStack::Pop();   // >> persInfo
    
    // TODO2: Notice: the following crashes in old PrFW version
    /*
    aPrInfo.AddDevicePresenceL ( NULL );
    aPrInfo.AddServicePresenceL( NULL );
    */
    DP_SDA("CPresencePluginData::NotifyToPrInfoL end");        
    }


// ---------------------------------------------------------------------------
// CPresencePluginData::CacheEntriesFromPrInfo
// ---------------------------------------------------------------------------
//
void CPresencePluginData::CacheEntriesFromPrInfo(
    MPresenceInfo& aPrInfo,
    MPresenceBuddyInfo2::TAvailabilityValues& aAvailability,
    TPtr& aExtendedAvailability,
    TPtr& aStatusMessage )
    {
    DP_SDA("CPresencePluginData::CacheEntriesFromPrInfoL");
    
    DP_SDA(" -> CacheEntriesFromPrInfoL, fetch status enum field");
    MPresenceInfoField* statusEnumInfoField = NULL;
    MPersonPresenceInfo* presInfo = aPrInfo.PersonPresence();
    if ( presInfo )
        {
        DP_SDA(" -> CacheEntriesFromPrInfoL, has person presence, get field");
        presInfo->Fields().LookupFieldByFieldType( 
            statusEnumInfoField, 
            NPresenceInfo::NFieldType::KAvailabilityEnum );
        DP_SDA(" -> CacheEntriesFromPrInfoL, fetch status enum field done, check item");
        if ( statusEnumInfoField )
            {
            DP_SDA(" -> status enum field found");
            const MPresenceInfoFieldValueEnum* availability = 
            TXIMPGetInterface< const MPresenceInfoFieldValueEnum >::From( 
                statusEnumInfoField->FieldValue(), MXIMPBase::EPanicIfUnknown );
             switch( availability->Value() )
                {
                case NPresenceInfo::EAvailable:
                    {
                    DP_SDA(" -> status enum field => available");
                    aAvailability = MPresenceBuddyInfo2::EAvailable;
                    aExtendedAvailability.Copy( KDefaultAvailableStatus() );
                    }
                    break;
                    
                case NPresenceInfo::EBusy:
                    {
                    DP_SDA(" -> status enum field => busy");
                    aAvailability = MPresenceBuddyInfo2::EBusy;
                    aExtendedAvailability.Copy( KDndState() );
                    }
                    break;
                    
                case NPresenceInfo::EOnPhone:
                    {
                    DP_SDA(" -> status enum field => on phone");
                    aAvailability = MPresenceBuddyInfo2::EBusy;
                    aExtendedAvailability.Copy( KOnPhoneState() );
                    }
                    break;
                    
                case NPresenceInfo::EAway:
                    {
                    DP_SDA(" -> status enum field => away");
                    aAvailability = MPresenceBuddyInfo2::EBusy;
                    aExtendedAvailability.Copy( KAwayState() );
                    }
                    break;
                
                case NPresenceInfo::EDoNotDisturb:
                    {
                    DP_SDA(" -> status enum field => dnd");
                    aAvailability = MPresenceBuddyInfo2::EBusy;
                    aExtendedAvailability.Copy( KDndState() );
                    }
                    break;
                
                case NPresenceInfo::EOffline:
                case NPresenceInfo::ENotAvailable:
                default:
                    {
                    DP_SDA(" -> status enum field => offline/not available/default");
                    aAvailability = MPresenceBuddyInfo2::ENotAvailable;
                    aExtendedAvailability.Copy( KInvisibleState() );
                    }
                    break;
                }        
            }
        else
            {
            DP_SDA(" -> status enum not found => set unknown");
            aAvailability = MPresenceBuddyInfo2::EUnknownAvailability;
            aExtendedAvailability.Copy( KNullDesC() );
            }
        DP_SDA(" -> fetch status message field");    
        MPresenceInfoField* statusMsgInfoField = NULL;
        presInfo->Fields().LookupFieldByFieldType( 
            statusMsgInfoField, 
            NPresenceInfo::NFieldType::KStatusMessage );
        DP_SDA(" -> CacheEntriesFromPrInfoL, fetch status message field done, check item");    
        if ( statusMsgInfoField )
            {
            DP_SDA(" -> status msg field found");
            const MPresenceInfoFieldValueText* statusMsg = 
            TXIMPGetInterface< const MPresenceInfoFieldValueText >::From( 
                statusMsgInfoField->FieldValue(), MXIMPBase::EPanicIfUnknown );
            if ( statusMsg && statusMsg->TextValue().Length() )
                {
                DP_SDA(" -> status msg field found, copy content");
                aStatusMessage.Copy( statusMsg->TextValue() );
                }
            }
        else
            {
            DP_SDA(" -> status msg field not found, set empty");
            aStatusMessage.Copy( KNullDesC() );
            }
        }
        
    DP_SDA("CPresencePluginData::CacheEntriesFromPrInfoL out");    
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::SingleUserStatusToXIMPL
// ---------------------------------------------------------------------------
//
void CPresencePluginData::SingleUserStatusToXIMPL(
    MPresenceObjectFactory& aPresenceFactory,
    MSimpleElement* aBasicElement,
    MSimpleElement* aActivitiesElement,
    MSimpleElement* aNoteElement,
    MPresenceInfoFieldCollection& aCollection )
    {
    DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL");

    using namespace NPresenceInfo::NFieldType;
    using namespace NPresencePlugin::NPresenceStates;
    
    //Get aBasicElem content 
    HBufC* nodeContent = aBasicElement->ContentUnicodeL();
    CleanupStack::PushL( nodeContent );
    
    TBuf<KBufSize100> basicContentBuf; 
    basicContentBuf.Copy( *nodeContent );
    DP_SDA2("SingleUserStatusToXIMPL basicContent %S", &basicContentBuf);
    
    TBuf<KBufSize100> activitiesContentBuf;
    
    if ( !aActivitiesElement )
        {
        activitiesContentBuf.Copy( KPresenceUnknow );
        DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL activities NULL");
        }
    else
        {
        activitiesContentBuf.Copy( aActivitiesElement->LocalName() );
        DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL else");
        }
        
    if ( nodeContent )
        {
        using namespace NPresenceInfo::NFieldType;
        MPresenceInfoField* field = aPresenceFactory.NewInfoFieldLC();
        field->SetFieldTypeL( KAvailabilityEnum );
        
        // use mapped enums to pass status    
        MPresenceInfoFieldValueEnum* enumField = 
            aPresenceFactory.NewEnumInfoFieldLC();

        // Busy case
        if ( !nodeContent->Des().CompareF( KPresenceOpen ) &&
            !activitiesContentBuf.CompareF ( KPresenceBusy ) )
            {
            DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL open/busy");
            enumField->SetValueL( NPresenceInfo::EBusy );
            }
        // On-The-Phone case
        else if ( !nodeContent->Des().CompareF( KPresenceOpen ) &&
            !activitiesContentBuf.CompareF ( KPresenceOnThePhone ) )
            {
            DP_SDA("CPresencePluginData::SingleUserStatusToXIMPL open/on-the-phone");
            enumField->SetValueL( NPresenceInfo::EOnPhone );
            }
        //Away case  
        else if ( !nodeContent->Des().CompareF( KPresenceOpen ) &&
            !activitiesContentBuf.CompareF ( KPresenceAway ) )
            {
            DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL open/away");
            enumField->SetValueL( NPresenceInfo::EAway );
            }
        //Dnd case  
        else if ( !nodeContent->Des().CompareF( KPresenceOpen ) &&
            !activitiesContentBuf.CompareF ( KPresenceDoNotDisturb ) )
            {
            DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL open/dnd");
            enumField->SetValueL( NPresenceInfo::EDoNotDisturb );
            }            
        // Unknown open
        else if ( !nodeContent->Des().CompareF( KPresenceOpen ) &&
            !activitiesContentBuf.CompareF ( KPresenceUnknow ) )
            {
            DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL open/unknown");
            enumField->SetValueL( NPresenceInfo::EAvailable );
            }
        // availale open
        else if ( !nodeContent->Des().CompareF( KPresenceOpen ) &&
            !activitiesContentBuf.CompareF ( KPresenceAvailable ) )
            {
            DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL open/available");
            enumField->SetValueL( NPresenceInfo::EAvailable );
            }            
        //Unknown closed
        else if ( !nodeContent->Des().CompareF( KPresenceClosed ) &&
            !activitiesContentBuf.CompareF ( KPresenceUnknow ) )
            {
            DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL closed/unknown");
            enumField->SetValueL( NPresenceInfo::EOffline );
            }
        //Else set status according to basic status
        else
            {
            if ( !nodeContent->Des().CompareF( KPresenceOpen ) )
                {
                DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL else open");
                enumField->SetValueL( NPresenceInfo::EAvailable );
                }
            else
                {
                DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL else closed");
                enumField->SetValueL( NPresenceInfo::ENotAvailable );
                }
            }
        
        field->SetFieldValue( enumField );
        aCollection.AddOrReplaceFieldL( field );
        
        CleanupStack::Pop( 2 );            // >> field, enumField
        
        DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL check for note");
        
        // Handle note field if found and buddy is "available".
        if ( aNoteElement && 
             NPresenceInfo::EOffline != enumField->Value() &&
             NPresenceInfo::ENotAvailable != enumField->Value() )
            {
            DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL note elems found");
            // consider mapping note text to enum value if value not mapped
            UserNoteToXIMPL( aPresenceFactory, aNoteElement, aCollection );
            }
        }
    CleanupStack::PopAndDestroy( nodeContent );
    DP_SDA(" CPresencePluginData::SingleUserStatusToXIMPL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::UserNoteToXIMPL
// ---------------------------------------------------------------------------
//
void CPresencePluginData::UserNoteToXIMPL(
    MPresenceObjectFactory& aPresenceFactory,
    MSimpleElement* aElement,
    MPresenceInfoFieldCollection& aCollection )
    {
    DP_SDA(" CPresencePluginData::UserNoteToXIMPL IN");
    HBufC* nodeContent = aElement->ContentUnicodeL();
    CleanupStack::PushL( nodeContent );
    if ( nodeContent )
        {
        DP_SDA(" CPresencePluginData::UserNoteToXIMPL - content found");
        // Save note, convert from unicode
        // notice: consider xml::lang-attribute
        // note <-> KStatusMessage
        // notice: no need to consider namespaces in XML?
        using namespace NPresenceInfo::NFieldType;
        
        DP_SDA(" CPresencePluginData::UserNoteToXIMPL - create fields");
        MPresenceInfoField* field = aPresenceFactory.NewInfoFieldLC();
        field->SetFieldTypeL( KStatusMessage );
        MPresenceInfoFieldValueText* text =
        	aPresenceFactory.NewTextInfoFieldLC();
        text->SetTextValueL( nodeContent->Des() );
        field->SetFieldValue( text );
        CleanupStack::Pop();            // >> text
        aCollection.AddOrReplaceFieldL( field );
        CleanupStack::Pop();            // >> field     
        DP_SDA("  -> fields added to collection");
        }
    CleanupStack::PopAndDestroy( nodeContent );
    DP_SDA(" CPresencePluginData::UserNoteToXIMPL OUT");
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::NotifyListToPrInfoL
// ---------------------------------------------------------------------------
//
void CPresencePluginData::NotifyListToPrInfoL(
    MXIMPObjectFactory& aFactory,
    MPresenceObjectFactory& aPresenceFactory,
    MSimplePresenceList& aList,
    RPointerArray<MPresenceInfo>& aEntities,
    RPointerArray<MXIMPIdentity>& aActives,
    RPointerArray<MXIMPIdentity>& aTerminated )
    {
    DP_SDA(" CPresencePluginData::NotifyListToPrInfoL");
    // Split array into individual prInfos

    aEntities.Reset();
    RPointerArray<MSimpleDocument> docs;
    CleanupClosePushL( docs );
    // get documents, ownership is not transferred.
    aList.GetDocuments( docs );

    TInt count = docs.Count();

    // active presentities
    for ( TInt i = 0; i < count; i++ )
        {
        MPresenceInfo* info =
            aPresenceFactory.NewPresenceInfoLC();//<< info
        aEntities.AppendL( info );
        // aEntities may contain entries even this method leaves
        CleanupStack::Pop();// >> info

        NotifyToPrInfoL( aFactory, aPresenceFactory, *docs[i], *info );
        // Add SIp identity to active users list
        MXIMPIdentity* active = aFactory.NewIdentityLC();// << active
        aActives.AppendL( active );
        CleanupStack::Pop();// >> active

        // Convert SIP entity URI from UTF to Unicode.
        const TDesC8* pUri8 = (docs[i])->EntityURI();
        HBufC16* uri16 = NULL;
        uri16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *pUri8 );
        CleanupStack::PushL( uri16 );  // << uri16
        active->SetIdentityL( uri16->Des() );
        CleanupStack::PopAndDestroy( uri16 );// >> uri16
        }

    using namespace NPresencePlugin::NPresence;

    // Search "terminated" presentities
    MSimpleMeta* meta = aList.MetaData();
    // ownership in not transferred
    if ( meta && !meta->LocalName().CompareF( KPresenceList8 ))
        {
        // list element found
        RPointerArray<MSimpleElement> elems2;
        CleanupClosePushL( elems2 );
        meta->SimpleElementsL( elems2);
        TInt count2 = elems2.Count();

        for ( TInt i = 0; i < count2; i++ )
            {
            MSimpleElement* elem2 = elems2[i];
            if ( !elem2->LocalName().CompareF( KPresenceResource8 ))
                {
                // resource element
                RPointerArray<MSimpleElement> elems3;
                CleanupClosePushL( elems3 );
                meta->SimpleElementsL( elems3 );
                TInt count3 = elems3.Count();

                for ( TInt j=0; j < count3; j++ )
                    {
                    MSimpleElement* elem3 = elems3[i];

                    if ( !elem3->LocalName().CompareF( KPresenceInstance8 ))
                        {
                        // instance element
                        const TDesC8* stateVal =
                            elem3->AttrValue( KPresenceState8 );
                        if ( stateVal && stateVal->
                            CompareF( KPresenceTerminated8 ))
                            {
                            // get next entity if this is not state="terminated"
                            break;
                            }
                        // Save resource element URI into list of
                        // "terminated" users.
                        const TDesC8* uri8 = elem2->AttrValue( KPresenceUri8 );
                        MXIMPIdentity* terminated =
                            aFactory.NewIdentityLC();// << terminated
                        aTerminated.AppendL( terminated );
                        CleanupStack::Pop();                   // >> terminated

                        // Convert SIP entity URI from UTF to Unicode.
                        HBufC16* uri16 = NULL;
                        uri16 =
                            CnvUtfConverter::ConvertToUnicodeFromUtf8L( *uri8 );
                        CleanupStack::PushL( uri16 );  // << uri16
                        terminated->SetIdentityL( uri16->Des() );
                        CleanupStack::PopAndDestroy( uri16 );  // >> uri16
                        }
                    }
                CleanupStack::PopAndDestroy( &elems3 );
                } // resource element
            }// for (i); list element subelement
        CleanupStack::PopAndDestroy( &elems2 );
        }

    CleanupStack::PopAndDestroy( &docs );
    DP_SDA(" CPresencePluginData::NotifyListToPrInfoL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::AddPrPersToSimpleDocumentL
// ---------------------------------------------------------------------------
//
void CPresencePluginData::AddPrPersToSimpleDocumentL(
    const MPersonPresenceInfo* aInfo,
    MSimpleDocument& aDocument,
    const TDesC8& aSipId, TInt aTupleId )
    {
    DP_SDA("CPresencePluginData::AddPrPersToSimpleDocumentL start");
    iNumBuf = aTupleId;
    // notice: later: generate random id.

    using namespace NPresencePlugin::NPresence;
    using namespace NPresenceInfo::NFieldType;

    const MPresenceInfoFieldCollection& coll = aInfo->Fields();

    const MPresenceInfoField* statusEmumField = NULL;
    const MPresenceInfoField* messageField = NULL;

    TInt myCount = coll.FieldCount();
    for ( TInt i = 0; i < myCount; i++ )
        {
        DP_SDA("CPresencePluginData::AddPrPersToSimpleDocumentL 1");
        
        const MPresenceInfoField& field =  coll.FieldAt( i );
        const TDesC8& fieldType = field.FieldType();
        TBuf<KBufSize100> printBuf;
        printBuf.Copy( fieldType );
   
        if ( !fieldType.Compare( KAvatar ) )
           {
           // Do nothing in sawfis, maybe later?
           }
        else if ( !fieldType.Compare( KAvailabilityEnum ) )
            {
            DP_SDA("AddPrPersToSimpleDocumentL Avaibility");
            statusEmumField = &field;
            DP_SDA("AddPrPersToSimpleDocumentL Avaibility done");
            }          
        else if ( !fieldType.Compare( KStatusMessage ) )
            {
            DP_SDA("AddPrPersToSimpleDocumentL statusmessage");
            messageField = &field;
            }
        }
    if ( statusEmumField )
        {
        DoCreateDocumentL( aDocument, aSipId, 
            statusEmumField, messageField );            
        }
    
        
    DP_SDA("CPresencePluginData::AddPrPersToSimpleDocumentL end");
    }
    
    
// ---------------------------------------------------------------------------
// CPresencePluginData::CreatePresenceUri8LC
// ---------------------------------------------------------------------------
//
HBufC8* CPresencePluginData::CreatePresenceUri8LC( 
    const TDesC8& aPresentityUri )
    {
    DP_SDA("CPresencePluginData::CreatePresenceUri8LC ");
    const TInt KMyLenSipPrefix = 4;
    const TInt KMyLenSipsPrefix = 5;
    _LIT8( KMySip8, "sip:" );
    _LIT8( KMySips8, "sips:" ); 
    TInt uriLen = aPresentityUri.Length();
    DP_SDA2( "CPresencePluginData::CreatePresenceUri8LC - param length: %d ", uriLen );
    
    //For debugging purposes
    //TBuf<256> tmpPresentityUri; 
    //tmpPresentityUri.Copy( aPresentityUri );
    //DP_SDA2( "CPresencePluginData::CreatePresenceUri8LC - URI: %S ", &tmpPresentityUri );
    
    HBufC8* buf = HBufC8::NewLC( uriLen );
    TPtr8 pBuf( buf->Des() );
    
    //SIP uri check
    if ( !aPresentityUri.Left( KMyLenSipPrefix ).CompareF( KMySip8 ) )
        {
        DP_SDA("CPresencePluginData::CreatePresenceUri8LC (sip:) prefix in SIP URI ");
        pBuf.Append( aPresentityUri );
        }
    else if ( !aPresentityUri.Left( KMyLenSipsPrefix ).CompareF( KMySips8 ) )
        {
        DP_SDA( "CPresencePluginData::CreatePresenceUri8LC (sips:) prefix in SIP URI " );
        pBuf.Append( aPresentityUri );
        }
    else // No prefix, strange ...
        {
        DP_SDA( "CPresencePluginData::CreatePresenceUri8LC NO prefix in SIP URI, adding... " );
        // Maybe should check which prefix to use,
        // but assume that no secure is used if prefix missing
        TInt len = aPresentityUri.Length() + KMyLenSipPrefix;
        buf = buf->ReAllocL( len );
        //update pointer after realloc
        CleanupStack::Pop( 1 );
        CleanupStack::PushL( buf );
        // Since realloc may have changed the location in memory
        // we must also reset ptr
        pBuf.Set( buf->Des() );
        pBuf.Append( KMySip8 );
        pBuf.Append( aPresentityUri );
        }   
    return buf; 
    } 
    
// ---------------------------------------------------------------------------
// CPresencePluginData::RemovePrefixLC
// ---------------------------------------------------------------------------
//
HBufC16* CPresencePluginData::RemovePrefixLC( 
    const TDesC& aPresentityUri )
    {
    DP_SDA2("CPresencePluginData::RemovePrefixLC, uri: %S", &aPresentityUri );

    HBufC* withouPrefix = HBufC::NewLC( KBufSize255 );
    TPtr withouPrefixPtr( withouPrefix->Des() );
    
    TInt prefixLocation = aPresentityUri.Locate( ':' );
    DP_SDA2("CPresencePluginData::RemovePrefixLC, prefix pos: %d", prefixLocation );
    if ( KErrNotFound != prefixLocation )
        {
        DP_SDA("CPresencePluginData::RemovePrefixLC - has prefix");        
        withouPrefixPtr.Copy( aPresentityUri.Mid( prefixLocation+1 ) );
        DP_SDA("CPresencePluginData::RemovePrefixLC - copy ok");        
        }
    else
        {
        DP_SDA("CPresencePluginData::RemovePrefixLC - has no prefix");        
        withouPrefixPtr.Copy( aPresentityUri );
        }    
        
    //DP_SDA2("CPresencePluginData::RemovePrefixLC - returns %S", *withouPrefix );            
    return withouPrefix;
    }     
       
// ---------------------------------------------------------------------------
// CPresencePluginData::DoCreateDocumentL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginData::DoCreateDocumentL( 
    MSimpleDocument& aDocument,
    const TDesC8& aSipId,
    const MPresenceInfoField* aStatusField,
    const MPresenceInfoField* aMessageField )
    {    
    DP_SDA("CPresencePluginData::DoCreateDocumentL ");
    const TInt KNumBuf = 20;
    const TInt KTupleBuf = 10;
    
    using namespace NPresencePlugin::NPresence;
    using namespace NPresencePlugin::NPresenceStates;
    using namespace NPresenceInfo::NFieldType;

    TBuf<KNumBuf> numBuf;
    TBuf<KTupleBuf> tupleBuf;

    tupleBuf.Copy( KIndent );
    numBuf.Num( iNumBuf );
    tupleBuf.Append( numBuf );
    DP_SDA2("CPresencePluginData::DoCreateDocumentL id %d", iNumBuf);
	DP_SDA("CPresencePluginData::DoCreateDocumentL 2");
    
	//Add tuple id to document
    MSimpleElement* tuple = aDocument.AddSimpleElementL( KPresenceTuple8 );
    CleanupClosePushL( *tuple );
    tuple->AddAttrL( KPresenceId8, tupleBuf);
    
    //Presence status field
    MSimpleElement* status = tuple->AddSimpleElementL( KPresenceStatus8 );
    CleanupClosePushL( *status );
    
    //basic field
    MSimpleElement* basic = status->AddSimpleElementL( KPresenceBasic8 );
    CleanupClosePushL( *basic );
    
    basic->SetContentUnicodeL( DoCheckBasicStatusValueL( *aStatusField ) );
    
    DP_SDA("CPresencePluginData::DoCreateDocumentL Basic ok, check msg"); 
    
    if ( aMessageField )
        {
        DP_SDA("CPresencePluginData::DoCreateDocumentL message field"); 
        //Creating note 
        MSimpleElement* note =
            aDocument.AddSimpleElementL( KPresenceNote8 );
        CleanupClosePushL( *note );        
        
        // Get status msg field from info field
        const MPresenceInfoFieldValueText* textField = 
            TXIMPGetInterface< 
                const MPresenceInfoFieldValueText >::From(
                aMessageField->FieldValue(), 
                    MXIMPBase::EPanicIfUnknown );
        note->SetContentUnicodeL( textField->TextValue() );
        CleanupStack::PopAndDestroy( note );
        }

    /* Basic model example
    <tuple id="c8dqui">
          <status>
            <basic>open</basic>
          </status>
             <ts:basic>closed</ts:basic>
          </ts:timed-status>
          <contact>sip:someone@example.com</contact>
        </tuple>
    <note>sleeping</note>    
    </presence>
    */
    
    DP_SDA("CPresencePluginData::DoCreateDocumentL Rich presence person");
    //Creatin person presence field
    MSimpleElement* pers =
    	aDocument.AddSimpleElementL( KSimpleNsPDM, KPresencePerson8 );
    CleanupClosePushL( *pers );
    TBuf<KBufSize10> buf;
    TBuf<KBufSize20> personId;
    personId.Copy( KPersonId );
    buf.Num( iNumBuf );
    personId.Append( buf );
    pers->AddAttrL( KPresenceId8, personId );
    DP_SDA("CPresencePluginData::DoCreateDocumentL Rich presence person ok");
    
    HBufC8* urlBuf = CreatePresenceUri8LC( aSipId );
    DP_SDA("CPresencePluginData::DoCreateDocumentL Push urlBuf"); 
      
    aDocument.SetEntityURIL( urlBuf->Des() );
    DP_SDA("CPresencePluginData::DoCreateDocumentL 2");

    DP_SDA("CPresencePluginData::DoCreateDocumentLC Rich presence activities");
    MSimpleElement* activ = pers->AddSimpleElementL( 
        KSimpleNsRPID, KPresenceActivities8 );
    CleanupClosePushL( *activ );
    
    //unknown used if person presence is just open or closed
    MSimpleElement* unknown = activ->AddSimpleElementL( 
        KSimpleNsRPID, DoCheckActivitiesValueL( *aStatusField ) );
    DP_SDA("CPresencePluginData::DoCreateDocumentL, unknown/status element created");  
    CleanupClosePushL( *unknown );
   
    DP_SDA("CPresencePluginData::DoCreateDocumentL Destroy unknown");  
    CleanupStack::PopAndDestroy( unknown );
    DP_SDA("CPresencePluginData::DoCreateDocumentL Destroy activ");  
    CleanupStack::PopAndDestroy( activ ); 
    
    DP_SDA("CPresencePluginData::DoCreateDocumentL Destroy urlBuf");
    CleanupStack::PopAndDestroy( urlBuf );

    DP_SDA("CPresencePluginData::DoCreateDocumentL Destroy pers");  
    CleanupStack::PopAndDestroy( pers );    
    DP_SDA("CPresencePluginData::DoCreateDocumentL Rich presence activities ok");

    DP_SDA("CPresencePluginData::DoCreateDocumentL Destroy basic");
    CleanupStack::PopAndDestroy( basic );
    
    DP_SDA("CPresencePluginData::DoCreateDocumentL Destroy status");
    CleanupStack::PopAndDestroy( status );
    
    DP_SDA("CPresencePluginData::DoCreateDocumentL Destroy tuple");
    CleanupStack::PopAndDestroy( tuple );   
    DP_SDA("CPresencePluginData::DoCreateDocumentL end");    
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::DoCheckBasicStatusValueL()
// ---------------------------------------------------------------------------
//
TPtrC CPresencePluginData::DoCheckBasicStatusValueL(
	const MPresenceInfoField& aField )
	{
    using namespace NPresencePlugin::NPresence;
    using namespace NPresencePlugin::NPresenceStates;
    using namespace NPresenceInfo::NFieldType;
    
	TPtrC basicStatus( KPresenceOpen ); //default status open
	//First check what is person status value
	const MXIMPBase& storage = aField.FieldValue();
    const MPresenceInfoFieldValueEnum* enumObject =
          TXIMPGetInterface< const MPresenceInfoFieldValueEnum >::
          From( storage,MXIMPBase::EReturnNullIfUnknown );
    
    __ASSERT_ALWAYS( NULL != enumObject, User::Leave( KErrArgument ) );

    DP_SDA2(" DoCheckBasicStatusValueL - enumValue: %d", enumObject->Value() );
    switch ( enumObject->Value() )
        {
        case NPresenceInfo::EAvailable:
            {
			DP_SDA("DoCheckBasicStatusValueL stopPublish false status open");
			iConnObs.SetStopPublishState( EFalse );
			basicStatus.Set( KPresenceOpen );
            }
            break;
            
        case NPresenceInfo::EOffline:
        case NPresenceInfo::EHidden:
        case NPresenceInfo::ENotAvailable:
            {
			//If state is closed we can stop publish
			//if client is going to offline
			DP_SDA("DoCheckBasicStatusValueL stopPublish true status Closed");
			iConnObs.SetStopPublishState( ETrue );
			basicStatus.Set( KPresenceClosed );                
            }
            break;                
            
        default:
            {
			DP_SDA("DoCheckBasicStatusValueL stopPublish false else open");
			iConnObs.SetStopPublishState( EFalse );
			basicStatus.Set( KPresenceOpen );                
            }
            break;    
        }
    return basicStatus;
	}

// ---------------------------------------------------------------------------
// CPresencePluginData::DoCheckActivitiesValueL()
// ---------------------------------------------------------------------------
//
TPtrC8 CPresencePluginData::DoCheckActivitiesValueL(
	const MPresenceInfoField& aField )
	{
	using namespace NPresencePlugin::NPresence;
    using namespace NPresencePlugin::NPresenceStates;
    using namespace NPresenceInfo::NFieldType;
    
	TPtrC8 activities( KPresenceUnknow8 ); //default activities unknown
	
	const MXIMPBase& storage = aField.FieldValue();
    const MPresenceInfoFieldValueEnum* enumObject =
          TXIMPGetInterface< const MPresenceInfoFieldValueEnum >::
          From( storage,MXIMPBase::EReturnNullIfUnknown );
    
    __ASSERT_ALWAYS( NULL != enumObject, User::Leave( KErrArgument ) );

    DP_SDA2(" DoCheckActivitiesValueL - enumValue: %d", enumObject->Value() );
    switch ( enumObject->Value() )
        {
        case NPresenceInfo::EAvailable:
            {
            DP_SDA("DoCheckActivitiesValueL - available, set publish");
    		iConnObs.SetStopPublishState( EFalse );
    		DP_SDA("DoCheckActivitiesValueL - available, set value");
    		activities.Set( KPresenceUnknow8 );
    		DP_SDA("DoCheckActivitiesValueL - available, ok");
            }
            break;
            
        case NPresenceInfo::EOffline:
        case NPresenceInfo::ENotAvailable:
            {
            DP_SDA("DoCheckActivitiesValueL - offline/not available");
    		iConnObs.SetStopPublishState( ETrue );
    		activities.Set( KPresenceUnknow8 );
            }
            break;                
            
        case NPresenceInfo::EBusy:
            {
            DP_SDA("DoCheckActivitiesValueL - busy");
    		iConnObs.SetStopPublishState( EFalse );
    		activities.Set( KPresenceBusy8 );
            }
            break;
            
        case NPresenceInfo::EExtAway:
        case NPresenceInfo::EAway:
            {
            DP_SDA("DoCheckActivitiesValueL - away");
    		iConnObs.SetStopPublishState( EFalse );
    		activities.Set( KPresenceAway8 );
            }
            break;
            
        case NPresenceInfo::EOnPhone:
            {
            DP_SDA("DoCheckActivitiesValueL - on phone");
    		iConnObs.SetStopPublishState( EFalse );
    		activities.Set( KPresenceOnThePhone8 );
            }
            break;

        
        case NPresenceInfo::EDoNotDisturb:
            {
            DP_SDA("DoCheckActivitiesValueL - dnd");
    		iConnObs.SetStopPublishState( EFalse );
    		activities.Set( KPresenceDoNotDisturb8 );
            }
            break;
            
        default:
            {
			DP_SDA("DoCheckActivitiesValueL default => unknown");
    		iConnObs.SetStopPublishState( EFalse );
    		activities.Set( KPresenceUnknow8  );
            }
            break;    
        }
    DP_SDA("DoCheckActivitiesValueL - done and return");        
    return activities;
	}

// ---------------------------------------------------------------------------
// CPresencePluginData::DoCheckNoteValueL()
// ---------------------------------------------------------------------------
//
TPtrC CPresencePluginData::DoCheckNoteValueL(
    const MPresenceInfoField& aField )
    {
    using namespace NPresencePlugin::NPresence;
    using namespace NPresencePlugin::NPresenceStates;
    using namespace NPresenceInfo::NFieldType;
    
    TPtrC note( KPresenceUnknow ); //default activities unknown
    //First check what is person status value
    const MXIMPBase& storage = aField.FieldValue();
    const MPresenceInfoFieldValueText* text =
          TXIMPGetInterface< const MPresenceInfoFieldValueText >::
          From( storage,MXIMPBase::EReturnNullIfUnknown );
    
    __ASSERT_ALWAYS( NULL != text, User::Leave( KErrArgument ) );

    TBuf<KBufSize200> buffer;
    buffer.Copy( text->TextValue() );

    //Check if status is closed
    if( !buffer.CompareF( KPresenceClosed ) )
        {
        //If state is closed we can stop publish 
        //if client is going to offline
        DP_SDA("DoCheckNoteValueL stopPublish true closed");
        iConnObs.SetStopPublishState( ETrue );
        note.Set( KPresenceOffline );       
        }
    else if( !buffer.CompareF( KPresenceOpen ) )
        {
        DP_SDA("DoCheckNoteValueL stopPublish false open");
        iConnObs.SetStopPublishState( EFalse );
        note.Set( KPresenceAvailable );
        }
    else if( !buffer.CompareF( KPresenceBusy ) )
        {
        DP_SDA("DoCheckNoteValueL activities BUSY");
        iConnObs.SetStopPublishState( EFalse );
        note.Set( KPresenceBusy );
        }
    else if( !buffer.CompareF( KPresenceOnThePhone ) )
        {
        DP_SDA("DoCheckNoteValueL activities OnThePhone");
        iConnObs.SetStopPublishState( EFalse );
        note.Set( KPresenceOnThePhone );
        }
    else if( !buffer.CompareF( KPresenceAway ) )
        {
        DP_SDA("DoCheckNoteValueL activities OnThePhone");
        iConnObs.SetStopPublishState( EFalse );
        note.Set( KPresenceAway );
        }
    else
        {
        DP_SDA("DoCheckNoteValueL stopPublish false");
        iConnObs.SetStopPublishState( EFalse );
        note.Set( KPresenceUnknow  );
        }
    return note;
    }


// ---------------------------------------------------------------------------
// CPresencePluginData::GenerateTupleId()
// ---------------------------------------------------------------------------
//
TInt CPresencePluginData::GenerateTupleId()
    {
    // Generate unique session tuple id
    DP_SDA("CPresencePluginData::GenerateTupleId start");
    const TInt KMaxRand = KMaxNumber;
    TInt64 seed;
    TTime time;
    time.HomeTime();
    seed = time.Int64();
    TInt random = Math::Rand( seed ) % KMaxRand;
    DP_SDA("CPresencePluginData::GenerateTupleId 1");
    return random; 
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::ServiceId()
// ---------------------------------------------------------------------------
//
TInt CPresencePluginData::ServiceId()
    {
    return iServiceId;
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::ServiceNameLC()
// ---------------------------------------------------------------------------
//
HBufC* CPresencePluginData::ServiceNameLC( TInt aServiceId ) const
    {
    CSPSettings* spSettings = CSPSettings::NewLC();
    CSPEntry* entry = CSPEntry::NewLC();
    HBufC* serviceName = NULL;
    
    User::LeaveIfError( spSettings->FindEntryL( aServiceId, *entry ) );
    
    serviceName = entry->GetServiceName().AllocL();
    
    CleanupStack::PopAndDestroy( entry );
    CleanupStack::PopAndDestroy( spSettings );
    CleanupStack::PushL( serviceName );
    return serviceName;
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::ResolveCacheXspIdentifierL()
// ---------------------------------------------------------------------------
//
HBufC* CPresencePluginData::ResolveCacheXspIdentifierL(
    const TDesC& aIdentity ) const
    {
    DP_SDA( "CPresencePluginData::ResolveCacheXspIdentifierL" ); 
    
    CSPSettings* spSettings = CSPSettings::NewLC();
    CSPEntry* entry = CSPEntry::NewLC();
    
    User::LeaveIfError( spSettings->FindEntryL( iServiceId, *entry ) );

    TInt cacheUriLength = ( entry->GetServiceName().Length() + 
            aIdentity.Length() + 1 );
    
    HBufC* cacheUri = HBufC::NewL( cacheUriLength );
    TPtr cacheUriPtr( cacheUri->Des() );
    
    cacheUriPtr.Append( entry->GetServiceName() );
    cacheUriPtr.Append( ':' );
    cacheUriPtr.Append( aIdentity );
    
    CleanupStack::PopAndDestroy( entry );
    CleanupStack::PopAndDestroy( spSettings );
    
    DP_SDA2( "CPresencePluginData::ResolveCacheXspIdentifierL returns: %S", cacheUri );
    return cacheUri;
    }

// ---------------------------------------------------------------------------
// CPresencePluginData::WriteBlockItemsToCacheL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginData::WriteStatusToCacheL(
    const TDesC& aPresentityId, 
    MPresenceBuddyInfo2::TAvailabilityValues aAvailability,
    const TDesC& aAvailabilityText,
    const TDesC& aStatusMessage )
    {
    DP_SDA( "CPresencePluginData::WriteStatusToCacheL start" );
    DP_SDA2( "CPresencePluginData::WriteStatusToCacheL, aStatus: %d", 
        aAvailability );
    DP_SDA2( "CPresencePluginData::WriteStatusToCacheL, availabilityText: %S", 
        &aAvailabilityText );
    DP_SDA2( "CPresencePluginData::WriteStatusToCacheL, statusMessage: %S", 
        &aStatusMessage );

    TBool updateCache( ETrue );
    
    // Decode encoded username (spaces to %20).
    HBufC* decodedUsername = EscapeUtils::EscapeDecodeL( aPresentityId );
    CleanupStack::PushL( decodedUsername );
    
    // convert to 8 bit version
    HBufC8* tmp = HBufC8::NewLC( decodedUsername->Length() );
    tmp->Des().Copy( *decodedUsername );
    
    // convert to unicode
    HBufC* userName16 =
            EscapeUtils::ConvertToUnicodeFromUtf8L( tmp->Des() );
    
    HBufC* cacheUri = ResolveCacheXspIdentifierL( userName16->Des() );
    CleanupStack::PopAndDestroy( tmp );
    CleanupStack::PopAndDestroy( decodedUsername );
    delete userName16;
    CleanupStack::PushL( cacheUri );
    
    DP_SDA(" -> WriteStatusToCacheL - read previous values from cache"); 
    
    // Read previous values from cache
    MPresenceBuddyInfo2* previousPresInfo = 
        iPresenceCacheReader->PresenceInfoLC( *cacheUri );
    
    if ( previousPresInfo )
        {
        DP_SDA(" -> WriteStatusToCacheL - get availability value"); 
        
        MPresenceBuddyInfo2::TAvailabilityValues availability = 
            previousPresInfo->Availability();
        
        DP_SDA(" -> WriteStatusToCacheL - get availability text"); 
        
        TPtrC availabilityText = previousPresInfo->AvailabilityText();
        
        DP_SDA(" -> WriteStatusToCacheL - get status message"); 
        
        TPtrC statusMessage = previousPresInfo->StatusMessage();
        CleanupStack::PopAndDestroy(); //previousPresInfo 
        
        DP_SDA2( "CPresencePluginData::WriteStatusToCacheL, OLD STATUS: %d", 
                availability );
        DP_SDA2( "CPresencePluginData::WriteStatusToCacheL, OLD AVAILABILITY TEXT: %S", 
            &availabilityText );
        DP_SDA2( "CPresencePluginData::WriteStatusToCacheL, OLD STATUS MESSAGE: %S", 
            &statusMessage );
        
        if ( ( aAvailability == availability ) && 
            ( aAvailabilityText.Compare( availabilityText ) == 0 ) &&
            ( aStatusMessage.Compare( statusMessage ) == 0 ))
            {
            DP_SDA(" -> WriteStatusToCacheL - no need to update cache");    
            updateCache = EFalse;
            } 
        }
        
    if ( updateCache )
        {
        MPresenceBuddyInfo2* newPresInfo = MPresenceBuddyInfo2::NewLC();
        newPresInfo->SetIdentityL( *cacheUri );
        
        DP_SDA(" -> WriteStatusToCacheL - update cache");
        
        TBuf<KBufSize20> buf;
        TBool handled = EFalse;
           
        buf.Copy( KBlockedExtensionValue );
          
        if( aAvailabilityText.Compare( buf ) == 0 )
            {
            DP_SDA( " -> WriteStatusToCacheL - set Blocked" );
            newPresInfo->SetAnyFieldL( KExtensionKey, KBlockedExtensionValue );
            handled = ETrue;
            }
           
        buf.Copy( KPendingRequestExtensionValue );
           
        if ( aAvailabilityText.Compare( buf ) == 0 )
            {
            DP_SDA( " -> WriteStatusToCacheL - set Pending request" );
            newPresInfo->SetAnyFieldL( KExtensionKey, KPendingRequestExtensionValue );
            handled = ETrue;
            }
           
        if ( !handled )
            {
            DP_SDA2( " -> WriteStatusToCacheL - set availability text: %S", &aAvailabilityText );
            newPresInfo->SetAvailabilityL( aAvailability, aAvailabilityText );
            }
           
        if ( aStatusMessage.Length() )
            {
            DP_SDA2( " -> WriteStatusToCacheL - set status message: %S", &aStatusMessage ); 
            newPresInfo->SetStatusMessageL( aStatusMessage );
            }

        DP_SDA( " -> WriteStatusToCacheL - write presence to cache" );    
        TInt cacheError = iPresenceCacheWriter->WritePresenceL( newPresInfo );
        DP_SDA2( "CPresencePluginEntityWatcher::WriteStatusToCacheL error: %d", 
            cacheError );
           
        DP_SDA( " -> destroy buddy info" );    
        CleanupStack::PopAndDestroy(); // newPresInfo            
        }
    
    DP_SDA(" -> destroy uri");    
    CleanupStack::PopAndDestroy( cacheUri );

    DP_SDA("CPresencePluginData::WriteStatusToCacheL end");
    }     
    
// ---------------------------------------------------------------------------
// CPresencePluginData::RemoveCacheL()
// ---------------------------------------------------------------------------
//      
void CPresencePluginData::RemoveCacheL()
    {
    DP_SDA("CPresencePluginData::RemoveCacheL");
    
    // Resolve service name (cache xsp identifier)
    CSPSettings* spSettings = CSPSettings::NewL();
    CleanupStack::PushL( spSettings );
    
    CSPEntry* entry = CSPEntry::NewLC();
    DP_SDA2(" -> RemoveCacheL look for service: %d", ServiceId() );
    spSettings->FindEntryL( ServiceId(), *entry );

    DP_SDA(" -> RemoveCacheL cache xsp identifier found");
    TInt cacheUriLength = entry->GetServiceName().Length();
    DP_SDA2(" -> cache uri length: %d", cacheUriLength );    
    
    HBufC* cacheUri = HBufC::NewLC( cacheUriLength );
    TPtr cacheUriPtr( cacheUri->Des() );
    
    DP_SDA(" -> RemoveCacheL - form cache entry");
    cacheUriPtr.Append( entry->GetServiceName() );
    
    DP_SDA(" -> RemoveCacheL - delete cache entries");
    TInt error = iPresenceCacheWriter->DeleteService( cacheUriPtr );
    DP_SDA2(" -> RemoveCacheL - delete error: %d", error );
    CleanupStack::PopAndDestroy( cacheUri );

    DP_SDA(" -> destroy sp entry");        
    CleanupStack::PopAndDestroy( entry );        
    DP_SDA(" -> destroy sp");        
    CleanupStack::PopAndDestroy( spSettings );
    DP_SDA("CPresencePluginData::RemoveCacheL out");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginData::DeletePresenceL()
// ---------------------------------------------------------------------------
//      
void CPresencePluginData::DeletePresenceL( const TDesC& aIdentity )
    {
    DP_SDA("CPresencePluginData::DeletePresenceL");
    
    // Resolve service name (cache xsp identifier)
    CSPSettings* spSettings = CSPSettings::NewL();
    CleanupStack::PushL( spSettings );
    CSPEntry* entry = CSPEntry::NewLC();
    DP_SDA2(" -> DeletePresenceL look for service: %d", ServiceId() );
    spSettings->FindEntryL( ServiceId(), *entry );

    DP_SDA(" -> DeletePresenceL cache xsp identifier found");
    TInt cacheUriLength = ( entry->GetServiceName().Length() + 
        aIdentity.Length() + 1 );
    DP_SDA2(" -> cache uri length: %d", cacheUriLength );    
    
    HBufC* cacheUri = HBufC::NewLC( cacheUriLength );
    TPtr cacheUriPtr( cacheUri->Des() );
    
    DP_SDA(" -> DeletePresenceL - form cache entry");
    cacheUriPtr.Append( entry->GetServiceName() );
    cacheUriPtr.Append( ':' );
    cacheUriPtr.Append( aIdentity );

    iPresenceCacheWriter->DeletePresenceL( cacheUriPtr );
    
    CleanupStack::PopAndDestroy( cacheUri ); 
    CleanupStack::PopAndDestroy( entry ); 
    DP_SDA(" -> destroy sp");        
    CleanupStack::PopAndDestroy( spSettings );
    DP_SDA("CPresencePluginData::DeletePresenceL out");
    }    

    
// ---------------------------------------------------------------------------
// CPresencePluginData::StorePresenceOwnPresenceL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginData::StorePresenceOwnPresenceL(
    const TUint aServiceId, 
    NPresenceInfo::TAvailabilityValues aAvailability,
    const TDesC& aStatusMessage )
    {  
    DP_SDA("CPresencePluginData::StorePresenceOwnPresenceL IN");
    DP_SDA2(" -> aServiceId: %d", aServiceId );
        
    DP_SDA2(" -> SAVE AVAILABILITY VALUE: %d", aAvailability );
    DP_SDA2(" -> SAVE STATUS MESSAGE: %S", &aStatusMessage );
        
    MVIMPSTSettingsStore* settings = CVIMPSTSettingsStore::NewLC();
    
    DP_SDA("CPresencePluginData::StorePresenceOwnPresenceL 1");
    
    // Store availability value to uiservicetabsettings
    TInt availabilityInt = ( ( TInt ) aAvailability );
    User::LeaveIfError( settings->SetL( 
        aServiceId, EServicePresenceAvailablilityValue, availabilityInt ) );
    
    DP_SDA("CPresencePluginData::StorePresenceOwnPresenceL 2");
    
    // Store custom status message to uiservicetabsettings
    User::LeaveIfError( settings->SetL( 
        aServiceId, EServiceCustomStatusMessage, aStatusMessage ) );
    
    CleanupStack::PopAndDestroy(); //settings
    
    DP_SDA("CPresencePluginData::StorePresenceOwnPresenceL OUT");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginData::ReadDocumentIdL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginData::ReadDocumentIdL(
    const TInt aServiceId, 
    TDes8& aDocumentId )
    {  
    DP_SDA("CPresencePluginData::ReadDocumentIdL IN");
        
    DP_SDA2(" -> aServiceId: %d", aServiceId );
    
    MVIMPSTSettingsStore* settings = CVIMPSTSettingsStore::NewLC();
    
    RBuf8 documentId;
    CleanupClosePushL( documentId );
    documentId.CreateL( KBufSize255 );
    
    TInt error = settings->GetL( 
        aServiceId, EServicePresenceSessionIdentifier, documentId );
    
    DP_SDA2(" -> ERROR: %d", error );
    DP_SDA2(" -> DOCUMENT ID LENGTH: %d", documentId.Length() );
        
    if ( !error && documentId.Length() )
        {
        aDocumentId.Copy( documentId );
        
        TBuf<KBufSize255> printDocumentId;
        printDocumentId.Copy( aDocumentId );
        DP_SDA("CPresencePluginData::ReadDocumentIdL - 1");
        DP_SDA2(" -> READ DOCUMENT ID: %S", &printDocumentId );
        }
    else if ( KErrNotFound == error || !documentId.Length() )
        {
        DP_SDA("CPresencePluginData::ReadDocumentIdL - 2");
        // If document id lenght is zero (KNullDesC8) or error
        // is KErrNotFound leave with KErrNotFound
        User::Leave( KErrNotFound );
        }
    else
        {
        User::Leave( error );
        }
    
    CleanupStack::PopAndDestroy( &documentId );
    CleanupStack::PopAndDestroy(); //settings
    
    DP_SDA("CPresencePluginData::ReadDocumentIdL OUT");
    }   
    
// ---------------------------------------------------------------------------
// CPresencePluginData::DeletePresenceVariablesL()
// ---------------------------------------------------------------------------
//    
void CPresencePluginData::DeletePresenceVariablesL( const TInt aServiceId )
    {  
    DP_SDA("CPresencePluginData::DeletePresenceVariablesL IN");
    DP_SDA2(" -> aServiceId: %d", aServiceId );
        
    MVIMPSTSettingsStore* settings = CVIMPSTSettingsStore::NewLC();
    
    // Reset availability value in uiservicetabsettings
    User::LeaveIfError( settings->SetL( 
        aServiceId, EServicePresenceAvailablilityValue, KErrNotFound ) );
       
    // Reset status message in uiservicetabsettings
    User::LeaveIfError( settings->SetL( 
        aServiceId, EServiceCustomStatusMessage, KNullDesC ) );
    
    // Reset document id value in uiservicetabsettings
    User::LeaveIfError( settings->SetL( 
        aServiceId, EServicePresenceSessionIdentifier, KNullDesC8 ) );
    
    CleanupStack::PopAndDestroy(); // settings
    
    DP_SDA("CPresencePluginData::DeletePresenceVariablesL OUT");
    }   


// ---------------------------------------------------------------------------
// CPresencePluginData::ResolveNoteElementL
// Returns <note> element corresponding current locale or first <note> 
// element if better match is not found.
// ---------------------------------------------------------------------------
//
MSimpleElement* CPresencePluginData::ResolveNoteElementL( 
        const RPointerArray<MSimpleElement>& aElements ) const
    {
    DP_SDA("CPresencePluginData::ResolveNoteElementL" )
    
    MSimpleElement* bestMatch = NULL;
    MSimpleElement* secondBestMatch = NULL;
    
    for ( TInt i = aElements.Count() - 1; i >= 0 && NULL == bestMatch; i-- )
        {
        MSimpleElement* element = aElements[i];
        
        using namespace NPresencePlugin::NPresence;
        if ( 0 == element->LocalName().CompareF( KPresenceNote8 ) )
            {
            if ( IsElementLanguageValidForCurrentLocaleL( *element ) )
                {
                DP_SDA("CPresencePluginData::ResolveNoteElementL, BEST MATCH.")
                bestMatch = element;
                }
            else
                {
                secondBestMatch = element;
                }
            }
        }
    
    return ( bestMatch ? bestMatch : secondBestMatch );
    }


// ---------------------------------------------------------------------------
// CPresencePluginData::IsElementLanguageValidForCurrentLocaleL
// ---------------------------------------------------------------------------
//
TBool CPresencePluginData::IsElementLanguageValidForCurrentLocaleL( 
        MSimpleElement& aElement ) const
    {
    DP_SDA( "CPresencePluginData::IsElementLanguageValidForCurrentLocaleL" )
    
    TBool isLanguageResolved = EFalse;
    _LIT8( KLanguageAttribute, "xml:lang" );
    
    TLanguage language = User::Language();
    RPointerArray<MSimpleAttribute> attributes;
    CleanupClosePushL( attributes );
    aElement.SimpleAttributesL( attributes );
    for ( TInt i = attributes.Count() - 1; i >= 0 && !isLanguageResolved; i-- )
        {
        if ( 0 == KLanguageAttribute().CompareF( attributes[i]->Name() ) )
            {
            const TDesC8& attributeValue = attributes[i]->Value();
            for ( TInt index = 0; index < KLanguageCodeMappingsCount; index++ )
                {
                if ( language == KLanguageCodeMappings[index].SymbianLanguageCode() &&
                     0 == attributeValue.CompareF( 
                         KLanguageCodeMappings[index].IsoLanguageCode() ) )
                    {
                    isLanguageResolved = ETrue;
                    }
                }
            }
        }
    CleanupStack::Pop( &attributes );
    
    return isLanguageResolved;
    }

// End of file
