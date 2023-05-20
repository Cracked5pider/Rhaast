#include <Rhaast.h>

NTSTATUS RsCallbacksRegister(
	VOID
) {
	NTSTATUS NtStatus = STATUS_SUCCESS; 

	PRINTF( "Register \"process create\" callbacks" )

	/* notify when a new process is created */
	if ( ! NT_SUCCESS( NtStatus = PsSetCreateProcessNotifyRoutineEx( RsCallbackOnProcessCreate, FALSE ) ) ) {
		PRINTF( "PsSetCreateProcessNotifyRoutineEx Failed: %p\n", NtStatus )
	}

	return NtStatus;
}

NTSTATUS RsCallbacksUnRegister(
	VOID
) {
	NTSTATUS NtStatus = STATUS_SUCCESS;

	PRINTF( "UnRegister \"process create\" callbacks" )

	/* remove callback */
	if ( ! NT_SUCCESS( NtStatus = PsSetCreateProcessNotifyRoutineEx( RsCallbackOnProcessCreate, TRUE ) ) ) {
		PRINTF( "PsSetCreateProcessNotifyRoutineEx Failed: %p\n", NtStatus )
	}

	return NtStatus;
}

VOID RsCallbackOnProcessCreate(
	IN OUT PEPROCESS				Process,
	IN	   HANDLE					ProcessId,
	IN OUT PPS_CREATE_NOTIFY_INFO	CreateInfo
) {
	/* check if CreateInfo has been specified. 
	 * if yes then it means that it's a new process */
	if ( CreateInfo ) {
		PRINTF(
			"Process Created: \n"
			" - File name    : %ls	\n"
			" - Parent Pid   : %d	\n"
			" - Pid          : %d	\n"
			" - Command Line : %ls	\n",
			CreateInfo->ImageFileName->Buffer,
			CreateInfo->ParentProcessId,
			CreateInfo->CreatingThreadId.UniqueProcess,
			CreateInfo->CommandLine->Buffer
		)
	} else {
		/* looks like we terminating a process */
		PRINTF( "Process Exiting: %d", ProcessId )
	}
}