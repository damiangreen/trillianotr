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


int connectCallback(int windowID, char* subwindow, char* event, void* data, void* userData) 
{
	connection_entry_t* entry = reinterpret_cast<connection_entry_t*>(data);
	CConfigDlg* cDlg = reinterpret_cast<CConfigDlg*>(userData);

	/* Entry pointer holds the data we need.
	 */
	if(strcmp(event, "enum_add") != 0)
		return 0;

	char buffer[1024];

	/* 
		* Getting a new connection entry.
		*
		*	struct connection_entry_t {
		*		unsigned int		struct_size; 
		* 
		*		char				*medium; 
		*		int                 connection_id; 
		*
		*		char				*name; 
		*		char				*status; 
		*	};
		*
		* Medium can be "AIM", "ICQ", "MSN", "YAHOO", "IRC".
		*
		* Connection ID is Trillian's internal ID for this connection; you'll need this stored somewhere.
		*
		* Name is the screenname/hotmail passport/whatever.
		*
		* Status is one of "Online" / "Offline" / "Away" / etc... can basically be anything Trillian can be.
		*/

	AccountList* cur;
	int indx = -1;

	if(proto_supports_otr(entry->medium))
	{
		cur = new AccountList;
		cur->accountname = reinterpret_cast<char*>(malloc(strlen(entry->name)+1));
		cur->medium = reinterpret_cast<char*>(malloc(strlen(entry->medium)+1));
			
		//_snprintf_s(cur->accountname, strlen(cur->accountname) + 1, _TRUNCATE, "%s", entry->name);
		free((cur->accountname));
		cur->accountname = _strdup(entry->name);

		//_snprintf(cur->medium, strlen(cur->medium) + 1, "%s", entry->medium);
		free((cur->medium));
		cur->medium = _strdup(entry->medium);

		cur->connection_id = entry->connection_id;

		_snprintf_s(buffer, 1024, 1023, "%s (%s)", entry->name, entry->medium);
		indx = cDlg->hAccounts.AddString(_T(buffer));

		if (indx >= 0)
			cDlg->hAccounts.SetItemDataPtr(indx, reinterpret_cast<void*>(cur));
	} 
	// MessageBox(HWND_DESKTOP, buffer, "Enumerated Connection", MB_OK);

	return 0;
}

HWND CConfigDlg::Create(HWND hWndParent, LPARAM dwInitParam)
{
	connection_enum_t cet;

	hWnd = CDialogPrefs::Create(hWndParent, dwInitParam);

	hAccounts.Attach(::GetDlgItem(hWnd, IDC_ACCOUNT_LIST));
	hStatus.Attach(::GetDlgItem(hWnd, IDC_KEY_FINGERPRINT));
	hGenerate.Attach(::GetDlgItem(hWnd, IDC_GENERATE_KEY));

	hEnabled.Attach(::GetDlgItem(hWnd, IDC_PRVMSGING));
	hAutoNeg.Attach(::GetDlgItem(hWnd, IDC_AUTOINIT));
	hRequired.Attach(::GetDlgItem(hWnd, IDC_REQPRIVMSG));

	hAutoClose.Attach(::GetDlgItem(hWnd, IDC_AUTOCLOSE));
	hSaveHistory.Attach(::GetDlgItem(hWnd, IDC_SAVEPRIVATEHISTORY)); // Added by Thisita 10/20/2011
	hOpenHistoryDlg.Attach(::GetDlgItem(hWnd, IDC_HISTORYFILEOPENBTN)); // Added by Thisita 10/20/2011
	hHistoryStorageDir.Attach(::GetDlgItem(hWnd, IDC_HISTORYFILELOCATION)); // Added by Thisita 10/20/2011
	hAutoCloseWithWindow.Attach(::GetDlgItem(hWnd, IDC_AUTOCLOSEWITHWINDOW)); // Added by Thisita 11/8/11

	// BEGIN Added by Thisita 10/20/11
	if(tmp_config->GetSaveHistory())
	{
		hSaveHistory.SetCheck(1);
		hOpenHistoryDlg.EnableWindow(1);
		hHistoryStorageDir.EnableWindow(1);
		hHistoryStorageDir.SetWindowTextA(tmp_config->GetHistoryDirectory());
	}
	else
	{
		hSaveHistory.SetCheck(0);
		hOpenHistoryDlg.EnableWindow(0);
		hHistoryStorageDir.EnableWindow(0);
		hHistoryStorageDir.SetWindowTextA("");
	} // END added by Thisita 10/20/11

	if(tmp_config->GetEnabled())
	{
		hEnabled.SetCheck(1);
		hAutoNeg.EnableWindow(TRUE);

		if(tmp_config->GetAutoNegotiate())
		{
			hAutoNeg.SetCheck(1);
			hRequired.EnableWindow(TRUE);

			if(tmp_config->GetRequired())
				hRequired.SetCheck(1);
		}
		else
		{
			hRequired.SetCheck(0);
			hRequired.EnableWindow(FALSE);
		}
	}
	else
	{
		hAutoNeg.SetCheck(0);
		hAutoNeg.EnableWindow(FALSE);

		hRequired.SetCheck(0);
		hRequired.EnableWindow(FALSE);
	}

	if(tmp_config->GetAutoClose())
		hAutoClose.SetCheck(1);
	else
		hAutoClose.SetCheck(0);

	/* Added by Thisita 11/8/11 */
	if(tmp_config->GetAutoCloseWithWindow())
		hAutoCloseWithWindow.SetCheck(1);
	else
		hAutoCloseWithWindow.SetCheck(0);

	/* To Do:
	 *
	 * - Populate the IDC_ACCOUNT_LIST combo box.
	 * - Display the key fingerprint in IDC_KEY_FINGERPRINT
	 * - Change the caption of IDB_GENERATE_KEY if key exists
	 * - Bind the 3 check boxes together in a 'cascade' fashion
	 */

	cet.callback = connectCallback;
	cet.medium = "all";					// Can also be "AIM", "ICQ", etc.
	cet.data = this;
		
	plugin_send(MYGUID, "connectionEnumerate", &cet);

	return hWnd;
}

