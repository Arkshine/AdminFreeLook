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

cvar_t amx_adminfreelook     = { "amx_adminfreelook", "1", 1, 1.0 };
cvar_t amx_adminfreelookflag = { "amx_adminfreelookflag", "d" };
cvar_t amx_adminfreelookmode = { "amx_adminfreelookmode", "0" };

cvar_t *CvarFreeLookEnable;
cvar_t *CvarFreeLookFlags;
cvar_t *CvarFreeLookMode;

void OnMetaAttach()
{
	CVAR_REGISTER(&amx_adminfreelook);
	CVAR_REGISTER(&amx_adminfreelookflag);
	CVAR_REGISTER(&amx_adminfreelookmode);

	CvarFreeLookEnable = CVAR_GET_POINTER(amx_adminfreelook.name);
	CvarFreeLookFlags  = CVAR_GET_POINTER(amx_adminfreelookflag.name);
	CvarFreeLookMode   = CVAR_GET_POINTER(amx_adminfreelookmode.name);;
}

void OnAmxxAttach()
{
	EnableDetours();
}

void OnPluginsLoaded()
{
	LogOnError();
}

void OnAmxxDetach()
{
	DisableDetours();
}
