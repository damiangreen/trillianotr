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

/* Our includes */
#include "stdafx.h"
#include "TrillianOTR.h"


/* Our Global Variables 
 * 
 * NOTE: Do we want to leave these here or move to the stdafx?
 */
//CAppModule				_Module;
#if 1
OtrlUserState			userstate = NULL;

HINSTANCE				hInstance;
CDialogPrefs*			prefDlg = NULL;

TotrConfig				totr_config;
TotrConfig*				tmp_config;
TotrState				totr_state;
plugin_function_send	plugin_send = NULL; 

unsigned char*			icon_32;
unsigned char*			icon_48;
unsigned long			icon_32_size;
unsigned long			icon_48_size;
#endif

/* Our Code */

/**
 * @brief Closes and reopens a message window really quickly
 *
 * @param message The structure containing the message window details.
 */
void reopenMessageWindow(const message_t* message)
{
	if(!plugin_send)
	{
		MessageBox(NULL, "plugin_send is NULL\nIt is recommend that you restart Trillian", "ERROR", MB_OK);
		return;
	}
	if(!message) // We kind of need this data >.>
		return;

	message_t close;
	trillianInitialize(close);
	close.window_id = message->window_id;
	
	message_t open;
	trillianInitialize(open);
	open.medium = message->medium;
	open.connection_id = NULL;
	open.name = message->name;
	open.location = NULL;
	open.window_id = 0;
	open.extra_information_size = 0;
	open.extra_information = NULL;

/* Notify the user that we ended the session */
	message_t notice;
	trillianInitialize(notice);
	notice.location = NULL;
	notice.connection_id = -1;
	notice.window_id = 0;
	notice.medium = message->medium;
	notice.name = message->name;
	notice.section = NULL;
	notice.extra_information = NULL;
	notice.extra_information_size = 0;
	notice.type = "information_standard";
	notice.text = NULL;
	notice.text_len = 0;


	plugin_send(MYGUID, "messageClose", &close); // closes the window
	plugin_send(MYGUID, "messageSend", &notice); // reopens the window

	MessageBox(NULL, "The session was automatically closed", "Trillian OTR", MB_OK);

	Sleep(100); // Prevent scope cleanup from happing too fast
}

/**
 *
 */
DWORD WINAPI threadedAddControls(PVOID arg)
{
	if(!plugin_send)
	{
		MessageBox(NULL, "plugin_send is NULL\nIt is recommend that you restart Trillian", "ERROR", MB_OK);
		return 0;
	}
	int ret;
	char msg[100];
	message_menu_t* mmt = reinterpret_cast<message_menu_t*>(arg);
	
	Sleep(1000);
	ret = plugin_send(MYGUID, "messageMenuAdd", mmt);
	
	if(ret < 0)
	{
		_snprintf_s(msg, 100, 99, "Error ret val = %d", ret);
		MessageBox(NULL, msg, "Error in messageMenuAdd", MB_OK);
	}

	totrMenuFree(mmt);

	return ret;
}

/**
 *
 */
DWORD WINAPI threadedInitSecureMem(PVOID arg)
{
	unsigned char* secmem = NULL;

	secmem = reinterpret_cast<unsigned char*>(gcry_random_bytes_secure(40, GCRY_STRONG_RANDOM));
	gcry_free(secmem);

	return 0;
}

/**
 *
 */
int liveWindowCallback(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	event_variables_t* evt = reinterpret_cast<event_variables_t*>(data);

	message_menu_t mmt;
	connection_enum_t cet;

	int ret = -1;

	if(windowID == EVENTS_EXECUTE)
	{
		trillianInitialize(cet);
		trillianInitialize(mmt);

		while(evt)
		{
			switch(TrillianTypes::stringToVariableType(evt->variable_name))
			{
				case TrillianTypes::cid:
					mmt.connection_id = *reinterpret_cast<int*>(evt->variable_data);
					break;
				case TrillianTypes::window_id:
					mmt.window_id = *reinterpret_cast<int*>(evt->variable_data);
					break;
				case TrillianTypes::displayname:
					mmt.name = reinterpret_cast<char*>(evt->variable_data);
					break;
				case TrillianTypes::location:
					mmt.location = reinterpret_cast<char*>(evt->variable_data);
					break;
				case TrillianTypes::medium:
					mmt.medium = reinterpret_cast<char*>(evt->variable_data);
					break;
			}
#if 0
			/* walk the variables. Look for our window_id */
			if(!stricmp(evt->variable_name, "cid"))
				mmt.connection_id = *reinterpret_cast<int*>(evt->variable_data);
			else if(!stricmp(evt->variable_name, "window_id"))
				mmt.window_id = *reinterpret_cast<int*>(evt->variable_data);
			else if(!stricmp(evt->variable_name, "displayname"))
				mmt.name = reinterpret_cast<char*>(evt->variable_data);
			else if(!stricmp(evt->variable_name, "location"))
				mmt.location = reinterpret_cast<char*>(evt->variable_data);
			else if(!stricmp(evt->variable_name, "medium"))
				mmt.medium = reinterpret_cast<char*>(evt->variable_data);
#endif

			evt = evt->next_evt;
		}
	}

#if 0
	/* New Console window created. Live connection! */
	if(!stricmp(mmt.name, "Console"))
	{
		/* Iterate the contexts and set our connection_id as approriate
		 * Don't forget to validate against conn->accountname (our name),
		 * and conn->protocol (the medium)
		 *
		 * Trillian only allows account names for a particular protocol
		 * to be added once. Bonus!
		 *
		 * Problem. This only occurs for medium plugins that connect after
		 * we have initialized.
		 */
	}
#endif

	return 0;
}

/**
 *
 */
