/*
 *
 *	HEART 0.1
 *
 *	mut.h
 *
 */

#ifndef _MUT_H_
#define _MUT_H_

#include "def.h"
#include "util.h"
#include "ptr.h"
#include "flist.h"

typedef ALIGN struct _HRT_MUT_FUNC {
	V	Start;
	V	End;
	V	Size;
	V8	Key;
	B	IsEncrypted;
	V8	FirstByte;
} HRT_MUT_FUNC, *PHRT_MUT_FUNC;

typedef ALIGN struct _HRT_MUT_FUNC_DATA {
	HRT_MUT_FUNC	Function;
	PHRT_MUT_FUNC	Pointer;
} HRT_MUT_FUNC_DATA, *PHRT_MUT_FUNC_DATA;

#define		HRT_MUT_PRO_PUSHEBP			0x55
#define		HRT_MUT_EPI_RET				0xC3
#define		HRT_MUT_EPI_RET_X			0xC2

B
HRTAPI
HrtMutInit( V FuncAddr,
			PHRT_MUT_FUNC Func );

B
HRTAPI
HrtMutTogg( PHRT_MUT_FUNC Func );

B
HRTAPI
HrtMutRst( PHRT_MUT_FUNC Func );

#endif