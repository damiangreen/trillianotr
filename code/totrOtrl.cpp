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

bool proto_supports_otr(const char* protocol)
{
	if (!stricmp(protocol, "AIM") || !stricmp(protocol, "ICQ") ||
		!stricmp(protocol, "MSN") || !stricmp(protocol, "JABBER") ||
		!stricmp(protocol, "YAHOO") || !stricmp(protocol, "RENDEZVOUS") ||
		!stricmp(protocol, "SAMETIME") || !stricmp(protocol,"FACEBOOK"))
		return 1;
	/* Facebook support added 10/14/2011 */
	return 0;
}

void stripCursorTag(char* out, const char* in)
{
	CString buf = in;
	buf.Replace("<cursor/>", "");
	/*unsigned int i = 0;
	while(i < strlen(in))
	{
		if(in[i] == '<' && in[i+1] == 'c' && in[i+2] == 'u'
			&& in[i+3] == 'r')
		{
			i += 9;
			if(i >= strlen(in))
				break;
			continue;
		}
		buf.AppendChar(in[i]);
		i++;
	}*/
	strncpy_s(out, strlen(out) + 1, buf.GetString(), _TRUNCATE);
}

int totrEncryptMessage(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	edit_event_t* msg = reinterpret_cast<edit_event_t*>(data);
	TotrAppData* eopts = reinterpret_cast<TotrAppData*>(userData);
	char* message = NULL;
	char* newmessage = NULL;
	char* account_name = NULL;
	int res = -1;
	int connection_id = -1;
	ConnContext* conn = userstate->context_root;
	//CAtlRegExp<> reStart, reEnd;
	//CAtlREMatchContext<> mcStart, mcEnd;
	BOOL resStart = FALSE;
	BOOL resEnd = FALSE;

	if(!msg || !reinterpret_cast<char*>(msg->data)
		|| stricmp(event, "edit_sendText") != 0 || !strlen(reinterpret_cast<char*>(msg->data))
		|| (reinterpret_cast<char*>(msg->data)[0]) == '/')
		return 0;
	/* Bail out of parsing any messages starting with /
		* Too bad we can't tell if this is a real command
		* or not without doing some interpretive lists.
		* Even then, such lists would not include other
		* plugins that impliment their own / commands.
		*
		* May need to exclude everything and include
		* /me. Unsure of that.
		*/

	while(conn)
	{
		if(conn->app_data != NULL)
		{
			TotrAppData *appdata = reinterpret_cast<TotrAppData*>(conn->app_data);

			if(appdata->window_id == msg->window_id)
			{
				connection_id = appdata->connection_id;
				break;
			}
		}
		conn = conn->next;
	}

	if(connection_id == -1)
		return 0;

	account_name = accountNameLookup(conn->protocol, connection_id);

	if(!account_name)
		return 0;

	/* Cleanse our text string of the <cursor/> tag Trillian puts in */
	message = _strdup(reinterpret_cast<char*>(msg->data));
	stripCursorTag(message,reinterpret_cast<char*>(msg->data));

#if 0
	char *search = strstr((char *) msg->data, "<cursor/>");

	if (search) {
		char *c = NULL, *s = NULL;
		size_t bufsize=strlen((char *)msg->data)-strlen("<cursor/>")+1;
		c = message = (char *)malloc(bufsize);
		ZeroMemory(c,bufsize);
		ZeroMemory(message,bufsize);
		strncpy(message, (char *)msg->data, search-((char *) msg->data));
		c = c + (search - (char *) msg->data);
		s = search + strlen("<cursor/>");
		strncpy(c, s, strlen(search)-strlen("<cursor/>")+1);
	}
	else
		message = _strdup(reinterpret_cast<char*>(msg->data));

#endif

	res = otrl_message_sending(userstate, &totr_ops, msg,
		account_name, conn->protocol, conn->username, message, 
		NULL, &newmessage, totrNewContext, eopts);

	if(res && !newmessage)
		newmessage = "";
		
	if(newmessage)
	{
		if(totr_config.GetSaveHistory()) 
			if(!totr_config.appendHistory(message,conn->protocol,account_name,conn->username))
				MessageBox(NULL,"Could not save history","ERROR",MB_OK); // Added by Thisita 10/22/11

		message_t msg2;
		message_options_t mopts;
		edit_event_t eet;
		int psres = -1;

		trillianInitialize(msg2);
		trillianInitialize(eet);
		
		memset(&mopts, -1, sizeof(mopts));
		mopts.struct_size = sizeof(mopts);
		mopts.disable_message = mopts.logging_name = mopts.echo_name = NULL;
		mopts.nicklist = NULL;

		reinterpret_cast<TotrAppData*>(conn->app_data)->last_msg = 
			_strdup(reinterpret_cast<char*>(msg->data));
		reinterpret_cast<TotrAppData*>(conn->app_data)->last_msg_len = 
			strlen(reinterpret_cast<char*>(msg->data)) + 1;
	

		eet.window_id = msg->window_id;
		eet.event = "edit_setText";

		if(strlen(newmessage) != 0)
		{
			msg2.connection_id = connection_id;
			msg2.window_id = msg->window_id;
			msg2.name = conn->username;
			msg2.medium = conn->protocol;
			msg2.text = "";
			//msg2.text = _strdup(newmessage);
			msg2.text_len = static_cast<int>(strlen(newmessage) + 1); // Change proposed by Thisita 10/20/11
			msg2.type = "outgoing_privateMessage";
			msg2.extra_information = &mopts;
			msg2.extra_information_size = mopts.struct_size;
			eet.data = newmessage;

			OutputDebugString("sent ");
			OutputDebugString(newmessage);

			if(otrl_proto_message_type(newmessage) != OTRL_MSGTYPE_TAGGEDPLAINTEXT)
			{
				messageWindowEchostate(&msg2, 1);
				psres = plugin_send(MYGUID, "editEventSend", &eet);
				msg2.type="outgoing_privateMessage";
				msg2.text = message; //<HTML><BODY BGCOLOR=\"#ffffff\">greets</BODY></HTML>";
				plugin_send(MYGUID, "messageReceive", &msg2);
			}
		}
	}

	otrl_message_free(newmessage);
	free(account_name);
	account_name = NULL;

	return 0;
}

