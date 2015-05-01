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

#ifndef DLGCONFIG_H
#define DLGCONFIG_H

typedef struct _accntList
{
	int connection_id;
	char* accountname;
	char* medium;
} AccountList;

class CConfigDlg : public CDialogPrefs
{
	private:
		HWND hWnd;
		CStatic hStatus;
		CButton hGenerate;

		CButton hEnabled;
		CButton hAutoNeg;
		CButton hRequired;

		CButton hAutoClose;
		CButton hSaveHistory; // Added by Thisita 10/20/11
		CButton hOpenHistoryDlg; // Added by Thisita 10/20/11
		CEdit hHistoryStorageDir; // Added by Thisita 10/20/11
		CButton hAutoCloseWithWindow; // Added by Thisita 11/8/11

	public:
		CComboBox hAccounts;
		AccountList* accts;

		CConfigDlg() : CDialogPrefs(IDD_PREFS_CONFIG), accts(NULL)
		{
		}

		BEGIN_MSG_MAP(CConfigDlg);		
			COMMAND_HANDLER(IDC_ACCOUNT_LIST, CBN_SELCHANGE, OnSelectChange);
			COMMAND_ID_HANDLER(IDC_PRVMSGING, OnChangePrivMessaging);
			COMMAND_ID_HANDLER(IDC_AUTOINIT, OnChangeAutoNegotiate);
			COMMAND_ID_HANDLER(IDC_REQPRIVMSG, OnChangeRequired);
			COMMAND_ID_HANDLER(IDC_AUTOCLOSE, OnChangeAutoClose);
			COMMAND_ID_HANDLER(IDC_GENERATE_KEY, OnGenerateKey);
			COMMAND_ID_HANDLER(IDC_HISTORYFILEOPENBTN, OnChooseDirButton);
			COMMAND_ID_HANDLER(IDC_SAVEPRIVATEHISTORY, OnChangeSaveHistory);
			COMMAND_ID_HANDLER(IDC_HISTORYFILELOCATION, OnChangeHistoryDirText);
			COMMAND_ID_HANDLER(IDC_AUTOCLOSEWITHWINDOW, OnChangeAutoCloseWithWindow);
		END_MSG_MAP();

		virtual HWND Create(HWND hWndParent, LPARAM dwInitParam = 0);
		virtual BOOL DestroyWindow();

		LRESULT OnGenerateKey(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnSelectChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnChangePrivMessaging(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnChangeAutoNegotiate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnChangeRequired(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnChangeAutoClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnChangeSaveHistory(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnChangeHistoryDirText(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnChooseDirButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnChangeAutoCloseWithWindow(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		void RefreshSelectedAccount()
		{
			hAccounts.SetCurSel(hAccounts.GetCurSel());
		}
};

#endif
