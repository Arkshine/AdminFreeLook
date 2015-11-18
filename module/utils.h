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

	int ReadFlags(const char* flags, int *numFlags = nullptr);

	bool IsAdmin(edict_t *pPlayer);
	bool ShouldRunCode();

	int GetFlagPosition(int flag);
	int GetUserMode(int *numFlags = nullptr);
	int GetNextUserMode(edict_t *pPlayer, int userFlags);
};