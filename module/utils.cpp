//
// AdminFreeLook AMX Mod X Module
// Copyright (C) Vincent Herbet (Arkshine)
//
// This software is licensed under the GNU General Public License, version 2.
// For full license details, see LICENSE file.
//

#include "adminfreelook.h"
#include "gamedatas.h"
#include <MemoryUtils.h>

namespace Util
{
	void *FindAddress(const char *entry, const char *library)
	{
		void *addressInBase = nullptr;
		void *finalAddress = nullptr;

		if (strcmp(library, "mod") == 0)
		{
			addressInBase = reinterpret_cast<void*>(MDLL_Spawn);
		}
		else if (strcmp(library, "engine") == 0)
		{
			addressInBase = reinterpret_cast<void*>(gpGlobals);
		}

		if (*entry != '\\')
		{
		#if defined(KE_WINDOWS)

			MEMORY_BASIC_INFORMATION mem;

			if (VirtualQuery(addressInBase, &mem, sizeof(mem)))
			{
				finalAddress = g_MemUtils.ResolveSymbol(mem.AllocationBase, entry);
			}

		#elif defined(KE_LINUX) || defined(KE_MACOSX)

			Dl_info info;
			void *handle = nullptr;

			if (dladdr(addressInBase, &info) != 0)
			{
				void *handle = dlopen(info.dli_fname, RTLD_NOW);
				if (handle)
				{
					finalAddress = g_MemUtils.ResolveSymbol(handle, entry);
					dlclose(handle);
				}
			}
		#endif
		}
		else
		{
			finalAddress = g_MemUtils.DecodeAndFindPattern(addressInBase, entry);
		}

		return finalAddress;
	}

	int ReadFlags(const char* c, int* numFlags)
	{
		auto flags = 0;

		while (*c)
		{
			if (*c >= 'a' && *c <= 'z')
			{
				flags |= (1 << (*c - 'a'));

				if (numFlags)
				{
					++*numFlags;
				}
			}

			*c++;
		}

		return flags;
	}

	bool IsAdmin(edict_t *pPlayer)
	{
		return !!(ReadFlags(CvarAdminAccessFlags->string, nullptr) & MF_GetPlayerFlags(TypeConversion.edict_to_id(pPlayer)));
	}

	bool ShouldRunCode()
	{
		return CvarEnabled->value > 0.0f && CvarFadeToblack->value <= 0.0f && CvarForceCamera->value > 0.0f && CvarForceChaseCam->value > 0.0f;
	}

	int GetFlagPosition(int flag)
	{
		int c;

		for (c = 0; flag; flag >>= 1)
		{
			c++;
		}

		return c;
	}

	int GetUserMode(int *numFlags)
	{
		return ReadFlags(CvarUserOverrideMode->string, numFlags);
	}

	int GetNextUserMode(edict_t *pPlayer, int userFlags)
	{
		auto currentMode = pPlayer->v.iuser1;

		for (auto i = OBS_NONE; i < OBS_MAP_CHASE; ++i)
		{
			currentMode = (currentMode % OBS_MAP_CHASE) + 1;

			if ((!userFlags || 1 << (currentMode - 1) & userFlags) && pPlayer->v.iuser1 != currentMode)
			{
				break;
			}
		}

		return currentMode;
	}
};

