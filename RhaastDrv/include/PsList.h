#pragma once

#include <Common.h>

typedef struct _PSHIDDEN_LIST
{
	PLIST_ENTRY ProcessEntry;
	ULONG		Pid;

	/* next entry */
	struct _PSHIDDEN_LIST* Next;
} PSHIDDEN_LIST, *PPSHIDDEN_LIST;

NTSTATUS PsListHiddenAdd(
	ULONG		Pid,
	PLIST_ENTRY ProcessEntry
);

NTSTATUS PsListHiddenRemove(
	ULONG Pid
);