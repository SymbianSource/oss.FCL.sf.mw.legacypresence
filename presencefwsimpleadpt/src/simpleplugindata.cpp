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





// *** system include files go here:
#include <e32std.h>
#include <s32strm.h>
#include <utf.h>

#include <presenceobjectfactory.h>
#include <presenceinfo.h>
#include <personpresenceinfo.h>
#include <presenceinfofieldcollection.h>
#include <presenceinfofield.h>
#include <presenceinfofieldvaluetext.h>
#include <presenceinfofieldvalueenum.h>
#include <presenceinfofieldvaluebinary.h>
#include <presenceinfofieldvalueenum.h>
#include <presenceerrors.hrh>

#include <ximpobjectfactory.h>
#include <ximpidentity.h>
#include <ximpdatasubscriptionstate.h>
#include <ximpprotocolconnectionhost.h>
#include <ximpstatus.h>

#include <msimpledocument.h>
#include <msimpleelement.h>
#include <msimplepresencelist.h>
#include <msimplemeta.h>
#include <msimplecontent.h>
#include <simpleutils.h>
#include <simplefactory.h>

#include "simpleplugindata.h"
#include "simpleplugincommon.h"
#include "simpleplugindebugutils.h"


// ----------------------------------------------------------
// CSimplePluginData::CSimplePluginData
// ----------------------------------------------------------
//
CSimplePluginData::CSimplePluginData( )
    {
    }

// ---------------------------------------------------------------------------
// CSimplePluginData::~CSimplePluginData
// ---------------------------------------------------------------------------
//
CSimplePluginData::~CSimplePluginData()
    {

    }