void totrAppDataFree(void* data)
{
	TotrAppData *eopts = reinterpret_cast<TotrAppData*>(data);
	struct edit_event_t eet;

	if(eopts->edit_event_id != -1)
	{
		eet.edit_event_id = eopts->edit_event_id;
		plugin_send(MYGUID, "editEventUnregister", &eet);
	}

	free((eopts->last_msg));
	eopts->last_msg = NULL;
	delete eopts;
}

void totrNewContext(void* data, ConnContext* conn)
{

	TotrAppData *eopts = reinterpret_cast<TotrAppData*>(data);
	TotrAppData *appdata = new TotrAppData;
	edit_event_t eet;

	trillianInitialize(eet);
	appdata->connection_id = appdata->edit_event_id = appdata->window_id = -1;
	appdata->last_msg = NULL;
	appdata->last_msg_len = 0;

	/* If we have data to add, add it, and generate our edit event. Otherwise,
	 * just fill in the app_data fields
	 */
	if(eopts && eopts->window_id != -1)
	{
		appdata->connection_id = eopts->connection_id;
		eet.window_id = appdata->window_id = eopts->window_id;

		eet.event = "edit_sendText";
		eet.callback = totrEncryptMessage;
		eet.data = appdata;
	
		plugin_send(MYGUID, "editEventRegister", &eet);

		if(!eet.edit_event_id)
		{
			MessageBox(NULL, "Error registering edit event.", "Error", IDOK);
			delete eopts;
			return;
		}

		appdata->edit_event_id = eet.edit_event_id;
	}

	conn->app_data = appdata;
	conn->app_data_free = totrAppDataFree;
}

OtrlPolicy policy_cb(void* opdata, ConnContext* context)
{
	/* We care here only about the global settings (for now).
	 * Because of that, our policy should either be 
	 * 'manual', 'opportunistic', or 'always'
	 */
	if (totr_config.GetEnabled())
	{
		if (totr_config.GetAutoNegotiate())
		{
			if (totr_config.GetRequired())
				return OTRL_POLICY_ALWAYS;
			else
				return OTRL_POLICY_OPPORTUNISTIC;
		}
		else
			return OTRL_POLICY_MANUAL;
	}

	return OTRL_POLICY_NEVER;
}

    /* Create a private key for the given accountname/protocol if
     * desired. */
