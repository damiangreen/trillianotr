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

#ifndef DLGUNKNOWNFINGERPRINT_H
#define DLGUNKNOWNFINGERPRINT_H

class CUnknownFingerDlg : public CDialogImpl<CUnknownFingerDlg>
{
	private:
		HWND	hWnd;
		CStatic hTitle;
		CStatic hFingerprint;
		CButton hOk;

	public:
		int		IDD;

		CUnknownFingerDlg() : IDD(IDD_UNKNOWN_FINGERPRINT)
		{
		}

		const int GetResource()
		{
			return IDD;
		}

		BEGIN_MSG_MAP(CUnknownFingerDlg);
			COMMAND_ID_HANDLER(IDOK, OnFinishDlg);
		END_MSG_MAP();

		virtual HWND Create(HWND hWndParent, LPARAM dwInitParam = 0);
		LRESULT OnFinishDlg(WORD, WORD, HWND, BOOL&);
		void OnFinalMessage(HWND hWnd)
		{
			delete this;
		}
		void PopulateMessages(const char*, const char*, const char*, unsigned char [20]);
};

#endif
