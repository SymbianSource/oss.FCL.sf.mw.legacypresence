/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    XDM GS plugin data holding class for XDM setting list
*
*/




#ifndef __SettingsData_H__
#define __SettingsData_H__

// INCLUDE FILES
#include <e32base.h>
#include <in_sock.h>

#include "XDMPlugin.hrh"


/**
*  CSettingsData data holding class for an XDM set
*/
class CSettingsData : public CBase
    {
    public:

        /**
        * Default 1st phase factory method.
        * Creates an instance of CSettingsData
        */
      static CSettingsData* NewL();
      
        /**
        * Default 1st phase factory method.
        * Creates an instance of CSettingsData, leaves it on stack
        */
        static CSettingsData* NewLC();
        
        /**
        * Destructor
        */
      virtual ~CSettingsData();
      
        /**
        * Resets all data to initial values
        */
      void Reset();
      
    private:

        /**
        * 2nd Phase constructor
        */
      void ConstructL();
      
        /**
        * C++ Constructor
        */
      CSettingsData();
      
    public:
        
        // Buffer holding the XDM set name
      TBuf<KMaxSettingSetNameLength>  iSettingName;

      // Buffer holding the XDM set name with localized digits for display
      TBuf<KMaxSettingSetNameLength>  iSettingNameDisp;

        // Buffer holding the XDM set server address
      TBuf<KMaxServerAddressLength>   iServerAddress;
      
      // Access point number
      TInt32                          iAccessPoint;
      
      // Buffer holding the access point number as a descriptor
      TBuf<KMaxAccessPointDesLength>  iAccessPointDes;
      
      // Buffer holding the SIP URL
      TBuf<KMaxSIPURLLength>          iSipURL;

        // Buffer holding the User ID
      TBuf<KMaxUserIDLength>          iUserID;
      
      // Buffer holding the password
      TBuf<KMaxPasswordLength>        iPassword;
      
      // Buffer holding the access point name
      TBuf<KMaxAccessPointNameLength> iAccessPointName;
      
      // XDM setting, settings id, used to identify in XDM settings API
      TInt32                          iSettingId;
    };

#endif // __SettingsData_H__