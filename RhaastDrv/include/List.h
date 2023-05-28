#pragma once

#include <Common.h>

VOID DoubleLinkedRemove( 
    IN OUT PLIST_ENTRY Entry
);

VOID DoubleLinkedAdd(
    IN OUT PLIST_ENTRY Main,
    IN OUT PLIST_ENTRY Node
);
