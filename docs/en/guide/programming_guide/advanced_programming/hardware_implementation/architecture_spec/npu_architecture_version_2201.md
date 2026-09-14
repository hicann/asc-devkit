# NPU Architecture Version 2201<a name="ZH-CN_TOPIC_0000002380477949"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-26T11:42:17.708Z pushedAt=2026-09-01T11:15:30.653Z -->

This section describes the hardware architecture and functional description of \_\_NPU\_\_ARCH\_\_ version 2201. The corresponding product models are:

-   Atlas A3 training products/Atlas A3 inference products
-   Atlas A2 training products/Atlas A2 inference products

## Hardware Architecture<a name="section2189135013710"></a>

As shown in the following figure, in this architecture the AI Core is divided into two independent cores, AIC and AIV, which are used for cube computation and vector computation, respectively. Each core has its own Scalar unit and can independently load its own code segment. Data is transferred between AIV and AIC through Global Memory.

![](../../../../figures/separated_architecture_12.png)

## Compute Unit<a name="section8490181519513"></a>

**Separate Deployment of Cube Compute Units and Vector Compute Units**

In this architecture, Cube compute units and Vector compute units are deployed on AIC cores and AIV cores respectively. Each core has its own Scalar unit and can independently load its own code segment.

**Vector Compute Unit**

-   The data of the Vector compute unit comes from the Unified Buffer and requires 32-byte alignment.

**Cube Compute Unit**

-   The storage units accessible to the Cube compute unit are the L0A Buffer, L0B Buffer, and L0C Buffer. The L0A Buffer stores the left cube, the L0B Buffer stores the right cube, and the L0C Buffer stores the results and intermediate results of cube multiplication.

## Storage Units<a name="section6500173264510"></a>

**Obtaining the Memory Size of Storage Units**

Developers can query the memory size of each storage unit through the [platform information](../../../../../api/Utils-API/platform_info/platform_information.md) API.

**Minimum Access Granularity (Alignment Requirement) of Each Storage Unit**

<a name="table12348145512210"></a>
<table><thead align="left"><tr id="row3348175522218"><th class="cellrowborder" valign="top" width="7.5200000000000005%" id="mcps1.1.4.1.1"><p id="p1680716381143"><a name="p1680716381143"></a><a name="p1680716381143"></a>Core</p></th>
<th class="cellrowborder" valign="top" width="46.22%" id="mcps1.1.4.1.2"><p id="p5348145515229"><a name="p5348145515229"></a><a name="p5348145515229"></a>Storage Unit</p></th>
<th class="cellrowborder" valign="top" width="46.26%" id="mcps1.1.4.1.3"><p id="p16348125520221"><a name="p16348125520221"></a><a name="p16348125520221"></a>Alignment Requirement</p></th>
</tr>
</thead>
<tbody><tr id="row11348135516221"><td class="cellrowborder" valign="top" width="7.5200000000000005%" headers="mcps1.1.4.1.1 "><p id="p48071138245"><a name="p48071138245"></a><a name="p48071138245"></a>AIV</p></td>
<td class="cellrowborder" valign="top" width="46.22%" headers="mcps1.1.4.1.2 "><p id="p63481755122216"><a name="p63481755122216"></a><a name="p63481755122216"></a><span id="ph15348135502220"><a name="ph15348135502220"></a><a name="ph15348135502220"></a>Unified Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="46.26%" headers="mcps1.1.4.1.3 "><p id="p1634825512226"><a name="p1634825512226"></a><a name="p1634825512226"></a>32-byte alignment.</p></td>
</tr>
<tr id="row1534865520225"><td class="cellrowborder" rowspan="6" valign="top" width="7.5200000000000005%" headers="mcps1.1.4.1.1 "><p id="p567812161851"><a name="p567812161851"></a><a name="p567812161851"></a>AIC</p></td>
<td class="cellrowborder" valign="top" width="46.22%" headers="mcps1.1.4.1.2 "><p id="p7349175582217"><a name="p7349175582217"></a><a name="p7349175582217"></a><span id="ph1234918559222"><a name="ph1234918559222"></a><a name="ph1234918559222"></a>L1 Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="46.26%" headers="mcps1.1.4.1.3 "><p id="p2034975519227"><a name="p2034975519227"></a><a name="p2034975519227"></a>32-byte alignment.</p></td>
</tr>
<tr id="row14349855162220"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="p2349175522213"><a name="p2349175522213"></a><a name="p2349175522213"></a><span id="ph12349175572211"><a name="ph12349175572211"></a><a name="ph12349175572211"></a>L0A Buffer</span></p></td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="p12349105510223"><a name="p12349105510223"></a><a name="p12349105510223"></a>512-byte alignment.</p></td>
</tr>
<tr id="row83498556224"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="p143491455162210"><a name="p143491455162210"></a><a name="p143491455162210"></a><span id="ph5349135515225"><a name="ph5349135515225"></a><a name="ph5349135515225"></a>L0B Buffer</span></p></td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="p234945516225"><a name="p234945516225"></a><a name="p234945516225"></a>512-byte alignment.</p></td>
</tr>
<tr id="row13491655162217"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="p43494555226"><a name="p43494555226"></a><a name="p43494555226"></a><span id="ph17349155514229"><a name="ph17349155514229"></a><a name="ph17349155514229"></a>L0C Buffer</span></p></td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="p14349125512220"><a name="p14349125512220"></a><a name="p14349125512220"></a>64-byte alignment.</p></td>
</tr>
<tr id="row15518316118"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="p13625119517"><a name="p13625119517"></a><a name="p13625119517"></a>BiasTable Buffer</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="p1462519119"><a name="p1462519119"></a><a name="p1462519119"></a>64-byte alignment.</p></td>
</tr>
<tr id="row16156956113"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="p126251896119"><a name="p126251896119"></a><a name="p126251896119"></a>Fixpipe Buffer</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="p2229125511616"><a name="p2229125511616"></a><a name="p2229125511616"></a>64-byte alignment.</p></td>
</tr>
</tbody>
</table>

