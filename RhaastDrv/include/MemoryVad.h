#pragma once

#include <Common.h>

/**
 * reference:
 *      https://www.vergiliusproject.com/kernels/x64/Windows%2011/22H2%20(2022%20Update)/_MMVAD
 */

//0x8 bytes (sizeof)
struct _MI_VAD_SEQUENTIAL_INFO
{
    ULONGLONG Length : 12;                                                    //0x0
    ULONGLONG Vpn : 52;                                                       //0x0
};

//0x4 bytes (sizeof)
struct _MMVAD_FLAGS2
{
    ULONG FileOffset : 24;                                                    //0x0
    ULONG Large : 1;                                                          //0x0
    ULONG TrimBehind : 1;                                                     //0x0
    ULONG Inherit : 1;                                                        //0x0
    ULONG NoValidationNeeded : 1;                                             //0x0
    ULONG PrivateDemandZero : 1;                                              //0x0
    ULONG Spare : 3;                                                          //0x0
};

//0x4 bytes (sizeof)
struct _MMVAD_FLAGS1
{
    ULONG CommitCharge : 31;                                                  //0x0
    ULONG MemCommit : 1;                                                      //0x0
};

//0x4 bytes (sizeof)
struct _MM_SHARED_VAD_FLAGS
{
    ULONG Lock : 1;                                                           //0x0
    ULONG LockContended : 1;                                                  //0x0
    ULONG DeleteInProgress : 1;                                               //0x0
    ULONG NoChange : 1;                                                       //0x0
    ULONG VadType : 3;                                                        //0x0
    ULONG Protection : 5;                                                     //0x0
    ULONG PreferredNode : 7;                                                  //0x0
    ULONG PageSize : 2;                                                       //0x0
    ULONG PrivateMemoryAlwaysClear : 1;                                       //0x0
    ULONG PrivateFixup : 1;                                                   //0x0
    ULONG HotPatchState : 2;                                                  //0x0
};

//0x4 bytes (sizeof)
struct _MM_GRAPHICS_VAD_FLAGS
{
    ULONG Lock : 1;                                                           //0x0
    ULONG LockContended : 1;                                                  //0x0
    ULONG DeleteInProgress : 1;                                               //0x0
    ULONG NoChange : 1;                                                       //0x0
    ULONG VadType : 3;                                                        //0x0
    ULONG Protection : 5;                                                     //0x0
    ULONG PreferredNode : 7;                                                  //0x0
    ULONG PageSize : 2;                                                       //0x0
    ULONG PrivateMemoryAlwaysSet : 1;                                         //0x0
    ULONG WriteWatch : 1;                                                     //0x0
    ULONG FixedLargePageSize : 1;                                             //0x0
    ULONG ZeroFillPagesOptional : 1;                                          //0x0
    ULONG GraphicsAlwaysSet : 1;                                              //0x0
    ULONG GraphicsUseCoherentBus : 1;                                         //0x0
    ULONG GraphicsNoCache : 1;                                                //0x0
    ULONG GraphicsPageProtection : 3;                                         //0x0
};

//0x4 bytes (sizeof)
struct _MM_PRIVATE_VAD_FLAGS
{
    ULONG Lock : 1;                                                           //0x0
    ULONG LockContended : 1;                                                  //0x0
    ULONG DeleteInProgress : 1;                                               //0x0
    ULONG NoChange : 1;                                                       //0x0
    ULONG VadType : 3;                                                        //0x0
    ULONG Protection : 5;                                                     //0x0
    ULONG PreferredNode : 7;                                                  //0x0
    ULONG PageSize : 2;                                                       //0x0
    ULONG PrivateMemoryAlwaysSet : 1;                                         //0x0
    ULONG WriteWatch : 1;                                                     //0x0
    ULONG FixedLargePageSize : 1;                                             //0x0
    ULONG ZeroFillPagesOptional : 1;                                          //0x0
    ULONG Graphics : 1;                                                       //0x0
    ULONG Enclave : 1;                                                        //0x0
    ULONG ShadowStack : 1;                                                    //0x0
    ULONG PhysicalMemoryPfnsReferenced : 1;                                   //0x0
};

