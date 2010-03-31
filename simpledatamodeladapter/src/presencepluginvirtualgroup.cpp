/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <protocolpresentitygroupsdatahost.h>
#include <presentitygroupmemberinfo.h>
#include <protocolpresencedatahost.h>
#include <presenceobjectfactory.h>
#include <ximpobjectcollection.h>
#include <ximpobjectfactory.h>
#include <ximpidentity.h>

#include "presencepluginvirtualgroup.h"
#include "mpresencepluginconnectionobs.h"
#include "presenceplugincommon.h"
#include "presenceplugindata.h"

const TInt KArrayGranularity = 5; 

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginVirtualGroup::~CPresencePluginVirtualGroup
// ---------------------------------------------------------------------------
CPresencePluginVirtualGroup::CPresencePluginVirtualGroup( 
    MPresencePluginConnectionObs& aObs,
    CPresencePluginData* aPresenceData )
    :iConnObs( aObs ),
    iPresenceData( aPresenceData ),
    iVirtualMember( NULL )
    {
    DP_SDA("CPresencePluginVirtualGroup::CPresencePluginVirtualGroup");
    }

// ---------------------------------------------------------------------------
// CPresencePluginVirtualGroup::~CPresencePluginVirtualGroup
// ---------------------------------------------------------------------------
CPresencePluginVirtualGroup::~CPresencePluginVirtualGroup()
    {
    DP_SDA("CPresencePluginVirtualGroup::~CPresencePluginVirtualGroup1");
    delete iVirtualMember;
    delete iListName;
    DP_SDA("CPresencePluginVirtualGroup::~CPresencePluginVirtualGroup2");
    }

