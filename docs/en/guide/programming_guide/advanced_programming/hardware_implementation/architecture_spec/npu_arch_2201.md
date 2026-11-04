# NPU Architecture Version 2201<a name="ZH-CN_TOPIC_0000002380477949"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T14:18:34.048Z pushedAt=2026-11-03T07:05:13.427Z nl=2 -->

This section describes the hardware architecture and functions of \_\_NPU\_ARCH\_\_ version 2201. The corresponding product models are:

<!-- npu="A3" id1 -->
-   Atlas A3 products
<!-- end id1 -->
<!-- npu="910b" id2 -->
-   Atlas A2 products
<!-- end id2 -->

## Hardware Architecture Diagram<a name="section2189135013710"></a>

As shown in the following figure, in this architecture, the AI Core is divided into two independent cores, AIC and AIV, which are used for cube computation and vector computation, respectively. The ratio of AIC cores to AIV cores is 1:2. Each core has its own Scalar unit and can independently load its own code segment. Data is transferred between AIV and AIC through Global Memory.

![](../../../../figures/sep_arch_12.png)

## Compute Unit<a name="section8490181519513"></a>

**Separate deployment of Cube compute units and Vector compute units**

In this architecture, Cube compute units and Vector compute units are deployed on AIC cores and AIV cores respectively. Each core has its own Scalar unit and can independently load its own code segment.

**Vector compute unit**

-   The data of the Vector compute unit comes from the Unified Buffer (UB) and requires 32-byte alignment.

**Cube compute unit**

-   The storage units accessible to the Cube compute unit are the L0A Buffer, L0B Buffer, and L0C Buffer. The L0A Buffer stores the left cube, the L0B Buffer stores the right cube, and the L0C Buffer stores the results and intermediate results of cube multiplication.

## Storage Units<a name="section6500173264510"></a>

**Obtaining the memory space size of a storage unit**

Developers can query the memory space size of each storage unit through the [platform information query](../../../../../api/Utils-API/platform_info/platform_info.md) API.

**Parameter specifications of each storage unit**

<a name="table12348145512210"></a>

| Storage Unit Name | Storage Space Size | Alignment Requirement | Kernel-Side Constant Name and Size (Unit: Byte) |
| --- | --- | --- | --- |
| Global Memory  | - | 1-byte alignment | - |
| UB | 192KB | 32-byte alignment | ASC_UB_SIZE = 192 * 1024 - 256 |
| L1 Buffer | 512KB | 32-byte alignment | ASC_L1_SIZE = 512 * 1024 - 256 |
| L0A Buffer | 64KB | 512-byte alignment | ASC_L0A_SIZE = 64 * 1024 |
| L0B Buffer | 64KB | 512-byte alignment | ASC_L0B_SIZE = 64 * 1024 |
| L0C Buffer | 128KB | 64-byte alignment | ASC_L0C_SIZE = 128 * 1024 |
| BiasTable Buffer | 1KB | 64-byte alignment | ASC_BT_SIZE = 1 * 1024 |
| Fixpipe Buffer | 2KB | 64-byte alignment | - |