// ---------------------------------------------------------------------------
// CSimplePluginData::NewL
// ---------------------------------------------------------------------------
//
CSimplePluginData* CSimplePluginData::NewL( )
    {
    CSimplePluginData* self = CSimplePluginData::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSimplePluginData::NewLC
// ---------------------------------------------------------------------------
//
CSimplePluginData* CSimplePluginData::NewLC( )
    {
    CSimplePluginData* self = new( ELeave ) CSimplePluginData;
    CleanupStack::PushL( self );
    self->ConstructL(  );
    return self;
    }

// ---------------------------------------------------------------------------
// CSimplePluginData::ConstructL
// ---------------------------------------------------------------------------
//
void CSimplePluginData::ConstructL( )
    {
    }

// ---------------------------------------------------------------------------
// CSimplePluginData::NotifyToPrInfoL
// ---------------------------------------------------------------------------
//
void CSimplePluginData::NotifyToPrInfoL(
        MPresenceObjectFactory& aPresenceFactory,
        MSimpleDocument& aDocument,
        MPresenceInfo& aPrInfo )
    {
    MPersonPresenceInfo* persInfo = aPresenceFactory.NewPersonPresenceInfoLC();
    MPresenceInfoFieldCollection& coll = persInfo->Fields();

    // Search own person info. Notice: Extend supported attributes later

    RPointerArray<MSimpleElement> elems;
    CleanupClosePushL( elems );
    TInt err = aDocument.SimpleElementsL( elems );

    if ( !err )
        {
        TInt myLatestPosition = SearchLatestTimestampL( elems );

        if ( myLatestPosition != KErrNotFound )
            {
            MSimpleElement* elem = elems[myLatestPosition];

            using namespace NSimplePlugin::NSimpleOma;

            // person element found
            RPointerArray<MSimpleElement> elems2;
            CleanupClosePushL( elems2 );
            elem->SimpleElementsL( elems2 );
            TInt count2 = elems2.Count();

            for ( TInt j = 0; j < count2; j++ )
                {
                MSimpleElement* elem2 = elems2[j];
                if ( !elem2->LocalName().CompareF( KSimpleOverridingwillingness8 ))
                    {
                    UserOverridingWillingnessToPrFwL( aPresenceFactory, elem2, coll );
                    }
                else if ( !elem2->LocalName().CompareF( KSimpleStatusicon8 ))
                    {
                    UserStatusIconToPrFwL( aPresenceFactory, elem2, coll, aDocument );
                    }
                else if ( !elem2->LocalName().CompareF( KSimpleNote8 ))
                    {
                    UserNoteToPrFwL( aPresenceFactory, elem2, coll );
                    }
                }

            CleanupStack::PopAndDestroy( &elems2 );
            }
        }

    CleanupStack::PopAndDestroy( &elems );

    aPrInfo.SetPersonPresenceL( persInfo );
    CleanupStack::Pop();   // >> persInfo
    }

// ---------------------------------------------------------------------------
// CSimplePluginData::UserOverridingWillingnessToPrFwL
// ---------------------------------------------------------------------------
//
void CSimplePluginData::UserOverridingWillingnessToPrFwL(
        MPresenceObjectFactory& aPresenceFactory,
        MSimpleElement* aElement,
        MPresenceInfoFieldCollection& aCollection )
    {

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginData: UserOverridingWillingnessToPrFwL") );
#endif
    using namespace NSimplePlugin::NSimpleOma;

    RPointerArray<MSimpleElement> elems;
    CleanupClosePushL( elems );
    User::LeaveIfError( aElement->SimpleElementsL( elems ));
    MSimpleElement* elem3 = elems[0]; // notice: we assume that there is no other sub-elements
    if ( !elem3->LocalName().CompareF( KSimpleBasic8 ))
        {
        // write the data into text field.
        HBufC* nodeContent = elem3->ContentUnicodeL();
        CleanupStack::PushL( nodeContent );
        if ( nodeContent )
            {
            // Save availability
            using namespace NPresenceInfo::NFieldType;
            MPresenceInfoField* field = aPresenceFactory.NewInfoFieldLC();
            field->SetFieldTypeL( KAvailability );
            MPresenceInfoFieldValueText* text = aPresenceFactory.NewTextInfoFieldLC();
            text->SetTextValueL( nodeContent->Des() );
            field->SetFieldValue( text );
            CleanupStack::Pop();            // >> text
            aCollection.AddOrReplaceFieldL( field );
            CleanupStack::Pop();            // >> field
            }
        CleanupStack::PopAndDestroy( nodeContent );
        }
    CleanupStack::PopAndDestroy( &elems );  // close the array, do not delete the content
    }

// ---------------------------------------------------------------------------
// CSimplePluginData::UserStatusIconToPrFwL
// ---------------------------------------------------------------------------
//
void CSimplePluginData::UserStatusIconToPrFwL(
        MPresenceObjectFactory& aPresenceFactory,
        MSimpleElement* aElement,
        MPresenceInfoFieldCollection& aCollection,
        MSimpleDocument& aDocument )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginData: UserStatusIconToPrFwL") );
#endif
    using namespace NSimpleDocument;

    using namespace NPresenceInfo::NFieldType;

    HBufC* nodeContent = aElement->ContentUnicodeL();
    CleanupStack::PushL( nodeContent );
    if ( nodeContent )
        {
        RPointerArray<MSimpleContent> contents;
        CleanupClosePushL( contents );

        // Search the corresponding direct content
        aDocument.GetDirectContentsL( contents );

        TInt contCount = contents.Count();
        for ( TInt i = 0; i<contCount; i++ )
            {
            TPtrC8 id = (contents[i])->ContentID();
            // nodecontent = "cid:simple.avatar.com"
            // content-id: simple.avatar.com
            HBufC8* cid8 = NULL;
            cid8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( nodeContent->Des() );
            CleanupStack::PushL( cid8 ); // << cid8

            TInt contentLen = cid8->Des().Length() - 4;  // "cid:"
            TInt headLen = id.Length();
            TPtrC8 pUnquoted( KNullDesC8 );
            if ( id.Locate('"') == 0 )
                {
                // remove "..." characters
                pUnquoted.Set( id.Mid( 1, headLen-2 ));
                }
            else
                {
                pUnquoted.Set( id );
                }
            TInt foundPos = cid8->Des().Find( pUnquoted );
            if ( contentLen == headLen && foundPos >= 0 )
                {
                MPresenceInfoField* field = aPresenceFactory.NewInfoFieldLC();
                field->SetFieldTypeL( KAvatar );
                MPresenceInfoFieldValueBinary* val = aPresenceFactory.NewBinaryInfoFieldLC();
                val->SetMimeTypeL( (contents[i])->ContentType() );
                // Body is not BASE64 encoded internally
                val->SetBinaryValueL( (contents[i])->Body() );
                field->SetFieldValue( val );
                CleanupStack::Pop();                // >> val
                aCollection.AddOrReplaceFieldL( field );
                CleanupStack::Pop();                // >> field
                }
            CleanupStack::PopAndDestroy( cid8 ); // >> cid8
            }
        CleanupStack::PopAndDestroy( &contents );
        }
    CleanupStack::PopAndDestroy( nodeContent );
    }

