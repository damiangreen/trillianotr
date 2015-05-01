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

/* Bring in our standard header */
#include "stdafx.h"


TotrConfig::TotrConfig()
{
	enabled = autonegotiate = autoclose = 1;
	required = savehistory = autoclosewithwindow = 0;
	home_directory = privkeyfile = storekeyfile = configfile = NULL;
	version = shortver = updatever = NULL;
}

TotrConfig::TotrConfig(TotrConfig &x)
{
	enabled = x.enabled;
	autonegotiate = x.autonegotiate;
	required = x.required;
	autoclose = x.autoclose;
	savehistory = x.savehistory;  // Added by Thisita 10/20/2011
	autoclosewithwindow = x.autoclosewithwindow;

	home_directory = privkeyfile = storekeyfile = configfile = historyDir = NULL; 
	version = shortver = updatever = NULL;

	if (x.home_directory)
		home_directory = _strdup(x.home_directory);
	if (x.privkeyfile)
		privkeyfile = _strdup(x.privkeyfile);
	if (x.storekeyfile)
		storekeyfile = _strdup(x.storekeyfile); 
	if (x.configfile)
		configfile = _strdup(x.configfile); 
	if(x.historyDir)
		historyDir = _strdup(x.historyDir); // Added by Thisita 10/20/2011
}

TotrConfig::~TotrConfig()
{
	free(home_directory);
	free(privkeyfile);
	free(storekeyfile);
	free(configfile);
	free(version);
	free(shortver);
	free(updatever);
	free(historyDir); // Added by Thisita 10/20/2011
}

void TotrConfig::ParseSettings(xml_tree_t *xml)
{
	xml_tag_t* tag = NULL;
	if(xml)
		tag = xml->root_tag;

	/* Decend to our root tag */
	if(tag)
		tag = tag->children;
	else
		tag = NULL;

	/* Confirm this is our XML schema */
	if(tag && !stricmp(tag->type, "tag") && !stricmp(tag->text, "trillianotr"))
		tag = tag->children;

	/* Ok, now search! */
	while(tag)
	{
		if(!stricmp(tag->type, "tag"))
		{
			if(!stricmp(tag->text, "enabled"))
			{
				if(tag->children)
				{
					if(!stricmp(tag->children->text, "yes")) 
						enabled = 1;
					else
						enabled = 0;
				}
			}
			else if(!stricmp(tag->text, "autonegotiate"))
			{
				if(tag->children)
				{
					if(!stricmp(tag->children->text, "yes")) 
						autonegotiate = 1;
					else
						autonegotiate = 0;
				}
			}
			else if(!stricmp(tag->text, "required"))
			{
				if(tag->children)
				{
					if(!stricmp(tag->children->text, "yes")) 
						required = 1;
					else
						required = 0;
				}
			}
			else if(!stricmp(tag->text, "autoclose"))
			{
				if(tag->children)
				{
					if(!stricmp(tag->children->text, "yes")) 
						autoclose = 1;
					else
						autoclose = 0;
				}
			}
			else if(!stricmp(tag->text, "savehistory"))
			{ // BEGIN Added by Thisita 10/20/2011
				if(tag->children)
				{
					if(!stricmp(tag->children->text, "yes"))
						savehistory = 1;
					else
						savehistory = 0;
				}
			}
			else if(!stricmp(tag->text, "historyDir"))
			{
				if(tag->children)
				{
					historyDir = _strdup(tag->children->text);
				}
			} // END Added by Thisita 10/20/2011
			else if(!stricmp(tag->text, "autoclosewithwindow"))
			{
				if(tag->children)
				{
					if(!stricmp(tag->children->text, "yes"))
						autoclosewithwindow = 1;
					else
						autoclosewithwindow = 0;
				}
			}
		}

		tag = tag->next_tag;
	}
}

