/*
 *
 *	HEART 0.1
 *
 *	unpk.h
 *
 */

#ifndef _UNPK_H_
#define _UNPK_H_

#include "def.h"
#include "flist.h"
#include "mem.h"
#include "mod.h"
#include "img.h"

#define HRT_UNPACK_KEYSIZE 10
#define HRT_UNPACK_CAVECNT 35

typedef struct _HRT_UNPACK_DATA_CAVE {
	V	StartOffset;
	V	EndOffset;
} HRT_UNPACK_DATA_CAVE, *PHRT_UNPACK_DATA_CAVE;

#define HRT_UNPK_FLAG_RUNINMEM		0x00000001
#define HRT_UNPK_FLAG_RUNFROMDISK	0x00000010

typedef struct _HRT_UNPACK_DATA {
	V						Flags;
	V						EncryptionKey[HRT_UNPACK_KEYSIZE];
	HRT_UNPACK_DATA_CAVE	CaveOffsets[HRT_UNPACK_CAVECNT];
	V						NumberOfPebbles;
	V						StartOffset;
	V						EndOffset;
	V						CheckSum;
} HRT_UNPACK_DATA, *PHRT_UNPACK_DATA;

typedef struct _HRT_LOCAL_PACK_DATA {
	PHRT_UNPACK_DATA	UnpackData;
	V					Base;
	V					Size;
	V					NewBase;
	V					NewSize;
} HRT_LOCAL_PACK_DATA, *PHRT_LOCAL_PACK_DATA;

VOID
HRTAPI
HrtUnpkInit(
	PHRT_LOCAL_PACK_DATA	Lpd,
	PHRT_UNPACK_DATA		Ud );

V
HRTAPI
HrtUnpkNewChkSum(
	PHRT_UNPACK_DATA Ud );

B
HRTAPI
HrtUnpkFindData(
	PHRT_LOCAL_PACK_DATA Lpd );

#define RVA_TO_RAW(Base, Off) ((Base) + (Off))

B
HRTAPI
HrtUnpkFixData(
	PHRT_LOCAL_PACK_DATA Lpd );

B
HRTAPI
HrtUnpkDecrData(
	PHRT_LOCAL_PACK_DATA Lpd );

B
HRTAPI
HrtUnpkPerform(
	PHRT_LOCAL_PACK_DATA	Lpd,
	PHRT_UNPACK_DATA		Ud );

B
HRTAPI
HrtUnpkRun(
	PHRT_LOCAL_PACK_DATA	Lpd,
	PHRT_UNPACK_DATA		Ud,
	READONLY V				Flags );

VOID
HRTAPI
HrtUnpkUnpack(
	VOID );

#endif