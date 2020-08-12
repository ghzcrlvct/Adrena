/*
 *
 *	HEART 0.1
 *
 *	str.h
 *
 */

#ifndef _STR_H_
#define _STR_H_

#include "def.h"

#define HRT_STR_TOLOW(X)	((((X) >= ('A')) && ((X) <= ('Z'))) ? ((X) - (('Z') - ('z'))) : (X))
#define HRT_STR_TOUP(X)		((((X) >= ('a')) && ((X) <= ('z'))) ? ((X) - (('a') - ('A'))) : (X))
#define HRT_STR_TOINT(X)	((((X) >= ('0')) && ((X) <= ('9'))) ? ((X) - ('0')) : (X))

VOID
HRTAPI
HrtStrToU(
	PC		Src,
	PUC		Dest,
	V		Max );

VOID
HRTAPI
HrtStrToA(
	PUC		Src,
	PC		Dest,
	V		Max );

V
HRTAPI
HrtStrLenA(
	PC		Str,
	V		Max );

V
HRTAPI
HrtStrLenU(
	PUC		Str,
	V		Max );

B
HRTAPI
HrtStrMtchA(
	PC		First,
	PC		Second,
	V		Max );

#define HrtStrMtchAQck(First, Second, Max)		(((*First) == (*Second)) ? (HrtStrMtchA(First, Second, Max)) : (NO))

B
HRTAPI
HrtStrMtchU(
	PUC		First,
	PUC		Second,
	V		Max );

#define HrtStrMtchUQck(First, Second, Max)		(((*First) == (*Second)) ? (HrtStrMtchU(First, Second, Max)) : (NO))

B
HRTAPI
HrtStrMtchIA(
	PC		First,
	PC		Second,
	V		Max );

#define HrtStrMtchIAQck(First, Second, Max)		((HRT_STR_TOLOW(*First) == HRT_STR_TOLOW(*Second)) \
												? (HrtStrMtchIA(First, Second, Max)) : (NO))

B
HRTAPI
HrtStrMtchIU(
	PUC		First,
	PUC		Second,
	V		Max );

#define HrtStrMtchIUQck(First, Second, Max)		((HRT_STR_TOLOW(*First) == HRT_STR_TOLOW(*Second)) \
												? (HrtStrMtchIU(First, Second, Max)) : (NO))

PV
HRTAPI
HrtStrChrA(
	PC		Str,
	C		Char,
	V		Max );

PV
HRTAPI
HrtStrChrU(
	PUC		Str,
	UC		Char,
	V		Max );

VOID
HRTAPI
HrtStrAppA(
	PC		Src,
	PC		Dest,
	V		Max );

VOID
HRTAPI
HrtStrAppU(
	PUC		Src,
	PUC		Dest,
	V		Max );

VOID
HRTAPI
HrtStrEncA(
	PC		Str,
	C		Key,
	V		Max );

VOID
HRTAPI
HrtStrEncU(
	PUC		Str,
	UC		Key,
	V		Max );

VOID
HRTAPI
HrtStrDecA(
	PC		Str,
	C		Key,
	V		Max );

VOID
HRTAPI
HrtStrDecU(
	PUC		Str,
	UC		Key,
	V		Max );

#endif