**Recommended Data Layout Formats for Each Storage Unit**

-   The L0A Buffer, L0B Buffer, and L0C Buffer are recommended to use the following fractal formats respectively:

    -   L0A Buffer: FRACTAL\_ZZ
    -   L0B Buffer: FRACTAL\_ZN
    -   L0C Buffer: FRACTAL\_NZ  
    These formats are optimized for compute-intensive tasks such as cube multiplication and can significantly improve computation efficiency.

-   It is recommended that the L1 Buffer cache use the FRACTAL\_NZ format. When the L1 Buffer uses the NZ format, the format conversion overhead can be reduced when data is moved to the L0A/L0B Buffer (which must be converted to the ZZ and ZN formats, respectively).
-   The Unified Buffer has no data format requirement.

**Resolving Access Conflicts in the Storage Unit to Improve Read/Write Performance**

When multiple operations attempt to access the same bank or bank group of the Unified Buffer at the same time, bank conflicts may occur, including read/write conflicts, write/write conflicts, and read/read conflicts. Such conflicts cause access queuing and degrade performance. You can improve read/write performance by optimizing bank allocation. For details, see [Avoiding UB Bank Conflicts](../../../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_ub_bank_conflict.md).

## Data Movement Unit<a name="section1073952984611"></a>

**Alignment Requirements for Data Movement**

Because the moved data is used for computation, there are requirements on the size of the moved data. Data moved to the Unified Buffer must be aligned by DataBlock, and data movement to other storage units must follow the [fractal requirements](../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md). For example, when data is moved from the L1 Buffer to the L0A Buffer, the data format must be converted from NZ to ZZ, and the size of the moved data must be aligned by the fractal size. If the remaining size of the L1 Buffer is less than one fractal, an exception occurs during hardware execution.

**Support for Cross-Device Data Movement (HCCS Physical Link)**

In cross-device communication operator development scenarios, the DataCopy APIs support cross-device data movement. On Atlas A2 training products/Atlas A2 inference products, only the HCCS physical link is supported, and no other paths are supported. During development, pay attention to the physical path used for inter-device communication. Run the **npu-smi info -t topo** command to query the HCCS physical path.

**Support for Fixpipe Hardware Acceleration**

Fixpipe is an acceleration module in the NPU that hardens typical operations. It is located inside the AIC and works with the Cube compute unit to perform in-line computation. Its main functions are as follows:

