#include "tp.h"

VOID
HRTAPI
HrtTpInit(
	PHRT_TIMEPOINT Tp )
{
	HrtMemSet8( Tp,
				0,
				SIZEOF( *Tp ) );

	MyQueryPerformanceFrequency( &Tp->Frequency );
}

VOID
HRTAPI
HrtTpStart(
	PHRT_TIMEPOINT Tp )
{
	MyQueryPerformanceCounter( &Tp->StartTime );
}

VOID
HRTAPI
HrtTpNext(
	PHRT_TIMEPOINT Tp )
{
	MyQueryPerformanceCounter( &Tp->LastTime );

	/*
	*	Subtract the start time from the current time, multiply by 10^9 to get nanoseconds
	*	and divide by the frequency to get an accurate time
	*/
	Tp->Difference		= Tp->LastTime.QuadPart - Tp->StartTime.QuadPart;
	Tp->Difference		*= 1000000000;
	Tp->Difference		/= Tp->Frequency.QuadPart;
	
	/*
	*	Add the current time difference to the average
	*/
	Tp->Average			+= Tp->Difference;
	if ( Tp->RunCount )
		/*
		*	If this function has been called more than once (So Tp->Average contains more than one
		*	added time difference), divide the number by 2
		*/
		Tp->Average		/= 2;

	++Tp->RunCount;
}