int newWindowCallback(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	if(!plugin_send)
	{
		MessageBox(NULL, "plugin_send is NULL\nIt is recommend that you restart Trillian", "ERROR", MB_OK);
		return 0;
	}
	/* Do some stuff. This would be where we register the edit_sendText
	 * event, once we know the window ID. One nice thing, too. We can
	 * look up the context and prior data, if any exists, in order
	 * to sensitize buttons as they may be needed. Possibly, also,
	 * add the OTR buttons and hooks here.
	 */

	event_variables_t* evt = reinterpret_cast<event_variables_t*>(data);

	message_menu_t* mmt = NULL;
	menu_entry_t* met = NULL;
	edit_event_t eet;
	TotrAppData* appdata = NULL;
	char* account_name = NULL;
	char* username = NULL;
	char* displayname = NULL;
	char* medium = NULL;
	char* location = NULL;
	int connection_id = -1;
	int window_id = -1;
	int ret = -1;

	trillianInitialize(eet);

	while(evt)
	{
		/* walk the variables. Look for our data.
			* 
			* This routine can be used to trap whether or not
			* a new connection has been made via the fact a new
			* console window is made. Console windows, when shown,
			* do not trigger this event again, so we can safely
			* assume that this means we have made a new connection.
			*/
		switch(TrillianTypes::stringToVariableType(evt->variable_name))
		{
			case TrillianTypes::cid:
				connection_id = *reinterpret_cast<int*>(evt->variable_data);
				break;
			case TrillianTypes::window_id:
				window_id = *reinterpret_cast<int*>(evt->variable_data);
				break;
			case TrillianTypes::displayname:
				displayname = reinterpret_cast<char*>(evt->variable_data);
				break;
			case TrillianTypes::address:
				username = reinterpret_cast<char*>(evt->variable_data);
				break;
			case TrillianTypes::location:
				location = reinterpret_cast<char*>(evt->variable_data);
				break;
			case TrillianTypes::medium:
				medium = reinterpret_cast<char*>(evt->variable_data);
				break;
		}

#if 0
		if(!stricmp(evt->variable_name, "cid"))
			connection_id = *reinterpret_cast<int*>(evt->variable_data);
		else if(!stricmp(evt->variable_name, "window_id"))
			window_id = *reinterpret_cast<int*>(evt->variable_data);
		else if(!stricmp(evt->variable_name, "displayname"))
			displayname = reinterpret_cast<char*>(evt->variable_data);
		else if(!stricmp(evt->variable_name, "address"))
			username = reinterpret_cast<char*>(evt->variable_data);
		else if(!stricmp(evt->variable_name, "location"))
			location = reinterpret_cast<char*>(evt->variable_data);
		else if(!stricmp(evt->variable_name, "medium"))
			medium = reinterpret_cast<char*>(evt->variable_data);
#endif

		evt = evt->next_evt;
	}

	/* Does this protocol even support OTR? If not, bail */
	if(!proto_supports_otr(medium))
		return 0;

	/* Look up my own account name. I need to add the window_id to the proper context */
	account_name = accountNameLookup(medium, connection_id);

	if(!stricmp(displayname, "Console"))
	{
		/* Iterate our contexts. Fill in the connection_id. */

		/* Is this necessary here? We do this in message_broadcast too,
			* since any plugin brought up before we were did not issue
			* us a connection notice. Perhaps enumerate open connections
			* once we start up?
			*/
		ConnContext* conn = userstate->context_root;

		while(conn)
		{
			if(!stricmp(conn->accountname, account_name) &&
				!stricmp(conn->protocol, medium) &&	conn->app_data) 
			{
				appdata = reinterpret_cast<TotrAppData*>(conn->app_data);
				appdata->connection_id = connection_id;
			}

			conn = conn->next;
		}

		/* Nothing really to do, since this is just a console window.
			* Call it good that we updated our contexts, and run screaming.
			* Don't forget to clean up after ourselves!
			*/

		free(account_name);
		account_name = NULL;
		return 0;
	} 

	ConnContext* conn = NULL;
	TotrAppData eopts;
	int added = 0;

	eopts.connection_id = connection_id;
	eopts.window_id = window_id;

	conn = otrl_context_find(userstate, username, account_name, medium, 
		1, &added, totrNewContext, &eopts);

	if(!added && conn && conn->app_data)
	{
		appdata = reinterpret_cast<TotrAppData*>(conn->app_data);
		appdata->connection_id = connection_id;
		appdata->window_id = window_id;
	}
	else if(conn && conn->app_data)
		appdata = reinterpret_cast<TotrAppData*>(conn->app_data);
	

	/* If we don't have any application data from our context search, we probably don't
	 * have a context for this. If that's the case, there's nothing we really need
	 * to do yet. Avoid our menus and registering our edit event.
	 */
	if(!appdata)
	{
		free(account_name);
		account_name = NULL;
		return 0;
	}

	/* Add the menu bars here. Right now, because the right click destroys all prior
		* entries, add it as a bar at top
		*/
	mmt = totrGenerateMenu(connection_id, window_id, medium, username, location);

	if(appdata->edit_event_id == -1)
	{
		/* Set up our edit event if we're not a console window,
			* and if we haven't already made one in otrl_context_find
			*/
		eet.window_id = window_id;

		eet.callback = totrEncryptMessage;
		eet.event = "edit_sendText";
		eet.data = appdata;

		int ret = plugin_send(MYGUID, "editEventRegister", &eet);

		if(!eet.edit_event_id)
		{
			MessageBox(NULL, "Error registering edit event.", "Error", IDOK);
			return 0;
		}

		appdata->edit_event_id = eet.edit_event_id;
	}

	{
		HANDLE hThread;
		DWORD threadId;
		hThread = CreateThread(NULL, 0, threadedAddControls, (PVOID)mmt, 0, &threadId);
	}

	free(account_name);
	account_name = NULL;
	return 0;
}

/**
 *
 */
