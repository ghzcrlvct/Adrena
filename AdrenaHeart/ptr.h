/*
 *
 *	HEART 0.1
 *
 *	ptr.h
 *
 */

#ifndef _PTR_H_
#define _PTR_H_

#include "def.h"
#include "mem.h"
#include "util.h"

#define HRT_PTR_SECRET_NAME		g_HrtPtrSecret
#define HRT_PTR_MAX_REF			0x100

typedef ALIGN struct HRT_PTR_SECRET
{
	PV	References[HRT_PTR_MAX_REF];
	V	NumberOfReferences;
	V	Secret;
} HRT_PTR_SECRET, *PHRT_PTR_SECRET;

VOID
HRTAPI
HrtPtrInit(
	PHRT_PTR_SECRET		Ps,
	V					Secret );

SV
HRTAPI
HrtPtrGetRef(
	PHRT_PTR_SECRET		Secret,
	PV					Ptr );

PV
HRTAPI
HrtPtrEnc(
	PHRT_PTR_SECRET		Ps,
	PV					OldPtr );

PV
HRTAPI
HrtPtrDec(
	PHRT_PTR_SECRET		Ps,
	PV					OldPtr );

/*
*	Initialize the global pointer secret
*/
#ifdef HEART64
#define HRT_PTR_DEFAULT_SECRET 0x7FFFFFFFFFFFFFFF
#else
#define HRT_PTR_DEFAULT_SECRET 0x7FFFFFFF
#endif
#define HRT_PTR_INIT_GLOBAL		GLOBAL_SET( HRT_PTR_SECRET HRT_PTR_SECRET_NAME, { HRT_PTR_DEFAULT_SECRET } )
#define HRT_PTR_INIT(X)			HrtPtrInit(&HRT_PTR_SECRET_NAME, X)
#define HRT_PTR_INIT_RAND		HRT_PTR_INIT( HRT_UTIL_QCKRAND( __FUNCTIONW__ ) )

/*
*	Pointer encryption/decryption algorithm
*/
#define HRT_PTR_ALG(X, Y)		(~(X ^ Y))

/*
*	Check if a returned reference is valid
*/
#define	HRT_PTR_BAD_REF(X)		((-1) == (X))

/*
*	Quickly encrypt or decrypt pointers
*/
#define HRT_PTR_ENC(X)			HrtPtrEnc(&HRT_PTR_SECRET_NAME, X)
#define HRT_PTR_DEC(X)			HrtPtrDec(&HRT_PTR_SECRET_NAME, X)
#define HRT_PTR_ENC_SELF(X)		((X) = (HRT_PTR_ENC(X)))
#define HRT_PTR_DEC_SELF(X)		((X) = (HRT_PTR_DEC(X)))

/*
*	Create the new global pointer secret
*/
GLOBAL_NEW( HRT_PTR_SECRET HRT_PTR_SECRET_NAME );

#endif