typedef enum _MI_VAD_TYPE
{
    VadTypeNone,
    VadTypeDevicePhysicalMemory,
    VadTypeImageMap,
    VadTypeAwe,
    VadTypeWriteWatch,
    VadTypeLargePages,
    VadTypeRotatePhysical,
    VadTypeLargePageSection
} MI_VAD_TYPE, * PMI_VAD_TYPE;

//0x4 bytes (sizeof)
struct _MMSUBSECTION_FLAGS
{
    USHORT SubsectionAccessed : 1;                                            //0x0
    USHORT Protection : 5;                                                    //0x0
    USHORT StartingSector4132 : 10;                                           //0x0
    USHORT SubsectionStatic : 1;                                              //0x2
    USHORT GlobalMemory : 1;                                                  //0x2
    USHORT Spare : 1;                                                         //0x2
    USHORT OnDereferenceList : 1;                                             //0x2
    USHORT SectorEndOffset : 12;                                              //0x2
};

//0x4 bytes (sizeof)
struct _MI_SUBSECTION_ENTRY1
{
    ULONG CrossPartitionReferences : 30;                                      //0x0
    ULONG SubsectionMappedLarge : 2;                                          //0x0
};

//0x4 bytes (sizeof)
struct _MMVAD_FLAGS
{
    ULONG Lock : 1;                                                           //0x0
    ULONG LockContended : 1;                                                  //0x0
    ULONG DeleteInProgress : 1;                                               //0x0
    ULONG NoChange : 1;                                                       //0x0
    ULONG VadType : 3;                                                        //0x0
    ULONG Protection : 5;                                                     //0x0
    ULONG PreferredNode : 7;                                                  //0x0
    ULONG PageSize : 2;                                                       //0x0
    ULONG PrivateMemory : 1;                                                  //0x0
};

//0x8 bytes (sizeof)
struct _EX_PUSH_LOCK
{
    union
    {
        struct
        {
            ULONGLONG Locked : 1;                                             //0x0
            ULONGLONG Waiting : 1;                                            //0x0
            ULONGLONG Waking : 1;                                             //0x0
            ULONGLONG MultipleShared : 1;                                     //0x0
            ULONGLONG Shared : 60;                                            //0x0
        };
        ULONGLONG Value;                                                    //0x0
        VOID* Ptr;                                                          //0x0
    };
};

//0x40 bytes (sizeof)
typedef struct _MMVAD_SHORT
{
    union
    {
        struct
        {
            struct _MMVAD_SHORT* NextVad;                                   //0x0
            VOID* ExtraCreateInfo;                                          //0x8
        };
        struct _RTL_BALANCED_NODE VadNode;                                  //0x0
    };
    ULONG StartingVpn;                                                      //0x18
    ULONG EndingVpn;                                                        //0x1c
    UCHAR StartingVpnHigh;                                                  //0x20
    UCHAR EndingVpnHigh;                                                    //0x21
    UCHAR CommitChargeHigh;                                                 //0x22
    UCHAR SpareNT64VadUChar;                                                //0x23
    LONG ReferenceCount;                                                    //0x24
    struct _EX_PUSH_LOCK PushLock;                                          //0x28
    union
    {
        ULONG LongFlags;                                                    //0x30
        struct _MMVAD_FLAGS VadFlags;                                       //0x30
        struct _MM_PRIVATE_VAD_FLAGS PrivateVadFlags;                       //0x30
        struct _MM_GRAPHICS_VAD_FLAGS GraphicsVadFlags;                     //0x30
        struct _MM_SHARED_VAD_FLAGS SharedVadFlags;                         //0x30
        volatile ULONG VolatileVadLong;                                     //0x30
    } u;                                                                    //0x30
    union
    {
        ULONG LongFlags1;                                                   //0x34
        struct _MMVAD_FLAGS1 VadFlags1;                                     //0x34
    } u1;                                                                   //0x34
    union
    {
        ULONGLONG EventListULongPtr;                                        //0x38
        UCHAR StartingVpnHigher : 4;                                          //0x38
    } u5;                                                                   //0x38
} MMVAD_SHORT, *PMMVAD_SHORT;