int deadWindowCallback(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	if(!plugin_send)
	{
		MessageBox(NULL, "plugin_send is NULL\nIt is recommend that you restart Trillian", "ERROR", MB_OK);
		return 0;
	}
	struct event_variables_t* evt = reinterpret_cast<struct event_variables_t*>(data);
	struct message_menu_t mmt;
	struct edit_event_t eet;
	char* medium = NULL;
	char* location = NULL;
	char* name = NULL;
	char* display_name = NULL;
	int connection_id = -1;
	int window_id = -1;
	int ret = -1;

	trillianInitialize(eet);
	trillianInitialize(mmt);

	while(evt)
	{
		switch(TrillianTypes::stringToVariableType(evt->variable_name))
		{
			case TrillianTypes::cid:
				connection_id = *reinterpret_cast<int*>(evt->variable_data);
				break;
			case TrillianTypes::window_id:
				window_id = *reinterpret_cast<int*>(evt->variable_data);
				break;
			case TrillianTypes::address:
				name = reinterpret_cast<char*>(evt->variable_data);
				break;
			case TrillianTypes::displayname:
				display_name = reinterpret_cast<char*>(evt->variable_data);
				break;
			case TrillianTypes::location:
				location = reinterpret_cast<char*>(evt->variable_data);
				break;
			case TrillianTypes::medium:
				medium = reinterpret_cast<char*>(evt->variable_data);
				break;
		}

#if 0
		/* walk the variables. Look for our window_id */
		if(!stricmp(evt->variable_name, "cid"))
			connection_id = *reinterpret_cast<int*>(evt->variable_data);
		else if(!stricmp(evt->variable_name, "window_id"))
			window_id = *reinterpret_cast<int*>(evt->variable_data);
		else if(!stricmp(evt->variable_name, "address"))
			name = reinterpret_cast<char*>(evt->variable_data);
		else if(!stricmp(evt->variable_name, "displayname"))
			display_name = reinterpret_cast<char*>(evt->variable_data);
		else if(!stricmp(evt->variable_name, "location"))
			location = reinterpret_cast<char*>(evt->variable_data);
		else if(!stricmp(evt->variable_name, "medium"))
			medium = reinterpret_cast<char*>(evt->variable_data);
#endif

		evt = evt->next_evt;
	}

	/* If the destroyed window is a console window, that means our connection is
		* completely terminated. We need to iterate through the contexts and set all
		* of the connection_id fields to -1, to indicate we're disconnected.
		*
		* FIXME: Handle resetting connection_id and window_id vars in app_data.
		*/
	if(!stricmp(name, "Console"))
	{
		/* Our dead window is a console window. Connection disconnected */

		ConnContext* conn = userstate->context_root;
		TotrAppData* appdata = NULL;

		while(conn)
		{
			appdata = reinterpret_cast<TotrAppData*>(conn->app_data);

			if(appdata && appdata->connection_id == connection_id)
				appdata->connection_id = -1;

			conn = conn->next;
		}
	}
	else
	{
		/* Our dead window is a message window. */
		if(!proto_supports_otr(medium)) return 0;

		char* account_name = accountNameLookup(medium, connection_id);
		if(!account_name) return 0;
		ConnContext* conn;
		try
		{
			conn = otrl_context_find(userstate, name, 
				account_name, medium, 0, NULL, NULL, NULL);
		}
		catch(...)
		{
			return 0;
		}
		if(!conn) return 0;
		TotrAppData* appdata = reinterpret_cast<TotrAppData*>(conn->app_data);

		if(totr_config.GetAutoCloseWithWindow() && conn->msgstate == OTRL_MSGSTATE_ENCRYPTED)
		{
			totrTerminateSession(conn); // Close the session if user wants us to
			message_t close;
			trillianInitialize(close);
			close.window_id = -1;
			close.display_name = conn->accountname;
			close.medium = conn->protocol;
			plugin_send(MYGUID, "messageClose", &close); // Close the window that opens up with the message
		}


		if(conn && appdata)
		{
			if(appdata->edit_event_id != -1)
			{
				eet.edit_event_id = appdata->edit_event_id;

				plugin_send(MYGUID, "editEventUnregister", &eet);

				appdata->edit_event_id = -1;			
			}

			if(appdata->window_id == window_id)
				appdata->window_id = -1;
		}

		mmt.connection_id = connection_id;
		mmt.window_id = window_id;
		mmt.medium = medium;
		mmt.name = name;
		mmt.location = location;

		/* Our type and name */
		mmt.menu_type = MENU_TYPE_BAR;
		mmt.menu_name = "Trillian OTR";

		plugin_send(MYGUID, "messageMenuRemove", &mmt);
	}

	return 0;
}

/**
 *
 */

/*
 * This is never called in the event a new connection is created nor when it is closed --
 * And I don't mean 'marked offline', but actually closed. However, even with no windows
 * open, a status window is created. Can this be used to track connections being created
 * and destroyed, by the state of the status window?
 */
int connectionChangeCallback(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	event_variables_t* evt = reinterpret_cast<event_variables_t*>(data);
	char* name = NULL;
	char* medium = NULL;
	char* status = NULL;
	int connection_id = -1;
	ConnContext *conn = userstate->context_root;

	while(evt)
	{
		switch(TrillianTypes::stringToVariableType(evt->variable_name))
		{
			case TrillianTypes::cid:
				connection_id = *reinterpret_cast<int*>(evt->variable_data);
				break;
			case TrillianTypes::displayname:
				name = reinterpret_cast<char*>(evt->variable_data);
				break;
			case TrillianTypes::status:
				status = reinterpret_cast<char*>(evt->variable_data);
				break;
			case TrillianTypes::medium:
				medium = reinterpret_cast<char*>(evt->variable_data);
				break;
		}

#if 0
		/* walk the variables. Look for our data */
		if(!stricmp(evt->variable_name, "cid"))
			connection_id = *reinterpret_cast<int*>(evt->variable_data);
		else if(!stricmp(evt->variable_name, "displayname"))
			name = reinterpret_cast<char*>(evt->variable_data);
		else if(!stricmp(evt->variable_name, "status"))
			status = reinterpret_cast<char*>(evt->variable_data);
		else if(!stricmp(evt->variable_name, "medium"))
			medium = reinterpret_cast<char*>(evt->variable_data);
#endif

		evt = evt->next_evt;
	}

	return 0;
}

/**
 *
 */
