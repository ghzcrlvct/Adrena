#include "img.h"

NOGLOBAL
B
HRTAPI
HrtImpMapToMem(
	READONLY PUC		Path,
	PHRT_PE_POINTERS	Ptr )
{
	REGISTER B Status = BAD;

	if( !STRING_VALID( Path ) || !Ptr )
		goto ErrorOccured3;

	HRT_PTR_DEC_SELF( Ptr );

	/*
	*	Open a handle to the file
	*/
	VV File = MyCreateFileW( Path,
							 GENERIC_READ,
							 FILE_SHARE_READ,
							 OPEN_EXISTING );
	if( !File || INVALID_HANDLE_VALUE == File )
		goto ErrorOccured3;

	/*
	*	Get the size of the file
	*/
	VV Size = MyGetFileSize( File );
	if( !Size )
		goto ErrorOccured2;

	/*
	*	Allocate memory for the file
	*/
	VV Mem = MyVirtualAlloc( NULL,
							 Size,
							 MEM_COMMIT | MEM_RESERVE,
							 PAGE_EXECUTE_READWRITE ); // !!! Remove execution protection
	if( !Mem )
		goto ErrorOccured2;

	/*
	*	Read the file into memory
	*/
	if ( !MyReadFile( File,
					  Mem,
					  Size ) )
		goto ErrorOccured;

	/*
	*	Store pointers to elements of the file's PE header
	*/
	HRT_STORE_PE_POINTERS( Ptr,
						   Mem );

	/*
	*	Verify the file's PE signatures
	*/
	if( IMAGE_DOS_SIGNATURE != Ptr->Dos->e_magic ||
		IMAGE_NT_SIGNATURE != Ptr->Nt->Signature ) {
		goto ErrorOccured;
	}

	/*
	*	Verify the architecture of the file
	*/
#ifdef HEART64
	if( IMAGE_FILE_MACHINE_AMD64 != Ptr->File->Machine ) {
		goto ErrorOccured;
	}
#else
	if( IMAGE_FILE_MACHINE_I386 != Ptr->File->Machine ) {
		goto ErrorOccured;
	}
#endif

	Status = GOOD;
	/*
	*	Skip the memory allocation freeing
	*/
	goto ErrorOccured2;

ErrorOccured:

	/*
	*	Free the allocated memory
	*/
	MyVirtualFree( Mem,
				   0,
				   MEM_RELEASE );
	
ErrorOccured2:

	/*
	*	Close the handle to the file
	*/
	MyCloseHandle( File );
	
ErrorOccured3:

	return Status;
}

NOGLOBAL
B
HRTAPI
HrtImgUnmapFromMem(
	PHRT_PE_POINTERS Ptr )
{
	REGISTER B Status = BAD;

	if( !Ptr )
		goto Cleanup;

	HRT_PTR_DEC_SELF( Ptr );

	/*
	*	Free the memory allocated for the image
	*/
	if ( !MyVirtualFree( Ptr->Dos,
						 0,
						 MEM_RELEASE ) );
		goto Cleanup;

	Status = GOOD;

Cleanup:

	return Status;
}

NOGLOBAL
B
HRTAPI
HrtImgRemapToMem(
	PHRT_PE_POINTERS	Ptr,
	PHRT_PE_POINTERS	NewPtr,
	READONLY PB			ReqBaseReloc )
{
	REGISTER B Status = BAD;

	*ReqBaseReloc = NO;

	if( !Ptr || !ReqBaseReloc )
		goto Cleanup;

	HRT_PTR_DEC_SELF( Ptr );
	HRT_PTR_DEC_SELF( NewPtr );

	/*
	*	Try to allocate memory at the prefered image base
	*/
	VV NewMem = MyVirtualAlloc( Ptr->Opt->ImageBase,
								Ptr->Opt->SizeOfImage,
								MEM_COMMIT | MEM_RESERVE,
								PAGE_EXECUTE_READWRITE ); // !!! Remove execution protection
	if( !NewMem ) {
		/*
		*	If memory could not be allocated at the prfered image base,
		*	allocate memory at any address
		*/
		NewMem = MyVirtualAlloc( NULL,
								 Ptr->Opt->SizeOfImage,
								 MEM_COMMIT | MEM_RESERVE,
								 PAGE_EXECUTE_READWRITE ); // !!! Remove execution protection
		if( !NewMem )
			goto Cleanup;
		else
			*ReqBaseReloc = YES;
	}

	/*
	*	Store pointers to elements of the new PE header
	*/
	HRT_STORE_PE_POINTERS( NewPtr,
						   NewMem );

	Status = GOOD;

Cleanup:
	
	return Status;
}