//0x88 bytes (sizeof)
typedef struct _MMVAD
{
    struct _MMVAD_SHORT Core;                                               //0x0
    union
    {
        ULONG LongFlags2;                                                   //0x40
        volatile struct _MMVAD_FLAGS2 VadFlags2;                            //0x40
    } u2;                                                                   //0x40
    struct _SUBSECTION* Subsection;                                         //0x48
    struct _MMPTE* FirstPrototypePte;                                       //0x50
    struct _MMPTE* LastContiguousPte;                                       //0x58
    struct _LIST_ENTRY ViewLinks;                                           //0x60
    struct _EPROCESS* VadsProcess;                                          //0x70
    union
    {
        struct _MI_VAD_SEQUENTIAL_INFO SequentialVa;                        //0x78
        struct _MMEXTEND_INFO* ExtendedInfo;                                //0x78
    } u4;                                                                   //0x78
    struct _FILE_OBJECT* FileObject;                                        //0x80
} MMVAD, *PMMVAD;

typedef RTL_BALANCED_NODE MMADDRESS_NODE, *PMMADDRESS_NODE;

//0x8 bytes (sizeof)
typedef struct _RTL_AVL_TREE
{
    struct _RTL_BALANCED_NODE* Root;                                        //0x0
} RTL_AVL_TREE, *PRTL_AVL_TREE;

typedef struct _MM_AVL_TABLE 
{
    PRTL_BALANCED_NODE BalancedRoot;
    PVOID              NodeHint;
    ULONG64            NumberGenericTableElements;
} MM_AVL_TABLE, * PMM_AVL_TABLE;

//0x4 bytes (sizeof)
struct _MMSECTION_FLAGS
{
    ULONG BeingDeleted : 1;                                                   //0x0
    ULONG BeingCreated : 1;                                                   //0x0
    ULONG BeingPurged : 1;                                                    //0x0
    ULONG NoModifiedWriting : 1;                                              //0x0
    ULONG FailAllIo : 1;                                                      //0x0
    ULONG Image : 1;                                                          //0x0
    ULONG Based : 1;                                                          //0x0
    ULONG File : 1;                                                           //0x0
    ULONG AttemptingDelete : 1;                                               //0x0
    ULONG PrefetchCreated : 1;                                                //0x0
    ULONG PhysicalMemory : 1;                                                 //0x0
    ULONG ImageControlAreaOnRemovableMedia : 1;                               //0x0
    ULONG Reserve : 1;                                                        //0x0
    ULONG Commit : 1;                                                         //0x0
    ULONG NoChange : 1;                                                       //0x0
    ULONG WasPurged : 1;                                                      //0x0
    ULONG UserReference : 1;                                                  //0x0
    ULONG GlobalMemory : 1;                                                   //0x0
    ULONG DeleteOnClose : 1;                                                  //0x0
    ULONG FilePointerNull : 1;                                                //0x0
    ULONG PreferredNode : 7;                                                  //0x0
    ULONG GlobalOnlyPerSession : 1;                                           //0x0
    ULONG UserWritable : 1;                                                   //0x0
    ULONG SystemVaAllocated : 1;                                              //0x0
    ULONG PreferredFsCompressionBoundary : 1;                                 //0x0
    ULONG PageSize64K : 1;                                                    //0x0
};

//0x4 bytes (sizeof)
struct _MMSECTION_FLAGS2
{
    USHORT PartitionId : 10;                                                  //0x0
    UCHAR NoCrossPartitionAccess : 1;                                         //0x2
    UCHAR SubsectionCrossPartitionReferenceOverflow : 1;                      //0x2
    UCHAR UsingFileExtents : 2;                                               //0x2
};

//0x8 bytes (sizeof)
struct _EX_FAST_REF
{
    union
    {
        VOID* Object;                                                       //0x0
        ULONGLONG RefCnt : 4;                                                 //0x0
        ULONGLONG Value;                                                    //0x0
    };
};

