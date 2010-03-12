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


#include <e32std.h>
#include <utf.h>
#include <ximpdatasubscriptionstate.h>	  
#include <protocolpresencepublishingdatahost.h>
#include <protocolpresencedatahost.h>
#include <ximpobjectfactory.h>
#include <presenceobjectfactory.h>
#include <presenceinfo.h>
#include <personpresenceinfo.h>
#include <ximpprotocolconnectionhost.h>
#include <ximpstatus.h>
#include <ximperrors.hrh>
#include <ximpidentity.h>
#include <ximpobjectcollection.h>
#include <presencewatcherinfo.h>
#include <simpleutils.h>   // For KSimplePDM
#include <simplefactory.h>
#include <msimpledocument.h>
#include <msimplepublisher.h>
#include <msimplewatcher.h>
#include <msimplewinfo.h>
#include <msimpleelement.h>
#include <msimpleconnection.h>
#include <simpleerrors.h>
#include <avabilitytext.h>

#include "presenceplugincommon.h"
#include "presencepluginpublisher.h"
#include "presencepluginwinfo.h"
#include "presenceplugindata.h"
#include "presencepluginxdmutils.h"
#include "presencepluginwatcherinfo.h"
#include "presenceplugincommon.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::CPresencePluginPublisher()
// ---------------------------------------------------------------------------
//
CPresencePluginPublisher::CPresencePluginPublisher(
    MPresencePluginConnectionObs& aObs,
    MSimpleConnection& aConn )
    : CActive( CActive::EPriorityStandard ),
    iConnObs(aObs), iConnection(aConn),
    iPublisher( NULL ), iWatcher( NULL ),
    iSubscribed(EFalse), iSubscribedOwn(EFalse), 
    iPublished(EFalse), iRePublish(EFalse),
    iWatcherList( CPresencePluginWatcherInfo::LinkOffset() ) 
    {
    DP_SDA("CPresencePluginPublisher::CPresencePluginPublisher ");
    CActiveScheduler::Add(this);        
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::ConstructL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::ConstructL( CPresencePluginData* aPresenceData )
    {
    DP_SDA("CPresencePluginPublisher::ConstructL ");
    iWatcher = TSimpleFactory::NewWatcherL( iConnection, *this );
    iPublisher = TSimpleFactory::NewPublisherL( iConnection, *this );       
    iPresenceData = aPresenceData;
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::NewL()
// ---------------------------------------------------------------------------
//
CPresencePluginPublisher* CPresencePluginPublisher::NewL(
    MPresencePluginConnectionObs& aObs,
    MSimpleConnection& aConn,
    CPresencePluginData* aPresenceData )
    {
    CPresencePluginPublisher* self =
        new( ELeave ) CPresencePluginPublisher( aObs, aConn );
    CleanupStack::PushL( self );
    self->ConstructL( aPresenceData );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::~CPresencePluginPublisher()
// ---------------------------------------------------------------------------
//
CPresencePluginPublisher::~CPresencePluginPublisher()
    {
    DP_SDA("CPresencePluginPublisher::~CPresencePluginPublisher");
    
    if ( iDocument )
        {
        iDocument->Close();
        }

    if ( iPublisher )
        {
        iPublisher->Close();
        }
        
    if ( iWatcher )
        {
        iWatcher->Close();
        }
         
    DeleteWatchers();
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::DeleteWatchers
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::DeleteWatchers()
    {
    DP_SDA("CPresencePluginPublisher::DeleteWatchers");
    // Delete all buffered transaction requests
    TDblQueIter<CPresencePluginWatcherInfo> rIter( iWatcherList );
    rIter.SetToFirst();

    while ( rIter )
        {
        DP_SDA("CPresencePluginPublisher::DeleteWatchers while"); 
        CPresencePluginWatcherInfo* w = rIter;
        rIter++;
        // delete wathcer info
        w->Destroy();
        } 
    DP_SDA("CPresencePluginPublisher::DeleteWatchers end"); 
    } 

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::AddWatcherIfNotExistsL
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::AddWatcherIfNotExistsL( 
    const TDesC8& aId,
    const TDesC& aSipId )
    {
    DP_SDA("CPresencePluginPublisher::AddWatcherIfNotExistsL");
    // Delete all buffered transaction requests
    TDblQueIter<CPresencePluginWatcherInfo> rIter( iWatcherList );
    rIter.SetToFirst();
    
    TBool found(EFalse);

    while ( rIter )
        {
        CPresencePluginWatcherInfo* w = rIter;
        rIter++;
        found = w->Match( aId, aSipId );
        if ( found )
            {
            DP_SDA("CPresencePluginPublisher::AddWatcherIfNotExistsL found");
            break;
            }
        }
    if ( !found )
        {
        DP_SDA("CPresencePluginPublisher::AddWatcherIfNotExistsL !found");       
        CPresencePluginWatcherInfo* w =
            CPresencePluginWatcherInfo::NewL( aId, aSipId );
        iWatcherList.AddLast( *w );
        } 
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::RemoveWatcherIfExistsL
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::RemoveWatcherIfExists( 
    const TDesC8& aId, const TDesC& aSipId )
    {
    DP_SDA("CPresencePluginPublisher::RemoveWatcherIfExistsL");
    // Delete all buffered transaction requests
    TDblQueIter<CPresencePluginWatcherInfo> rIter( iWatcherList );
    rIter.SetToFirst();
    
    TBool found(EFalse);

    while ( rIter )
        {
        CPresencePluginWatcherInfo* w = rIter;
        rIter++;
        // delete wathcer info
        found = w->Match( aId, aSipId );
        if ( found )
            {         
            w->Destroy();
            break;
            }
        }    
    }  
    
// ---------------------------------------------------------------------------
// CPresencePluginPublisher::DoPublishOwnPresenceL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::DoPublishOwnPresenceL(
    const MPresenceInfo& aOwnPresence,
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginPublisher::DoPublishOwnPresenceL");
    const MPersonPresenceInfo* persInfo = aOwnPresence.PersonPresence();
    if ( !persInfo )
        {
        // Notice: error codes
        CompleteXIMPReq( KErrArgument );
        return;
        }
    InitializeSimpleDocumentL( );    
    AddSimpleDocumentPersL( persInfo ); 
    
    StartXdmOperationL( aReqId );
    iOperation = EPublishOwn;                 
    DP_SDA("CPresencePluginPublisher::DoPublishOwnPresenceL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::DoSubscribeOwnPresenceL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::DoSubscribeOwnPresenceL(
    const MPresenceInfoFilter& /*aPif*/,   // notice: later
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginPublisher::DoSubscribeOwnPresenceL");         
    StartXdmOperationL( aReqId );    
    iOperation = ESubscribeOwn; 
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::DoUpdateOwnPresenceSubscriptionPifL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::DoUpdateOwnPresenceSubscriptionPifL(
    const MPresenceInfoFilter& /*aPif*/,
    TXIMPRequestId /*aReqId*/ )
    {
    // Notice: later
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::DoUnsubscribeOwnPresenceL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::DoUnsubscribeOwnPresenceL(
    TXIMPRequestId aReqId )
    {     
    DP_SDA("CPresencePluginPublisher::DoUnsubscribeOwnPresenceL");
    iSubscribedOwn = EFalse;
    iSimpleId = iWatcher->UnsubscribeL();
    iXIMPId = aReqId;
    iOperation = EUnsubscribeOwn;    
    }    

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::DoSubscribePresenceWatcherListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::DoSubscribePresenceWatcherListL(
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginPublisher::DoSubscribePresenceWatcherListL");
    if ( !iSubscribed )
        {    
        DP_SDA("DoSubscribePresenceWatcherListL !Subscribed");    
        iConnObs.WinfoHandlerL()->SubscribeWinfoListL( aReqId );
        iSubscribed = ETrue;
        iXIMPId = aReqId;
        iOperation = ESubscribeWinfo;
        }
    else
        {
        DP_SDA("DoSubscribePresenceWatcherListL else");
        iXIMPId = aReqId;         
        iOperation = ESubscribeWinfo;        
        CompleteXIMPReq( KErrNone );
        } 
    DP_SDA("CPresencePluginPublisher::DoSubscribePresenceWatcherListL end"); 
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::DoUnsubscribePresenceWatcherListL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::DoUnsubscribePresenceWatcherListL(
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginPublisher::DoUnsubscribePresenceWatcherListL");
    if ( iSubscribed )
        {
        DP_SDA("DoUnsubscribePresenceWatcherListL: if ( iSubscribed ) - begin");
        iConnObs.WinfoHandlerL()->UnsubscribeWinfoListL( aReqId );
        iSubscribed = EFalse;
        iXIMPId = aReqId;
        iOperation = EUnsubscribeWinfo;        
        DP_SDA("DoUnsubscribePresenceWatcherListL:  if ( iSubscribed ) - end");
        
        }
    else
        {
        DP_SDA("DoUnsubscribePresenceWatcherListL: iSubscribed ) is false");        
        iXIMPId = aReqId;
        iOperation = EUnsubscribeWinfo; //hameed
        CompleteXIMPReq( KErrNone );
        }
    }
  
      
// ---------------------------------------------------------------------------
// CPresencePluginPublisher::PublishReqCompleteL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::PublishReqCompleteL( 
    TInt /*aOpid*/,
    TInt aStatus )
    {
    DP_SDA("CPresencePluginPublisher::PublishReqCompleteL");
    // Notice: make 415 error code conversion here and other 
    //SIP code -> XIMP conversions
    DP_SDA2("PublishReqCompleteL STATUS %d",aStatus );
    
    if( iConnObs.GetSessionStatus() )
        {
        DP_SDA2("PublishReqComp stopState %d",iConnObs.GetStopPublishState());
        if ( !aStatus && !iConnObs.GetStopPublishState() )
            {
            DP_SDA("CPresencePluginPublisher::PublishReqCompleteL ETrue");
            iPublished = ETrue;
            iConnObs.SetStopPublishState( ETrue );
            }
        // Save E-Tag
        if( !aStatus )
            {
            DP_SDA("CPresencePluginPublisher::PublishReqCompleteL Save ETAG");
            TBuf8<KBufSize255> buf;
            buf.Copy( iPublisher->SIPETag() );
            iConnObs.SetETag( buf );
            }
    	//Do not complete if error republish is true. XIMPFW is not knowing we 
    	//are sending republish.
        if( EFalse == iRePublish && !aStatus )
            {
            //Complete Publish request
            DP_SDA("CPresencePluginPublisher::PublishReqCompleteL complete");
            if( !(iConnObs.IsStopPublishCalled()) )
                {
                DP_SDA("CPresencePluginPublisher::PublishReqCompleteL complete normal");
                // successfull Online / others: store status to cache
                StoreOwnStatusToCacheL( );
                CompleteXIMPReq( aStatus );
                }
            //Complete stopPublish and close session if stopPublish is called
            if( iConnObs.IsStopPublishCalled() )
                {
                DP_SDA("PublishReqCompleteL complete closeSession"); 
                iPresenceData->DeletePresenceVariablesL( iConnObs.ServiceId() );
                // successfull Online / others: store status to cache
                StoreOwnStatusToCacheL( );
                //Complete close session
                CompleteClientReq( aStatus );
                }
            }
        if( EFalse == iRePublish && ( KSimpleErrTemporary == aStatus  ) )
           {
           DP_SDA("PublishReqCompleteL KSimpleErrTemporary"); 
           // Make new publish request without ETag
           CreatePublisherL();        
           iSimpleId = iPublisher->StartPublishL( *iDocument, ETrue );
           }
        else if( EFalse == iRePublish && ( KErrTimedOut == aStatus  ) )
            {
            DP_SDA("PublishReqCompleteL KErrTimedOut"); 
            iPublisher->StopPublishL();
            CompleteXIMPReq( KXIMPErrServicRequestTimeouted );
            }
        else if( KErrCommsBreak == aStatus )
            {
            DP_SDA("PublishReqCompleteL KErrCommsBreak");
            CompleteXIMPReq( KErrCommsBreak );
            }
        else
        	{
        	//Set rePublish flag back to false.
        	iRePublish = EFalse;
        	}
        }
    else
        {
        //Run this case if connection is not good any more
        DP_SDA("PublishReqCompleteL No connect CANCEL");
        Cancel();
        }
    DP_SDA("CPresencePluginPublisher::PublishReqCompleteL end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::PublishTerminatedL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::PublishTerminatedL( TInt aOpid )
    {
    DP_SDA("CPresencePluginPublisher::PublishTerminatedL");
    DP_SDA2("CPresencePluginPublisher::PublishTerminatedL opID %d",aOpid );
    DP_SDA2("PublishTerminatedL iSimpleId %d",iSimpleId );
    DP_SDA2("PublishTerminatedL iStatus %d",iStatus.Int() );
    
    if ( iSimpleId == aOpid && iConnObs.GetSessionStatus() )
        {
        DP_SDA("CPresencePluginPublisher::PublishTerminatedL NewPublish");
        CreatePublisherL();       
        TRAPD( error , iSimpleId = iPublisher->StartPublishL( 
        		*iDocument, ETrue ) );
        if ( KErrNone == error )
            {
            DP_SDA("PublishTerminatedL NewPublish KErrNone");
            iRePublish = ETrue;
            iConnObs.SetStopPublishState( EFalse );
            }        
        }
    else
        {
        DP_SDA("CPresencePluginPublisher::PublishTerminatedL else");
        iPublished = EFalse;
        if( iPublisher )
            {
            iPublisher->Close();
            iPublisher = NULL; 
            }    
        }  
    }   
    
// ---------------------------------------------------------------------------
// CPresencePluginPublisher::WatcherReqCompleteL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::WatcherReqCompleteL( TInt /*aOpid*/,
    TInt aStatus )
    {
    DP_SDA("CPresencePluginPublisher::WatcherReqCompleteL");
    if ( !aStatus )
        {      
        iSubscribedOwn = ETrue;   
        }
    
    CompleteXIMPReq( aStatus );       
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::WatcherNotificationL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::WatcherNotificationL( 
    MSimpleDocument& aDocument )
    {       
    // Notice: Do not need to check the expiration here since
    // WatcherTerminatedL is called then too.
    DP_SDA("CPresencePluginPublisher::WatcherNotificationL");
    
    MProtocolPresencePublishingDataHost& publishHost =
        iConnObs.ProtocolPresenceHost().PublishingDataHost();
            
    MPresenceInfo* prInfo =
        iConnObs.PresenceObjectFactoryOwn().NewPresenceInfoLC();// << prInfo
        
    iPresenceData->NotifyToPrInfoL(  
        iConnObs.ObjectFactory(),
        iConnObs.PresenceObjectFactoryOwn(), 
        aDocument,
        *prInfo );

    // XIMP Host API callbacks
  
    publishHost.HandleSubscribedOwnPresenceL( prInfo );
    CleanupStack::Pop();  // >> prInfo            
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::WatcherListNotificationL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::WatcherListNotificationL( 
    MSimplePresenceList& /*aList*/ )
    {
    //Do nothing now
    DP_SDA("CPresencePluginPublisher::WatcherListNotificationL EMPTY");     
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::WatcherTerminatedL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::WatcherTerminatedL(
    TInt /*aOpId*/, TInt aReason )
    {
    DP_SDA("CPresencePluginPublisher::WatcherTerminatedL");
    
    iSubscribedOwn = EFalse;    

    MXIMPObjectFactory& myFactory = iConnObs.ObjectFactory();    
    MProtocolPresencePublishingDataHost& publishHost =
        iConnObs.ProtocolPresenceHost().PublishingDataHost();
        
    MXIMPDataSubscriptionState *state =
        myFactory.NewDataSubscriptionStateLC();
    MXIMPStatus* status = myFactory.NewStatusLC();
    state->SetSubscriptionStateL( 
        MXIMPDataSubscriptionState::ESubscriptionInactive );
        
    status->SetResultCode( aReason );
    publishHost.SetOwnPresenceDataSubscriptionStateL( state, status );
    CleanupStack::Pop( 2 ); // status, state    
    
    DP_SDA("CPresencePluginPublisher::WatcherTerminatedL - end");
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginPublisher::CompleteXIMPReq()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::CompleteXIMPReq( TInt aStatus )
    {
    DP_SDA("CPresencePluginPublisher::CompleteXIMPReq");
    DP_SDA2("CompleteXIMPReq:  iOperation = %d",(TInt) iOperation );
    // Notice: should this yield active scheduler???
    if ( iOperation != ENoOperation )
        {
        DP_SDA("CPresencePluginPublisher::CompleteXIMPReq complete");                
        iOperation = ENoOperation;
        iConnObs.CompleteReq( iXIMPId, aStatus );
        iXIMPId = TXIMPRequestId();
        }
    DP_SDA("CPresencePluginPublisher::CompleteXIMPReq end"); 
    }  

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::CompleteClientReq()
// ---------------------------------------------------------------------------
//               
void CPresencePluginPublisher::CompleteClientReq( TInt aStatus )
    {
    DP_SDA2("CPresencePluginPublisher::CompleteClientReq status %d ", aStatus);
    iOperation = ENoOperation;
    TRequestStatus* s = iClientStatus;
    User::RequestComplete( s, aStatus );
    DP_SDA("CPresencePluginPublisher::CompleteClientReq end");
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::DoCancel()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::DoCancel(  )
    {
    DP_SDA("CPresencePluginPublisher::DoCancel CANCEL");
    iXdmUtils->Cancel();
    iPublisher->Close();
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::RunL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::RunL(  )
    {
    DP_SDA("CPresencePluginPublisher::RunL");
    TInt status = iStatus.Int();
    DP_SDA2("CPresencePluginPublisher::RunL status %d", status );
    if ( !status )
        {
        
        if ( iOperation == ESubscribeOwn )
            {
            DP_SDA("CPresencePluginPublisher::RunL Subscribe Own");
            iSimpleId = iWatcher->SubscribeL(
                iConnObs.CurrentSipPresentity8(),
                NULL,  // aFilter <-> aPif
                ETrue, EFalse );          
            }
        else if( iOperation == EPublishOwn )
            {
            DP_SDA("CPresencePluginPublisher::RunL MakePublishReqL");
            MakePublishReqL();
            }
        }
    else
        {
        DP_SDA("CPresencePluginPublisher::RunL complete");
        CompleteXIMPReq( status );
        }           
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::RunError
// ---------------------------------------------------------------------------
//
TInt CPresencePluginPublisher::RunError( TInt aError )
    {
    DP_SDA2("CPresencePluginPublisher::RunError %d",aError );
    
    CompleteXIMPReq( aError );  
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginPublisher::GetInterface()
// ---------------------------------------------------------------------------
//
TAny* CPresencePluginPublisher::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions )
    {
    DP_SDA("CPresencePluginPublisher::GetInterface");
    
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        MProtocolPresencePublishing* myIf = this;
        return myIf;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( KPluginName, KErrExtensionNotSupported );
        }  
    return NULL;
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::GetInterface()
// ---------------------------------------------------------------------------
//
const TAny* CPresencePluginPublisher::GetInterface(
    TInt32 aInterfaceId,
    TIfGetOps aOptions ) const
    {
    DP_SDA("CPresencePluginPublisher::GetInterface 2 ");
    
    if ( aInterfaceId == GetInterfaceId() )
        {
        // caller wants this interface
        const MProtocolPresencePublishing* myIf = this;
        return myIf;
        }
    if ( aOptions == MXIMPBase::EPanicIfUnknown )
        {
        User::Panic( KPluginName, KErrExtensionNotSupported );
        }  
    return NULL;
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::GetInterfaceId()
// ---------------------------------------------------------------------------
//
TInt32 CPresencePluginPublisher::GetInterfaceId() const
    {
    DP_SDA("CPresencePluginPublisher::GetInterfaceId");
    
    return MProtocolPresencePublishing::KInterfaceId;
    }
       
// ---------------------------------------------------------------------------
// CPresencePluginPublisher::InitializeSimpleDocumentL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::InitializeSimpleDocumentL( )
    {
    _LIT8 ( KCipidNS, "urn:ietf:params:xml:ns:pidf:cipid" );
    _LIT8 ( KCipidCP, "c" );
    if ( iDocument )
        {
        DP_SDA("CPresencePluginPublisher:: deletedocument");
        iDocument->Close();
        iDocument = NULL;
        iDocument = TSimpleFactory::NewDocumentL();
        }
    else
        {
        DP_SDA("CPresencePluginPublisher:: create document");
        iDocument = TSimpleFactory::NewDocumentL();
        }
    iDocument->AddNamespaceL( KSimplePDM, KSimpleNsPDM );
    iDocument->AddNamespaceL( KSimpleRPID, KSimpleNsRPID );
    iDocument->AddNamespaceL( KCipidCP, KCipidNS );
    }      
    
// ---------------------------------------------------------------------------
// CPresencePluginPublisher::AddSimpleDocumentPersL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::AddSimpleDocumentPersL(
    const MPersonPresenceInfo* aInfo )
    {
    TInt tupleId = iConnObs.GetTupleId();
    
    DP_SDA2("AddSimpleDocumentPersL tuple id get: %d", tupleId);
    if ( tupleId == 0 )
        {
        DP_SDA("AddSimpleDocumentPersL tuple generate");
        tupleId = iPresenceData->GenerateTupleId();
        iConnObs.SetTupleId(tupleId);
        }
   
    iPresenceData->AddPrPersToSimpleDocumentL(
        aInfo, *iDocument, iConnObs.CurrentSipPresentity8(), tupleId ); 
    }
          
// ---------------------------------------------------------------------------
// CPresencePluginPublisher::StopPublishL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::StopPublishL( TRequestStatus& aStatus )
    {
    DP_SDA2("StopPublish - begin : iPublished= %d ",(TInt) iPublished);
    
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    
    TInt err1( 0 );
    TInt err2( 0 );
    
    // get stored document id if available
    HBufC8* oldDocumentId = HBufC8::NewLC( KBufSize255 );
    TPtr8 oldDocumentIdPtr( oldDocumentId->Des() );
    TRAP( err1, iPresenceData->ReadDocumentIdL( 
            iConnObs.ServiceId(), oldDocumentIdPtr ) );
    
    if ( iConnObs.GetStopPublishState() && iPublished )
        {
        DP_SDA("CPresencePluginPublisher::StopPublish really Stop");
        if ( !err1 )
            {
            TRAP( err2, iPublisher->StopPublishL( oldDocumentIdPtr ) );
            }
        else 
            {
            // if stored document id is not available try with this
            TRAP( err2, iPublisher->StopPublishL( iConnObs.GetETag() ) );
            }
        DP_SDA2("StopPublish iPublisher->StopPublishL : error = %d ", err2 );
        if ( KErrNone != err2 )
            {            
            if( iPublisher )
                {
                DP_SDA("StopPublish delete and recreate publisher");
                iPublisher->Close();
                iPublisher = NULL;
                DP_SDA("StopPublish really Stop try again 1 ");
                iPublisher =
                	TSimpleFactory::NewPublisherL( iConnection, *this ); 
                DP_SDA("StopPublish really Stop try again 2 ");
                err2 = KErrGeneral;
                TRAP( err2, iPublisher->StopPublishL( iConnObs.GetETag() ););
                DP_SDA2("StopPublishL 2nd try : error = %d ", err2 );
                if ( KErrNone != err2 )
                    {
                    DP_SDA("StopPublish TWO TIME Error, last try without tag ");
                    TRAP( err2, iPublisher->StopPublishL(); );
                    DP_SDA2("StopPublishL 3rd try : error = %d ", err2 );
                    }
                }
            }
        iConnObs.SetStopPublishState( ETrue );
        iPublished = EFalse;
        }
    CleanupStack::PopAndDestroy( oldDocumentId );
    
    DP_SDA("CPresencePluginPublisher::StopPublish- end");        
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginPublisher::Published()
// ---------------------------------------------------------------------------
//
TBool CPresencePluginPublisher::Published()
    {
    return iPublished;
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::MakePublishReqL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::MakePublishReqL( )
    {
    DP_SDA("CPresencePluginPublisher::MakePublishReqL");
    
        DP_SDA(" -> MakePublishReqL, check for old doc id");       
        HBufC8* oldDocumentId = HBufC8::NewLC( KBufSize255 );
        TPtr8 oldDocumentIdPtr( oldDocumentId->Des() );
        TRAPD( error, iPresenceData->ReadDocumentIdL( 
            iConnObs.ServiceId(), oldDocumentIdPtr ) );
    if ( !iPublished )
        {
        DP_SDA("CPresencePluginPublisher::MakePublishReqL 1");       
        CreatePublisherL();
        
        DP_SDA2(" -> MakePublishReqL, doc id error: %d", error );
        if ( !error )
            {
            DP_SDA(" -> MakePublishReqL, publish with old id");       
            iSimpleId = iPublisher->ContinuePublishL( 
                *iDocument, ETrue, oldDocumentIdPtr );
            }
        else
            {
            DP_SDA(" -> MakePublishReqL, publish with new id");       
            iSimpleId = iPublisher->StartPublishL( *iDocument, ETrue );    
            }
        }
    else
        {
        DP_SDA("CPresencePluginPublisher::MakePublishReqL 2");
        CreatePublisherL();
        DP_SDA("CPresencePluginPublisher::MakePublishReqL Continue Publish");
        if ( !error )
            {
            DP_SDA(" -> MakePublishReqL, publish with stored id");
            iSimpleId = iPublisher->ContinuePublishL( 
                *iDocument ,ETrue, oldDocumentIdPtr );
            }
        else
            {
            DP_SDA(" -> MakePublishReqL, stored id not found" );
            DP_SDA(", publish with old id");
            iSimpleId = iPublisher->ContinuePublishL( 
                            *iDocument ,ETrue, iConnObs.GetETag() );
            }
		DP_SDA("CPresencePluginPublisher::MakePublishReqL modifyed");
        }
    CleanupStack::PopAndDestroy( oldDocumentId );
    DP_SDA("CPresencePluginPublisher::MakePublishReqL end");     
    } 

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::StartXdmOperationL
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::StartXdmOperationL(
    TXIMPRequestId aReqId )
    {
    DP_SDA("CPresencePluginPublisher::StartXdmOperationL");
    __ASSERT_DEBUG( !IsActive(), User::Leave( KErrNotReady ) );
    iXIMPId = aReqId;
    if ( !iXdmUtils )
        {
        DP_SDA("StartXdmOperationL Initialize XDMUtils");
        iXdmUtils = iConnObs.XdmUtilsL();
        }

    // Allways check those XDM files are ok / 
    //publish seems to be jam if xdm not initialized ? 
    iXdmUtils->InitializeXdmL( iStatus ); 
    SetActive();
    DP_SDA("CPresencePluginPublisher::StartXdmOperationL end");     
    }
                 
// ---------------------------------------------------------------------------
// CPresencePluginPublisher::WinfoNotificationL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::WinfoNotificationL(
    MSimpleWinfo& aWinfo )
    {
    DP_SDA("CPresencePluginPublisher::WinfoNotificationL");
   
    RPointerArray<MSimpleElement> elems;
    CleanupClosePushL( elems );         // << elems
    TInt err = aWinfo.SimpleElementsL( elems );
    User::LeaveIfError( err );
    TInt count = elems.Count();
    
    DP_SDA2("WinfoNotificationL elems count%d", count);

    __ASSERT_ALWAYS( count == 1, User::Leave( KErrCorrupt ) );

    using namespace NPresencePlugin::NPresence;

    const TDesC8* stateVal = aWinfo.AttrValue( KPresenceState8 );
    
    if ( stateVal && !stateVal->CompareF( KPresenceFull8 )) //state full
        {
        // full winfo-list is received  (New empty list )
        DP_SDA("CPresencePluginPublisher::WinfoNotificationL full list found");
        DeleteWatchers();
        }
	else
        {
		DP_SDA("PluginPublisher: state != full");
		// Test if state is partitial
		if ( stateVal && !stateVal->CompareF( KPresencePartial8 ))
			{
			DP_SDA("PluginPublisher: state = partitial");	
			}
		}

    MSimpleElement* elem = elems[0];
    TPtrC8 p8 = elem->LocalName();
    err = p8.CompareF( KPresenceWatcherList8 );
    DP_SDA2("WinfoNotificationL KPresenceWatcherList8 err %d", err);
    User::LeaveIfError( err );
    DP_SDA("WinfoNotificationL KPresenceWatcherList8 continue");
    
    //Get elemets from document
    err = elem->SimpleElementsL( elems );
    User::LeaveIfError( err );
    DP_SDA2("WinfoNotificationL get elems err %d", err);
    //Make collect from active watchers
    CollectWatchersL( elems );
    
    CDesCArrayFlat* watchers = MakeCurrentWatcherListLC();// << watchers
    MXIMPObjectCollection *actives =
        iConnObs.ObjectFactory().NewObjectCollectionLC();// << actives       

    // Create MXIMPPresenceWatcherInfo entities for
    // all active watchers and add to actives.
    TInt wCount = watchers->MdcaCount();
    DP_SDA2("CPresencePluginPublisher::WinfoNotificationL wCount %d", wCount);
    for ( TInt j = 0; j < wCount; j++ )
        {       
        // create MXIMPPresenceWatcherInfo object
        MPresenceWatcherInfo* wInfo =
            iConnObs.PresenceObjectFactoryOwn().
                NewPresenceWatcherInfoLC();// << wInfo

        MXIMPIdentity* identity =
        	iConnObs.ObjectFactory().NewIdentityLC();// << identity
         
        identity->SetIdentityL( watchers->MdcaPoint( j ) );                
        wInfo->SetWatcherIdL( identity );
        CleanupStack::Pop( );// >> identity

        wInfo->SetWatcherDisplayNameL( watchers->MdcaPoint( j ) );
        wInfo->SetWatcherTypeL( MPresenceWatcherInfo::EPresenceSubscriber );

        actives->AddObjectL( wInfo );
        CleanupStack::Pop( );                           // >> wInfo
        }

    MProtocolPresencePublishingDataHost& publishHost =
         iConnObs.Host()->ProtocolPresenceDataHost().PublishingDataHost();

    publishHost.HandlePresenceWatcherListL( actives );
   
    CleanupStack::Pop();                  // >> actives      
    CleanupStack::PopAndDestroy( watchers );        // >> watchers
   
    CleanupStack::PopAndDestroy( &elems );          // >> elems
    DP_SDA("CPresencePluginPublisher::WinfoNotificationL end");      
    }
        
// ---------------------------------------------------------------------------
// CPresencePluginPublisher::WinfoTerminatedL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::WinfoTerminatedL( TInt aReason )
    {
    DP_SDA("CPresencePluginPublisher::WinfoTerminatedL - begin");      
    
    // call SetPresenceWatcherListDataSubscriptionStateL  
    iSubscribed = EFalse;    

    MXIMPObjectFactory& myFactory = iConnObs.ObjectFactory();    
    MProtocolPresencePublishingDataHost& publishHost =
        iConnObs.Host()->ProtocolPresenceDataHost().PublishingDataHost();
    MXIMPDataSubscriptionState *state =
        myFactory.NewDataSubscriptionStateLC();
        
    MXIMPStatus* status = myFactory.NewStatusLC();
    state->SetSubscriptionStateL( 
        MXIMPDataSubscriptionState::ESubscriptionInactive );
        
    state->SetDataStateL( MXIMPDataSubscriptionState::EDataUnavailable );
    status->SetResultCode( aReason );
    publishHost.SetPresenceWatcherListDataSubscriptionStateL( state, status );
    CleanupStack::Pop( 2 ); // status, state  
    DP_SDA("CPresencePluginPublisher::WinfoTerminatedL - end");
    }  

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::NewETagL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::NewETagL( const TDesC8& aVal )
    {
    DP_SDA("CPresencePluginPublisher::NewETagL");        
    // Set ETag value 
    DP_SDA("CPresencePluginPublisher::NewETagL Set ETag");
    TBuf8<KBufSize255> buf;
    buf.Copy( aVal );
    iConnObs.SetETag( buf );
    buf.Zero(); 
    }
    
// ---------------------------------------------------------------------------
// CPresencePluginPublisher::MakeCurrentWatcherListLC
// ---------------------------------------------------------------------------
//
CDesCArrayFlat* CPresencePluginPublisher::MakeCurrentWatcherListLC()
    {
    DP_SDA("CPresencePluginPublisher::MakeCurrentWatcherListLC - begin");        
    // No one should be added more than once
    const TInt KMyGran = 10;    
    CDesCArrayFlat* watchers = new (ELeave) CDesCArrayFlat( KMyGran );
    CleanupStack::PushL( watchers );      // << watchers            
       
    // add user only once here.
    TDblQueIter<CPresencePluginWatcherInfo> rIter( iWatcherList );
    rIter.SetToFirst();
    
    while ( rIter )
        {
        CPresencePluginWatcherInfo* w = rIter;
        rIter++;
        
        TInt dummy = 0;
        // Zero -> Found
        if ( watchers->Find( w->SipId(), dummy ))
            {            
            watchers->AppendL( w->SipId() );
            }                    
        }
    
    DP_SDA("CPresencePluginPublisher::MakeCurrentWatcherListLC - end");        
    return watchers;    
    }

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::CollectWatchers
// --------------------------------------------------------------------------- 
//
void CPresencePluginPublisher::CollectWatchersL(  
	RPointerArray<MSimpleElement>& aElems )
	{
	using namespace NPresencePlugin::NPresence;
	HBufC* nodeContent = NULL;
	
	TInt count = aElems.Count();
	DP_SDA2("CPresencePluginPublisher::CollectWatchers elem count %d",count);
		
	for ( TInt i = 0; i < count; i++ )
		{
		MSimpleElement* elem = aElems[i];
		TPtrC8 p8( elem->LocalName());

		if (!( p8.CompareF( KPresenceWatcher8 )))
			{
			DP_SDA("CPresencePluginPublisher::CollectWatchers watchers found");
			const TDesC8* pp8 = elem->AttrValue( KPresenceStatus8 );
			// Active wathers here
			if ( pp8 && !pp8->CompareF( KPresenceActive8 ))
    			{
    			DP_SDA("CollectWatchers: ACTIVE found");	
				// save id since there may be multiple subscriptions 
				// from a single watcher SIP identity.
				const TDesC8* pId8 = elem->AttrValue( KPresenceId8 );
                                             
				// Build collection of grant requests
				// Find the child node containing the SIP entity
				nodeContent = elem->ContentUnicodeL();
				CleanupStack::PushL( nodeContent );// << nodeContent
                    
				AddWatcherIfNotExistsL( 
				    pId8 ? *pId8 : KNullDesC8, nodeContent->Des() );
				
				CleanupStack::PopAndDestroy( nodeContent ); //>> nodeContent
				}
			// Terminated wathers here
			else if ( pp8 && !pp8->CompareF( KPresenceTerminated8 ))
    			{
			    DP_SDA("CollectWatchers: Terminated");
				const TDesC8* pId8 = elem->AttrValue( KPresenceId8 );
				                
				// Remove terminated from iWatcherList
				nodeContent = elem->ContentUnicodeL();
				CleanupStack::PushL( nodeContent );// << nodeContent                
	    
				RemoveWatcherIfExists( 
    				pId8 ? *pId8 : KNullDesC8, nodeContent->Des() );                     
    
				CleanupStack::PopAndDestroy( nodeContent );// >> nodeContent
        		}
    		}
		}
	} 

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::CreatePublisherL()
// ---------------------------------------------------------------------------
//
void CPresencePluginPublisher::CreatePublisherL()
    {
    DP_SDA("CPresencePluginPublisher::CreatePublisherL");
    if( iPublisher )
        {
        DP_SDA("CPresencePluginPublisher::CreatePublisherL create");
        iPublisher->Close();
        iPublisher = NULL; 
        iPublisher = TSimpleFactory::NewPublisherL( iConnection, *this );
        }
    DP_SDA("CPresencePluginPublisher::CreatePublisherL end");
    }
	
// ---------------------------------------------------------------------------
// CPresencePluginPublisher::StoreOwnStatusToCacheL
// --------------------------------------------------------------------------- 
//
void CPresencePluginPublisher::StoreOwnStatusToCacheL()
	{	
	DP_SDA("CPresencePluginPublisher::StoreOwnStatusToCacheL" );
	
	using namespace NPresencePlugin::NPresence;
    using namespace NPresencePlugin::NPresenceStates;
    using namespace NPresenceInfo::NFieldType;
	
    DP_SDA(" CPresencePluginPublisher -> convert uri" );
    HBufC* entityUri = 
        CnvUtfConverter::ConvertToUnicodeFromUtf8L( 
            *iDocument->EntityURI() );
    CleanupStack::PushL( entityUri );

    DP_SDA(" CPresencePluginPublisher -> strip prefix" );
    HBufC16* prefixStripped = iPresenceData->RemovePrefixLC( 
        *entityUri );
             
    DP_SDA(" CPresencePluginPublisher -> fetch simple elements" );
    RPointerArray<MSimpleElement> simpleElements;
    CleanupClosePushL( simpleElements );
    DP_SDA(" CPresencePluginPublisher -> do fetch simple elements" );
    TInt error = iDocument->SimpleElementsL( simpleElements );    
    DP_SDA2(" CPresencePluginPublisher -> fetch error: %d", error );
    User::LeaveIfError( error );
    TBool handled = EFalse;
    DP_SDA2(" CPresencePluginPublisher -> element count: %d", 
        simpleElements.Count() );
    
    MSimpleElement* basicSimpleElem(NULL);
    MSimpleElement* noteSimpleElem(NULL);
    
    TBuf<KBufSize100> activitiesContentBuf;
    TBool activitiesHandled( EFalse );
    
    for( TInt index = 0 ; index < simpleElements.Count() ; index++ )
        {
        DP_SDA(" CPresencePluginPublisher -> check for person element" );
        // Check for person element
        if (!( simpleElements[index]->LocalName().CompareF( KPresencePerson8 )) ||
            !( simpleElements[index]->LocalName().CompareF( KPresenceTuple8 )) )
            {
            DP_SDA(" CPresencePluginPublisher -> person/tuple element found" );
            RPointerArray<MSimpleElement> personSubElems;
            CleanupClosePushL( personSubElems );
            simpleElements[index]->SimpleElementsL( personSubElems );
            DP_SDA2(" CPresencePluginPublisher -> person/tuple sub elemen count: %d", 
                personSubElems.Count() );

            for ( TInt j = 0; j < personSubElems.Count(); j++ )
                {
                DP_SDA(" CPresencePluginPublisher -> check for status/activities element");
                if ( !personSubElems[j]->LocalName().CompareF( KPresenceStatus8 ) ||
                     !personSubElems[j]->LocalName().CompareF( KPresenceActivities8 ))
                    {
         	        DP_SDA(" CPresencePluginPublisher -> status/activities element found");
         	        //Get childs
         	        RPointerArray<MSimpleElement> basicElems;
         	        CleanupClosePushL( basicElems );
         	        
		            personSubElems[j]->SimpleElementsL( basicElems );
                    DP_SDA2(" CPresencePluginPublisher -> child elem count: %d", 
                        basicElems.Count() );
		            for ( TInt k = 0; k < basicElems.Count(); k++ )
		                {
		                DP_SDA(" CPresencePluginPublisher -> check child elems");
		                if ( !basicElems[k]->LocalName().CompareF( 
		                		KPresenceBasic8 ))
		                    {
		                    DP_SDA(" -> basic element found");
		                    basicSimpleElem = basicElems[k];
		                    }
                 	    else if ( basicSimpleElem && 
                 	        !basicElems[k]->LocalName().CompareF( KPresenceActivities8 ) )
                 	        {
                 	        DP_SDA(" -> activities found from basic elem");
                 	        RPointerArray<MSimpleElement> activitiesElems;
                 	        CleanupClosePushL( activitiesElems );
    			            
    			            DP_SDA(" -> get activities");
    			            basicElems[k]->SimpleElementsL( activitiesElems );
    			            DP_SDA2(" -> activities count: %d", 
    			                activitiesElems.Count() );
                            if ( activitiesElems.Count() )
                                {
                                DP_SDA(" -> get activities, multiple, use last");
        			            TInt lastActiviesIndex = 
        			                ( activitiesElems.Count() - 1 );
    			                activitiesContentBuf.Copy( 
    			                    activitiesElems[lastActiviesIndex]->LocalName() );
    			                activitiesHandled = ETrue;
    			                DP_SDA(" -> activity elem ok");
                                }
    			            CleanupStack::PopAndDestroy( &activitiesElems );    
                 	        }
                 	    else if ( basicSimpleElem && 
                 	            !personSubElems[j]->LocalName().CompareF( KPresenceActivities8 ) )
                 	        {
                 	        DP_SDA(" -> activities found from person elem");    
                 	        RPointerArray<MSimpleElement> activitiesElems;
                 	        CleanupClosePushL( activitiesElems );
    			            
    			            DP_SDA(" -> get activities");
    			            personSubElems[j]->SimpleElementsL( activitiesElems );
    			            DP_SDA2(" -> activities count: %d", 
    			                activitiesElems.Count() );
                            if ( activitiesElems.Count() )
                                {
                                DP_SDA(" -> get activities, multiple, use last");
        			            TInt lastActiviesIndex = 
        			                ( activitiesElems.Count() - 1 );
    			                activitiesContentBuf.Copy( 
    			                    activitiesElems[lastActiviesIndex]->LocalName() );
    			                activitiesHandled = ETrue;
    			                DP_SDA2(" -> activities len: %d", 
    			                    activitiesContentBuf.Length() );
    			                DP_SDA(" -> activity elem ok");
                                }
    			            CleanupStack::PopAndDestroy( &activitiesElems );                     	                     	     
                 	        }
		                } 
		            CleanupStack::PopAndDestroy( &basicElems );
         	        }
                }
            CleanupStack::PopAndDestroy( &personSubElems );
            }
 	    else if ( !simpleElements[index]->LocalName().CompareF( KPresenceNote8 ))
 	        {
 	        DP_SDA(" CPresencePluginPublisher -> note element found" );
            noteSimpleElem = simpleElements[index];    
 	        }
        }
    DP_SDA(" CPresencePluginPublisher -> elements handled");
    __ASSERT_ALWAYS( basicSimpleElem, User::Leave( KErrCorrupt ) );
    
    if ( !activitiesHandled )
        {
        DP_SDA(" -> activities not handled, set unknown");
        activitiesContentBuf.Copy( KPresenceUnknow );
        }
    
    HBufC* nodeContent = basicSimpleElem->ContentUnicodeL();
    CleanupStack::PushL( nodeContent );
    
    HBufC* noteContent = NULL;
    if ( noteSimpleElem )
        {
        noteContent = noteSimpleElem->ContentUnicodeL();
        CleanupStack::PushL( noteContent );
        }
    else
        {
        noteContent = KNullDesC().AllocLC();
        }
    
    MPresenceBuddyInfo2::TAvailabilityValues cacheAvailability =
        MPresenceBuddyInfo2::ENotAvailable;
    NPresenceInfo::TAvailabilityValues ximpAvailability =
        NPresenceInfo::ENotAvailable;
    
    HBufC* extendedCacheAvailability = HBufC::NewLC( KBufSize255 );
    TPtr extendedCacheAvailabilityPtr( extendedCacheAvailability->Des() );
    if ( nodeContent )
        {
        handled = ResolveOwnCacheValues( *nodeContent, 
            activitiesContentBuf, cacheAvailability, 
            extendedCacheAvailabilityPtr, ximpAvailability );
        }

    __ASSERT_ALWAYS( handled, User::Leave( KErrCorrupt ));

    DP_SDA(" CPresencePluginPublisher -> write to cache" );
    
    DP_SDA2(" CPresencePluginPublisher -> availability: %d", 
        cacheAvailability );
    DP_SDA2(" CPresencePluginPublisher -> ext availability len: %d", 
        extendedCacheAvailabilityPtr.Length() );        

    iPresenceData->WriteStatusToCacheL( 
        *prefixStripped, cacheAvailability, 
        extendedCacheAvailabilityPtr, *noteContent );           
             
    // Store publish variables for republish
    iPresenceData->StorePresenceOwnPresenceL(
        iConnObs.ServiceId(), ximpAvailability, *noteContent );

    CleanupStack::PopAndDestroy( extendedCacheAvailability );    
    CleanupStack::PopAndDestroy( noteContent );        
    CleanupStack::PopAndDestroy( nodeContent );        

    DP_SDA(" CPresencePluginPublisher -> attributes handled" );    
    CleanupStack::PopAndDestroy( &simpleElements );
        
    DP_SDA(" CPresencePluginPublisher -> write to cache ok, cleanup" );
    
    // prefixStripped, entityUri
    CleanupStack::PopAndDestroy( 2 );	
    DP_SDA("CPresencePluginPublisher::StoreOwnStatusToCacheL out" );
	}

// ---------------------------------------------------------------------------
// CPresencePluginPublisher::ResolveOwnCacheValues
// --------------------------------------------------------------------------- 
//
TBool CPresencePluginPublisher::ResolveOwnCacheValues( 
    const TDesC& aUnicodeContent,
    const TDesC& aActivities, 
    MPresenceBuddyInfo2::TAvailabilityValues& aCacheAvailability,
    TPtr& aExtendedCacheAvailability,
    NPresenceInfo::TAvailabilityValues& aXimpApiAvailability )
    {
    DP_SDA("CPresencePluginPublisher::ResolveOwnCacheValuesL" );
    using namespace NPresencePlugin::NPresence;
    using namespace NPresencePlugin::NPresenceStates;
    using namespace NPresenceInfo::NFieldType;
    
    TBool handled = EFalse;
    if ( !aUnicodeContent.CompareF( KPresenceOpen ) &&
        !aActivities.CompareF ( KPresenceBusy ) )
        {
        DP_SDA(" CPresencePluginPublisher -> busy");
        aCacheAvailability = MPresenceBuddyInfo2::EBusy;
        aExtendedCacheAvailability.Copy( KDndState() );
        aXimpApiAvailability = NPresenceInfo::EBusy;
        handled = ETrue;
        }
    // On-The-Phone case
    else if ( !aUnicodeContent.CompareF( KPresenceOpen ) &&
        !aActivities.CompareF ( KPresenceOnThePhone ) )
        {
        DP_SDA(" CPresencePluginPublisher ->  on-the-phone");
        aCacheAvailability = MPresenceBuddyInfo2::EBusy;
        aExtendedCacheAvailability.Copy( KOnPhoneState() );
        aXimpApiAvailability = NPresenceInfo::EOnPhone;
        handled = ETrue;
        }
    //Away case  
    else if ( !aUnicodeContent.CompareF( KPresenceOpen ) &&
        !aActivities.CompareF ( KPresenceAway ) )
        {
        DP_SDA(" CPresencePluginPublisher -> away");
        aCacheAvailability = MPresenceBuddyInfo2::EBusy;
        aExtendedCacheAvailability.Copy( KAwayState() );
        aXimpApiAvailability = NPresenceInfo::EAway;
        handled = ETrue;
        }
    //Dnd case  
    else if ( !aUnicodeContent.CompareF( KPresenceOpen ) &&
        !aActivities.CompareF ( KPresenceDoNotDisturb ) )
        {
        DP_SDA(" CPresencePluginPublisher -> dnd");
        aCacheAvailability = MPresenceBuddyInfo2::EBusy;
        aExtendedCacheAvailability.Copy( KDndState() );
        aXimpApiAvailability = NPresenceInfo::EDoNotDisturb;
        handled = ETrue;
        }        
    // Unknown open
    else if ( !aUnicodeContent.CompareF( KPresenceOpen ) &&
        !aActivities.CompareF ( KPresenceUnknow ) )
        {
        DP_SDA(" CPresencePluginPublisher -> open");
        aCacheAvailability = MPresenceBuddyInfo2::EAvailable;
        aExtendedCacheAvailability.Copy( KDefaultAvailableStatus() );
        aXimpApiAvailability = NPresenceInfo::EAvailable;
        handled = ETrue;
        }
    // available open
    else if ( !aUnicodeContent.CompareF( KPresenceOpen ) &&
        !aActivities.CompareF ( KPresenceAvailable ) )
        {
        DP_SDA(" CPresencePluginPublisher -> open");
        aCacheAvailability = MPresenceBuddyInfo2::EAvailable;
        aExtendedCacheAvailability.Copy( KDefaultAvailableStatus() );
        aXimpApiAvailability = NPresenceInfo::EAvailable;
        handled = ETrue;
        } 
    // available open
    else if ( !aUnicodeContent.CompareF( KPresenceOpen ) &&
        !aActivities.CompareF ( KPresenceOpen ) )
        {
        DP_SDA(" CPresencePluginPublisher -> open/open");
        aCacheAvailability = MPresenceBuddyInfo2::EAvailable;
        aExtendedCacheAvailability.Copy( KDefaultAvailableStatus() );
        aXimpApiAvailability = NPresenceInfo::EAvailable;
        handled = ETrue;
        }                
    //Unknown closed
    else if ( !aUnicodeContent.CompareF( KPresenceClosed ) &&
        !aActivities.CompareF ( KPresenceUnknow ) )
        {
        DP_SDA(" CPresencePluginPublisher -> closed");
        aCacheAvailability = MPresenceBuddyInfo2::ENotAvailable;
        aExtendedCacheAvailability.Copy( KInvisibleState() );
        aXimpApiAvailability = NPresenceInfo::ENotAvailable;
        handled = ETrue;
        }
    //All other states are closed
    else
        {
        DP_SDA(" CPresencePluginPublisher -> else closed");
        aCacheAvailability = MPresenceBuddyInfo2::ENotAvailable;
        aExtendedCacheAvailability.Copy( KInvisibleState() );
        aXimpApiAvailability = NPresenceInfo::ENotAvailable;
        handled = ETrue;
        }
    DP_SDA2("CPresencePluginPublisher::ResolveOwnCacheValuesL out, ret: %d", 
        handled );    
    return handled;
    }

// End of file