// ---------------------------------------------------------------------------
// CSimplePluginData::UserNoteToPrFwL
// ---------------------------------------------------------------------------
//
void CSimplePluginData::UserNoteToPrFwL(
        MPresenceObjectFactory& aPresenceFactory,
        MSimpleElement* aElement,
        MPresenceInfoFieldCollection& aCollection )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginData: UserNoteToPrFwL") );
#endif
    // using namespace NSimplePlugin::NSimpleOma;
    HBufC* nodeContent = aElement->ContentUnicodeL();
    CleanupStack::PushL( nodeContent );
    if ( nodeContent )
        {
        // Save note, convert from unicode
        // notice: consider xml::lang-attribute
        // note <-> KStatusMessage
        using namespace NPresenceInfo::NFieldType;
        MPresenceInfoField* field = aPresenceFactory.NewInfoFieldLC();
        field->SetFieldTypeL( KStatusMessage );
        MPresenceInfoFieldValueText* text = aPresenceFactory.NewTextInfoFieldLC();
        text->SetTextValueL( nodeContent->Des() );
        field->SetFieldValue( text );
        CleanupStack::Pop();            // >> text
        aCollection.AddOrReplaceFieldL( field );
        CleanupStack::Pop();            // >> field
        }
    CleanupStack::PopAndDestroy( nodeContent );
    }

