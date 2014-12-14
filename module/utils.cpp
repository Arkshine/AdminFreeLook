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

#include "adminfreelook.h"
#include "gamedatas.h"

void *UTIL_FindAddressFromEntry(const char *entry, bool isHidden, const char *library)
{
	void *addressInBase = NULL;
	void *finalAddress;

	if (strcmp(library, "mod") == 0)
	{
		addressInBase = (void *)MDLL_Spawn;
	}
	else if (strcmp(library, "engine") == 0)
	{
		addressInBase = (void *)gpGlobals;
	}

	finalAddress = NULL;

	if (*entry != '\\')
	{
#if defined(WIN32)

		MEMORY_BASIC_INFORMATION mem;

		if (VirtualQuery(addressInBase, &mem, sizeof(mem)))
		{
			finalAddress = g_MemUtils.ResolveSymbol(mem.AllocationBase, entry);
		}

#elif defined(__linux__) || defined(__APPLE__)

		Dl_info info;
		void *handle = NULL;

		if (dladdr(addressInBase, &info) != 0)
		{
			void *handle = dlopen(info.dli_fname, RTLD_NOW);
			if (handle)
			{
				if (isHidden)
				{
					finalAddress = g_MemUtils.ResolveSymbol(handle, entry);
				}
				else
				{
					finalAddress = dlsym(handle, entry);
				}

				dlclose(handle);
			}
		}
#endif
	}
	else
	{
		finalAddress = g_MemUtils.DecodeAndFindPattern(addressInBase, entry);
	}

	return finalAddress != NULL ? finalAddress : NULL;
}

int UTIL_PrivateToIndex(const void* pdata)
{
	if (pdata)
	{
		char* ptr = (char*)pdata;
		ptr += 4;

		entvars_t* pev = *(entvars_t**)ptr;

		if (pev && pev->pContainingEntity)
		{
			return ENTINDEX(pev->pContainingEntity);
		}
	}

	return 0;
}

int UTIL_ReadFlags(const char* c, int& numFlags)
{
	int flags = 0;

	while (*c)
	{
		if (*c >= 'a' && *c <= 'z')
		{
			flags |= (1 << (*c - 'a'));
			numFlags++;
		}

		*c++;
	}

	return numFlags ? flags : 0;
}

bool UTIL_IsAdmin(int index)
{
	int numFlags = 0;
	return !!(UTIL_ReadFlags(CvarFreeLookFlags->string, numFlags) & MF_GetPlayerFlags(index));
}

int UTIL_GetFlagPosition(int flag)
{
	int c;

	for (c = 0; flag; flag >>= 1)
	{
		c++;
	}

	return c;
}

int UTIL_GetUserMode(int& numFlags)
{
	return UTIL_ReadFlags(CvarFreeLookMode->string, numFlags);
}

int UTIL_GetNextUserMode(int currentMode, int allowedModes)
{
	for (int i = OBS_NONE; i < OBS_MAP_CHASE; i++)
	{
		currentMode = (currentMode % OBS_MAP_CHASE) + 1;

		if ((!allowedModes || 1 << (currentMode - 1) & allowedModes) && INDEXENT2(CurrentPlayerIndex)->v.iuser1 != currentMode)
		{
			break;
		}
	}

	return currentMode;
}