AdminFreeLook
=============
[![Build Status](https://travis-ci.org/Arkshine/AdminFreeLook.svg?branch=master)](https://travis-ci.org/Arkshine/AdminFreeLook) [![Build status](https://ci.appveyor.com/api/projects/status/gyd7556y6dqr80e3/branch/master?svg=true)](https://ci.appveyor.com/project/Arkshine/adminfreelook/branch/master) [![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/Arkshine/adminfreelook/trend.png)](https://bitdeli.com/free "Bitdeli Badge")


AMX Mod X module whichs allows spectating admins to use any camera modes whatever `mp_forcecamera` or `mp_forcechasecam` value.
This only works for latest HLDS version.

Originally created by [jim_yang](https://forums.alliedmods.net/showthread.php?t=100067).
Rewritten to be used after Steam February update. 


##Configuration (cvars)


* A reminder about `mp_forcecamera` and `mp_forcechasecam`.  
  This is the base configuration for all players, which can be overwritten by below cvars.  
    
   `0` : Observers can spectate anyone, any mode.  
   `1` : Observers can only spectate his own team.  
   `2` : Observers can only spectate his own team and in-eye mode only.  

* __`amx_adminfreelook`__
 
  Module state. Whether you want to activate module functionnalities.  

    `0` : Disable  
    `1` : Enable  
  
  Default value : *"1"*

* __`amx_adminfreelookflag`__  

  Admin's access flags.  
  Ability to observe without being restricted by gameplay cvar.  
  Flags are additives.
  
  Default flags are (from `users.ini` file) : 

  ```
  ; Access flags:
  ; a - immunity (can't be kicked/baned/slayed/slaped and affected by other commmands)
  ; b - reservation (can join on reserved slots)
  ; c - amx_kick command
  ; d - amx_ban and amx_unban commands (permanent and temporary bans)
  ; e - amx_slay and amx_slap commands
  ; f - amx_map command
  ; g - amx_cvar command (not all cvars will be available)
  ; h - amx_cfg command
  ; i - amx_chat and other chat commands
  ; j - amx_vote and other vote commands
  ; k - access to sv_password cvar (by amx_cvar command)
  ; l - access to amx_rcon command and rcon_password cvar (by amx_cvar command)
  ; m - custom level A (for additional plugins)
  ; n - custom level B
  ; o - custom level C
  ; p - custom level D
  ; q - custom level E
  ; r - custom level F
  ; s - custom level G
  ; t - custom level H
  ; u - menu access
  ; v - amx_ban and amx_unban commands (temporary bans only, about amx_unban, only self performed ban during map gonna be allowed)
  ; z - user (no admin)
  ```  
  
  Default value : *"d"*

* __`amx_adminfreelookmode`__

  This will force *non-admin* players to use specific modes only.  
  Flags are additives.
  
  `0` : None  
  `a` : Chase Locked  
  `b` : Chase Free  
  `c` : Roaming  
  `d` : In Eye  
  `e` : Map Free  
  `f` : Map Chase  
  
  Default value : *"0"*
  
  
##Installation

1. From [Releases section](https://github.com/Arkshine/AdminFreeLook/releases/latest), get the binary file:  <table>
  <tbody>
    <tr>
      <th align="center">OS</th>
      <th align="center">Windows</th>
      <th align="center">Linux</th>
    </tr>
    <tr>
      <td align="center">Binary file</td>
      <td align="center">`adminfreelook_amxx.dll`</td>
      <td align="center">`adminfreelook_amxx_i386.so`</td>
    </tr>
  </tbody>
</table>
2. Upload it in your *amxmodx/modules/* folder.
3. Open your *configs/modules.ini* and add at the end `adminfreelook`
4. Configure `mp_forcechasecam` and `mp_forcecamera` gameplay cvars. Both should have the same value.
5. Configure `amx_adminfreelookflag` and `amx_adminfreelookmode` modules cvars.
6. Restart. 
