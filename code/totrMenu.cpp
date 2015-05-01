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

/* Bring in our standard header */
#include "stdafx.h"

#if 0
// Do we need to move these?
extern	OtrlPolicy policy_cb(void* opdata, ConnContext* context);
extern	void inject_message_cb(void* opdata, const char* accountname,
	    const char* protocol, const char* recipient, const char* message);
#endif

/* Our Code */

int totrSMPVerification(int windowID, char* subwindow, char* event, void* data, void* userData)
{
	/* Set up our variables */
	ConnContext* conn = reinterpret_cast<ConnContext*>(userData);
	menu_entry_t* met = reinterpret_cast<menu_entry_t*>(data);
	if(!conn || strcmp(event, "menu-select") != 0)
		return 0;
	CSMPStartDlg* dlg = new CSMPStartDlg();
	dlg->Create(met->hwnd, (LPARAM) conn);
	dlg->ShowWindow(TRUE);
	return 0;
}

int totrNewConversation(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	ConnContext* conn = reinterpret_cast<ConnContext*>(userData);
	if(!conn || strcmp(event, "menu-select") != 0)
		return 0;

	TotrAppData* appdata = reinterpret_cast<TotrAppData*>(conn->app_data);
	if(!appdata)
		return 0;


#if 0
		/* Starting a new conversation and refreshing an old conversation
		 * are fired off in exactally the same way. 
		 */
		struct message_t msgt;
        char *msg = otrl_proto_default_query_msg(conn->accountname, 
												 totr_ops.policy(NULL, conn));
		char *account_name = accountNameLookup(conn->protocol, appdata->connection_id);
		//char *display_name = userDisplayNameLookup(conn->protocol, conn->username, appdata->connection_id);
		int online = -1;

		if (!account_name)
			return 0;

		trillianInitialize(msgt);

		msgt.connection_id = appdata->connection_id;
		msgt.window_id = appdata->window_id;
		msgt.medium = conn->protocol;
		msgt.name = conn->username;
		//msgt.display_name = display_name;

		online = totr_ops.is_logged_in(&msgt, account_name, conn->protocol, conn->username);

		if (online == 1) {
			totr_ops.inject_message(&msgt, account_name, conn->protocol, conn->username, 
				(msg ? msg : "?OTRv2?"));
		} else {
			/* Dare we want to try and fire off some status message, popup window, or
			 * something to instruct the user that the system cannot send the
			 * initiation request?
			 */
			;
		}

		free(account_name);
		account_name = NULL;
//		totr_free(display_name);
#else
		totrInitiateSession(conn);
#endif

	return 0;
}

int totrEndConversation(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	ConnContext *conn = reinterpret_cast<ConnContext*>(userData);
	if(!conn || strcmp(event, "menu-select") != 0)
		return 0;

	TotrAppData *appdata = reinterpret_cast<TotrAppData*>(conn->app_data);
	if(!appdata)
		return 0;

#if 0
		if (!(account_name = accountNameLookup(conn->protocol, appdata->connection_id)))
			return 0;

		trillianInitialize(msg);

		msg.connection_id = appdata->connection_id;
		msg.window_id = appdata->window_id;
		msg.medium = conn->protocol;
		msg.name = conn->username;

		otrl_message_disconnect(userstate, &totr_ops, &msg, account_name, conn->protocol,
			conn->username);
#else
		totrTerminateSession(conn);
#endif

	return 0;
}

int totrVerifyFingerprint(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	menu_entry_t* met = reinterpret_cast<menu_entry_t*>(data);
	ConnContext* conn = reinterpret_cast<ConnContext*>(userData);
	if(!conn || !met || strcmp(event, "menu-select") != 0)
		return 0;

	totrShowVerifyDialog(met->hwnd, conn, conn->active_fingerprint);

	return 0;
}

int totrViewSessionID(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	CSessionDlg* session = NULL;
	menu_entry_t* met = reinterpret_cast<menu_entry_t*>(data);
	ConnContext *conn = reinterpret_cast<ConnContext*>(userData);
	if(!conn || strcmp(event, "menu-select") != 0)
		return 0;

	session = new CSessionDlg; // This is never cleaned up...
	session->Create(met->hwnd, reinterpret_cast<LPARAM>(conn)); // used to be reconn
	session->ShowWindow(TRUE);

	return 0;
}

