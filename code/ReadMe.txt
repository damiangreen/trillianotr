Project: Trillian OTR - Risen
License: GNU General Public License 3
Developer: Ian Z Ledrick aka Thisita
Developer Contact: thisita@users.sourceforge.net
State: Beta
Version: 0.6.4 Build x

About:
This project is a continuation of the Trillian OTR plugin created by Kittyfox Communications.

What does it do:
Trillian Off-the-Record is a cross-platform encryption plugin that provides 
Off the Record messaging support in Trillian. Unlike SecureIM,
this style of encryption allows both authentication that the source of the 
messages is a known individual as well as plausable deniability. Messages sent 
during an encrypted session are secure, but messages can easily be forged after 
the fact, preventing them from being easily attributable to any one person.

For more information, please see http://www.cypherpunks.ca/otr/

To learn about the original plugin, please see http://trillianotr.kittyfox.net/

Installation:
- Copy the TrillianOTR.dll file into the plugins folder found in your Trillian installation directory.

Uninstallation:
- Delete the TrillianOTR.dll file from your plugins folder found in your Trillian installation directory.

Changelog:
Version 0.7.0.x
- TODO

Version 0.6.3.x
- Beginnings of SMP support
- Code improvements in message handling
- Users are now notified of automatically terminated sessions
- Upgraded license to GPL v3
- Fixed Auto-close session with window bug
- Fixed the second load crash
- Minor source reorganization
- Fixed language change crash
- Fixed crash when exiting a window using a medium not supporting OTR
- Speed improvements with menu handling code
- Fixed a bug which caused certain MSN users to see only the encrypted messages

Version 0.6.2.x
- Feature: Added option to end OTR sessions when the user closes a message window
- Hardstopper fix: Fixed a crash at load for Trillian 5.1
- Bug fix: Fixed crashes and bugginess for users with multiple accounts under the same medium
- Hardstopper fix: Fixed a crash during use of the preferences dialog for vanilla users

Version 0.6.1.x
- Internal code restructuring and reorganization
- Added Facebook support
- OTR messages are displayed as system messages instead of conversational messages
- Multiple performance increasing changes
- Removed untested strings from MSN and Yahoo
- Added option to open a previous encrypted history log
- Added a DLLMain() function
- Security updates to string handling
- Fixed an access violation at exit
- History is now saved with folder format "dd-mmm" format
- Users now recieve a notification confirming that the OTR session has ended if they are the terminating party
- Implemented workaround for the no text on terminate bug
Known Bugs
- Workaround removed notification that the session has ended
- Crash at load for Trillian beta
- OTR Sessions are not closing when a user exits Trillian
- Unconfirmed incompatability with Pidgin OTR
- Intermittent history bug (Trillian logging unencrypted OTR messages)

Version 0.6.0.1
- Initial Release
