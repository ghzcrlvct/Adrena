/*
 *
 *	HEART 0.1
 *
 *	vm.h
 *
 */

#ifndef _VM_H_
#define _VM_H_

#include "def.h"
#include "tp.h"

typedef ALIGN struct _HRT_CPUID_RES {
	V	EAX;
	V	EBX;
	V	ECX;
	V	EDX;
} HRT_CPUID_RES;

VOID
HRTAPI
HrtVmCpuInstr(
	VOID );

#ifdef HEART64
extern
VOID
HRTAPI
_HrtVmFp(
	VOID );
#endif

VOID
HRTAPI
HrtVmFpInstr(
	VOID );

B
HRTAPI
HrtVmChkInt(
	VOID );

B
HRTAPI
HrtVmChk(
	VOID );

#endif