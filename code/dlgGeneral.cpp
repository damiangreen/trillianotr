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

HWND CGeneralDlg::Create(HWND hWndParent, LPARAM dwInitParam)
{
	hWnd = CDialogPrefs::Create(hWndParent, dwInitParam);

	/* Associate our backend structures with the dialog items */
	hKeyList = ::GetDlgItem(hWnd, IDC_KEYLIST);
	hStartConv = ::GetDlgItem(hWnd, IDC_STARTPRVCONV);
	hEndConv = ::GetDlgItem(hWnd, IDC_ENDPRVCONV);
	hVerifyFngr = ::GetDlgItem(hWnd, IDC_VRFYFINGER);
	hForgetFngr = ::GetDlgItem(hWnd, IDC_FORGETFINGER);

	/* Cap off our headers for the key list */
	hKeyList.InsertColumn(0, _T("Screenname"), LVCFMT_LEFT, 90);
	hKeyList.InsertColumn(1, _T("Status"), LVCFMT_LEFT, 90);
	hKeyList.InsertColumn(2, _T("Verified"), LVCFMT_LEFT, 60);
	hKeyList.InsertColumn(3, _T("Fingerprint"), LVCFMT_LEFT, 400);
	hKeyList.InsertColumn(4, _T("Account"), LVCFMT_LEFT, 200);

#if 0
	for (context = userstate->context_root; context; context = context->next)
	{
		fingerprint = context->fingerprint_root.next;
		while (fingerprint)
		{
			LVITEM lvi;

			otrl_privkey_hash_to_human(hash, fingerprint->fingerprint);

			lvi.mask = LVIF_TEXT | LVIF_PARAM;
			lvi.iItem = 0;
			lvi.iSubItem = 0;
			lvi.pszText = context->username;
			lvi.lParam = (LPARAM) fingerprint;

			sprintf(acct, "%s (%s)", context->accountname, context->protocol);
			/* Insert our primary item
			 *   0 - Remote Username 
			 */
			int n = hKeyList.InsertItem(&lvi);

			/* Fill in subitem elements 
			 *   1 - Trusted State
			 *   2 - Verified
			 *   3 - Key Fingerprint
			 *   4 - Local Username/Protocol
			 */
			if (fingerprint == context->active_fingerprint) {
				if (totrTrustedFingerprint(fingerprint))
					hKeyList.AddItem(n, 1, "Private");
				else
					hKeyList.AddItem(n, 1, "Unverified");
			} else {
				hKeyList.AddItem(n, 1, "Not private");
			}
			hKeyList.AddItem(n, 2, (totrTrustedFingerprint(fingerprint) ? "Yes" : "No"));
			hKeyList.AddItem(n, 3, hash);
			hKeyList.AddItem(n, 4, acct);

			fingerprint = fingerprint->next;
		}
	}
#else
	UpdateListView();
#endif

	/* Disable the buttons until a selection is made */
	hStartConv.EnableWindow(FALSE);
	hEndConv.EnableWindow(FALSE);
	hVerifyFngr.EnableWindow(FALSE);
	hForgetFngr.EnableWindow(FALSE);

	return hWnd;
}

BOOL CGeneralDlg::DestroyWindow()
{
	hKeyList.DeleteAllItems();

	return CDialogPrefs::DestroyWindow();
}

LRESULT CGeneralDlg::OnStartConv(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LVITEM lvi;
	Fingerprint* fp = NULL;

	ZeroMemory(&lvi,sizeof(lvi));
	lvi.mask = LVIF_PARAM;

	hKeyList.GetSelectedItem(&lvi);
	fp = reinterpret_cast<Fingerprint*>(lvi.lParam);

	if(!fp)
		return 0;

	totrInitiateSession(fp->context);

	UpdateListView();
	return 0;
}

LRESULT CGeneralDlg::OnEndConv(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LVITEM lvi;
	Fingerprint* fp = NULL;

	ZeroMemory(&lvi,sizeof(lvi));
	lvi.mask = LVIF_PARAM;

	hKeyList.GetSelectedItem(&lvi);
	fp = reinterpret_cast<Fingerprint*>(lvi.lParam);

	if(!fp)
		return 0;

	totrTerminateSession(fp->context);

	UpdateListView();
	return 0;
}

