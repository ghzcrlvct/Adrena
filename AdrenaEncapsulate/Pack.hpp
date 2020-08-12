/*
*
*	HEART 0.1
*
*	Pack.hpp
*
*/

#ifndef _PACK_HPP_
#define _PACK_HPP_

#include "Definitions.hpp"
#include "Image.hpp"

/*
*	Pack flags
*/
constexpr unsigned int ENC_FLAG_RUNINMEM		= 0x00000001;	/* Run the Input file using the manual loader */
constexpr unsigned int ENC_FLAG_RUNFROMDISK		= 0x00000010;	/* Drop the input file on disk and run it */

constexpr unsigned int ENC_PACK_KEYSIZE			= 10;	/* The size of the encryption key */
constexpr unsigned int ENC_PACK_CAVECNT			= 35;	/* The number of caves */

/*
*	Cave offsets
*/
struct APackDataCave {
	unsigned int	CaveStartOffset;
	unsigned int	CaveEndOffset;
};

/*
*	Pack information
*/
struct APackData {
	unsigned int	Flags;
	unsigned int	EncryptionKey[ENC_PACK_KEYSIZE];
	APackDataCave	CaveOffsets[ENC_PACK_CAVECNT];
	unsigned int	NumberOfPebbles;
	unsigned int	StartOffset;
	unsigned int	EndOffset;
	unsigned int	CheckSum;
};
using PAPackData = APackData *;

/*
*	Generate the encryption key
*/
bool APackGenerateKey( PAPackData	Pd )
{
	ALog( ALogType::Information,
		  "Generating key...\n" );

	/*
	*	For each key part
	*/
	for ( unsigned int i = 0; i < ENC_PACK_KEYSIZE; ++i ) {
		/*
		*	Generate a random unsigned integer for the current key part
		*/
		Pd->EncryptionKey[i] = ARand();
		
		/*
		*	If the current key part is zero
		*/
		if ( !Pd->EncryptionKey[i] ) {
			ALog( ALogType::Fail,
				  "Key generation failed!\n" );
			
			return false;
		}
	}

	ALog( ALogType::Success,
		  "Key generation succeeded!\n" );
	
	return true;
}

/*
*	Encrypt the Input file
*/
bool APackEncrypt( PAPackData	Pd,
				   PAImage		Src )
{
	ALog( ALogType::Information,
		  "Encrypting...\n" );

	/*
	*	Get the start and end of the Input file in memory
	*/
	unsigned int	*SrcStrt	= ReCa< unsigned int * >(Src->Base);
	unsigned int	*SrcEnd		= ReCa< unsigned int * >(ReCa< unsigned char * >(SrcStrt) + Src->Size);
	unsigned int	KeyInd		= 0;
	
	/*
	*	While the end of the file has not been reached
	*/
	while ( SrcStrt < SrcEnd ) {
		/*
		*	Encrypt the current 4-bytes of the file with the key at the current key index
		*/
		*SrcStrt++ ^= Pd->EncryptionKey[KeyInd++];

		/*
		*	If the key index reaches the size of the key, reset it to 0
		*/
		if ( KeyInd == ENC_PACK_KEYSIZE )
			KeyInd = 0;
	}

	ALog( ALogType::Success,
		  "Encryption succeeded!\n" );

	return true;
}

/*
*	Calculate the number of pebbles required for a cave
*/
bool APackCalculatePebbles( PAPackData		Pd,
							PAImage			SrcImg )
{
	ALog( ALogType::Information,
		  "Calculating required pebbles for binary...\n",
		  Pd->NumberOfPebbles );

	/*
	*	The number of pebbles required is the ceiling of the size of the Input file in bytes divided by the number of caves
	*/
	Pd->NumberOfPebbles = ceil( SrcImg->Size / ENC_PACK_CAVECNT );
	/*
	*	If the number of pebbles calculated is 0
	*/
	if ( !Pd->NumberOfPebbles ) {
		ALog( ALogType::Fail,
			  "Cannot calculate required number of pebbles!\n" );

		return false;
	}

	ALog( ALogType::Information,
		  "The binary requires %d pebbles for 25 caves!\n",
		  Pd->NumberOfPebbles );

	return true;
}