// ---------------------------------------------------------------------------
// CSimplePluginData::NotifyListToPrInfoL
// ---------------------------------------------------------------------------
//
void CSimplePluginData::NotifyListToPrInfoL(
    MXIMPObjectFactory& aFactory,
    MPresenceObjectFactory& aPresenceFactory,
    MSimplePresenceList& aList,
    RPointerArray<MPresenceInfo>& aEntities,
    RPointerArray<MXIMPIdentity>& aActives,
    RPointerArray<MXIMPIdentity>& aTerminated,
    TBool& aFullList )
    {
    // Split array into individual prInfos

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginData: NotifyListToPrInfoL") );
#endif

    RPointerArray<MSimpleDocument> docs;
    CleanupClosePushL( docs );
    // get documents, ownership is not transferred.
    aList.GetDocuments( docs );

    CollectAllPresentitiesL( aEntities, aActives, docs, aFactory, aPresenceFactory );

    using namespace NSimplePlugin::NSimpleOma;

    MSimpleMeta* meta = aList.MetaData();

#ifdef _DEBUG
    // TODO: remove
    if ( meta == NULL )
    PluginLogger::Log(_L("PluginData: META Is NULL !!! *****"));
#endif



    // ownership in not transferred
    if ( meta && !meta->LocalName().CompareF( KSimpleList8 ))
        {
        // list element found, find out if fullstate
        const TDesC8* fullStateVal = meta->AttrValue( KSimpleFullState8 );
        if ( fullStateVal && !fullStateVal->CompareF( KSimpleTrue8 ))
            {
            aFullList = ETrue;
            }
        else
            {
            aFullList = EFalse;
            }

#ifdef _DEBUG
        PluginLogger::Log(_L("PluginData: fullState === %d"), aFullList );
#endif

        // Search "terminated" presentities
        RPointerArray<MSimpleElement> elems2;
        CleanupClosePushL( elems2 );
        meta->SimpleElementsL( elems2);
        CollectTerminatedPresentitiesL( aTerminated, elems2, aFactory );
        CleanupStack::PopAndDestroy( &elems2 );
        }

    CleanupStack::PopAndDestroy( &docs );
    }

        /*
        example:
   Content-Transfer-Encoding: binary
   Content-ID: <nXYxAE@pres.vancouver.example.com>
   Content-Type: application/rlmi+xml;charset="UTF-8"

   <?xml version="1.0" encoding="UTF-8"?>
   <list xmlns="urn:ietf:params:xml:ns:rlmi"
         uri="sip:adam-friends@pres.vancouver.example.com"
         version="1" fullState="true">
     <name xml:lang="en">Buddy List at COM</name>
     <name xml:lang="de">Liste der Freunde an COM</name>
     <resource uri="sip:bob@vancouver.example.com"">        <------- note
       <name>Bob Smith</name>
       <instance id="juwigmtboe" state="active"
                 cid="bUZBsM@pres.vancouver.example.com"/>
     </resource>
     <resource uri="sip:dave@vancouver.example.com">
       <name>Dave Jones</name>
       <instance id="hqzsuxtfyq" state="active"
                 cid="ZvSvkz@pres.vancouver.example.com"/>
     </resource>
     <resource uri="sip:ed@dallas.example.net">
       <name>Ed at NET</name>
     </resource>
     <resource uri="sip:adam-friends@stockholm.example.org">
       <name xml:lang="en">My Friends at ORG</name>
       <name xml:lang="de">Meine Freunde an ORG</name>
     </resource>
   </list>
        */