// ---------------------------------------------------------------------------
// CPresencePluginVirtualGroup::NewL
// ---------------------------------------------------------------------------
//
CPresencePluginVirtualGroup* CPresencePluginVirtualGroup::NewL( 
    MPresencePluginConnectionObs& aObs,
    const TDesC16& aListName,
    CPresencePluginData* aPresenceData )
    {
    CPresencePluginVirtualGroup* self =
        new (ELeave) CPresencePluginVirtualGroup( aObs, aPresenceData );
    CleanupStack::PushL( self );
    self->ConstructL( aListName );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginVirtualGroup::ConstructL
// ---------------------------------------------------------------------------
//
void CPresencePluginVirtualGroup::ConstructL( const TDesC16& aListName )
    {
    DP_SDA("CPresencePluginVirtualGroup::ConstructL begin");
    iVirtualMember = new (ELeave) CDesCArrayFlat( KArrayGranularity );
    iListName = aListName.AllocL();
    DP_SDA("CPresencePluginVirtualGroup::ConstructL end");        
    }

// ---------------------------------------------------------------------------
// CPresencePluginVirtualGroup::GetVirtualEntities
// ---------------------------------------------------------------------------
void CPresencePluginVirtualGroup::GetVirtualEntitiesL( 
    MXIMPObjectCollection& aColl )
    {           
    DP_SDA("CPresencePluginVirtualGroup::GetVirtualEntities begin");
    TInt count = iVirtualMember->Count();
    if ( count )
        {
        DP_SDA("CPresencePluginVirtualGroup MXIMPObjectFactory& myFactory");
        MXIMPObjectFactory& myFactory = iConnObs.ObjectFactory();
        for ( TInt i=0; i < count; i++ )
            {
            MXIMPIdentity* member = myFactory.NewIdentityLC(); 
            HBufC* withoutPrefix = iPresenceData->RemovePrefixLC( (*iVirtualMember)[i] );
            member->SetIdentityL( *withoutPrefix );
            CleanupStack::PopAndDestroy( withoutPrefix );
            // create here MXIMPPresentityGroupMemberInfo
            MPresentityGroupMemberInfo* info = 
                iConnObs.PresenceObjectFactoryOwn().
                    NewPresentityGroupMemberInfoLC();
                    
            DP_SDA("CPresencePluginVirtualGroup:: info->SetGroupMemberIdL");
            info->SetGroupMemberIdL( member ); //ownership taken
            info->SetGroupMemberDisplayNameL( member->Identity() );
            aColl.AddObjectL( info );           
            CleanupStack::Pop( 2 ); // >> info, member
            DP_SDA2("CPresencePluginVirtualGroup:: done= handle next = %d", i);
            }
        DP_SDA("CPresencePluginVirtualGroup::GetOnlineEntities all done");
        }
    DP_SDA("CPresencePluginVirtualGroup::GetOnlineEntities end");        
    }

// ---------------------------------------------------------------------------
// CPresencePluginVirtualGroup::AddEntityL
// ---------------------------------------------------------------------------
void CPresencePluginVirtualGroup::AddEntityL( const TDesC& aIdentityUri )
    {
    DP_SDA("CPresencePluginVirtualGroup::AddOnlineEntity - begin");
    HBufC* prefixStripped = aIdentityUri.AllocLC();
    if ( KErrNotFound == IdentityExists( aIdentityUri ) )
        {
        DP_SDA("CPresencePluginVirtualGroup::ivirtuaBuddys->AddObjectL");
        iVirtualMember->AppendL( *prefixStripped );
        }
    CleanupStack::PopAndDestroy( prefixStripped );
    DP_SDA("CPresencePluginVirtualGroup::AddOnlineEntity - end");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginVirtualGroup::RemoveOnlineEntity
// ---------------------------------------------------------------------------
void CPresencePluginVirtualGroup::RemoveEntityL( 
    const TDesC& aIdentityUri )
    {
    DP_SDA("CPresencePluginVirtualGroup::RemoveOnlineEntity - begin");
    if ( iVirtualMember->Count() )
        {
        DP_SDA("CPresencePluginVirtualGroup::RemoveOnlineEntity - find");        
        HBufC* prefixStripped = aIdentityUri.AllocLC();
        TInt result = IdentityExists( *prefixStripped );
        if ( KErrNotFound != result && result < iVirtualMember->Count() )
            {
            DP_SDA("CPresencePluginVirtualGroup:: - Remove");        
            iVirtualMember->Delete( result );
            }        
        CleanupStack::PopAndDestroy( prefixStripped );                   
        }
    DP_SDA("CPresencePluginVirtualGroup::RemoveOnlineEntity - end");        
    }

// ---------------------------------------------------------------------------
// CPresencePluginVirtualGroup::IdentityExists
// ---------------------------------------------------------------------------
TInt CPresencePluginVirtualGroup::IdentityExists( const TDesC& aIdentityUri )
    {
    DP_SDA("CPresencePluginVirtualGroup::IdentityExists - begin");
    
    TInt result( KErrNotFound );
    
    for ( TInt i=0; i<iVirtualMember->Count(); i++ )
        {
        if ( (*iVirtualMember)[i].CompareF( aIdentityUri ) == 0 )
            {
            DP_SDA2("IdentityExists - found at index = %d", i);
            result = i;
            break;
            }
        }
            
    DP_SDA("CPresencePluginVirtualGroup::IdentityExists - end");                
    return result;
    }
 
// ---------------------------------------------------------------------------
// CPresencePluginVirtualGroup::GetVirtualIdentityArray
// --------------------------------------------------------------------------- 
CDesCArray* CPresencePluginVirtualGroup::GetVirtualIdentityArray( )
	{
 	DP_SDA("CPresencePluginVirtualGroup::GetVirtualIdentityArray - begin");
 	return iVirtualMember;	
 	} 
 	
// ---------------------------------------------------------------------------
// CPresencePluginVirtualGroup::HandlePresentityGroupMemberAddedL
// --------------------------------------------------------------------------- 
void CPresencePluginVirtualGroup::HandlePresentityGroupMemberAddedL(
    const TDesC& aIdentityUri )
    {
    DP_SDA("CPresencePluginVirtualGroup::HandlePresentityGroupMemberAddedL");

    MXIMPObjectFactory& myFactory = iConnObs.ObjectFactory();
    MProtocolPresentityGroupsDataHost& dataHost =
        iConnObs.ProtocolPresenceHost().GroupsDataHost();
    MXIMPIdentity* groupid = myFactory.NewIdentityLC(); 
    groupid->SetIdentityL( iListName->Des() );
    MXIMPIdentity* member = myFactory.NewIdentityLC(); 
    member->SetIdentityL( aIdentityUri );
    MPresentityGroupMemberInfo* info = 
       iConnObs.PresenceObjectFactoryOwn().NewPresentityGroupMemberInfoLC();
    info->SetGroupMemberIdL( member ); //ownership taken
    info->SetGroupMemberDisplayNameL( member->Identity() );
    dataHost.HandlePresentityGroupMemberAddedL( 
        groupid, info );// ownership taken
    DP_SDA("CPresencePluginVirtualGroup:: Notify Pop( 3 )");
    CleanupStack::Pop( 3 ); // groupid, member, info
    
    DP_SDA("CPresencePluginVirtualGroup::HandlePresentityGroupMemberAddedL End");
    }

// End of file
