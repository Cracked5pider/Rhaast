#pragma once

#include <Common.h>

NTSTATUS RsCallbacksRegister(
    VOID
);

NTSTATUS RsCallbacksUnRegister(
    VOID
);

VOID RsCallbackOnProcessCreate(
    IN OUT PEPROCESS				Process, 
    IN	   HANDLE					ProcessId,
    IN OUT PPS_CREATE_NOTIFY_INFO	CreateInfo
);