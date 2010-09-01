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


#ifndef C_CSIMPLEIMFEATURES_H
#define C_CSIMPLEIMFEATURES_H

#include <e32base.h>
#include <cprotocolimfeatures.h>

class CSimpleImConversation;
class MPresencePluginConnectionObs;
class MSimpleConnection;

/**
 * Main class of XIMP FW plugin providing SIP IM feature.
 *
 * @lib simpleimplugin.dll
 * @since S60 v5.1
 */
NONSHARABLE_CLASS( CSimpleImFeatures ) : public CProtocolImFeatures
{
public:
    
    /**
     * This class encapsulates SIP IM plugin initialization parameters.
     *
     * @lib simpleimplugin.dll
     * @since S60 v5.1
     */
    class TSimpleImPluginInitParams
        {
        public:

            /**
             * Constructor.
             * @param   aObs    Presence plugin connection observer.
             * @param   aConn   Simple engine connection.
             */
            TSimpleImPluginInitParams( MPresencePluginConnectionObs& aObs,
                MSimpleConnection& aConn, TUint aServiceId ) : iObserver( aObs ),
                iConnection( aConn ), iServiceId( aServiceId ) {};

        public:
        
            /**
             * Presence plugin connnection observer.
             * Not own.
             */    
            MPresencePluginConnectionObs& iObserver;
        
            /**
             * Simple engine connection.
             * Not own.
             */
            MSimpleConnection& iConnection;
            
            /**
             * Service identifier.
             */
            TUint iServiceId;
        };

    /**
     * Constructor.
     * @param   aInitParams   Plugin initialization parameters.
     */
    static CSimpleImFeatures* NewL( TAny* aInitParams );

    /**
     * Standard C++ destructor.
     */ 
    virtual ~CSimpleImFeatures();

private:

    CSimpleImFeatures();

    void ConstructL( TAny* aInitParams );

public: // From MXIMPBase

    /**
     * From MXIMPBase.
     * @see MXIMPBase.
     */
    TAny* GetInterface( 
        TInt32 aInterfaceId, 
        TIfGetOps aOptions );

    /**
     * From MXIMPBase.
     * @see MXIMPBase.
     */
    const TAny* GetInterface( 
        TInt32 aInterfaceId, 
        TIfGetOps aOptions ) const;

    /**
     * From MXIMPBase.
     * @see MXIMPBase.
     */
    TInt32 GetInterfaceId() const;

public : // From CProtocolImFeatures

    /**
     * From CProtocolImFeatures.
     * @see CProtocolImFeatures.
     */
    MProtocolImGroup& ImGroup();
    
    /**
     * From CProtocolImFeatures.
     * @see CProtocolImFeatures.
     */
    MProtocolImConversation& ImConversation();
    
    /**
     * From CProtocolImFeatures.
     * @see CProtocolImFeatures.
     */
    MProtocolImInvitation& ImInvitation();
    
    /**
     * From CProtocolImFeatures.
     * @see CProtocolImFeatures.
     */
    MProtocolImSearch& ImSearch();
    
    /**
     * From CProtocolImFeatures.
     * @see CProtocolImFeatures.
     */
    void SetHost( MProtocolImDataHost& aHost );
    
    /**
     * From CProtocolImFeatures.
     * @see CProtocolImFeatures.
     */
    TInt SupportedSubInterfaces() const;

private : // data members
        
    /**
     * IM conversation.
     * Own.
     */
    CSimpleImConversation* iImConversation;

    /**
     * Protocol IM data host.
     * Not own.
     */
    MProtocolImDataHost* iHost;

    };

#endif // C_CSIMPLEIMFEATURES_H
