# Sort32<a name="ZH-CN_TOPIC_0000001487536634"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:53:33.505Z pushedAt=2026-10-28T03:14:39.596Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/sort_and_merge_ISASI/Sort32_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_proposal_intf.h"`

The Sort32 API performs descending-order sorting of 32 numbers within a single iteration.

As shown in the following figure, src0 and src1 represent the data score to be sorted and the index of the corresponding RP, respectively. After sorting is complete, the result is stored in dst as a (score, index) structure.

src1 is fixed as the uint32\_t type. Regardless of whether src0 is of the float or half type, the (score, index) structure in dst always occupies 8B space. Specifically:

- When src0 is of the float type, the index in dst is stored in the upper 4B, and the score is stored in the lower 4B.
- When src0 is of the half type, the index in dst is stored in the upper 4B, the score is stored in the lower 2B, and the middle 2B are reserved.

**Figure 1** Sort32 sorting diagram

![](../../../../figures/sort32.png)<a id="figure-1-sort32-sorting-diagram"></a>

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void Sort32(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<uint32_t>& src1, const int32_t repeatTime)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|---|---|
| T | Data type of the operand.|

**Table 2**  Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br> The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br> The start address of LocalTensor must be 32-byte aligned. |
| src0 | Input | Source operand.<br> The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br> The start address of LocalTensor must be 32-byte aligned. |
| src1 | Input | Source operand.<br> The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br> The start address of LocalTensor must be 32-byte aligned.<br> **Note: The data type of src1 is fixed to uint32_t.** |
| repeatTime | Input | Number of repeated iterations. Each iteration sorts 32 elements. In the next iteration, src0 skips 128 bytes of space, src1 skips 128 bytes of space, and dst skips 256 bytes of space. For each iteration, the input src0 must be 128-byte aligned, src1 must be 128-byte aligned, and the output dst must be 256-byte aligned. repeatTime∈[0, 255]. |

## Data Types

**Table 3**  Data type combinations

| src0 data type | src1 data type | dst data type | 
|---|---|---|
| half | uint32_t | half |
| float | uint32_t | float |

## Constraints<a name="section633mcpsimp"></a>

- For address alignment constraints, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
<!-- npu="A3,910b,950" id12 -->
- When the repeatTime parameter is set to 0, this API behaves as follows:
    <!-- npu="A3,910b" id10 -->
    - For the following models, when the repeatTime parameter is set to 0, no computation is performed and no data is written to the destination operand. This API is treated as a NOP (no operation).
       <!-- npu="A3" id8 -->
       - Atlas A3 products
       <!-- end id8 -->
       <!-- npu="910b" id9 -->
       - Atlas A2 products
       <!-- end id9 -->
    <!-- end id10 -->
    <!-- npu="950" id11 -->
    - For Ascend 950PR&950DT products, this API implements compatibility by calling the [Reg vector compute](../../reg_vector_compute/reg_vector_compute.md) API through VF. When the repeatTime parameter is set to 0, this API is not guaranteed to be treated as a NOP (no operation).
    <!-- end id11 -->
<!-- end id12 -->

- If score\[i\]==score\[j\] and i\>j exist, score\[j\] is placed first.
- Data within each iteration is sorted, while data across different iterations is not sorted.
- For each iteration, the input src0 requires 128B\(float\)/64B\(half\) alignment, src1 requires 128B alignment, and the output dst requires 256B alignment.

## Example<a name="section642mcpsimp"></a>

For the complete example, see [Sort32 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/mrg_sort).

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
