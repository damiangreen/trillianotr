/*
This file is a part of "Trillian OTR - Risen" which is a continuation of "Trillian OTR"
by Kittyfox Communications (c) 2006.
Copyright (C) 2011 Ian Zachary Ledrick

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

This project makes use of the following toolsets or libraries:
lib-OTR Copyright (C) 2004-2008  Ian Goldberg, Chris Alexander, Nikita Borisov. <otr@cypherpunks.ca>
GPLv2.1

lib-GPG-Error Copyright (C) 2003, 2004 g10 Code GmbH
GPLv2.1

lib-GCrypt Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.
GPLv2.1

Trillian Plugin Exports (C) 2011 Cerulean Studios
Trillian Developer License

Trillian OTR (C) 2006 Kittyfox Communications
GPLv2

Special thanks to Alextheblade and Adipose from Trillian Developer forums
*/

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef STDAFX_H
#define STDAFX_H

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>
#include <atlwin.h>
#include <atlapp.h>
#include <atlctrls.h>
#include <atlrx.h>

// TODO: reference additional headers your program requires here
extern "C"
{
#include <gcrypt.h>
}

/* Import our OTR headers */
extern "C"
{
#include <userstate.h>
#include <proto.h>
#include <privkey.h>
#include <message.h>
#include "plugin.h"
}

/* Windows related */
#include <wininet.h>
#include <shellapi.h>
#include <direct.h>
#include <CommDlg.h>
#include <ShlObj.h>
#include <Shlwapi.h>

/* Standard Libraries */
#include <vector>
#include <fstream>
#include <iostream>
#include <string>

/* Project Files */
#include "resource.h"
#include "dlgPrefs.h"
#include "dlgConfig.h"
#include "dlgGeneral.h"
#include "dlgSession.h"
#include "totrConfig.h"
#include "dlgAbout.h"
#include "dlgNewFingerprint.h"
#include "dlgUnknownFingerprint.h"
#include "dlgVerifyFingerprint.h"
#include "TotrState.h"
#include "TotrAppData.h"
#include "totrMenu.h"
#include "ConnectionEntryWrapperT.h"
#include "totrUtils.h"
#include "totrOtrl.h"
#include "trillianotr.h"
#include "TrillianTypes.h"
#include "SMPStartDlg.h"
#include "SMPChallengeDlg.h"


/* Set up my GUID */
#define MYGUID							"902A5401-D29B-4b44-8F9D-A9E81B2800CC"
#define MYNAME							"Trillian OTR - Risen"
#define COMPANY							"Ian Z Ledrick aka Thisita"
/* #define VERSION						Version data is now stored in the resources */
#define DESCRIPTION						"Off the Record messaging support for Trillian."

#define PRIVKEYFILE						"otr.private_key"
#define STOREKEYFILE					"otr.fingerprints"
#define CONFIGFILE						"trillianotr.xml"

#if 0
typedef int (*ttkCallback)(int windowID, char* subwindow, char* event, void* data, void* userData); 
typedef int (*plugin_function_main)(char* event, void *data); 
typedef int (*plugin_function_send)(char* guid, char* event, void* data);
#endif

typedef struct _acctlookup
{
	int connection_id;
	char* account_name;
} TotrAcctLookup;

typedef struct _userlookup
{
	int connection_id;
	char* real_name;
	char* name;
} TotrUserLookup;

/* Define our data structures that span files */
extern HINSTANCE				hInstance;
extern plugin_function_send		plugin_send;

extern OtrlUserState			userstate;
extern OtrlMessageAppOps		totr_ops;

extern CDialogPrefs*			prefDlg;

extern TotrConfig				totr_config;
extern TotrConfig*				tmp_config;
extern TotrState				totr_state;


#endif
