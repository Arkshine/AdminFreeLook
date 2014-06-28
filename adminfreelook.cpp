
#include "adminfreelook.h"

CDetour *ObserverSetModeDetour = NULL;
CDetour *IsValidTargetDetour = NULL;

int CurrentPlayerIndex = 0;
bool ForceNextMode = false;

ke::Vector<ke::AString> ErrorLog;

#if defined(__linux__) || defined(__APPLE__)
	void *FuncSetMode2 = NULL;
#endif


float CVarGetFloat(const char* cvarName)
{
	float value = g_engfuncs.pfnCVarGetFloat(cvarName);
	int numFlags = 0;

	if (CvarFreeLookEnable->value && value > 0)
	{
		if ((value == FORCECAMERA_ONLY_FRIST_PERSON || UTIL_GetUserMode(numFlags)) && UTIL_IsAdmin(CurrentPlayerIndex))
		{
			value = CurrentPlayerIndex = 0;
		}
	}

	RETURN_META_VALUE(MRES_SUPERCEDE, value);
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

	#if defined(__linux__) || defined(__APPLE__)
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

#if defined(WIN32)

	DETOUR_MEMBER_CALL(Observer_SetMode)(mode);

#elif defined(__linux__) || defined(__APPLE__)

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
DETOUR_DECL_MEMBER2(Observer_IsValidTarget, BOOL, int, index, bool, checkteam)
{
	bool override = false;
	int  result = 0;

	if (checkteam && CvarFreeLookEnable->value && UTIL_IsAdmin(UTIL_PrivateToIndex((const void *)this)))
	{
		override = true;
	}

	return DETOUR_MEMBER_CALL(Observer_IsValidTarget)(index, checkteam);
}



void EnableDetours()
{
	void *funcIsvalidTarget = UTIL_FindAddressFromEntry(FUNC_ISVALIDTARGET, FUNC_IDENT_HIDDEN_STATE);
	void *funcSetMode       = UTIL_FindAddressFromEntry(FUNC_SETMODE, FUNC_IDENT_HIDDEN_STATE);

	#if defined(__linux__) || defined(__APPLE__)
		FuncSetMode2 = UTIL_FindAddressFromEntry(FUNC_SETMODE2, FUNC_IDENT_HIDDEN_STATE);
	#endif

	IsValidTargetDetour   = DETOUR_CREATE_MEMBER_FIXED(Observer_IsValidTarget, funcIsvalidTarget);
	ObserverSetModeDetour = DETOUR_CREATE_MEMBER_FIXED(Observer_SetMode, funcSetMode);
	
#if defined(__linux__) || defined(__APPLE__)
	if (ObserverSetModeDetour != NULL && IsValidTargetDetour != NULL && FuncSetMode2 != NULL)
#elif defined(WIN32)
	if (ObserverSetModeDetour != NULL && IsValidTargetDetour != NULL)
#endif
	{
		IsValidTargetDetour->EnableDetour();
		ObserverSetModeDetour->EnableDetour();
	}
	else
	{
		if (!funcIsvalidTarget)
			ErrorLog.append(ke::AString("CBasePlayer::Observer_IsValidTarget cound not be found."));

		if (!funcSetMode)
			ErrorLog.append(ke::AString("CBasePlayer::Observer_SetMode cound not be found."));

	#if defined(__linux__) || defined(__APPLE__)
		if (!FuncSetMode2)
			ErrorLog.append(ke::AString("CBasePlayer::Observer_SetMode (second part) cound not be found"));
	#endif

		if (ObserverSetModeDetour == NULL || IsValidTargetDetour == NULL)
			ErrorLog.append(ke::AString("Observer forwards could not be initialized -Disabled module."));
	}
}

void DisableDetours()
{
	if (IsValidTargetDetour)
		IsValidTargetDetour->Destroy();

	if (ObserverSetModeDetour)
		ObserverSetModeDetour->Destroy();
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

