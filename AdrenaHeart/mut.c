#include "mut.h"

B
HRTAPI
HrtMutInit( V FuncAddr,
			PHRT_MUT_FUNC Func )
{
	if ( !FuncAddr || !Func )
		return BAD;

	HRT_PTR_DEC_SELF( FuncAddr );
	HRT_PTR_DEC_SELF( Func );

	/*
	*	Go backwards starting from the function address until a prologue (Beginning of function) is found
	*/
	for ( REGISTER PV8 Curr = ( PV8 ) (FuncAddr);; --Curr ) {
		if ( HRT_MUT_PRO_PUSHEBP == *Curr ) {
			Func->Start = ( V ) (Curr);
			
			break;
		}
	}

	VV ProCnt = 0;

	/*
	*	Go forward starting from the start of the function until at least 1 epilogue (End of function) is found
	*/
	for ( REGISTER PV8 Curr = ( PV8 ) (Func->Start);; ++Curr ) {
		if ( HRT_MUT_PRO_PUSHEBP == *Curr )
			++ProCnt;

		if ( HRT_MUT_EPI_RET == *Curr || HRT_MUT_EPI_RET_X == *Curr ) {
			/*
			*	If at least 1 prologue has been found before the current epilogue (Meaning we have reached another
			*	function, so we know for sure the last epilogue address we found was the last one of the previous function),
			*	leave the loop
			*/
			if ( 1 < ProCnt )
				break;

			/*
			*	Save the current epilogue address found
			*/
			Func->End = ( V ) (Curr);
		}
	}

	/*
	*	Calculate the size of the function
	*/
	Func->Size			= Func->End - Func->Start;
	/*
	*	Generate an encryption key
	*/
	Func->Key			= HRT_UTIL_QCKRAND8( );
	Func->IsEncrypted	= NO;
	/*
	*	Save the first byte
	*/
	Func->FirstByte		= *( PV8 ) (Func->Start);

	/*
	*	Enable write permissions for the function's code
	*/
	V Old = 0;
	MyVirtualProtect( Func->Start,
					  Func->Size,
					  PAGE_EXECUTE_READWRITE,
					  &Old );

	/*
	*	Mutilate the function
	*/
	HrtMutTogg( HRT_PTR_ENC( Func ) );

	return GOOD;
}

B
HRTAPI
HrtMutTogg( PHRT_MUT_FUNC Func )
{
	if ( !Func )
		return BAD;

	HRT_PTR_DEC_SELF( Func );

	/*
	*	Encrypt the function
	*/
	for ( PV8 Curr = ( PV8 ) (Func->Start); Curr <= ( PV8 ) (Func->End); ++Curr ) {
		*Curr ^= Func->Key;
	}

	if(Func->IsEncrypted) {
		/*
		*	If the function was previously encrypted, restore the first byte
		*/
		*( PV8 ) (Func->Start)	= Func->FirstByte;
		Func->IsEncrypted		= NO;
	} else {
		/*
		*	If the function was previously unencrypted, set the first byte to a RET instruction
		*/
		*( PV8 ) (Func->Start)	= HRT_MUT_EPI_RET;
		Func->IsEncrypted		= YES;
	}

	return GOOD;
}

B
HRTAPI
HrtMutRst( PHRT_MUT_FUNC Func )
{
	HRT_PTR_DEC_SELF( Func );

	if ( Func->IsEncrypted )
		HrtMutTogg( Func );

	Func->Key = HRT_UTIL_QCKRAND8( );

	HrtMutTogg( Func );
}