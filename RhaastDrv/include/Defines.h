#pragma once

#define RHAAST_NAME         L"Rhaast"
#define RHAAST_DRIVER_NAME  L"\\Driver\\" RHAAST_NAME
#define RHAAST_DEVICE_NAME  L"\\Device\\" RHAAST_NAME
#define RHAAST_SYMBOL_LINK  L"\\??\\"     RHAAST_NAME

/* windows build numbers */
#define WINBUILD_1507		10240
#define WINBUILD_1511		10586
#define WINBUILD_1607		14393
#define WINBUILD_1703		15063
#define WINBUILD_1709		16299
#define WINBUILD_1803		17134
#define WINBUILD_1809		17763
#define WINBUILD_1903		18362
#define WINBUILD_1909		18363
#define WINBUILD_2004		19041
#define WINBUILD_20H2		19042
#define WINBUILD_21H1		19043
#define WINBUILD_21H2		19044
#define WINBUILD_22H2		19045
#define WINBUILD_1121H2		22000
#define WINBUILD_1122H2		22621

#define SYSTEM_PROCESS_PID  0x4

/* rhaast memory pool tags */
#define RS_POOL_TAG_RHST	'tshR'
#define RS_POOL_TAG_PSHD	'DHsP'

/* hash key */
#define H_KEY 5381

/* module hashes */
#define H_MODULE_NTOSKRNL   0x9bf03a73

/* function hashes */
#define H_API_PSISPROTECTEDPROCESSLIGHT             0xe34e5025
#define H_API_ZWQUERYSYSTEMINFORMATION              0x8754a7f7
#define H_API_PSSETCREATEPROCESSNOTIFYROUTINE       0x7923f7e6
#define H_API_PSSETCREATETHREADNOTIFYROUTINE        0xbedbd03f
#define H_API_PSSETLOADIMAGENOTIFYROUTINE           0xd919f4f6
#define H_API_SEREGISTERIMAGEVERIFICATIONCALLBACK   0xd8007615

/* x64 assembly instruction */
#define ASM_RET      0xC3
#define ASM_JMP      0xE9
#define ASM_CALL     0xE8
#define ASM_LEA_R13  0x8D4C
#define ASM_LEA_RCX  0x8D48
#define ASM_MOV_RCX  0x8B48