/*
 *
 *	HEART 0.1
 *
 *	def.h
 *
 */

#ifndef _DEF_H_
#define _DEF_H_

#define _WINVER			0x0601
#define _WIN32_WINNT	0x0601

#include <Windows.h>
#include <intrin.h>
#include <wchar.h>
#include <stdio.h>
#include <stdint.h>

#include <ShlObj.h>
#pragma comment(lib, "Shell32.lib")

#ifdef _WIN64
#define		HRTAPI				__fastcall
#define		HEART64
#else
#define		HRTAPI				__cdecl
#define		HEART32
#endif

typedef		int					SV,		*PSV;

typedef		unsigned char		V8,		*PV8;
typedef		unsigned short		V16,	*PV16;
typedef		unsigned int		V32,	*PV32;
typedef		unsigned long long	V64,	*PV64;
typedef		V8					B,		*PB;

#define		V8_SIZE				(sizeof(V8))
#define		V16_SIZE			(sizeof(V16))
#define		V32_SIZE			(sizeof(V32))
#define		V64_SIZE			(sizeof(V64))

#define		V8_BIT_SIZE			(8)
#define		V16_BIT_SIZE		(16)
#define		V32_BIT_SIZE		(32)
#define		V64_BIT_SIZE		(64)

#define		BAD					(B)(0)
#define		GOOD				(B)(1)

#define		NO					BAD
#define		YES					GOOD

#define		CUSTOM_ALIGN(X)		__declspec(align(X))

#define		ALIGN8				CUSTOM_ALIGN(0x1)
#define		ALIGN16				CUSTOM_ALIGN(0x2)
#define		ALIGN32				CUSTOM_ALIGN(0x4)
#define		ALIGN64				CUSTOM_ALIGN(0x8)

#ifdef HEART64
typedef		V64					V,		* PV;
#define		ALIGN				ALIGN64
#else
typedef		V32					V,		* PV;
#define		ALIGN				ALIGN32
#endif

typedef		char				C,		* PC;
typedef		wchar_t				UC,		* PUC;

#define		U(X)				(L##X)

#define		REGISTER			register

#define		GLOBAL				extern
#define		NOGLOBAL			static
#define		NOREINITIALIZE		static

#define		GLOBAL_NEW(X)		GLOBAL X
#define		GLOBAL_SET(X, Y)	GLOBAL X = Y

#define		READONLY			const

#define		INLINE				__forceinline
#define		NAKED				__declspec(naked)
#define		NORETURN			__declspec(noreturn)

#define		VV					REGISTER ALIGN V
#define		PVV					VV *

#define		STRING_VALID(X)		((X) && ((L'\0') != (X[0])))
#define		FLAG_SET(X, Y)		((Y) == ((X) & (Y)))

#define		NEG_TO_POS(X)		((X) = (~(X) + (1)))
#define		NEG_TO_POS_NR(X)	(~(X) + (1))
#define		ENSURE_POS(X)		(((0) > ((X) + (1))) ? (NEG_TO_POS(X)) : (X))

/*
*	Enable Debugging
*/
// #define HRT_DBG

#ifdef HRT_DBG
#define HRT_DBG_SINGLE_TRACE	printf
#define HRT_DBG_TRACE			HRT_DBG_SINGLE_TRACE( "[%s][%s][%d] ", __FILE__, __FUNCTION__, __LINE__); HRT_DBG_SINGLE_TRACE
#else
#define HRT_DBG_SINGLE_TRACE
#define HRT_DBG_TRACE
#endif

#endif