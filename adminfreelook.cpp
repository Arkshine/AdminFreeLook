#include "amxxmodule.h"
#include "chooker.h"

CHooker		HookerClass;
CHooker*	Hooker = &HookerClass;

cvar_t amx_adminfreelook		= { "amx_adminfreelook"		, "1", 1, 1.0 };
cvar_t amx_adminfreelookflag	= { "amx_adminfreelookflag"	, "d" };
cvar_t amx_adminfreelookmode	= { "amx_adminfreelookmode"	, "0" };

#ifdef _WIN32

	#define DUMMY_VAL 0
	typedef int DUMMY;

	typedef int	( __fastcall *FuncIsValidTarget )	( void*, DUMMY, int, bool );
	typedef int	( __fastcall *FuncSetMode )			( void*, DUMMY, int );

	int		__fastcall Observer_IsValidTarget_Hook	( void* pvPlayer, DUMMY, int index, bool checkteam );
	void	__fastcall Observer_SetMode_Hook		( void* pvPlayer, DUMMY, int mode );

#else

	typedef int		( *FuncIsValidTarget )	( void*, int, bool );
	typedef void	( *FuncSetMode )		( void*, int );

	int		Observer_IsValidTarget_Hook	( void* pvPlayer, int index, bool checkteam );
	void	Observer_SetMode_Hook		( void* pvPlayer, int mode );
	void	Observer_SetMode_Hook2		( void* pvPlayer, int mode );

#endif

bool	isAdmin				( int index );
int		readFlags			( const char* c, int& numFlags );
int		privateToIndex		( const void* pdata );
void	printModuleStatus	( void );
float	CVarGetFloat		( const char* cvarName );
int		getUserMode			( int& numFlags );
int		getNextUserMode		( int currentMode, int allowedModes );
int		getFlagPosition		( int flag );

inline edict_t* INDEXENT2( int iEdictNum )
{ 
	if( iEdictNum >= 1 && iEdictNum <= gpGlobals->maxClients )
		return MF_GetPlayerEdict(iEdictNum);
	else
		return ( *g_engfuncs.pfnPEntityOfEntIndex )( iEdictNum ); 
}

CFunc*				FuncIsValidTargetHook	= NULL;
CFunc*				FuncSetModeHook			= NULL;
CFunc*				FuncSetModeHook2		= NULL;

FuncIsValidTarget	FuncIsvalidTargetOrig	= NULL;
FuncSetMode			FuncSetModetOrig		= NULL;

#if defined( __linux__ )
	FuncSetMode		FuncSetModetOrig2		= NULL;
#endif

cvar_t* CvarFreeLookEnable;
cvar_t* CvarFreeLookFlags;
cvar_t* CvarFreeLookMode;

extern enginefuncs_t* g_pengfuncsTable;

int CurrentPlayerIndex = 0;

#define OBS_NONE				0
#define OBS_CHASE_LOCKED		1 // 1 << 0, 1 , a
#define OBS_CHASE_FREE			2 // 1 << 1, 2 , b
#define OBS_ROAMING				3 // 1 << 2, 4 , c	
#define OBS_IN_EYE				4 // 1 << 3, 8 , d
#define OBS_MAP_FREE			5 // 1 << 4, 16, e
#define OBS_MAP_CHASE			6 // 1 << 5, 32, f

void OnMetaAttach()
{
	CVAR_REGISTER( &amx_adminfreelook );
	CVAR_REGISTER( &amx_adminfreelookflag );
	CVAR_REGISTER( &amx_adminfreelookmode );

	CvarFreeLookEnable	= CVAR_GET_POINTER( amx_adminfreelook.name );
	CvarFreeLookFlags	= CVAR_GET_POINTER( amx_adminfreelookflag.name );
	CvarFreeLookMode	= CVAR_GET_POINTER( amx_adminfreelookmode.name );

	void* libraryContained = ( void* )MDLL_Spawn;

	#if defined( WIN32 )
		FuncIsvalidTargetOrig	= Hooker->MemorySearch< FuncIsValidTarget>	( "0x8B,*,*,*,0x56,0x8B,*,0x8B,*,*,*,*,*,0x3B"	, libraryContained, FALSE );
		FuncSetModetOrig		= Hooker->MemorySearch< FuncSetMode>		( "0x83,*,*,0x55,0x8B,*,*,*,0x56,0x8B,*,0x8B"	, libraryContained, FALSE );
	#else
		FuncIsvalidTargetOrig	= Hooker->MemorySearch< FuncIsValidTarget>	( "_ZN11CBasePlayer22Observer_IsValidTargetEib"	, libraryContained, TRUE );
		FuncSetModetOrig		= Hooker->MemorySearch< FuncSetMode>		( "0x55,0x57,0x56,0x89,*,0x53,0x89,*,0x83,*,*,0xD9,*,0xD9", libraryContained, FALSE );
		FuncSetModetOrig2		= Hooker->MemorySearch< FuncSetMode>		( "_ZN11CBasePlayer16Observer_SetModeEi"		, libraryContained, TRUE );
	#endif

	FuncIsValidTargetHook	= Hooker->CreateHook( ( void* )FuncIsvalidTargetOrig, ( void* )Observer_IsValidTarget_Hook	, TRUE );
	FuncSetModeHook			= Hooker->CreateHook( ( void* )FuncSetModetOrig		, ( void* )Observer_SetMode_Hook		, TRUE );

	printModuleStatus();
}

