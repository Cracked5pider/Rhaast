#include <Rhaast.h>

/* declare global instance object */
INSTANCE Instance = { 0 };

/* Driver entrypoint */
NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT  DriverObject,
	IN PUNICODE_STRING RegistryPath
) {
	/* init instance */
	Instance.DriverObject = DriverObject; 
	Instance.RegistryPath = RegistryPath;

	/* execute the main entrypoint of Rhaast */
	return RhaastEntry( );
}