# NPU Architecture Version 3510<a name="ZH-CN_TOPIC_0000002468380701"></a>

<!-- md-trans-meta sourceCommit=c0c5cde6df534c03224f524ed2b2d081018dca09 translatedAt=2026-10-23T14:21:46.515Z pushedAt=2026-11-03T07:05:13.429Z nl=2 -->

This section describes the hardware architecture and its functional description for [version 3510](../../../language_extension/simd_builtin_keywords.md#npu-arch) of **\_\_NPU\_ARCH\_\_**.

## Hardware Architecture Diagram<a name="section2189135013710"></a>

As shown in the following figure, in this architecture, the AI Core is divided into two independent cores, AIC and AIV, which are used for cube computation and vector computation, respectively. The ratio of AIC cores to AIV cores is 1:2. Each core has its own Scalar unit and can independently load its own code segment.

![](../../../../figures/npu_3510_hw_arch.png)

The key features of this architecture are as follows:

-   Add the data path from L0C Buffer to UB, from UB to L1 Buffer, and from L1 Buffer to UB.
-   Remove the data paths Global Memory -\> L0A Buffer and Global Memory -\> L0B Buffer.
-   Remove the data path L1 Buffer -\> Global Memory.
-   SSBuffer, used for inter-core communication between AIC and AIV.
-   Add the SIMD Register File storage hierarchy. In SIMD programs, data is moved from the Unified Buffer (UB) to registers for computation, and the intermediate results generated can be computed directly in registers without being moved back to the UB.
-   Add SIMT-related hardware units. The SIMT-related hardware units are described as follows:

    <a name="table45836437171"></a>
    <table><thead align="left"><tr id="row1558324391711"><th class="cellrowborder" valign="top" width="18.85%" id="mcps1.1.3.1.1"><p id="p558318431177"><a name="p558318431177"></a><a name="p558318431177"></a>SIMT Hardware Unit Name</p>
    </th>
    <th class="cellrowborder" valign="top" width="81.15%" id="mcps1.1.3.1.2"><p id="p3583443191718"><a name="p3583443191718"></a><a name="p3583443191718"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row358314316175"><td class="cellrowborder" valign="top" width="18.85%" headers="mcps1.1.3.1.1 "><p id="p1158311438177"><a name="p1158311438177"></a><a name="p1158311438177"></a>SIMT DCache</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.15%" headers="mcps1.1.3.1.2 "><p id="p658313438176"><a name="p658313438176"></a><a name="p658313438176"></a>SIMT access to GM must go through the SIMT DCache. SIMT supports a maximum 128 KB Data Cache. The Data Cache directly reuses the UB as cacheline. All external memory accesses by SIMT are performed at a granularity of 128 B.</p>
    </td>
    </tr>
    <tr id="row97402054191814"><td class="cellrowborder" valign="top" width="18.85%" headers="mcps1.1.3.1.1 "><p id="p17741125491813"><a name="p17741125491813"></a><a name="p17741125491813"></a>Warp Scheduler</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.15%" headers="mcps1.1.3.1.2 "><p id="p1974115414186"><a name="p1974115414186"></a><a name="p1974115414186"></a>Implements hardware multithreaded scheduling.</p>
    <p id="p129229500325"><a name="p129229500325"></a><a name="p129229500325"></a>In SIMT, each AIV has four Warp Schedulers.</p>
    </td>
    </tr>
    <tr id="row1943773001919"><td class="cellrowborder" valign="top" width="18.85%" headers="mcps1.1.3.1.1 "><p id="p104371530101917"><a name="p104371530101917"></a><a name="p104371530101917"></a>SIMT Register File</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.15%" headers="mcps1.1.3.1.2 "><p id="p74371930131917"><a name="p74371930131917"></a><a name="p74371930131917"></a>An ultra-large-capacity register with a total capacity of 128KB provided for SIMT applications. The number of registers available to each thread depends on the number of threads, as follows:</p>
    <a name="ul3236101542314"></a><a name="ul3236101542314"></a><ul id="ul3236101542314"><li>1025 to 2048 threads: 16 registers.</li><li>513 to 1024 threads: 32 registers.</li><li>257 to 512 threads: 64 registers.</li><li>1 to 256 threads: 127 registers.</li></ul>
    </td>
    </tr>
    </tbody>
    </table>

## Compute Unit<a name="section8490181519513"></a>

**Separate deployment of Cube compute units and Vector compute units**

In this architecture, Cube compute units and Vector compute units are deployed on AIC cores and AIV cores respectively. Each core has its own Scalar unit and can independently load its own code segment.

**Vector compute unit**

-   The Vector compute unit supports the U8, U16, U32, S8, S16, S32, BF16, FP16, and FP32 data types.
-   The Vector compute unit can process 256 bytes of data per cycle.
-   The data processed by the Vector compute unit comes from registers.
-   In this architecture version, the mask value passed in through the high-dimensional splitting interface is converted to MaskReg and passed to the Vector compute unit, whereas in [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md#npu-arch), the mask value is read from a special mask register.

    **Figure 1**  High-dimensional splitting in NPU architecture version 2201<a name="fig7285103215410"></a>
    ![](../../../../figures/npu_arch_2201_high_dim_split.png "High-dimensional splitting in NPU architecture version 2201")

    **Figure 2**  High-dimensional splitting in this architecture version<a name="fig7787845174216"></a>  
    ![](../../../../figures/high_dim_split.png "High-dimensional splitting in this architecture version")

**Cube compute unit**

-   The Cube compute unit supports FP32/FP16/BF16/HiF8/FP8\_E4M3/FP8\_E5M2/U8/S8. It completes a 16x16 by 16x16 cube multiplication of the float16 data type per cycle; for the int8\_t data type, it completes a 16\*32 by 32\*16 cube multiplication per cycle.
-   The storage units accessible to the Cube compute unit are the L0A Buffer, L0B Buffer, and L0C Buffer. The L0A Buffer stores the left cube, the L0B Buffer stores the right cube, and the L0C Buffer stores the results and intermediate results of cube multiplication.

**Scalar unit**

-   The Scalar unit supports the U16/S16/U32/S32/U64/S64/FP64 data types.
-   In the Regbase architecture, the Aux Scalar compute unit separately handles Scalar computation inside SIMD_VF functions, while the Scalar compute unit handles Scalar computation outside SIMD_VF functions.

## Storage Units<a name="section6500173264510"></a>

**Obtaining the memory space size of a storage unit**

Developers can query the memory space size of each storage unit through the [platform information query](../../../../../api/Utils-API/platform_info/platform_info.md) API.

**Parameter specifications of each storage unit**

<a name="table12348145512210"></a>

| Storage Unit Name | Storage Space Size | Alignment Requirement | Kernel-Side Constant Name and Size (Unit: Byte) |
| --- | --- | --- | --- |
| Global Memory | - | 1-byte alignment | - |
| UB | 256KB | 32-byte alignment | ASC_UB_SIZE = 248 × 1024 + BISHENG_VF_STACK + ASC_UB_RESERVE,<br>where BISHENG_VF_STACK and ASC_UB_RESERVE are constants whose default value is 0. After the [compile option](#compile-options-description) is enabled, they are 6K and 2K respectively. |
| L1 Buffer | 512KB | 32-byte alignment | ASC_L1_SIZE = 512 * 1024 |
| L0A Buffer | 64KB | 512-byte alignment | ASC_L0A_SIZE = 64 * 1024 |
| L0B Buffer | 64KB | 512-byte alignment | ASC_L0B_SIZE = 64 * 1024 |
| L0C Buffer | 256KB | 64-byte alignment | ASC_L0C_SIZE = 256 * 1024 |
| BiasTable Buffer | 4KB | 64-byte alignment | ASC_BT_SIZE = 4 * 1024 |
| Fixpipe Buffer | 4KB | 64-byte alignment | - |

**Parameter specifications of each register**

| Register Name | Register Size | Maximum Number of Registers |
| --- | --- | --- |
| Vector data register | 256B | 32 |
| Mask register | 32B | 8 |
| Address register | 4B | 8 |
| Unaligned move-in register | 32B | 4 |
| Unaligned move-in/out register | 32B | 4 |

<a id="compile-options-description"></a>

> [!NOTE] Note
>
> The total UB capacity is 256KB, part of which may be reserved by the system or divided into Data Cache. Therefore, the UB space available to users varies with the compile options and programming scenarios.
>
> Compile options:
>
> - When the [--cce-disable-vf-stack-reserved-ubuf](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461) option is enabled at compile time, the constant BISHENG_VF_STACK = 6K, and the originally reserved 6KB SIMD VF stack space can be used as normal UB space. This reserved space is used by the compiler to cache register spill data. After this option is enabled, the compiler can no longer use this space to cache register spill data, and developers must ensure that no register spill occurs.
> - When the [--cce-disable-asc-reserved-ubuf](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461) option is enabled at compile time, the constant ASC_UB_RESERVE = 2K, and the 2KB UB space originally used to support [some Ascend C APIs](../../../programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#api-scope-for-reservation-usage-of-ub-space) can be used as normal UB space. After this option is enabled, the preceding APIs are unavailable, and a compilation error occurs when they are called.
>
> Programming scenarios:
> - In the SIMD programming scenario, after the preceding two compile options are enabled, the actual UB space available to users is: `248KB + 6KB + 2KB = 256KB`.
> 
> - In the SIMD and SIMT hybrid programming scenario, after the preceding two compile options are enabled, the maximum actual UB space available to users is: `248KB + 6KB + 2KB - 32KB = 224KB`. This is because 32KB to 128KB must be divided from the UB as Data Cache to cache the data when SIMT threads access Global Memory.
>
> When using the UB, users should calculate the available capacity based on the actual programming scenario and compile options, and ensure that the actually used UB space does not exceed this capacity.
>
> For more compile options, see [Common compile options](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461).

**Bandwidth of different data paths**

| Source (SRC) | Destination (DST) | Pipeline | Bandwidth (Bytes/cycle) |
| --- | --- | --- | --- |
| L1 Buffer | L0A Buffer | PIPE_MTE1 | 256 |
| L1 Buffer | L0B Buffer | PIPE_MTE1 | 256 |
| L1 Buffer | UB | PIPE_MTE1 | 128 |
| UB | L1 Buffer | PIPE_MTE3 | 128 |
| L0C Buffer | UB | PIPE_FIX | 128 |
| L0C Buffer | L1 Buffer | PIPE_FIX | 128 |

**Recommended data layout formats for each storage unit**

-   The following fractal formats are recommended for L0A Buffer, L0B Buffer, and L0C Buffer respectively:

    -   L0A Buffer: FRACTAL\_NZ (due to hardware structure changes, the fractal of L0A Buffer is changed to NZ in this architecture)
    -   L0B Buffer: FRACTAL\_ZN
    -   L0C Buffer: FRACTAL\_NZ

    These formats are optimized for compute-intensive tasks such as cube multiplication, which can significantly improve computation efficiency.

-   For the L1 Buffer cache, the FRACTAL\_NZ format is recommended. When the L1 Buffer uses the NZ format, the format conversion overhead can be reduced when data is moved to the L0A/L0B Buffers (whose format requirements are NZ/ZN respectively).
-   The Unified Buffer has no requirement on the data format.

**Access conflicts of storage units**

The UB structure of this NPU architecture version is shown in the following figure. When multiple operations attempt to access the same bank or bank group of the UB simultaneously, bank conflicts may occur, including read-write conflicts, write-write conflicts, and read-read conflicts. Such conflicts cause access queuing and degrade performance. Compared with NPU architecture version 2201, in this architecture version, the same bank group supports two read operations, or one read operation and one write operation, in a single cycle; it does not support completing two write operations in the same bank group in a single cycle. The related read/write constraints are as follows:

-   **Read-write conflict**: A read operation and a write operation attempt to access the same bank at the same time.
-   **Write-write conflict**: Multiple write operations attempt to access the same bank group at the same time.
-   **Read-read conflict**: Two read operations attempt to access the same bank at the same time, or more than two read operations attempt to access the same bank group at the same time.

**Figure 3**  UB bank diagram of this architecture version<a name="fig59155716511"></a>  
![](../../../../figures/ub_bank.png "UB bank diagram of this architecture version")

**Register**

-   RegTensor

    RegTensor is used to store Reg vector computation data. The bit width of RegTensor is Vector Length (VL) (256 bytes).

-   MaskReg

    MaskReg is used to indicate which elements participate in the computation. Its width is one-eighth of RegTensor (VL/8).

-   UnalignRegForLoad & UnalignRegForStore

    UnalignRegForLoad and UnalignRegForStore are used as buffers to optimize the overhead of consecutive unaligned address access between UB and RegTensor. Before reading an unaligned address, UnalignRegForLoad and UnalignRegForStore should be initialized through the LoadUnAlignPre API, and then the LoadUnAlign API is used. When writing an unaligned address, use the StoreUnAlign API first, and then use the StoreUnAlignPost API for post-processing.

-   AddrReg

    AddrReg is short for Address Register, a register used to store address offsets. AddrReg should be initialized through [CreateAddrReg](../../../../../api/SIMD-API/basic_api/reg_vector_compute/register_data_types/AddrReg.md), and then used within a loop to store address offsets. AddrReg increments by the configured stride in each loop iteration.

## Data Movement Unit<a name="section1073952984611"></a>

**Alignment requirements for data movement**

Because the moved data is used for data computation, there are requirements on the size of the moved data. The size of data moved to the UB must be aligned by DataBlock, and data moved to other storage units must follow the [fractal requirement](../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md). For example, when data is moved from the L1 Buffer to the L0A Buffer, the data format needs to be converted from NZ to ZN format, and the size of the moved data must be aligned by the fractal size. If the remaining size of the L1 Buffer is less than one fractal, an exception occurs during hardware execution.

**MTE hard channel**

-   AIV adds a hard channel between the UB and the L1 Buffer.
-   Adds support for the Loop mode for GM-to-UB and UB-to-GM moves. In Loop mode, each loop can be in Normal mode or Compact mode. For details about Normal mode and Compact mode, see [DataCopyPad\(ISASI\)](../../../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopyPad_GMToUB.md).
    -   A single loop moves data in Normal mode.

        If the length of a single data block is 32B-aligned, no padding needs to be inserted, and multiple data blocks can be moved through multiple repeats.

        ![](../../../../figures/loop_normal.png)

        If the length of a single data block is not 32B-aligned, padding must be inserted after each data block to make it 32B-aligned before movement.

        ![](../../../../figures/forloop.png)

    -   A single loop moves data in Compact mode.

        In Compact mode, a group of data blocks can be moved at a time. When the total length of these data blocks is 32B-aligned, no padding needs to be inserted at the end.

        ![](../../../../figures/loop_normal_19.png)

        When the length of a group of data blocks is not 32B-aligned, padding needs to be inserted after this group of data blocks so that the total length is 32B-aligned.

        ![](../../../../figures/loop_comact.png)

**Support for Fixpipe hardware acceleration**

Fixpipe is an acceleration module in the NPU that hardens typical operations. It is located inside the AIC and works with the Cube compute unit to complete inline computation. Its main functions are as follows:

-   Quantization and dequantization: including S4/S8/S32/FP16/FP32/FP8\_E4M3/HiF8/BF16.
-   ReLU functions, including typical activation functions such as ReLU, PReLU, and Leaky ReLU.
-   Data format conversion, including:
    -   Channel merge and channel split can convert the fractal size to ensure that the fractal output to the L1 Buffer/GM meets the requirement.
    -   Inline NZ2ND and NZ2DN conversion of the L0C source is supported.

Channel merge supports the S8, U8, S4, and U4 data types, while channel split supports the FP32 data type.

-   Channel merge (S8 and U8 data types)

    For conversion to the S8 or U8 target data type, the fractal cube is converted from 16x16 to 16x32 by hardware. If the number of output channels N is an even multiple of 16, every two adjacent 16x16 fractal cubes in the N direction are merged into one 16x32 fractal cube. If N is an odd multiple of 16, channels 1 to (N–16) are merged, and the last 16 channels remain unmerged.

    As shown below, the target data type is S8, M is 32, and N is 48. First, the first two columns of 16x16 fractal cubes are merged into one 16x32 cube, and then the remaining 16x16 fractal cubes are directly moved into the L1 Buffer.

    ![](../../../../figures/fixpipe_i8_1.png)

-   Channel merge (S4 and U4 data types):

    For conversion to the S4 or U4 target data type, the fractal cube is converted from 16x16 to 16x64 by hardware. If the number of output channels N is a multiple of 64, every four adjacent 16x16 fractal cubes in the N direction are merged into one single 16x64 fractal cube.

    For example, here the target data type is S4, M is 32, and N is 64. First, the 16x16 fractal cubes in row 1 are merged into one 16x64 cube, and then the 16x16 fractal cubes in row 2 are also merged.

    In this case, N must be configured as a multiple of 64.

    ![](../../../../figures/fixpipe_i4_1.png)

-   FP32 Channel split:

    For the target type FP32, the fractal cube can be converted from 16x16 to 16x8 by hardware. If Channel split is enabled, each 16x16 fractal cube is split into two 16x8 fractal cubes.

    As shown in the following figure, here the target data type is FP32, M is 64, and N is 32. It is split into 16 16x8 fractals.

    ![](../../../../figures/fixpipe_f32_1.png)

## Instruction Sequence Depth<a name="section1073952984688"></a>

The following table lists the instruction queue depths of the compute units and move units:

| Instruction Sequence | Queue Depth in AIC Core | Queue Depth in AIV Core |
| --- | --- | --- |
| Vector instruction sequence | 0 | 32 |
| Cube instruction sequence | 16 | 0 |
| MTE1 instruction sequence | 32 | 0 |
| MTE2 instruction sequence | 16 | 16 |
| MTE3 instruction sequence | 0 | 16 |
| FixPipe instruction sequence | 32 | 0 |

## AIC-AIV Inter-Core Communication<a name="section1965819584325"></a>

This architecture supports AIC:AIV inter-core communication at ratios of 1:1 and 1:2. Inter-core communication is performed through SSBuf, which differs from the NPU220 architecture, where inter-core communication is completed through GM.

![](../../../../figures/fig_1b2.png)

## Synchronization Control<a name="section41753664816"></a>

-   Intra-core synchronization

    Because the execution units inside the AI Core (such as the MTE2 move unit and the Vector compute unit) run in an asynchronous and parallel manner, data dependencies may exist when reading from and writing to Local Memory (such as the UB). To ensure data consistency and computation correctness, synchronization control is required to coordinate the operation timing.

    Take the process in which MTE2 moves data from GM to the UB, the Vector compute unit performs the Abs computation, and then the data is moved back to GM as an example. The following synchronization conditions must be met:

    1.  Data movement and computation order
        -   Start the Abs computation of the Vector unit only after the GM→UB data movement is complete (to avoid data loss caused by starting the computation before the movement is complete);
        -   Perform the UB→GM data movement only after the Vector computation is complete (to ensure that the result data is ready).

    2.  Synchronization rules for loop-based data movement and computation scenarios
        -   Start new data movement only after the preceding computation is complete: when the previous computation is not complete, new data movement must not be triggered (to prevent old data in the UB from being overwritten);
        -   Start new computation only after the preceding data has been moved out: when the previous data has not been completely moved out of the UB, new computation tasks must not be triggered (to avoid overwrite conflicts in the target memory area).

    The synchronization control process is shown in the following figure:

    ![](../../../../figures/core_sync_final_23.png)

    In the preceding figure, ID1, ID2, ID3, ID4, ID5, and ID6 represent event IDs (EventIDs). Each EventID corresponds to the movement status of a block of stored data, ensuring the correctness and consistency of data operations.

    Note the following points:

    -   It is recommended that you obtain the EventID through the [AllocEventID](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/AllocEventID.md) or [FetchEventID](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/FetchEventID.md) API to ensure its validity and effectiveness.
    -   The number of EventIDs is limited. After use, call ReleaseEventID immediately to release the resource to avoid EventID exhaustion, which may affect normal system running.
    -   SetFlag and WaitFlag must be used in pairs, and their parameters (including template parameters and event IDs) must be exactly the same. If they do not match, computation on the current core may become abnormal, or operator execution on the next core may be affected, causing a timeout issue.

        For example, `SetFlag<HardEvent::S_MTE3>(1)` and `SetFlag<HardEvent::MTE3_MTE1>(1)` do not set the same EventID because their template parameters are different. Only when both the template parameters and the event ID are exactly the same do they represent the same EventID.

    -   Do not set the same EventID consecutively, because this may cause the event status to become confused or fail to be processed correctly.
    -   It is not recommended to manually insert a TEventID. Do not manually insert TEventIDs 6 and 7, because they may be reserved by the system or used for special purposes.

-   Inter-core synchronization

    When different cores operate on the same global memory, data dependency issues such as read-after-write, write-after-read, and write-after-write may occur, requiring inter-core synchronization control.

    Inter-core synchronization control is divided into the following modes, as shown in the following figure:

    -   Mode 0: Synchronization control between AI Core cores. For the AIC scenario, all AIC cores are synchronized. The instructions following CrossCoreWaitFlag are executed only after all AIC cores have executed CrossCoreSetFlag. For the AIV scenario, all AIV cores are synchronized. The instructions following CrossCoreWaitFlag are executed only after all AIV cores have executed CrossCoreSetFlag.
    -   Mode 1: Synchronization control between AIV cores within an AI Core. The instructions following CrossCoreWaitFlag are executed only after both AIV cores have executed CrossCoreSetFlag.
    -   Mode 2: Synchronization control between AIC and AIV within an AI Core (1:2). After the AIC core executes CrossCoreSetFlag, the instructions following CrossCoreWaitFlag on the two AIVs continue to execute. After both AIVs execute CrossCoreSetFlag, the instructions following CrossCoreWaitFlag on the AIC can be executed.
    -   Mode 4: Synchronization control between AIC and AIV within an AI Core (1:1). AIV0 and AIV1 can independently trigger the AIC to wait. For example, after the AIC core executes CrossCoreSetFlag, the instructions following CrossCoreWaitFlag on AIV0 continue to execute. After AIV0 executes CrossCoreSetFlag, the instructions following CrossCoreWaitFlag on the AIC can be executed.

    ![](../../../../figures/inter_core_sync.png)

    The following uses mode 2 as an example to describe the synchronization process between the AIC and the AIVs within the same AI Core: After the AIC moves the computation result in L0C to the GM, it calls `CrossCoreSetFlag<0x2, PIPE_FIX>(ID1)`. The two AIVs wait for the synchronization flag through `CrossCoreWaitFlag<0x2, PIPE_MTE2>(ID1)` and then move the data from the GM to the UB. Here, `0x2` indicates mode 2, `PIPE_FIX` and `PIPE_MTE2` indicate the pipeline types, and `ID1` is the synchronization flag ID. After the instructions in the PIPE_FIX pipeline before CrossCoreSetFlag are completed, the instructions following CrossCoreWaitFlag in the MTE2 pipeline can be executed, while other pipelines are not blocked.

    ![](../../../../figures/inter_sync_1.png)

    [CrossCoreSetFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md) are used together. When using them, you need to pass the inter-core synchronization flag ID\(flagId\), that is, ID1 in the preceding figure. Each ID corresponds to a counter with an initial value of 0. After CrossCoreSetFlag is executed, the counter corresponding to the ID increases by 1. When CrossCoreWaitFlag is executed, if the corresponding counter value is 0, the execution is blocked. If the corresponding counter is greater than 0, the counter decreases by 1 and subsequent instructions start to execute. For the flagId value range, see [flagId value range description](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md#flagid-value-range-description).

    Note the following points:

    -   **Pairing and Consistency Requirements**

        CrossCoreSetFlag and CrossCoreWaitFlag must be used in pairs. Otherwise, operator timeout may occur. The parameter consistency requirements of the two APIs are as follows:

        - modeId: It is recommended that CrossCoreWaitFlag explicitly configure the same modeId as CrossCoreSetFlag, so as to confirm that the synchronization modes of the two APIs match.
        - pipe: The pipe of CrossCoreSetFlag indicates the pipeline that must be completed before this API is called, and the pipe of CrossCoreWaitFlag indicates the pipeline to be blocked. They do not need to be consistent.
        - flagId: In mode 0, mode 1, and mode 2, CrossCoreSetFlag and CrossCoreWaitFlag must use the same flagId. In mode 4, the flagIds of the two APIs must comply with the corresponding relationship.

    -   **Avoiding consecutive setting**

        Consecutive setting of the same flagId is not allowed, to prevent counter state confusion.

    -   **flagId Conflicts with Other APIs**

        Some APIs use CrossCoreSetFlag and CrossCoreWaitFlag in their internal implementation, which occupies some flagIds. When developers use these APIs together with CrossCoreSetFlag/CrossCoreWaitFlag, they must avoid flagId usage conflicts. For the complete description of flagId occupation by each API (including differences across NPU architecture versions), see [inter-core synchronization flagId usage description](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/key_features.md#inter_core_sync_flagid_usage).

    -   **Counter limit**

        The counter of the same flagId can be set up to 15 times.

    -   **Default value**

        - modeId: The default value is 0. It is recommended to explicitly configure the same modeId as CrossCoreSetFlag.
        - pipe: The default value is PIPE\_S, which blocks the subsequent instructions of the PIPE\_S pipeline. To block other pipelines, you must explicitly configure the corresponding pipe.
