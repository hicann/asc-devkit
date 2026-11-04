# MrgSort<a name="ZH-CN_TOPIC_0000001538216637"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:48:39.563Z pushedAt=2026-10-28T03:14:39.586Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/sort_and_merge_ISASI/MrgSort_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_proposal_intf.h"`

Merges and arranges up to 4 already-sorted queues into a single queue, with the result sorted by the score field from largest to smallest.

The MrgSort API typically processes data preprocessed by the Sort32 API. For the queue structure, see [Sort32 Description](Sort32.md).

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void MrgSort(const LocalTensor<T>& dst, const MrgSortSrcList<T>& src, const MrgSort4Info& params)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|--------|------|
| T | Data type of the destination operand. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
|:---|:---|:---|
| dst | Output | Destination operand.<br> Type is LocalTensor, and the supported TPosition is VECIN/VECCALC/VECOUT.<br> The start address of LocalTensor needs 32-byte alignment. |
| src | Input | Source operand, four sorted queues, type is the MrgSortSrcList structure, defined as follows:<br>template \<typename T\> struct MrgSortSrcList {<br>    \_\_aicore\_\_ MrgSortSrcList() {}<br>    \_\_aicore\_\_ MrgSortSrcList(const LocalTensor&lt;T&gt; src1In, const LocalTensor&lt;T&gt; src2In, const LocalTensor&lt;T&gt; src3In, const LocalTensor&lt;T&gt; src4In)<br>    {<br>        src1 = src1In[0];<br>        src2 = src2In[0];<br>        src3 = src3In[0];<br>        src4 = src4In[0];<br>    }<br>    LocalTensor&lt;T&gt; src1;<br>    LocalTensor&lt;T&gt; src2;<br>    LocalTensor&lt;T&gt; src3;<br>    LocalTensor&lt;T&gt; src4;<br>};<br>src1, src2, src3, and src4 are of type LocalTensor, and the supported TPosition is VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor needs 8-byte alignment.|
| params | Input | Sorting parameter, type is the MrgSort4Info structure.<br> For the MrgSort4Info parameter description, see [Table MrgSort4Info structure parameter definition](MrgSort.md#parameters).|

**Table 3**  MrgSort4Info structure parameter definition

| Parameter | Meaning |
|---|---|
| elementLengths | Array of length 4 uint16\_t, used to store the lengths of the four source queues, in 8B units. Value range: [0, 4095]. |
| ifExhaustedSuspension | Controls whether instruction execution stops when a queue is exhausted. Type is bool, default value is false.<br>&bull; false: When a queue is exhausted, the instruction does not stop.<br>&bull; true: When a queue is exhausted, the instruction stops.|
| validBit | Controls the queues participating in the computation, with valid values 3, 7, and 15.<br>&bull;Value is 3 (0'b0011), indicating the first 2 queues are valid.<br>&bull;Value is 7 (0'b0111), indicating the first 3 queues are valid.<br>&bull;Value is 15 (0'b1111), indicating the first 4 queues are valid.|
| repeatTimes | Repeat iteration count. Each iteration completes the merge sort of four queues, and the source and destination operands skip the total length of the four queues. Value range: repeatTimes∈[0, 255].<br> For the repeatTimes parameter to take effect, the following four conditions must be met: <br>&bull; src contains four queues with identical lengths.<br>&bull; The four queues are stored contiguously.<br>&bull; ifExhaustedSuspension = false.<br>&bull; validBit = 15.|

## Data Types

The supported data types for operands are half/float.

## Constraints<a name="section633mcpsimp"></a>

- Overlapping between source and destination operand addresses is not supported.
- The start address of dst needs 32-byte alignment, and the start address of the LocalTensor in src needs 8-byte alignment.
- If score\[i\]==score\[j\] and i\>j exist, score\[j\] is prioritized to be placed first.
- Data within each iteration is sorted, while data across different iterations is not sorted.
<!-- npu="A3,910b,950" id12 -->
- When the parameter repeatTimes is 0, the behavior of this API is as follows:
    <!-- npu="A3,910b" id10 -->
    - For the following models, when the parameter repeatTimes is 0, no computation is performed, no write is made to the destination operand, and this API is treated as a NOP (no operation).
       <!-- npu="A3" id8 -->
       - Atlas A3 products
       <!-- end id8 -->
       <!-- npu="910b" id9 -->
       - Atlas A2 products
       <!-- end id9 -->
    <!-- end id10 -->
    <!-- npu="950" id11 -->
    - For Ascend 950PR&950DT products, this API implements compatibility by calling the [Reg vector compute](../../reg_vector_compute/reg_vector_compute.md) API through VF. When the parameter repeatTimes is 0, it is not guaranteed that this API is treated as a NOP (no operation).
    <!-- end id11 -->
<!-- end id12 -->


## Example<a name="section642mcpsimp"></a>

For the complete example, see [MrgSort example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/mrg_sort).

```cpp
// Merge and sort eight sorted queues, with repeatTimes = 2 and data stored contiguously.
// Each queue contains 32 8-byte (score, index) structures.
// Finally output the result after sorting the 256 numbers in the score field.
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
srcList.src2 = workLocal[64]; // workLocal is of the float type, and each queue occupies 256 bytes of space.
srcList.src3 = workLocal[128];
srcList.src4 = workLocal[192];

AscendC::MrgSort<float>(dstLocal, srcList, params);
outQueueDst.EnQue<float>(dstLocal);
outQueueDst.FreeTensor(dstLocal);
```
