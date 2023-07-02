#include <Rhaast.h>

/* declare global instance object */
INSTANCE Instance = { 0 };

/* Driver entrypoint */
NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
) {
    /* init instance */
    Instance.DriverObject = DriverObject; 
    Instance.RegistryPath = RegistryPath;

    /* execute the main entry point of Rhaast */
    return RhaastEntry( );
}