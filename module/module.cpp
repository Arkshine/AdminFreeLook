//
// AdminFreeLook AMX Mod X Module
// Copyright (C) Vincent Herbet (Arkshine)
//
// This software is licensed under the GNU General Public License, version 2.
// For full license details, see LICENSE file.
//

#include "adminfreelook.h"

cvar_t afl_enabled            = { "afl_enabled"           , "1", 1, 1.0 };
cvar_t afl_admin_access_flags = { "afl_admin_access_flags", "d" };
cvar_t afl_user_override_mode = { "afl_user_override_mode", "0" };

cvar_t *CvarEnabled;
cvar_t *CvarAdminAccessFlags;
cvar_t *CvarUserOverrideMode;

cvar_t *CvarFadeToblack;
cvar_t *CvarForceCamera;
cvar_t *CvarForceChaseCam;

void OnAmxxAttach()
{
	CVAR_REGISTER(&afl_enabled);
	CVAR_REGISTER(&afl_admin_access_flags);
	CVAR_REGISTER(&afl_user_override_mode);

	CvarEnabled          = CVAR_GET_POINTER(afl_enabled.name);
	CvarAdminAccessFlags = CVAR_GET_POINTER(afl_admin_access_flags.name);
	CvarUserOverrideMode = CVAR_GET_POINTER(afl_user_override_mode.name);

	InitDetours();

	MF_PrintSrvConsole("\n   %s v%s Copyright (c) 2015 %s.\n", MODULE_NAME, MODULE_VERSION, MODULE_AUTHOR);
	MF_PrintSrvConsole("   Status: %s.\n\n", ErrorLogs.empty() ? "ready" : "disabled, check your log");
}

void OnPluginsLoaded()
{
	CvarFadeToblack   = CVAR_GET_POINTER("mp_fadetoblack");
	CvarForceCamera   = CVAR_GET_POINTER("mp_forcecamera");
	CvarForceChaseCam = CVAR_GET_POINTER("mp_forcechasecam");

	TypeConversion.init();

	if (!ErrorLogs.empty())
	{
		for (size_t line = 0; line < ErrorLogs.length(); ++line)
		{
			MF_Log(ErrorLogs[line].chars());
		}

		ErrorLogs.clear();
	}
}

void OnAmxxDetach()
{
	DestroyDetours();
}
