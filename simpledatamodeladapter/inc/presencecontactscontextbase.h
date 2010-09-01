/*
* Copyright (c) 20010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef M_MPRESENCECONTACTSCONTEXTBASE_H
#define M_MPRESENCECONTACTSCONTEXTBASE_H


class MVPbkContactLinkArray;
class MVPbkContactLink;


/**
 *  Base class to state context
 *
 *  @lib presenceplugin.dll
 *  @since S60 v5.0
 */
class MPresenceContactsContextBase
    {

public:

    /**
     * Open virtual phonebook contact stores
     *
     * @since S60 v5.0
     */
    virtual void OpenL() = 0;

    /**
     * Start virtual phonebook contacts search
     *
     * @since S60 v5.0
     */
    virtual void SearchL() = 0;

    /**
     * Return virtual phonebook contact links
     *
     * @since S60 v5.0
     * @return virtual phonebook contact links
     */
    virtual const MVPbkContactLinkArray& ContactLinkArrayL() = 0;
    
    /**
     * Resolve virtual phonebook contact from contact link
     *
     * @since S60 v5.0
     * @param aContactLink contact link to resolve
     */
    virtual void RetrieveContactL(
            const MVPbkContactLink& aContactLink ) = 0;

    };


#endif // M_MPRESENCECONTACTSCONTEXTBASE_H
