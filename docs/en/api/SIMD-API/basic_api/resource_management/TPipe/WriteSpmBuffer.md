# WriteSpmBuffer<a name="ZH-CN_TOPIC_0000001560560012"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:40:45.452Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Copies the data that needs to be spilled and temporarily stored into the SPM Buffer.

## Prototype<a name="section620mcpsimp"></a>

-   Applies to both contiguous and non-contiguous data staging:

    ```
    template <typename T>
    __aicore__ inline void WriteSpmBuffer(const LocalTensor<T>& writeBuffer, const DataCopyParams& copyParams, int32_t writeOffset = 0)
    ```

-   Applies to contiguous data staging:

    ```
    template <typename T>
    __aicore__ inline void WriteSpmBuffer(const LocalTensor<T>& writeBuffer, const int32_t writeSize, int32_t writeOffset = 0)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** API parameter description

<a name="table1794522316251"></a>
<table><thead align="left"><tr id="row19456238252"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p119458239258"><a name="p119458239258"></a><a name="p119458239258"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.2.4.1.2"><p id="p9945152332514"><a name="p9945152332514"></a><a name="p9945152332514"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.2.4.1.3"><p id="p1594552312513"><a name="p1594552312513"></a><a name="p1594552312513"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1694552372511"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1094516239250"><a name="p1094516239250"></a><a name="p1094516239250"></a>**writeBuffer**</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p8945112312514"><a name="p8945112312514"></a><a name="p8945112312514"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p169454239253"><a name="p169454239253"></a><a name="p169454239253"></a>Local memory that requires overflow temporary storage.</p></td>
</tr>
<tr id="row524916295111"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p92491829141111"><a name="p92491829141111"></a><a name="p92491829141111"></a>**copyParams**</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p6249142931114"><a name="p6249142931114"></a><a name="p6249142931114"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p18111121482617"><a name="p18111121482617"></a><a name="p18111121482617"></a>Copy parameters, of the **DataCopyParams** type. For the definition of the **DataCopyParams** structure, see <a href="#table9182515919">Table 2</a>.</p></td>
</tr>
<tr id="row138643616158"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1986436201513"><a name="p1986436201513"></a><a name="p1986436201513"></a>writeSize</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p208648611152"><a name="p208648611152"></a><a name="p208648611152"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p198121859191513"><a name="p198121859191513"></a><a name="p198121859191513"></a>Number of elements to copy.</p></td>
</tr>
<tr id="row126421762566"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p2642463560"><a name="p2642463560"></a><a name="p2642463560"></a>writeOffset</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p18642364562"><a name="p18642364562"></a><a name="p18642364562"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p146421267562"><a name="p146421267562"></a><a name="p146421267562"></a>Offset for copying to the SPM Buffer, in bytes.</p></td>
</tr>
</tbody>
</table>

**Table 2**  DataCopyParams structure parameter definition

<a name="table9182515919"></a>
<table><thead align="left"><tr id="row151816516917"><th class="cellrowborder" valign="top" width="15%" id="mcps1.2.3.1.1"><p id="p18182513916"><a name="p18182513916"></a><a name="p18182513916"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="85%" id="mcps1.2.3.1.2"><p id="p41815515920"><a name="p41815515920"></a><a name="p41815515920"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1818105113916"><td class="cellrowborder" valign="top" width="15%" headers="mcps1.2.3.1.1 "><p id="p17780347142614"><a name="p17780347142614"></a><a name="p17780347142614"></a>**blockCount**</p></td>
<td class="cellrowborder" valign="top" width="85%" headers="mcps1.2.3.1.2 "><p id="p478014752618"><a name="p478014752618"></a><a name="p478014752618"></a>Number of consecutive data blocks to be transferred. **uint16_t** type, with a value range of blockCount∈[1, 4095].</p></td>
</tr>
<tr id="row2968131992515"><td class="cellrowborder" valign="top" width="15%" headers="mcps1.2.3.1.1 "><p id="p878011470264"><a name="p878011470264"></a><a name="p878011470264"></a>**blockLen**</p></td>
<td class="cellrowborder" valign="top" width="85%" headers="mcps1.2.3.1.2 "><p id="p11780174752617"><a name="p11780174752617"></a><a name="p11780174752617"></a>Length of each consecutive data block to be transferred, in **DataBlock** (32 bytes). **uint16_t** type, with a value range of blockLen∈[1, 65535].</p></td>
</tr>
<tr id="row1589112062510"><td class="cellrowborder" valign="top" width="15%" headers="mcps1.2.3.1.1 "><p id="p378018478265"><a name="p378018478265"></a><a name="p378018478265"></a>**srcGap**</p></td>
<td class="cellrowborder" valign="top" width="85%" headers="mcps1.2.3.1.2 "><p id="p19780547162614"><a name="p19780547162614"></a><a name="p19780547162614"></a>Gap between adjacent consecutive data blocks of the source operand (the gap between the end of the preceding data block and the start of the following data block), in **DataBlock** (32 bytes). **uint16_t** type. Ensure that **srcGap** does not exceed the value range of this data type.</p></td>
</tr>
<tr id="row3593192082512"><td class="cellrowborder" valign="top" width="15%" headers="mcps1.2.3.1.1 "><p id="p18780347152610"><a name="p18780347152610"></a><a name="p18780347152610"></a>**dstGap**</p></td>
<td class="cellrowborder" valign="top" width="85%" headers="mcps1.2.3.1.2 "><p id="p18780947162613"><a name="p18780947162613"></a><a name="p18780947162613"></a>Gap between adjacent consecutive data blocks of the destination operand (the gap between the end of the preceding data block and the start of the following data block), in **DataBlock** (32 bytes). **uint16_t** type. Ensure that **dstGap** does not exceed the value range of this data type.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

-   When temporarily copying to L1, ensure that **writeSize** and **writeOffset** are 32-byte aligned.
-   The copied memory must not exceed the initialized SPM Buffer size; otherwise, issues such as overflow and memory corruption may occur.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

-   Copy data using DataCopyParams.

    ```
    AscendC::TPipe pipe;
    int dataSize = 32; // Assume that T is of the half type. Apply for a memory block of 32 * sizeof(half) bytes from UB.
    int offset = 32; // Offset by 32 bytes when copying to spmBuffer.
    AscendC::DataCopyParams copyParams{1, 2, 0, 0}; // Copy a contiguous data block from UB. The length of one data block is 2 datablocks, and one datablock is 32 bytes.
    // writeLocal is a LocalTensor of the half type on the SPM Buffer.
    pipe.WriteSpmBuffer(writeLocal, copyParams, offset); // Copy the contiguous data block from UB to the SPM Buffer.
    pipe.ReadSpmBuffer(writeLocal, copyParams, offset); // Read the data temporarily stored in the SPM Buffer back to local data.
    ...
    // When the UB memory is sufficient, copy the data block temporarily stored in the SPM Buffer back to GM. dstGlobal is a GlobalTensor of the half type.
    AscendC::DataCopy(dstGlobal, writeLocal, copyParams);
    ```

-   Copy data continuously using writeSize.

    ```
    AscendC::TPipe pipe;
    int dataSize = 32; // Assume T is of the half type. Apply for a memory block of 32 * sizeof(half) bytes from the UB.
    int offset = 32; // Offset by 32 bytes when copying to the SPM Buffer.
    ;
    // writeLocal is a LocalTensor of the half type on the SPM Buffer.
    pipe.WriteSpmBuffer(writeLocal, dataSize, offset); // Move the continuous transfer data block on the UB to the SPM Buffer.
    pipe.ReadSpmBuffer(writeLocal, dataSize, offset); // Read the data temporarily stored in the SPM Buffer back to the local data.
    ...
    // When the UB memory is sufficient, move the data block temporarily stored in the SPM Buffer back to the GM. dstGlobal is a GlobalTensor of the half type.
    AscendC::DataCopy(dstGlobal, writeLocal, dataSize);
    ```

