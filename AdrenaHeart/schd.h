/*
 *
 *	HEART 0.1
 *
 *	schd.h
 *
 */

#ifndef _SCHD_H_
#define _SCHD_H_

#include "def.h"
#include "mem.h"
#include "ptr.h"
#include "mut.h"

#define	HRT_SCHD_MAXTASK 50

typedef struct _HRT_SCHD_DATA {
	V	Tasks[HRT_SCHD_MAXTASK];
	V	TaskIndex;
	V	InitialTask;
	V	CheckSum;
} HRT_SCHD_DATA, *PHRT_SCHD_DATA;

VOID
HRTAPI
HrtSchdInit( PHRT_SCHD_DATA Sd );

NOGLOBAL
V
HRTAPI
HrtSchdChkSum( PHRT_SCHD_DATA Sd );

NOGLOBAL
VOID
HRTAPI
HrtSchdVerifChkSum( PHRT_SCHD_DATA Sd );

B
HRTAPI
HrtSchdAddTsks( PHRT_SCHD_DATA Sd,
				V TskCnt,
				... );

VOID
HRTAPI
HrtSchdTskNxt( PHRT_SCHD_DATA Sd );

#endif