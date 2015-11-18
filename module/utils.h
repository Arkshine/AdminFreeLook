//
// AdminFreeLook AMX Mod X Module
// Copyright (C) Vincent Herbet (Arkshine)
//
// This software is licensed under the GNU General Public License, version 2.
// For full license details, see LICENSE file.
//

namespace Util
{
	void *FindAddress(const char *entry, const char *library = "mod");
	int ReadFlags(const char* c, int& numFlags);
	bool IsAdmin(int index);
	int GetFlagPosition(int flag);
	int GetUserMode(int& numFlags);
	int GetNextUserMode(int currentMode, int allowedModes);
};