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

CDetour *ObserverSetModeDetour = NULL;
CDetour *IsValidTargetDetour = NULL;

int CurrentPlayerIndex = 0;
bool ForceNextMode = false;

ke::Vector<ke::AString> ErrorLog;

#if defined(__linux__)
	void *FuncSetMode2 = NULL;
#endif


float CVarGetFloat(const char* cvarName)
{
	META_RES result = MRES_IGNORED;
	int numFlags;

	if (CvarFreeLookEnable->value <= 0 || !CurrentPlayerIndex)
	{
		result = MRES_IGNORED;
	}
	else if (g_engfuncs.pfnCVarGetFloat(cvarName) > 0 && (UTIL_GetUserMode(numFlags) || UTIL_IsAdmin(CurrentPlayerIndex)))
	{
		result = MRES_SUPERCEDE;

		if (!strcmp(cvarName, "mp_forcecamera")) // just for safety
		{
			CurrentPlayerIndex = 0;
		}
	}	

	RETURN_META_VALUE(result, 0);
}


/**
 * void CBasePlayer::Observer_SetMode(int mode)
 * 
 * @param mode        Observer mode.
 * @noreturn
 */
DETOUR_DECL_MEMBER1(Observer_SetMode, void, int, mode) 
{
	const void *pvPlayer = (const void *)this;

	#if defined(__linux__)
		asm volatile
		(
			"movl %%edx, %0;"
			"movl %%eax, %1;"
			: "=d" (mode), "=a" (pvPlayer) : :
		);
	#endif

	CurrentPlayerIndex = UTIL_PrivateToIndex(pvPlayer);

	g_pengfuncsTable->pfnCVarGetFloat = CVarGetFloat;

	if (!UTIL_IsAdmin(CurrentPlayerIndex))
	{
		int numFlags = 0;
		int userMode = UTIL_GetUserMode(numFlags);

		if (numFlags)
		{
			if (numFlags == 1)
			{
				mode = UTIL_GetFlagPosition(userMode);
			}
			else
			{
				mode = UTIL_GetNextUserMode(INDEXENT2(CurrentPlayerIndex)->v.iuser1, userMode);
			}
		}
	}

#if defined(WIN32) || defined(__APPLE__)

	DETOUR_MEMBER_CALL(Observer_SetMode)(mode);

#elif defined(__linux__)

	ObserverSetModeDetour->DisableDetour();
	((void(*)(void*, int))FuncSetMode2)((void *)pvPlayer, mode);
	ObserverSetModeDetour->EnableDetour();

#endif

	g_pengfuncsTable->pfnCVarGetFloat = NULL;
}

/**
 * BOOL CBasePlayer::Observer_IsValidTarget(int index, bool checkteam)
 *
 * @param index            Player's index to check validity.
 * @param checkteam        Whether player's team should be checked.
 *
 * @return                 True on success, otherwise false.
 */
DETOUR_DECL_MEMBER2(Observer_IsValidTarget, void*, int, index, bool, checkteam)
{
	if (checkteam && CvarFreeLookEnable->value && UTIL_IsAdmin(UTIL_PrivateToIndex((const void *)this)))
	{
		checkteam = false;
	}

	return DETOUR_MEMBER_CALL(Observer_IsValidTarget)(index, checkteam);
}



void EnableDetours()
{
	void *funcIsvalidTarget = UTIL_FindAddressFromEntry(FUNC_ISVALIDTARGET, FUNC_IDENT_HIDDEN_STATE);
	void *funcSetMode       = UTIL_FindAddressFromEntry(FUNC_SETMODE, FUNC_IDENT_HIDDEN_STATE);

#if defined(__linux__)
	FuncSetMode2 = UTIL_FindAddressFromEntry(FUNC_SETMODE2, FUNC_IDENT_HIDDEN_STATE);
#endif

	IsValidTargetDetour   = DETOUR_CREATE_MEMBER_FIXED(Observer_IsValidTarget, funcIsvalidTarget);
	ObserverSetModeDetour = DETOUR_CREATE_MEMBER_FIXED(Observer_SetMode, funcSetMode);
	
#if defined(__linux__) 
	if (ObserverSetModeDetour != NULL && IsValidTargetDetour != NULL && FuncSetMode2 != NULL)
#elif defined(WIN32) || defined(__APPLE__)
	if (ObserverSetModeDetour != NULL && IsValidTargetDetour != NULL)
#endif
	{
		IsValidTargetDetour->EnableDetour();
		ObserverSetModeDetour->EnableDetour();
	}
	else
	{
		if (!funcIsvalidTarget)
		{
			ErrorLog.append(ke::AString("CBasePlayer::Observer_IsValidTarget cound not be found."));
		}

		if (!funcSetMode)
		{
			ErrorLog.append(ke::AString("CBasePlayer::Observer_SetMode cound not be found."));
		}

	#if defined(__linux__)
		if (!FuncSetMode2)
		{
			ErrorLog.append(ke::AString("CBasePlayer::Observer_SetMode (second part) cound not be found"));
		}
	#endif

		if (ObserverSetModeDetour == NULL || IsValidTargetDetour == NULL)
		{
			ErrorLog.append(ke::AString("Observer forwards could not be initialized -Disabled module."));
		}
	}
}

void DisableDetours()
{
	if (IsValidTargetDetour)
	{
		IsValidTargetDetour->Destroy();
	}

	if (ObserverSetModeDetour)
	{
		ObserverSetModeDetour->Destroy();
	}
}

void LogOnError()
{
	if (!ErrorLog.empty())
	{
		for (size_t i = 0; i < ErrorLog.length(); ++i)
		{
			MF_Log(ErrorLog[i].chars());
		}

		ErrorLog.clear(); // Outputs error one time in log.
	}

}