void handleIncomingMessage(message_t* msg)
{
	char* newmessage = NULL;
    OtrlTLV* tlvs = NULL;
    OtrlTLV* tlv = NULL;
	int res;
	msg->text_len = static_cast<int>(strlen(msg->text) + 1);
			
	/* Record some stateful information about our connections?
	* After all, we get connection_id, but not our own user name
	*/
	TotrAppData eopts;
	char* account_name = accountNameLookup(msg->medium, msg->connection_id);

	if(!account_name) return;

	eopts.connection_id = msg->connection_id;
	eopts.window_id = msg->window_id;
	eopts.edit_event_id = 0;

	
	res = otrl_message_receiving(userstate, &totr_ops, msg,
		account_name, msg->medium, msg->name, msg->text,
		&newmessage, &tlvs, totrNewContext, &eopts);

	if(newmessage)  // If there is a message, then it is either a plaintext or a report message
	{
		if(totr_config.GetSaveHistory())
			if(!totr_config.appendHistory(newmessage,msg->medium,msg->name,msg->name))
				MessageBox(NULL,"Could not save history.","ERROR",MB_OK);
		/* Copy our decoded message to our buffer. */
		strncpy_s(msg->text, msg->text_len, newmessage, _TRUNCATE); // Getting more secure :D
		otrl_message_free(newmessage);
	}

	if(res) 
	{
			
		OtrlMessageType mtype = otrl_proto_message_type(msg->text);
		ConnContext* conn = otrl_context_find(userstate, msg->name, 
			account_name, msg->medium, 0, NULL, NULL, NULL);
		if(mtype == OTRL_MSGTYPE_TAGGEDPLAINTEXT)
			/* Covert OTR requests are handled immediately by the OTR
				* library on our behalf. No need to respond to this directly.
				*/
				;
		else if(mtype == OTRL_MSGTYPE_DATA && conn->msgstate == OTRL_MSGSTATE_PLAINTEXT)
		{
			msg->type = "information_standard";
			_snprintf_s(msg->text, msg->text_len, _TRUNCATE, "%s has sent an unexpected encrypted message that cannot be decrypted.",
				msg->name);
		}
		else if(mtype != OTRL_MSGTYPE_ERROR)
		{
			msg->type = "information_standard";
			if(conn)
			{
				totrHandleSMP(conn, tlvs); // Watch the SMP state

				switch(conn->msgstate)
				{
					case OTRL_MSGSTATE_PLAINTEXT:
						switch(conn->auth.authstate)
						{
							case OTRL_AUTHSTATE_AWAITING_DHKEY:
								_snprintf_s(msg->text, msg->text_len, _TRUNCATE, "<b>%s has requested a private OTR session.</b>",
								msg->name);
								break;
							case OTRL_AUTHSTATE_AWAITING_SIG:
							case OTRL_AUTHSTATE_AWAITING_REVEALSIG:
								strncpy_s(msg->text, msg->text_len, "<b>Negotiating session keys.</b>", _TRUNCATE);
								break;
							default:
								strncpy_s(msg->text, msg->text_len, "Unknown OTR Control Message", _TRUNCATE);
								break;
						}
						break;
					case OTRL_MSGSTATE_ENCRYPTED:
						switch(conn->auth.authstate)
						{
							case OTRL_AUTHSTATE_NONE:
								_snprintf_s(msg->text, msg->text_len, _TRUNCATE, "<b>%s session with %s established.</b>", 
									(totrTrustedFingerprint(conn->active_fingerprint)?"Private":"Unverified"), 
									msg->name);
								break;
							case OTRL_AUTHSTATE_AWAITING_DHKEY:
								_snprintf_s(msg->text, msg->text_len, _TRUNCATE, "<b>%s has requested a refresh of the existing OTR session.</b>",
									msg->name);
								break;
							case OTRL_AUTHSTATE_AWAITING_SIG:
								strncpy_s(msg->text, msg->text_len, "<b>Negotiating session keys.</b>", _TRUNCATE);
								break;
							case OTRL_AUTHSTATE_AWAITING_REVEALSIG:
								_snprintf_s(msg->text, msg->text_len, _TRUNCATE, "<b>Requesting session refresh with %s.</b>", msg->name);
								break;
							default:
								strncpy_s(msg->text, msg->text_len, "Unknown OTR Control Message", _TRUNCATE);
								break;
						}
						break;
					case OTRL_MSGSTATE_FINISHED:
						/* How do we put this back into a plaintext state, or
							* do we need to? 
							*/
						if(totr_config.GetAutoClose())
						{
							_snprintf_s(msg->text, msg->text_len, _TRUNCATE, "<b>%s session with %s lost and automatically closed.</b>",
								(totrTrustedFingerprint(conn->active_fingerprint) ? "Private" : "Unverified"), 
								msg->name);
							otrl_message_disconnect(userstate, &totr_ops, NULL, account_name, msg->medium, msg->name);
							reopenMessageWindow(msg);
						}
						else
						{
							_snprintf_s(msg->text, msg->text_len, _TRUNCATE, "<b>%s session with %s lost. You should end or refresh the conversation before sending again.</b>",
								(totrTrustedFingerprint(conn->active_fingerprint) ? "Private" : "Unverified"), 
								msg->name);
						}
						break;
					default:
						strncpy_s(msg->text, msg->text_len, "Unknown OTR Control Message", _TRUNCATE);
						break;
				}
			}
			else
			{
				strncpy_s(msg->text, msg->text_len, "Unknown OTR Control Message", _TRUNCATE);
			}
		}
	}

	otrl_tlv_free(tlvs);
	tlvs = NULL;
	free(account_name);
	account_name = NULL;
}

/**
 *
 */
void handleOutgoingMessage(message_t* msg)
{
	msg->text_len = static_cast<int>(strlen(msg->text) + 1);

	/* Can we determine if we're sending an outbound message? */
	OtrlMessageType mtype = otrl_proto_message_type(msg->text);
	char* account_name = accountNameLookup(msg->medium, msg->connection_id);
	TotrAppData* appdata = NULL;
	ConnContext* conn = otrl_context_find(userstate, msg->name, 
		account_name, msg->medium, 0, NULL, NULL, NULL);

	free(account_name);
	account_name = NULL;

	if(conn)
		appdata = reinterpret_cast<TotrAppData*>(conn->app_data);
	ConnContext* pconn = NULL;
	switch(mtype)
	{
		case OTRL_MSGTYPE_NOTOTR:
			return;
			break;
		case OTRL_MSGTYPE_DATA:
		case OTRL_MSGTYPE_TAGGEDPLAINTEXT:
			pconn = userstate->context_root;

			while(pconn)
			{
				if(pconn->app_data)
				{
					appdata = reinterpret_cast<TotrAppData*>(conn->app_data);

					if(appdata->window_id == msg->window_id)
						break;
				}

				pconn = conn->next;
			}

			if(appdata->last_msg)
			{
				strncpy_s(msg->text, msg->text_len, appdata->last_msg, _TRUNCATE);
				memset(appdata->last_msg, 0, appdata->last_msg_len);
			}
			break;
		default:
			msg->type = "information_standard";
			strncpy_s(msg->text, msg->text_len, "Unknown OTR Control Message", _TRUNCATE);
			break;
	}
	if(appdata)
	{
		free(appdata->last_msg);
		appdata->last_msg = NULL;
		appdata->last_msg_len = 0;
	}
}

