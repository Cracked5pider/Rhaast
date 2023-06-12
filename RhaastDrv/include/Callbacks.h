#pragma once

#include <Common.h>

#define RSCB_MAX_BUFFER_SIZE 258
#define RSCB_MAX_INST_SEARCH 300
#define RSCB_MAX_CALLBACKS   64

/* amount of available callback types */
#define RS_CALLBACK_TYPE_SIZE 4

typedef enum _RS_CALLBACK_TYPE
{
    NoneCallback,
    PsProcessCreationCallback,
    PsThreadCreationCallback,
    PsImageLoadCallback,
    DriverVerificationCallback,
} RS_CALLBACK_TYPE, *PRS_CALLBACK_TYPE;

typedef struct _RS_CALLBACK_DATA
{
    BYTE      Type;
    UINT_PTR  DriverBase;
    UINT_PTR  Callback;
    CHAR      DriverName[ RSCB_MAX_BUFFER_SIZE ];

    /* offset to the next query data */
    UINT_PTR  NextEntryOffset; 
} RS_CALLBACK_DATA, *PRS_CALLBACK_DATA;

NTSTATUS RsCallbackQuery( 
    _In_       RS_CALLBACK_TYPE  Type,
    _Out_opt_  PRS_CALLBACK_DATA Data,
    _Out_      PULONG            Size
);

NTSTATUS RsCallbackRemove(
    _In_ RS_CALLBACK_TYPE Type,
    _In_ ULONG_PTR        Callback
);

NTSTATUS RsCallbackPatch(
    _In_ RS_CALLBACK_TYPE CallbackType
);