NOGLOBAL
B
HRTAPI
HrtImgCopyHdr(
	PHRT_PE_POINTERS	SrcPtr,
	PHRT_PE_POINTERS	DestPtr )
{
	REGISTER B Status = BAD;
	 
	if ( !SrcPtr || !DestPtr )
		goto Cleanup;

	HRT_PTR_DEC_SELF( SrcPtr );
	HRT_PTR_DEC_SELF( DestPtr );

	/*
	*	Copy the PE header from the source image to the destination image
	*/
	HrtMemCpy8( SrcPtr->Dos,
				DestPtr->Dos,
				SrcPtr->Opt->SizeOfHeaders );
		
	/*
	*	Fix the PE header pointers of the destination image
	*/
	HRT_FIX_PE_POINTERS( DestPtr,
						 ( V ) (DestPtr->Dos) );

	Status = GOOD;

Cleanup:

	return Status;
}

NOGLOBAL
B
HRTAPI
HrtImgCopySect(
	PHRT_PE_POINTERS	SrcPtr,
	PHRT_PE_POINTERS	DestPtr )
{
	REGISTER B Status = BAD;

	if( !SrcPtr || !DestPtr )
		goto ErrorOccured;

	HRT_PTR_DEC_SELF( SrcPtr );
	HRT_PTR_DEC_SELF( DestPtr );

	/*
	*	Get the first section of the source image
	*/
	REGISTER PIMAGE_SECTION_HEADER CurrSect = IMAGE_FIRST_SECTION( SrcPtr->Nt );
	if( !CurrSect || !SrcPtr->File->NumberOfSections )
		goto ErrorOccured;

	/*
	*	Go trough each one of the sections
	*/
	for( VV i = SrcPtr->File->NumberOfSections; i != 0; --i, ++CurrSect ) {
		/*
		*	Calculate the source and destination base address and get the section size
		*/
		VV SrcBase = ( V ) (SrcPtr->Dos) + CurrSect->PointerToRawData;
		VV DestBase = ( V ) (DestPtr->Dos) + CurrSect->VirtualAddress;
		VV SectSize = CurrSect->SizeOfRawData;

		/*
		*	If the current section contains any raw data
		*/
		if ( CurrSect->SizeOfRawData ) {
			/*
			*	Copy the current section from the source image to the destination image
			*/
			HrtMemCpy8( SrcBase,
						DestBase,
						SectSize );
		}
	}

	Status = GOOD;

ErrorOccured:
	
	return Status;
}

NOGLOBAL
B
HRTAPI
HrtImgFixProt(
	PHRT_PE_POINTERS Ptr )
{
	REGISTER B Status = BAD;
	
	if( !Ptr )
		goto ErrorOccured;

	HRT_PTR_DEC_SELF( Ptr );

	/*
	*	Get the first section of the source image
	*/
	REGISTER PIMAGE_SECTION_HEADER CurrSect = IMAGE_FIRST_SECTION( Ptr->Nt );
	if( !CurrSect || !Ptr->File->NumberOfSections )
		goto ErrorOccured;

	V OldProt = 0;
	
	/*
	*	Make the PE header read-only
	*/
	MyVirtualProtect( Ptr->Dos,
					  Ptr->Opt->SizeOfHeaders,
					  PAGE_READONLY,
					  &OldProt );
	
	/*
	*	Go trough each one of the sections
	*/
	for( VV i = Ptr->File->NumberOfSections; i != 0; --i, ++CurrSect ) {
		HRT_DBG_TRACE( "%s: ", CurrSect->Name );

		VV SectBase = ( V )( Ptr->Dos ) + CurrSect->VirtualAddress;
		VV SectSize = CurrSect->Misc.VirtualSize;

		/*
		*	If the current section is discardable
		*/
		if( IMAGE_SCN_MEM_DISCARDABLE == CurrSect->Characteristics ) {
			/*
			*	Decommit the memory of the current section
			*/
			MyVirtualFree( SectBase,
						   SectSize,
						   MEM_DECOMMIT );
			continue;
		}

		VV NewProt = 0;

		/*
		*	Read the current section's characteristics and build a new protection mask
		*/
		if( IMAGE_SCN_MEM_READ & CurrSect->Characteristics ) {
			NewProt |= PAGE_READONLY;
			HRT_DBG_SINGLE_TRACE( "Readable " );
		}

		if( IMAGE_SCN_MEM_WRITE & CurrSect->Characteristics ) {
			NewProt |= PAGE_WRITECOPY;
			HRT_DBG_SINGLE_TRACE( "Writable " );
		}

		if( IMAGE_SCN_MEM_EXECUTE & CurrSect->Characteristics ) {
			NewProt |= PAGE_EXECUTE;
			HRT_DBG_SINGLE_TRACE( "Executable " );
		}

		if( IMAGE_SCN_MEM_NOT_CACHED & CurrSect->Characteristics ) {
			NewProt |= PAGE_NOCACHE;
			HRT_DBG_SINGLE_TRACE( "No Cache " );
		}

		HRT_DBG_SINGLE_TRACE( "\n" );

		/*
		*	Protect the current section
		*/
		MyVirtualProtect( SectBase,
						  SectSize,
						  NewProt,
						  &OldProt );
	}

	Status = GOOD;

ErrorOccured:
	
	return Status;
}

