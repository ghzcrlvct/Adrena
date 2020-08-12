#include "vm.h"

VOID
HRTAPI
HrtVmCpuInstr(
	VOID )
{
	HRT_CPUID_RES Res;
	__cpuid( ( int * ) (&Res),
			 0x1 );
}

VOID
HRTAPI
HrtVmFpInstr(
	VOID )
{
#ifdef HEART64

	_HrtVmFp();
#else
	__asm {
		FYL2XP1;
	}
#endif
}

B
HRTAPI
HrtVmChkInt(
	VOID )
{
	REGISTER B		Result	= YES;
	VV				LapCnt	= 5;
	REGISTER V64	CpuRes	= 0;
	REGISTER V64	FpRes	= 0;
	LARGE_INTEGER	Freq;
	QueryPerformanceFrequency( &Freq );
	LARGE_INTEGER	Strt, End;

	/*
	*	Count the average time it takes to execute a CPUID instruction
	*/
	for ( VV i = LapCnt; i != 0; --i ) {
		MyQueryPerformanceCounter( &Strt );
		HrtVmCpuInstr();
		MyQueryPerformanceCounter( &End );
		V64 Temp = End.QuadPart - Strt.QuadPart;
		Temp *= 1000000000;
		Temp /= Freq.QuadPart;
		CpuRes += Temp;
	}

	/*
	*	Count the average time it takes to execute a FYL2XP1 instruction
	*/
	for ( VV i = LapCnt; i != 0; --i ) {
		MyQueryPerformanceCounter( &Strt );
		HrtVmFpInstr();
		MyQueryPerformanceCounter( &End );
		V64 Temp = End.QuadPart - Strt.QuadPart;
		Temp *= 1000000000;
		Temp /= Freq.QuadPart;
		FpRes += Temp;
	}

	/*
	*	If the CPUID instruction takes longer on average to exectute compared
	*	to the FYL2XP1 instruction
	*/
	if ( FpRes <= CpuRes )
		Result = YES;
	else
		Result = NO;

	return Result;
}

B
HRTAPI
HrtVmChk(
	VOID )
{
	REGISTER B	Res		= YES;
	VV			LapCnt	= 5;
	VV			PosCnt	= 0;
	VV			NegCnt	= 0;

	/*
	*	Run the internal VM check multiple times to get an average result
	*/
	for ( VV i = LapCnt; i != 0; --i ) {
		REGISTER B Res = HrtVmChkInt();
		if ( Res )
			++PosCnt;
		else
			++NegCnt;
	}

	/*
	*	If there are more positive results than negative results, the
	*	process is likely running inside a VM
	*/
	return (PosCnt >= NegCnt);
}