// ---------------------------------------------------------------------------
// CSimplePluginData::AddPrPersToSimpleDocumentL
// ---------------------------------------------------------------------------
//
void CSimplePluginData::AddPrPersToSimpleDocumentL(
    const MPersonPresenceInfo* aInfo,
    MSimpleDocument& aDocument,
    const TDesC8& aSipId )
    {
    using namespace NSimpleDocument;

    TPtrC8 p8( KSimpleAvatarCIDURI8 );
    TInt bufSize = p8.Length();

    HBufC16* CIDcontent = HBufC16::NewLC( bufSize );   // << CID
    CIDcontent->Des().Copy( KSimpleAvatarCIDURI8 );

    // notice: we do not generate random id but a static one.

    using namespace NSimplePlugin::NSimpleOma;

    aDocument.SetEntityURIL( aSipId );

    MSimpleElement* persElem = aDocument.AddSimpleElementL(
        KSimpleNsPDM, KSimplePerson8 );
    CleanupClosePushL( *persElem );             // << persElem

    using namespace NPresenceInfo::NFieldType;

    const MPresenceInfoFieldCollection& coll = aInfo->Fields();

    TInt myCount = coll.FieldCount();
    for ( TInt i = 0; i < myCount; i++ )
        {
        const MPresenceInfoField& field =  coll.FieldAt( i );
        const TDesC8& fieldType = field.FieldType();
        const MXIMPBase& storage = field.FieldValue();

        // Notice: tuple id saving for partial data in future.

        /*
        Mapping:
        Avatar -> Person/status-icon
        status-msg -> Person/note
        Availability -> Person/overriding-willingness
        */

        if ( !fieldType.CompareF( KAvatar ) )
            {
            const MPresenceInfoFieldValueBinary* bin =
                  TXIMPGetInterface< const MPresenceInfoFieldValueBinary >::From( storage,
                  MXIMPBase::EReturnNullIfUnknown );
            if ( bin )
                {
                MSimpleElement* elem = persElem->AddSimpleElementL(
                        KSimpleNsRPID,
                        KSimpleStatusicon8 );
                CleanupClosePushL( *elem );             // << elem
                elem->SetContentUnicodeL( CIDcontent->Des());
                // Create the MIME multipart content
                MSimpleContent* content = TSimpleFactory::NewContentL(
                    KSimpleAvatarContent8, bin->MimeType() );
                CleanupClosePushL( *content );          // << content
                content->CopyBodyL( bin->BinaryValue( ) );
                aDocument.AddDirectContentL( *content, EFalse );
                CleanupStack::PopAndDestroy( content );         // >> content
                CleanupStack::PopAndDestroy( elem );  // >> elem
                }
            }
        else if ( !fieldType.CompareF( KStatusMessage ) )
            {
            const MPresenceInfoFieldValueText* text =
                  TXIMPGetInterface< const MPresenceInfoFieldValueText >::From( storage,
                  MXIMPBase::EReturnNullIfUnknown );
            if ( text )
                {
                MSimpleElement* elem = persElem->AddSimpleElementL(
                    KSimpleNsPDM,
                    KSimpleNote8 );
                CleanupClosePushL( *elem );             // << elem
                elem->SetContentUnicodeL( text->TextValue() );
                CleanupStack::PopAndDestroy( elem );  // >> elem
                }
            }
        else if ( !fieldType.CompareF( KAvailability ))
            {
            const MPresenceInfoFieldValueText* text =
                  TXIMPGetInterface< const MPresenceInfoFieldValueText >::From( storage,
                  MXIMPBase::EReturnNullIfUnknown );
            if ( text )
                {
                AddPrPersAvailabilityToDocL( text, persElem );
                }         
            }
        else
            {
            // Notice: currently all the fields in the namespace are supported,
            // but this ensures that if namespace is extended later, it is
            // handled right way in the adaptation
            User::Leave( KPresenceErrPresenceInfoFieldTypeNotSupported );
            }
        }
    CleanupStack::PopAndDestroy( persElem );
    CleanupStack::PopAndDestroy( CIDcontent );
    }


// ---------------------------------------------------------------------------
// CSimplePluginData::CollectAllPresentitiesL
// ---------------------------------------------------------------------------
//
void CSimplePluginData::CollectAllPresentitiesL(
    RPointerArray<MPresenceInfo>& aEntities,
    RPointerArray<MXIMPIdentity>& aActives,
    RPointerArray<MSimpleDocument>& aDocs,
    MXIMPObjectFactory& aFactory,
    MPresenceObjectFactory& aPresenceFactory )
    {
    TInt count = aDocs.Count();

    // active presentities
    for ( TInt i = 0; i < count; i++ )
        {
        MPresenceInfo* info = aPresenceFactory.NewPresenceInfoLC();   // << info
        aEntities.AppendL( info );
        // aEntities may contain entries even this method leaves
        CleanupStack::Pop();                                      // >> info

        NotifyToPrInfoL( aPresenceFactory, *aDocs[i], *info );

        // Add SIP identity to active users list
        MXIMPIdentity* active = aFactory.NewIdentityLC();  // << active
        aActives.AppendL( active );
        CleanupStack::Pop();                            // >> active

        // Convert SIP entity URI from UTF to Unicode.
        const TDesC8* pUri8 = (aDocs[i])->EntityURI();
        HBufC16* uri16 = NULL;
        uri16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *pUri8 );
        CleanupStack::PushL( uri16 );  // << uri16
        active->SetIdentityL( uri16->Des() );

#ifdef _DEBUG
    // ---------------------------------------------------------
    TBuf<200> buffer2;
    buffer2.Copy(uri16->Des());
    const MPersonPresenceInfo* pers_debug = info->PersonPresence();
    const MPresenceInfoFieldCollection& coll_debug = pers_debug->Fields();
    TInt count_debug = coll_debug.FieldCount();
    PluginLogger::Log(_L("PluginData: nbr of fields received =%d"), count_debug );
    PluginLogger::Log(_L("PluginData: User added to actives: %S"), &buffer2 );
    // ---------------------------------------------------------
#endif
        CleanupStack::PopAndDestroy( uri16 );  // >> uri16
        }
    }

