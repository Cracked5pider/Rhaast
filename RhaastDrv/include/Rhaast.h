#pragma once

#include <Common.h>

/* import headers */
#include <Macros.h>
#include <Defines.h>
#include <Callbacks.h>
#include <Process.h>
#include <PsList.h>
#include <List.h>

/* Driver Instance struct */
typedef struct _INSTANCE {
    /* driver main object */
    PDRIVER_OBJECT  DriverObject;
    PUNICODE_STRING RegistryPath;

    /* Windows Build number */
    ULONG WindowsBuild;

    /* linked list */
    PPSHIDDEN_LIST PsHiddenList;
} INSTANCE, *PINSTANCE;

/* defines */
DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD     DriverUnload;

/* global instance */
extern INSTANCE Instance;

NTSTATUS RhaastEntry(
    VOID
);

NTSTATUS RhaastInit(
    VOID
);

VOID RhaastUnLoad(
    IN PDRIVER_OBJECT DriverObject
); 