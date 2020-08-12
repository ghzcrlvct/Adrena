/*
 *
 *	HEART 0.1
 *
 *	tp.h
 *
 */

#ifndef _TP_H_
#define _TP_H_

#include "def.h"
#include "mem.h"
#include "flist.h"

typedef ALIGN64 struct _HRT_TIMEPOINT
{
	LARGE_INTEGER	Frequency;
	LARGE_INTEGER	StartTime;
	LARGE_INTEGER	LastTime;
	V64				Difference;
	V64				Average;
	V64				RunCount;
} HRT_TIMEPOINT, *PHRT_TIMEPOINT;

VOID
HRTAPI
HrtTpInit(
	PHRT_TIMEPOINT Tp );

VOID
HRTAPI
HrtTpStart(
	PHRT_TIMEPOINT Tp );

VOID
HRTAPI
HrtTpNext(
	PHRT_TIMEPOINT Tp );

#endif