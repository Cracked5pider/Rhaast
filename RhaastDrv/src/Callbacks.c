#include <Rhaast.h>

NTSTATUS RspCallbackSize(
    _In_  RS_CALLBACK_TYPE Type,
    _Out_ PSIZE_T         Size  
);

PVOID RspCallbackArray(
    _In_ RS_CALLBACK_TYPE Type
);

/**
 * @brief
 *      query list of registered callback
 *      from the specified callback type
 *
 * @param Type
 *      callback to query.
 *      if NoneCallback has been specified the function
 *      is going to attempt to query every
 *      supporting callback type 
 *
 * @param QueryData
 *      pointer to allocated memory where
 *      the queried data is going to be written to.
 *
 * @param Size 
 *      size of allocated memory in QueryData and how much can be written to it.
 *      if 0 then it is going to attempt to calculate the callback list size
 *      of the given callback type and how much should be allocated for QueryData. 
 *
 * @return
 *      status of function 
 */
NTSTATUS RsCallbackQuery(
    _In_       RS_CALLBACK_TYPE   Type,
    _Out_opt_  PRS_CALLBACK_DATA* QueryData,
    _Out_      PSIZE_T            Size
) {
    NTSTATUS NtStatus  = STATUS_UNSUCCESSFUL;
    SIZE_T   QuerySize = 0;
    PVOID    ArrayAddr = NULL;


    if ( Type == PsCreationCallback ) {

        /* get array address */
        ArrayAddr = RspCallbackArray( Type );

        PRINTF( "ArrayAddr: %p\n", ArrayAddr );
        
    }

END:

    return NtStatus; 
}

/**
 * @brief
 *      retrieve the callback notify array based
 *      on the specified type.
 *
 * @param Type
 *      callback type to get array address from
 *
 * @return
 *      pointer to callback notify array
 */
PVOID RspCallbackArray(
    _In_ RS_CALLBACK_TYPE Type
) {
    PVOID Address = NULL;
    ULONG Offset  = 0;

    /* this function doesn't support this type */
    if ( Type == NoneCallback ) {
        return NULL;
    }

    if ( Type == PsCreationCallback ) {

        /* try to resolve PsSetCreateProcessNotifyRoutine routine */
        if ( ! ( Address = RsLdrFunction( H_API_PSSETCREATEPROCESSNOTIFYROUTINE ) ) ) {
            return NULL;
        }

        /*
         *
         * =====
         *  PsSetCreateProcessNotifyRoutine:
         *  48 83 EC 28         sub     rsp, 28h
         *  8A C2               mov     al, dl
         *  33 D2               xor     edx, edx
         *  84 C0               test    al, al
         *  0F 95 C2            setnz   dl
         *  E8 D6 01 00 00      call    PspSetCreateProcessNotifyRoutine
         *  48 83 C4 28         add     rsp, 28h
         *  C3                  retn
         * =====
         *
         * search for "PspSetCreateProcessNotifyRoutine" pointer
         */
        for ( int i = 0; i < RSCB_MAX_INST_SEARCH; i++ ) {

            /* check if it's "call/jmp PspSetCreateProcessNotifyRoutine" */
            if ( ( DREF_U8( U_PTR( Address ) + i ) == ASM_CALL ) || ( DREF_U8( U_PTR( Address ) + i ) == ASM_JMP ) ) {
            
                /* get offset of PspSetCreateProcessNotifyRoutine */
                Offset = DREF_U32( U_PTR( Address ) + i + 1 );
                
                /* get the pointer of PspSetCreateProcessNotifyRoutine routine */
                Address = C_PTR( U_PTR( Address ) + RsUtilRoutineEnd( Address ) + U_PTR( Offset ) );

                break;
            }

        }

        /*
         * ====
         * PspSetCreateProcessNotifyRoutine:
         * ...
         * 65 48 8B 2C 25 88 01 00 00   mov     rbp, gs:188h
         * 4C 8D 2D ?? ?? ?? ??         lea     r13, PspCreateProcessNotifyRoutine ; <-- we are searching for this.
         * 83 C8 FF                     or      eax, 0FFFFFFFFh
         * 66 01 85 E4 01 00 00         add     [rbp+1E4h], ax
         * 90                           nop
         * 45 33 FF                     xor     r15d, r15d
         * ==== 
         *
         * searching for the PspCreateProcessNotifyRoutine array address
         */
        for ( int i = 0; i < RSCB_MAX_INST_SEARCH; i++ ) {

            /* check for "lea" instruction */
            
            if ( ( DREF_U16( U_PTR( Address ) + i ) == ASM_LEA ) ) {

                DbgBreakPoint();

                /* get offset of PspCreateProcessNotifyRoutine */
                Offset = DREF_U32( U_PTR( Address ) + i + 3 );

                /* get PspCreateProcessNotifyRoutine array address */
                Address = C_PTR( U_PTR( Address ) + i + 7 + Offset );

            }

        }

    }

    return Address; 
}
