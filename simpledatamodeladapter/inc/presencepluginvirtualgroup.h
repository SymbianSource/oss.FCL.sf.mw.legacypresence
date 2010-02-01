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
* Description:  IETF SIMPLE Protocol, creating virtualgroup
*
*/


#ifndef CPRESENCEPLUGINVIRTUALGROUP_H
#define CPRESENCEPLUGINVIRTUALGROUP_H

#include <e32base.h>
#include <e32std.h>
#include <ximpbase.h>
#include <badesca.h>

#include "mpresencepluginconnectionobs.h"
#include "presencelogger.h"

class MXIMPObjectCollection;
class CPresencePluginData;

/**
 * CPresencePluginVirtualGroup
 *
 * virtual groups
 *
 * @lib presenceplugin.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CPresencePluginVirtualGroup ): public CBase
    {
    public:

        /**
         * Constructor.
         * @param aId watcher id
         * @param aSipId watcher SIP identity
         */
        static CPresencePluginVirtualGroup* NewL( 
            MPresencePluginConnectionObs& aObs,
            const TDesC16& aListName,
            CPresencePluginData* aPresenceData );

        /**
         * Standard C++ destructor
         */ 
        virtual ~CPresencePluginVirtualGroup();           
        
    private:
    
        /**
         * Standard C++ constructor
         * @param aObs, Presence plugin connection             
         * @param aPresenceData, access to presence data
         */
        CPresencePluginVirtualGroup( MPresencePluginConnectionObs& aObs,
            CPresencePluginData* aPresenceData );
        
        /**
         * Performs the 2nd phase of construction.
         */ 
        void ConstructL( const TDesC16& aListName );
        
        /**
         * IdentityExists()
         *
         * @since S60 3.2 
         * @param aIdentityUri, identity url
         * @return TInt, return KErrNotFound if indentity not exists 
         */ 
        TInt IdentityExists( const TDesC& aIdentityUri ); 

    public:
        
        /**
         * GetVirtualIdentityArray()
         *
         * @since S60 3.2 
         * @param aColl, XIMP object collection
         * @return none
         */     
        CDesCArray* GetVirtualIdentityArray( );
        
        /**
         * GetOnlineEntitiesL()
         *
         * @since S60 3.2 
         * @param aColl, XIMP object collection
         * @return none
         */     
        void GetVirtualEntitiesL( MXIMPObjectCollection& aColl );
        
        /**
         * AddOnlineEntity()
         *
         * @since S60 3.2 
         * @param aIdentityUri, identity url
         * @return none
         */ 
        void AddEntityL( const TDesC& aIdentityUri );
        
        /**
         * RemoveOnlineEntity()
         *
         * @since S60 3.2 
         * @param aIdentityUri, identity url
         * @return none
         */ 
        void RemoveEntityL( const TDesC& aIdentityUri );       
        
        /**
         * HandlePresentityGroupMemberAddedL()
         *
         * @since S60 5.1 
         * @param aIdentityUri, identity url
         * @return none
         */ 
        void HandlePresentityGroupMemberAddedL( const TDesC& aIdentityUri );
        
    private: // data
        
        /**
         * XIMP Plugin connection observer
         */
        MPresencePluginConnectionObs& iConnObs;
        
        /**
         * Access to presence data.
         * Not Own.
         */        
        CPresencePluginData*    iPresenceData;
        
        /**
         * Identities/Uri of online Buddies.
         */        
        CDesCArray* iVirtualMember;
        
        /**
         * list ident
         * own
         */
        HBufC16* iListName;
        
        SIMPLE_UNIT_TEST( T_CPresencePluginVirtualGroup )
        SIMPLE_UNIT_TEST( T_CPresencePluginXdmUtils )
        
    };
    
#endif // CPRESENCEPLUGINVIRTUALGROUP_H