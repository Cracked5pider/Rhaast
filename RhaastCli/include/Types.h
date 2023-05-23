#ifndef RHAAST_TYPES_H
#define RHAAST_TYPES_H

#include <string>

typedef std::wstring WSTRING;

typedef struct _BUFFER {
    PVOID Buffer;
    ULONG Length;
} BUFFER, *PBUFFER;

#endif