> [!NOTE] Note
>
> The total UB capacity is 192KB. In the Cube and Vector fusion operator development scenario, the Ascend C framework and compiler reserve 256B of space. The space actually available to users must subtract this reserved space, so users can use at most `192KB - 256B` of UB space.
>
> - If the [--cce-disable-asc-reserved-ubuf](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461) option is enabled at compilation, the actual UB space available to users is `192KB - 256B`. In this case, Ascend C no longer reserves the 8KB UB space used to support [some Ascend C APIs](../../../programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#api-scope-for-reservation-usage-of-ub-space), and this space can be used as ordinary UB space. After this option is enabled, the preceding APIs are unavailable, and a compilation error occurs when they are called.
>
> - If the [--cce-disable-asc-reserved-ubuf](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461) option is not enabled at compilation, the actual UB space available to users is `192KB - 256B - 8KB`.
>
> The total L1 Buffer capacity is 512KB. In the Cube and Vector fusion operator development scenario, the Ascend C framework and compiler reserve 256B of space. The space actually available to users must subtract this reserved space, so users can use at most `512KB - 256B` of L1 Buffer space.

**Bandwidth of different data paths**

| Source (SRC) | Destination (DST) | Pipeline | Bandwidth (Bytes/cycle) |
| --- | --- | --- | --- |
| L1 Buffer | L0A Buffer | PIPE_MTE1 | 256 |
| L1 Buffer | L0B Buffer | PIPE_MTE1 | 128 |

**Recommended data layout formats for each storage unit**

-   The following fractal formats are recommended for L0A Buffer, L0B Buffer, and L0C Buffer respectively:

    -   L0A Buffer: FRACTAL\_ZZ
    -   L0B Buffer: FRACTAL\_ZN
    -   L0C Buffer: FRACTAL\_NZ

    These formats are optimized for compute-intensive tasks such as cube multiplication, which can significantly improve computation efficiency.

-   It is recommended that the L1 Buffer cache use the FRACTAL\_NZ format. When the L1 Buffer uses the NZ format, the format conversion overhead can be reduced when data is moved to the L0A/L0B Buffer (which must be converted to the ZZ and ZN formats, respectively).
-   UB has no requirement on the data format.

**Resolving Access Conflicts in Storage Units to Improve Read/Write Performance**

When multiple operations attempt to access the same bank or bank group of UB at the same time, bank conflicts may occur, including read-write conflicts, write-write conflicts, and read-read conflicts. Such conflicts cause access queuing and degrade performance. You can improve read/write performance by optimizing bank allocation. For details, see [Avoiding UB Bank Conflicts](../../../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_ub_bank_conflict.md).

## Data Movement Unit<a name="section1073952984611"></a>

**Alignment requirements for data movement**

Because the moved data is used for data computation, there are requirements on the size of the moved data. The data moved to UB must be aligned by DataBlock, and data movement to other storage units must follow the [fractal requirements](../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md). For example, when data is moved from L1 Buffer to L0A Buffer, the data format must be converted from NZ to ZZ, and the size of the moved data must be aligned by the fractal size. If the remaining size of L1 Buffer is less than one fractal, an exception occurs during hardware execution.

**Supports cross-card data movement (Hccs physical link)**

In cross-card communication operator development scenarios, DataCopy APIs support cross-card data movement. On Atlas A2 products devices, only the Hccs physical link is supported, and no other paths are supported. During development, pay attention to the physical path involved in inter-card communication. Run the `npu-smi  info -t topo` command to query the Hccs physical path.

**Support for Fixpipe hardware acceleration**

Fixpipe is an acceleration module in the NPU that hardens typical operations. It is located inside the AIC and works with the Cube compute unit to complete inline computation. Its main functions are as follows:

-   Quantization and dequantization: including S322FP16, S322S32, S322S4, S322S8, S322S16, FP322FP16, FP322BF16, FP322S8, FP322S4, and FP322FP32.
-   ReLU functions, including typical activation functions such as ReLU, PReLU, and Leaky ReLU.
-   Data format conversion, including:
    -   Channel merge and channel split can convert the fractal size to ensure that the fractal output to the L1 Buffer/GM meets the requirement.
    -   NZ2ND data format conversion.

![](../../../../figures/fixpipe.png)

In the preceding figure, channel merge supports the S8, U8, S4, and U4 data types, while channel split supports the FP32 data type.

-   Channel merge (S8 and U8 data types)

    For conversion to the S8 or U8 target data type, the fractal cube is converted from 16x16 to 16x32 by hardware. If the number of output channels N is an even multiple of 16, every two adjacent 16x16 fractal cubes in the N direction are merged into one 16x32 fractal cube. If N is an odd multiple of 16, channels 1 to (N–16) are merged, and the last 16 channels remain unmerged.

    As shown below, the target data type is S8, M is 32, and N is 48. First, the first two columns of 16x16 fractal cubes are merged into one 16x32 cube, and then the remaining 16x16 fractal cubes are directly moved into the L1 Buffer.

    ![](../../../../figures/fixpipe_i8_1.png)

-   Channel merge (S4 and U4 data types)

    For the target data type converted to S4 or U4, the fractal cube is converted from 16x16 to 16x64 by hardware. If the number of output channels N is a multiple of 64, every four adjacent 16x16 fractal cubes in the N direction are merged into one 16x64 fractal cube.

    For example, here the target data type is S4, M is 32, and N is 64. First, the 16x16 fractal cubes in row 1 are merged into one 16x64 cube, and then the 16x16 fractal cubes in row 2 are also merged.

    In this case, N must be configured as a multiple of 64.

    ![](../../../../figures/fixpipe_i4_1.png)

-   FP32 Channel split:

    For the target type FP32, the fractal cube can be converted from 16x16 to 16x8 by hardware. If Channel split is enabled, each 16x16 fractal cube is split into two 16x8 fractal cubes.

    As shown in the following figure, here the target data type is FP32, M is 64, and N is 32. It is split into 16 16x8 fractals.

    ![](../../../../figures/fixpipe_f32_1.png)

## Instruction Sequence Depth<a name="section1073952984666"></a>

The following table lists the instruction queue depths of the compute units and move units:

| Instruction Sequence | Queue Depth in AIC Core | Queue Depth in AIV Core |
| --- | --- | --- |
| Vector instruction sequence | 0 | 64 |
| Cube instruction sequence | 16 | 0 |
| MTE1 instruction sequence | 32 | 0 |
| MTE2 instruction sequence | 32 | 16 |
| MTE3 instruction sequence | 32 | 16 |
| FixPipe instruction sequence | 32 | 0 |

## Synchronization Control<a name="section41753664816"></a>

-   Intra-core synchronization

    Because the execution units inside the AI Core (such as the MTE2 data movement unit and the Vector compute unit) run in an asynchronous and parallel manner, data dependencies may exist when reading from and writing to Local Memory (such as the UB). To ensure data consistency and computation correctness, synchronization control is required to coordinate the operation timing.

    Take the process in which MTE2 moves data from GM to the UB, the Vector compute unit performs the Abs computation, and then the data is moved back to GM as an example. The following synchronization conditions must be met:

    1.  Data movement and computation order
        -   Start the Abs computation of the Vector unit only after the GM→UB data movement is complete (to avoid data loss caused by starting the computation before the movement is complete);
        -   Perform the UB→GM data movement only after the Vector computation is complete (to ensure that the result data is ready).

    2.  Synchronization rules for loop-based data movement and computation scenarios
        -   Start new data movement only after the preceding computation is complete: when the previous computation is not complete, new data movement must not be triggered (to prevent old data in the UB from being overwritten);
        -   Start new computation only after the preceding data has been moved out: when the previous data has not been completely moved out of the UB, new computation tasks must not be triggered (to avoid overwrite conflicts in the target memory area).

    The synchronization control process is shown in the following figure:

    ![](../../../../figures/core_sync_final_13.png)

    In the preceding figure, ID1, ID2, ID3, ID4, ID5, and ID6 represent event IDs (EventIDs). Each EventID corresponds to the movement status of a block of stored data, ensuring the correctness and consistency of data operations.

    Note the following points:

    -   It is recommended that you obtain the EventID by calling [AllocEventID](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/AllocEventID.md) or [FetchEventID](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/FetchEventID.md) to ensure its validity and effectiveness.
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
    -   Mode 2: Synchronization control between AIC and AIV within an AI Core. After the AIC core executes CrossCoreSetFlag, the instructions following CrossCoreWaitFlag on the two AIVs continue to execute. After both AIVs execute CrossCoreSetFlag, the instructions following CrossCoreWaitFlag on the AIC can be executed.

    ![](../../../../figures/aicore.png)

    The following uses mode 2 as an example to describe the synchronization process between the AIC and the AIVs in the same AI Core: After the AIC moves the computation results from L0C to GM, it calls `CrossCoreSetFlag<0x2, PIPE_FIX>(ID1)`. The two AIVs call `CrossCoreWaitFlag<0x2>(ID1)` to ensure that the data has been successfully moved from L0C to GM, and then move the data from GM to the UB. In this example, `0x2` indicates mode 2, `PIPE_FIX` indicates the pipeline type, and `ID1` is the synchronization flag ID. CrossCoreWaitFlag explicitly passes only the modeId and does not pass the pipe template parameter. After the instructions in the PIPE_FIX pipeline before CrossCoreSetFlag are complete, the instructions in all pipelines following CrossCoreWaitFlag can be executed. For details, see ["Default Values"](#cross_core_wait_flag_default_values).

    ![](../../../../figures/inter_sync_1.png)

    The [CrossCoreSetFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md) APIs are used together. When using them, you need to pass the inter-core synchronization flag ID\(flagId\), that is, ID1 in the preceding figure. Each ID corresponds to a counter with an initial value of 0. After CrossCoreSetFlag is executed, the counter corresponding to the ID increases by 1. When CrossCoreWaitFlag is executed, if the corresponding counter value is 0, execution is blocked; if the corresponding counter is greater than 0, the counter decreases by 1 and subsequent instructions start to execute. For the flagId value range, see [flagId value range description](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md#flagid-value-range-description).

    Note the following points:

    -   **Paired usage and consistency requirements**

        CrossCoreSetFlag and CrossCoreWaitFlag must be used in pairs; otherwise, operator timeout may occur. The parameter consistency requirements for the two APIs are as follows:

        - modeId: The hardware instructions of this architecture version do not use the modeId of CrossCoreWaitFlag. However, it is recommended that you explicitly configure the same modeId as CrossCoreSetFlag to confirm that the synchronization modes of the two APIs match.
        - pipe: The pipe of CrossCoreWaitFlag does not take effect and does not need to be consistent with the pipe of CrossCoreSetFlag.
        - flagId: CrossCoreSetFlag and CrossCoreWaitFlag must use the same flagId.

    -   **Avoiding consecutive setting**

        Consecutive setting of the same flagId is not allowed, to prevent counter state confusion.

    -   **flagId conflicts with other APIs**

        Some APIs use CrossCoreSetFlag and CrossCoreWaitFlag in their internal implementation and occupy some flagIds. When developers use these APIs together with CrossCoreSetFlag/CrossCoreWaitFlag, they need to avoid flagId usage conflicts. For a complete description of the flagIds occupied by each API (including differences across NPU architecture versions), see [inter-core synchronization flagId usage description](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/key_features.md#inter_core_sync_flagid_usage).

    -   **Counter limit**

        The counter of the same flagId can be set up to 15 times.

    <a id="cross_core_wait_flag_default_values"></a>

    -   **Default value**

        - modeId: The default value is 0. It is recommended that you explicitly configure the same modeId as CrossCoreSetFlag.
        - pipe: The default value is PIPE\_S. The CrossCoreWaitFlag API provides the pipe template parameter, but the hardware instructions of this architecture version do not distinguish the pipeline to wait for. Therefore, the value of pipe does not affect the actual execution result, and CrossCoreWaitFlag blocks subsequent instructions of all pipelines.
