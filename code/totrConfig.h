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

#ifndef TOTRCONFIG_H
#define TOTRCONFIG_H

class TotrConfig
{
	private:
		int enabled;
		int autonegotiate;
		int required;
		int autoclose;
		int savehistory; // Added by Thisita 10/20/2011
		int autoclosewithwindow; // Added by Thisita 11/8/11

		char* home_directory;
		char* privkeyfile;
		char* storekeyfile;
		char* configfile;

		char* version;
		char* shortver;
		char* updatever;

		char* historyDir; // Added by Thisita 10/20/2011

		/**
		 * Parses Trillian OTR settings within an XML tree.
		 *
		 * @param xml An XML tree containing the settings.
		 */
		void ParseSettings(xml_tree_t*);

		/**
		 * Removes the HTML markup code from a string.
		 *
		 * @param output A pointer to a string to contain the stipped input.
		 * This should be initialized to NULL, as the function will allocate memory
		 * for the string.
		 * @param input The string to remove HTML from.
		 */
		void stripHTML(char*, const char*);
	public:
		/**
		 * Constructs a Trillian OTR Configuration object.
		 */
		TotrConfig();

		/**
		 * Constructs a Trillian OTR Configuration object from another
		 * Trillian OTR Configuration object.
		 *
		 * @param x The Configuration object to be copied.
		 */
		TotrConfig(TotrConfig &x);

		/**
		 * Destroys the Trillian OTR Configuration object.
		 */
		~TotrConfig();

		/**
		 * @brief Loads the Trillian configuration.
		 *
		 * Loads the Trillian configuration from the default file.
		 */
		void	LoadConfig();

		/**
		 * @brief Saves the Trillian configuration.
		 *
		 * Saves the Trillian configuration settings as an XML tree
		 * to the default file.
		 *
		 * @return TRUE on success.
		 */
		BOOL	SaveConfig();

		/**
		 * Gets if OTR is enabled.
		 *
		 * @return TRUE if enabled, FALSE if not.
		 */
		int	GetEnabled() const;

		/**
		 * Sets whether or not OTR is enabled.
		 *
		 * @param on TRUE to enable, FALSE to disable.
		 */
		void SetEnabled(const int);

		/**
		 * Gets if Auto Negotiation is enabled.
		 *
		 * @return TRUE if enabled, FALSE if not.
		 */
		int	GetAutoNegotiate() const;

		/**
		 * Sets whether or not Auto Negotiation is enabled.
		 *
		 * @param on TRUE to enable, FALSE to disable.
		 */
		void SetAutoNegotiate(const int);

		/**
		 * Gets if OTR is required.
		 *
		 * @return TRUE if enabled, FALSE if not.
		 */
		int GetRequired() const;

		/**
		 * Sets whether or not OTR is required.
		 *
		 * @param on TRUE to enable, FALSE to disable.
		 */
		void SetRequired(const int);

		/**
		 * Gets if OTR should automatically close ended sessions.
		 *
		 * @return TRUE if enabled, FALSE if not.
		 */
		int GetAutoClose() const;

		/**
		 * Sets whether or not OTR should automatically close ended sessions.
		 *
		 * @param on TRUE to enable, FALSE to disable.
		 */
		void SetAutoClose(const int);

		/**
		 * Gets if OTR should automatically close sessions when
		 * the message window is closed.
		 *
		 * @return TRUE if enabled, FALSE if not.
		 */
		int GetAutoCloseWithWindow() const;

		/**
		 * Sets whether or not OTR should automatically close sessions when
		 * the message window is closed.
		 *
		 * @param on TRUE to enable, FALSE to disable.
		 */
		void SetAutoCloseWithWindow(const int);

		/**
		 * Gets if OTR history should be logged.
		 *
		 * @return TRUE if enabled, FALSE if not.
		 */
		int GetSaveHistory() const;

		/**
		 * Sets whether or not OTR history should be logged.
		 *
		 * @param on TRUE to enable, FALSE to disable.
		 */
		void SetSaveHistory(const int);

		/**
		 * Gets the OTR history log directory.
		 *
		 * @return A string containing the file path.
		 */
		char* GetHistoryDirectory() const;

		/**
		 * Sets the history directory string.
		 *
		 * @param directory String containing the path.
		 */
		void SetHistoryDirectory(const char*);

		/**
		 * Gets the home directory.
		 *
		 * @return A string containing the home directory.
		 */
		char* GetHomeDirectory() const;

		/**
		 * Sets the home directory string.
		 *
		 * @param directory String containing the path.
		 */
		void SetHomeDirectory(const char*);

		/**
		 * Gets the Private Key filename.
		 *
		 * @return A string containing the file name.
		 */
		char* GetPrivKeyFilename() const;

		/**
		 * Sets the Private Key file name string.
		 *
		 * @param filename String containing the file name.
		 */
		void SetPrivKeyFilename(const char*);

		/**
		 * Gets the store filename.
		 *
		 * @return A string containing the file name.
		 */
		char* GetStoreFilename() const;

		/**
		 * Sets the store file name string.
		 *
		 * @param filename String containing the file name.
		 */
		void SetStoreFilename(const char*);

		/**
		 * Gets the config filename.
		 *
		 * @return A string containing the file name.
		 */
		char* GetConfigFilename() const;

		/**
		 * Gets the plugin version.
		 *
		 * @return A string containing the plugin version.
		 */
		char* GetVersion() const
		{
			return version;
		}

		/**
		 * Gets the short plugin version.
		 *
		 * @return A string containing the short plugin version.
		 */
		char* GetShortVersion() const
		{
			return shortver;
		}

		/**
		 * Gets the update version.
		 *
		 * @return A string containing the update version.
		 */
		char* GetUpdateVersion() const
		{
			return updatever;
		}

		/**
		 * Overrides the assignment operator.
		 */
		TotrConfig& operator=(const TotrConfig &x);

		/**
		 * Appends a message to the OTR history logs.
		 *
		 * @param message The message text to be saved.
		 * @param medium The medium of the conversation.
		 * @param disp_name The name of the message sender.
		 * @param name The name of the conversational partner.
		 * 
		 * @return A string containing the file name.
		 */
		bool appendHistory(const char*,const char*,const char*, const char*);
};

#endif
