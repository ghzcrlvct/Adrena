/*
*
*	HEART 0.1
*
*	Image.hpp
*
*/

#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include "Definitions.hpp"

class AImage {
public:

	AImage( const wchar_t *Path ) : Success{ }, Base{ }, Path{ }, Size{ }
	{
		wcscpy_s( this->Path, Path );
		this->Success = this->LoadIntoMemory( Path );
	}

	~AImage( void )
	{
		this->UnloadFromMemory();
	}
	
	bool LoadIntoMemory( const wchar_t *Path )
	{
		if ( !Path || '\0' == Path[0] )
			return false;

		void * File = CreateFileW( Path,
								   FILE_READ_ACCESS | FILE_WRITE_ACCESS,
								   FILE_SHARE_READ | FILE_SHARE_WRITE,
								   nullptr,
								   OPEN_EXISTING,
								   FILE_ATTRIBUTE_NORMAL,
								   nullptr );
		if ( INVALID_HANDLE_VALUE == File ) {
			ALog( ALogType::Fail,
				  "Cannot open file \"%ws\"!\n",
				  Path );

			return false;
		}

		ALog( ALogType::Information,
			  "Opened file \"%ws\"!\n",
			  Path );

		unsigned int Size = GetFileSize( File,
										 nullptr );
		if ( !Size ) {
			ALog( ALogType::Fail,
				  "Cannot get size of file \"%ws\"!\n",
				  Path );
			CloseHandle( File );
			return false;
		}

		ALog( ALogType::Information,
			  "Size of file \"%ws\" is %dkb\n",
			  Path,
			  Size / 1000 );

		this->Size = Size;

		void * Mem = VirtualAlloc( nullptr,
								   Size,
								   MEM_COMMIT | MEM_RESERVE,
								   PAGE_READWRITE );
		if ( !Mem ) {
			ALog( ALogType::Fail,
				  "Cannot allocate memory for \"%ws\"!\n",
				  Path );
			CloseHandle( File );
			return false;
		}

		ALog( ALogType::Information,
			  "Allocated memory for \"%ws\"!\n",
			  Path );

		DWORD Read = {};

		if ( !ReadFile( File,
						Mem,
						Size,
						&Read,
						nullptr ) ) {
			ALog( ALogType::Fail,
				  "Cannot read file \"%ws\"!\n",
				  Path );
			CloseHandle( File );
			VirtualFree( Mem,
						 0,
						 MEM_RELEASE );
			return false;
		}
		ALog( ALogType::Information,
			  "Read \"%ws\" into memory!\n",
			  Path );

		CloseHandle( File );
		this->Base = Mem;

		return true;
	}

	bool UnloadFromMemory( void )
	{
		if ( !this->Base )
			return false;

		if ( !VirtualFree( this->Base,
						   0,
						   MEM_RELEASE ) )
			return false;

		return true;
	}

	bool			Success;
	wchar_t			Path[MAX_PATH];
	void *			Base;
	unsigned int	Size;
};
using PAImage = AImage *;

#endif