void TotrConfig::LoadConfig()
{
	xml_tree_t xml; // Rewritten by Thisita 10/23/11
	HRSRC src;
	char ver[50] = {0};
	int		majver;
	int		minver;
	int		patch;
	int		build;

	trillianInitialize(xml);
	
	char fileName[_MAX_DIR];
	GetModuleFileName(GetModuleHandle("TrillianOTR"), fileName, _MAX_DIR);

	DWORD dwHandle;
	DWORD dwLen;
	unsigned int BufLen;
	char* lpData;
	VS_FIXEDFILEINFO* pFileInfo;

	dwLen = GetFileVersionInfoSize(fileName, &dwHandle);
	lpData = reinterpret_cast<char*>(malloc(dwLen));
	GetFileVersionInfo( fileName, dwHandle, dwLen, lpData);

	VerQueryValue(lpData, "\\", reinterpret_cast<LPVOID*>(&pFileInfo),
		reinterpret_cast<PUINT>(&BufLen));
	majver = HIWORD(pFileInfo->dwFileVersionMS);
	minver = LOWORD(pFileInfo->dwFileVersionMS);
	patch = HIWORD(pFileInfo->dwFileVersionLS);
	build = LOWORD(pFileInfo->dwFileVersionLS);

	free(lpData);

	_snprintf_s(ver, 50, 49, "%d.%d.%d (Build %d)",
				majver, minver, patch, build);
	//version = reinterpret_cast<char*>(malloc(strlen(ver)+1));
	//strncpy_s(version, strlen(version) + 1, ver, _TRUNCATE);
	version = _strdup(ver);

	_snprintf_s(ver, 50, 49, "%d.%d.%d",
				majver, minver, patch);
	//shortver = reinterpret_cast<char*>(malloc(strlen(ver)+1));
	//strcpy(shortver, ver);
	shortver = _strdup(ver);

	_snprintf_s(ver, 50, 49, "%d.%d.%d.%d",
				majver, minver, patch, build);
	//updatever = reinterpret_cast<char*>(malloc(strlen(ver)+1));
	//strcpy(updatever, ver);
	updatever = _strdup(ver);

	xml.filename = totr_config.GetConfigFilename();

	if (!plugin_send(MYGUID, "xmlGenerateTree", &xml))
		ParseSettings(&xml);

	plugin_send(MYGUID, "xmlFreeTree", &xml);
}

/* Our basic schema
 *
 * <trillianotr>
 *   <enabled>yes/no</enabled>
 *   <autonegotiate>yes/no</autonegotiate>
 *   <required>yes/no</required>
 *   <autoclose>yes/no</autoclose>
 *   <savehistory>yes/no</savehistory>   <- Added by Thisita 10/20/2011
 *   <historyDir>dir</historyDir>        <- Added by Thisita 10/20/2011
 ****** Start Future Use *******
 *   <privkeyfile>filename</privkeyfile>
 *   <storekeyfile>filename</storekeyfile>
 *   <account>
 *     <username>username</username>
 *     <enabled>yes/no</enabled>
 *     <autonegotiate>yes/no</autonegotiate>
 *     <required>yes/no</required>
 *   </account> ...
 ****** End Future Use *******
 * </trillianotr>
 */

BOOL TotrConfig::SaveConfig()
{
	std::ofstream file(totr_config.GetConfigFilename());
	if(file.fail())
		return FALSE;

	file << "<trillianotr>" << std::endl;
	file << "<enabled>" << (enabled ? "yes" : "no") << "</enabled>" << std::endl;
	file << "<autonegotiate>" << (autonegotiate ? "yes" : "no") << "</autonegotiate>" << std::endl;
	file << "<required>" << (required ? "yes" : "no") << "</required>" << std::endl;
	file << "<autoclose>" << (autoclose ? "yes" : "no") << "</autoclose>" << std::endl;
	file << "<savehistory>" << (savehistory ? "yes" : "no") << "</savehistory>" << std::endl;
	file << "<historyDir>" << (historyDir ? historyDir : "") << "</historyDir>" << std::endl; // Patch suggested by Thisita 11/7/11
	file << "<autoclosewithwindow>" << (autoclosewithwindow ? "yes" : "no") << "</autoclosewithwindow>" << std::endl;
	file << "</trillianotr>" << std::endl;

#if 0
	FILE *outfile = NULL;

	outfile = fopen(totr_config.GetConfigFilename(), "w");

	if (!outfile)
		return FALSE;

	fprintf(outfile, "<trillianotr>\r\n");
	fprintf(outfile, "  <enabled>%s</enabled>\r\n", (enabled ? "yes" : "no"));
	fprintf(outfile, "  <autonegotiate>%s</autonegotiate>\r\n", (autonegotiate ? "yes" : "no"));
	fprintf(outfile, "  <required>%s</required>\r\n", (required ? "yes" : "no"));
	fprintf(outfile, "  <autoclose>%s</autoclose>\r\n", (autoclose ? "yes" : "no"));
	fprintf(outfile, "	<savehistory>%s</savehistory>\r\n",(savehistory ? "yes" : "no"));  // Added by Thisita 10/20/2011
	fprintf(outfile, "	<historyDir>%s</historyDir>\r\n",historyDir); // Added by Thisita 10/20/11
	fprintf(outfile, "</trillianotr>\r\n");
    fclose(outfile);
#endif

	return TRUE;
}

