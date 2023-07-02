#pragma once

#include <Common.h>

NTSTATUS RsMapDriver(
    _In_  PVOID   DrvBuffer,
    _Out_ PVOID*  DrvBase
);
