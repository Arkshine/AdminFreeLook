
#include "amxxmodule.h"
#include "chooker.h"

CHooker		HookerClass;
CHooker*		Hooker = &HookerClass;

cvar_t amx_adminfreelook		= { "amx_adminfreelook"		, "1", 1, 1.0 };
cvar_t amx_adminfreelookflag	= { "amx_adminfreelookflag"	, "d" };

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

#endif

bool	isAllowedPlayer	( int index );
int		readFlags		( const char* c );
int		privateToIndex	( const void* pdata );

CFunc*				FuncIsValidTargetHook	= NULL;
CFunc*				FuncSetModeHook			= NULL;

FuncIsValidTarget	FuncIsvalidTargetOrig	= NULL;
FuncSetMode			FuncSetModetOrig		= NULL;

cvar_t* CvarFreeLookEnable;
cvar_t* CvarFreeLookFlags;

void OnMetaAttach()
{
	CVAR_REGISTER( &amx_adminfreelook );
	CVAR_REGISTER( &amx_adminfreelookflag );

	CvarFreeLookEnable	= CVAR_GET_POINTER( amx_adminfreelook.name );
	CvarFreeLookFlags	= CVAR_GET_POINTER( amx_adminfreelookflag.name );
	
	#ifdef _WIN32 
		FuncIsvalidTargetOrig	= Hooker->MemorySearch< FuncIsValidTarget>	( "0x8B,*,*,*,0x56,0x8B,*,0x8B,*,*,*,*,*,0x3B"	, ( void* )MDLL_Spawn, FALSE );
		FuncSetModetOrig		= Hooker->MemorySearch< FuncSetMode>		( "0x83,*,*,0x55,0x8B,*,*,*,0x56,0x8B,*,0x8B"	, ( void* )MDLL_Spawn, FALSE );
	#else
		FuncIsvaludTargetOrig	= Hooker->MemorySearch< FuncIsValidTarget>	( "_ZN11CBasePlayer22Observer_IsValidTargetEib"	, ( void* )MDLL_Spawn, TRUE );
		FuncSetModetOrig		= Hooker->MemorySearch< FuncSetMode>		( "_ZN11CBasePlayer16Observer_SetModeEi"		, ( void* )MDLL_Spawn, TRUE );
	#endif

	FuncIsValidTargetHook	= Hooker->CreateHook( ( void* )FuncIsvalidTargetOrig, ( void* )Observer_IsValidTarget_Hook	, TRUE );
	FuncSetModeHook			= Hooker->CreateHook( ( void* )FuncSetModetOrig		, ( void* )Observer_SetMode_Hook		, TRUE );

	printf( "\n %s v%s - by %s.\n -\n", MODULE_NAME, MODULE_VERSION, MODULE_AUTHOR );

	if( !FuncIsvalidTargetOrig )
		printf( " Signature/symbol could not be found.\n\n" );
	else if( !FuncIsValidTargetHook )
		printf( " Hook creation failed.\n\n" );
	else 
		printf( " Loaded with success.\n\n" );
}

#ifdef _WIN32
	void __fastcall Observer_SetMode_Hook( void* pvPlayer, DUMMY, int mode )
#else
	void Observer_SetMode_Hook( void* pvPlayer, int mode )
#endif
	{
		printf( "Observer_SetMode_Hook - mode  = %d\n", mode );

		if( FuncSetModeHook->Restore() )
		{
			#ifdef WIN32
				FuncSetModetOrig( pvPlayer, DUMMY_VAL, mode );
			#else
				FuncSetModetOrig( pvPlayer, mode );
			#endif

			FuncSetModeHook->Patch();
		}
	}

#ifdef _WIN32
	int __fastcall Observer_IsValidTarget_Hook( void* pvPlayer, DUMMY, int index, bool checkteam )
#else
	int Observer_IsValidTarget_Hook( void* pvPlayer, int index, bool checkteam )
#endif
	{
		bool	override = false;
		int		result = 0;

		if( checkteam && isAllowedPlayer( privateToIndex( pvPlayer ) ) )
		{
			override = true;
		}

		if( FuncIsValidTargetHook->Restore() )
		{
			#ifdef WIN32
				result = FuncIsvalidTargetOrig( pvPlayer, DUMMY_VAL, index, override ? false : checkteam );
			#else
				result = FuncIsvaludTargetOrig( pvPlayer, index, override ? false : checkteam );
			#endif

			FuncIsValidTargetHook->Patch();
		}

		return result;
	}

bool isAllowedPlayer( int index )
{
	return CvarFreeLookEnable->value > 0 && readFlags( CvarFreeLookFlags->string ) & MF_GetPlayerFlags( index );
}

int privateToIndex( const void* pdata )
{
	if( pdata )
	{
		char* ptr = ( char* )pdata;
		ptr += 4;

		entvars_t *pev = *( entvars_t ** )ptr;
		
		if( pev && pev->pContainingEntity )
		{
			return ENTINDEX( pev->pContainingEntity );
		}
	}

	return NULL;
}

int readFlags( const char* c )
{
	int flags = 0;

	while( *c )
	{
		flags |= ( 1 <<( *c++ - 'a' ) );
	}

	return flags;
}