/* Added by Thisita 11/8/11 */
int TotrConfig::GetAutoCloseWithWindow() const
{
	return autoclosewithwindow;
}

void TotrConfig::SetAutoCloseWithWindow(const int on)
{
	autoclosewithwindow = on;
}

/* Added by Thisita 10/20/2011 BEGIN */
int TotrConfig::GetSaveHistory() const
{
	return savehistory;
}

void TotrConfig::SetSaveHistory(const int on)
{
	savehistory = on;
}

void TotrConfig::stripHTML(char* output, const char* input)
{
	CString buf = "";
	unsigned int i = 0;
	while(i < strlen(input))
	{
		if(input[i] == '<')
		{
			while(input[i] != '>')
				i++;
			i++;
			if(i >= strlen(input))
				break;
			continue;
		}
		buf.AppendChar(input[i]);
		i++;
	}
	buf.Replace("&lt;", "<");
	buf.Replace("&gt;", ">");
	buf.Replace("&amp;", "&");
	buf.Replace("&qhot;", "\"");
	buf.Replace("&ndash;", "-");

	strncpy_s(output, strlen(output) + 1, buf.GetString(), _TRUNCATE);
}

bool TotrConfig::appendHistory(const char* message, const char* medium,const char* disp_name, const char* name)
{
	if(!message || !medium || !disp_name || !name)
		return FALSE;
	char filename[_MAX_PATH];
	char date[256];
	const time_t curTime = time(NULL);

	strftime(date,256,"%d-%b",localtime(&curTime));
	_snprintf(filename, _MAX_PATH, "%s\\%s",historyDir,medium);
	filename[_MAX_PATH-1] = '\0';
	_mkdir(filename);

	_snprintf(filename,_MAX_PATH, "%s\\%s\\%s",historyDir,medium,date);
	filename[_MAX_PATH-1] = '\0';
	_mkdir(filename);

	_snprintf(filename, _MAX_PATH, "%s\\%s\\%s\\%s.txt",historyDir,medium,date,name);
	filename[_MAX_PATH-1] = '\0';

	std::ofstream file(filename, std::ios::app);
	if(file.fail())
	{
		file.open(filename);
		if(file.fail())
			return FALSE;
	}

	strftime(date, 256, "%x-%X", localtime(&curTime));

	char* mes = _strdup(message);
	stripHTML(mes, message);

	file << disp_name << " [" << date
		<< "]: " << mes << std::endl;
	free(mes);

#if 0
	FILE *outfile = NULL;
	outfile = fopen(filename, "a"); // Step 1: Get a handle to the file
	if (!outfile) outfile = fopen(filename, "w");
	if(!outfile)
	{
		MessageBox(NULL,filename,"FILE ERROR",MB_OK);
		return false; // Step 2: Bail if fail
	}
	strftime(date,256,"%x-%X",localtime(&curTime));
	char* mes = _strdup(message);
	stripHTML(mes,message);
	fprintf(outfile, "%s [%s]: %s\r\n",disp_name,date,mes);  // Step 3: Append w/eth is in message and set newline
    
	fclose(outfile); // Step 4: Clean up
	free(mes);
#endif

	// Step 5: ???

	return true; // Step 6: PROFIT
}

char* TotrConfig::GetHistoryDirectory() const
{
	return historyDir;
}

void TotrConfig::SetHistoryDirectory(const char* dir)
{
	free(historyDir);
	historyDir = _strdup(dir);
}
/* END Added by Thisita 10/20/2011 */



int	TotrConfig::GetEnabled() const
{
	return enabled;
}

void TotrConfig::SetEnabled(const int on)
{
	enabled = on;
}

int	TotrConfig::GetAutoNegotiate() const
{
	return autonegotiate;
}

void TotrConfig::SetAutoNegotiate(const int autoneg)
{
	autonegotiate = autoneg;
}

