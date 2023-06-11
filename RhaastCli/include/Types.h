#ifndef RHAAST_TYPES_H
#define RHAAST_TYPES_H

#include <string>

typedef std::wstring WSTRING;

/* defines */
#define RSCB_MAX_BUFFER_SIZE 258

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

typedef enum _RS_CALLBACK_TYPE
{
    NoneCallback,
    PsProcessCreationCallback,
    PsThreadCreationCallback,
    PsImageLoadCallback,
    DriverVerification
} RS_CALLBACK_TYPE, *PRS_CALLBACK_TYPE;

typedef struct _RS_CALLBACK_DATA
{
    RS_CALLBACK_TYPE Type;
    UINT_PTR         DriverBase;
    UINT_PTR         Callback;
    CHAR             DriverName[ RSCB_MAX_BUFFER_SIZE ];

    /* offset to the next query data */
    UINT_PTR         NextEntryOffset;
} RS_CALLBACK_DATA, *PRS_CALLBACK_DATA;

#endif