NOGLOBAL
B
HRTAPI
HrtImgFixImp(
	PHRT_PE_POINTERS	Ptr,
	READONLY V			Type )
{
	REGISTER B Status = BAD;

	if( !Ptr )
		goto ErrorOccured;

	HRT_PTR_DEC_SELF( Ptr );

	REGISTER PIMAGE_DATA_DIRECTORY ImpDir = NULL;

	/*
	*	Get the import table based on the import type given
	*/
	switch( Type )
	{
		case(HRT_IMG_IMPTYPE_DEL):
			ImpDir = Ptr->DelImp;
			break;

		default:
			ImpDir = Ptr->Imp;
			break;
	}

	/*
	*	If the import directory is invalid
	*/
	if( !ImpDir || !ImpDir->Size )
		goto ErrorOccured;

	VV BaseAddr = ( V )( Ptr->Dos );

	REGISTER PIMAGE_IMPORT_DESCRIPTOR ImpDescr = ( PIMAGE_IMPORT_DESCRIPTOR )( BaseAddr + ImpDir->VirtualAddress );
	if( !ImpDescr )
		goto ErrorOccured;

	/*
	*	While the import descriptor name or time/date stamp are valid
	*/
	while( ImpDescr->Name || ImpDescr->TimeDateStamp ) {
		/*
		*	Get the name of the module exporting the functions that need to be imported
		*/
		REGISTER PC ModName = ( PC ) (BaseAddr + ImpDescr->Name);
		if( !STRING_VALID( ModName ) )
			goto ErrorOccured;

		/*
		*	Ensure the module is loaded into the process
		*/
		HRT_DBG_TRACE( "Ensuring %s is loaded...\n",
					   ModName );
		VV ModBase = MyLoadLibraryA( ModName );
		if( !ModBase )
			goto ErrorOccured;

		REGISTER PIMAGE_THUNK_DATA	OrgThnk		= ( PIMAGE_THUNK_DATA )( BaseAddr + ImpDescr->OriginalFirstThunk );
		REGISTER PIMAGE_THUNK_DATA	FrThnk		= ( PIMAGE_THUNK_DATA )( BaseAddr + ImpDescr->FirstThunk );
		REGISTER PIMAGE_THUNK_DATA	ActThnk		= NULL;

		if( !OrgThnk )
			ActThnk = FrThnk;
		else
			ActThnk = OrgThnk;

		while( ActThnk->u1.AddressOfData ) {
			REGISTER PC FncName = NULL;

			/*
			*	If the function name is an ordinal
			*/
			if( IMAGE_ORDINAL_FLAG & ActThnk->u1.Ordinal )
				FncName = ( PC )( ActThnk->u1.Ordinal & 0xFFFF );
			/*
			*	If the function name is a string
			*/
			else {
				REGISTER PIMAGE_IMPORT_BY_NAME ImpName = ( PIMAGE_IMPORT_BY_NAME )( BaseAddr + ActThnk->u1.AddressOfData );
				FncName = ImpName->Name;
			}

			/*
			*	Get and store the address of the current function
			*/
			FrThnk->u1.Function = HrtModGetFunc( ModBase,
												 FncName );
			//FrThnk->u1.Function = GetProcAddress( ModBase, FncName );
			if( !FrThnk->u1.Function )
				goto ErrorOccured;

			ActThnk++;
			FrThnk++;
		}

		++ImpDescr;
	}

	Status = GOOD;

ErrorOccured:

	return Status;
}

