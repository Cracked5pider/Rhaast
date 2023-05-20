#pragma once

#include <Common.h>

ULONG ProcessLockOffset(
	VOID
);

ULONG ProcessActiveListOffset(
	VOID
);

NTSTATUS ProcessHide(
	ULONG Pid
);