LRESULT CGeneralDlg::OnVerifyFingerprint(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	HWND hWnd = ::GetParent(hWndCtl);
	LVITEM lvi;
	Fingerprint* fp = NULL;

	ZeroMemory(&lvi,sizeof(lvi));
	lvi.mask =  LVIF_PARAM;

	hKeyList.GetSelectedItem(&lvi);
	fp = reinterpret_cast<Fingerprint*>(lvi.lParam);

	if(!fp)
		return 0;

	totrShowVerifyDialog(hWnd, fp->context, fp);

	/* Update our list in the callback in CVerifyFingerprintDlg */

	return 0;
}

LRESULT CGeneralDlg::OnForgetFingerprint(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LVITEM lvi;
	Fingerprint* fp = NULL;

	ZeroMemory(&lvi,sizeof(lvi));
	lvi.mask =  LVIF_PARAM;

	hKeyList.GetSelectedItem(&lvi);
	fp = reinterpret_cast<Fingerprint*>(lvi.lParam);

	if(!fp)
		return 0;

	otrl_context_forget_fingerprint(fp, 1);
	otrl_privkey_write_fingerprints(userstate, totr_config.GetStoreFilename());

	UpdateListView();
	return 0;
}

LRESULT CGeneralDlg::OnKeySelected(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMLISTVIEW nmlv = reinterpret_cast<LPNMLISTVIEW>(pnmh);

	if(nmlv->uNewState & LVIS_SELECTED)
	{
		LVITEM lvi;
		Fingerprint* fp = NULL;
		ConnContext* conn = NULL;

		ZeroMemory(&lvi,sizeof(lvi));
		lvi.mask =  LVIF_PARAM;

		hKeyList.GetSelectedItem(&lvi);
		fp = reinterpret_cast<Fingerprint*>(lvi.lParam);

		if(!fp)
			return 0;

		conn = fp->context;

		hVerifyFngr.EnableWindow(TRUE);

		if(conn->msgstate == OTRL_MSGSTATE_ENCRYPTED ||
			conn->msgstate == OTRL_MSGSTATE_FINISHED)
		{
			hStartConv.EnableWindow(FALSE);
			hEndConv.EnableWindow(TRUE);
			hForgetFngr.EnableWindow(FALSE);
		}
		else
		{
			hStartConv.EnableWindow(TRUE);
			hEndConv.EnableWindow(FALSE);
			hForgetFngr.EnableWindow(TRUE);
		}
	}
	else
	{
		/* Nothing selected? Clear our button states */
		hStartConv.EnableWindow(FALSE);
		hEndConv.EnableWindow(FALSE);
		hVerifyFngr.EnableWindow(FALSE);
		hForgetFngr.EnableWindow(FALSE);
	}

	return 0;
}

void CGeneralDlg::UpdateListView()
{
	ConnContext* context = NULL;
	Fingerprint* fingerprint = NULL;
	char hash[45] = {0}, acct[200] = {0};

	hKeyList.DeleteAllItems();

	for(context = userstate->context_root; context; context = context->next)
	{
		fingerprint = context->fingerprint_root.next;
		while(fingerprint)
		{
			LVITEM lvi;

			ZeroMemory(&lvi,sizeof(lvi));

			otrl_privkey_hash_to_human(hash, fingerprint->fingerprint);

			lvi.mask = LVIF_TEXT | LVIF_PARAM;
			lvi.iItem = 0;
			lvi.iSubItem = 0;
			lvi.pszText = context->username;
			lvi.lParam = (LPARAM) fingerprint;

			_snprintf_s(acct, 200, 199, "%s (%s)", context->accountname, context->protocol);
			/* Insert our primary item
			 *   0 - Remote Username 
			 */
			int n = hKeyList.InsertItem(&lvi);

			/* Fill in subitem elements 
			 *   1 - Trusted State
			 *   2 - Verified
			 *   3 - Key Fingerprint
			 *   4 - Local Username/Protocol
			 */
			if(fingerprint == context->active_fingerprint)
			{
				if(totrTrustedFingerprint(fingerprint))
					hKeyList.AddItem(n, 1, "Private");
				else
					hKeyList.AddItem(n, 1, "Unverified");
			}
			else
				hKeyList.AddItem(n, 1, "Not private");

			hKeyList.AddItem(n, 2, (totrTrustedFingerprint(fingerprint) ? "Yes" : "No"));
			hKeyList.AddItem(n, 3, hash);
			hKeyList.AddItem(n, 4, acct);

			fingerprint = fingerprint->next;
		}
	}

	/* We deleted all the items. If no selection is currently made, and
	 * it shouldn't, clear all button states */
	hStartConv.EnableWindow(FALSE);
	hEndConv.EnableWindow(FALSE);
	hVerifyFngr.EnableWindow(FALSE);
	hForgetFngr.EnableWindow(FALSE);
}
