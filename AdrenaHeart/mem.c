#include "mem.h"

VOID
HRTAPI
HrtMemSet8(
	PV8		Dest,
	V8		Val,
	V		Size )
{
	do {
		*Dest++ = Val;
	} while ( Size -= SIZEOF( V8 ) );
}

VOID
HRTAPI
HrtMemSet16(
	PV16	Dest,
	V16		Val,
	V		Size )
{
	do {
		*Dest++ = Val;
	} while ( Size -= SIZEOF( V16 ) );
}

VOID
HRTAPI
HrtMemSet32(
	PV32	Dest,
	V32		Val,
	V		Size )
{
	do {
		*Dest++ = Val;
	} while ( Size -= SIZEOF( V32 ) );
}

VOID
HRTAPI
HrtMemSet64(
	PV64	Dest,
	V64		Val,
	V		Size )
{
	do {
		*Dest++ = Val;
	} while ( Size -= SIZEOF( V64 ) );
}

VOID
HRTAPI
HrtMemCpy8(
	PV8		Src,
	PV8		Dest,
	V		Size )
{
	do {
		*Dest++ = *Src++;
	} while ( Size -= SIZEOF( V8 ) );
}

VOID
HRTAPI
HrtMemCpy16(
	PV16	Src,
	PV16	Dest,
	V		Size )
{
	do {
		*Dest++ = *Src++;
	} while ( Size -= SIZEOF( V16 ) );
}

VOID
HRTAPI
HrtMemCpy32(
	PV32	Src,
	PV32	Dest,
	V		Size )
{
	do {
		*Dest++ = *Src++;
	} while ( Size -= SIZEOF( V32 ) );
}

VOID
HRTAPI
HrtMemCpy64(
	PV64	Src,
	PV64	Dest,
	V		Size )
{
	do {
		*Dest++ = *Src++;
	} while ( Size -= SIZEOF( V64 ) );
}

B
HRTAPI
HrtMemMtch8(
	PV8		First,
	PV8		Second,
	V		Size )
{
	for ( VV i = Size; i != 0; i -= SIZEOF( V8 ) ) {
		if ( *First++ != *Second++ )
			return NO;
	}

	return YES;
}

B
HRTAPI
HrtMemMtch16(
	PV16	First,
	PV16	Second,
	V		Size )
{
	for ( VV i = Size; i != 0; i -= SIZEOF( V16 ) ) {
		if ( *First++ != *Second++ )
			return NO;
	}

	return YES;
}

B
HRTAPI
HrtMemMtch32(
	PV32	First,
	PV32	Second,
	V		Size )
{
	for ( VV i = Size; i != 0; i -= SIZEOF( V32 ) ) {
		if ( *First++ != *Second++ )
			return NO;
	}

	return YES;
}

B
HRTAPI
HrtMemMtch64(
	PV64	First,
	PV64	Second,
	V		Size )
{
	for ( VV i = Size; i != 0; i -= SIZEOF( V64 ) ) {
		if ( *First++ != *Second++ )
			return NO;
	}

	return YES;
}