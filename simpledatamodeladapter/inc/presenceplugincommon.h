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

#ifndef PRESENCEPLUGINCOMMON_H
#define PRESENCEPLUGINCOMMON_H

#include <e32std.h>

/**
 * common definitions for Presence Plugin
 */

//Define max tuple and person id integer
const TInt KMaxNumber = 99999999;

//Buf size defines

const TInt KBufSize100 = 100;
const TInt KBufSize200 = 200;
const TInt KBufSize255 = 255;
const TInt KBufSize20 = 20;
const TInt KBufSize10 = 10;

// Presence Plugin specifications

_LIT( KPluginName, "CPresencePlugin");

/** Nothing subscribed */
const TUint KPresencePluginSubsNone = 0x00000000;
/** Grant list subscribed */
const TUint KPresencePluginSubsGrant = 0x00000001;
/** Block list subscribed */
const TUint KPresencePluginSubsBlock = 0x00000002;
/** Winfo list subscribed */
const TUint KPresencePluginSubsWinfo = 0x00000004;

//XDM Resourcelist
/** buddy list id */
_LIT( KPresenceBuddyList, "buddylist");
/**  blocked list id */
_LIT( KPresenceBlockedList, "blockedcontacts");
/** for virtual online buddy group */
_LIT( KPresenceOnlineBuddy ,"online_buddies");
/** Subscribed buddys */
_LIT( KPresenceSubcribedBuddys, "subs_buddys");

//XDM Presencerules list 
/** Presence XDM block rule id */
_LIT( KPresenceXdmBlockRule, "pres_blacklist");
/** Presence XDM whitelist*/
_LIT( KPresenceXdmWhiteList, "pres_whitelist");
/** Presence XDM grant rule id */
_LIT( KPresenceXdmGrantRule, "pres_whitelist");
/** S60 Shared XDM list for granted entities */
_LIT( KPresenceS60GrantedList, "s60_granted_list");

//For localmode check
_LIT(KLocalhost, "http://localhost");

// when contact goes offline this is content of aBasicElement->ContentUnicodeL()
_LIT( KClose ,"close");

//For own presence status NOTIFY
_LIT(KIndent, "t"); // for tupleId
_LIT(KPersonId, "p"); // for person

// Temporary hard coded values for presence cache 2 support. Common definitions needed!
_LIT( KDefaultAvailableStatus, "available" );


namespace NPresencePlugin
    {
    namespace NPresence
        {
        _LIT( KPresenceDisplayName, "display-name");        
        _LIT( KPresenceIndex,   "index");
        _LIT( KPresenceResourceLists, "resource-lists");
        _LIT( KPresenceResourceList, "resource-list.xml");
        _LIT( KPresenceConditions, "conditions");
        _LIT( KPresenceIdentity, "identity");
        _LIT( KPresenceRuleset, "ruleset" );
        _LIT( KPresenceRule, "rule");
        _LIT( KPresenceAction, "actions");
        _LIT( KPresenceAllow, "allow");
        _LIT8( KPresenceAllow8, "allow");
        _LIT( KPresenceBlock, "block");
        _LIT8( KPresenceBlock8, "block");        
        _LIT( KPresenceSub, "sub-handling");
        _LIT( KPresencePresRules, "presrules");
        _LIT( KPresenceList, "list");
        _LIT( KPresenceName, "name");
        _LIT( KPresenceEntry, "entry");
        _LIT( KPresenceOne , "one");
        _LIT( KPresencePerson, "person");
        _LIT8( KPresencePerson8, "person"); 
        _LIT8( KPresenceWatcher8, "watcher"); 
        _LIT8( KPresenceWatherInfo8, "watcherinfo");                   
        _LIT8( KPresenceWatcherList8, "watcher-list"); 
        _LIT8( KPresenceStatus8, "status");  
        _LIT8( KPresenceActive8, "active");
        _LIT8( KPresenceEvent8, "event");        
        _LIT8( KPresencePending8, "pending");
        _LIT8( KPresenceBasic8, "basic"); 
        _LIT( KPresenceBasic, "basic");    
        _LIT8( KPresenceStatusicon8, "status-icon");
        _LIT8( KPresenceNote8, "note");                               
        _LIT8( KPresenceSubscribe8, "subscribe");                                                                 
        _LIT( KPresenceUri, "uri");
        _LIT8( KPresenceUri8, "uri");
        _LIT8( KPresenceState8, "state");
        _LIT8( KPresenceInstance8, "instance");
        _LIT8( KPresenceTerminated8, "terminated");
        _LIT8( KPresenceResource8, "resource");
        _LIT8( KPresenceList8, "list");
        _LIT8( KPresenceTuple8, "tuple");
        _LIT( KPresenceTuple, "tuple"); 
        _LIT( KPresenceId, "id");
        _LIT8( KPresenceId8, "id");      
        _LIT( KPresenceActivities, "activities");
        _LIT8( KPresenceActivities8, "activities");
        _LIT8( KPresenceFull8, "full");
        _LIT( KPresencePartial, "partial");
        _LIT8( KPresencePartial8, "partial");       
        }
    namespace NPresenceStates
        {
        _LIT( KPresenceOpen, "open");
        _LIT( KPresenceClosed, "closed");
        _LIT( KPresenceClose, "close");
        _LIT( KPresenceOffline, "offline");
        _LIT8( KPresenceOffline8, "offline");
        _LIT( KPresenceAvailable, "available");
        _LIT8( KPresenceAvailable8, "available");
        _LIT( KPresenceUnknow, "unknown");      
        _LIT8( KPresenceUnknow8, "unknown");
        _LIT( KPresenceBusy, "busy");
        _LIT8( KPresenceBusy8, "busy");
        _LIT( KPresenceOnThePhone, "on-the-phone");
        _LIT8( KPresenceOnThePhone8, "on-the-phone");
        _LIT( KPresenceAway, "away");
        _LIT8( KPresenceAway8, "away");
        _LIT( KPresenceDoNotDisturb, "do-not-disturb");
        _LIT8( KPresenceDoNotDisturb8, "do-not-disturb");
        _LIT( KPresencePending, "pending" );
        }
    namespace NPresenceTransformations
        {
        _LIT( KPresenceTRUE, "TRUE");
        _LIT( KPresenceFALSE, "FALSE");
        _LIT( KPresenceTrasformations, "transformations");
        _LIT( KPresenceProvideServices, "provide-services");
        _LIT( KPresenceAllServices, "all-services");
        _LIT( KPresenceProvidePersons, "provide-persons");
        _LIT( KPresenceAllPersons, "all-persons");
        _LIT( KPresenceProvidedevices, "provide-devices");
        _LIT( KPresenceAlldevices, "all-devices");
        _LIT( KPresenceProvideAllAttributes, "provide-all-attributes");
        _LIT( KPresenceProvideActivities, "provide-activities");
        _LIT( KPresenceProvideNote, "provide-note");
        } 
    }
    
#endif // PRESENCEPLUGINCOMMON_H