/*
 *
 *	HEART 0.1
 *
 *	mod.h
 *
 */

#ifndef _MOD_H_
#define _MOD_H_

#include "def.h"
#include "nt.h"
#include "str.h"
#include "mem.h"
#include "flist.h"

PPEB
HRTAPI
HrtModGetPEB(
	VOID );

V
HRTAPI
HrtModGetBase(
	PUC ModName );

PUC
HRTAPI
HrtModGetPath(
	PUC ModName );

V
HRTAPI
HrtModGetFunc(
	V ModBase,
	PC FuncName );

#endif