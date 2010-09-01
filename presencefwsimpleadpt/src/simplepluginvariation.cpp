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




#include <e32std.h>
#include <centralrepository.h>

#include "simpleplugincommon.h"
#include "simplepluginvariation.h"
#include "presencefwsimpleadptprivatecrkeys.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSimplePluginVariation::CSimplePluginVariation
// ---------------------------------------------------------------------------
//
CSimplePluginVariation::CSimplePluginVariation( )
: iDefaultRuleAction(ERuleConfirm), iBlockRuleAction(ERulePoliteBlock)
    {
    }

// ---------------------------------------------------------------------------
// CSimplePluginVariation::ConstructL
// ---------------------------------------------------------------------------
//
void CSimplePluginVariation::ConstructL( )
    {
    }

// ---------------------------------------------------------------------------
// CSimplePluginVariation::NewL
// ---------------------------------------------------------------------------
//
CSimplePluginVariation* CSimplePluginVariation::NewL( )
    {
    CSimplePluginVariation* self =
        new( ELeave ) CSimplePluginVariation();
    CleanupStack::PushL( self );
    self->ConstructL(  );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSimplePluginVariation::~CSimplePluginVariation
// ---------------------------------------------------------------------------
//
CSimplePluginVariation::~CSimplePluginVariation()
    {
    delete iGrantAllRuleName;
    delete iGrantOwnRuleName;
    delete iDefaultRuleName;
    delete iBlockRuleName;
    delete iRlsGroupUriTemplate;
    delete iSharedXdmTop;
    delete iGrantListName;
    }

// ---------------------------------------------------------------------------
// CSimplePluginVariation::InitL
// ---------------------------------------------------------------------------
//
void CSimplePluginVariation::InitL()
    {
    // Read Central Repository settings
    CRepository* repository = NULL;

    TBuf<KSimplePluginMaxRuleLength> myBuffer;

    TRAP_IGNORE( repository =
        CRepository::NewL( KCRUIDPresencefwSimpleadptVariation ));

    CleanupStack::PushL( repository );

    // initialize strings
    delete iGrantAllRuleName;
    iGrantAllRuleName = NULL;
    delete iGrantOwnRuleName;
    iGrantOwnRuleName = NULL;
    delete iDefaultRuleName;
    iDefaultRuleName = NULL;
    delete iBlockRuleName;
    iBlockRuleName = NULL;
    delete iRlsGroupUriTemplate;
    iRlsGroupUriTemplate = NULL;
    delete iSharedXdmTop;
    iSharedXdmTop = NULL;
    delete iGrantListName;
    iGrantListName = NULL;
    
    // Read values from CenRep
    InitGrantRuleNameL( repository );
    InitGrantOwnRuleNameL( repository );
    InitDefaultRuleNameL( repository );
    InitBlockRuleNameL( repository );
    InitDefaultRuleActionL( repository );
    InitBlockRuleActionL( repository );           
    InitRlsGroupUriTemplateL( repository );
    InitSharedXdmTopL( repository );
    InitGrantListNameL( repository );

    CleanupStack::PopAndDestroy( repository );
    }

// ---------------------------------------------------------------------------
// CSimplePluginVariation::GrantRuleName
// ---------------------------------------------------------------------------
//
TPtrC CSimplePluginVariation::GrantRuleName()
    {
    return iGrantAllRuleName ? iGrantAllRuleName->Des() : TPtrC();
    }

// ---------------------------------------------------------------------------
// CSimplePluginVariation::GrantOwnRuleName
// ---------------------------------------------------------------------------
//
TPtrC CSimplePluginVariation::GrantOwnRuleName()
    {
    return iGrantOwnRuleName ? iGrantOwnRuleName->Des() : TPtrC();
    }

// ---------------------------------------------------------------------------
// CSimplePluginVariation::DefaultRuleName
// ---------------------------------------------------------------------------
//
TPtrC CSimplePluginVariation::DefaultRuleName()
    {
    return iDefaultRuleName ? iDefaultRuleName->Des() : TPtrC();
    }

// ---------------------------------------------------------------------------
// CSimplePluginVariation::BlockRuleName
// ---------------------------------------------------------------------------
//
TPtrC CSimplePluginVariation::BlockRuleName()
    {
    return iBlockRuleName ? iBlockRuleName->Des() : TPtrC();
    }

// ---------------------------------------------------------------------------
// CSimplePluginVariation::DefaultRuleAction
// ---------------------------------------------------------------------------
//
CSimplePluginVariation::TSimplePluginAuthRule CSimplePluginVariation::DefaultRuleAction()
    {
    return iDefaultRuleAction;
    }

// ---------------------------------------------------------------------------
// CSimplePluginVariation::BlockRuleAction
// ---------------------------------------------------------------------------
//
CSimplePluginVariation::TSimplePluginAuthRule CSimplePluginVariation::BlockRuleAction()
    {
    return iBlockRuleAction;
    }

// ---------------------------------------------------------------------------
// CSimplePluginVariation::RlsGroupUriTemplate
// ---------------------------------------------------------------------------
//
TPtrC CSimplePluginVariation::RlsGroupUriTemplate()
    {
    return iRlsGroupUriTemplate ? iRlsGroupUriTemplate->Des() : TPtrC();
    }

// ---------------------------------------------------------------------------
// CSimplePluginVariation::SharedXdmTop
// ---------------------------------------------------------------------------
//
TPtrC CSimplePluginVariation::SharedXdmTop()
    {
    return iSharedXdmTop ? iSharedXdmTop->Des() : TPtrC();
    }

// ---------------------------------------------------------------------------
// CSimplePluginVariation::GrantListName
// ---------------------------------------------------------------------------
//
TPtrC CSimplePluginVariation::GrantListName()
    {
    return iGrantListName ? iGrantListName->Des() : TPtrC();
    }
    
// ---------------------------------------------------------------------------
// CSimplePluginVariation::InitGrantRuleNameL
// ---------------------------------------------------------------------------
//    
void CSimplePluginVariation::InitGrantRuleNameL( CRepository* aRepository )
    {    
    TBuf<KSimplePluginMaxRuleLength> myBuffer;    
    
    if ( !aRepository || aRepository->Get( KPrFwSimpleGrantAllRuleName, myBuffer ))
        {
        // Use deault value when not found in Central Repository
        myBuffer = KSimpleXdmGrantRule;
        }
    iGrantAllRuleName = myBuffer.AllocL( );
    }
    
// ---------------------------------------------------------------------------
// CSimplePluginVariation::InitGrantOwnRuleNameL
// ---------------------------------------------------------------------------
//    
void CSimplePluginVariation::InitGrantOwnRuleNameL( CRepository* aRepository )
    {    
    TBuf<KSimplePluginMaxRuleLength> myBuffer;    
    
    if ( !aRepository || aRepository->Get( KPrFwSimpleGrantOwnRuleName, myBuffer ))
        {
        // Use deault value when not found in Central Repository
        myBuffer = KSimpleXdmOwnGrantRule;
        }
    iGrantOwnRuleName = myBuffer.AllocL( );
    } 
    
// ---------------------------------------------------------------------------
// CSimplePluginVariation::InitDefaultRuleNameL
// ---------------------------------------------------------------------------
//    
void CSimplePluginVariation::InitDefaultRuleNameL( CRepository* aRepository )
    {    
    TBuf<KSimplePluginMaxRuleLength> myBuffer;    
    
    if ( !aRepository || aRepository->Get( KPrFwSimpleDefaultRuleName, myBuffer ))
        {
        // Use deault value when not found in Central Repository
        myBuffer = KSimpleXdmDefaultRule;
        }
    iDefaultRuleName = myBuffer.AllocL( );
    }
    
// ---------------------------------------------------------------------------
// CSimplePluginVariation::InitBlockRuleNameL
// ---------------------------------------------------------------------------
//    
void CSimplePluginVariation::InitBlockRuleNameL( CRepository* aRepository )
    {    
    TBuf<KSimplePluginMaxRuleLength> myBuffer;    
    
    if ( !aRepository || aRepository->Get( KPrFwSimpleBlockRuleName, myBuffer ))
        {
        // Use deault value when not found in Central Repository
        myBuffer = KSimpleXdmBlockRule;
        }
    iBlockRuleName = myBuffer.AllocL( );
    }    
    
// ---------------------------------------------------------------------------
// CSimplePluginVariation::InitDefaultRuleActionL
// ---------------------------------------------------------------------------
//    
void CSimplePluginVariation::InitDefaultRuleActionL( CRepository* aRepository )
    {    
    TBuf<KSimplePluginMaxRuleLength> myBuffer;  
    
    using namespace NSimplePlugin::NSimpleOma;      
    
    iDefaultRuleAction = ERuleAllow;
    if ( aRepository && !aRepository->Get( KPrFwSimpleDefaultRuleAction, myBuffer ))
        {
        if ( !myBuffer.CompareF( KSimpleBlock ))
            {
            iDefaultRuleAction = ERuleBlock;
            }
        else if ( !myBuffer.CompareF( KSimplePoliteBlock ))
            {
            iDefaultRuleAction = ERulePoliteBlock;
            }
        else if ( !myBuffer.CompareF( KSimpleConfirm ))
            {
            iDefaultRuleAction = ERuleConfirm;
            }
        else if ( !myBuffer.CompareF( KSimpleAllow ))
            {
            iDefaultRuleAction = ERuleAllow;
            }
        }
    }
        
// ---------------------------------------------------------------------------
// CSimplePluginVariation::InitBlockRuleActionL
// ---------------------------------------------------------------------------
//    
void CSimplePluginVariation::InitBlockRuleActionL( CRepository* aRepository )
    {    
    TBuf<KSimplePluginMaxRuleLength> myBuffer;  
    
    using namespace NSimplePlugin::NSimpleOma;      
    
    iBlockRuleAction = ERuleBlock;
    if ( aRepository && !aRepository->Get( KPrFwSimpleBlockRuleAction, myBuffer ))
        {
        if ( myBuffer.CompareF( KSimpleBlock ))
            {
            iBlockRuleAction = ERulePoliteBlock;
            }
        else 
            {
            // default value is used            
            }
        }
    else
        {
        // default value is used
        }    
    } 
    
// ---------------------------------------------------------------------------
// CSimplePluginVariation::InitRlsGroupUriTemplateL
// ---------------------------------------------------------------------------
//    
void CSimplePluginVariation::InitRlsGroupUriTemplateL( CRepository* aRepository )
    {    
    TBuf<KSimplePluginMaxRuleLength> myBuffer;  
            
    if ( !aRepository || aRepository->Get( KPrFwSimpleRlsGroupUriTemplate, myBuffer ))
        {
        myBuffer = KSimpleRlsGroupUriTemplate;
        }
    iRlsGroupUriTemplate = myBuffer.AllocL();    
    }
    
// ---------------------------------------------------------------------------
// CSimplePluginVariation::InitSharedXdmTopL
// ---------------------------------------------------------------------------
//    
void CSimplePluginVariation::InitSharedXdmTopL( CRepository* aRepository )
    {
    TBuf<KSimplePluginMaxRuleLength> myBuffer;

    if ( !aRepository || aRepository->Get( KPrFwSimpleSharedXdmTop, myBuffer ))
        {
        myBuffer = KSimpleOMABuddyList;
        }
    iSharedXdmTop = myBuffer.AllocL();
    }
    
// ---------------------------------------------------------------------------
// CSimplePluginVariation::InitGrantListNameL
// ---------------------------------------------------------------------------
//    
void CSimplePluginVariation::InitGrantListNameL( CRepository* aRepository )
    {    
    TBuf<KSimplePluginMaxRuleLength> myBuffer;  
            
    if ( !aRepository || aRepository->Get( KPrFwSimpleGrantListName, myBuffer ))
        {
        myBuffer = KSimpleS60GrantedList;
        }
    iGrantListName = myBuffer.AllocL();   
    }               

// End of file
