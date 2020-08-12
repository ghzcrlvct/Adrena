/*
 *
 *	HEART 0.1
 *
 *	flist.h
 *
 */

#ifndef _FLIST_H_
#define _FLIST_H_

#include "def.h"
#include "mod.h"
#include "ptr.h"

GLOBAL_NEW( B g_FListIsEncrypted );
GLOBAL_NEW( V g_CreateFileW );
GLOBAL_NEW( V g_GetFileSize );
GLOBAL_NEW( V g_VirtualAlloc );
GLOBAL_NEW( V g_VirtualFree );
GLOBAL_NEW( V g_VirtualProtect );
GLOBAL_NEW( V g_CloseHandle );
GLOBAL_NEW( V g_ReadFile );
GLOBAL_NEW( V g_LoadLibraryA );
GLOBAL_NEW( V g_QueryPerformanceFrequency );
GLOBAL_NEW( V g_QueryPerformanceCounter );
GLOBAL_NEW( V g_ConvertThreadToFiber );
GLOBAL_NEW( V g_CreateFiber );
GLOBAL_NEW( V g_SwitchToFiber );
GLOBAL_NEW( V g_ShowWindow );
GLOBAL_NEW( V g_GetConsoleWindow );

GLOBAL_NEW( C g_StringKey );

VOID
HRTAPI
HrtFListPrepare(
	VOID );

typedef B( HRTAPI *f_HrtFListInit )( VOID );

B
HRTAPI
HrtFListInit(
	VOID );

typedef HANDLE( WINAPI *f_CreateFileW )( LPCWSTR					FileName,
										 DWORD						DesiredAccess,
										 DWORD						ShareMode,
										 LPSECURITY_ATTRIBUTES		SecurityAttributes,
										 DWORD						CreationDisposition,
										 DWORD						FlagsAndAttributes,
										 HANDLE						TemplateFile );

V
HRTAPI
MyCreateFileW(
	PUC FileName,
	V	DesiredAccess,
	V	ShareMode,
	V	CreationDisposition );

typedef DWORD( WINAPI *f_GetFileSize )( HANDLE		File,
										LPDWORD		FileSizeHigh );

V
HRTAPI
MyGetFileSize(
	V File );

typedef LPVOID( WINAPI *f_VirtualAlloc )( LPVOID	Address,
										  SIZE_T	Size,
										  DWORD		AllocationType,
										  DWORD		Protect );

V
HRTAPI
MyVirtualAlloc(
	V Address,
	V Size,
	V AllocationType,
	V Protect );

typedef BOOL( WINAPI *f_VirtualFree )( LPVOID		Address,
									   SIZE_T		Size,
									   DWORD		FreeType );

SV
HRTAPI
MyVirtualFree(
	V Address,
	V Size,
	V FreeType );

typedef BOOL( WINAPI *f_VirtualProtect )( LPVOID	Address,
										  SIZE_T	Size,
										  DWORD		NewProtect,
										  PDWORD	OldProtect );

SV
HRTAPI
MyVirtualProtect(
	V	Address,
	V	Size,
	V	NewProtect,
	PV	OldProtect );

typedef BOOL( WINAPI *f_CloseHandle )( HANDLE Object );

SV
HRTAPI
MyCloseHandle(
	V Object );

typedef BOOL( WINAPI *f_ReadFile )( HANDLE			File,
									LPVOID			Buffer,
									DWORD			NumberOfBytesToRead,
									LPDWORD			NumberOfBytesRead,
									LPOVERLAPPED	Overlapped );

SV
HRTAPI
MyReadFile(
	V File,
	V Buffer,
	V NumberOfBytes );

typedef HMODULE( WINAPI *f_LoadLibraryA )( LPCSTR LibFileName );

V
HRTAPI
MyLoadLibraryA(
	PC LibFileName );

typedef BOOL( WINAPI *f_QueryPerformanceFrequency )(PLARGE_INTEGER Frequency);

B
HRTAPI
MyQueryPerformanceFrequency(
	PLARGE_INTEGER Frequency );

typedef BOOL( WINAPI *f_QueryPerformanceCounter )(PLARGE_INTEGER PerformanceCount);

B
HRTAPI
MyQueryPerformanceCounter(
	PLARGE_INTEGER PerformanceCount );

typedef LPVOID( WINAPI *f_ConvertThreadToFiber )(LPVOID Parameter);

PV
HRTAPI
MyConvertThreadToFiber(
	PV Parameter );

typedef LPVOID( WINAPI *f_CreateFiber )( SIZE_T StackSize,
										 LPFIBER_START_ROUTINE StartAddress,
										 LPVOID Parameter );

PV
HRTAPI
MyCreateFiber(
	PV StartAddress,
	PV Parameter );

typedef VOID( WINAPI *f_SwitchToFiber )(LPVOID Fiber);

VOID
HRTAPI
MySwitchToFiber(
	PV Fiber );

typedef BOOL( WINAPI *f_ShowWindow )( HWND Window,
									  int Show );

VOID
HRTAPI
MyShowWindow(
	V Window,
	V Show );

typedef HWND( WINAPI *f_GetConsoleWindow )(VOID);

V
HRTAPI
MyGetConsoleWindow(
	VOID );

#endif