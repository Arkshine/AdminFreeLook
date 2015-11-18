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

extern cvar_t *CvarEnabled;
extern cvar_t *CvarAdminAccessFlags;
extern cvar_t *CvarUserOverrideMode;

extern cvar_t *CvarFadeToblack;
extern cvar_t *CvarForceCamera;
extern cvar_t *CvarForceChaseCam;

extern enginefuncs_t* g_pengfuncsTable;
extern ke::Vector<ke::AString> ErrorLogs;
extern HLTypeConversion TypeConversion;

#endif // _ADMINFREELOOK_H_