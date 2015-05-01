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

bool totrHandleSMP(ConnContext* context, OtrlTLV* tlvs)
{
	/* Keep track of our current progress in the Socialist Millionaires'
     * Protocol. */
	if(!context) return false;
	NextExpectedSMP nextMsg = context->smstate->nextExpected;

	if (context->smstate->sm_prog_state == OTRL_SMP_PROG_CHEATED)
	{
		totrPluginAbortSMP(context);
		context->smstate->nextExpected = OTRL_SMP_EXPECT1;
		context->smstate->sm_prog_state = OTRL_SMP_PROG_OK;
		return true;
	}

	bool isSMP = false;
	OtrlTLV* tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP1Q);
	if(tlv)
	{
		isSMP = true;
		if(nextMsg != OTRL_SMP_EXPECT1)
			totrPluginAbortSMP(context);
		else
		{
			char *question = reinterpret_cast<char*>(tlv->data);
			char *eoq = reinterpret_cast<char*>(memchr(question, '\0', tlv->len));
			if(eoq)
			{
				CSMPChallengeDlg* dlg = new CSMPChallengeDlg(question);
				dlg->Create(NULL, (LPARAM) context);
				dlg->ShowWindow(TRUE);
			}
		}
	}
	tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP1);
	if(tlv)
	{
		isSMP = true;
		if(nextMsg != OTRL_SMP_EXPECT1)
			totrPluginAbortSMP(context);
		else
		{
			CSMPChallengeDlg* dlg = new CSMPChallengeDlg("What is the shared secret?");
			dlg->Create(NULL, (LPARAM) context);
			dlg->ShowWindow(TRUE);
		}
	}
	tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP2);
	if(tlv)
	{
		isSMP = true;
		if(nextMsg != OTRL_SMP_EXPECT2)
			totrPluginAbortSMP(context);
		else
		{
			context->smstate->nextExpected = OTRL_SMP_EXPECT4;
		}
	}
	tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP3);
	if(tlv)
	{
		isSMP = true;
		if(nextMsg != OTRL_SMP_EXPECT3)
			totrPluginAbortSMP(context);
		else
		{
			context->smstate->nextExpected = OTRL_SMP_EXPECT1;
		}
	}
	tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP4);
	if(tlv)
	{
		isSMP = true;
		if(nextMsg != OTRL_SMP_EXPECT4)
			totrPluginAbortSMP(context);
		else
		{
			context->smstate->nextExpected = OTRL_SMP_EXPECT1;
		}
	}
	tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP_ABORT);
	if(tlv)
	{
		isSMP = true;
		context->smstate->nextExpected = OTRL_SMP_EXPECT1;
	}
	return isSMP;
}

/* Abort the SMP protocol.  Used when malformed or unexpected messages
 * are received.
 */
void totrPluginAbortSMP(ConnContext* context)
{
    otrl_message_abort_smp(userstate, &totr_ops, NULL, context);
}

/* Start the Socialist Millionaires' Protocol over the current connection,
 * using the given initial secret, and optionally a question to pass to
 * the buddy.
 */
void totrPluginStartSMP(ConnContext* context, const char* question,
	const char* secret, size_t secretlen)
{
    otrl_message_initiate_smp_q(userstate, &totr_ops, NULL,
	    context, question, reinterpret_cast<const unsigned char*>(secret), secretlen);
}

/* Continue the Socialist Millionaires' Protocol over the current connection,
 * using the given initial secret (ie finish step 2).
 */
void totrPluginContinueSMP(ConnContext* context,
	const char* secret, size_t secretlen)
{
    otrl_message_respond_smp(userstate, &totr_ops, NULL,
	    context, reinterpret_cast<const unsigned char*>(secret), secretlen);
}


int accountLookupCallback(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	TotrAcctLookup* lookup = reinterpret_cast<TotrAcctLookup*>(userData);

	if(stricmp(event, "enum_add") != 0)
		return 0;

	connection_entry_t *cet = reinterpret_cast<connection_entry_t*>(data);

	if(lookup->connection_id != -1 && lookup->connection_id == cet->connection_id
		&& !lookup->account_name)
			lookup->account_name = _strdup(cet->name);
	else if(lookup->account_name != NULL && !stricmp(lookup->account_name, cet->name))
		lookup->connection_id = cet->connection_id;

	return 0;
}

char* accountNameLookup(char* medium, int connection_id)
{
	connection_enum_t cet;
	TotrAcctLookup lookup;

	trillianInitialize(cet);

	cet.callback = accountLookupCallback;
	cet.data = &lookup;
	cet.medium = medium;

	lookup.connection_id = connection_id;
	lookup.account_name = NULL;

	plugin_send(MYGUID, "connectionEnumerate", &cet);

	return lookup.account_name;
}

int accountConnectionLookup(char* medium, char* account_name)
{
	connection_enum_t cet;
	TotrAcctLookup lookup;

	trillianInitialize(cet);

	cet.callback = accountLookupCallback;
	cet.data = &lookup;
	cet.medium = medium;

	lookup.connection_id = -1;
	lookup.account_name = account_name;

	plugin_send(MYGUID, "connectionEnumerate", &cet);

	return lookup.connection_id;
}