int	TotrConfig::GetRequired() const
{
	return required;
}

void TotrConfig::SetRequired(const int req)
{
	required = req;
}

int	TotrConfig::GetAutoClose() const
{
	return autoclose;
}

void TotrConfig::SetAutoClose(const int ac)
{
	autoclose = ac;
}

char* TotrConfig::GetHomeDirectory() const
{
	return home_directory;
}

void TotrConfig::SetHomeDirectory(const char* dir)
{
	if(!dir)
		return;
	char buf[_MAX_PATH];

	free(home_directory);
	home_directory = _strdup(dir);
	//home_directory = reinterpret_cast<char*>(malloc(strlen(dir)+1));
	//_snprintf(home_directory, strlen(home_directory) + 1, "%s", dir);

	/* When we set the homedir, if our Privkeyfile and Storekeyfile
		* filenames are empty, go ahead and populate them by default.
		* Don't overwrite if populated, though.
		*/
	if (!privkeyfile)
	{
		//privkeyfile = reinterpret_cast<char*>(malloc(strlen(home_directory)+strlen(PRIVKEYFILE)+1));
		_snprintf_s(buf, _MAX_PATH, _MAX_PATH - 1, "%s%s", home_directory, PRIVKEYFILE);
		privkeyfile = _strdup(buf);
	}

	if (!storekeyfile)
	{
		//storekeyfile = reinterpret_cast<char*>(malloc(strlen(home_directory)+strlen(STOREKEYFILE)+1));
		_snprintf_s(buf, _MAX_PATH, _MAX_PATH - 1, "%s%s", home_directory, STOREKEYFILE);
		storekeyfile = _strdup(buf);
	}

	if (!configfile)
	{
		//configfile = reinterpret_cast<char*>(malloc(strlen(home_directory)+strlen(CONFIGFILE)+1));
		_snprintf_s(buf, _MAX_PATH, _MAX_PATH - 1, "%s%s", home_directory, CONFIGFILE);
		configfile = _strdup(buf);
	}
}

char *TotrConfig::GetPrivKeyFilename() const
{
	return privkeyfile;
}

void TotrConfig::SetPrivKeyFilename(const char* filename)
{
	if(!filename)
		return;
	free(privkeyfile);
	char buf[_MAX_PATH];

	//privkeyfile = reinterpret_cast<char*>(malloc(strlen(filename)+1));

	if(home_directory)
	{
		_snprintf_s(buf, _MAX_PATH, _MAX_PATH -1, "%s%s", home_directory, filename);
		privkeyfile = _strdup(buf);
	}
	else
		privkeyfile = _strdup(filename);
		//_snprintf(privkeyfile, strlen(privkeyfile) + 1, "%s", filename);
}

char* TotrConfig::GetStoreFilename() const
{
	return storekeyfile;
}

void TotrConfig::SetStoreFilename(const char* filename)
{
	if(!filename)
		return;
	char buf[_MAX_PATH];

	free(storekeyfile);

	//storekeyfile = reinterpret_cast<char*>(malloc(strlen(filename)+1));

	if (home_directory)
	{
		_snprintf_s(buf, _MAX_PATH, _MAX_PATH - 1, "%s%s", home_directory, filename);
		storekeyfile = _strdup(filename);
	}
	else
		storekeyfile = _strdup(filename);
		//_snprintf(storekeyfile, strlen(storekeyfile) + 1, "%s", filename);
}

char* TotrConfig::GetConfigFilename() const
{
	return configfile;
}

TotrConfig& TotrConfig::operator=(const TotrConfig& x)
{
	enabled = x.enabled;
	autonegotiate = x.autonegotiate;
	required = x.required;
	autoclose = x.autoclose;
	savehistory = x.savehistory; // Added by Thisita 10/20/2011
	autoclosewithwindow = x.autoclosewithwindow;

	free(home_directory);
	free(privkeyfile);
	free(storekeyfile);
	free(historyDir);

	if (x.home_directory)
		home_directory = _strdup(x.home_directory);
	if (x.privkeyfile)
		privkeyfile = _strdup(x.privkeyfile);
	if (x.storekeyfile)
		storekeyfile = _strdup(x.storekeyfile);
	if (x.configfile)
		configfile = _strdup(x.configfile);
	if(x.historyDir)
		historyDir = _strdup(x.historyDir); // Added by Thisita 10/20/2011

	return *this;
}

