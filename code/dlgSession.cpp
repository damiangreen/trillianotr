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

LRESULT CSessionDlg::OnFinishDlg(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ShowWindow(SW_HIDE);
	DestroyWindow();

	return 0;
}

HWND CSessionDlg::Create(HWND hWndParent, LPARAM dwInitParam)
{
	ConnContext* conn = reinterpret_cast<ConnContext*>(dwInitParam);
	char first[21];
	char last[21];
	char* primary = NULL;
	const char* fmt = "Private session with %s\n   established.";
	char buf[1024];
	CLogFont lf;
	CFont f;
	unsigned int i;
    size_t mid = (conn->sessionid_len) / 2;

	hWnd = CDialogImpl<CSessionDlg>::Create(hWndParent, 0);

	//primary = reinterpret_cast<char*>(malloc(strlen(fmt)+strlen(conn->username)-1));
	_snprintf_s(buf, 1024, 1023, fmt, conn->username);
	primary = _strdup(buf);

    for(i=0;i<mid;++i)
	{
		_snprintf(first+(2*i), 21-2*i, "%02x", conn->sessionid[i]);
//    for(i=0;i<mid;++i)
		_snprintf(last+(2*i), 21-2*i, "%02x", conn->sessionid[i+mid]);
	}

	hTitle.Attach(::GetDlgItem(hWnd, IDC_SESSIONTITLE));
	hFingerprint.Attach(::GetDlgItem(hWnd, IDC_SESSIONDLG_FINGERPRINT));
	hIDFirst.Attach(::GetDlgItem(hWnd, IDC_IDFIRSTHALF));
	hIDLast.Attach(::GetDlgItem(hWnd, IDC_IDLASTHALF));

	if(conn->sessionid_half == OTRL_SESSIONID_FIRST_HALF_BOLD)
	{
		lf = hIDFirst.GetFont();
		lf.SetBold();
		f.CreateFontIndirect(&lf);
	
		hIDFirst.SetFont(f);
		/* Commented out due to unintentional bolding
		 * Probably an issue with the modification
		 * of the fonts.
		 *
		f.DeleteObject();

		CLogFont lf_plain = hIDLast.GetFont();
		lf_plain.SetHeight(14);
		CFont f_plain;
		f_plain.CreateFontIndirect(&lf_plain);
	
		hIDLast.SetFont(f_plain);
		*/
	}
	else
	{
		lf = hIDLast.GetFont();
		lf.SetBold();
		f.CreateFontIndirect(&lf);
	
		hIDLast.SetFont(f);
		/* Commented out due to unintentional bolding
		 * Probably an issue with the modification
		 * of the fonts.
		 *
		f.DeleteObject();

		CLogFont lf_plain = hIDFirst.GetFont();
		lf_plain.SetHeight(14);
		CFont f_plain;
		f_plain.CreateFontIndirect(&lf_plain);
	
		hIDFirst.SetFont(f_plain);
		*/
	}

	f.DeleteObject();
	lf = hTitle.GetFont();
	lf.SetBold();
	lf.SetHeight(18);
	f.CreateFontIndirect(&lf);
	hTitle.SetFont(f);

	hTitle.SetWindowText(primary);
	hIDFirst.SetWindowText(first);
	hIDLast.SetWindowText(last);

	free(primary);
	primary = NULL;

	return hWnd;
}

