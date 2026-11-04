# ReducePairElem

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:25:57.816Z pushedAt=2026-10-28T03:14:39.556Z nl=2 -->

## Applicable Products

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
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReducePairElem_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_vec_reduce_intf.h"`.

The `ReducePairElem` API performs a reduction operation on two adjacent (odd-even) elements based on the template parameter `reduceType`, and writes the results to the destination address in order. **Currently only the sum operation is supported.**

For example, the sum of [a1, a2, a3, a4, a5, a6, ...] is [a1+a2, a3+a4, a5+a6, ...]. The computation process is shown in the following figure, where the gray parts represent invalid elements.

**Figure 1**  `ReducePairElem<ReduceType::SUM>` schematic diagram

![ReducePairElem<ReduceType::SUM> schematic diagram](../../../../figures/ReducePairElem.png "ReducePairElem<ReduceType::SUM> schematic diagram")

## Function Prototype

- `mask` bit-by-bit mode:

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReducePairElem(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
    ```

- `mask` continuous mode:

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReducePairElem(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
    ```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| reduceType | Reduction operation type.<br>The type is the `ReduceType` enumeration, which supports:<br>&bull; `ReduceType::SUM`: sum. |
| T | Data type of the destination operand. |
| U | Data type of the source operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: set the mask inside the API.<br>&bull; false: set the mask outside the API. You need to use [SetVectorMask](../mask_operations/SetVectorMask.md) to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2**  Parameters

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| src | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| mask[]/mask | Input | `mask` controls the source operands that participate in the computation within each iteration. For details, see [Mask Overview](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of iterations. The value range is [0, 255]. |
| dstRepStride | Input | Address stride between adjacent iterations of the destination operand, in units of the length after one repeatTime reduction, that is, 128 bytes. The value range is [0, $2^{16}-1$].<!-- npu="910" id28 --><br>**Note that Atlas training products do not support setting this parameter to 0.**<!-- end id28 --> |
| srcBlkStride | Input | Address stride of the DataBlock within a single iteration, in units of 32 bytes. The value range is [0, $2^{16}-1$]. |
| srcRepStride | Input | Address stride between adjacent iterations of the source operand, that is, the number of DataBlocks skipped by the source operand in each iteration. The value range is [0, $2^{16}-1$]. |

**Note: For the high-dimensional split related parameters `mask`, `repeatTime`, `dstRepStride`, `srcBlkStride`, and `srcRepStride` above, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md).**

## Data Types

The supported data types are as follows:

<!-- npu="950" id8 -->
- Ascend 950PR&950DT products support half and float.
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 products support half and float.
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 products support half and float.
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products support half and float.
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core support half and float.
<!-- end id12 -->
<!-- npu="910" id13 -->
- Atlas training products support half.
<!-- end id13 -->

The data types of the destination operand and the source operand must be consistent.

## Return Value

None

## Constraints

- For the start address alignment constraint of the operand, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#general-address-alignment-constraints). 32-byte alignment is required.
- For the operand address overlap constraint, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- The value range of `dstRepStride`, `srcBlkStride`, and `srcRepStride` is [0, $2^{16}-1$]. Consider the actual size of the Unified Buffer (UB) to avoid out-of-bounds access.

<!-- npu="950,A3,910b" id14 -->
- When the value of mask or repeatTime is 0, this API behaves as follows:
  <!-- npu="A3,910b" id15 -->
  - For the following models, when the value of mask or repeatTime is 0, no computation is performed and no write is made to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id16 -->
    - Atlas A3 products
    <!-- end id16 -->
    <!-- npu="910b" id17 -->
    - Atlas A2 products
    <!-- end id17 -->
  <!-- end id15 -->

  <!-- npu="950" id18 -->
  - For Ascend 950PR&950DT products, this API implements compatibility by calling the [Reg vector compute](../../reg_vector_compute/reg_vector_compute.md) API through VF. When the value of mask or repeatTime is 0, this API is not guaranteed to be treated as a NOP (no operation).
  <!-- end id18 -->
<!-- end id14 -->

<!-- npu="950" id19 -->
- UB space usage. For Ascend 950PR&950DT products:
  - The tensor high-dimensional split computation occupies 8KB of UB.
  - The computation of the first n consecutive tensor data does not involve the 8KB UB usage.
<!-- end id19 -->

<!-- npu="950,A3,910b,310p,310b" id20 -->
- When the masks of a pair of adjacent elements are both 0, this API behaves as follows:
  <!-- npu="A3,910b,310p" id21 -->
  - For the following models, the corresponding destination element is skipped for writing and remains unchanged.
    <!-- npu="A3" id22 -->
    - Atlas A3 products
    <!-- end id22 -->
    <!-- npu="910b" id23 -->
    - Atlas A2 products
    <!-- end id23 -->
    <!-- npu="310p" id24 -->
    - Atlas inference products AI Core
    <!-- end id24 -->
  <!-- end id21 -->

  <!-- npu="950,310b" id25 -->
  - For the following models, the value in the corresponding destination operand is set to 0.
    <!-- npu="950" id26 -->
    - Ascend 950PR&950DT products
    <!-- end id26 -->
    <!-- npu="310b" id27 -->
    - Atlas 200I/500 A2 inference products
    <!-- end id27 -->
  <!-- end id25 -->
<!-- end id20 -->

## Example

This sample shows only part of the code in the `Compute` process. For more samples, see [ReducePairElem sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_pair_elem).

- `ReducePairElem` tensor high-dimensional split computation example - `mask` continuous mode:

    ```cpp
    // Set mask so that all 128 elements participate in the computation.
    int32_t mask = 256/sizeof(half);
    // Each repeat contains 128 elements, 128 elements in total.
    int repeat = 1;
    // dstLocal: destination operand tensor
    // srcLocal: source operand tensor
    // srcBlkStride = 1, no gap between blocks within a repeat.
    // dstRepStride = 1, srcRepStride = 8, no gap between repeats.
    AscendC::ReducePairElem<AscendC::ReduceType::SUM, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

- `ReducePairElem` tensor high-dimensional split computation example - `mask` bit-by-bit mode:

    ```cpp
    // Set mask so that all 128 elements participate in the computation.
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // Each repeat contains 128 elements, 128 elements in total.
    int repeat = 1;
    // dstLocal: destination operand tensor.
    // srcLocal: source operand tensor.
    // srcBlkStride = 1, no gap between blocks within a repeat.
    // dstRepStride = 1, srcRepStride = 8, no gap between repeats.
    AscendC::ReducePairElem<AscendC::ReduceType::SUM, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

- Example result

    > Input data src_gm:

    ```plain
    [1, 1, 1, -1, 2, 2, -1, 2,
     3, 3, 3, -1, 4, 4, -2, 4,
     ....
    ]
    ```

    > Output data dst_gm:
    
    ```plain
    [2, 0, 4, 1, 6, 2, 8, 2,
    ....
    ]
    ```
