//
// AdminFreeLook AMX Mod X Module
// Copyright (C) Vincent Herbet (Arkshine)
//
// This software is licensed under the GNU General Public License, version 2.
// For full license details, see LICENSE file.
//

#include "adminfreelook.h"
#include "gamedatas.h"
#include "utils.h"

#include <CDetour/detours.h>

CDetour *ObserverSetModeDetour;
CDetour *IsValidTargetDetour;

HLTypeConversion TypeConversion;

int CurrentPlayerIndex;
ke::Vector<ke::AString> ErrorLogs;

#if defined(KE_LINUX)

	typedef void(*SetModePart2Fn)(void*, int);
	SetModePart2Fn SetModePart2;

#endif

float CVarGetFloat(const char* cvarName)
{
	auto currentValue = g_engfuncs.pfnCVarGetFloat(cvarName);

	if (afl_enabled.value <= 0.0f || !CurrentPlayerIndex)
	{
		RETURN_META_VALUE(MRES_IGNORED, currentValue);
	}

	int numFlags;

	if (currentValue > 0.0f && (Util::GetUserMode(numFlags) || Util::IsAdmin(CurrentPlayerIndex)))
	{
		currentValue = 0.0f;

		if (strcmp(cvarName, "mp_forcecamera") == 0 || strcmp(cvarName, "mp_forcechasecam") == 0) // just for safety
		{
			CurrentPlayerIndex = 0;
		}
	}

	RETURN_META_VALUE(MRES_SUPERCEDE, currentValue);
}


/**
 * void CBasePlayer::Observer_SetMode(int mode)
 *
 * @param mode        Observer mode.
 * @noreturn
 */
DETOUR_DECL_MEMBER1(Observer_SetMode, void, int, mode)
{
	auto pvPlayer = reinterpret_cast<void*>(this);

	#if defined(KE_LINUX)

		asm volatile
		(
			"movl %%edx, %0;"
			"movl %%eax, %1;"
			: "=d" (mode), "=a" (pvPlayer) : :
		);

	#endif

	CurrentPlayerIndex = TypeConversion.cbase_to_id(pvPlayer);

	g_pengfuncsTable->pfnCVarGetFloat = CVarGetFloat;

	if (!Util::IsAdmin(CurrentPlayerIndex))
	{
		auto numFlags = 0;
		auto userMode = Util::GetUserMode(numFlags);

		if (numFlags)
		{
			if (numFlags == 1)
			{
				mode = Util::GetFlagPosition(userMode);
			}
			else
			{
				mode = Util::GetNextUserMode(TypeConversion.id_to_edict(CurrentPlayerIndex)->v.iuser1, userMode);
			}
		}
	}

#if defined(KE_WINDOWS) || defined(KE_MACOSX)

	DETOUR_MEMBER_CALL(Observer_SetMode)(mode);

#elif defined(KE_LINUX)

	ObserverSetModeDetour->DisableDetour();
	SetModePart2(reinterpret_cast<void*>(pvPlayer), mode);
	ObserverSetModeDetour->EnableDetour();

#endif

	g_pengfuncsTable->pfnCVarGetFloat = nullptr;
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
	if (checkteam && afl_enabled.value > 0.0f && Util::IsAdmin(TypeConversion.cbase_to_id(reinterpret_cast<void*>(this))))
	{
		checkteam = false;
	}

	return DETOUR_MEMBER_CALL(Observer_IsValidTarget)(index, checkteam);
}



void InitDetours()
{
	IsValidTargetDetour   = DETOUR_CREATE_MEMBER_FIXED(Observer_IsValidTarget, Util::FindAddress(FUNC_ISVALIDTARGET));
	ObserverSetModeDetour = DETOUR_CREATE_MEMBER_FIXED(Observer_SetMode      , Util::FindAddress(FUNC_SETMODE));

#if defined(KE_LINUX)

	void SetModePart2Address = Util::FindAddress(FUNC_SETMODE2);

	if (ObserverSetModeDetour && IsValidTargetDetour && SetModePart2Address)
	{
		SetModePart2 = reinterpret_cast<SetModePart2Fn>(SetModePart2Address);

#elif defined(KE_WINDOWS) || defined(KE_MACOSX)

	if (ObserverSetModeDetour && IsValidTargetDetour)
	{
#endif
		IsValidTargetDetour->EnableDetour();
		ObserverSetModeDetour->EnableDetour();
	}
	else
	{
		if (!IsValidTargetDetour)
		{
			ErrorLogs.append("CBasePlayer::Observer_IsValidTarget is not available.");
		}

		if (!ObserverSetModeDetour)
		{
			ErrorLogs.append("CBasePlayer::Observer_SetMode is not available.");
		}

	#if defined(KE_LINUX)

		if (!SetModePart2Address)
		{
			ErrorLogs.append("CBasePlayer::Observer_SetMode (second part) is not available.");
		}
	#endif

		ErrorLogs.append("Some functions are not availble, module has been disabled.");

		DestroyDetours();
	}
}

void DestroyDetours()
{
	if (IsValidTargetDetour)
	{
		IsValidTargetDetour->Destroy();
		IsValidTargetDetour = nullptr;
	}

	if (ObserverSetModeDetour)
	{
		ObserverSetModeDetour->Destroy();
		ObserverSetModeDetour = nullptr;
	}
}