BOOL CConfigDlg::DestroyWindow()
{
	AccountList* cur = accts;
	int indx = hAccounts.GetCount();
	int err = 0;

	if(indx == CB_ERR) 
		return CDialogPrefs::DestroyWindow(); /* Error occured. Assume this means no items? */

	for(int i = 0; i < indx; i++)
	{
		/* Delete the first element. */
		cur = reinterpret_cast<AccountList*>(hAccounts.GetItemDataPtr(0));
		free((cur->accountname));
		free((cur->medium));
		free(cur);
		cur = NULL;

		/* Delete that entry all together. This pushes all
			* the indexes up one.
			*/
		err = hAccounts.DeleteString(static_cast<UINT>(0));
	}

	return CDialogPrefs::DestroyWindow();
}


LRESULT CConfigDlg::OnGenerateKey(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	/* This command button is likely to spawn a modal dialog
	 * box that informs the user that a new key is being
	 * generated for the requested account. This command
	 * button will need to pull the account information from
	 * the CComboBox IDC_ACCOUNT_LIST and generate the
	 * key for it.
	 *
	 * This may utilize the IDC_KEY_FINGERPRINT field that is set up
	 * for display of the fingerprint for the given account.
	 */

	int indx = hAccounts.GetCurSel();
	AccountList* cur = accts;
	char* account_name = NULL;
	CNewFingerDlg* finger;
	if(indx < 0)
		return 0;

	cur = reinterpret_cast<AccountList*>(hAccounts.GetItemDataPtr(indx));

	/* Pull our own account name
		* Create a new dialog box for this.
		* Create the window, give it our name. Show it.
		* Fire off the generation of the private key.
		* Cleanup.
		*
		* This data structure will clean itself up when it
		* is destroyed
		*/
	account_name = accountNameLookup(cur->medium, cur->connection_id);

	finger = new CNewFingerDlg;

	finger->Create(hWnd, reinterpret_cast<LPARAM>(account_name));
	finger->ShowWindow(TRUE);
	finger->GenerateFingerprint(cur->accountname, cur->medium);

	free(account_name);
	account_name = NULL;

	return 0;
}

