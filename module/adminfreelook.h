//
// AdminFreeLook AMX Mod X Module
// Copyright (C) Vincent Herbet (Arkshine)
//
// This software is licensed under the GNU General Public License, version 2.
// For full license details, see LICENSE file.
//

#ifndef _ADMINFREELOOK_H_
#define _ADMINFREELOOK_H_

#include <amxxmodule.h>
#include <amtl/am-vector.h>
#include <amtl/am-string.h>
#include <HLTypeConversion.h>

void InitDetours();
void DestroyDetours();
void LogOnError();

extern cvar_t afl_enabled;
extern cvar_t afl_admin_access_flags;
extern cvar_t afl_user_override_mode;

extern enginefuncs_t* g_pengfuncsTable;
extern int CurrentPlayerIndex;
extern ke::Vector<ke::AString> ErrorLogs;
extern HLTypeConversion TypeConversion;

#endif // _ADMINFREELOOK_H_