int totrViewPreferences(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	ConnContext* conn = reinterpret_cast<ConnContext*>(userData);
	if(!conn || strcmp(event, "menu-select") != 0)
		return 0;

	plugin_prefs_t ppt;
	trillianInitialize(ppt);
	ppt.pref_name = "Trillian OTR";
	plugin_send(MYGUID, "prefsOpen", &ppt);

	return 0;
}

/**
* Adds a number of days to a time_t object
*/
time_t subtractDays(const time_t date, const int days)
{
	return (date - (days * 60 * 60 * 24));
}

/**
* Formats the filename string for a history file.
* @Comment Do we want to move this to totr_config class?
*/
void formatHistoryFileString(std::string* filename, const char* medium, const char* date, const char* name)
{
	*filename += totr_config.GetHistoryDirectory();
	*filename += "\\";
	*filename += medium;
	*filename += "\\";
	*filename += date;
	*filename += "\\";
	*filename += name;
	*filename += ".txt";
}

/**
* Opens the latest history file in the default text editor.
*/
int totrViewHistory(int windowID, char* subwindow, char* event, void* data, void* userData)
{
	if (strcmp(event, "menu-select") != 0)
		return 0; // If the item isn't being selected
				// We bail.

	ConnContext* conn = reinterpret_cast<ConnContext*>(userData);
	if(!conn)
		return 0; // If we are given dead data, bail out

	/* Check to see if a file exists */
	std::string filename = "";
	char date[256];
	const time_t curTime = time(NULL);
	time_t bufTime = curTime;
	//sprintf_s(filename,256,"%s\\%s\\%s\\%s.txt",totr_config.GetHistoryDirectory(),conn->protocol,date,conn->username);
	
	int count = 0;
	while(count <= 10) // We don't want to go back more than 10 days in the future
	{ /* Do we want to add a user option to change this behavior? */
		bufTime = subtractDays(curTime, count);
		strftime(date,256,"%d-%b",localtime(&bufTime));
		formatHistoryFileString(&filename, conn->protocol, date, conn->username);
		HINSTANCE result = ShellExecute(NULL,"edit",filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
		if((int)result > 32) return 0;
		count++;
	}

	// If we get this far, there is no history, or it is too far back
	MessageBox(NULL, "A history file could not be opened.", "ERROR", MB_OK);
	return 0;
}

int totrOptionsMenu(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	if(!strcmp(event, "menu-open-finish"))
	{
		menu_entry_t *met = reinterpret_cast<menu_entry_t*>(data);
		menu_entry_t *met_next = NULL;

		while(met)
		{
			met_next = met->next_menu;
			delete met;
			met = met_next;
		}
	}

	if(strcmp(event, "menu-open") != 0)
		return 0;

	menu_entry_t *met = NULL;
	menu_entry_t *met_head = NULL;
	ConnContext *conn = reinterpret_cast<ConnContext*>(userData);

	if(!conn)
		return 0;

	/* Start with the sub menus */

	/* Added by Thisita: Putting these in their own submenu */
	met_head = met = new menu_entry_t;
	trillianInitialize((*met));

	met->type = MENU_POPUP;
	met->menu_id = -1;
	met->sub_menu_id = -1;
	met->disabled = 0;
	met->text = "OTR Options";
	met->timeout = MENU_TIMEOUT_INFINITE;
	/* End Thisita's changes */


	/*met_head = */met->sub_menu = new menu_entry_t;
	met = met->sub_menu; // Change by Thisita
	trillianInitialize((*met));

	met->type = MENU_TEXT;
	met->menu_id = -1;
	met->sub_menu_id = -1;
	met->disabled = 0;
	if (conn->msgstate == OTRL_MSGSTATE_PLAINTEXT)
		met->text = "Start Conversation";
	else
		met->text = "Refresh Private Session";

	met->timeout = MENU_TIMEOUT_INFINITE;
	met->callback = totrNewConversation;
	met->data = conn;

	met = met->next_menu = new menu_entry_t;
	trillianInitialize((*met));

	met->type = MENU_TEXT;
	met->menu_id = -1;
	met->sub_menu_id = -1;
	met->disabled = conn->msgstate == OTRL_MSGSTATE_PLAINTEXT;
	met->text = "End Conversation";
	met->timeout = MENU_TIMEOUT_INFINITE;
	met->callback = totrEndConversation;
	met->data = conn;

	met = met->next_menu = new menu_entry_t;
	trillianInitialize((*met));

	met->type = MENU_SEPARATOR;
	met->menu_id = -1;
	met->sub_menu_id = -1;

	met = met->next_menu = new menu_entry_t;
	trillianInitialize((*met));

	met->type = MENU_TEXT;
	met->menu_id = -1;
	met->sub_menu_id = -1;
	met->disabled = !(conn->active_fingerprint);
	met->text = "Verify Fingerprint";
	met->timeout = MENU_TIMEOUT_INFINITE;
	met->callback = totrVerifyFingerprint;
	met->data = conn;


	met = met->next_menu = new menu_entry_t;
	trillianInitialize((*met));

	met->type = MENU_TEXT;
	met->menu_id = -1;
	met->sub_menu_id = -1;
	met->disabled = (conn->sessionid[0] == '\0');
	met->text = "View Session ID";
	met->timeout = MENU_TIMEOUT_INFINITE;
	met->callback = totrViewSessionID;
	met->data = conn;

	met = met->next_menu = new menu_entry_t;
	trillianInitialize((*met));

	met->type = MENU_TEXT;
	met->menu_id = -1;
	met->sub_menu_id = -1;
	met->disabled = !totr_config.GetSaveHistory();
	met->text = "View Last History Log";
	met->timeout = MENU_TIMEOUT_INFINITE;
	met->callback = totrViewHistory;
	met->data = conn;

	met = met->next_menu = new menu_entry_t;
	trillianInitialize((*met));

	met->type = MENU_TEXT;
	met->menu_id = -1;
	met->sub_menu_id = -1;
	met->disabled = conn->msgstate == OTRL_MSGSTATE_PLAINTEXT;
	met->text = "Verify via Question";
	met->timeout = MENU_TIMEOUT_INFINITE;
	met->callback = totrSMPVerification;
	met->data = conn;

	met = met->next_menu = new menu_entry_t;
	trillianInitialize((*met));

	met->type = MENU_SEPARATOR;
	met->menu_id = -1;
	met->sub_menu_id = -1;

	met = met->next_menu = new menu_entry_t;
	trillianInitialize((*met));

	met->type = MENU_TEXT;
	met->menu_id = -1;
	met->sub_menu_id = -1;
	met->text = "Preferences";
	met->timeout = MENU_TIMEOUT_INFINITE;
	met->callback = totrViewPreferences;
	met->data = conn;
	
	return (int) met_head;
}

void totrMenuFree(message_menu_t* mmt)
{
	menu_entry_t* met = mmt->menu_entry;
	menu_entry_t* met_next = NULL;

	while(met)
	{
		met_next = met->next_menu;
		delete met;
		met = met_next;
	}

	delete mmt;
}

message_menu_t* totrGenerateMenu(int connection_id, int window_id,
										char* medium, char* name, char* location)
{
	message_menu_t* mmt = new message_menu_t;
	menu_entry_t* met = NULL;
	ConnContext* conn = NULL;
	char* account_name = NULL;

	if(!mmt)
		return NULL;

	/* Basic info fillin from what we got */
	mmt->connection_id = connection_id;
	mmt->window_id = window_id;
	mmt->medium = medium;
	mmt->name = name;
	mmt->location = location;

	account_name = accountNameLookup(medium, connection_id);
	conn = otrl_context_find(userstate, name, account_name, medium, 0, NULL, NULL, NULL);

	/* Our type and name */
	mmt->menu_type = MENU_TYPE_RIGHTCLICK ;
	mmt->menu_name = /*"Trillian OTR"*/ NULL;

	/* Start with the sub menus */
	mmt->menu_entry = met = new menu_entry_t;
	trillianInitialize((*met));

	met->type = MENU_CALLBACK;
	met->menu_id = -1;
	met->sub_menu_id = -1;
	met->disabled = 0;
	met->text = NULL;
	met->timeout = MENU_TIMEOUT_INFINITE;
	mmt->menu_entry->callback = totrOptionsMenu;
	mmt->menu_entry->data = reinterpret_cast<void*>(conn);

	return mmt;
}