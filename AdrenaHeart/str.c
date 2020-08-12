#include "str.h"

VOID
HRTAPI
HrtStrToU(
	PC		Src,
	PUC		Dest,
	V		Max )
{
	do {
		*Dest++ = *Src++;
	} while ( --Max );
}

VOID
HRTAPI
HrtStrToA(
	PUC		Src,
	PC		Dest,
	V		Max )
{
	do {
		*Dest++ = *Src++;
	} while( --Max );
}

V
HRTAPI
HrtStrLenA(
	PC		Str,
	V		Max )
{
	V Len = 0;
	while ( --Max && '\0' != *Str++ )
		++Len;

	return Len;
}

V
HRTAPI
HrtStrLenU(
	PUC		Str,
	V		Max )
{
	V Len = 0;
	while( --Max && '\0' != *Str++ )
		++Len;

	return Len;
}

B
HRTAPI
HrtStrMtchA(
	PC		First,
	PC		Second,
	V		Max )
{
	do {
		if ( (*First != *Second) && Max-- )
			return NO;
	} while ( L'\0' != *First++ && L'\0' != *Second++ );

	return YES;
}

B
HRTAPI
HrtStrMtchU(
	PUC		First,
	PUC		Second,
	V		Max )
{
	do {
		if ( (*First != *Second) && Max-- )
			return NO;
	} while( L'\0' != *First++ && L'\0' != *Second++ );

	return YES;
}

B
HRTAPI
HrtStrMtchIA(
	PC		First,
	PC		Second,
	V		Max )
{
	do {
		if ( (HRT_STR_TOLOW( *First ) != HRT_STR_TOLOW( *Second )) && Max-- )
			return NO;
	} while ( L'\0' != *First++ && L'\0' != *Second++ );

	return YES;
}

B
HRTAPI
HrtStrMtchIU(
	PUC		First,
	PUC		Second,
	V		Max )
{
	do {
		if ( (HRT_STR_TOLOW( *First ) != HRT_STR_TOLOW( *Second )) && Max-- )
			return NO;
	} while ( L'\0' != *First++ && L'\0' != *Second++ );

	return YES;
}

PV
HRTAPI
HrtStrChrA(
	PC		Str,
	C		Char,
	V		Max )
{
	while ( --Max && '\0' != *Str++ ) {
		if ( Char == *Str )
			return Str;
	}

	return 0;
}

PV
HRTAPI
HrtStrChrU(
	PUC		Str,
	UC		Char,
	V		Max )
{
	while ( --Max && '\0' != *Str++ ) {
		if ( Char == *Str )
			return Str;
	}

	return 0;
}

VOID
HRTAPI
HrtStrAppA(
	PC		Src,
	PC		Dest,
	V		Max )
{
	while ( '\0' != *Dest )
		++Dest;

	while ( --Max && '\0' != *Src ) {
		*Dest++ = *Src++;
	}

	*Dest = '\0';
}

VOID
HRTAPI
HrtStrAppU(
	PUC		Src,
	PUC		Dest,
	V		Max )
{
	while ( '\0' != *Dest )
		++Dest;
	
	while ( --Max && '\0' != *Src ) {
		*Dest++ = *Src++;
	}

	*Dest = '\0';
}

VOID
HRTAPI
HrtStrEncA(
	PC		Str,
	C		Key,
	V		Max )
{
	while(--Max && '\0' != *Str) {
		*Str++ ^= Key;
	}
}

VOID
HRTAPI
HrtStrEncU(
	PUC		Str,
	UC		Key,
	V		Max )
{
	while ( --Max && '\0' != *Str ) {
		*Str++ ^= Key;
	}
}

VOID
HRTAPI
HrtStrDecA(
	PC		Str,
	C		Key,
	V		Max )
{
	while ( Max-- ) {
		*Str++ ^= Key;
	}
	*Str = '\0';
}

VOID
HRTAPI
HrtStrDecU(
	PUC		Str,
	UC		Key,
	V		Max )
{
	while ( Max-- ) {
		*Str++ ^= Key;
	}
	*Str = '\0';
}