typedef struct _HRT_BASERELOC_ENTRY {
	V16 Offset	: 12;
	V16 Type	: 4;
} HRT_BASERELOC_ENTRY, *PHRT_BASERELOC_ENTRY;

NOGLOBAL
B
HRTAPI
HrtImgRelocBase(
	PHRT_PE_POINTERS	Ptr,
	V					OldBase )
{
	B Status = BAD;

	if( !Ptr )
		goto ErrorOccured;

	HRT_PTR_DEC_SELF( Ptr );

	if( !Ptr->BaseReloc || !Ptr->BaseReloc->Size || !Ptr->BaseReloc->VirtualAddress )
		goto ErrorOccured;
	
	VV						BaseAddr	= ( V )( Ptr->Dos );
	VV						MaxSize		= Ptr->BaseReloc->Size;
	VV						RelocAddr	= Ptr->BaseReloc->VirtualAddress;
	PIMAGE_BASE_RELOCATION	Reloc		= 0;
	VV						Parsed		= 0;

	for ( ; Parsed < MaxSize; Parsed += Reloc->SizeOfBlock ) {
		Reloc = ( PIMAGE_BASE_RELOCATION ) (RelocAddr + Parsed + BaseAddr);
		if ( !Reloc || !Reloc->VirtualAddress || !Reloc->SizeOfBlock )
			break;

		VV EntryCnt		= (Reloc->SizeOfBlock - sizeof( IMAGE_BASE_RELOCATION )) / sizeof( HRT_BASERELOC_ENTRY );
		VV Page			= Reloc->VirtualAddress;

		REGISTER PHRT_BASERELOC_ENTRY CurrEntr = ( PHRT_BASERELOC_ENTRY ) (( V ) Reloc +sizeof( IMAGE_BASE_RELOCATION ));

		for ( VV i = 0; i < EntryCnt; i++ ) {
			VV Off = CurrEntr->Offset;
			VV Typ = CurrEntr->Type;
			VV RelocFld = Page + Off;

			if ( !CurrEntr || !Typ )
				break;
			if ( 3 != Typ || RelocFld >= Ptr->Opt->SizeOfImage )
				goto ErrorOccured;

			PVV RelocAddr = ( PV ) (BaseAddr + RelocFld);
			HRT_DBG_TRACE( "Patched for relocation at %p\n",
						   RelocAddr );
			*RelocAddr = ((*RelocAddr) - OldBase + BaseAddr);
			CurrEntr = ( PHRT_BASERELOC_ENTRY ) (( V ) ( CurrEntr ) +sizeof( HRT_BASERELOC_ENTRY ));
		}
	}

	Status = (0 != Parsed);

ErrorOccured:
	
	return Status;
}

NOGLOBAL
B
HRTAPI
HrtImgCallTLS(
	PHRT_PE_POINTERS Ptr )
{
	if ( !Ptr )
		return BAD;

	HRT_PTR_DEC_SELF( Ptr );

	if ( !Ptr->TLS || !Ptr->TLS->Size || !Ptr->TLS->VirtualAddress )
		return BAD;
	
	VV BaseAddr = ( V ) (Ptr->Dos);

	/*
	*	Get a list of all TLS callbacks
	*/
	REGISTER PIMAGE_TLS_DIRECTORY TLSDir	= ( PIMAGE_TLS_DIRECTORY ) (BaseAddr + Ptr->TLS->VirtualAddress);
	REGISTER PIMAGE_TLS_CALLBACK *TLSCbk	= ( PIMAGE_TLS_CALLBACK ) (TLSDir->AddressOfCallBacks);
	while( TLSCbk && *TLSCbk ) {
		/*
		*	Call the current TLS callback
		*/
		(*TLSCbk)( BaseAddr,
				   DLL_PROCESS_ATTACH,
				   NULL );

		++TLSCbk;
	}

	return GOOD;
}

NOGLOBAL
B
HRTAPI
HrtImgCallEP(
	PHRT_PE_POINTERS Ptr )
{
	if ( !Ptr )
		return BAD;

	HRT_PTR_DEC_SELF( Ptr );

	VV BaseAddr = ( V ) (Ptr->Dos);

	/*
	*	If the offset of the entry point is valid
	*/
	if ( Ptr->Opt->AddressOfEntryPoint ) {
		/*
		*	Calculate the address of the entry point
		*/
		f_DllMain EntryPoint = ( f_DllMain ) (BaseAddr + Ptr->Opt->AddressOfEntryPoint);

		/*
		*	Call the entry point
		*/
		EntryPoint( BaseAddr,
					DLL_PROCESS_ATTACH,
					NULL );
	}

	return GOOD;
}