int messageWindowEchostate(message_t* msg, int echo_off)
{
	message_options_t mopts;
	int ret = -1;

	memset(&mopts, -1, sizeof(mopts));
	mopts.struct_size = sizeof(mopts);
	mopts.disable_message = mopts.logging_name = mopts.echo_name = NULL;
	mopts.nicklist = NULL;

	msg->extra_information = &mopts;
	msg->extra_information_size = mopts.struct_size;

	if(msg->text)
		msg->text_len = static_cast<int>(strlen(msg->text) + 1); // Change suggested by Thisita 10/20/11

	mopts.echo_off = echo_off;
	mopts.logging_off = echo_off;

	ret = plugin_send(MYGUID, "messageUpdate", msg);

	msg->extra_information = NULL;
	msg->extra_information_size = 0;

	return ret;
}

int messageWindowEchostate(int connection_id, int window_id, char* medium, char* username, int echo_off)
{
	/* This might be what I need. A way to disable echoing and logging
	 * for a particular window. If I do this, it may hide the messages
	 * since it seems that no other way is capable of doing so. HOWEVER,
	 * it also seems like I cannot do a quick flip of settings off and
	 * on, I have to record stateful information about negotiations
	 *
	 * Isn't this going to be fun?
	 */

	message_t msg;
	message_options_t mopts;
	int ret = -1;

	trillianInitialize(msg);
	trillianInitialize(mopts);

	msg.connection_id = connection_id;
	msg.window_id = window_id;

	msg.medium = medium;
	msg.name = username;

	msg.extra_information = &mopts;
	msg.extra_information_size = mopts.struct_size;

	mopts.echo_off = echo_off;
	mopts.logging_off = echo_off;

	ret = plugin_send(MYGUID, "messageUpdate", &msg);

	return ret;
}

int messageSecureState(int connection_id, int window_id, char* medium, char* username, int secure)
{
	/* Sweet. We can set the encrypted icon with this thing!
	 */

	message_state_t mst;
	int ret = -1;

	trillianInitialize(mst);

	mst.connection_id = connection_id;
	mst.window_id = window_id;

	mst.medium = medium;
	mst.name = username;

	mst.control = "encrypted_icon";

	if(secure)
		mst.state = "on";
	else
		mst.state = "off";

	ret = plugin_send(MYGUID, "messageSetState", &mst);

	return ret;
}

int userStatusLookupCallback(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	if(stricmp(event, "enum_add") != 0)
		return 0;

	message_t *status = reinterpret_cast<message_t*>(userData);
	contactlist_entry_t *cet = reinterpret_cast<contactlist_entry_t*>(data);
	int *online = reinterpret_cast<int*>(status ? status->extra_information : NULL);

	if (cet->connection_id != status->connection_id && stricmp(cet->real_name, status->name) != 0)
		return 0;

		/* Pay attention to AIM goodiness. 'mobile' is a valid status for 'online' connections,
			* but really means that they are listening via SMS, not a true client.
			*/
	if (!stricmp(cet->status, "offline") ||	!stricmp(cet->status, "recent-offline") ||
		!stricmp(cet->status, "mobile"))
	{
		if (online)
			*online = 0;
	}
	else
	{
		if (online)
			*online = 1;
	}

	return 0;
}

int userDisplayNameCallback(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	TotrUserLookup* lookup = reinterpret_cast<TotrUserLookup*>(userData);
	contactlist_entry_t *cet = reinterpret_cast<contactlist_entry_t*>(data);
	
	if(stricmp(event, "enum_add") != 0)
		return 0;

	if(cet->connection_id == lookup->connection_id && !stricmp(cet->real_name, lookup->real_name))
	{
		if (!(lookup->name))
			lookup->name = _strdup(cet->name);
	}
	return 0;
}

char* userDisplayNameLookup(char* medium, char* real_name, int connection_id)
{
	contactlist_enum_t cet;
	TotrUserLookup lookup;

	trillianInitialize(cet);
	ZeroMemory(&lookup,sizeof(lookup));

	cet.callback = userDisplayNameCallback;
	cet.data = &lookup;
	cet.medium = medium;
	cet.connection_id = connection_id;

	lookup.connection_id = connection_id;
	lookup.real_name = real_name;

	plugin_send(MYGUID, "contactlistEnumerate", &cet);

	return lookup.name;
}

int totrTrustedFingerprint(Fingerprint* fp)
{
	if(!fp)
		return 0;

	if(fp->trust && fp->trust[0] != '\0')
		return 1;

	return 0;
}

void totrShowVerifyDialog(HWND hWnd, ConnContext* conn, Fingerprint* fp)
{
	CVerifyFingerDlg* verify = NULL;

	verify = new CVerifyFingerDlg;
	verify->Create(hWnd, (LPARAM) fp);
	verify->PopulateMessages(conn->protocol, conn->username, fp);
	verify->ShowWindow(TRUE);
}

