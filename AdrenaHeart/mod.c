#include "mod.h"

PPEB
HRTAPI
HrtModGetPEB(
	VOID )
{
#ifdef HEART64
	return ( PPEB ) (__readgsqword( 0x60 ));
#else
	return ( PPEB ) (__readfsdword( 0x30 ));
#endif
}

V
HRTAPI
HrtModGetBase(
	PUC ModName )
{
	HRT_DBG_TRACE(
		"ModName: %ws\n",
		ModName );

	REGISTER PPEB					Peb			= HrtModGetPEB();
	if ( !Peb )
		goto Cleanup;

	REGISTER PPEB_LDR_DATA			Ldr			= Peb->Ldr;
	if ( !Ldr )
		goto Cleanup;

	REGISTER PLIST_ENTRY			Head		= &Ldr->InLoadOrderModuleList;
	REGISTER PLIST_ENTRY			Curr		= 0;
	REGISTER PLDR_DATA_TABLE_ENTRY	Entr		= 0;
	VV								ModBase		= 0;

	for ( Curr = Head->Flink; Curr != Head; Curr = Curr->Flink ) {
		Entr = CONTAINING_RECORD( Curr, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks );
		if ( !ModName ) {
			ModBase = Entr->DllBase;
			break;
		}

		if ( HrtStrMtchIUQck( ModName,
							  Entr->BaseDllName.Buffer,
							  50 ) ) {
			ModBase = Entr->DllBase;
			break;
		}
	}

Cleanup:

	HRT_DBG_TRACE(
		"Returning... Base: 0x%p\n",
		ModBase );

	if(!ModBase) {
		C CvModName[MAX_PATH];
		HrtMemZeroArr( CvModName );
		HrtStrToA( ModName,
				   CvModName,
				   MAX_PATH );
		ModBase = MyLoadLibraryA( CvModName );

		HRT_DBG_TRACE(
			"Attempted to load module! 0x%08X\n",
			ModBase );
	}

	return ModBase;
}

PUC
HRTAPI
HrtModGetPath(
	PUC ModName )
{
	HRT_DBG_TRACE(
		"ModName: %ws\n",
		ModName );

	REGISTER PPEB					Peb			= HrtModGetPEB();
	if ( !Peb )
		goto Cleanup;

	REGISTER PPEB_LDR_DATA			Ldr			= Peb->Ldr;
	REGISTER PLIST_ENTRY			Head		= &Ldr->InLoadOrderModuleList;
	REGISTER PLIST_ENTRY			Curr		= 0;
	REGISTER PLDR_DATA_TABLE_ENTRY	Entr		= 0;
	REGISTER PUC					ModPath		= 0;

	for ( Curr = Head->Flink; Curr != Head; Curr = Curr->Flink ) {
		Entr = CONTAINING_RECORD( Curr, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks );
		if ( !ModName ) {
			ModPath = Entr->FullDllName.Buffer;
			break;
		}

		if ( HrtStrMtchIUQck( ModName,
							  Entr->BaseDllName.Buffer,
							  50 ) ) {
			ModPath = Entr->FullDllName.Buffer;
			break;
		}
	}

Cleanup:

	HRT_DBG_TRACE(
		"Returning... Path: %ws\n",
		ModPath );

	return ModPath;
}