/**
 *
 */
int broadCallback(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	if(!plugin_send)
	{
		MessageBox(NULL, "plugin_send is NULL\nIt is recommend that you restart Trillian", "ERROR", MB_OK);
		return 0;
	}
	message_t* msg = reinterpret_cast<message_t*>(data);
	if(strcmp(event, "message_broadcast") != 0)
		return 0;

	if(!strcmp(msg->type, "incoming_privateMessage"))
	{
		handleIncomingMessage(msg);
	}
	else if (!strcmp(msg->type, "outgoing_privateMessage"))
	{
		handleOutgoingMessage(msg);
	}
	return 0;
}

void mainLoad(plugin_info_t* pi)
{
	const bool firstTime = !plugin_send;
	HRSRC src;
	if(!pi)
		return;

	/* Extract what we need and store locally. */
	tmp_config = NULL;
	totr_config.SetHomeDirectory(pi->config_directory);

	/* Must initialize this before we can load our config */
	if(reinterpret_cast<int>(pi->plugin_send) > 32)
		plugin_send = pi->plugin_send;
	if(!plugin_send)
	{
		MessageBox(NULL, "plugin_send is NULL\nIt is recommend that you restart Trillian", "ERROR", MB_OK);
	}

	/* Load our configuration and initialize our version */
	if(firstTime && reinterpret_cast<int>(pi->plugin_send) > 32)
		totr_config.LoadConfig();

	/* Fill in plugin information.
		*/
	strncpy_s(pi->guid, 128, MYGUID, _TRUNCATE); 
	strncpy_s(pi->name, 256, MYNAME, _TRUNCATE); 
	strncpy_s(pi->company, 256, COMPANY, _TRUNCATE); 
	strncpy_s(pi->version, 64, totr_config.GetShortVersion(), _TRUNCATE); 
	strncpy_s(pi->description, 1024, DESCRIPTION, _TRUNCATE);


	/* Gather up and fill in our icon info
		* Need to come up with some better icons
		* Need to make the icons actually work
		*/
	src = FindResource(hInstance, MAKEINTRESOURCE(IDB_SMALL_ICO), _T("PNG"));
	HGLOBAL hbytes = LoadResource(hInstance, src);
	LPVOID pdata = LockResource(hbytes);
	icon_32_size = SizeofResource(hInstance, src);
	icon_32 = reinterpret_cast<unsigned char*>(malloc(icon_32_size));
	memcpy(icon_32, pdata, icon_32_size);
	pi->png_image_32 = icon_32;
	pi->png_image_32_len = icon_32_size;
	
	src = FindResource(hInstance, MAKEINTRESOURCE(IDB_LARGE_ICO), _T("PNG"));
	HGLOBAL hbytesb = LoadResource(hInstance, src);
	LPVOID pdatab = LockResource(hbytesb);
	icon_48_size = SizeofResource(hInstance, src);
	icon_48 = reinterpret_cast<unsigned char*>(malloc(icon_48_size));
	memcpy(icon_48, pdatab, icon_48_size);
	pi->png_image_48 = icon_48;
	pi->png_image_48_len = icon_48_size;

	if(reinterpret_cast<int>(plugin_send) > 32)
		plugin_send(MYGUID, "pluginInformationUpdate", pi);

	if(!firstTime && reinterpret_cast<int>(pi->plugin_send) > 32)
	{
		mainStart();
	}

	/* Initialize our WTL library */
	//_Module.Init(NULL, hInstance);
}

void mainStart()
{
	/* Check if we have plugin_send */
	if(!plugin_send)
	{
		MessageBox(NULL, "plugin_send is NULL\nIt is recommend that you restart Trillian", "ERROR", MB_OK);
		return;
	}

	/* Bring up the OTR library */
	OTRL_INIT;
	userstate = otrl_userstate_create();
	otrl_privkey_read(userstate, totr_config.GetPrivKeyFilename());

			

	otrl_privkey_read_fingerprints(userstate, totr_config.GetStoreFilename(), 
				                    totrNewContext, NULL);
	plugin_prefs_t pp;
	plugin_prefs_entry_t ppe1;
	plugin_prefs_entry_t ppe2;
	message_broadcast_t mbt;
	event_connect_t ect;

	trillianInitialize(pp);
	trillianInitialize(ppe1);
	trillianInitialize(ppe2);

	trillianInitialize(mbt);
	trillianInitialize(ect);
		
	/* Request some random memory to populate our data pool. */
	{
		HANDLE hThread;
		DWORD threadId;
		hThread = CreateThread(NULL, 0, threadedInitSecureMem, 0, 0, &threadId);
	}
	/* Register our preference dialog structure with Trillian */
	ppe2.next = NULL;
	ppe2.sub_name = "OTR Config";

	ppe1.next = &ppe2;
	ppe1.sub_name = "OTR General";

	pp.enabled = 1; 
	pp.pref_name = "Trillian OTR"; 
	pp.sub_entry = &ppe1; 

	plugin_send(MYGUID, "prefsInitialize", &pp); 

	/* This works to work with modifying inbound messages
		* however, outbound messages must be modified through
		* the event edit_sendText
		*/
	if(totr_state.GetState(TOTR_STATE_BROADCAST_ID) == -1)
	{
		mbt.callback = broadCallback;

		totr_state.SetState(TOTR_STATE_BROADCAST_ID, 
			plugin_send(MYGUID, "messageBroadcastEnable", &mbt));
	}

	/* Register for edit_sendText event here 
		*   - edit_sendText events must be called in the
		*     context of a window_id. Therefor, this is
		*     context-specific and cannot be placed
		*     here. What we should register here is
		*     an event that listens to unmuteable
		*     window creation events.
		*
		* We could do this in a global fashion by using
		*     window_id = -1, but for now, they are
		*     registered on a per-window basis, excluding
		*     console windows, and populated by the
		*     newWindowCallback function.
		*/
	if(totr_state.GetState(TOTR_STATE_NEW_WINDOW_EVENT_ID) == -1)
	{
		ect.type = "Message: IM Window Initial View";
		ect.callback = newWindowCallback;

		totr_state.SetState(TOTR_STATE_NEW_WINDOW_EVENT_ID, 
			plugin_send(MYGUID, "eventsConnect", &ect));
	}

	/* Register for window distruction events. This
		*     allows us to keep all our contexts up to
		*     date. Console windows being destroyed means that
		*     all those encrypted links must be disconnected.
		*/
	trillianInitialize(ect);

	if(totr_state.GetState(TOTR_STATE_DEAD_WINDOW_EVENT_ID) == -1)
	{
		ect.type = "Message: IM Window Destruction";
		ect.callback = deadWindowCallback;

		totr_state.SetState(TOTR_STATE_DEAD_WINDOW_EVENT_ID, 
			plugin_send(MYGUID, "eventsConnect", &ect));
	}

	/* The idea behind registering for this event is to try
		*      to send some disconnects before the link is actually
		*      closed. Only question is, is this called before the
		*      connection is closed or after.
		*/
	trillianInitialize(ect);

	if(totr_state.GetState(TOTR_STATE_CONNECT_EVENT_ID) == -1)
	{
		ect.type = "Connection: Change";
		ect.callback = connectionChangeCallback;

		totr_state.SetState(TOTR_STATE_CONNECT_EVENT_ID, 
			plugin_send(MYGUID, "eventsConnect", &ect));
	}

	totrPopulateKnownConnections();
}

