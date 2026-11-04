# ReduceDataBlock

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:21:00.196Z pushedAt=2026-10-28T03:14:39.551Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceDataBlock_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_vec_reduce_intf.h"`.

As shown in Figure 1, the `ReduceDataBlock` API performs a reduction operation on the input data in units of DataBlock. Based on the template parameter `reduceType`, it computes the sum/maximum/minimum of the data within each DataBlock.

`ReduceDataBlock` performs multiple iterative calculations on the input data. In each iteration, it takes 8 DataBlocks (each DataBlock has contiguous internal addresses and a size of 32 bytes) for calculation, and the output results are written to the destination address contiguously.

**Figure 1**  `ReduceDataBlock` reduction diagram

![ReduceDataBlock reduction diagram](../../../../figures/ReduceDataBlock.png "ReduceDataBlock reduction diagram")

## Function Prototype

- `mask` bit-by-bit mode:

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReduceDataBlock(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
    ```

- `mask` contiguous mode:

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReduceDataBlock(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
    ```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ---- |
| reduceType | Reduction operation type.<br>The type is the `ReduceType` enum type, which supports:<br>&bull; `ReduceType::SUM`: sum;<br>&bull; `ReduceType::MAX`: maximum;<br>&bull; `ReduceType::MIN`: minimum. |
| T | Data type of the destination operand. |
| U | Data type of the source operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API, and you need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2**  Parameters

| Parameter | Input/Output | Meaning |
| -------- | -------- | ---- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| src | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| mask[]/mask | Input | `mask` controls the source operands that participate in the calculation within each iteration. For details, see [Mask Overview](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of iterations. The value range is [0, 255]. |
| dstRepStride | Input | Address stride between adjacent iterations of the destination operand.<br>It is measured in units of the length after one `repeatTime` reduction. For the `half` data type, the unit is 16 bytes; for the `float` data type, the unit is 32 bytes. The value range is [0, $2^{16}-1$].<!-- npu="910" id29 --><br>**Note: Atlas training products do not support configuring 0.**<!-- end id29 --> |
| srcBlkStride | Input | Address stride between DataBlocks within a single iteration of the source operand, in units of 32 bytes. The value range is [0, $2^{16}-1$]. |
| srcRepStride | Input | Address stride between adjacent iterations of the source operand, that is, the number of DataBlocks skipped by the source operand in each iteration. The value range is [0, $2^{16}-1$]. |

**Note: For the high-dimensional split related parameters `mask`, `repeatTime`, `dstRepStride`, `srcBlkStride`, and `srcRepStride` above, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md).**

## Data Types

The supported data types are as follows:

<!-- npu="950" id10 -->
- Ascend 950PR&950DT products support half and float.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 products support half and float.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 products support half and float.
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference products support half and float.
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core support half and float.
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas training products support half.
<!-- end id15 -->

The data types of the destination operand and the source operand must be consistent.

## Return Value

None

## Constraints

- For source operand alignment constraints, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#general-address-alignment-constraints). The start address must be 32-byte aligned. The alignment constraint on the start address of the destination operand depends on the operand data type: it must be 16-byte aligned for the `half` data type and 32-byte aligned for the `float` data type.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- The value range of `dstRepStride`, `srcBlkStride`, and `srcRepStride` is [0, $2^{16}-1$]. Use them together with the actual size of the Unified Buffer (UB) to avoid out-of-bounds access.

<!-- npu="950,A3,910b" id18 -->
- When the value of the mask or repeatTime parameter is 0, the API behaves as follows:
  <!-- npu="A3,910b" id19 -->
  - For the following models, when the value of the mask or repeatTime parameter is 0, no computation is performed and no value is written to the destination operand. The API is treated as a NOP (no operation).
    <!-- npu="A3" id20 -->
    - Atlas A3 products
    <!-- end id20 -->
    <!-- npu="910b" id21 -->
    - Atlas A2 products
    <!-- end id21 -->
  <!-- end id19 -->

  <!-- npu="950" id22 -->
  - For Ascend 950PR&950DT products, this API implements compatibility by calling the [Reg vector compute](../../reg_vector_compute/reg_vector_compute.md) API through VF. When the value of the count or repeatTime parameter is 0, it is not guaranteed that the API is treated as a NOP (no operation).
  <!-- end id22 -->
<!-- end id18 -->

<!-- npu="950" id23 -->
- UB space usage. For Ascend 950PR&950DT products:
  - Tensor high-dimensional split computation occupies 8 KB of UB.
  - Continuous computation of the first n data elements of a tensor does not involve the 8 KB UB usage.
<!-- end id23 -->

<!-- npu="A3,910b" id24 -->
- For the following models, if the `mask[]/mask` parameter is configured and no element in a DataBlock participates in the computation, no value is written to the corresponding destination operand, and the original data in the destination operand is retained.
  <!-- npu="A3" id25 -->
  - Atlas A3 products
  <!-- end id25 -->
  <!-- npu="910b" id26 -->
  - Atlas A2 products
  <!-- end id26 -->
<!-- end id24 -->

<!-- npu="310b" id27 -->
- For Atlas 200I/500 A2 inference products, if the `mask[]/mask` parameter is configured and no element in a DataBlock participates in the computation, the corresponding destination operand is filled with a default value. The default value depends on `reduceType` and the data type:
  - When `reduceType == ReduceType::SUM`, 0 is filled.
  - When `reduceType == ReduceType::MAX`, -inf is filled if the operand data type is `float`, and -65504 is filled if the operand data type is `half`.
  - `reduceType == ReduceType::MIN`, fill with inf when the operand data type is `float`, and fill with 65504 when the operand data type is `half`.
  - For example, in the `float` scenario, when `reduceType == ReduceType::MAX` and `mask=32`, that is, only the first four DataBlocks are calculated, the maximum values in the last four DataBlocks return -inf.
<!-- end id27 -->

<!-- npu="950" id28 -->
- For Ascend 950PR&950DT products, if after the `mask[]/mask` parameter is configured, none of the elements in a DataBlock participate in the calculation, the corresponding destination operand is filled with a default value. The default value depends on `reduceType` and the data type:
  - Fill with 0 when `reduceType == ReduceType::SUM`.
  - Fill with -inf when `reduceType == ReduceType::MAX`.
  - Fill with inf when `reduceType == ReduceType::MIN`.
  - For example, in the `float` scenario, when `reduceType == ReduceType::MAX` and `mask=32`, that is, only the first four DataBlocks are calculated, the maximum values in the last four DataBlocks return -inf.
<!-- end id28 -->

- The `float` data type supports only the register non-saturation mode, and the `half` data type uses the register non-saturation mode by default. For details about how to configure the register non-saturation/saturation mode, see [SetCtrlSpr(ISASI).md](../../special_register_access/SetCtrlSpr_ISASI.md).
    - The following figure illustrates the calculation process of the `half` data type in saturation mode when `reduceType` is `ReduceType::SUM`. The source operand is $[60000,60000,-30000,100]$. First, $60000+60000$ overflows, and the result is $65504$. Then, $-30000+100=-29900$ is calculated. Finally, $65504-29900=35604$ is calculated.

        **Figure 2**  Handling of numeric overflow in saturation mode for `ReduceDataBlock<ReduceType::SUM>`

        ![ReduceDataBlock<ReduceType::SUM> saturation mode numeric overflow case handling](../../../../figures/vcgadd_overflow.png "ReduceDataBlock<ReduceType::SUM> saturation mode numeric overflow case handling")

## Key Features

The API computes the result within each DataBlock in a binary-tree manner.

Taking the summation of `half` data as an example, each DataBlock contains 16 numbers. They are added pairwise in a binary-tree manner, and the calculation process is shown in the following figure:

1. data01 is obtained by adding data1 and data2, data02 is obtained by adding data3 and data4, ..., data07 is obtained by adding data13 and data14, and data08 is obtained by adding data15 and data16;
2. data001 is obtained by adding data01 and data02, data002 is obtained by adding data03 and data04, ..., data004 is obtained by adding data07 and data08;
3. By analogy, the destination operand is obtained as one `half` data value sum.

**Figure 3**  `ReduceDataBlock<ReduceType::SUM>` summation diagram

![ReduceDataBlock<ReduceType::SUM> summation diagram](../../../../figures/vcgadd_binary_add.png "ReduceDataBlock<ReduceType::SUM> summation diagram")

## Example

This example shows only part of the code in the `Compute` process. For more examples, see [ReduceDataBlock series reduction instruction examples](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_data_block).

- `ReduceDataBlock<ReduceType::MAX>`-tensor high-dimensional split calculation example-`mask` continuous mode:

    ```cpp
    // Set the mask so that all 128 elements participate in the calculation.
    int32_t mask = 256/sizeof(half);
    // 128 elements per repeat, 128 elements in total.
    int repeat = 1;
    // dstLocal: destination operand tensor.
    // srcLocal: source operand tensor.
    // srcBlkStride = 1, no gap between blocks within a repeat.
    // dstRepStride = 1, srcRepStride = 8, no gap between repeats.
    AscendC::ReduceDataBlock<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

    The result is as follows:
    
    > Input data src_gm:

    ```plain
    [1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 2, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 3, 1, 1,
     ...
     1, 1, 1, 4, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1,
    ]
    ```

    > Output data dst_gm: [2, 3, ..., 4]

- `ReduceDataBlock<ReduceType::MAX>`-tensor high-dimensional split calculation example-`mask` bit-by-bit mode:

    ```cpp
    // Set the mask so that all 128 elements participate in the calculation.
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // Each repeat has 128 elements, totaling 128 elements.
    int repeat = 1;
    // dstLocal: destination operand tensor
    // srcLocal: source operand tensor
    // srcBlkStride = 1, meaning there is no gap between blocks within a repeat.
    // dstRepStride = 1, srcRepStride = 8, meaning there is no gap between repeats.
    AscendC::ReduceDataBlock<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

    The result is as follows:

    > Input data src_gm:

    ```plain
    [1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 2, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 3, 1, 1,
     ...
     1, 1, 1, 4, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1,
    ]
    ```

    > Output data dst_gm: [2, 3, ..., 4]

- `ReduceDataBlock<ReduceType::MIN>`-tensor high-dimensional split calculation example-`mask` bit-by-bit mode:

    ```cpp
    // Set mask so that all 128 elements participate in the calculation.
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // Each repeat has 128 elements, totaling 128 elements.
    int repeat = 1;
    // dstLocal: destination operand tensor
    // srcLocal: source operand tensor
    // srcBlkStride = 1, no gap between blocks within one repeat.
    // dstRepStride = 1, srcRepStride = 8, no gap between repeats.
    AscendC::ReduceDataBlock<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

    The result is as follows:

    > Input data src_gm:

    ```plain
    [10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 2, 10, 10, 10, 10,
     10, 10, 10, 10, 10, 10, 10, 10,
     10, 10, 10, 10, 10, -3, 10, 10,
     ...
     10, 10, 10, 10, 10, 10, 10, 10,
     10, 4, 10, 10, 10, 10, 10, 10,
    ]
    ```

    > Output data dst_gm: [2, -3, ..., 4]

- `ReduceDataBlock<ReduceType::SUM>`-tensor high-dimensional split calculation example-`mask` bit-by-bit mode:

    ```cpp
    // Set mask so that all 128 elements participate in the computation.
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // 128 elements per repeat, 128 elements in total.
    int repeat = 1;
    // dstLocal: destination operand tensor
    // srcLocal: source operand tensor.
    // srcBlkStride = 1, meaning there is no gap between blocks within a repeat.
    // dstRepStride = 1 and srcRepStride = 8, meaning there is no gap between repeats.
    ReduceDataBlock<AscendC::ReduceType::SUM, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

    The result is as follows:

    > Input data src_gm:

    ```plain
    [1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1,
     2, 2, 2, 2, 2, 2, 2, 2,
     2, 2, 2, 2, 2, 2, 2, 2,
     ...
     3, 3, 3, 3, 3, 3, 3, 3,
     3, 3, 3, 3, 3, 3, 3, 3]
    ```

    > Output data dst_gm: [16, 32, ..., 48]
