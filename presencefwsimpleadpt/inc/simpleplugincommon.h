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
* Description:    C++ compatible definitions for PrFw SIMPLE protocol
*
*/



#ifndef SIMPLEPLUGINCOMMON_H
#define SIMPLEPLUGINCOMMON_H

#include <e32std.h>
#include <e32err.h>


/**
 * common definitions for SIMPLE Plugin
 */

// Simple Plugin specifications

/** Nothing subscribed */
const TUint KSimplePluginSubsNone = 0x00000000;
/** Grant list subscribed */
const TUint KSimplePluginSubsGrant = 0x00000001;

/** max length of auth rule id in auth rules document */
const TUint KSimplePluginMaxRuleLength = 100;

/** the last general error code in e32err.h */
const TInt KSimplePluginGeneralErrorLow = KErrCommsBreak;

// OMA specifications

/** OMA buddy list id */
_LIT( KSimpleOMABuddyList, "oma_buddylist");
/** OMA blocked list id */
_LIT( KSimpleOMABlockedList, "oma_blockedcontacts");


namespace NSimplePlugin
    {
    namespace NSimpleRls
        {
        _LIT( KOne,   "http://");   // 7
        _LIT( KThree, "/resource-lists/users/"); // 22
        _LIT( KFive,  "/index/~~/resource-lists/list%5b@name=%22");  // 41
        _LIT( KSeven, "%22%5d");  // 6
        const TInt totalSize = 76;
        }
    namespace NSimpleRlsBuddy
        {
        _LIT( KOne,   "http://"); // 7
        _LIT( KThree, "/resource-lists/users/"); // 22
        _LIT( KFive,  "/index/~~/resource-lists/list%5b@name=%22oma_buddylist%22%5d/list%5b@name=%22");  // 77
        _LIT( KSeven, "%22%5d");  // 6
        const TInt totalSize = 112;
        }
    namespace NSimpleOma
        {
        _LIT( KSimpleIndex,   "index");
        _LIT( KSimpleResourceLists, "resource-lists");
        _LIT( KSimpleList, "list");
        _LIT( KSimpleName, "name");
        _LIT( KSimpleDisplayName, "display-name");
        _LIT( KSimpleEntry, "entry");
        _LIT( KSimpleExternal, "external");  
        _LIT( KSimpleAnchor, "anchor");               
        _LIT( KSimplePerson, "person");      
        _LIT( KSimpleBlock, "block");
        _LIT( KSimplePoliteBlock, "polite-block");
        _LIT( KSimpleConfirm, "confirm");
        _LIT( KSimpleAllow, "allow");         
        _LIT( KSimpleOpen, "open");
        _LIT( KSimpleClosed, "closed");
        _LIT( KSimpleUri, "uri"); 
        _LIT8( KSimplePerson8, "person");                      
        _LIT8( KSimpleWatcher8, "watcher");
        _LIT8( KSimpleWatherInfo8, "watcherinfo");
        _LIT8( KSimpleWatcherList8, "watcher-list");
        _LIT8( KSimpleStatus8, "status");
        _LIT8( KSimpleActive8, "active");
        _LIT8( KSimpleEvent8, "event");
        _LIT8( KSimplePending8, "pending");
        _LIT8( KSimpleOverridingwillingness8, "overriding-willingness");
        _LIT8( KSimpleBasic8, "basic");       
        _LIT8( KSimpleStatusicon8, "status-icon");
        _LIT8( KSimpleNote8, "note");
        _LIT8( KSimpleSubscribe8, "subscribe");
        _LIT8( KSimpleList8, "list");
        _LIT8( KSimpleResource8, "resource");
        _LIT8( KSimpleInstance8, "instance");
        _LIT8( KSimpleState8, "state");
        _LIT8( KSimpleTerminated8, "terminated");
        _LIT8( KSimpleUri8, "uri");
        _LIT8( KSimpleFull8, "full");
        _LIT8( KSimpleFullState8, "fullState");
        _LIT8( KSimpleTrue8, "true");
        _LIT8( KSimpleId8, "id"); 
        _LIT8( KSimpleTimestamp8, "timestamp");       
        }
    }


// S60 specifications

/** Presence XDM block rule id */
_LIT( KSimpleXdmBlockRule, "blockall");
/** Presence XDM grant rule id */
_LIT( KSimpleXdmGrantRule, "allowall");
/** Presence XDM grant rule id for own sbscribe*/
_LIT( KSimpleXdmOwnGrantRule, "grantown");
/** Presence XDM default rule id */
_LIT( KSimpleXdmDefaultRule, "default");

/** S60 Shared XDM list for granted entities */
_LIT( KSimpleS60GrantedList, "oma_grantlist");

/** S60 default RLS URI template */ 
_LIT( KSimpleRlsGroupUriTemplate, "<GROUP_ID>_<USER_ID>@<DOMAIN>");
_LIT16( KSimpleGroupTag, "<GROUP_ID>");
_LIT16( KSimpleUserTag, "<USER_ID>"); 
_LIT16( KSimpleDomainTag, "<DOMAIN>"); 

/**
 example:

 <rls-services ...
   <service uri="sip:my_friends_MY-IDENTITY@EXAMPLE.COM">
      <resource-list>http://EXAMPLE.COM/resource-lists/users/
        sip:USER@EXAMPLE.COM/index/~~/resource-lists/list%5b@name=%22oma_buddylist%22%5d/list%5b@name=%22my_friends%22%5d
      </resource-list>
      <packages>
        <package>presence</package>
      </packages>
   </service>
   <service uri="sip:my_colleques_MY-IDENTITY@EXAMPLE@EXAMPLE.COM">
      <resource-list>http://EXAMPLE.COM/resource-lists/users/
        sip:USER@EXAMPLE.COM/index/~~/resource-lists/list%5b@name=%22oma_buddylist%22%5d/list%5b@name=%22my_colleques%22%5d
      </resource-list>
      <packages>
        <package>presence</package>
      </packages>
   </service>
 </rls-services

 */

namespace NSimpleDocument
    {
    _LIT8( KSimpleAvatarContent8, "simpleavatar.s60.com" ); // notice: static now, supports only one
    _LIT8( KSimpleAvatarCIDURI8, "cid:simpleavatar.s60.com" ); // notice: static now, supports only one
    }


#endif // SIMPLEPLUGINDEF_H

