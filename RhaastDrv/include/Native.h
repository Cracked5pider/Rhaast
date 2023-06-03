#pragma once

#include <Common.h>

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

typedef unsigned short WORD,  *PWORD;
typedef unsigned long  DWORD, *PDWORD;

//0xa0 bytes (sizeof)
typedef struct _KLDR_DATA_TABLE_ENTRY
{
    struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
    VOID* ExceptionTable;                                                   //0x10
    ULONG ExceptionTableSize;                                               //0x18
    VOID* GpValue;                                                          //0x20
    struct _NON_PAGED_DEBUG_INFO* NonPagedDebugInfo;                        //0x28
    VOID* DllBase;                                                          //0x30
    VOID* EntryPoint;                                                       //0x38
    ULONG SizeOfImage;                                                      //0x40
    struct _UNICODE_STRING FullDllName;                                     //0x48
    struct _UNICODE_STRING BaseDllName;                                     //0x58
    ULONG Flags;                                                            //0x68
    USHORT LoadCount;                                                       //0x6c
    union
    {
        USHORT SignatureLevel : 4;                                          //0x6e
        USHORT SignatureType : 3;                                           //0x6e
        USHORT Frozen : 2;                                                  //0x6e
        USHORT HotPatch : 1;                                                //0x6e
        USHORT Unused : 6;                                                  //0x6e
        USHORT EntireField;                                                 //0x6e
    } u1;                                                                   //0x6e
    VOID* SectionPointer;                                                   //0x70
    ULONG CheckSum;                                                         //0x78
    ULONG CoverageSectionSize;                                              //0x7c
    VOID* CoverageSection;                                                  //0x80
    VOID* LoadedImports;                                                    //0x88
    union
    {
        VOID* Spare;                                                        //0x90
        struct _KLDR_DATA_TABLE_ENTRY* NtDataTableEntry;                    //0x90
    };
    ULONG SizeOfImageNotRounded;                                            //0x98
    ULONG TimeDateStamp;                                                    //0x9c
} KLDR_DATA_TABLE_ENTRY, *PKLDR_DATA_TABLE_ENTRY;

#define HASH_KEY 5381

ULONG_PTR RsHasher(
    _In_     PVOID Data,
    _In_opt_ ULONG Length
);

PVOID RsKernelBase(
    VOID
);

PVOID RsLdrFunction( 
    _In_ ULONG Hash
);

ULONG_PTR RsUtilRoutineEnd(
    _In_ PVOID Routine
);
