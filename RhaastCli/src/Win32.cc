#include <Rhaast.h>


/*!
 * @brief
 *      query process name based on process id
 *
 * @param Pid
 *      process id to query name
 *
 * @param Name
 *      name buffer to store process name
 *
 * @return if function ran successfully
 */
BOOL ProcessQueryNameById(
    IN  ULONG   Pid,
    OUT PBUFFER Name
) {
    HANDLE Process = NULL;
    BOOL   Success = FALSE;
    HANDLE Heap    = NULL;

    /* check if buffer has been specified */
    if ( ! Name ) {
        return false;
    }

    /* get heap handle */
    Heap = GetProcessHeap();

    /* set max path size */
    Name->Length = MAX_PATH;

    /* try to open a handle to the process */
    if ( ! ( Process = OpenProcess( PROCESS_QUERY_INFORMATION, false, Pid ) ) ) {
        spdlog::error( "OpenProcess Failed: {}", GetLastError() );
        goto END;
    }

    /* allocate memory for process name */
    if ( ! ( Name->Buffer = HeapAlloc( Heap, HEAP_ZERO_MEMORY, Name->Length ) ) ) {
        goto END;
    }

    /* get size of process name */
    if ( ! ( Name->Length = GetProcessImageFileNameA( Process, ( LPSTR ) Name->Buffer, Name->Length ) ) ) {
        spdlog::error( "GetProcessImageFileNameA Failed: {}", GetLastError() );
        goto END;
    }

    /* end of function.
     * looks like we were succeeding so far */
    Success = TRUE;

END:
    if ( Process ) {
        CloseHandle( Process );
        Process = NULL;
    }

    /* if we still failed let's free up memory */
    if ( ( ! Success ) && ( Name->Buffer ) ) {
        RtlSecureZeroMemory( Name->Buffer, Name->Length );
        HeapFree( Heap, 0, Name->Buffer );
        Name->Buffer = nullptr;
    }

    return Success;
}

