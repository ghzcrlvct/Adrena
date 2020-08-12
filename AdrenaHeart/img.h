/*
 *
 *	HEART 0.1
 *
 *	img.h
 *
 */

#ifndef _IMG_H_
#define _IMG_H_

#include "def.h"
#include "mem.h"
#include "ptr.h"
#include "mod.h"
#include "flist.h"

typedef ALIGN struct _HRT_PE_POINTERS {
	PIMAGE_DOS_HEADER			Dos;
	PIMAGE_NT_HEADERS			Nt;
	PIMAGE_FILE_HEADER 			File;
	PIMAGE_OPTIONAL_HEADER 		Opt;
	PIMAGE_DATA_DIRECTORY 		Imp;
	PIMAGE_DATA_DIRECTORY 		DelImp;
	PIMAGE_DATA_DIRECTORY		Exp;
	PIMAGE_DATA_DIRECTORY		BaseReloc;
	PIMAGE_DATA_DIRECTORY		TLS;
} HRT_PE_POINTERS, *PHRT_PE_POINTERS;

#define HRT_STORE_PE_POINTERS(X, Y) {												\
	X->Dos			= (PIMAGE_DOS_HEADER)(Y);										\
	X->Nt			= (PIMAGE_NT_HEADERS)(Y + X->Dos->e_lfanew);					\
	X->File			= (PIMAGE_FILE_HEADER)(&X->Nt->FileHeader);						\
	X->Opt			= (PIMAGE_OPTIONAL_HEADER)(&X->Nt->OptionalHeader);				\
	X->Imp			= &X->Opt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];			\
	X->DelImp		= &X->Opt->DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT];	\
	X->Exp			= &X->Opt->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];			\
	X->BaseReloc	= &X->Opt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];		\
	X->TLS			= &X->Opt->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];			\
}

#define HRT_FIX_PE_POINTERS HRT_STORE_PE_POINTERS

#define HRT_IMG_IMPTYPE_DEF			0x00000001
#define HRT_IMG_IMPTYPE_DEL			0x00000010

#define HRT_IMG_FLAG_NOREQDELIMP	0x00000001
#define HRT_IMG_FLAG_NOREQTLS		0x00000010
#define HRT_IMG_FLAG_NOREQEP		0x00000100

#define HRT_IMG_TYPE_EXE			0x00000001
#define HRT_IMG_TYPE_DLL			0x00000010

typedef VOID( __cdecl *f_Start )(
	VOID);

typedef BOOL( WINAPI *f_DllMain )(
	HINSTANCE	Inst,
	DWORD		Reason,
	LPVOID		Res );

NOGLOBAL
B
HRTAPI
HrtImpMapToMem(
	READONLY PUC		Path,
	PHRT_PE_POINTERS	Ptr );

NOGLOBAL
B
HRTAPI
HrtImgUnmapFromMem(
	PHRT_PE_POINTERS Ptr );

NOGLOBAL
B
HRTAPI
HrtImgRemapToMem(
	PHRT_PE_POINTERS	Ptr,
	PHRT_PE_POINTERS	NewPtr,
	READONLY PB			ReqBaseReloc );

NOGLOBAL
B
HRTAPI
HrtImgCopyHdr(
	PHRT_PE_POINTERS	SrcPtr,
	PHRT_PE_POINTERS	DestPtr );

NOGLOBAL
B
HRTAPI
HrtImgCopySect(
	PHRT_PE_POINTERS SrcPtr,
	PHRT_PE_POINTERS DestPtr );

NOGLOBAL
B
HRTAPI
HrtImgFixProt(
	PHRT_PE_POINTERS Ptr );

NOGLOBAL
B
HRTAPI
HrtImgFixImp(
	PHRT_PE_POINTERS	Ptr,
	READONLY V			Type );

NOGLOBAL
B
HRTAPI
HrtImgRelocBase(
	PHRT_PE_POINTERS	Ptr,
	V					OldBase );

NOGLOBAL
B
HRTAPI
HrtImgCallTLS(
	PHRT_PE_POINTERS Ptr );

NOGLOBAL
B
HRTAPI
HrtImgCallEP(
	PHRT_PE_POINTERS Ptr );
		
V
HRTAPI
HrtImgLoad(
	READONLY PUC	Path,
	READONLY V		Type,
	READONLY V		Flags );

B
HRTAPI
HrtImgTryRunFromMem(
	READONLY V Base );

#endif