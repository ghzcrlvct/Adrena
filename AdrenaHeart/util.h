/*
 *
 *	HEART 0.1
 *
 *	util.h
 *
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include "tp.h"
#include "mem.h"
#include "str.h"
#include "ptr.h"

V64
HRTAPI
HrtUtilQckRand(
	VOID );

/*
*	Generate random numbers of different sizes by right shifting the default 64-bit seed
*/

#define HRT_UTIL_QCKRAND8()		(HrtUtilQckRand() >> (V64_BIT_SIZE - V8_BIT_SIZE))
#define HRT_UTIL_QCKRAND16()	(HrtUtilQckRand() >> (V64_BIT_SIZE - V16_BIT_SIZE))
#define HRT_UTIL_QCKRAND32()	(HrtUtilQckRand() >> (V64_BIT_SIZE - V32_BIT_SIZE))
#define HRT_UTIL_QCKRAND64()	(HrtUtilQckRand())

#ifdef HEART64
#define HRT_UTIL_QCKRAND HRT_UTIL_QCKRAND64
#else
#define HRT_UTIL_QCKRAND HRT_UTIL_QCKRAND32
#endif

NORETURN
VOID
HRTAPI
HrtUtilExtCorrHeap(
	VOID );

#define HRT_EXIT HrtUtilExtCorrHeap()

VOID
HRTAPI
HrtUtilDropAndExec(
	V Bin,
	V Size );

#endif