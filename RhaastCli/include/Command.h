#ifndef RHAAST_COMMANDS_H
#define RHAAST_COMMANDS_H

#include <Types.h>

/* command id's */
#define RHAAST_COMMAND_PROCESS_HIDE     0x501
#define RHAAST_COMMAND_PROCESS_UNHIDE   0x502
#define RHAAST_COMMAND_MEMORY_VAD       0x503
#define RHAAST_COMMAND_PROCESS_PROTECT  0x504

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

#endif