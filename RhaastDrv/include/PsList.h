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
    IN ULONG Pid
);

NTSTATUS PsListHiddenAdd(
    IN ULONG       Pid,
    IN PLIST_ENTRY ProcessEntry
);

NTSTATUS PsListHiddenRemove(
    IN  ULONG        Pid,
    OUT PLIST_ENTRY* ProcessEntry
);