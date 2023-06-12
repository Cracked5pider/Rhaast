#pragma once

#include <MemoryVad.h>

PVOID RsMemAlloc(
    _In_ SIZE_T Size 
);

VOID RsMemFree(
    _In_ PVOID Memory
);

NTSTATUS MemoryVadSearch(
    _In_  PEPROCESS     Process,
    _In_  ULONG_PTR     Address,
    _Out_ PMMVAD*       MmVadEntry
);

NTSTATUS MemoryVadHide(
    _In_ ULONG     Pid,
    _In_ ULONG_PTR Address
);

NTSTATUS MemoryVadUnHide(
    _In_ ULONG     Pid,
    _In_ ULONG_PTR Address
);