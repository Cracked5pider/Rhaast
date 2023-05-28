#include <Rhaast.h>

/**
 * @brief
 *	    Remove/Unlink entry from linked list
 *
 * @param Entry
 *      double linked list entry to be
 *	    removed from the double linked list
 */
VOID DoubleLinkedRemove(
    IN OUT PLIST_ENTRY Entry
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

/**
 * @brief
 *      Inserts node into main linked list
 *
 * @param Main
 *      Main node that is gonna be used to insert the node
 *
 * @param Node
 *      Node that is going to be inserted into main     
 */
VOID DoubleLinkedAdd(
    IN OUT PLIST_ENTRY Main,
    IN OUT PLIST_ENTRY Node
) {
    PLIST_ENTRY Next = NULL;

    Next = Main->Flink;

    Node->Blink = Main;
    Node->Flink = Next;

    Next->Blink = Node;
    Main->Flink = Node;
}