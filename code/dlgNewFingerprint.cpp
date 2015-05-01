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

class TotrNewFingerData
{
	public:
		CNewFingerDlg* finger;
		char* accountname;
		char* medium;

		TotrNewFingerData() : finger(NULL), accountname(NULL), medium(NULL)
		{
		}
		~TotrNewFingerData()
		{
			free(accountname);
			free(medium);
			accountname = medium = NULL;
		}
};

DWORD WINAPI threadedGenerateFingerprint(PVOID arg)
{
	TotrNewFingerData* nfd = reinterpret_cast<TotrNewFingerData*>(arg);

	Sleep(100);

	otrl_privkey_generate(userstate, totr_config.GetPrivKeyFilename(), 
		nfd->accountname, nfd->medium);

	nfd->finger->MarkComplete(nfd->accountname, nfd->medium);
	delete nfd;

	return 0;
}


LRESULT CNewFingerDlg::OnFinishDlg(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(prefDlg && prefDlg->GetResource() == IDD_PREFS_CONFIG)
	{
		CConfigDlg* config = reinterpret_cast<CConfigDlg*>(prefDlg);
		BOOL handled;

		config->OnSelectChange(0, 0, 0, handled);
	}
    
	ShowWindow(SW_HIDE);
	DestroyWindow();

	return 0;
}

HWND CNewFingerDlg::Create(HWND hWndParent, LPARAM dwInitParam)
{
	const char* fmt = "Generating private key for %s...";
	char buf[1024];
	char* primary = NULL;
	char* username = reinterpret_cast<char*>(dwInitParam);
	CLogFont lf;
	CFont f;

	if(!username)
		username = "Unknown";

	hWnd = CDialogImpl<CNewFingerDlg>::Create(hWndParent, 0);

	//primary = reinterpret_cast<char*>(malloc(strlen(fmt)+strlen(username)-1));
	_snprintf_s(buf, 1024, 1023, fmt, username);
	primary = _strdup(buf);

	hTitle.Attach(::GetDlgItem(hWnd, IDC_NEWFINGERTITLE));
	hFingerprint.Attach(::GetDlgItem(hWnd, IDC_NEWFINGERPRINT));
	hOk.Attach(::GetDlgItem(hWnd, IDOK));

	lf = hTitle.GetFont();
	lf.SetBold();
	lf.SetHeight(18);
	f.CreateFontIndirect(&lf);
	hTitle.SetFont(f);
	f.DeleteObject();

	hTitle.SetWindowText(primary);
	hFingerprint.SetWindowText("Pending.");

	hOk.EnableWindow(FALSE);

	free(primary);
	primary = NULL;

	return hWnd;
}

void CNewFingerDlg::GenerateFingerprint(const char* accountname, const char* medium)
{
	TotrNewFingerData* nfd = new TotrNewFingerData;

	nfd->accountname = _strdup(accountname);
	nfd->medium = _strdup(medium);
	nfd->finger = this;

	HANDLE hThread;
	DWORD threadId;
	hThread = CreateThread(NULL, 0, threadedGenerateFingerprint, reinterpret_cast<PVOID>(nfd), 0, &threadId);
}

void CNewFingerDlg::MarkComplete(char* accountname, char* medium)
{
	const char *fmt = "Generating private key for %s...done.";
	char *primary = NULL;
	char buf[1024];
	char *fp = NULL;
	char fp_buf[45] = {0};

	//primary = reinterpret_cast<char*>(malloc(strlen(fmt)+strlen(accountname)-1));
	_snprintf_s(buf, 1024, 1023, fmt, accountname);
	primary = _strdup(buf);

	fp = otrl_privkey_fingerprint(userstate, fp_buf, accountname, medium);

	hTitle.SetWindowText(primary);

	if(fp)
		hFingerprint.SetWindowText(fp);
	else
		hFingerprint.SetWindowText("Error creating private key; No key found.");

	hOk.EnableWindow();
}