LRESULT CConfigDlg::OnSelectChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int indx = hAccounts.GetCurSel();
	AccountList* cur = accts;
	char* fp = NULL;
	char fp_buf[45] = {0};
	char fp_msg[100] = {0};

	if(indx >= 0)
	{
		cur = reinterpret_cast<AccountList*>(hAccounts.GetItemDataPtr(indx));

		fp = otrl_privkey_fingerprint(userstate, fp_buf, cur->accountname, cur->medium);

		if(fp)
		{
			_snprintf_s(fp_msg, 100, 99, "Fingerprint: %s", fp_buf);
			hGenerate.EnableWindow(FALSE);
		}
		else
		{
			_snprintf_s(fp_msg, 100, 99, "No key available.");
			hGenerate.EnableWindow(TRUE);
		}
		hStatus.SetWindowText(fp_msg);
	}
	else
	{
		hStatus.SetWindowText("No account present");
		hGenerate.EnableWindow(FALSE);
	}

	return 0;
}

LRESULT CConfigDlg::OnChangePrivMessaging(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(hEnabled.GetCheck())
	{
		tmp_config->SetEnabled(1);

		hAutoNeg.EnableWindow(TRUE);
	}
	else
	{
		tmp_config->SetEnabled(0);
		tmp_config->SetAutoNegotiate(0);
		tmp_config->SetRequired(0);

		hAutoNeg.SetCheck(0);
		hAutoNeg.EnableWindow(FALSE);

		hRequired.SetCheck(0);
		hRequired.EnableWindow(FALSE);
	}

	return 0;
}

LRESULT CConfigDlg::OnChangeAutoNegotiate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(hAutoNeg.GetCheck())
	{
		tmp_config->SetAutoNegotiate(1);
		hRequired.EnableWindow(TRUE);
	}
	else
	{
		tmp_config->SetAutoNegotiate(0);
		tmp_config->SetRequired(0);

		hRequired.SetCheck(0);
		hRequired.EnableWindow(FALSE);
	}

	return 0;
}

LRESULT CConfigDlg::OnChangeRequired(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(hRequired.GetCheck())
		tmp_config->SetRequired(1);
	else
		tmp_config->SetRequired(0);

	return 0;
}

LRESULT CConfigDlg::OnChangeAutoClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(hAutoClose.GetCheck())
		tmp_config->SetAutoClose(1);
	else
		tmp_config->SetAutoClose(0);

	return 0;
}

int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if(uMsg == BFFM_INITIALIZED)
		SendMessage(hWnd,BFFM_SETSELECTION, TRUE, lpData);
	return 0;
}

bool BrowseForFolder1(HWND parent, char* path, const char* title)
{
	char name[_MAX_PATH];
	BROWSEINFO info={parent, NULL, name, title, BIF_USENEWUI, BrowseCallbackProc,
		reinterpret_cast<LPARAM>(path)};
	LPITEMIDLIST items = SHBrowseForFolder(&info);
	if(!items)
		return false;

	SHGetPathFromIDList(items, path);
	LPMALLOC pMalloc;
	SHGetMalloc(&pMalloc);
	pMalloc->Free(items);
	pMalloc->Release();

	return true;
}

LRESULT CConfigDlg::OnChooseDirButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char* path = new char[_MAX_PATH];
	BROWSEINFO info={NULL, NULL, NULL, "Choose a Directory....", BIF_USENEWUI, BrowseCallbackProc,
		reinterpret_cast<LPARAM>(path)};
	LPITEMIDLIST items = SHBrowseForFolder(&info);
	if (!items)
		return false;

	SHGetPathFromIDList(items,path);
	LPMALLOC pMalloc;
	SHGetMalloc(&pMalloc);
	pMalloc->Free(items);
	pMalloc->Release();

	hHistoryStorageDir.SetWindowTextA(path);
	tmp_config->SetHistoryDirectory(path);

	return 0;
}

LRESULT CConfigDlg::OnChangeSaveHistory(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (hSaveHistory.GetCheck())
	{
		tmp_config->SetSaveHistory(1);
		hOpenHistoryDlg.EnableWindow(1);
		hHistoryStorageDir.EnableWindow(1);
	}
	else
	{
		hOpenHistoryDlg.EnableWindow(0);
		hHistoryStorageDir.EnableWindow(0);
		tmp_config->SetSaveHistory(0);
	}
	return 0;
}

LRESULT CConfigDlg::OnChangeHistoryDirText(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(!hSaveHistory.GetCheck())
		return 0;

	CString txt = "";
	hHistoryStorageDir.GetWindowTextA(txt);
	tmp_config->SetHistoryDirectory(txt);

	return 0;
}


LRESULT CConfigDlg::OnChangeAutoCloseWithWindow(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	tmp_config->SetAutoCloseWithWindow(hAutoCloseWithWindow.GetCheck());
	return 0;
}