V
HRTAPI
HrtImgLoad(
	READONLY PUC	Path,
	READONLY V		Type,
	READONLY V		Flags )
{
	VV Base = 0;

	if ( !STRING_VALID( Path ) )
		goto ErrorOccured;

	HRT_DBG_TRACE( "Path: %ws\n",
				   Path );

	/*
	*	Map the file into memory
	*/
	HRT_PE_POINTERS PEPointers;
	HrtMemZero( &PEPointers,
				SIZEOF( PEPointers ) );
	PHRT_PE_POINTERS PEPointersEnc = HRT_PTR_ENC( &PEPointers );

	if ( !HrtImpMapToMem( Path,
						  PEPointersEnc ) )
		goto ErrorOccured;

	HRT_DBG_TRACE( "Mapped: 0x%08X\n",
				   PEPointers.Dos );

	VV PrefBase		= PEPointers.Opt->ImageBase;
	B ReqBaseReloc	= NO;

	/*
	*	Remap the file into memory
	*/
	HRT_PE_POINTERS NewPEPointers;
	HrtMemZero( &NewPEPointers,
				SIZEOF( NewPEPointers ) );
	PHRT_PE_POINTERS NewPEPointersEnc = HRT_PTR_ENC( &NewPEPointers );

	if ( !HrtImgRemapToMem( PEPointersEnc,
							NewPEPointersEnc,
							&ReqBaseReloc ) )
		goto ErrorOccured;

	HRT_DBG_TRACE( "Remapped: 0x%08X (Requires Base Relocation: %d)\n",
				   NewPEPointers.Dos,
				   ReqBaseReloc );

	/*
	*	Copy the PE header
	*/
	if ( !HrtImgCopyHdr( PEPointersEnc,
						 NewPEPointersEnc ) )
		goto ErrorOccured;

	HRT_DBG_TRACE( "Copied PE...\n" );

	/*
	*	Copy the sections
	*/
	if ( !HrtImgCopySect( PEPointersEnc,
						  NewPEPointersEnc ) )
		goto ErrorOccured;

	HRT_DBG_TRACE( "Copied Sections...\n" );

	/*
	*	Fix the default imports
	*/
	if ( !HrtImgFixImp( NewPEPointersEnc,
						HRT_IMG_IMPTYPE_DEF ) )
		goto ErrorOccured;

	HRT_DBG_TRACE( "Fixed Imports...\n" );

	/*
	*	Fix the delay imports
	*/
	if ( !HrtImgFixImp( NewPEPointersEnc,
						HRT_IMG_IMPTYPE_DEL ) ) {
		if ( !FLAG_SET( Flags,
						HRT_IMG_FLAG_NOREQDELIMP ) )
			goto ErrorOccured;

		HRT_DBG_TRACE( "Cannot fix delay imports...\n" );
	}

	HRT_DBG_TRACE( "Fixed delay Imports...\n" );

	/*
	*	If base relocation is required
	*/
	if ( YES == ReqBaseReloc ) {
		/*
		*	Relocate the image base
		*/
		if ( !HrtImgRelocBase( NewPEPointersEnc,
							   PrefBase ) )
			goto ErrorOccured;

		HRT_DBG_TRACE( "Relocated Base...\n" );
	}

	/*
	*	Fix the image PE header page protections
	*/
	if ( !HrtImgFixProt( NewPEPointersEnc ) )
		goto ErrorOccured;

	HRT_DBG_TRACE( "Fixed PE Header Protection...\n" );

	/*
	*	Call the TLS callbacks
	*/
	if ( !HrtImgCallTLS( NewPEPointersEnc ) ) {
		if ( !FLAG_SET( Flags,
						HRT_IMG_FLAG_NOREQTLS ) )
			goto ErrorOccured;
		HRT_DBG_TRACE( "Cannot call TLS callbacks!\n" );
	}
	else {
		HRT_DBG_TRACE( "Called TLS callbacks...\n" );
	}

	/*
	*	Call the entry point
	*/
	if ( Type == HRT_IMG_TYPE_DLL ) {
		if ( !HrtImgCallEP( NewPEPointersEnc ) ) {
			if ( !FLAG_SET( Flags,
							HRT_IMG_FLAG_NOREQEP ) )
				goto ErrorOccured;
			HRT_DBG_TRACE( "Cannot call DLL entry point!\n" );
		} else {
			HRT_DBG_TRACE( "Called DLL entry point...\n" );
		}
	} else {
		VV ExeEp = ( V ) (NewPEPointers.Dos) + NewPEPointers.Opt->AddressOfEntryPoint;
		f_Start StartFunc = ( f_Start ) (ExeEp);
		StartFunc();
		HRT_DBG_TRACE( "Called EXE entry point...\n" );
	}

	Base = ( V ) (NewPEPointers.Dos);

ErrorOccured:

	HRT_DBG_TRACE( "Returning!\n" );

	return Base;
}