/*
*	Holds information about the new image (The Input file's caves + the pack data)
*/
struct ANewImage {
	void			*Base;
	unsigned int	Size;
};
using PANewImage = ANewImage *;

/*
*	Create and fill the caves with data
*/
bool APackFillCaves( PAPackData		Pd,
					 PAImage		SrcImg,
					 PAImage		DestImg,
					 PANewImage		NewImg )
{
	ALog( ALogType::Information,
		  "Filling caves...\n" );

	/*
	*	The new size of the image is the size of the Heart file + The total number of pebbles in all caves
	*	(There are 2 caves, one for code and one for indirection, a.k.a. zeros) + The size of the pack data
	*/
	NewImg->Size = DestImg->Size + ((ENC_PACK_CAVECNT * Pd->NumberOfPebbles) * 2) + sizeof( APackData );
	ALog( ALogType::Information,
		  "Binary's size will be extended from %dkb to %dkb\n",
		  SrcImg->Size / 1000,
		  (NewImg->Size + DestImg->Size) / 1000 );

	/*
	*	Allocate memory for the new image
	*/
	NewImg->Base = VirtualAlloc( nullptr,
								 NewImg->Size,
								 MEM_COMMIT | MEM_RESERVE,
								 PAGE_READWRITE );
	if ( !NewImg->Base )
		return false;

	/*
	*	The start of the Input file in memory
	*/
	unsigned char	*SrcStart	= ReCa< unsigned char * >(SrcImg->Base);
	/*
	*	The start of the new image in memory
	*/
	unsigned char	*Start		= ReCa< unsigned char * >(NewImg->Base);
	/*
	*	The end of the new image in memory
	*/
	unsigned char	*End		= Start + NewImg->Size;
	/*
	*	The number of caves filled (The code and indirection caves count as 1)
	*/
	unsigned int	Count		= 0;
	/*
	*	Current offset in the new image's memory
	*/
	unsigned int	Off			= DestImg->Size;

	/*
	*	While the end of the new image has not been reached
	*/
	while ( Start <= End ) {
		/*
		*	Save the offset of the current cave (Code cave)
		*/
		Pd->CaveOffsets[Count].CaveStartOffset = Off;
		ALogSin( "\t\t\tCave %d: Code=0x%08X, Indirection=",
				 Count + 1,
				 Pd->CaveOffsets[Count].CaveStartOffset );

		/*
		*	Copy the memory at the current offset in the Input file into the new image
		*/
		memcpy( Start,
				SrcStart,
				Pd->NumberOfPebbles );
		/*
		*	Advance the offset in the Input file, the new image and the offset variable by
		*	the number of bytes that were copied (The number of pebbles)
		*/
		Start		+= Pd->NumberOfPebbles;
		SrcStart	+= Pd->NumberOfPebbles;
		Off			+= Pd->NumberOfPebbles;

		/*
		*	Save the offset for the current cave (Indirection cave)
		*/
		Pd->CaveOffsets[Count].CaveEndOffset = Off;
		ALogSin( "0x%08X\n",
				 Pd->CaveOffsets[Count].CaveEndOffset );

		/*
		*	Set the memory in the indirection cave to 0
		*/
		memset( Start,
				0,
				Pd->NumberOfPebbles );
		/*
		*	Advance the offset in the image memory and offset variable (We are not copying
		*	memory from the Input file, so we are not advancing the offset there)
		*/
		Start		+= Pd->NumberOfPebbles;
		Off			+= Pd->NumberOfPebbles;

		/*
		*	If the maximum number of caves has been reached (Meaning all of the caves were created)
		*/
		if ( ++Count == ENC_PACK_CAVECNT ) {
			/// !!!
			/// TO FIX: If important stuff is at the end of the file, this might zero it out. Prefer copying the memory instead
			/// !!!
			/*
			*	Calculate the number of bytes needed for padding
			*/
			unsigned int RemainSize = ReCa< unsigned int >(End) - ReCa< unsigned int >(Start);
			ALogSin( "\t\t\tPadding Size: %d\n",
					 RemainSize );
			
			/*
			*	Set the memory at the end of the new image to 0
			*/
			memset( Start,
					0,
					RemainSize );

			/*
			*	Increase the offset by the number of padded bytes
			*/
			Off += RemainSize;

			break;
		}
	}

	/*
	*	If less than the needed number of caves (25) have been filled
	*/
	if ( Count < ENC_PACK_CAVECNT ) {
		ALog( ALogType::Fail,
			  "Cannot fill caves!\n" );

		return false;
	}

	ALog( ALogType::Success,
		  "Filled %d caves!\n",
		  Count );

	/*
	*	Save the start and end offset of the Input file (Now split into caves) in the
	*	pack data structure
	*/
	Pd->StartOffset		= DestImg->Size;
	Pd->EndOffset		= Off;

	ALog( ALogType::Information,
		  "Start=0x%08X, End=0x%08X\n",
		  Pd->StartOffset,
		  Pd->EndOffset );

	return true;
}

