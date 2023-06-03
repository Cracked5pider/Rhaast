#pragma once

#include <MemoryVad.h>

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