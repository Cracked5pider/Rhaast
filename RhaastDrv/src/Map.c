#include <Rhaast.h>

NTSTATUS RspLdrRelocateImage(
    _In_ PVOID                 Image,
    _In_ PIMAGE_DATA_DIRECTORY ImageDir
);

NTSTATUS RspLdrResolveIat(
    _In_ PVOID                 Image,
    _In_ PIMAGE_DATA_DIRECTORY ImageDir
);

/**
 * @brief
 *      maps driver into memory and executes it.
 *
 * @param DrvBuffer
 *      Driver buffer to map into kernel memory.
 *
 * @param DrvBase
 *      driver base address of mapped driver
 *
 * @return
 *      status of function 
 */
NTSTATUS RsMapDriver(
    _In_  PVOID   DrvBuffer,
    _Out_ PVOID*  DrvBase
) {
    NTSTATUS              NtStatus  = STATUS_UNSUCCESSFUL;
    PIMAGE_NT_HEADERS     NtHeader  = NULL;
    PIMAGE_SECTION_HEADER SecHeader = NULL;
    SIZE_T                ImageSize = 0;
    PVOID                 BaseAddr  = NULL;
    PIMAGE_DATA_DIRECTORY ImageDir  = NULL;
    PDRIVER_INITIALIZE    DrvEntry  = NULL;

    /* check if all params have been specified */
    if ( ( ! DrvBuffer ) || ( ! DrvBase ) ) {
        return STATUS_INVALID_PARAMETER;
    }

    DbgBreakPoint();

    /* check if it's a PE image */
    if ( ( ( PIMAGE_DOS_HEADER ) DrvBuffer )->e_magic != IMAGE_DOS_SIGNATURE ) {
        return STATUS_INVALID_PARAMETER;
    }

    /* resolve some header & image info */
    NtHeader  = C_PTR( U_PTR( DrvBuffer ) + ( ( PIMAGE_DOS_HEADER ) DrvBuffer )->e_lfanew );
    ImageSize = NtHeader->OptionalHeader.SizeOfImage;

    /* allocate memory for driver */
    if ( ! ( BaseAddr = ExAllocatePool2( POOL_FLAG_NON_PAGED, ImageSize, RS_POOL_TAG_RHST ) ) ) {
        NtStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto END;
    }

    /* copy over header */
    RtlCopyMemory( BaseAddr, DrvBuffer, NtHeader->OptionalHeader.SizeOfHeaders );

    /* get first section header */
    SecHeader = IMAGE_FIRST_SECTION( NtHeader );

    /* copy over sections */
    for ( DWORD i = 0; i < NtHeader->FileHeader.NumberOfSections; i++ ) {
        RtlCopyMemory(
            ( U_PTR( BaseAddr  ) + SecHeader[ i ].VirtualAddress   ),
            ( U_PTR( DrvBuffer ) + SecHeader[ i ].PointerToRawData ),
            SecHeader[ i ].SizeOfRawData
        );
    }
    
    /* relocate image */
    ImageDir = & NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_BASERELOC ];
    if ( ImageDir->VirtualAddress ) {
        if ( ! NT_SUCCESS( NtStatus = RspLdrRelocateImage( BaseAddr, ImageDir ) ) ) {
            PRINTF( "RspLdrRelocateImage Failed: %p\n", NtStatus )
            goto END; 
        }
    }

    /* process image import table.
     * atm only function for ntoskrnl are getting resolved */
    ImageDir = & NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ];
    if ( ImageDir->VirtualAddress ) {
        if ( ! NT_SUCCESS( NtStatus = RspLdrResolveIat( BaseAddr, ImageDir ) ) ) {
            PRINTF( "RspLdrResolveIat Failed: %p\n", NtStatus )
            goto END; 
        }
    }

    /* get entry address */
    DrvEntry = C_PTR( U_PTR( BaseAddr ) + NtHeader->OptionalHeader.AddressOfEntryPoint );

    /* Execute driver entry */
    NtStatus = DrvEntry( NULL, NULL );

END:
    /* only free if failed */
    if ( ( ! NT_SUCCESS( NtStatus ) ) && BaseAddr ) {
        ExFreePool2( BaseAddr, RS_POOL_TAG_RHST, NULL, 0 );
        BaseAddr = NULL;
    }

    return NtStatus; 
}

/**
 * @brief
 *      relocate driver image
 *
 * @param Image
 *      image base address to relocate
 *
 * @param ImageDir
 *      Image data directory of IMAGE_DIRECTORY_ENTRY_BASERELOC
 */
NTSTATUS RspLdrRelocateImage(
    _In_ PVOID                 Image,
    _In_ PIMAGE_DATA_DIRECTORY ImageDir
) {
    NTSTATUS NtStatus = STATUS_SUCCESS;

    return NtStatus;
}

/**
 * @brief
 *      resolve import address table of driver
 *
 * @param Image
 *      image base address to resolve import address table.
 *
 * @param ImageDir
 *      Image data directory of IMAGE_DIRECTORY_ENTRY_IMPORT
 */
NTSTATUS RspLdrResolveIat(
    _In_ PVOID                 Image,
    _In_ PIMAGE_DATA_DIRECTORY ImageDir
) {
    NTSTATUS NtStatus = STATUS_SUCCESS;

    return NtStatus;
}
