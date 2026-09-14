# NPU Architecture Version 2002<a name="ZH-CN_TOPIC_0000002346677682"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T11:40:18.399Z pushedAt=2026-09-01T09:27:20.543Z -->

This section describes the hardware architecture of \_\_NPU\_ARCH\_\_ version 2002 and its functional description. The corresponding product model is the Atlas inference products.

## Hardware Architecture Diagram<a name="section166031113713"></a>

![](../../../../figures/npu_architecture_2002.png)

## Compute Unit<a name="section843933313481"></a>

**Cube Compute Unit and Vector Compute Unit Deployed on the Same Core**

In this architecture, the Cube compute unit and the Vector compute unit are deployed on the same core and share the same Scalar compute unit.

**Vector Compute Unit**

-   The data source of the Vector compute unit is the Unified Buffer, which requires 32-byte alignment.
-   Data transferred from the L0C Buffer to the Unified Buffer must be relayed through the Vector compute unit.

**Cube Compute Unit**

-   The storage units accessible to the Cube compute unit are the L0A Buffer, L0B Buffer, and L0C Buffer. The L0A Buffer stores the left matrix, the L0B Buffer stores the right matrix, and the L0C Buffer stores the results and intermediate results of matrix multiplication.

## Storage Unit<a name="section9689958161012"></a>

**Obtaining the Memory Size of a Storage Unit**

Developers can query the memory size of each storage unit through the [platform information](../../../../../api/Utils-API/platform_info/platform_information.md) API.

**Minimum Access Granularity (Alignment Requirement) of Each Storage Unit**

<a name="table12348145512210"></a>
<table><thead align="left"><tr id="row3348175522218"><th class="cellrowborder" valign="top" width="49.980000000000004%" id="mcps1.1.3.1.1"><p id="p5348145515229"><a name="p5348145515229"></a><a name="p5348145515229"></a>Storage Unit</p></th>
<th class="cellrowborder" valign="top" width="50.019999999999996%" id="mcps1.1.3.1.2"><p id="p16348125520221"><a name="p16348125520221"></a><a name="p16348125520221"></a>Alignment Requirement</p></th>
</tr>
</thead>
<tbody><tr id="row11348135516221"><td class="cellrowborder" valign="top" width="49.980000000000004%" headers="mcps1.1.3.1.1 "><p id="p63481755122216"><a name="p63481755122216"></a><a name="p63481755122216"></a><span id="ph15348135502220"><a name="ph15348135502220"></a><a name="ph15348135502220"></a>Unified Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="50.019999999999996%" headers="mcps1.1.3.1.2 "><p id="p1634825512226"><a name="p1634825512226"></a><a name="p1634825512226"></a>32-byte alignment.</p></td>
</tr>
<tr id="row1534865520225"><td class="cellrowborder" valign="top" width="49.980000000000004%" headers="mcps1.1.3.1.1 "><p id="p7349175582217"><a name="p7349175582217"></a><a name="p7349175582217"></a><span id="ph1234918559222"><a name="ph1234918559222"></a><a name="ph1234918559222"></a>L1 Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="50.019999999999996%" headers="mcps1.1.3.1.2 "><p id="p2034975519227"><a name="p2034975519227"></a><a name="p2034975519227"></a>32-byte alignment.</p></td>
</tr>
<tr id="row14349855162220"><td class="cellrowborder" valign="top" width="49.980000000000004%" headers="mcps1.1.3.1.1 "><p id="p2349175522213"><a name="p2349175522213"></a><a name="p2349175522213"></a><span id="ph12349175572211"><a name="ph12349175572211"></a><a name="ph12349175572211"></a>L0A Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="50.019999999999996%" headers="mcps1.1.3.1.2 "><p id="p12349105510223"><a name="p12349105510223"></a><a name="p12349105510223"></a>512-byte alignment.</p></td>
</tr>
<tr id="row83498556224"><td class="cellrowborder" valign="top" width="49.980000000000004%" headers="mcps1.1.3.1.1 "><p id="p143491455162210"><a name="p143491455162210"></a><a name="p143491455162210"></a><span id="ph5349135515225"><a name="ph5349135515225"></a><a name="ph5349135515225"></a>L0B Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="50.019999999999996%" headers="mcps1.1.3.1.2 "><p id="p234945516225"><a name="p234945516225"></a><a name="p234945516225"></a>512-byte alignment.</p></td>
</tr>
<tr id="row13491655162217"><td class="cellrowborder" valign="top" width="49.980000000000004%" headers="mcps1.1.3.1.1 "><p id="p43494555226"><a name="p43494555226"></a><a name="p43494555226"></a><span id="ph17349155514229"><a name="ph17349155514229"></a><a name="ph17349155514229"></a>L0C Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="50.019999999999996%" headers="mcps1.1.3.1.2 "><p id="p14349125512220"><a name="p14349125512220"></a><a name="p14349125512220"></a>64-byte alignment.</p></td>
</tr>
</tbody>
</table>

