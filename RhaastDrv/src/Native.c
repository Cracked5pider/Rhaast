#include <Rhaast.h>

/**
 * @brief
 *      hashes given data and returns the calculated hash
 *
 * @param String
 *      data pointer to hash
 *
 * @param Length
 *      Length to calculate.
 *      if 0 then it is going to stop at a null terminator
 *
 * @return
 *      calculated hash
 */
ULONG64 RsHasher(
    _In_ PVOID String,
    _In_ ULONG Length
) {
    ULONG  Hash = HASH_KEY;
    PUCHAR Ptr  = String;
    UCHAR  Chr  = 0;

    if ( ! String ) {
        return 0;
    }

    do {
        Chr = *Ptr;

        if ( ! Length ) {
            if ( ! *Ptr ) {
                break;
            }
        } else {
            if ( ( ULONG ) ( U_PTR( Ptr ) - U_PTR( String ) ) >= Length ) {
                break;
            }

            if ( ! *Ptr ) {
                ++Ptr;
            }
        }

        if ( Chr >= 'a' ) {
            Chr -= 0x20;
        }

        Hash = ( ( Hash << 5 ) + Hash ) + Chr;

        ++Ptr;
    } while ( TRUE );

    return Hash;
}

/**
 * @brief
 *      gets ntoskrnl.exe base address by iterating
 *      over the KLDR_DATA_TABLE_ENTRY.InLoadOrderLinks linked list
 *      and checking the BaseDllName hashes with the ntoskrnl.exe hash.
 *
 *      original code taken from this unknowncheats thread + minor fix based on responses.
 *      https://www.unknowncheats.me/forum/general-programming-and-reversing/427419-getkernelbase.html
 *
 * @return
 *      returns the base address of ntoskrnl.exe if found. 
 */
PVOID RsKernelBase(
    VOID
) {
    PKLDR_DATA_TABLE_ENTRY KLdrEntry = NULL;
    PKLDR_DATA_TABLE_ENTRY KLdrFirst = NULL;

    if ( ! Instance.DriverObject ) {
        return NULL;
    }

    KLdrEntry = KLdrFirst = Instance.DriverObject->DriverSection;
        
    while ( C_PTR( KLdrEntry->InLoadOrderLinks.Flink ) != KLdrFirst )
    {
        if ( RsHasher( KLdrEntry->BaseDllName.Buffer, KLdrEntry->BaseDllName.Length ) == H_MODULE_NTOSKRNL ) {
            return KLdrEntry->DllBase;
        }

        KLdrEntry = C_PTR( KLdrEntry->InLoadOrderLinks.Flink );
    }

    return 0;
}

/**
 * @brief
 *      resolves a function address from ntoskrnl.exe
 *
 * @param Hash
 *      Hashed function name to resolve
 *
 * @return
 *      Function address if managed to resolve it.
 *      NULL is being returned if failed to resolve or
 *      find function by hash
 */
PVOID RsLdrFunction(
    _In_ ULONG Hash
) {
    PIMAGE_NT_HEADERS       NtHeader  = { 0 };
    PIMAGE_EXPORT_DIRECTORY ExportDir = { 0 };
    PDWORD32                AddrNames = { 0 };
    PDWORD32                AddrFuncs = { 0 };
    PWORD                   AddrOrdls = { 0 };
    PVOID                   Kernel    = { 0 };
    
    /* get kernel address */
    if ( ! ( Kernel = Instance.NtKrnlOs ) ) {
        return NULL; 
    }

    /* parse header */
    NtHeader  = C_PTR( U_PTR( Kernel ) + ( ( PIMAGE_DOS_HEADER ) Kernel )->e_lfanew );
    ExportDir = C_PTR( U_PTR( Kernel ) + NtHeader->OptionalHeader.DataDirectory[ 0 ].VirtualAddress );

    /* get export directory function & name addresses */
    AddrNames = C_PTR( U_PTR( Kernel ) + ExportDir->AddressOfNames        );
    AddrFuncs = C_PTR( U_PTR( Kernel ) + ExportDir->AddressOfFunctions    );
    AddrOrdls = C_PTR( U_PTR( Kernel ) + ExportDir->AddressOfNameOrdinals );
    
    for ( DWORD i = 0; i < ExportDir->NumberOfNames; i++ ) 
    {
        /* check if exported function name it the name we are searching for */
        if ( RsHasher( C_PTR( U_PTR( Kernel ) + AddrNames[ i ] ), 0 ) == Hash ) {
            return C_PTR( U_PTR( Kernel ) + AddrFuncs[ AddrOrdls[ i ] ] ); 
        }
    }

    return NULL; 
}

/**
 * @brief
 *      search for routine end and return offset of end of function.
 *
 *      NOTE:
 *      it does this by searching for the "ret" instruction (0xC3).
 *      if it wont find this function then this function might go for infinity
 *      (unlikly lol. gonna stop at a random function ret or you get an
 *      exception trying to read non readable memory)
 *
 * @param Routine
 *      Routine pointer 
 *
 * @return
 *      Routine end offset 
 */
ULONG_PTR RsUtilRoutineEnd(
    _In_ PVOID Routine
) {
    ULONG_PTR Offset = 0;

    /* search for the "ret" instruction */
    for ( ;; Offset++ ) {
        if ( DREF_U8( U_PTR( Routine ) + Offset ) == ASM_RET ) {
            break; 
        }
    }

    return Offset;

}