float CVarGetFloat( const char* cvarName )
{
	float value = g_engfuncs.pfnCVarGetFloat( cvarName );
	int numFlags = 0;

	if( CvarFreeLookEnable->value && value > 0 )
	{
		if( ( value == 2 && isAdmin( CurrentPlayerIndex ) ) || getUserMode( numFlags ) ) 
		{
			value = CurrentPlayerIndex = 0;
		}
	}

	RETURN_META_VALUE( MRES_SUPERCEDE, value );
}

#if defined( WIN32 )
	void __fastcall Observer_SetMode_Hook( void* pvPlayer, DUMMY, int mode )
#else
	void Observer_SetMode_Hook( void* pvPlayer, int mode )
#endif
	{
		#if defined( __linux__ )

		asm volatile
		( 
			"movl %%edx, %0;" 
			"movl %%eax, %1;"
			: "=d" (mode), "=a" (pvPlayer) : :
		);

		#endif

		CurrentPlayerIndex = privateToIndex( pvPlayer );

		g_pengfuncsTable->pfnCVarGetFloat = CVarGetFloat;

		if( FuncSetModeHook->Restore() )
		{
			if( !isAdmin( CurrentPlayerIndex ) )
			{
				int numFlags = 0;
				int userMode = getUserMode( numFlags );

				if( numFlags )
				{
					if( numFlags == 1 )
					{
						mode = getFlagPosition( userMode );
					}
					else
					{
						mode = getNextUserMode( INDEXENT2( CurrentPlayerIndex )->v.iuser1, userMode );
					}
				}
			}

			#if defined( WIN32 )
				FuncSetModetOrig( pvPlayer, DUMMY_VAL, mode );
			#else
				FuncSetModetOrig2( pvPlayer, mode );
			#endif

			FuncSetModeHook->Patch();
		}

		g_pengfuncsTable->pfnCVarGetFloat = NULL;
	}

#ifdef _WIN32
	int __fastcall Observer_IsValidTarget_Hook( void* pvPlayer, DUMMY, int index, bool checkteam )
#else
	int Observer_IsValidTarget_Hook( void* pvPlayer, int index, bool checkteam )
#endif
	{
		bool	override = false;
		int		result = 0;

		if( checkteam && CvarFreeLookEnable->value && isAdmin( privateToIndex( pvPlayer ) ) )
		{
			override = true;
		}

		if( FuncIsValidTargetHook->Restore() )
		{
			#ifdef WIN32
				result = FuncIsvalidTargetOrig( pvPlayer, DUMMY_VAL, index, override ? false : checkteam );
			#else
				result = FuncIsvalidTargetOrig( pvPlayer, index, override ? false : checkteam );
			#endif

			FuncIsValidTargetHook->Patch();
		}

		return result;
	}

bool isAdmin( int index )
{
	int numFlags = 0;
	return !!( readFlags( CvarFreeLookFlags->string, numFlags ) & MF_GetPlayerFlags( index ) );
}

int privateToIndex( const void* pdata )
{
	if( pdata )
	{
		char* ptr = ( char* )pdata;
		ptr += 4;

		entvars_t* pev = *( entvars_t** )ptr;
		
		if( pev && pev->pContainingEntity )
		{
			return ENTINDEX( pev->pContainingEntity );
		}
	}

	return NULL;
}

int readFlags( const char* c, int& numFlags )
{
	int flags = 0;

	while( *c )
	{  
		if( *c >= 'a' && *c <= 'z' )
		{
			flags |= ( 1 <<( *c - 'a' ) );
			numFlags++;
		}

		*c++;
	}

	return numFlags ? flags : 0;
}

int getFlagPosition( int flag )
{
	int c;

	for( c = 0; flag; flag >>= 1 )
	{
		c++;
	}

	return c;
}

int getUserMode( int& numFlags )
{
	return readFlags( CvarFreeLookMode->string, numFlags );
}

int getNextUserMode( int currentMode, int allowedModes )
{
	for( int i = OBS_NONE; i < OBS_MAP_CHASE; i++ )
	{
		currentMode = ( currentMode % OBS_MAP_CHASE ) + 1;

		if( 1 << ( currentMode - 1 ) & allowedModes && INDEXENT2( CurrentPlayerIndex )->v.iuser1 != currentMode )
		{
			break;
		}
	}

	return currentMode;
}

void printModuleStatus( void )
{
	printf( "\n %s v%s - by %s.\n -\n", MODULE_NAME, MODULE_VERSION, MODULE_AUTHOR );

	if( !FuncIsvalidTargetOrig )
		printf( " CBasePlayer::Observer_IsValidTarget - Signature/symbol could not be found.\n\n" );
	else if( !FuncSetModetOrig )
		printf( " CBasePlayer::Observer_SetMode - Signature/symbol could not be found.\n\n" );
#if defined( __linux__ )
	else if( !FuncSetModetOrig2 )
		printf( " CBasePlayer::Observer_SetMode (second part) - Signature/symbol could not be found.\n" );
#endif
	else if( !FuncIsValidTargetHook )
		printf( " CBasePlayer::Observer_IsValidTarget - Hook creation failed.\n\n" );
	else if( !FuncSetModeHook )
		printf( " BasePlayer::Observer_SetMode - Hook creation failed.\n\n" );
	else 
		printf( " Loaded with success.\n\n" );
}