// ---------------------------------------------------------------------------
// CSimplePluginData::CollectTerminatedPresentitiesL
// ---------------------------------------------------------------------------
//
void CSimplePluginData::CollectTerminatedPresentitiesL(
    RPointerArray<MXIMPIdentity>& aTerminated,
    RPointerArray<MSimpleElement>& aElems,
    MXIMPObjectFactory& aFactory )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginData: CollectTerminatedPresentitiesL"));
#endif
    using namespace NSimplePlugin::NSimpleOma;

    TInt count2 = aElems.Count();
    for ( TInt i = 0; i < count2; i++ )
        {
        MSimpleElement* elem2 = aElems[i];
        if ( !elem2->LocalName().CompareF( KSimpleResource8 ))
            {
            // resource element
            RPointerArray<MSimpleElement> elems3;
            CleanupClosePushL( elems3 );
            elem2->SimpleElementsL( elems3 );
            TInt count3 = elems3.Count();
            const TDesC8* uri8 = elem2->AttrValue( KSimpleUri8 );
            if ( !uri8 )
                {
                continue;
                }
            for ( TInt j=0; j < count3; j++ )
                {
                MSimpleElement* elem3 = elems3[j];
                SearchTerminatedInstanceL( aTerminated, elem3, uri8, aFactory  );
                }
            CleanupStack::PopAndDestroy( &elems3 );
            } // resource element
        }// for (i); list element subelement

#ifdef _DEBUG
    PluginLogger::Log(_L("PluginData: CollectTerminatedPresentitiesL ends"));
#endif
    }


// ---------------------------------------------------------------------------
// CSimplePluginData::SearchLatestTimestampL
// ---------------------------------------------------------------------------
//
TInt CSimplePluginData::SearchLatestTimestampL(
    RPointerArray<MSimpleElement>& aElems  )
    {
    TInt ret = KErrNotFound;

    const TInt KMaxTimestamp = 40;
    TBuf<KMaxTimestamp> myTimestamp;

    // Timestamp to save the latest one person data
    myTimestamp = KNullDesC;

    // They want to get empty notifications too.
    TInt count = aElems.Count();

    using namespace NSimplePlugin::NSimpleOma;

    MSimpleElement* elem = NULL;
    TPtrC8 p8;

    for ( TInt i = 0; i < count; i++ )
        {
        elem = aElems[i];
        p8.Set( elem->LocalName());
        if (!( p8.CompareF( KSimplePerson8 )))
            {

            if ( ret == KErrNotFound )
                {
                // Save since the time stamp is optional, this is first <person> .
                // the whole <person> may also be missing.
                ret = i;
                }

            // person element found
            RPointerArray<MSimpleElement> elems2;
            CleanupClosePushL( elems2 );
            elem->SimpleElementsL( elems2 );
            TInt count2 = elems2.Count();

            // Search optional timestamp element first, the latest one
            for ( TInt j = 0; j < count2; j++ )
                {
                //
                MSimpleElement* elem2 = elems2[j];
                // Search optional timestamp element
                if ( !elem2->LocalName().CompareF( KSimpleTimestamp8 ))
                    {
                    HBufC* nodeContent = elem2->ContentUnicodeL();
                    // Notice: Timestamp comparision assumes now
                    // that all the timestamps have a similar format.
                    if ( myTimestamp.Compare( nodeContent->Des() ) < 0 )
                        {
                        // Save latest timestamp
                        myTimestamp.Copy( nodeContent->Des() );
                        ret = i;
                        }
                    delete nodeContent;
                    }
                }
            CleanupStack::PopAndDestroy( &elems2 );
            }
        }  // for

    return ret;
    }