**Recommended Data Layout Formats for Each Storage Unit**

-   L0A Buffer, L0B Buffer, and L0C Buffer are recommended to use the following fractal formats respectively:

    -   L0A Buffer: FRACTAL\_ZZ
    -   L0B Buffer: FRACTAL\_ZN
    -   L0C Buffer: FRACTAL\_NZ

    These formats are optimized for compute-intensive tasks such as cube multiplication, which can significantly improve computing efficiency.

-   For the L1 Buffer cache, the FRACTAL\_NZ format is recommended. When L1 uses the NZ format, the format conversion overhead can be reduced when data is moved to the L0A/L0B Buffers (which need to be converted to the ZZ and ZN formats, respectively).
-   The Unified Buffer has no format requirement for data.

**Resolving Access Conflicts of Storage Units to Improve Read/Write Performance**

When multiple operations attempt to access the same bank or bank group of the Unified Buffer simultaneously, bank conflicts may occur, including read/write conflicts, write/write conflicts, and read/read conflicts. Such conflicts cause access queuing and degrade performance. You can improve read/write performance by optimizing bank allocation. For details, see [Avoiding UB Bank Conflicts](../../../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_ub_bank_conflict.md).

## Data Transfer Unit<a name="section3376191651312"></a>

**Alignment Requirements During Data Transfer**

Because the transferred data is used for computation, there are requirements on the size of the transferred data. The size of the data transferred to the Unified Buffer must be aligned with the DataBlock, and data transfer to other storage units must follow the [fractal requirements](../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md). For example, when data is transferred from the L1 Buffer to the L0A Buffer, the data format must be converted from NZ to ZZ, and the size of the transferred data must be aligned with the fractal size. If the remaining size of the L1 Buffer is less than one fractal, an exception occurs during hardware execution.

## Synchronization Control<a name="section184418406169"></a>

**Intra-core Synchronization**

Because the execution units inside an AI Core (such as the MTE2 data transfer unit and the Vector compute unit) run asynchronously and in parallel, data dependencies may exist when reading from and writing to Local Memory (such as the Unified Buffer). To ensure data consistency and computation correctness, synchronization control is required to coordinate the operation timing.

Take the following process as an example: MTE2 transfers data from GM to the UB, the Vector compute unit performs the Abs computation, and then the data is transferred back to GM. The following synchronization conditions must be met:

1.  Data transfer and computation order
    -   Start the Abs computation on the Vector unit only after the GM-to-UB transfer is complete (to avoid data loss caused by an incomplete transfer when the computation starts);
    -   Perform the UB-to-GM data transfer only after the Vector computation is complete (to ensure that the result data is ready).

2.  Synchronization rules for loop transfer and computation scenarios
    -   Start a new transfer only after the preceding computation is complete: when the previous computation is not yet complete, a new data transfer must not be triggered (to prevent the old data in the UB from being overwritten);
    -   Start a new computation only after the preceding data transfer out is complete: when the previous data has not been fully transferred out of the UB, a new computation task must not be triggered (to avoid overwrite conflicts in the target memory area).

The synchronization control process is shown in the following figure:

![](../../../../figures/0000000_intra_core_sync_final.png)

In the preceding figure, ID1, ID2, ID3, ID4, ID5, and ID6 represent event IDs. Each event ID corresponds to the transfer status of a block of stored data, ensuring the correctness and consistency of data operations.

Note the following points:

-   It is recommended that you obtain an event ID through the [AllocEventID](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/AllocEventID.md) or [FetchEventID](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/FetchEventID.md) API to ensure its validity and effectiveness.
-   The number of event IDs is limited. After use, call ReleaseEventID immediately to release resources, preventing event ID exhaustion from affecting normal system operation.
-   SetFlag and WaitFlag must be used in pairs, and their parameters must be exactly the same (including template parameters and event IDs). If they do not match, the computation on the current core may become abnormal, or the operator execution on the next core may be affected, causing a timeout issue.

    For example, `SetFlag<HardEvent::S_MTE3>(1)` and `SetFlag<HardEvent::MTE3_MTE1>(1)` do not set the same event ID because their template parameters are different. Only when both the template parameters and the event IDs are exactly the same do they represent the same event ID.

-   Setting the same event ID consecutively is not allowed, because this may cause the event status to become confused or fail to be processed correctly.

**Inter-core Synchronization**

This hardware architecture does not support inter-core synchronization.
