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

LRESULT CVerifyFingerDlg::OnFinishDlg(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ShowWindow(SW_HIDE);
	DestroyWindow();

	otrl_privkey_write_fingerprints(userstate, totr_config.GetStoreFilename());

	return 0;
}

HWND CVerifyFingerDlg::Create(HWND hWndParent, LPARAM dwInitParam)
{
	CLogFont lf;
	CFont f;

	fp = reinterpret_cast<Fingerprint*>(dwInitParam);
	hWnd = CDialogImpl<CVerifyFingerDlg>::Create(hWndParent, 0);

	hTitle.Attach(::GetDlgItem(hWnd, IDC_VERIFY_FINGERTITLE));
	hFingerprint.Attach(::GetDlgItem(hWnd, IDC_FINGER_TO_VERIFY));
	hVerify.Attach(::GetDlgItem(hWnd, IDC_VERIFY_COMBO));
	hOk.Attach(::GetDlgItem(hWnd, IDOK));

	lf = hTitle.GetFont();
	lf.SetBold();
	lf.SetHeight(18);
	f.CreateFontIndirect(&lf);
	hTitle.SetFont(f);
	f.DeleteObject();

	hVerify.InsertString(0, "have not");
	hVerify.InsertString(1, "have");

	return hWnd;
}

void CVerifyFingerDlg::PopulateMessages(const char* medium, const char* username, Fingerprint* fp)
{
	const char* fmt = "Verify fingerprint for %s (%s).";
	char* primary = NULL;
	char buf[1024];
	char fp_buf[45] = {0};

	//primary = reinterpret_cast<char*>(malloc(strlen(fmt)+strlen(medium)+strlen(username)-3));
	_snprintf_s(buf, 1024, 1023, fmt, username, medium);
	primary = _strdup(buf);

	hTitle.SetWindowText(primary);

	otrl_privkey_hash_to_human(fp_buf, fp->fingerprint);

	hFingerprint.SetWindowText(fp_buf);

	hVerify.SetCurSel(totrTrustedFingerprint(fp));

	free(primary);
	primary = NULL;
}

LRESULT CVerifyFingerDlg::OnSelectChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int indx = hVerify.GetCurSel();

	if(indx && fp)
		fp->trust = _strdup("Verified");
	else
	{
		free((fp->trust));
		fp->trust = NULL;
	}

	if(prefDlg && prefDlg->GetResource() == IDD_PREFS_GENERAL)
	{
		CGeneralDlg* general = reinterpret_cast<CGeneralDlg*>(prefDlg);

		general->UpdateListView();
	}

	return 0;
}