/*
*	If a number is negative, turn it into a positive number (And vice-versa)
*/
#define ENSURE_POS(X) (((0) > ((X) + (1))) ? (((X) = (~(X) + (1)))) : (X))

/*
*	Calculate the checksum of the pack data structure
*/
bool APackGenChkSum( PAPackData Pd )
{
	ALog( ALogType::Information,
		  "Calculating checksum...\n" );

	/*
	*	The size of the structure that will take part in the check sum (The CheckSum field, last one, is ommited)
	*/
	unsigned int Size		= sizeof( *Pd ) - sizeof( Pd->CheckSum );
	/*
	*	Get the start of the structure in memory
	*/
	unsigned char *Strt		= ReCa< unsigned char * >(Pd);
	/*
	*	Get the end of the structure in memory
	*/
	unsigned char *End		= ReCa< unsigned char * >(Pd) + Size;
	/*
	*	Store checksum here
	*/
	unsigned int CheckSum	= 0;

	/*
	*	While the end of the structure has not been reached
	*/
	for ( ; Strt < End; ++Strt ) {
		/*
		*	Add the current byte of the structure to the total sum
		*/
		CheckSum += *Strt;
	};

	/*
	*	Ensure the number has not overflowed (Does not apply anymore as I am not adding unsigned integers,
	*	but unsigned chars, but I will leave it here for the future(
	*/
	ENSURE_POS( CheckSum );

	ALog( ALogType::Success,
		  "Checksum calculated succesfully! (0x%08X)\n",
		  CheckSum );

	/*
	*	Store the calculate checksum in the pack data structure
	*/
	Pd->CheckSum = CheckSum;

	return true;
}

/*
*	Save the Heart, Input file (Now split into caves, so it is part of the new image)
*	and pack data structure into a single file
*/
bool APackOutput( const wchar_t		*OutName,
				  PAPackData		Pd,
				  PANewImage		NewImg,
				  PAImage			DestImg )
{
	/*
	*	Create the new file
	*/
	void *File = CreateFileW( OutName,
							  FILE_READ_ACCESS | FILE_WRITE_ACCESS | FILE_APPEND_DATA,
							  FILE_SHARE_READ | FILE_SHARE_WRITE,
							  nullptr,
							  CREATE_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL,
							  nullptr );
	if ( !File )
		return false;

	/*
	*	Write the Heart file to the new file
	*/
	if ( !WriteFile( File,
					 DestImg->Base,
					 DestImg->Size,
					 nullptr,
					 nullptr ) ) {
		ALog( ALogType::Fail,
			  "Cannot write Heart binary to output file!\n" );
		CloseHandle( File );
		return false;
	}

	/*
	*	Write the new image to the file (Input file split into caves)
	*/
	if ( !WriteFile( File,
					 NewImg->Base,
					 NewImg->Size,
					 nullptr,
					 nullptr ) ) {
		ALog( ALogType::Fail,
			  "Cannot write input binary to output file!\n" );
		CloseHandle( File );
		return false;
	}

	/*
	*	Write the pack data structure to the file
	*/
	if ( !WriteFile( File,
					 Pd,
					 sizeof( APackData ),
					 nullptr,
					 nullptr ) ) {
		ALog( ALogType::Fail,
			  "Cannot write data to output file!\n" );
		CloseHandle( File );
		return false;
	}

	ALog( ALogType::Success,
		  "Files written succesfully!\n" );
	CloseHandle( File );
	return true;
}

