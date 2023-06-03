#pragma once

#include <Common.h>
#include <Command.h>

NTSTATUS ProcessHide(
    _In_ ULONG Pid
);

NTSTATUS ProcessUnHide(
    _In_ ULONG Pid
);

NTSTATUS ProcessProtect(
    _In_ PRS_C_PROCESS_PROTECTION Protection
);