//0x80 bytes (sizeof)
struct _CONTROL_AREA
{
    struct _SEGMENT* Segment;                                               //0x0
    union
    {
        struct _LIST_ENTRY ListHead;                                        //0x8
        VOID* AweContext;                                                   //0x8
    };
    ULONGLONG NumberOfSectionReferences;                                    //0x18
    ULONGLONG NumberOfPfnReferences;                                        //0x20
    ULONGLONG NumberOfMappedViews;                                          //0x28
    ULONGLONG NumberOfUserReferences;                                       //0x30
    union
    {
        ULONG LongFlags;                                                    //0x38
        struct _MMSECTION_FLAGS Flags;                                      //0x38
    } u;                                                                    //0x38
    union
    {
        ULONG LongFlags;                                                    //0x3c
        struct _MMSECTION_FLAGS2 Flags;                                     //0x3c
    } u1;                                                                   //0x3c
    struct _EX_FAST_REF FilePointer;                                        //0x40
    volatile LONG ControlAreaLock;                                          //0x48
    ULONG ModifiedWriteCount;                                               //0x4c
    struct _MI_CONTROL_AREA_WAIT_BLOCK* WaitList;                           //0x50
    union
    {
        struct
        {
            union
            {
                ULONG NumberOfSystemCacheViews;                             //0x58
                ULONG ImageRelocationStartBit;                              //0x58
            };
            union
            {
                volatile LONG WritableUserReferences;                       //0x5c
                struct
                {
                    ULONG ImageRelocationSizeIn64k : 16;                      //0x5c
                    ULONG SystemImage : 1;                                    //0x5c
                    ULONG CantMove : 1;                                       //0x5c
                    ULONG StrongCode : 2;                                     //0x5c
                    ULONG BitMap : 2;                                         //0x5c
                    ULONG ImageActive : 1;                                    //0x5c
                    ULONG ImageBaseOkToReuse : 1;                             //0x5c
                };
            };
            union
            {
                ULONG FlushInProgressCount;                                 //0x60
                ULONG NumberOfSubsections;                                  //0x60
                struct _MI_IMAGE_SECURITY_REFERENCE* SeImageStub;           //0x60
            };
        } e2;                                                               //0x58
    } u2;                                                                   //0x58
    struct _EX_PUSH_LOCK FileObjectLock;                                    //0x68
    volatile ULONGLONG LockedPages;                                         //0x70
    union
    {
        ULONGLONG IoAttributionContext : 61;                                  //0x78
        ULONGLONG Spare : 3;                                                  //0x78
        ULONGLONG ImageCrossPartitionCharge;                                //0x78
        ULONGLONG CommittedPageCount : 36;                                    //0x78
    } u3;                                                                   //0x78
};

//0x38 bytes (sizeof)
struct _SUBSECTION
{
    struct _CONTROL_AREA* ControlArea;                                      //0x0
    struct _MMPTE* SubsectionBase;                                          //0x8
    struct _SUBSECTION* NextSubsection;                                     //0x10
    union
    {
        struct _RTL_AVL_TREE GlobalPerSessionHead;                          //0x18
        struct _MI_CONTROL_AREA_WAIT_BLOCK* CreationWaitList;               //0x18
        struct _MI_PER_SESSION_PROTOS* SessionDriverProtos;                 //0x18
    };
    union
    {
        ULONG LongFlags;                                                    //0x20
        struct _MMSUBSECTION_FLAGS SubsectionFlags;                         //0x20
    } u;                                                                    //0x20
    ULONG StartingSector;                                                   //0x24
    ULONG NumberOfFullSectors;                                              //0x28
    ULONG PtesInSubsection;                                                 //0x2c
    union
    {
        struct _MI_SUBSECTION_ENTRY1 e1;                                    //0x30
        ULONG EntireField;                                                  //0x30
    } u1;                                                                   //0x30
    ULONG UnusedPtes : 30;                                                    //0x34
    ULONG ExtentQueryNeeded : 1;                                              //0x34
    ULONG DirtyPages : 1;                                                     //0x34
};