-   Quantization and dequantization, including S322FP16, S322S32, S322S4, S322S8, S322S16, FP322FP16, FP322BF16, FP322S8, FP322S4, and FP322FP32.
-   ReLU functions, including typical activation functions such as ReLU, PReLU, and Leaky ReLU.
-   Data format conversion, including:
    -   Channel merge and channel split can convert the fractal size to ensure that the fractal output to the L1 Buffer/GM meets the requirement.
    -   NZ2ND data format conversion.

![](../../../../figures/0707_Fixpipe.png)

In the preceding figure, channel merge supports the S8, U8, S4, and U4 data types, while channel split supports the FP32 data type.

-   Channel merge (S8 and U8 data types)

    For conversion to the S8 or U8 target data type, the fractal cube is converted from 16x16 to 16x32 by hardware. If the number of output channels N is an even multiple of 16, every two adjacent 16x16 fractal cubes in the N direction are merged into one 16x32 fractal cube. If N is an odd multiple of 16, channels 1 to (N–16) are merged, and the last 16 channels remain unmerged.

    As shown below, the target data type is S8, M is 32, and N is 48. The first two columns of 16x16 fractal cubes are first merged into one 16x32 cube, and then the remaining 16x16 fractal cubes are directly moved into the L1 Buffer.

    ![](../../../../figures/Fixpipe_int8-channel_merge1.png)

-   Channel merge (S4 and U4 data types)

    For conversion to the S4 or U4 target data type, the fractal matrix is converted from 16x16 to 16x64 by hardware. If the number of output channels N is a multiple of 64, every four adjacent 16x16 fractal cubes in the N direction are merged into one 16x64 fractal cube.

    For example, here the target data type is S4, M is 32, and N is 64. First, the 16x16 fractal cubes in row 1 are merged into one 16x64 cube, and then the 16x16 fractal cubes in row 2 are also merged.

    In this case, N must be configured as a multiple of 64.

    ![](../../../../figures/Fixpipe_int4_channel_merge1.png)

-   FP32 channel split:

    For the target type FP32, the fractal cube can be converted from 16x16 to 16x8 by hardware. If channel split is enabled, each 16x16 fractal cube is split into two 16x8 fractal cubes.

    As shown in the following figure, here the target data type is FP32, M is 64, and N is 32. It is split into 16 16x8 fractals.

    ![](../../../../figures/Fixpipe_F32_channel_split1.png)

## Synchronization Control<a name="section41753664816"></a>

-   Intra-core synchronization

    Because the execution units inside the AI Core (such as the MTE2 data movement unit and the Vector compute unit) run in an asynchronous and parallel manner, data dependencies may exist when reading from and writing to Local Memory (such as the Unified Buffer). To ensure data consistency and computation correctness, synchronization control is required to coordinate the operation timing.

    Take the following process as an example: MTE2 moves data from GM to UB, the Vector compute unit performs the Abs computation, and then the data is moved back to GM. The following synchronization conditions must be met:

    1.  Data movement and computation order
        -   Start the Abs computation of the Vector unit only after the GM→UB movement is complete (to avoid data loss caused by starting the computation before the movement is complete);
        -   Perform the UB→GM data movement only after the Vector computation is complete (to ensure that the result data is ready).

    2.  Synchronization rules for loop movement and computation scenarios
        -   Start a new movement only after the preceding computation is complete: when the previous computation is not complete, new data movement must not be triggered (to prevent the old data in UB from being overwritten);
        -   Start a new computation only after the preceding data movement out is complete: when the previous data has not been fully moved out of UB, a new computation task must not be triggered (to avoid overwrite conflicts in the target memory area).

    The synchronization control process is shown in the following figure:

    ![](../../../../figures/0000000_intra_core_sync_final_13.png)

    In the preceding figure, ID1, ID2, ID3, ID4, ID5, and ID6 represent event IDs. Each event ID corresponds to the movement status of a block of stored data, ensuring the correctness and consistency of data operations.

    Note the following points:

    -   It is recommended that you obtain an event ID through the [AllocEventID](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/AllocEventID.md) or [FetchEventID](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/FetchEventID.md) API to ensure its validity and effectiveness.
    -   The number of event IDs is limited. After use, call ReleaseEventID immediately to release the resource to prevent event ID exhaustion, which may affect normal system running.
    -   SetFlag and WaitFlag must be used in pairs, and their parameters (including template parameters and event IDs) must be exactly the same. If they do not match, computation on the current core may become abnormal, or operator execution on the next core may be affected, causing a timeout issue.

        For example, `SetFlag<HardEvent::S_MTE3>(1)` and `SetFlag<HardEvent::MTE3_MTE1>(1)` do not set the same event ID because their template parameters are different. Only when both the template parameters and the event IDs are exactly the same do they represent the same event ID.

    -   Do not set the same event ID consecutively, because this may cause event state confusion or incorrect processing.
    -   It is not recommended to manually insert TEventIDs. Do not manually insert TEventIDs 6 and 7, because they may be reserved by the system or used for special purposes.

