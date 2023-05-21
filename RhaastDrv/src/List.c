#include <Rhaast.h>

/**
 * @brief
 *	Remove/Unlink entry from linked list
 *
 * @param Entry
 *	double linked list entry to be
 *	removed from the double linked list
 */
VOID DoubleLinkedRemove(
	PLIST_ENTRY Entry
) {
	PLIST_ENTRY Prev = NULL;
	PLIST_ENTRY Next = NULL;

	/* save current forward/backward links */
	Next = Entry->Flink;
	Prev = Entry->Blink;

	/* replace links */
	Prev->Flink = Next;
	Next->Blink = Prev;

	/* Re-write the current LIST_ENTRY to point to itself (avoiding BSOD) */
	Entry->Blink = &Entry->Flink;
	Entry->Flink = &Entry->Flink;
}