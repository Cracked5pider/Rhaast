#pragma once

#include <Common.h>

#define RSCB_MAX_BUFFER_SIZE 100
#define RSCB_MAX_INST_SEARCH 300

typedef enum _RS_CALLBACK_TYPE
{
    NoneCallback,
    PsCreationCallback,
} RS_CALLBACK_TYPE, *PRS_CALLBACK_TYPE;

typedef struct _RS_CALLBACK_DATA
{
    RS_CALLBACK_TYPE Type;
    WCHAR            DriverName[ RSCB_MAX_BUFFER_SIZE ];
    UINT_PTR         CallbackOffset;  
    UINT_PTR         CallbackPointer; 

    /* offset to the next query data */
    UINT_PTR         NextEntryOffset; 
} RS_CALLBACK_DATA, *PRS_CALLBACK_DATA;

NTSTATUS RsCallbackQuery( 
    _In_       RS_CALLBACK_TYPE   CallbackType,
    _Out_opt_  PRS_CALLBACK_DATA* CallbackQueryData,
    _Out_      PSIZE_T            Size
);

NTSTATUS RsCallbackPatch(
    _In_ RS_CALLBACK_TYPE CallbackType
);