void mainStop()
{
	/* Check if we have plugin_send */
	if(!plugin_send)
	{
		//MessageBox(NULL, "plugin_send is NULL\nIt is recommend that you restart Trillian", "ERROR", MB_OK);
		return;
	}
	/* Clean up our userstate */
	ConnContext* conn = userstate->context_root;
	while(conn)
	{
		TotrAppData* appdata = reinterpret_cast<TotrAppData*>(conn->app_data);
		if(!appdata)
		{
			conn = conn->next;
			continue;
		}
		//struct message_menu_t mmt; // This is unnecessary, we have a right click menu

		//trillianInitialize(mmt);

		/* For every encrypted session, 
			*    send disconnect string 
			*/
		if(conn->msgstate == OTRL_MSGSTATE_ENCRYPTED)
		{
#if 0
			if(!appdata)
				break;
#endif
			message_t msg;
			trillianInitialize(msg);

			msg.connection_id = appdata->connection_id;

			otrl_message_disconnect(userstate, &totr_ops, &msg,
				conn->accountname, conn->protocol, conn->username);
		}
		
#if 0
		if(!appdata)
		{
			conn = conn->next;
			continue;
		}
#endif

		if(appdata->edit_event_id != -1)
		{
			edit_event_t eet;

			trillianInitialize(eet);

			eet.edit_event_id = appdata->edit_event_id;

			plugin_send(MYGUID, "editEventUnregister", &eet);
		}
#if 0
		if(appdata->connection_id != -1 && appdata->window_id != -1)
		{
			mmt.connection_id = appdata->connection_id;
			mmt.window_id = appdata->window_id;
			mmt.medium = conn->protocol;
			mmt.name = conn->username;
			mmt.location = NULL;

			/* Our type and name */
			mmt.menu_type = MENU_TYPE_RIGHTCLICK;
			mmt.menu_name = "Trillian OTR";

			plugin_send(MYGUID, "messageMenuRemove", &mmt);
		}
#endif

		conn->app_data_free(appdata);
		conn->app_data = NULL;
		conn = conn->next;
	}

	totr_config.SaveConfig();
	otrl_privkey_forget_all(userstate);
	userstate->context_root = NULL;
	otrl_userstate_free(userstate);
	userstate = NULL;
	totr_state.StopAllEvents();

	free(icon_32);
	free(icon_48);
	icon_32 = NULL;
	icon_48 = NULL;
}

void mainUnload()
{
	if(prefDlg)
	{
		prefDlg->DestroyWindow();
		delete prefDlg;
		prefDlg = NULL;
	}

//	if(tmp_config) // This check is unneccessary
//	{
		delete tmp_config;
		tmp_config = NULL;
//	}
	
	//plugin_send = NULL;

	//_Module.Term();
}

