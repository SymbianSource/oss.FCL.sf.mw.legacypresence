/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Connection manager
*
*/




#ifndef __MSGCONNMANAGER__
#define __MSGCONNMANAGER__

// INCLUDES
#include <commdb.h>
#include <es_sock.h>
#include <commdbconnpref.h>
#include <msgconnmanagerapi.h>
#include "msgconntimercallback.h"


//CONSTANTS
_LIT( KConnManLogDir,                           "MsgConnMan" );
_LIT( KConnManLogFile,                          "MsgConnManager.txt" );
const TInt KConnManLogBufferMaxSize		        = 2000;

//FORWARD DECLARATIONS
class CMsgConnTimer;
class MMsgBearerObsCallback;

// CLASS DECLARATION
class CMsgConnManager : public CActive,
                        public MMsgConnManager,
                        public MMsgConnTimerCallback
    {
    public:  // Constructors and destructor

        /**
        * Symbian OS default constructor.
        * @param TInt Default Access Point ID
        * @return CMsgConnManager*
        */     
        static CMsgConnManager* NewL( const TInt aAccessPoint );

        /**
        * Destructor.
        */      
        virtual ~CMsgConnManager();

    public:   //New functions   
        
        /**
        * Symbian OS default constructor.
        * @param TRequestStatus& Request status of the caller
        * @return void
        */  
        virtual void StartConnection( TRequestStatus& aStatus );
    
        /**
        * Stop a connection, synchronous version
        * @return void
        */  
        virtual void StopConnection();
        
        /**
        * Stop a connection, asynchronous version
        * @return void
        */  
        virtual void StopConnection( TRequestStatus& aStatus );
        
        /**
        * Cancel StartConnection(). The method may leave with
        *   - KErrAbort if the TRequestStatus object the client
        *     supplied is not active, hence, there is no pending
        *     request that would require cancelling.
        * In normal cases - when RConnection::Start() or its 
        * progress notifications are pending - ConnMan completes
        * the client's request with the "error" KErrCancel.
        */
        virtual void CancelStartL();

        /**
        * Set the ID of the Access Point to connect to.
        * @param aAccessPointID
        * @return void
        */
        virtual void SetAccessPointIDL( const TInt aAccessPointID );

        /**
        * Returns the number of connections presently active
        * @return TInt
        */
        virtual TInt NumberOfActiveConns();
        
        /**
        * Read from the Comms Database
        * @param const TMsgSupportedCommDbRecords& aParameter
        * @return HBufC8*
        */
        virtual HBufC* ReadFromCommsDbLC( const TSupportedCommDbRecords aParameter );
        
        /**
        * Returns a reference to the active socket server.
        * @return RConnection&
        */
        virtual RConnection& Connection();

        /**
        * Returns a reference to the active socket server.
        * @return RSocketServ&
        */
        virtual RSocketServ& SocketSession();

        /**
        * Add an object to the queue of listeners. The object to be added
        * must inplement the interface MMsgBearerObsCallback in order
        * to receive events from the system agent. It is important to note
        * that the call to the notification handler (HandleBearerEventL())
        * takes place inside the RunL() method of this Connection Manager,
        * so the listening object MUST return the control to the Manager
        * AS SOON AS POSSIBLE in order not to clog the scheduler.
        * @param MMsgBearerObsCallback* aClient
        * @return void
        */
        virtual void AddEventSubscriberL( MMsgBearerObsCallback* aClient );
        
        /**
        * Remove an object from the queue of listeners. It is no necessary
        * to call this method at deletion time, as the destructor destroys
        * the subscriber queue altogether. If, however, a listener object is
        * destroyed before an instance of Connection Manager, it is mandatory
        * to remove the listener from the queue prior to deleting the listener.
        * @param MMsgBearerObsCallback* aClient
        * @return void
        */
        virtual void RemoveEventSubscriber( MMsgBearerObsCallback* aClient );

        /**
        * Returns the status of the ConnMan: ETrue if sending is OK, EFalse otherwise
        * @return TBool
        */
        virtual TBool Status() const;
        
        /**
        * Destructor.
        * @return void
        */
        virtual void Destroy();
        
    public:

    #ifdef _DEBUG

        /**
        * A Global logging function
        * @param TRefByValue<const TDesC8> aFmt,...
        * @return void
        */ 
        static void WriteToLog( TRefByValue<const TDesC8> aFmt,... );

    #endif

    private:
        
        /**
        * Start to keep an eye on the opened connection
        * @return void
        */ 
        void StartNetworkObserver();
        
        /**
        * Determine the state of the connection opening
        * @return void
        */ 
        void WatchConnectionOpening();
        
        /**
        * Determine the state of the (opened) connection
        * @return void
        */ 
        void DetermineConnectionStatusL();
        
        /**
        * Broadcast the bearer event to registered listeners
        * @param TMsgBearerEvent aBearerEvent
        * @return void
        */ 
        void BroadcastBearerEventL( const TMsgBearerEvent aBearerEvent );

        /**
        * Determine which value is to be fetched from the CommsDb
        * @param TPtrC& aDbView
        * @param TPtrC& TPtrC& aDbColumn
        * @param TMsgSupportedCommDbRecords aParameter
        * @return TBool
        */ 
        TInt DetermineRecordTypeLC( TPtrC& aDbColumn,
                                    CCommsDbTableView*& aView,
                                    const TSupportedCommDbRecords aParameter );

        /**
        * Broadcast the bearer event to registered listeners
        * @param TMsgBearerEvent aBearerEvent
        * @return void
        */ 
        TInt InitCommDbViewLC( TPtrC& aDbRecord,
                               CCommsDbTableView*& aView,
                               const TSupportedCommDbRecords aParameter );
        
        /**
        * Broadcast the bearer event to registered listeners
        * @param TMsgBearerEvent aBearerEvent
        * @return void
        */ 
        TBool CheckProxyUsageLC( CCommsDbTableView*& aView );

        /**
        * Issue a new asynchronous request
        * @return void
        */
        void IssueRequest();
        
        /**
        * Check the availability of GPRS Network
        * @return TBool Available or not
        */
        TBool IsValidApL( TUint32 aUid );
        
        /**
        * Handle the opening of a new connection
        * @return void
        */
        void HandleNewConnection();

        /**
        * Handle an existing connection
        * @return void
        */
        void HandleExistingConnection();

        /**
        * Symbian OS second-phase constructor
        * @param TInt aAccessPoint
        * @return void
        */ 
        void ConstructL( const TInt aAccessPoint );
        
        /**
        * Delete the log files
        * @return void
        */ 
        void DeleteLogFileL();

        /**
        * C++ default constructor is private
        * @return void
        */      
        CMsgConnManager();

        /**
        * From CActive
        * @return void
        */
        void RunL();

        /**
        * From CActive
        * @return void
        */
        void DoCancel();
    
    private:  //From MMsgConnectionTimerCallback
        
        /**
        * Catch a timer event
        * @param TInt aStatus
        * @return void
        */
        virtual void HandleTimerEvent( TInt aStatus );
        
    private: //Data
        
        enum TMsgConnManState
            {
            EMsgConnManIdle = 0,
            EMsgConnManWaitingForStartComplete,
            EMsgConnManWaitingForNotifications,
            EMsgConnManDisconnectingPrevious,
            EMsgConnManObservingAccessPoint,
            EMsgConnManBearerSuspended,
            EMsgConnManBearerLost
            };
        
        TInt                                            iAccessPointID;
        TInt                                            iLastConnectionStage;
        TBool                                           iProgressStarted;
        TBool                                           iConnectionClosed;
        TNifProgressBuf                                 iProgressBuffer;
        TMsgConnManState                                iConnManStatus;
        TRequestStatus*                                 iClientStatus;
        TRequestStatus*                                 iPingRequestStatus;
        TRequestStatus*                                 iTimedCompleteStatus;
        CCommsDatabase*                                 iCommsDatabase;
        CMsgConnTimer*                                  iConnectionTimer;
        RConnection                                     iConnection;
        TCommDbConnPref                                 iAccessPointSettings;     
        RSocketServ                                     iSocketSession;
        RPointerArray<MMsgBearerObsCallback>	        iEventSubscribers;
    };

// ---------------------------------------------------------
// CMsgConnManager::StartBearerObserver
// If the status is any other than EMsgConnManObservingAccessPoint
// there is something fishy going on => do not send/receive anything
// ---------------------------------------------------------
//
inline TBool CMsgConnManager::Status() const
    {
    return iConnManStatus == EMsgConnManObservingAccessPoint;
    }

// ---------------------------------------------------------
// CMsgConnManager::SocketSession
// 
// ---------------------------------------------------------
//
inline RSocketServ& CMsgConnManager::SocketSession()
    {
    return iSocketSession;
    }

// ---------------------------------------------------------
// CMsgConnManager::Connection
// 
// ---------------------------------------------------------
//
inline RConnection& CMsgConnManager::Connection()
    {
    return iConnection;
    }

// ---------------------------------------------------------
// CMsgConnManager::Destroy
// 
// ---------------------------------------------------------
//
inline void CMsgConnManager::Destroy()
    {
    delete this;
    }

#endif


// End of File
