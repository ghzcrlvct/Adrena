#include "ptr.h"

VOID
HRTAPI
HrtPtrInit(
	PHRT_PTR_SECRET		Ps,
	V					Secret )
{
	HrtMemZero( Ps,
				SIZEOF( *Ps ) );

	Ps->Secret = Secret;

	HRT_DBG_TRACE( "Secret: 0x%p\n",
				   Ps->Secret );
}

SV
HRTAPI
HrtPtrGetRef(
	PHRT_PTR_SECRET		Ps,
	PV					Ptr )
{
	if ( !Ptr )
		return -1;

	/*
	*	Go trough each pointer reference in the list
	*/
	for ( VV i = 0; i <= Ps->NumberOfReferences; i++ ) {
		/*
		*	If the pointer given matches the address of the reference at
		*	the current index, return the index number
		*/
		if ( ( V ) Ptr == &Ps->References[i] ) {
			return i;
		}
	}

	/*
	*	Return a bad reference
	*/
	return -1;
}

PV
HRTAPI
HrtPtrEnc(
	PHRT_PTR_SECRET		Ps,
	PV					OldPtr )
{
//	HRT_DBG_TRACE( "Ptr: 0x%p\n",
//				   OldPtr );

	/*
	*	If there is no more space for an extra pointer reference 
	*/
	if ( HRT_PTR_MAX_REF <= Ps->NumberOfReferences + 1 ) {
		HRT_DBG_TRACE( "Max Ref!\n",
					   OldPtr );

		return 0;
	}

	/*
	*	Encrypt the pointer with the secret and store it in the pointer reference list
	*/
	Ps->References[Ps->NumberOfReferences] = HRT_PTR_ALG( ( V ) OldPtr, Ps->Secret );

	/*
	*	Encrypt the pointer to the reference entry and return it
	*/
	return HRT_PTR_ALG( ( V ) (&Ps->References[Ps->NumberOfReferences++]), Ps->Secret );
}

PV
HRTAPI
HrtPtrDec(
	PHRT_PTR_SECRET		Ps,
	PV					OldPtr )
{
//	HRT_DBG_TRACE( "OldPtr: 0x%p\n",
//				   OldPtr );

	/*
	*	Decrypt the pointer
	*/
	OldPtr = HRT_PTR_ALG( ( V ) (OldPtr), Ps->Secret );

	/*
	*	Try to find the reference entry it points to
	*/
	SV RefNr = HrtPtrGetRef( Ps,
							 OldPtr );
	if ( HRT_PTR_BAD_REF( RefNr ) ) {
		HRT_DBG_TRACE(
			"Bad Ref!\n" );
	}

	/*
	*	Decrypt the pointer stored at the reference entry
	*/
	return HRT_PTR_ALG( ( V ) (Ps->References[RefNr]), Ps->Secret );
}