# NPU Architecture Version 3002<a name="ZH-CN_TOPIC_0000002346837454"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-26T11:42:33.095Z pushedAt=2026-09-01T11:43:28.515Z -->

This section describes the hardware architecture and functions of \_\_NPU\_\_ARCH\_\_ version 3002. The corresponding product model is:

-   Atlas 200I/500 A2 inference product

## Hardware Architecture Diagram<a name="section129231322122015"></a>

![](../../../../figures/new_1911_tight_coupling.png)

## Compute Unit<a name="section8171163594515"></a>

**Cube Compute Unit and Vector Compute Unit Deployed on the Same Core**

In this architecture, the Cube compute unit and the Vector compute unit are deployed on the same core and share the same Scalar compute unit.

**Vector Compute Unit**

-   The data source of the Vector compute unit comes from the Unified Buffer and requires 32-byte alignment.

**Cube Compute Unit**

-   The storage units accessible to the Cube compute unit are the L0A Buffer, L0B Buffer, and L0C Buffer. The L0A Buffer stores the left cube, the L0B Buffer stores the right cube, and the L0C Buffer stores the results and intermediate results of cube multiplication.

## Storage Units<a name="section319585512210"></a>

**Obtaining the Memory Space Size of Each Storage Unit**

Developers can query the memory space size of each storage unit through the [platform information acquisition](../../../../../api/Utils-API/platform_info/platform_information.md) API.

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
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="p5364111320919"><a name="p5364111320919"></a><a name="p5364111320919"></a>64-byte alignment.</p></td>
</tr>
</tbody>
</table>

**Recommended Data Layout Formats for Each Storage Unit**

-   The following fractal formats are recommended for the L0A Buffer, L0B Buffer, and L0C Buffer respectively:

    -   L0A Buffer: FRACTAL\_ZZ
    -   L0B Buffer: FRACTAL\_ZN
    -   L0C Buffer: FRACTAL\_NZ

    These formats are optimized for compute-intensive tasks such as cube multiplication and can significantly improve compute efficiency.

-   It is recommended that the L1 Buffer cache use the FRACTAL\_NZ format. When the L1 Buffer uses the NZ format, the format conversion overhead can be reduced when data is moved to the L0A/L0B Buffer (which must be converted to the ZZ and ZN formats, respectively).
-   The Unified Buffer has no requirement on the data format.

**Resolving Access Conflicts in Storage Units to Improve Read/Write Performance**

When multiple operations attempt to access the same bank or bank group of the Unified Buffer at the same time, bank conflicts may occur, including read/write conflicts, write/write conflicts, and read/read conflicts. Such conflicts cause access queuing and degrade performance. You can improve read/write performance by optimizing bank allocation. For details, see [Avoiding UB Bank Conflicts](../../../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_ub_bank_conflict.md).

## Data Movement Unit<a name="section2819205515273"></a>

**Alignment Requirements for Data Movement**

Because the moved data is used for computation, there are requirements on the size of the moved data. The size of data moved to the Unified Buffer must be aligned with DataBlock, and data movement for other storage units must follow the [fractal requirements](../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md). For example, when data is moved from the L1 Buffer to the L0A Buffer, the data format must be converted from NZ to ZZ, and the size of the moved data must be aligned with the fractal size. If the remaining size of the L1 Buffer is less than one fractal, an exception occurs during hardware execution.

**Fixpipe Hardware Acceleration Support**

Fixpipe is an acceleration module in the NPU that hardens typical operations. It is located inside the AIC and works with the Cube compute unit to complete in-line computation. Its main functions are as follows:

-   Quantization and dequantization: including S322FP16, S322S32, S322S4, S322S8, S322S16, FP322FP16, FP322BF16, FP322S8, FP322S4, and FP322FP32.
-   ReLU functions, including typical activation functions such as ReLU, PReLU, and Leaky ReLU.
-   Data format conversion, including:
    -   Channel merge and channel split can convert the fractal size to ensure that the fractal output to the L1 Buffer/GM meets the requirements.
    -   NZ2ND data format conversion.

![](../../../../figures/0707_Fixpipe-14.png)

In the preceding figure, channel merge supports the S8, U8, S4, and U4 data types, while channel split supports the FP32 data type.

