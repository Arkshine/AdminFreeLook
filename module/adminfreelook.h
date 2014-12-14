//
// AMX Mod X, based on AMX Mod by Aleksander Naszko ("OLO").
// Copyright (C) The AMX Mod X Development Team.
//
// This software is licensed under the GNU General Public License, version 3 or higher.
// Additional exceptions apply. For full license details, see LICENSE.txt or visit:
//     https://alliedmods.net/amxmodx-license

//
// AdminFreeLook Module
//

#ifndef ADMINFREELOOK_H
#define ADMINFREELOOK_H

#include <amxxmodule.h>
#include <MemoryUtils.h>
#include <CDetour/detours.h>
#include <am-string.h>
#include <am-vector.h>

inline edict_t* INDEXENT2(int iEdictNum)
{
	if (iEdictNum >= 1 && iEdictNum <= gpGlobals->maxClients)
		return MF_GetPlayerEdict(iEdictNum);
	else
		return (*g_engfuncs.pfnPEntityOfEntIndex)(iEdictNum);
}

void EnableDetours();
void DisableDetours();
void LogOnError();

void *UTIL_FindAddressFromEntry(const char *entry, bool isHidden, const char *library = "mod");
int   UTIL_PrivateToIndex(const void* pdata);
int   UTIL_ReadFlags(const char* c, int& numFlags);
bool  UTIL_IsAdmin(int index);
int   UTIL_GetUserMode(int& numFlags);
int   UTIL_GetNextUserMode(int currentMode, int allowedModes = 0);
int   UTIL_GetFlagPosition(int flag);

extern cvar_t* CvarFreeLookEnable;
extern cvar_t* CvarFreeLookFlags;
extern cvar_t* CvarFreeLookMode;

extern enginefuncs_t* g_pengfuncsTable;
extern int CurrentPlayerIndex;

#endif // ADMINFREELOOK_H