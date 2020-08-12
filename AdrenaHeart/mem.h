/*
 *
 *	HEART 0.1
 *
 *	mem.h
 *
 */

#ifndef _MEM_H_
#define _MEM_H_

#include "def.h"

#define SIZEOF(X)				(sizeof(X))
#define SIZEOFREF(X)			(sizeof(*X))
#define SIZEOFARR(X)			((sizeof(X)) / (sizeof(X[0])))

VOID
HRTAPI
HrtMemSet8(
	PV8		Dest,
	V8		Val,
	V		Size );

VOID
HRTAPI
HrtMemSet16(
	PV16	Dest,
	V16		Val,
	V		Size );

VOID
HRTAPI
HrtMemSet32(
	PV32	Dest,
	V32		Val,
	V		Size );

VOID
HRTAPI
HrtMemSet64(
	PV64	Dest,
	V64		Val,
	V		Size );

#ifdef HEART64
#define		HrtMemSet								HrtMemSet64
#else
#define		HrtMemSet								HrtMemSet32
#endif

#define		HrtMemSetAuto(Dest, Val)				HrtMemSet8(&Dest, Val, SIZEOF(Dest))
#define		HrtMemSetArr(DestArr, Val)				HrtMemSet8(DestArr, Val, SIZEOFARR(DestArr))

#define		HrtMemZero(Dest, Size)					HrtMemSet8(Dest, 0, Size)
#define		HrtMemZeroAuto(Dest)					HrtMemSetAuto(Dest, 0)
#define		HrtMemZeroArr(Arr)						HrtMemSetArr(Arr, 0)

VOID
HRTAPI
HrtMemCpy8(
	PV8		Src,
	PV8		Dest,
	V		Size );

VOID
HRTAPI
HrtMemCpy16(
	PV16	Src,
	PV16	Dest,
	V		Size );

VOID
HRTAPI
HrtMemCpy32(
	PV32	Src,
	PV32	Dest,
	V		Size );

VOID
HRTAPI
HrtMemCpy64(
	PV64	Src,
	PV64	Dest,
	V		Size );

#ifdef HEART64
#define		HrtMemCpy								HrtMemCpy64
#else
#define		HrtMemCpy								HrtMemCpy32
#endif

#define		HrtMemCpyAuto(Src, Dest)				HrtMemCpy8(Src, Dest, SIZEOF(Dest))
#define		HrtMemCpyArr(SrcArr, DestArr)			HrtMemCpy8(SrcArr, DestArr, SIZEOFARR(DestArr))

B
HRTAPI
HrtMemMtch8(
	PV8		First,
	PV8		Second,
	V		Size );

B
HRTAPI
HrtMemMtch16(
	PV16	First,
	PV16	Second,
	V		Size );

B
HRTAPI
HrtMemMtch32(
	PV32	First,
	PV32	Second,
	V		Size );

B
HRTAPI
HrtMemMtch64(
	PV64	First,
	PV64	Second,
	V		Size );

#define		HrtMemMtchQck8(First, Second, Size)		(((*First) == (*Second)) ? (HrtMemMtch8(First, Second, Size)) : (NO))
#define		HrtMemMtchQck16(First, Second, Size)	(((*First) == (*Second)) ? (HrtMemMtch16(First, Second, Size)) : (NO))
#define		HrtMemMtchQck32(First, Second, Size)	(((*First) == (*Second)) ? (HrtMemMtch32(First, Second, Size)) : (NO))
#define		HrtMemMtchQck64(First, Second, Size)	(((*First) == (*Second)) ? (HrtMemMtch64(First, Second, Size)) : (NO))

#ifdef HEART64
#define		HrtMemMtch								HrtMemMtch64
#define		HrtMemMtchQck							HrtMemMtchQck64
#else
#define		HrtMemMtch								HrtMemMtch32
#define		HrtMemMtchQck							HrtMemMtchQck32
#endif

#endif