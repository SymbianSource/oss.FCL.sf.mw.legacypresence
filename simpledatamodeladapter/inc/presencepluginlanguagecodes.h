/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef T_PRESENCEPLUGINLANGUAGECODES_H
#define T_PRESENCEPLUGINLANGUAGECODES_H

#include <e32base.h>

/** ISO 639-1 language code length. */
const TInt KIsoLanguageCodeLength = 2;

/**
 *  Maps Symbian and ISO 639 language codes.
 *
 *  @since S60 v5.1
 */
class TLanguageCodeMapping
    {

public:

    /**
     * Returns Symbian language code.
     * 
     * @since   S60 5.1
     * @return  Symbian language code.
     */
    TInt SymbianLanguageCode() const 
        { return iSymbianLanguageCode; }
    
    /**
     * Returns ISO language code.
     * 
     * @since   S60 5.1
     * @return  ISO language code.
     */
    TPtrC8 IsoLanguageCode() const
        { return TPtrC8( iIsoLanguageCode ); }
    
public: // data

    /**
     * Symbian language code as specified in e32lang.h.
     */
    TInt iSymbianLanguageCode;
    
    /**
     * ISO 639 language code.
     */
    TText8 iIsoLanguageCode[__Align8( KIsoLanguageCodeLength + sizeof( "" ) )];
    };

/** Mapping table for Symbian and ISO 639 language codes. */
static const TLanguageCodeMapping KLanguageCodeMappings[] =
    {
        { ELangEnglish,             "en" },
        { ELangFrench,              "fr" },
        { ELangGerman,              "de" },
        { ELangSpanish,             "es" },
        { ELangItalian,             "it" },
        { ELangSwedish,             "sv" },
        { ELangDanish,              "da" },
        { ELangNorwegian,           "no" },
        { ELangFinnish,             "fi" },
        { ELangAmerican,            "en" },
        { ELangSwissFrench,         "fr" },
        { ELangSwissGerman,         "de" },
        { ELangPortuguese,          "pt" },
        { ELangTurkish,             "tr" },
        { ELangIcelandic,           "is" },
        { ELangRussian,             "ru" },
        { ELangHungarian,           "hu" },
        { ELangDutch,               "nl" },
        { ELangBelgianFlemish,      "nl" },
        { ELangAustralian,          "en" },
        { ELangBelgianFrench,       "fr" },
        { ELangAustrian,            "de" },
        { ELangNewZealand,          "en" },
        { ELangInternationalFrench, "fr" },
        { ELangCzech,               "cs" },
        { ELangSlovak,              "sk" },
        { ELangPolish,              "pl" },
        { ELangSlovenian,           "sl" },
        { ELangTaiwanChinese,       "zh" },
        { ELangHongKongChinese,     "zh" },
        { ELangPrcChinese,          "zh" },
        { ELangJapanese,            "ja" },
        { ELangThai,                "th" },
        { ELangArabic,              "ar" },
        { ELangAfrikaans,           "af" },
        { ELangAlbanian,            "sq" },
        { ELangAmharic,             "am" },
        { ELangArabic,              "ar" },
        { ELangArmenian,            "hy" },
        { ELangTagalog,             "tl" },
        { ELangBelarussian,         "be" },
        { ELangBengali,             "bn" },
        { ELangBulgarian,           "bg" },
        { ELangBurmese,             "my" },
        { ELangCatalan,             "ca" },
        { ELangCroatian,            "hr" },
        { ELangCanadianEnglish,     "en" },
        { ELangInternationalEnglish,"en" },
        { ELangSouthAfricanEnglish, "en" },
        { ELangEstonian,            "et" },
        { ELangFarsi,               "fa" },
        { ELangCanadianFrench,      "fr" },
        { ELangScotsGaelic,         "gd" },
        { ELangGeorgian,            "ka" },
        { ELangGreek,               "el" },
        { ELangCyprusGreek,         "el" },
        { ELangGujarati,            "gu" },
        { ELangHebrew,              "he" },
        { ELangHindi,               "hi" },
        { ELangIndonesian,          "id" },
        { ELangIrish,               "ga" },
        { ELangSwissItalian,        "it" },
        { ELangKannada,             "kn" },
        { ELangKazakh,              "kk" },
        { ELangKhmer,               "km" },
        { ELangKorean,              "ko" },
        { ELangLao,                 "lo" },
        { ELangLatvian,             "lv" },
        { ELangLithuanian,          "lt" },
        { ELangMacedonian,          "mk" },
        { ELangMalay,               "ms" },
        { ELangMalayalam,           "ml" },
        { ELangMarathi,             "mr" },
        { ELangMoldavian,           "mo" },
        { ELangMongolian,           "mn" },
        { ELangNorwegianNynorsk,    "no" },
        { ELangBrazilianPortuguese, "pt" },
        { ELangPunjabi,             "pa" },
        { ELangRomanian,            "ro" },
        { ELangSerbian,             "sr" },
        { ELangSinhalese,           "si" },
        { ELangSomali,              "so" },
        { ELangInternationalSpanish,"es" },
        { ELangLatinAmericanSpanish,"es" },
        { ELangSwahili,             "sw" },
        { ELangFinlandSwedish,      "sv" },
        { ELangTamil,               "ta" },
        { ELangTelugu,              "te" },
        { ELangTibetan,             "bo" },
        { ELangTigrinya,            "ti" },
        { ELangCyprusTurkish,       "tr" },
        { ELangTurkmen,             "tk" },
        { ELangUkrainian,           "uk" },
        { ELangUrdu,                "ur" },
        { ELangVietnamese,          "vi" },
        { ELangWelsh,               "cy" },
        { ELangZulu,                "zu" },
        { ELangManufacturerEnglish, "en" },
        { ELangSouthSotho,          "st" },
        { ELangBasque,              "eu" },
        { ELangGalician,            "gl" },
        { ELangEnglish_Apac,        "en" },
        { ELangEnglish_Taiwan,      "en" },
        { ELangEnglish_HongKong,    "en" },
        { ELangEnglish_Prc,         "en" },
        { ELangEnglish_Japan,       "en" },
        { ELangEnglish_Thailand,    "en" },
        { ELangMalay_Apac,          "ms" }
    };

/** Number of language code mappings. */
static const TInt KLanguageCodeMappingsCount =
    sizeof ( KLanguageCodeMappings ) / sizeof ( TLanguageCodeMapping );

#endif // T_PRESENCEPLUGINLANGUAGECODES_H
