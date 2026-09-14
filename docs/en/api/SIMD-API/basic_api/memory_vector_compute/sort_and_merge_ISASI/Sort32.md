# Sort32<a name="ZH-CN_TOPIC_0000001487536634"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-09T15:33:28.163Z -->

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

The Sort32 API sorts 32 numbers in descending order within each iteration.

As shown in the following figure, src0 and src1 represent the data score to be sorted and the index of the corresponding RP, respectively. After sorting is complete, the result is stored in dst in the (score, index) structure.

src1 is fixed as the uint32\_t type. Regardless of whether src0 is of the float or half type, the (score, index) structure in dst always occupies 8B of space. Specifically:

- When src0 is of the float type, the index in dst is stored in the upper 4B, and the score is stored in the lower 4B.
- When src0 is of the half type, the index in dst is stored in the upper 4B, the score is stored in the lower 2B, and the middle 2B are reserved.

**Figure 1**  Sort32 sorting diagram

![](../../../../figures/sort32.png)<a id="figure-1-sort32-sorting-diagram"></a>

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void Sort32(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<uint32_t>& src1, const int32_t repeatTime)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br> The type is LocalTensor, and the supported TPosition is VECIN/VECCALC/VECOUT.<br> The starting address of the LocalTensor requires 32-byte alignment. |
| src0 | Input | Source operand.<br> The type is LocalTensor, and the supported TPosition is VECIN/VECCALC/VECOUT.<br> The starting address of the LocalTensor requires 32-byte alignment. |
| src1 | Input | Source operand.<br> The type is LocalTensor, and the supported TPosition is VECIN/VECCALC/VECOUT.<br> The starting address of the LocalTensor requires 32-byte alignment.<br> **Note: The data type of src1 is fixed to uint32_t.** |
| repeatTime | Input | Number of repeated iterations. Each iteration sorts 32 elements. In the next iteration, src0 skips 128B of space, src1 skips 128B of space, and dst skips 256B of space. In each iteration, the input src0 requires 128B alignment, src1 requires 128B alignment, and the output dst requires 256B alignment. repeatTime∈[0, 255].<br> **Note: repeatTime = 0 indicates that no sorting is performed, and the API is treated as a NOP (no operation).** |

## Data Type

**Table 3** Data type combinations

| src0 data type | src1 data type | dst data type | 
|---|---|---|
| half | uint32_t | half |
| float | uint32_t | float |

## Constraints<a name="section633mcpsimp"></a>

- For address alignment constraints, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- repeatTime = 0 indicates that no sorting is performed, and the API is treated as a NOP (no operation).
- If score\[i\]==score\[j\] and i\>j, score\[j\] is preferentially placed first.
- Data within each iteration is sorted, while data across different iterations is not sorted.
- In each iteration, the input src0 requires 128B\(float\)/64B\(half\) alignment, src1 requires 128B alignment, and the output dst requires 256B alignment.

## Example<a name="section642mcpsimp"></a>

For the complete sample, see [Sort32 sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/mrg_sort).

```cpp
AscendC::LocalTensor<float> srcLocal0 = inQueueSrc0.DeQue<float>();
AscendC::LocalTensor<uint32_t> srcLocal1 = inQueueSrc1.DeQue<uint32_t>();
AscendC::LocalTensor<float> dstLocal = outQueueDst.AllocTensor<float>();
// repeatTime = 4, divide 128 numbers into 4 groups for sorting, and sort 32 numbers in one group each time.
AscendC::Sort32<float>(dstLocal, srcLocal0, srcLocal1, 4);
outQueueDst.EnQue<float>(dstLocal);
inQueueSrc0.FreeTensor(srcLocal0);
inQueueSrc1.FreeTensor(srcLocal1);
```