-   Channel merge (S8 and U8 data types)

    For the target data type converted to S8 or U8, the fractal cube is converted from 16x16 to 16x32 by hardware. If the number of output channels N is an even multiple of 16, every two adjacent 16x16 fractal cubes in the N direction are merged into one 16x32 fractal cube. If N is an odd multiple of 16, channels 1 to (N–16) are merged, and the last 16 channels remain unmerged.

    As shown below, the target data type is S8, M is 32, and N is 48. The first two columns of 16x16 fractal cubes are first merged into one 16x32 cube, and then the remaining 16x16 fractal cube is directly moved into the L1 Buffer.

    ![](../../../../figures/Fixpipe_int8-channel_merge1.png)

-   Channel merge (S4 and U4 data types)

    For the target data type converted to S4 or U4, the fractal cube is converted from 16x16 to 16x64 by hardware. If the number of output channels N is a multiple of 64, every four adjacent 16x16 fractal cubes in the N direction are merged into one 16x64 fractal cube.

    For example, here the target data type is S4, M is 32, and N is 64. The 16x16 fractal cubes in the first row are first merged into one 16x64 cube, and then the 16x16 fractal cubes in the second row are also merged.

    In this case, the configuration of N must be a multiple of 64.

    ![](../../../../figures/Fixpipe_int4_channel_merge1.png)

-   FP32 channel split:

    For the target type FP32, the fractal cube can be converted from 16x16 to 16x8 by hardware. If channel split is enabled, each 16x16 fractal cube is split into two 16x8 fractal cubes.

    As shown in the following figure, the target data type here is FP32, M is 64, and N is 32. It is split into 16 16x8 fractals.

    ![](../../../../figures/Fixpipe_F32_channel_split1.png)

## Synchronization Control<a name="section15421223142815"></a>

-   Intra-core synchronization

    Because the execution units inside the AI Core (such as the MTE2 data movement unit and the Vector compute unit) run asynchronously and in parallel, data dependencies may exist when reading from and writing to Local Memory (such as the Unified Buffer). To ensure data consistency and computation correctness, synchronization control is required to coordinate the operation timing.

    Take the following process as an example: MTE2 moves data from GM to UB, the Vector compute unit performs the Abs computation, and then the data is moved back to GM. The following synchronization conditions must be met:

    1.  Data movement and computation order
        -   Start the Abs computation of the Vector unit only after the GM-to-UB movement is complete (to avoid data loss caused by incomplete movement when the computation starts);
        -   Perform the UB-to-GM data transfer only after the Vector computation is complete (to ensure that the result data is ready).

    2.  Synchronization rules for cyclic movement and computation scenarios
        -   Start a new movement only after the preceding computation is complete: when the previous computation is not complete, a new data movement must not be triggered (to prevent old data in the UB from being overwritten);
        -   Start a new computation only after the preceding data copy-out is complete: when the previous data has not been fully copied out of the UB, a new computation task must not be triggered (to avoid overwrite conflicts in the target memory area).

    The synchronization control process is shown in the following figure:

    ![](../../../../figures/0000000_intra_core_sync_final_18.png)

    In the preceding figure, ID1, ID2, ID3, ID4, ID5, and ID6 represent event IDs. Each event ID corresponds to the data movement status of a storage block, ensuring the correctness and consistency of data operations.

    Note the following points:

    -   You are advised to obtain an event ID by calling [AllocEventID](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/AllocEventID.md) or [FetchEventID](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/FetchEventID.md) to ensure its validity and effectiveness.
    -   The number of event IDs is limited. After use, call ReleaseEventID immediately to release the resource to prevent event ID exhaustion, which may affect normal system running.
    -   SetFlag and WaitFlag must be used in pairs, and their parameters (including template parameters and event IDs) must be exactly the same. Otherwise, the computation on the current core may become abnormal, or the operator execution on the next core may be affected, causing a timeout issue.

        For example, `SetFlag<HardEvent::S_MTE3>(1)` and `SetFlag<HardEvent::MTE3_MTE1>(1)` do not set the same event ID because their template parameters are different. Only when both the template parameters and the event ID are exactly the same do they represent the same event ID.

    -   Setting the same event ID consecutively is not allowed, because this may cause the event status to become confused or fail to be processed correctly.
    -   Manually inserting a TEventID is not recommended. Do not manually insert TEventIDs 6 and 7, because they may be reserved by the system or used for special purposes.

-   Inter-core synchronization

    This hardware architecture does not support inter-core synchronization.