void totrInitiateSession(ConnContext* conn)
{
	TotrAppData* appdata = NULL;

	if(!conn)
		return;

	appdata = reinterpret_cast<TotrAppData*>(conn->app_data);
	if(!appdata)
		return;

	/* Starting a new conversation and refreshing an old conversation
		* are fired off in exactally the same way. 
		*/
	message_t msgt;
    char* msg = otrl_proto_default_query_msg(conn->accountname, totr_ops.policy(NULL, conn));
	char* account_name = accountNameLookup(conn->protocol, appdata->connection_id);
	//int online = -1;

	if(!account_name)
		return;

	trillianInitialize(msgt);

	msgt.connection_id = appdata->connection_id;
	msgt.window_id = appdata->window_id;
	msgt.medium = conn->protocol;
	msgt.name = conn->username;

	//online = totr_ops.is_logged_in(&msgt, account_name, conn->protocol, conn->username);
	
	//if(online == 1 || online == -1)
	//{
		totr_ops.inject_message(&msgt, account_name, conn->protocol, conn->username, 
			(msg ? msg : "?OTRv2?"));
	//}

	free(account_name);
	account_name = NULL;
}

void totrTerminateSession(ConnContext* conn)
{
	message_t msg;
	TotrAppData* appdata = NULL;
	char* account_name = NULL;

	if(!conn)
		return;

	appdata = reinterpret_cast<TotrAppData*>(conn->app_data);
	if(!appdata)
		return;

	account_name = accountNameLookup(conn->protocol, appdata->connection_id);
	if(!account_name)
		return;

	trillianInitialize(msg);

	msg.connection_id = appdata->connection_id;
	msg.window_id = appdata->window_id;
	msg.medium = conn->protocol;
	msg.name = conn->username;

	otrl_message_disconnect(userstate, &totr_ops, &msg, account_name, conn->protocol,
		conn->username);

	/* Notify the user that we ended the session */
	message_t notice;
	trillianInitialize(notice);
	notice.window_id = appdata->window_id; // We have this so we don't have to fill ou tanything else :D
	notice.name = "Trillian_OTR_Risen"; // I could possibly set this to NULL
	notice.type = "information_standard";
	notice.text = "<b>Encrypted session successfully terminated</b>";
	
	plugin_send(MYGUID, "messageReceive", &notice);
	Sleep(100); // Make sure we don't scope kill the data to fast

	free(account_name);
	account_name = NULL;
}

int collectConnectionsCallback(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
    connection_entry_wrapper_t **lookup = reinterpret_cast<connection_entry_wrapper_t**>(userData);
    connection_entry_wrapper_t *current = NULL;

	if(stricmp(event, "enum_add") != 0)
		return 0;

	connection_entry_t* cet = reinterpret_cast<connection_entry_t*>(data);

	// Populate a new member in our list. Can we just copy the
	// connection_entry_t structures?
	if (!lookup)
		return 0;
		    
	current = new connection_entry_wrapper_t;
	//current->data = (struct connection_entry_t *)malloc(sizeof(struct connection_entry_t));
	current->data = new connection_entry_t;
	current->next = *lookup;

	trillianInitialize((*current->data));

	current->data->connection_id = cet->connection_id;
	current->data->medium = _strdup(cet->medium);
	current->data->name = _strdup(cet->name);
	current->data->status = _strdup(cet->status);

	*lookup = current;

    return 0;
}

connection_entry_wrapper_t* collectActiveConnections()
{
	connection_enum_t cet;
	connection_entry_wrapper_t* cwrap = NULL;
		
	trillianInitialize(cet);

	cet.callback = collectConnectionsCallback;
	cet.data = &cwrap;
	cet.medium = "all";

	plugin_send(MYGUID, "connectionEnumerate", &cet);
	
	return cwrap;

}

void collectConnectionsFree(connection_entry_wrapper_t* cwrap)
{
	connection_entry_wrapper_t* next = NULL;
	
	while(cwrap)
	{
		next = cwrap->next;
		delete cwrap;
		cwrap = next;
	}
}

void totrPopulateKnownConnections()
{
	connection_entry_wrapper_t* cwrap = NULL;
	connection_entry_wrapper_t* ccurrent = NULL;
	ConnContext* conn = NULL;
	TotrAppData* appdata = NULL;
	
	cwrap = collectActiveConnections();

	for(conn = userstate->context_root; conn; conn = conn->next)
	{
		appdata = reinterpret_cast<TotrAppData*>(conn->app_data);
		if(!appdata)
			continue;
		
		if (appdata->connection_id == -1)
		{
			for (ccurrent = cwrap; ccurrent; ccurrent = ccurrent->next)
			{
				if (ccurrent->data && !stricmp(ccurrent->data->name, conn->accountname) &&
					!stricmp(ccurrent->data->medium, conn->protocol))
				{
					appdata->connection_id = ccurrent->data->connection_id;
					break;
				}
			}
		}
	}
	
	collectConnectionsFree(cwrap);
}