-   Inter-core synchronization

    When different cores operate on the same global memory, data dependency issues such as read-after-write, write-after-read, and write-after-write may occur, requiring inter-core synchronization control.

    Inter-core synchronization control is divided into the following modes, as shown in the following figure:

    -   Mode 0: synchronization control between AI Cores. For the AIC scenario, all AIC cores are synchronized, and the instructions following CrossCoreWaitFlag are executed only after all AIC cores have executed CrossCoreSetFlag. For the AIV scenario, all AIV cores are synchronized, and the instructions following CrossCoreWaitFlag are executed only after all AIV cores have executed CrossCoreSetFlag.
    -   Mode 1: synchronization control between AIV cores within an AI Core. The instructions following CrossCoreWaitFlag are executed only after both AIV cores have executed CrossCoreSetFlag.
    -   Mode 2: synchronization control between AIC and AIV within an AI Core. After the AIC core executes CrossCoreSetFlag, the instructions following CrossCoreWaitFlag on the two AIVs continue to execute. After both AIVs execute CrossCoreSetFlag, the instructions following CrossCoreWaitFlag on the AIC can be executed.

    ![](../../../../figures/figure_1_ai_core_internal_parallel_compute_architecture_abstract_diagram.png)

    For example, after the computation result in L0C is moved to GM in the AIC, the AIV needs to move the data from GM to UB. In this case, the CrossCoreSetFlag and CrossCoreWaitFlag commands can be used to ensure that the data is moved from L0C to GM before being moved from GM to UB. The process is shown in the following figure.

    ![](../../../../figures/new_inter_core_sync_1.png)

    [CrossCoreSetFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md) are used together. When using them, you need to pass in the flag ID (flagId) for inter-core synchronization, that is, ID1 in the preceding figure. Each ID corresponds to a counter with an initial value of 0. After CrossCoreSetFlag is executed, the counter corresponding to the ID increases by 1. When CrossCoreWaitFlag is executed, if the corresponding counter value is 0, execution is blocked. If the corresponding counter value is greater than 0, the counter decreases by 1 and the subsequent instructions start to execute. The value range of flagId is 0 to 10.

    Note the following points:

    -   **Use in pairs**

        CrossCoreSetFlag and CrossCoreWaitFlag must be used in pairs. Otherwise, operator timeout may occur.

    -   **Consistency requirement**

        The template parameters and flagId of CrossCoreSetFlag must be exactly the same as those of CrossCoreWaitFlag. Otherwise, they are regarded as different flagIds. For example, CrossCoreSetFlag<0x0, PIPE\_MTE3\>\(0x8\) and CrossCoreSetFlag<0x2, PIPE\_FIX\>\(0x8\) do not set the same flagId.

    -   **Avoid consecutive setting**

        Consecutive setting of the same flagId is not allowed, to prevent the counter state from becoming disordered.

    -   **Conflict with high-level APIs**

        The Matmul high-level API uses this API for inter-core synchronization control in its internal implementation. Therefore, you are not advised to use this API together with the Matmul high-level API. Otherwise, there is a risk of flagId conflict.

    -   **Counter limit**

        The counter of the same flagId can be set up to 15 times.

    -   **Default pipeline type**

        CrossCoreWaitFlag does not require explicitly setting the pipeline type of the instruction, and PIPE\_S is used by default.
