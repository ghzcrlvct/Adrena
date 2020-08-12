/*
 *
 *	HEART 0.1
 *
 *	nt.h
 *
 */

#ifndef _NT_H_
#define _NT_H_

#include "def.h"

typedef struct _UNICODE_STRING
{
	V16	Length;
	V16	MaximumLength;
	PUC	Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _PEB_LDR_DATA
{
	V32			Length;
	B			Initialized;
	V			SsHandle;
	LIST_ENTRY	InLoadOrderModuleList;
	LIST_ENTRY	InMemoryOrderModuleList;
	LIST_ENTRY	InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB
{
	V8				InheritedAddressSpace;
	V8				ReadImageFileExecOptions;
	V8				BeingDebugged;
	V8				SpareBool;
	V				Mutant;
	V				ImageBaseAddress;
	PPEB_LDR_DATA	Ldr;
} PEB, *PPEB;

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY		InLoadOrderLinks;
	LIST_ENTRY		InMemoryOrderLinks;
	LIST_ENTRY		InInitializationOrderLinks;
	V				DllBase;
	V				EntryPoint;
	V32				SizeOfImage;
	UNICODE_STRING	FullDllName;
	UNICODE_STRING	BaseDllName;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

#endif