V
HRTAPI
HrtModGetFunc(
	V ModBase,
	PC FuncName )
{
	HRT_DBG_TRACE( "Mod base: 0x%08X\n",
				   ModBase );

	HRT_DBG_TRACE( "Func: %s\n",
				   FuncName );

	if ( !ModBase || !STRING_VALID( FuncName ) )
		goto Cleanup;

	REGISTER PIMAGE_DOS_HEADER			Dos		= ( PIMAGE_DOS_HEADER ) (ModBase);
	REGISTER PIMAGE_NT_HEADERS			Nt		= ( PIMAGE_NT_HEADERS ) (ModBase + Dos->e_lfanew);
	REGISTER PIMAGE_OPTIONAL_HEADER		Opt		= ( PIMAGE_OPTIONAL_HEADER ) (&Nt->OptionalHeader);
	REGISTER PIMAGE_DATA_DIRECTORY		ExpDir	= &Opt->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	if ( !ExpDir || !ExpDir->Size || !ExpDir->VirtualAddress )
		goto Cleanup;

	HRT_DBG_TRACE( "Valid PE...\n" );

	REGISTER PIMAGE_EXPORT_DIRECTORY	Exp		= ( PIMAGE_EXPORT_DIRECTORY ) (ModBase + ExpDir->VirtualAddress);

	REGISTER PV		Funcs	= ( PV ) (ModBase + Exp->AddressOfFunctions);
	REGISTER PV32	Names	= ( PV32 ) (ModBase + Exp->AddressOfNames);
	REGISTER PV16	Ords	= ( PV16 ) (ModBase + Exp->AddressOfNameOrdinals);

	HRT_DBG_TRACE( "Valid Export Directory...\n" );

	if ( MAXWORD >= ( V32 ) (FuncName) ) {
		REGISTER V16 Ord = LOWORD( FuncName ) - LOWORD( Exp->Base - 1 );
		REGISTER V32 Off = (( PV32 ) (ModBase + Funcs))[Ord];

		HRT_DBG_TRACE( "Function is ordinal...\n" );
	}

	HRT_DBG_TRACE( "Finding function ordinal...\n" );

	REGISTER V16 CurrOrd = 0;

	for ( VV i = 0; i < Exp->NumberOfNames; ++i ) {
		PC CurrFuncName = ( PC ) (ModBase + Names[i]);
		if ( HrtStrMtchAQck( FuncName,
							 CurrFuncName,
							 MAX_PATH ) ) {
			HRT_DBG_TRACE( "Found function ordinal as 0x%04X...\n",
						   Ords[i] );

			CurrOrd = Ords[i];
		}
	}
	
	if ( !CurrOrd )
		goto Cleanup;

	VV FuncOff = Funcs[CurrOrd];
	VV DirOff = ExpDir->VirtualAddress;

	if ( (FuncOff >= DirOff) && (FuncOff < (DirOff + ExpDir->Size)) ) {
		HRT_DBG_TRACE( "Function is forwarded!\n" );

		C Buff[MAX_PATH];
		HrtMemZeroArr( Buff );
		HrtMemCpy8( ( PV8 ) (ModBase + FuncOff),
					Buff,
					MAX_PATH );
		PC ForwFuncName = HrtStrChrA( Buff,
									  '.',
									  MAX_PATH );
		if ( !ForwFuncName )
			goto Cleanup;

		*ForwFuncName++ = '\0';
		if ( '#' == *ForwFuncName ) {
			ForwFuncName = ( PC ) (LOWORD( HRT_STR_TOINT( ( PC ) (++ForwFuncName) ) ));

			HRT_DBG_TRACE( "Function is forwaded by ordinal! 0x%04X\n",
						   ForwFuncName );
		}

		UC NewBuff[MAX_PATH];
		HrtStrToU( Buff,
				   NewBuff,
				   MAX_PATH );
		HrtStrAppU( U( ".DLL" ),
					NewBuff,
					MAX_PATH );

		HRT_DBG_TRACE( "Forwarded module: %ws\n",
					   NewBuff );

		VV NewModBase = HrtModGetBase( NewBuff );
		if ( !NewModBase || ModBase == NewModBase )
			goto Cleanup;

		HRT_DBG_TRACE(
			"Forwarded Function: %s->%ws:%s\n",
			FuncName,
			NewBuff,
			ForwFuncName );

		return HrtModGetFunc( NewModBase,
							  ForwFuncName );
	}

	HRT_DBG_TRACE(
		"Found %s: 0x%p\n",
		FuncName,
		(ModBase + FuncOff) );

	return (ModBase + FuncOff);

Cleanup:

	HRT_DBG_TRACE(
		"Failed! Returning...\n" );

	return 0;
}