/*
*	Save the PE header checksum of the new file
*/
void APackFixPECheckSum( const wchar_t *OutPath,
						 void *File )
{
	/*
	*	Calculate the checksum
	*/
	DWORD OldChk = 0;
	DWORD NewChk = 0;
	MapFileAndCheckSumW( OutPath, &OldChk, &NewChk );

	ALog( ALogType::Information,
		  "Old CheckSum: 0x%08X, New CheckSum: 0x%08X\n",
		  OldChk,
		  NewChk );

	DWORD Last = {};

	/*
	*	Move the file pointer to the offset of the file's checksum
	*/
	OVERLAPPED Ovr = {};
	Ovr.Offset = 0x158;

	/*
	*	Write the new checksum value
	*/
	WriteFile( File,
			   &NewChk,
			   sizeof( NewChk ),
			   &Last,
			   &Ovr );
}

/*
*	Check if the input file has a valid PE header
*/
bool AInitialCheck( PAImage SrcImg )
{
	ALog( ALogType::Information,
		  "Checking input file...\n" );

	IMAGE_DOS_HEADER *Dos = reinterpret_cast< IMAGE_DOS_HEADER * >(SrcImg->Base);
	if ( IMAGE_DOS_SIGNATURE != Dos->e_magic ) {
		ALog( ALogType::Fail,
			  "Invalid PE file (DOS)!\n" );

		return false;
	}

	IMAGE_NT_HEADERS *Nt = reinterpret_cast< IMAGE_NT_HEADERS * >(reinterpret_cast< unsigned char * >(SrcImg->Base) + Dos->e_lfanew);
	if ( IMAGE_NT_SIGNATURE != Nt->Signature ) {
		ALog( ALogType::Fail,
			  "Invalid PE file (NT)!\n" );

		return false;
	}

	return true;
}

constexpr unsigned int RICH_HEADER_DANS_ID = 0x908A851F;
constexpr unsigned int RICH_HEADER_RICH_ID = 0x68636952;

enum class RichHeaderModify : unsigned char
{
	Remove,
	Fake
};

bool APackModifyRichHeader( void				*File,
							RichHeaderModify	Modify )
{
	SetLastError( 0 );
	unsigned char *Data = reinterpret_cast< unsigned char * >(VirtualAlloc( nullptr,
																			0x1000,
																			MEM_COMMIT | MEM_RESERVE,
																			PAGE_READWRITE ));
	DWORD Read = {};

	ALog( ALogType::Information,
		  "Reading PE header...\n" );

	OVERLAPPED Ovr = {};
	if ( !ReadFile( File,
					Data,
					0x1000,
					&Read,
					&Ovr ) ) {
		ALog( ALogType::Fail,
			  "Cannot read PE header!\n" );

		VirtualFree( Data,
					 0,
					 MEM_RELEASE );
		return false;
	}

	ALog( ALogType::Information,
		  "Read PE header!\n" );

	int i = {};
	unsigned int StartOff = {};
	unsigned int EndOff = {};

	for ( ; i < 0x1000;
		  ++Data, ++i ) {
		unsigned int Curr = *reinterpret_cast< unsigned int * >(Data);
		if ( RICH_HEADER_DANS_ID == Curr
			 && (!StartOff && !EndOff) ) {
			StartOff = i;
			ALog( ALogType::Information,
				  "Found DanS ID at 0x%04X!\n",
				  StartOff );
		}
		if ( RICH_HEADER_RICH_ID == Curr
			 && (StartOff && !EndOff) ) {
			EndOff = i;
			EndOff += (sizeof( DWORD ) * 2);
			ALog( ALogType::Information,
				  "Found Rich ID at 0x%04X!\n",
				  EndOff );
			break;
		}
	}

	unsigned int Size	= EndOff - StartOff;

	if( RichHeaderModify::Remove == Modify ) {
		ALog( ALogType::Information,
			  "Removing Rich header...\n",
			  EndOff );

		VirtualFree( Data,
					 0,
					 MEM_RELEASE );
		unsigned char *NewData = new unsigned char[Size];
		memset( NewData,
				0,
				Size );

		Ovr.Offset = StartOff;
		bool Res = WriteFile( File,
							  NewData,
							  Size,
							  &Read,
							  &Ovr );

		ALog( ALogType::Information,
			  "Removed!\n",
			  EndOff );
		delete[ ] NewData;
		return Res;
	}

	ALog( ALogType::Information,
		  "Exiting...\n",
		  EndOff );

	VirtualFree( Data,
				 0,
				 MEM_RELEASE );
	return true;
}

