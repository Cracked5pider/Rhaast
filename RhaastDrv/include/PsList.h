#pragma once

#include <Common.h>

typedef struct _PSHIDDEN_LIST
{
    PLIST_ENTRY ProcessEntry;
    ULONG       Pid;

    /* next entry */
    struct _PSHIDDEN_LIST* Next;
} PSHIDDEN_LIST, *PPSHIDDEN_LIST;

BOOLEAN PsListHiddenCheck(
    _In_ ULONG Pid
);

NTSTATUS PsListHiddenAdd(
    _In_ ULONG       Pid,
    _In_ PLIST_ENTRY ProcessEntry
);

NTSTATUS PsListHiddenRemove(
    _In_  ULONG        Pid,
    _Out_ PLIST_ENTRY* ProcessEntry
);