B
HRTAPI
HrtImgTryRunFromMem(
	READONLY V Base )
{
	REGISTER B Status = BAD;

	if ( !Base )
		goto ErrorOccured;

	HRT_PE_POINTERS PEPointers;
	HrtMemZero( &PEPointers,
				SIZEOF( PEPointers ) );
	PHRT_PE_POINTERS PEPointersEnc = &PEPointers;
	HRT_STORE_PE_POINTERS( PEPointersEnc, Base );
	HRT_PTR_ENC_SELF( PEPointersEnc );

	VV PrefBase = PEPointers.Opt->ImageBase;
	B ReqBaseReloc = NO;

	/*
	*	Remap the file into memory
	*/
	HRT_PE_POINTERS NewPEPointers;
	HrtMemZero( &NewPEPointers,
				SIZEOF( NewPEPointers ) );
	PHRT_PE_POINTERS NewPEPointersEnc = HRT_PTR_ENC( &NewPEPointers );

	if ( !HrtImgRemapToMem( PEPointersEnc,
							NewPEPointersEnc,
							&ReqBaseReloc ) )
		goto ErrorOccured;

	HRT_DBG_TRACE( "Remapped: 0x%08X (Requires Base Relocation: %d)\n",
				   NewPEPointers.Dos,
				   ReqBaseReloc );

	/*
	*	Copy the PE header
	*/
	if ( !HrtImgCopyHdr( PEPointersEnc,
						 NewPEPointersEnc ) )
		goto ErrorOccured;

	HRT_DBG_TRACE( "Copied PE...\n" );

	/*
	*	Copy the sections
	*/
	if ( !HrtImgCopySect( PEPointersEnc,
						  NewPEPointersEnc ) )
		goto ErrorOccured;

	HRT_DBG_TRACE( "Copied Sections...\n" );

	/*
	*	Fix the default imports
	*/
	if ( !HrtImgFixImp( NewPEPointersEnc,
						HRT_IMG_IMPTYPE_DEF ) )
		goto ErrorOccured;

	HRT_DBG_TRACE( "Fixed Imports...\n" );

	/*
	*	Fix the delay imports
	*/
	if ( !HrtImgFixImp( NewPEPointersEnc,
						HRT_IMG_IMPTYPE_DEL ) ) {
		HRT_DBG_TRACE( "Cannot fix delay imports...\n" );
	} else {
		HRT_DBG_TRACE( "Fixed delay Imports...\n" );
	}

	/*
	*	If base relocation is required
	*/
	if ( YES == ReqBaseReloc ) {
		/*
		*	Relocate the image base
		*/
		if ( !HrtImgRelocBase( NewPEPointersEnc,
							   PrefBase ) ) {
			HRT_DBG_TRACE( "Cannot relocate base!\n" );
			goto ErrorOccured;
		}

		HRT_DBG_TRACE( "Relocated Base...\n" );
	}

	/*
	*	Fix the image PE header page protections
	*/
	if ( !HrtImgFixProt( NewPEPointersEnc ) )
		goto ErrorOccured;

	HRT_DBG_TRACE( "Fixed PE Header Protection...\n" );

	/*
	*	Call the TLS callbacks
	*/
	if ( !HrtImgCallTLS( NewPEPointersEnc ) ){
		HRT_DBG_TRACE( "Cannot call TLS callbacks!\n" );
	} else {
		HRT_DBG_TRACE( "Called TLS callbacks...\n" );
	}
	/*
	*	Call the entry point
	*/
	VV ExeEp = ( V ) (NewPEPointers.Dos) + NewPEPointers.Opt->AddressOfEntryPoint;
	((VOID( * )())ExeEp)();
	HRT_DBG_TRACE( "Called EXE entry point...\n" );

	Status = GOOD;

ErrorOccured:

	return Status;
}