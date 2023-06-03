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

/*!
 * @brief
 *      check if the specified pid exists
 *
 * @param Pid
 *      Process Id to check
 *
 * @param Name
 *
 *
 * @return
 *      if specified process id exists
 */
BOOL ProcessCheckById(
    _In_      ULONG   Pid,
    _Out_opt_ PBUFFER Name
) {
    PROCESSENTRY32 Pe32Entry = { sizeof( PROCESSENTRY32 ) };
    HANDLE         Snapshot  = NULL;
    BOOL           Result    = FALSE;
    BOOL           Found     = FALSE;
    HANDLE         Heap      = NULL;

    /* create a snapshot */
    if ( ( Snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 ) ) == INVALID_HANDLE_VALUE ) {
        spdlog::error( "CreateToolhelp32Snapshot Failed: {}", GetLastError() );
        goto CLEANUP;
    }

    Result = Process32First( Snapshot, &Pe32Entry );
    Heap   = GetProcessHeap();

    do
    {
        /* check if is our process id */
        if ( Pe32Entry.th32ProcessID == Pid )
        {
            Found = TRUE;

            /* get process name */
            if ( Name )
            {
                /* allocate memory for process name */
                Name->Length = MAX_PATH;
                if ( ! ( Name->Buffer = HeapAlloc( Heap, HEAP_ZERO_MEMORY, Name->Length ) ) ) {
                    break;
                }

                /* copy name to allocated buffer */
                memcpy( Name->Buffer, Pe32Entry.szExeFile, MAX_PATH );
            }

            break;
        }

        Result = Process32Next( Snapshot, &Pe32Entry );
    } while ( Result );

CLEANUP:
    if ( Snapshot ) {
        CloseHandle( Snapshot );
    }

    return Found;
}

