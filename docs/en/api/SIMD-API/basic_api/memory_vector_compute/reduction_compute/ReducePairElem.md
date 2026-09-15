# ReducePairElem

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T09:43:18.721Z -->

## Applicable Products

<!-- npu="950" id25 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id25 -->
<!-- npu="A3" id26 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id26 -->
<!-- npu="910b" id27 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id27 -->
<!-- npu="310b" id28 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id28 -->
<!-- npu="310p" id29 -->
- Atlas inference products AI Core: Supported
<!-- end id29 -->
<!-- npu="310p" id30 -->
- Atlas inference products Vector Core: Not supported
<!-- end id30 -->
<!-- npu="910" id31 -->
- Atlas training products: Supported
<!-- end id31 -->

## Description

Header file path: `"basic_api/kernel_operator_vec_reduce_intf.h"`.

The `ReducePairElem` API performs a reduction operation on two adjacent (odd-even) elements based on the template parameter `reduceType`, and writes the results to the destination address in order. **Currently, only the summation operation is supported.**

For example, the summation result of [a1, a2, a3, a4, a5, a6, ...] is [a1+a2, a3+a4, a5+a6, ...]. The calculation process is shown in the following figure, where the gray part represents invalid elements.

**Figure 1**  `ReducePairElem<ReduceType::SUM>` schematic diagram

![Schematic diagram of ReducePairElem<ReduceType::SUM>](../../../../figures/ReducePairElem.png "Schematic diagram of ReducePairElem<ReduceType::SUM>")

## Prototype

- `mask` bit-by-bit mode:

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReducePairElem(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[],const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
    ```

- `mask` continuous mode:

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReducePairElem(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
    ```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| reduceType | Reduction operation type.<br>The type is the `ReduceType` enum, which supports:<br>&bull; `ReduceType::SUM`: sum. |
| T | Data type of the destination operand. |
| U | Data type of the source operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). The supported TPosition values are VECIN, VECCALC, and VECOUT (the storage location is Unified Buffer). |
| src | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). The supported TPosition values are VECIN, VECCALC, and VECOUT (the storage location is Unified Buffer). |
| mask[]/mask | Input | `mask` controls the source operands that participate in the computation in each iteration. For details, see [Mask Overview](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of iterations. The value range is [0, 255]. |
| dstRepStride | Input | Address stride between adjacent iterations of the destination operand, in the unit of the length after one repeatTime reduction, that is, 128 bytes. The value range is [0, $2^{16}-1$].<!-- npu="910" id24 --><br>**Note: Atlas training products do not support setting this parameter to 0.** <!-- end id24 -->|
| srcBlkStride | Input | Address stride of a DataBlock within a single iteration, in the unit of 32 bytes. The value range is [0, $2^{16}-1$]. |
| srcRepStride | Input | Address stride between adjacent iterations of the source operand, that is, the number of DataBlocks skipped by the source operand in each iteration. The value range is [0, $2^{16}-1$]. |

**Note: For the high-dimensional split related parameters `mask`, `repeatTime`, `dstRepStride`, `srcBlkStride`, and `srcRepStride`, see [High-Dimensional Split](../SIMD_compute/high_dimension_slicing.md).**

## Data Type

The supported data types are as follows:

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: half and float are supported.
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: half and float are supported.
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: half and float are supported.
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: half and float are supported.
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: half and float are supported.
<!-- end id12 -->
<!-- npu="910" id13 -->
- Atlas training products: half is supported.
<!-- end id13 -->

The data types of the destination operand and the source operand must be consistent.

## Return Value

None

## Constraints

- For the start address alignment constraint of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#general-address-alignment-constraints). 32-byte alignment is required.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- The value range of `dstRepStride`, `srcBlkStride`, and `srcRepStride` is [0, $2^{16}-1$]. The actual size of UB must be considered to avoid out-of-bounds access.

<!-- npu="A3,910b" id16 -->
- For the following models, when `mask=0` or `repeatTime=0`, the reduction operation is not performed, the destination operand is not written, and this API is treated as `NOP` (no operation).
  <!-- npu="A3" id14 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id14 -->
  <!-- npu="910b" id15 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id15 -->
<!-- end id16 -->

<!-- npu="A3,910b,310p" id1 -->
- For the following models, when the masks of a pair of adjacent elements are both 0, the corresponding destination elements are skipped and remain unchanged.
  <!-- npu="A3" id3 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id3 -->
  <!-- npu="910b" id4 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id4 -->
  <!-- npu="310p" id5 -->
  - Atlas inference products AI Core
  <!-- end id5 -->
<!-- end id1 -->

  <!-- npu="950,310b" id2 -->
- For the following models, when the masks of a pair of adjacent elements are both 0, the values in the corresponding destination operand are set to 0.
  <!-- npu="950" id6 -->
  - Ascend 950PR/Ascend 950DT
  <!-- end id6 -->
  <!-- npu="310b" id7 -->
  - Atlas 200I/500 A2 inference products
  <!-- end id7 -->
  <!-- end id2 -->

## Example

This sample shows only part of the code in the `Compute` process. For more samples, see [ReducePairElem sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_pair_elem).

- `ReducePairElem` tensor high-dimensional split computation sample - `mask` continuous mode:

    ```cpp
    // Set mask so that all 128 elements participate in the computation.
    int32_t mask = 256/sizeof(half);
    // 128 elements per repeat, 128 elements in total.
    int repeat = 1;
    // dstLocal: destination operand tensor
    // srcLocal: source operand tensor
    // srcBlkStride = 1, no gap between blocks in a repeat.
    // dstRepStride = 1, srcRepStride = 8, no gap between repeats.
    AscendC::ReducePairElem<AscendC::ReduceType::SUM, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

- `ReducePairElem` tensor high-dimensional split computation sample - `mask` bit-by-bit mode:

    ```cpp
    // Set mask so that all 128 elements participate in the computation.
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // 128 elements per repeat, 128 elements in total.
    int repeat = 1;
    // dstLocal: destination operand tensor
    // srcLocal: source operand tensor
    // srcBlkStride = 1, no gap between blocks within a repeat.
    // dstRepStride = 1, srcRepStride = 8, no gap between repeats.
    AscendC::ReducePairElem<AscendC::ReduceType::SUM, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

- Example result

    > Input data src_gm:
    > > [1, 1, 1, -1, 2, 2, -1, 2,
    > >  3, 3, 3, -1, 4, 4, -2, 4,
    > >  ....
    > > ]
    >
    > Output data dst_gm:
    > > [2, 0, 4, 1, 6, 2, 8, 2,
    > >  ....
    > > ]