template <typename T> static float ShannonEntropy( T data[ ], int elements )
{
	float entropy = 0;
	std::map<T, long> counts;
	typename std::map<T, long>::iterator it;
	for ( int dataIndex = 0; dataIndex < elements; ++dataIndex ) {
		counts[data[dataIndex]]++;
	}
	it = counts.begin();
	while ( it != counts.end() ){
		float p_x = ( float ) it->second / elements;
		if ( p_x > 0 ) entropy -= p_x * log( p_x ) / log( 2 );
		it++;
	}
	return entropy;
}

float APackCalculateEntropy( void *File )
{
	unsigned int Size = GetFileSize( File,
									 nullptr );
	void *Memory = VirtualAlloc( nullptr,
								 Size,
								 MEM_COMMIT | MEM_RESERVE,
								 PAGE_READWRITE );
	OVERLAPPED Ovr = {};
	DWORD Read = {};
	ReadFile( File,
			  Memory,
			  Size,
			  &Read,
			  &Ovr );
	float Entr = ShannonEntropy( reinterpret_cast< unsigned char * >(Memory),
								 Size );
	VirtualFree( Memory,
				 0,
				 MEM_RELEASE );
	return Entr;
}

bool APackEncapsulate( const wchar_t	*OutName,
					   PAImage			SrcImg,
					   PAImage			DestImg,
					   unsigned int		Flags )
{
	if ( !AInitialCheck( SrcImg ) )
		return false;

	ALog( ALogType::Information,
		  "Encapsulating \"%ws\"...\n",
		  SrcImg->Path );

	APackData	PackData	= {};
	ANewImage	NewImage	= {};

	if ( !APackGenerateKey( &PackData ) )
		return false;
	if ( !APackEncrypt( &PackData,
						SrcImg ) )
		return false;
	if ( !APackCalculatePebbles( &PackData,
								 SrcImg ) )
		return false;
	if ( !APackFillCaves( &PackData,
						  SrcImg,
						  DestImg,
						  &NewImage ) )
		return false;

	/*
	*	Store the flags in the pack data structure
	*/
	PackData.Flags |= Flags;

	if ( !APackGenChkSum( &PackData ) )
		return false;
	
	if ( !APackOutput( OutName,
					   &PackData,
					   &NewImage,
					   DestImg ) )
		return false;

	void *File = CreateFileW( OutName,
							  FILE_READ_ACCESS | FILE_WRITE_ACCESS,
							  FILE_SHARE_READ | FILE_SHARE_WRITE,
							  nullptr,
							  OPEN_EXISTING,
							  FILE_ATTRIBUTE_NORMAL,
							  nullptr );

	float Entropy = APackCalculateEntropy( File );

	ALog( ALogType::Information,
		  "File entropy is %.5f!\n",
		  Entropy );

	if ( !APackModifyRichHeader( File,
								 RichHeaderModify::Remove ) )
		return false;

	APackFixPECheckSum( OutName,
						File );

	/*
	*	Close the handle to the Output file
	*/
	CloseHandle( File );

	ALog( ALogType::Success,
		  "Encapsulating \"%ws\" into file \"%ws\" was succesful!\n",
		  SrcImg->Path,
		  OutName );

	return true;
}

#endif