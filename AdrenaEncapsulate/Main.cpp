#include "Definitions.hpp"
#include "Image.hpp"
#include "Pack.hpp"

int wmain( int ArgCount,
		   wchar_t **Args )
{
	AInit();

	/*
	*	If less than 4 arguments have been passed
	*/
	constexpr unsigned int NeededArgCnt = 4;
	if ( ArgCount < (NeededArgCnt + 1) ) {
		AShowUsage();

		return 0;
	}

	/*
	*	Print the path to the Heart file
	*/
	ALog( ALogType::Information,
		  "HEART FILE IS \"%ws\"\n",
		  Args[1] );

	/*
	*	Load the Heart file (First argument)
	*/
	AImage DestImg = Args[1];
	if ( !DestImg.Success ) {
		ALog( ALogType::Fail,
			  "HEART FILE CANNOT BE FOUND!\n" );

		return 0;
	}

	/*
	*	Print the path to the Input file
	*/
	AImage SrcImg = Args[2];
	ALog( ALogType::Information,
		  "INPUT FILE IS \"%ws\"\n",
		  Args[2] );

	/*
	*	Load the Input file (Second argument)
	*/
	if ( !SrcImg.Success ) {
		ALog( ALogType::Fail,
			  "INPUT FILE CANNOT BE FOUND!\n" );

		return 0;
	}

	/*
	*	Print the path to the Output file (Third argument)
	*/
	ALog( ALogType::Information,
		  "OUTPUT FILE IS \"%ws\"\n",
		  Args[3] );

	/*
	*	Get the flags (Fourth argument)
	*/
	unsigned int Flags = std::stoi( Args[4] );
	ALog( ALogType::Information,
		  "FLAGS ARE 0x%08X\n",
		  Flags );

	/*
	*	Attempt to encapsulate the Input file
	*/
	APackEncapsulate( Args[3],
					  &SrcImg,
					  &DestImg,
					  Flags );
	
	return 0;
}