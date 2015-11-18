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

void OnAmxxAttach()
{
	CVAR_REGISTER(&afl_enabled);
	CVAR_REGISTER(&afl_admin_access_flags);
	CVAR_REGISTER(&afl_user_override_mode);

	InitDetours();

	MF_PrintSrvConsole("\n   %s v%s Copyright (c) 2015 %s.\n", MODULE_NAME, MODULE_VERSION, MODULE_AUTHOR);
	MF_PrintSrvConsole("   Status: %s.\n\n", ErrorLogs.empty() ? "ready" : "disabled, check your log");
}

void OnPluginsLoaded()
{
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