void mainPrefsShow(plugin_prefs_show_t* pps)
{
	if(!plugin_send)
	{
		MessageBox(NULL, "plugin_send is NULL\nIt is recommend that you restart Trillian", "ERROR", MB_OK);
		return;
	}
	int resource = 0;
	if(strcmp(pps->pref_name, "Trillian OTR"))
		return;

	strcpy_s(pps->prefs_info.name, 128, "Trillian Off-The-Record: Risen");
	strcpy_s(pps->prefs_info.description, 1024, "Off the Record messaging support for Trillian.");

	if (!tmp_config)
		tmp_config = new TotrConfig(totr_config);

	if(pps->sub_entry == NULL)
		resource = IDD_PREFS_DIALOG;
	else if(!strcmp(pps->sub_entry, "OTR General"))
		resource = IDD_PREFS_GENERAL;
	else if(!strcmp(pps->sub_entry, "OTR Config"))
		resource = IDD_PREFS_CONFIG;

	if(pps->show == TRILLIAN_PREFS_SHOW)
	{
		HWND hwnd = NULL;

		if(prefDlg && prefDlg->GetResource() != resource) 
		{
			prefDlg->DestroyWindow();
			delete prefDlg;
			prefDlg = NULL;
		}

		if(prefDlg == NULL)
		{
			switch(resource)
			{
				case IDD_PREFS_DIALOG:
					prefDlg = reinterpret_cast<CDialogPrefs*>(new CAboutDlg);
					break;
				case IDD_PREFS_GENERAL:
					prefDlg = reinterpret_cast<CDialogPrefs*>(new CGeneralDlg);
					break;
				case IDD_PREFS_CONFIG:
					prefDlg = reinterpret_cast<CDialogPrefs*>(new CConfigDlg);
					break;
				default:
					break;
			}
                
			hwnd = prefDlg->Create(pps->hwnd);
			prefDlg->SetWindowPos(hwnd, pps->x, pps->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}

		prefDlg->ShowWindow(SW_SHOW);
	}
	else if(pps->show == TRILLIAN_PREFS_HIDE && prefDlg && prefDlg->GetResource() == resource)
	{
		prefDlg->ShowWindow(SW_HIDE);
	}
}

/**
 *
 */
void mainPrefsAction(plugin_prefs_action_t* ppa)
{
	switch (ppa->type)
	{
		case TRILLIAN_PREFS_APPLY:
			totr_config = *tmp_config;
			totr_config.SaveConfig();
			break;
		case TRILLIAN_PREFS_OK:
			totr_config = *tmp_config;
			totr_config.SaveConfig(); // No break, because we want it to kill window as well
		case TRILLIAN_PREFS_CANCEL:
			if(prefDlg != NULL)  // TODO: test to see if this is ever actually NULL.
			{
				prefDlg->DestroyWindow();
				delete prefDlg;
				prefDlg = NULL;
			}

			
			//if(tmp_config)  This statement is uneccessary.  delete/free already do this check.
			//{
				delete tmp_config;
				tmp_config = NULL;
			//}

			break;
	}
}

/**
 * Export function that is called by Trillian.
 */
//extern "C" {
int __declspec(dllexport)plugin_version(void)
{
	return 3;
}

/**
 * Export function that is called by Trillian.
 * This function contains response to Trillian events.
 */
int __declspec(dllexport)plugin_main(char* event, void* data) 
{
	switch(TrillianTypes::stringToEventType(event))
	{
		case TrillianTypes::load:
			mainLoad(reinterpret_cast<plugin_info_t*>(data));
			break;
		case TrillianTypes::initialLoad:
			break;
		case TrillianTypes::start:
			mainStart();
			break;
		case TrillianTypes::stop:
			mainStop();
			break;
		case TrillianTypes::unload:
			mainUnload();
			break;
		case TrillianTypes::finalUnload:
			break;
		case TrillianTypes::prefsShow:
			mainPrefsShow(reinterpret_cast<plugin_prefs_show_t*>(data));
			break;
		case TrillianTypes::prefsAction:
			mainPrefsAction(reinterpret_cast<plugin_prefs_action_t*>(data));
			break;
	}
#if 0
	if (!_stricmp(event,"load")) { 

	   struct plugin_info_t *pi = (struct plugin_info_t *)data;
	   HRSRC src;

		if (pi) {

			/* Extract what we need and store locally.
			 */
			tmp_config = NULL;
			totr_config.SetHomeDirectory(pi->config_directory);

			/* Must initialize this before we can load our config */
			plugin_send = pi->plugin_send;

			/* Load our configuration and initialize our version */
			totr_config.LoadConfig();

			/* Fill in plugin information.
			 */
			strcpy(pi->guid, MYGUID); 
			strcpy(pi->name, MYNAME); 
			strcpy(pi->company, COMPANY); 
			strcpy(pi->version, totr_config.GetShortVersion()); 
			strcpy(pi->description, DESCRIPTION);

			/* Gather up and fill in our icon info
			 * Need to come up with some better icons
			 */
			src = ::FindResource(hInstance, MAKEINTRESOURCE(IDB_SMALL_ICO), _T("PNG"));
			pi->png_image_32 = (unsigned char *) ::LockResource(::LoadResource(hInstance, src));
			pi->png_image_32_len = (int) ::SizeofResource(hInstance, src);
         
			src = ::FindResource(hInstance, MAKEINTRESOURCE(IDB_LARGE_ICO), _T("PNG"));
			pi->png_image_48 = (unsigned char *) ::LockResource(::LoadResource(hInstance, src));
			pi->png_image_48_len = (int) ::SizeofResource(hInstance, src);

			

			/* Initialize our WTL library */
			_Module.Init(NULL, hInstance);
	   } 
	} else if (!_stricmp(event, "initialLoad")) {
	} else if (!_stricmp(event, "start")) {
	/* Bring up the OTR library */
			OTRL_INIT;
			userstate = otrl_userstate_create();
			otrl_privkey_read(userstate, totr_config.GetPrivKeyFilename());

			

			otrl_privkey_read_fingerprints(userstate, totr_config.GetStoreFilename(), 
				                           totrNewContext, NULL);
		struct plugin_prefs_t		pp;
		struct plugin_prefs_entry_t	ppe1;
		struct plugin_prefs_entry_t	ppe2;
		struct message_broadcast_t	mbt;
		struct event_connect_t		ect;

		trillianInitialize(pp);
		trillianInitialize(ppe1);
		trillianInitialize(ppe2);

		trillianInitialize(mbt);
		trillianInitialize(ect);
		
		/* Request some random memory to populate our data pool. */
		{
			HANDLE hThread;
			DWORD threadId;
			hThread = CreateThread(NULL, 0, threadedInitSecureMem, 0, 0, &threadId);
		}
		/* Register our preference dialog structure with Trillian */
		ppe2.next						= NULL;
		ppe2.sub_name					= "OTR Config";

		ppe1.next						= &ppe2;
		ppe1.sub_name					= "OTR General";

		pp.enabled						= 1; 
		pp.pref_name					= "Trillian OTR"; 
		pp.sub_entry					= &ppe1; 

		plugin_send(MYGUID, "prefsInitialize", &pp); 

		/* This works to work with modifying inbound messages
		 * however, outbound messages must be modified through
		 * the event edit_sendText
		 */
		if (totr_state.GetState(TOTR_STATE_BROADCAST_ID) == -1) {
			mbt.callback = broadCallback;

			totr_state.SetState(TOTR_STATE_BROADCAST_ID, 
				plugin_send(MYGUID, "messageBroadcastEnable", &mbt));

		}

		/* Register for edit_sendText event here 
		 *   - edit_sendText events must be called in the
		 *     context of a window_id. Therefor, this is
		 *     context-specific and cannot be placed
		 *     here. What we should register here is
		 *     an event that listens to unmuteable
		 *     window creation events.
		 *
		 * We could do this in a global fashion by using
		 *     window_id = -1, but for now, they are
		 *     registered on a per-window basis, excluding
		 *     console windows, and populated by the
		 *     newWindowCallback function.
		 */
		if (totr_state.GetState(TOTR_STATE_NEW_WINDOW_EVENT_ID) == -1) {
			ect.type = "Message: IM Window Initial View";
			ect.callback = newWindowCallback;

			totr_state.SetState(TOTR_STATE_NEW_WINDOW_EVENT_ID, 
				plugin_send(MYGUID, "eventsConnect", &ect));
		}

		/* Register for window distruction events. This
		 *     allows us to keep all our contexts up to
		 *     date. Console windows being destroyed means that
		 *     all those encrypted links must be disconnected.
		 */
		trillianInitialize(ect);

		if (totr_state.GetState(TOTR_STATE_DEAD_WINDOW_EVENT_ID) == -1) {
			ect.type = "Message: IM Window Destruction";
			ect.callback = deadWindowCallback;

			totr_state.SetState(TOTR_STATE_DEAD_WINDOW_EVENT_ID, 
				plugin_send(MYGUID, "eventsConnect", &ect));
		}

		/* The idea behind registering for this event is to try
		 *      to send some disconnects before the link is actually
		 *      closed. Only question is, is this called before the
		 *      connection is closed or after.
		 */
		trillianInitialize(ect);

		if (totr_state.GetState(TOTR_STATE_CONNECT_EVENT_ID) == -1) {
			ect.type = "Connection: Change";
			ect.callback = connectionChangeCallback;

			totr_state.SetState(TOTR_STATE_CONNECT_EVENT_ID, 
				plugin_send(MYGUID, "eventsConnect", &ect));
		}

		totrPopulateKnownConnections();

	} else if (!_stricmp(event,"stop") && plugin_send) {
		/* Clean up our userstate
		 */
		ConnContext *conn = userstate->context_root;
		while (conn)
		{
			TotrAppData *appdata = (TotrAppData *) conn->app_data;
			struct message_menu_t mmt;

			trillianInitialize(mmt);

			/* For every encrypted session, 
			 *    send disconnect string 
			 */
			if (conn->msgstate == OTRL_MSGSTATE_ENCRYPTED) {
				
				if (appdata) {
					struct message_t msg;
					trillianInitialize(msg);

					msg.connection_id = appdata->connection_id;

               		otrl_message_disconnect(userstate, &totr_ops, &msg,
						conn->accountname, conn->protocol, conn->username);
				}
			}

			if (appdata && appdata->edit_event_id != -1) {
				struct edit_event_t eet;

				trillianInitialize(eet);

				eet.edit_event_id = appdata->edit_event_id;

				plugin_send(MYGUID, "editEventUnregister", &eet);
			}

			if (appdata && appdata->connection_id != -1 && appdata->window_id != -1) {
				mmt.connection_id = appdata->connection_id;
				mmt.window_id = appdata->window_id;
				mmt.medium = conn->protocol;
				mmt.name = conn->username;
				mmt.location = NULL;

				/* Our type and name */
				mmt.menu_type = MENU_TYPE_RIGHTCLICK;
				mmt.menu_name = "Trillian OTR";

				plugin_send(MYGUID, "messageMenuRemove", &mmt);
			}
			if (appdata) {
				conn->app_data_free(appdata);
				conn->app_data=NULL;
				

			}
			conn = conn->next;
		}

		totr_config.SaveConfig();
		otrl_privkey_forget_all(userstate);
		userstate->context_root=NULL;
		otrl_userstate_free(userstate);
		totr_state.StopAllEvents();

	} else if (!_stricmp(event, "unload")) {
		if (prefDlg) {
			prefDlg->DestroyWindow();
			delete prefDlg;
			prefDlg = NULL;
		}

		if (tmp_config) {
			delete tmp_config;
			tmp_config = NULL;
		}

		plugin_send = NULL;

  	    _Module.Term();

		return 0;
	} else if (!_stricmp(event, "finalUnload")) {
	} else if (!_stricmp(event, "prefsShow")) {
		struct plugin_prefs_show_t	*pps = (struct plugin_prefs_show_t *)data;
		int resource = 0;

		if (!strcmp(pps->pref_name, "Trillian OTR")) {
			strcpy(pps->prefs_info.name, "Trillian Off-The-Record: Risen");
			strcpy(pps->prefs_info.description, "Off the Record messaging support for Trillian.");

			if (!tmp_config)
				tmp_config = new TotrConfig(totr_config);

			if (pps->sub_entry == NULL)
				resource = IDD_PREFS_DIALOG;
			else if (!strcmp(pps->sub_entry, "OTR General"))
				resource = IDD_PREFS_GENERAL;
			else if (!strcmp(pps->sub_entry, "OTR Config"))
				resource = IDD_PREFS_CONFIG;

			if (pps->show == TRILLIAN_PREFS_SHOW)
			{
				HWND hwnd = NULL;

				if (prefDlg && prefDlg->GetResource() != resource) 
				{
					prefDlg->DestroyWindow();
					delete prefDlg;
					prefDlg = NULL;
				}

				if (prefDlg == NULL)
				{
					switch (resource)
					{
						case IDD_PREFS_DIALOG:
							prefDlg = (CDialogPrefs *) new CAboutDlg;
							break;
						case IDD_PREFS_GENERAL:
							prefDlg = (CDialogPrefs *) new CGeneralDlg;
							break;
						case IDD_PREFS_CONFIG:
							prefDlg = (CDialogPrefs *) new CConfigDlg;
							break;
						default:
							break;
					}
                
					hwnd = prefDlg->Create(pps->hwnd);
					prefDlg->SetWindowPos(hwnd, pps->x, pps->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}

				prefDlg->ShowWindow(SW_SHOW);
			}
			else if (pps->show == TRILLIAN_PREFS_HIDE && prefDlg && prefDlg->GetResource() == resource)
			{
				prefDlg->ShowWindow(SW_HIDE);
			}
		}

	} else if (!_stricmp(event, "prefsAction")) {
		struct plugin_prefs_action_t	*ppa = (struct plugin_prefs_action_t *)data;
		
		switch (ppa->type) {
			case TRILLIAN_PREFS_APPLY:
				totr_config = *tmp_config;
				totr_config.SaveConfig();
				break;
			case TRILLIAN_PREFS_OK:
				totr_config = *tmp_config;
				totr_config.SaveConfig();
			case TRILLIAN_PREFS_CANCEL:
				if (prefDlg != NULL)
				{
					prefDlg->DestroyWindow();
					delete prefDlg;
					prefDlg = NULL;
				}

				if (tmp_config) {
					delete tmp_config;
					tmp_config = NULL;
				}

				break;
		}
	}
#endif
	return 0;
}

//}

int WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID reserved)
{
	/* This is wasted processing since we break and return TRUE
	 * no matter what.
	 */
#if 0
	hInstance = reinterpret_cast<HINSTANCE>(hinstDLL);

	switch(dwReason)
	{
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
#endif

	return TRUE;
}
