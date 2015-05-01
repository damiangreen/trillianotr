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

#include "stdafx.h"

#ifndef TOTROTRL_H
#define TOTROTRL_H

bool proto_supports_otr(const char*);
void stripCursorTag(char*, const char*);
int totrEncryptMessage(int, char*, char*, void*, void*);
void totrAppDataFree(void*);
void totrNewContext(void*, ConnContext*);
OtrlPolicy policy_cb(void*, ConnContext*);
void create_privkey_cb(void*, const char*, const char*);
int is_logged_in_cb(void*, const char*, const char*, const char*);
void inject_message_cb(void*, const char*, const char*, const char*, const char*);
void notify_cb(void*, OtrlNotifyLevel, const char*, const char*, const char*,
	const char*, const char*, const char*);
int display_otr_message_cb(void*, const char*, const char*, const char*, const char*);
void update_context_list_cb(void*);
const char* protocol_name_cb(void*, const char*);
void protocol_name_free_cb(void*, const char*);
void new_fingerprint_cb(void*, OtrlUserState, const char*, const char*, const char*, unsigned char[20]);
void write_fingerprints_cb(void*);
void gone_secure_cb(void*, ConnContext*);
void gone_insecure_cb(void*, ConnContext*);
void still_secure_cb(void*, ConnContext*, int);
void log_message_cb(void*, const char*);

#endif
