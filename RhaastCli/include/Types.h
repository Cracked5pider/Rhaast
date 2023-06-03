#ifndef RHAAST_TYPES_H
#define RHAAST_TYPES_H

#include <string>

typedef std::wstring WSTRING;

typedef struct _BUFFER {
    PVOID Buffer;
    ULONG Length;
} BUFFER, *PBUFFER;

typedef struct _PS_PROTECTION
{
    union {
        UCHAR Level;
        struct {
            UCHAR Type   : 3;
            UCHAR Audit  : 1;
            UCHAR Signer : 4;
        };
    };
} PS_PROTECTION, *PPS_PROTECTION;

enum SignatureType
{
    PsProtectedTypeNone           = 0,
    PsProtectedTypeProtectedLight = 1,
    PsProtectedTypeProtected      = 2
};

typedef struct _PS_SIG_PROTECTION {
    UCHAR         SignatureLevel;
    UCHAR         SectionSignatureLevel;
    PS_PROTECTION Protection;
} PS_SIG_PROTECTION, *PPS_SIG_PROTECTION;

#endif