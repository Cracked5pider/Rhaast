#pragma once

#include <MemoryVad.h>

NTSTATUS MemoryVadSearch(
    IN  PEPROCESS     Process,
    IN  ULONG_PTR     Address,
    OUT PMMVAD*       MmVadEntry
);

NTSTATUS MemoryVadHide(
    IN ULONG     Pid,
    IN ULONG_PTR Address 
);

NTSTATUS MemoryVadUnHide(
    IN ULONG     Pid,
    IN ULONG_PTR Address
);