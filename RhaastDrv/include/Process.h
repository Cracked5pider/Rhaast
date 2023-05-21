#pragma once

#include <Common.h>

NTSTATUS ProcessHide(
    ULONG Pid
);

ULONG ProcessLockOffset(
    VOID
);

ULONG ProcessActiveListOffset(
    VOID
);