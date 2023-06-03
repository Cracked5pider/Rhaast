#pragma once

#include <Common.h>

VOID DoubleLinkedRemove( 
    _In_ _Out_ PLIST_ENTRY Entry
);

VOID DoubleLinkedAdd(
    _In_ _Out_ PLIST_ENTRY Main,
    _In_ _Out_ PLIST_ENTRY Node
);