void create_privkey_cb(void* opdata, const char* accountname,
	    const char* protocol)
{
	message_t* msg = reinterpret_cast<message_t*>(opdata);
	CNewFingerDlg* finger = NULL;

	finger = new CNewFingerDlg;

	finger->Create(NULL, reinterpret_cast<LPARAM>(accountname));
	finger->ShowWindow(TRUE);
	finger->GenerateFingerprint(accountname, protocol);
}

    /* Report whether you think the given user is online.  Return 1 if
     * you think he is, 0 if you think he isn't, -1 if you're not sure.
     *
     * If you return 1, messages such as heartbeats or other
     * notifications may be sent to the user, which could result in "not
     * logged in" errors if you're wrong. */
int is_logged_in_cb(void* opdata, const char* accountname,
	    const char* protocol, const char* recipient)
{
	message_t* msg = reinterpret_cast<message_t*>(opdata);
	contactlist_enum_t cet;
	message_t status;
	int online = -1;

	if(!msg)
		return -1;

	trillianInitialize(cet);

	cet.medium = _strdup(protocol);
	cet.connection_id = msg->connection_id;
	cet.callback = userStatusLookupCallback;
	cet.data = &status;

	status.connection_id = msg->connection_id;
	status.extra_information = &online;
	status.medium = _strdup(protocol);
	status.name = _strdup(recipient);

	plugin_send(MYGUID, "contactlistEnumerate", &cet);

	Sleep(100);

	free(cet.medium);
	free(status.medium);
	free(status.name);

	cet.medium = status.medium = status.name = NULL;

	return online;
}

    /* Send the given IM to the given recipient from the given
     * accountname/protocol. */
void inject_message_cb(void* opdata, const char* accountname,
	    const char* protocol, const char* recipient, const char* message)
{
	message_t outmsg;
	message_options_t outmsgopts;
	message_t* msg = reinterpret_cast<message_t*>(opdata);
	char* account_name = NULL;
	ConnContext* conn = NULL;
	TotrAppData* appdata = NULL;
	int err;

	/* Forcibly attempting to create an OTR connection to a mobile device will
	 * do so, with mixed results. This is partly instigated through our UI, as
	 * there is no direct otrl_...() call that fires off a new conversation.
	 *
	 * If OTR won't do this for us, we should validate against our own
	 * is_logged_in_cb callback function that the user is online before sending.
	 */

	trillianInitialize(outmsg);

	memset(&outmsgopts, -1, sizeof(outmsgopts));
	outmsgopts.struct_size = sizeof(outmsgopts);
	outmsgopts.disable_message = outmsgopts.logging_name = outmsgopts.echo_name = NULL;
	outmsgopts.nicklist = NULL;

	/* Build our message to inject */
	if(msg)
	{
		outmsg.connection_id = msg->connection_id;
		outmsg.type = "outgoing";

		outmsg.medium = msg->medium;
		outmsg.name = msg->name;
	}
	else
	{
		outmsg.type = "outgoing";

		outmsg.medium = _strdup(protocol);
		outmsg.name = _strdup(recipient);
		outmsg.display_name = _strdup(account_name);
		/* Turn echoing off */
		messageWindowEchostate(&outmsg, 1);

		outmsg.text = _strdup(message);
		outmsg.text_len = static_cast<int>(strlen(message) + 1);
		OutputDebugString("sent ");
		OutputDebugString(message);

		/* Fire in the hole! */
		err = plugin_send(MYGUID, "messageSend", &outmsg);
		/* Turn echoing back on. We're all done now */
		messageWindowEchostate(&outmsg, 0);

		delete [] outmsg.text;
		delete [] outmsg.name;
		delete [] outmsg.medium;
		delete [] outmsg.display_name;

		return;
	}

	/* Check to see if we have a window already. We need this
	 * for echo control.
	 */
	if(msg->window_id == -1)
	{

		/* Open a window. */
		err = plugin_send(MYGUID, "messageSend", &outmsg);
		OutputDebugString("sent ");
		OutputDebugString(message);

		/* Now, find it */
		account_name = accountNameLookup(msg->medium, msg->connection_id);
		conn = otrl_context_find(userstate, msg->name, account_name, msg->medium,
			0, NULL, NULL, NULL);

		/* And fill in our window_ids */
		if(conn)
		{
			appdata = reinterpret_cast<TotrAppData*>(conn->app_data);
			if(appdata)
				outmsg.window_id = msg->window_id = appdata->window_id;
		}

		/* Don't forget to clean up our account name */
		free(account_name);
		account_name;
	}

	/* Turn echoing off */
	messageWindowEchostate(msg, 1);

	//outmsg.text = new char [strlen(message)];
	outmsg.text = _strdup(message); // Fix suggested by http://forums.ceruleanstudios.com/showpost.php?p=831067&postcount=88
	outmsg.text_len = static_cast<int>(strlen(message) + 1);
	//sprintf_s(outmsg.text,outmsg.text_len, "%s", message);
	//_snprintf(outmsg.text, outmsg.text_len, "%s", message);
	OutputDebugString("sent ");
	OutputDebugString(message);

	/* Fire in the hole! */
	err = plugin_send(MYGUID, "messageSend", &outmsg);
	/* Turn echoing back on. We're all done now */
	messageWindowEchostate(msg, 0);

	delete [] outmsg.text;
}

    /* Display a notification message for a particular accountname /
     * protocol / username conversation. */
