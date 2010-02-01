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
* Description:    Simple Protocol implementation for Presence Framework
*
*/




#ifndef CSIMPLEPLUGIN_H
#define CSIMPLEPLUGIN_H

#include <e32std.h>

#include <ximpbase.h>
#include <ximpprotocolpluginbase.h>

class MXIMPProtocolConnection;
class MXIMPServiceInfo;
class MXIMPClientContextInfo;
class CSimplePluginConnection;


/**
 * CSimplePlugin
 *
 * Implementation of CPrFwProtocolPluginBase
 *
 * @lib ?library
 * @since s60 v5.0
 */
class CSimplePlugin: public CXIMPProtocolPluginBase
    {
public:

    static CSimplePlugin* NewL();
    static CSimplePlugin* NewLC();

    virtual ~CSimplePlugin();

private:

    CSimplePlugin();
    void ConstructL();


public:

// from base class MXIMPProtocolPlugin

    /**
     * Defined in a base class
     */
    void PrimeHost( MXIMPProtocolPluginHost& aHost );

    /**
     * Defined in a base class
     */
    MXIMPProtocolConnection& AcquireConnectionL(
        const MXIMPServiceInfo& aService,
        const MXIMPContextClientInfo& aContextClient );

    /**
     * Defined in a base class
     */
    void ReleaseConnection(
        MXIMPProtocolConnection& aConnection );

// from base class MXIMPBase

    /**
     * Defined in a base class
     */
    TAny* GetInterface(
        TInt32 aInterfaceId,
        TIfGetOps aOps );

    /**
     * Defined in a base class
     */
    const TAny* GetInterface(
        TInt32 aInterfaceId,
        TIfGetOps aOps ) const;

    /**
     * Defined in a base class
     */
    TInt32 GetInterfaceId() const;


private: // Data

    /*
     * Prime host
     * Not own.
     */
    MXIMPProtocolPluginHost* iHost;

    /**
     * Simple plugin connections
     */
    RPointerArray< CSimplePluginConnection > iConnections;

    };

#endif // CSIMPLEPLUGIN_H
