# MrgSort<a name="ZH-CN_TOPIC_0000001538216637"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T15:13:28.075Z -->

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
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_proposal_intf.h"`

Merges up to four sorted queues into a single queue, with the results sorted by the score field in descending order.

The MrgSort API usually processes data preprocessed by the Sort32 API. For the queue structure, see [Sort32 Description](Sort32.md#description).

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void MrgSort(const LocalTensor<T>& dst, const MrgSortSrcList<T>& src, const MrgSort4Info& params)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|--------|------|
| T | Data type of the destination operand. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|:---|:---|:---|
| dst | Output | Destination operand.<br> Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br> The start address of the LocalTensor requires 32-byte alignment. |
| src | Input | Source operand, consisting of four sorted queues. Type: MrgSortSrcList struct, defined as follows:<br>template \<typename T\> struct MrgSortSrcList {<br>    \_\_aicore\_\_ MrgSortSrcList() {}<br>    \_\_aicore\_\_ MrgSortSrcList(const LocalTensor&lt;T&gt; src1In, const LocalTensor&lt;T&gt; src2In, const LocalTensor&lt;T&gt; src3In, const LocalTensor&lt;T&gt; src4In)<br>    {<br>        src1 = src1In[0];<br>        src2 = src2In[0];<br>        src3 = src3In[0];<br>        src4 = src4In[0];<br>    }<br>    LocalTensor&lt;T&gt; src1;<br>    LocalTensor&lt;T&gt; src2;<br>    LocalTensor&lt;T&gt; src3;<br>    LocalTensor&lt;T&gt; src4;<br>};<br>src1, src2, src3, and src4 are of type LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor requires 8-byte alignment.|
| params | Input | Sorting parameters. Type: MrgSort4Info struct.<br> For details about the MrgSort4Info parameters, see [Table MrgSort4Info struct parameter definition](MrgSort.md#parameters).|

**Table 3** MrgSort4Info struct parameter definition

| Parameter | Meaning |
|---|---|
| elementLengths | A uint16\_t array of length 4, used to store the lengths of the four source queues, in 8B units. Value range: [0, 4095]. |
| ifExhaustedSuspension | Controls whether instruction execution stops when a queue is exhausted. Type: bool. Default value: false.<br>&bull; false: When a queue is exhausted, the instruction does not stop.<br>&bull; true: When a queue is exhausted, the instruction stops.|
| validBit | Controls the queues that participate in the computation. Valid values: 3, 7, 15.<br>&bull; Value 3 (0'b0011) indicates that the first 2 queues are valid.<br>&bull; Value 7 (0'b0111) indicates that the first 3 queues are valid.<br>&bull; Value 15 (0'b1111) indicates that the first 4 queues are valid.|
| repeatTimes | Number of repeated iterations. Each iteration completes the merge sort of four queues, and the source and destination operands skip the total length of the four queues. Value range: repeatTimes∈[0, 255].<br> The repeatTimes parameter takes effect only when the following four conditions are met: <br>&bull; src contains four queues of equal length.<br>&bull; The four queues are stored contiguously.<br>&bull; ifExhaustedSuspension = false.<br>&bull; validBit = 15.<br> **Note: repeatTimes = 0 indicates that no sorting is performed, and the API is treated as a NOP (no operation).**|

## Data Type

The supported data types of the operands are half and float.

## Constraints<a name="section633mcpsimp"></a>

- Overlapping between the source and destination operation addresses is not supported.
- The start address of dst requires 32-byte alignment, and the start address of the LocalTensor in src requires 8-byte alignment.
- If score\[i\]==score\[j\] exists and i\>j, score\[j\] is preferentially placed first.
- Data within each iteration is sorted, while data across different iterations is not sorted.

## Example<a name="section642mcpsimp"></a>

For the complete example, see [MrgSort sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/mrg_sort).

```cpp
// Merge-sorts 8 sorted queues, repeatTimes = 2, data stored contiguously
// Each queue contains 32 (score, index) 8-byte structures
// Finally outputs the result of sorting the 256 values in the score field
AscendC::MrgSort4Info params;
params.elementLengths[0] = 32;
params.elementLengths[1] = 32;
params.elementLengths[2] = 32;
params.elementLengths[3] = 32;
params.ifExhaustedSuspension = false;
params.validBit = 0b1111;
params.repeatTimes = 2;

AscendC::MrgSortSrcList<float> srcList;
srcList.src1 = workLocal[0];
srcList.src2 = workLocal[64]; // workLocal is float type, each queue occupies 256 bytes of space
srcList.src3 = workLocal[128];
srcList.src4 = workLocal[192];

AscendC::MrgSort<float>(dstLocal, srcList, params);
outQueueDst.EnQue<float>(dstLocal);
outQueueDst.FreeTensor(dstLocal);
```