void notify_cb(void* opdata, OtrlNotifyLevel level,
	    const char* accountname, const char* protocol,
	    const char* username, const char* title,
	    const char* primary, const char* secondary)
{
}

    /* Display an OTR control message for a particular accountname /
     * protocol / username conversation.  Return 0 if you are able to
     * successfully display it.  If you return non-0 (or if this
     * function is NULL), the control message will be displayed inline,
     * as a received message, or else by using the above notify()
     * callback. */
int display_otr_message_cb(void* opdata, const char* accountname,
		const char* protocol, const char* username, const char* msg)
{
	return 0;
}

    /* When the list of ConnContexts changes (including a change in
     * state), this is called so the UI can be updated. */
void update_context_list_cb(void* opdata)
{
}

    /* Return a newly-allocated string containing a human-friendly name
     * for the given protocol id */
const char* protocol_name_cb(void* opdata, const char* protocol)
{
	return protocol;
}

    /* Deallocate a string allocated by protocol_name */
void protocol_name_free_cb(void* opdata, const char* protocol_name)
{
}

    /* A new fingerprint for the given user has been received. */
void new_fingerprint_cb(void* opdata, OtrlUserState us,
	    const char* accountname, const char* protocol,
	    const char* username, unsigned char fingerprint[20])
{
	CUnknownFingerDlg* unkfinger = new CUnknownFingerDlg;

	unkfinger->Create(NULL);
	unkfinger->PopulateMessages(accountname, protocol, username, fingerprint);
	unkfinger->ShowWindow(TRUE);
}

    /* The list of known fingerprints has changed.  Write them to disk. */
void write_fingerprints_cb(void* opdata)
{
	otrl_privkey_write_fingerprints(userstate, totr_config.GetStoreFilename());
}

    /* A ConnContext has entered a secure state. */
void gone_secure_cb(void* opdata, ConnContext* context)
{
	TotrAppData* appdata = reinterpret_cast<TotrAppData*>(context->app_data);

	messageSecureState(appdata->connection_id, appdata->window_id, context->protocol,
		context->username, 1);
}

    /* A ConnContext has left a secure state. */
void gone_insecure_cb(void* opdata, ConnContext* context)
{
	TotrAppData* appdata = reinterpret_cast<TotrAppData*>(context->app_data);

	messageSecureState(appdata->connection_id, appdata->window_id, context->protocol,
		context->username, 0);
}

    /* We have completed an authentication, using the D-H keys we
     * already knew.  is_reply indicates whether we initiated the AKE. */
void still_secure_cb(void* opdata, ConnContext* context, int is_reply)
{
}

    /* Log a message.  The passed message will end in "\n". */
void log_message_cb(void* opdata, const char* message)
{
}

#if 1
OtrlMessageAppOps totr_ops = {
	policy_cb,
	create_privkey_cb,
	is_logged_in_cb,
	inject_message_cb,
	notify_cb,
	display_otr_message_cb,
	update_context_list_cb,
	protocol_name_cb,
	protocol_name_free_cb,
	new_fingerprint_cb,
	write_fingerprints_cb,
	gone_secure_cb,
	gone_insecure_cb,
	still_secure_cb,
	log_message_cb,
	NULL, // max_message_size_cb
	NULL, // account_name
	NULL // account_name_free
};
#endif