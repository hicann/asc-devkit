# WriteSpmBuffer<a name="ZH-CN_TOPIC_0000001560560012"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:51:42.151Z pushedAt=2026-10-26T07:41:44.698Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TPipe/WriteSpmBuffer_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tpipe.h"`.

Copies the data that needs to be spilled and temporarily stored due to insufficient Unified Buffer (UB) space into the SPM Buffer.

## Prototype<a name="section620mcpsimp"></a>

-   Applies to both continuous and discontinuous data staging:

    ```cpp
    template <typename T>
    __aicore__ inline void WriteSpmBuffer(const LocalTensor<T>& writeBuffer, const DataCopyParams& copyParams, int32_t writeOffset = 0)
    ```

-   Applies to continuous data staging:

    ```cpp
    template <typename T>
    __aicore__ inline void WriteSpmBuffer(const LocalTensor<T>& writeBuffer, const int32_t writeSize, int32_t writeOffset = 0)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

<a name="table012895562310"></a>
<table><thead align="left"><tr id="row2128195532318"><th class="cellrowborder" valign="top" width="12.65%" id="mcps1.2.3.1.1"><p id="p1212885512232"><a name="p1212885512232"></a><a name="p1212885512232"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="87.35000000000001%" id="mcps1.2.3.1.2"><p id="p16129155552315"><a name="p16129155552315"></a><a name="p16129155552315"></a>Meaning</p></th>
</tr>
</thead>
<tbody><tr id="row6129355182310"><td class="cellrowborder" valign="top" width="12.65%" headers="mcps1.2.3.1.1 "><p id="p81291855102318"><a name="p81291855102318"></a><a name="p81291855102318"></a>T</p></td>
<td class="cellrowborder" valign="top" width="87.35000000000001%" headers="mcps1.2.3.1.2 "><p id="p14983161812418"><a name="p14983161812418"></a><a name="p14983161812418"></a>Data type of the Tensor to be obtained. For supported types, see <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>.</p></td>
</tr>
</tbody>
</table>

**Table 2**  API parameter description

<a name="table1794522316251"></a>
<table><thead align="left"><tr id="row19456238252"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p119458239258"><a name="p119458239258"></a><a name="p119458239258"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.2.4.1.2"><p id="p9945152332514"><a name="p9945152332514"></a><a name="p9945152332514"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.2.4.1.3"><p id="p1594552312513"><a name="p1594552312513"></a><a name="p1594552312513"></a>Meaning</p></th>
</tr>
</thead>
<tbody><tr id="row1694552372511"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1094516239250"><a name="p1094516239250"></a><a name="p1094516239250"></a>writeBuffer</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p8945112312514"><a name="p8945112312514"></a><a name="p8945112312514"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p169454239253"><a name="p169454239253"></a><a name="p169454239253"></a>Local memory that needs to be spilled and temporarily stored.</p></td>
</tr>
<tr id="row524916295111"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p92491829141111"><a name="p92491829141111"></a><a name="p92491829141111"></a>copyParams</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p6249142931114"><a name="p6249142931114"></a><a name="p6249142931114"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p18111121482617"><a name="p18111121482617"></a><a name="p18111121482617"></a>Copy parameters of the **DataCopyParams** type. For the definition of the **DataCopyParams** structure, see <a href="#table9182515919">Table 3</a>.</p></td>
</tr>
<tr id="row138643616158"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1986436201513"><a name="p1986436201513"></a><a name="p1986436201513"></a>writeSize</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p208648611152"><a name="p208648611152"></a><a name="p208648611152"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p198121859191513"><a name="p198121859191513"></a><a name="p198121859191513"></a>Number of elements to be copied.</p></td>
</tr>
<tr id="row126421762566"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p2642463560"><a name="p2642463560"></a><a name="p2642463560"></a>**writeOffset**</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p18642364562"><a name="p18642364562"></a><a name="p18642364562"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p146421267562"><a name="p146421267562"></a><a name="p146421267562"></a>Offset for copying to the SPM Buffer, in bytes.</p></td>
</tr>
</tbody>
</table>

**Table 3** Parameter definition of the DataCopyParams structure

<a name="table9182515919"></a>
<table><thead align="left"><tr id="row151816516917"><th class="cellrowborder" valign="top" width="15%" id="mcps1.2.3.1.1"><p id="p18182513916"><a name="p18182513916"></a><a name="p18182513916"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="85%" id="mcps1.2.3.1.2"><p id="p41815515920"><a name="p41815515920"></a><a name="p41815515920"></a>Meaning</p></th>
</tr>
</thead>
<tbody><tr id="row1818105113916"><td class="cellrowborder" valign="top" width="15%" headers="mcps1.2.3.1.1 "><p id="p17780347142614"><a name="p17780347142614"></a><a name="p17780347142614"></a>**blockCount**</p></td>
<td class="cellrowborder" valign="top" width="85%" headers="mcps1.2.3.1.2 "><p id="p478014752618"><a name="p478014752618"></a><a name="p478014752618"></a>Number of contiguous data blocks to be transferred. Type: uint16_t. Value range: blockCount∈[1, 4095].</p></td>
</tr>
<tr id="row2968131992515"><td class="cellrowborder" valign="top" width="15%" headers="mcps1.2.3.1.1 "><p id="p878011470264"><a name="p878011470264"></a><a name="p878011470264"></a>**blockLen**</p></td>
<td class="cellrowborder" valign="top" width="85%" headers="mcps1.2.3.1.2 "><p id="p11780174752617"><a name="p11780174752617"></a><a name="p11780174752617"></a>Length of each contiguous data block to be transferred. The unit is DataBlock (32 bytes). The type is uint16_t, and the value range is blockLen∈[1, 65535].</p></td>
</tr>
<tr id="row1589112062510"><td class="cellrowborder" valign="top" width="15%" headers="mcps1.2.3.1.1 "><p id="p378018478265"><a name="p378018478265"></a><a name="p378018478265"></a>**srcGap**</p></td>
<td class="cellrowborder" valign="top" width="85%" headers="mcps1.2.3.1.2 "><p id="p19780547162614"><a name="p19780547162614"></a><a name="p19780547162614"></a>Gap between adjacent contiguous data blocks of the source operand (the gap between the tail of the preceding data block and the head of the following data block). The unit is DataBlock (32 bytes). The type is uint16_t. Do not let srcGap exceed the value range of this data type.</p></td>
</tr>
<tr id="row3593192082512"><td class="cellrowborder" valign="top" width="15%" headers="mcps1.2.3.1.1 "><p id="p18780347152610"><a name="p18780347152610"></a><a name="p18780347152610"></a>**dstGap**</p></td>
<td class="cellrowborder" valign="top" width="85%" headers="mcps1.2.3.1.2 "><p id="p18780947162613"><a name="p18780947162613"></a><a name="p18780947162613"></a>Gap between adjacent contiguous data blocks of the destination operand (the gap between the tail of the preceding data block and the head of the following data block). The unit is DataBlock (32 bytes). The type is uint16_t. Do not let dstGap exceed the value range of this data type.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

-   When temporarily storing and copying data to L1, ensure that **writeSize** and **writeOffset** are 32-byte aligned.
-   The copied memory must not exceed the initialized **SPM Buffer** size; otherwise, issues such as overflow and memory corruption may occur.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

-   Copy data using **DataCopyParams**.

    ```cpp
    AscendC::TPipe pipe;
    int offset = 32; // Offset by 32 bytes when copying to the SPM Buffer.
    AscendC::DataCopyParams copyParams{2, 2, 1, 1}; // Copy two data blocks. The length of one data block is 2 datablocks, one datablock is 32 bytes, and both the source and destination addresses are separated by one data block between adjacent data blocks.
    // writeLocal is a LocalTensor of the half type on the UB.
    pipe.WriteSpmBuffer(writeLocal, copyParams, offset); // Copy the data block on the UB to the SPM Buffer.
    ...
    pipe.ReadSpmBuffer(writeLocal, copyParams, offset); // When the UB has sufficient space, read the data temporarily stored in the SPM Buffer back to the UB.
    ```

-   Copy data continuously using **writeSize**.

    ```cpp
    AscendC::TPipe pipe;
    int dataSize = 32;
    int offset = 32; // Offset by 32 bytes when copying to the SPM Buffer.
    
    // writeLocal is a LocalTensor of the half type on the UB.
    pipe.WriteSpmBuffer(writeLocal, dataSize, offset); // Move the continuous transfer data block on the UB to the SPM Buffer.
    ...
    pipe.ReadSpmBuffer(writeLocal, dataSize, offset); // When the UB has sufficient space, read the data temporarily stored in the SPM Buffer back to the UB.
    ```
