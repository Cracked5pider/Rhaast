#pragma once

#include <Common.h>

ULONG_PTR RsHasher(
    _In_     PVOID Data,
    _In_opt_ ULONG Length
);

PVOID RsKernelBase(
    VOID
);

NTSTATUS RsDrvNameBaseFromAddr(
    _In_      PVOID  Address,
    _Out_opt_ PCHAR* DrvName,
    _Out_opt_ PVOID* DrvBase
);

PVOID RsLdrFunction( 
    _In_ ULONG Hash
);

ULONG_PTR RsUtilRoutineEnd(
    _In_ PVOID Routine
);
