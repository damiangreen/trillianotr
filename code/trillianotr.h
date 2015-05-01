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

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TRILLIANOTR_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TRILLIANOTR_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TRILLIANOTR_EXPORTS
#define TRILLIANOTR_API __declspec(dllexport)
#else
#define TRILLIANOTR_API __declspec(dllimport)
#endif


#ifndef TRILLIANOTR_H
#define TRILLIANOTR_H

void reopenMessageWindow(const message_t* message);
DWORD WINAPI threadedAddControls(PVOID arg);
DWORD WINAPI threadedInitSecureMem(PVOID arg);
int liveWindowCallback(int windowID, char* subwindow, char* event, void* data, void* userData);
int newWindowCallback(int windowID, char* subwindow, char* event, void* data, void* userData);
int deadWindowCallback(int windowID, char* subwindow, char* event, void* data, void* userData);
int connectionChangeCallback(int windowID, char* subwindow, char* event, void* data, void* userData);
int broadCallback(int windowID, char* subwindow, char* event, void* data, void* userData);
void mainLoad(plugin_info_t* pi);
void mainStart();
void mainStop();
void mainUnload();
void mainPrefsShow(plugin_prefs_show_t* pps);
void mainPrefsAction(plugin_prefs_action_t* ppa);

#endif