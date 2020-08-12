/*
*
*	HEART 0.1
*
*	Definitions.hpp
*
*/

#ifndef _DEFINITIONS_HPP_
#define _DEFINITIONS_HPP_

#include <Windows.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <ImageHlp.h>
#pragma comment(lib, "ImageHlp.lib")

#define ReCa	reinterpret_cast
#define StCa	static_cast

/*
*	Enable debugging
*/
//#define A_DBG_ENABLED

enum class ALogType {
	Success,
	Fail,
	Information
};

void ALogClr( ALogType Type )
{
	static void *Out = GetStdHandle( STD_OUTPUT_HANDLE );

	switch(Type) {
		case(ALogType::Success): {
			SetConsoleTextAttribute( Out,
									 0xA0 );
			printf( "%% Success %%" );
			break;
		}
		case(ALogType::Fail):
		{
			SetConsoleTextAttribute( Out,
									 0xC0 );
			printf( "%% Fail %%" );
			break;
		}
		case(ALogType::Information):
		{
			SetConsoleTextAttribute( Out,
									 0xB0 );
			printf( "%% Information %%" );
			break;
		}
	}

	SetConsoleTextAttribute( Out,
							 0x0F );
	printf( "\t\t" );
}

template<typename... ArgumentPack>
void ALog( ALogType Type,
		   const char *Format,
		   ArgumentPack... Arguments )
{
	ALogClr( Type );
	printf( Format,
			Arguments... );
}

template<typename... ArgumentPack>
void ALogSin( const char *Format,
			  ArgumentPack... Arguments )
{
	printf( Format,
			Arguments... );
}

#ifdef A_DBG_ENABLED
#define ALog ALogSin("[%ws][%d][LE:%d] ", __FUNCTIONW__, __LINE__, GetLastError()); ALog
#define ALogSin ALogSin
#else
#define ALog
#define ALogSin
#endif

void ARandInit( void )
{
	srand( GetTickCount() );
}

unsigned int ARand( void )
{
	/*
	*	Combine 2 random integers into 1 random unsigned integer
	*/
	return (static_cast< unsigned int >(rand()) << 16 | static_cast< unsigned int >(rand()) ^ GetTickCount());
}

void AInit( void )
{
	SetConsoleTitleW( L"ADRENA ~ ENCAPSULATE" );

	ARandInit();
}

void AShowUsage( void )
{
	printf( "%% ADRENA ~ ENCAPSULATE %%\nUSAGE:\t adr_enc.exe [HEART FILE PATH] [INPUT FILE PATH] [OUTPUT FILE PATH] [FLAGS]\n" );
}

#endif