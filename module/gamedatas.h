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

#ifndef GAMEDATAS_H
#define GAMEDATAS_H


#if defined(WIN32)

	#define FUNC_SETMODE            "\\x83\\x2A\\x2A\\x55\\x8B\\x2A\\x2A\\x2A\\x56\\x8B\\x2A\\x8B"
	#define FUNC_ISVALIDTARGET      "\\x8B\\x2A\\x2A\\x2A\\x56\\x8B\\x2A\\x8B\\x2A\\x2A\\x2A\\x2A\\x2A\\x3B"
	#define FUNC_IDENT_HIDDEN_STATE false

#elif defined(__linux__)

	#define FUNC_SETMODE            "\\x55\\x57\\x56\\x89\\x2A\\x53\\x89\\x2A\\x83\\x2A\\x2A\\xD9\\x2A\\xD9"
	#define FUNC_SETMODE2           "_ZN11CBasePlayer16Observer_SetModeEi"						 
	#define FUNC_ISVALIDTARGET      "_ZN11CBasePlayer22Observer_IsValidTargetEib"
	#define FUNC_IDENT_HIDDEN_STATE false

#elif defined(__APPLE__)

	#define FUNC_SETMODE            "_ZN11CBasePlayer16Observer_SetModeEi"
	#define FUNC_ISVALIDTARGET      "_ZN11CBasePlayer22Observer_IsValidTargetEib"
	#define FUNC_IDENT_HIDDEN_STATE true

#endif

#define OBS_NONE           0
#define OBS_CHASE_LOCKED   1 // 1 << 0, 1 , a
#define OBS_CHASE_FREE     2 // 1 << 1, 2 , b
#define OBS_ROAMING        3 // 1 << 2, 4 , c    
#define OBS_IN_EYE         4 // 1 << 3, 8 , d
#define OBS_MAP_FREE       5 // 1 << 4, 16, e
#define OBS_MAP_CHASE      6 // 1 << 5, 32, f

#define FORCECAMERA_SPECTATE_ANYONE      0
#define FORCECAMERA_SPECTATE_ONLY_TEAM   1
#define FORCECAMERA_ONLY_FRIST_PERSON    2

#endif // GAMEDATAS_H