// ---------------------------------------------------------------------------
// CSimplePluginData::AddPrPersAvailabilityToDocL
// ---------------------------------------------------------------------------
//
void CSimplePluginData::AddPrPersAvailabilityToDocL(
    const MPresenceInfoFieldValueText* aText, MSimpleElement* aPersElem )
    {
    using namespace NSimplePlugin::NSimpleOma;

    MSimpleElement* elem = aPersElem->AddSimpleElementL(
            KSimpleNsOP,
            KSimpleOverridingwillingness8 );
    CleanupClosePushL( *elem );             // << elem
    MSimpleElement* elem2 = elem->AddSimpleElementL(
            KSimpleNsOP,
            KSimpleBasic8 );
    CleanupClosePushL( *elem2 );             // << elem2
    if ( !aText->TextValue().CompareF( KSimpleOpen ) )
        {
        elem2->SetContentUnicodeL( KSimpleOpen );
        }
    else 
        {
        elem2->SetContentUnicodeL( KSimpleClosed );
        }
    CleanupStack::PopAndDestroy( elem2 ); // >> elem2
    CleanupStack::PopAndDestroy( elem );  // >> elem
    }

// ---------------------------------------------------------------------------
// CSimplePluginData::SearchTerminatedInstanceL
// ---------------------------------------------------------------------------
//
void CSimplePluginData::SearchTerminatedInstanceL(
    RPointerArray<MXIMPIdentity>& aTerminated,
    MSimpleElement* aElem,
    const TDesC8* uri8,
    MXIMPObjectFactory& aFactory )
    {
#ifdef _DEBUG
    PluginLogger::Log(_L("PluginData: SearchTerminatedInstanceL"));
#endif

    using namespace NSimplePlugin::NSimpleOma;
    if ( !aElem->LocalName().CompareF( KSimpleInstance8 ))
        {
        // instance element
        const TDesC8* stateVal = aElem->AttrValue( KSimpleState8 );
        if ( stateVal && !stateVal->CompareF( KSimpleTerminated8 ))
            {
            // Add presentity into terminated list
            MXIMPIdentity* terminated = aFactory.NewIdentityLC();  // +terminated
            aTerminated.AppendL( terminated );
            CleanupStack::Pop();                   // -terminated

            // Convert SIP entity URI from UTF to Unicode.
            HBufC16* uri16 = NULL;
            uri16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *uri8 );
            CleanupStack::PushL( uri16 );  // +uri16
            terminated->SetIdentityL( uri16->Des() );

#ifdef _DEBUG
            // Debug buffer size
            const TInt KMyBufSize = 200;
            // ---------------------------------------------------------
            TBuf<KMyBufSize> buffer3;
            buffer3.Copy(uri16->Des());
            PluginLogger::Log(_L("PluginData:   SearchTerminatedInstanceL TERMINATED INSTANCE FOUND ***"));
            PluginLogger::Log(_L("PluginData:   User added to terminated: %S"), &buffer3 );
            // ---------------------------------------------------------
#endif
            CleanupStack::PopAndDestroy( uri16 );   // -uri16
            }
        else
            {
            // -------------------------------------------------------------
#ifdef _DEBUG
            if ( stateVal != NULL )
                {
                // Debug buffer size
                const TInt KMyBufSize = 100;
                HBufC16* uri16 = NULL;
                uri16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *stateVal );
                CleanupStack::PushL( uri16 );  // +uri16
                TBuf<KMyBufSize> buffer4;
                buffer4.Copy(uri16->Des());
                PluginLogger::Log(_L("    STATEVAL: %S"), &buffer4 );
                CleanupStack::PopAndDestroy( uri16 );   // -uri16
                }
#endif
            // -------------------------------------------------------------
            }
        }
    }

