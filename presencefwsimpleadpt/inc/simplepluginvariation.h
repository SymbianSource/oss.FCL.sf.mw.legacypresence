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
* Description:    CSimplePluginVariation implementation.
*
*/




#ifndef CSIMPLEPLUGINVARIATION_H
#define CSIMPLEPLUGINVARIATION_H

#include <e32std.h>

class CRepository;

/**
 * CSimplePluginVariation
 *
 * Implementation of MXIMPProtocolConnection
 *
 * @lib ?library
 * @since s60 v5.0
 */
NONSHARABLE_CLASS(CSimplePluginVariation) : public CBase
    {

public:

    enum TSimplePluginAuthRule
        {
        ERuleNone,
        ERulePoliteBlock,
        ERuleBlock,
        ERuleConfirm,
        ERuleAllow
        };

    static CSimplePluginVariation* NewL( );

    ~CSimplePluginVariation();

private:

    CSimplePluginVariation();


    void ConstructL( );


public:

    /**
     * Get data from CenRep
     */
    void InitL();

    /**
     * Grant rule name
     * @return rule name
     */
    TPtrC GrantRuleName(); 
    
    /**
     * Grant own rule name
     * @return rule name
     */
    TPtrC GrantOwnRuleName();
    
    /**
     * Default rule name
     * @return rule name
     */
    TPtrC DefaultRuleName();
    
    /**
     * Block rule name
     * @return rule name
     */
    TPtrC BlockRuleName();  
    
    /**
     * Default rule action
     * @return action
     */
    TSimplePluginAuthRule DefaultRuleAction();
    
    /**
     * Block rule action
     * @return action
     */
    TSimplePluginAuthRule BlockRuleAction();
    
    /**
     * RlsGroupUriTemplate
     * @return RlsGroupUriTemplate
     */
    TPtrC RlsGroupUriTemplate();
    
    /**
     * SharedXdmTop
     * @return SharedXdmTop
     */
    TPtrC SharedXdmTop();
    
    /**
     * GrantListName
     * @return GrantListName
     */
    TPtrC GrantListName();  
    
private:

    void InitGrantRuleNameL( CRepository* aRepository );
    
    void InitGrantOwnRuleNameL( CRepository* aRepository );
    
    void InitDefaultRuleNameL( CRepository* aRepository );
    
    void InitBlockRuleNameL( CRepository* aRepository );    
    
    void InitDefaultRuleActionL( CRepository* aRepository );
    
    void InitBlockRuleActionL( CRepository* aRepository );
    
    void InitRlsGroupUriTemplateL( CRepository* aRepository );
    
    void InitSharedXdmTopL( CRepository* aRepository );  
    
    void InitGrantListNameL( CRepository* aRepository );


private:    // data members

    /**
     * Id for rule to grant all elements
     * Own
     */
    HBufC* iGrantAllRuleName;

    /**
     * Id for rule to grant own data
     * Own
     */
    HBufC* iGrantOwnRuleName;

    /**
     * Id for default rule
     * Own
     */
    HBufC* iDefaultRuleName;

    /**
     * Id for block rule
     * Own
     */
    HBufC* iBlockRuleName;

    /**
     * Default rule action, allow/confirm/block/polite-block
     */
    TSimplePluginAuthRule iDefaultRuleAction;

    /**
     * Block rule action, block/polite-block
     */
    TSimplePluginAuthRule iBlockRuleAction;

    /**
     * Template for RLS service URI for a user group
     * Own
     */
     HBufC* iRlsGroupUriTemplate;

    /**
     * Top level for lists in shared XDM index document
     * Own
     */
    HBufC* iSharedXdmTop;
    
    /**
     * Name of list for granted users in XDM list
     * Own
     */    
    HBufC* iGrantListName;

    };


#endif // CSIMPLEPLUGINVARIATION_H
