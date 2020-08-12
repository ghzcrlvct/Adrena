#include "schd.h"

VOID
HRTAPI
HrtSchdInit( PHRT_SCHD_DATA Sd ) 
{
	/*
	*	Decrypt the pointer
	*/
	HRT_PTR_DEC_SELF( Sd );

	HrtMemZero( Sd,
				SIZEOFREF( Sd ) );

	/*
	*	Convert the current thread to a fiber
	*/
	MyConvertThreadToFiber( 0 );
	/*
	*	After all tasks run, exit the process
	*/
	Sd->InitialTask = MyCreateFiber( HrtUtilExtCorrHeap,
									 0 );
}

V
HRTAPI
HrtSchdChkSum( PHRT_SCHD_DATA Sd )
{
	/*
	*	Decrypt the pointer
	*/
	HRT_PTR_DEC_SELF( Sd );

	VV ChkSum = 0;

	/*
	*	Go trough the task list byte by byte and calculate a checksum
	*/
	PV8 Strt	= ( PV8 ) (Sd->Tasks);
	PV8 End		= Strt + sizeof( Sd->Tasks );
	for ( ; Strt < End; ++Strt ) {
		ChkSum += *Strt;
	}

	/*
	*	Ensure the result is positive
	*/
	ENSURE_POS( ChkSum );

	return ChkSum;
}

VOID
HRTAPI
HrtSchdVerifChkSum( PHRT_SCHD_DATA Sd )
{
	/*
	*	Calculate the current checksum and compare with the old one
	*/
	VV CurrChkSum = HrtSchdChkSum( Sd );

	/*
	*	Decrypt the pointer
	*/
	HRT_PTR_DEC_SELF( Sd );

	if ( CurrChkSum != Sd->CheckSum )
		HRT_EXIT;
}

B
HRTAPI
HrtSchdAddTsks( PHRT_SCHD_DATA Sd,
				V TskCnt,
				... )
{
	REGISTER B Status = BAD;

	if ( !TskCnt || TskCnt >= HRT_SCHD_MAXTASK )
		goto ErrorOccured;

	/*
	*	Capture all function addresses from the stack
	*/
	va_list Lst;
	va_start( Lst,
			  TskCnt );

	/*
	*	Decrypt the pointer
	*/
	HRT_PTR_DEC_SELF( Sd );

	/*
	*	Convert all functions to tasks and add them to the list
	*/
	for ( VV i = 0; i < TskCnt; ++i ) {
		Sd->Tasks[i] = MyCreateFiber( va_arg( Lst, V ),
									  HRT_PTR_ENC( Sd ) );
		if ( !Sd->Tasks[i] )
			goto ErrorOccured;
	}

	va_end( Lst );

	/*
	*	Calculate a checksum and save it
	*/
	Sd->CheckSum = HrtSchdChkSum( HRT_PTR_ENC( Sd ) );
	if ( !Sd->CheckSum )
		HRT_EXIT;

	Status = GOOD;

ErrorOccured:

	return Status;
}

VOID
HRTAPI
HrtSchdTskNxt( PHRT_SCHD_DATA Sd )
{
	/*
	*	Verify the checksum
	*/
	HrtSchdVerifChkSum( Sd );

	/*
	*	Decrypt the pointer
	*/
	HRT_PTR_DEC_SELF( Sd );

	/*
	*	Save the address of the next task that needs to execute
	*/
	VV TskCpy = Sd->Tasks[Sd->TaskIndex++];
	/*
	*	If there are no more tasks to execute, execute the initial task
	*/
	if ( !TskCpy )
		TskCpy = Sd->InitialTask;

	/*
	*	Execute the next task
	*/
	MySwitchToFiber( TskCpy );
}