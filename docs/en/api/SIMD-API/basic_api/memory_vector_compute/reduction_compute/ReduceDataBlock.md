# ReduceDataBlock

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T09:23:22.711Z -->

## Applicable Products

<!-- npu="950" id21 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id21 -->
<!-- npu="A3" id22 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id22 -->
<!-- npu="910b" id23 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id23 -->
<!-- npu="310b" id24 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id24 -->
<!-- npu="310p" id25 -->
- Atlas inference products AI Core: Supported
<!-- end id25 -->
<!-- npu="310p" id26 -->
- Atlas inference products Vector Core: Not supported
<!-- end id26 -->
<!-- npu="910" id27 -->
- Atlas training products: Supported
<!-- end id27 -->

## Description

Header file path: `"basic_api/kernel_operator_vec_reduce_intf.h"`

As shown in Figure 1, the `ReduceDataBlock` API performs reduction on the input data in units of DataBlock. Based on the template parameter `reduceType`, it computes the sum/maximum/minimum of the data in each DataBlock.

`ReduceDataBlock` performs multiple iterative computations on the input data. In each iteration, it takes out 8 DataBlocks (each DataBlock has contiguous internal addresses and a size of 32 bytes) for computation, and the output results are written to the destination address consecutively.

**Figure 1**  `ReduceDataBlock` reduction diagram

![ReduceDataBlock reduction diagram](../../../../figures/ReduceDataBlock.png "ReduceDataBlock reduction diagram")

## Prototype

- `mask` bit-by-bit mode:

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReduceDataBlock(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
    ```

- `mask` continuous mode:

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReduceDataBlock(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
    ```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| ------ | ---- |
| reduceType | Reduction operation type.<br>The type is the `ReduceType` enum, which supports:<br>&bull; `ReduceType::SUM`: sum;<br>&bull; `ReduceType::MAX`: maximum value;<br>&bull; `ReduceType::MIN`: minimum value. |
| T | Data type of the destination operand. |
| U | Data type of the source operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Meaning |
| -------- | -------- | ---- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN, VECCALC, and VECOUT (stored in the Unified Buffer). |
| src | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN, VECCALC, and VECOUT (stored in the Unified Buffer). |
| mask[]/mask | Input | `mask` controls the source operands that participate in the computation in each iteration. For details, see [Mask Overview](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of iterations. The value range is [0, 255]. |
| dstRepStride | Input | Address stride between adjacent iterations of the destination operand.<br>The unit is the length after one `repeatTime` reduction. For the `half` data type, the unit is 16 bytes; for the `float` data type, the unit is 32 bytes. The value range is [0, $2^{16}-1$].<!-- npu="910" id20 --><br>**Note: Atlas training products do not support configuring 0.** <!-- end id20 -->|
| srcBlkStride | Input | Address stride between DataBlocks within a single iteration of the source operand, in 32 bytes. The value range is [0, $2^{16}-1$]. |
| srcRepStride | Input | Address stride between adjacent iterations of the source operand, that is, the number of DataBlocks skipped in each iteration of the source operand. The value range is [0, $2^{16}-1$]. |

**Note: For the high-dimensional split related parameters `mask`, `repeatTime`, `dstRepStride`, `srcBlkStride`, and `srcRepStride`, see [High-dimensional Split](../SIMD_compute/high_dimension_slicing.md).**

## Data Type

The supported data types are as follows:

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: half and float.
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: half and float.
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: half and float.
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: half and float.
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: half and float.
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products support half.
<!-- end id6 -->

The data types of the destination operand and the source operand must be consistent.

## Return Value

None

## Constraints

- For the alignment constraints of the source operand, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#general-address-alignment-constraints). The start address must be 32-byte aligned. The alignment constraint of the destination operand start address depends on the operand data type: 16-byte alignment is required for the `half` data type, and 32-byte alignment is required for the `float` data type.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- The value range of `dstRepStride`, `srcBlkStride`, and `srcRepStride` is [0, $2^{16}-1$]. Consider the actual size of the UB to avoid out-of-bounds access.

<!-- npu="A3,910b" id12 -->
- For the following models, when `mask=0` or `repeatTime=0`, the reduction operation is not executed, no value is written to the destination operand, and this API is treated as `NOP` (no operation).
  <!-- npu="A3" id7 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id7 -->
  <!-- npu="910b" id8 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id8 -->
- For the following models, if after configuring the `mask[]/mask` parameter, none of the elements in a DataBlock participate in the computation, no value is written to the corresponding destination operand, and the original data of the destination operand is retained.
  <!-- npu="A3" id9 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id9 -->
  <!-- npu="910b" id10 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id10 -->
<!-- end id12 -->

<!-- npu="310b" id11 -->
- For Atlas 200I/500 A2 inference products, if after configuring the `mask[]/mask` parameter, none of the elements in a DataBlock participates in the computation, the corresponding destination operand is padded with a default value. The default value depends on `reduceType` and the data type:
  - When `reduceType == ReduceType::SUM`, 0 is padded.
  - When `reduceType == ReduceType::MAX`, -inf is padded if the operand data type is `float`, and -65504 is padded if the operand data type is `half`.
  - `reduceType == ReduceType::MIN`, when the operand data type is `float`, inf is padded; the operand data type is `half`, 65504 is padded.
  - For example, in the `float` scenario, when `reduceType == ReduceType::MAX` and `mask=32`, that is, only the first four DataBlocks are calculated, the maximum value in the last four DataBlocks returns -inf.
<!-- end id11 -->

- The `float` data type supports only the register non-saturation mode, and the `half` data type uses the register non-saturation mode by default. For details about how to configure the register non-saturation/saturation mode, see [SetCtrlSpr_ISASI.md](../../special_register_access/SetCtrlSpr_ISASI.md).
    - The following figure illustrates the computation process of the `half` data type in saturation mode when `reduceType` is `ReduceType::SUM`. The source operand is $[60000,60000,-30000,100]$. First, $60000+60000$ overflows, and the result is $65504$. Then, $-30000+100=-29900$ is calculated. Finally, $65504-29900=35604$ is calculated.

        **Figure 2**  `ReduceDataBlock<ReduceType::SUM>` saturation mode numeric overflow handling

        ![ReduceDataBlock<ReduceType::SUM> saturation mode numeric overflow handling](../../../../figures/vcgadd_overflow.png "ReduceDataBlock<ReduceType::SUM> saturation mode numeric overflow handling")

## Key Features

The API computes the result in each DataBlock in the form of a binary tree.

Take the summation of `half`-type data as an example. Each DataBlock contains 16 numbers, which are added pairwise in a binary tree manner. The computation process is shown in the following figure:

1. data1 and data2 are added to obtain data01, data3 and data4 are added to obtain data02, ..., data13 and data14 are added to obtain data07, and data15 and data16 are added to obtain data08;
2. data01 and data02 are added to obtain data001, data03 and data04 are added to obtain data002, ..., and data07 and data08 are added to obtain data004;
3. By continuing this process, the destination operand is obtained as a single `half`-type value, sum.

**Figure 3**  `ReduceDataBlock<ReduceType::SUM>` summation diagram

![ReduceDataBlock<ReduceType::SUM> summation diagram](../../../../figures/vcgadd_binary_add.png "ReduceDataBlock<ReduceType::SUM> summation diagram")

## Example

This sample shows only part of the code in the `Compute` process. For more samples, see [ReduceDataBlock series reduction instruction samples](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_data_block).

- `ReduceDataBlock<ReduceType::MAX>`-tensor high-dimensional slicing computation example-`mask` continuous mode:

    ```cpp
    // Set mask so that all 128 elements participate in the computation.
    int32_t mask = 256/sizeof(half);
    // 128 elements per repeat, 128 elements in total.
    int repeat = 1;
    // dstLocal: destination operand tensor
    // srcLocal: source operand tensor
    // srcBlkStride = 1, no gap between blocks within a repeat.
    // dstRepStride = 1, srcRepStride = 8, no gap between repeats.
    AscendC::ReduceDataBlock<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

    The result is as follows:

    > Input data src_gm:
    > 
    > > [1, 1, 1, 1, 1, 1, 1, 1,
    > > 1, 1, 2, 1, 1, 1, 1, 1,
    > > 1, 1, 1, 1, 1, 1, 1, 1,
    > > 1, 1, 1, 1, 1, 3, 1, 1,
    > > ...
    > > 1, 1, 1, 4, 1, 1, 1, 1,
    > > 1, 1, 1, 1, 1, 1, 1, 1,
    > > ]
    >
    > Output data dst_gm:
    > > [2, 3, ..., 4]

- `ReduceDataBlock<ReduceType::MAX>`-tensor high-dimensional slicing computation example-`mask` bitwise mode:

    ```cpp
    // Set mask so that all 128 elements participate in the computation.
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // 128 elements per repeat, 128 elements in total.
    int repeat = 1;
    // dstLocal: destination operand tensor
    // srcLocal: source operand tensor
    // srcBlkStride = 1, no gap between blocks within a repeat.
    // dstRepStride = 1, srcRepStride = 8, no gap between repeats.
    AscendC::ReduceDataBlock<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

    The result is as follows:

    > Input data src_gm:
    > > [1, 1, 1, 1, 1, 1, 1, 1,
    > > 1, 1, 2, 1, 1, 1, 1, 1,
    > > 1, 1, 1, 1, 1, 1, 1, 1,
    > > 1, 1, 1, 1, 1, 3, 1, 1,
    > > ...
    > > 1, 1, 1, 4, 1, 1, 1, 1,
    > > 1, 1, 1, 1, 1, 1, 1, 1,
    > > ]
    > 
    > Output data dst_gm:
    > > [2, 3, ..., 4]

- `ReduceDataBlock<ReduceType::MIN>`-tensor high-dimensional slicing computation example-`mask` bit-by-bit mode:

    ```cpp
    // Set mask so that all 128 elements participate in the computation.
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // 128 elements per repeat, 128 elements in total.
    int repeat = 1;
    // dstLocal: destination operand tensor
    // srcLocal: source operand tensor
    // srcBlkStride = 1, no gap between blocks within a repeat.
    // dstRepStride = 1, srcRepStride = 8, no gap between repeats.
    AscendC::ReduceDataBlock<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

    The result is as follows:

    > Input data src_gm:
    > 
    > > [10, 10, 10, 10, 10, 10, 10, 10,
    > > 10, 10, 10, 2, 10, 10, 10, 10,
    > > 10, 10, 10, 10, 10, 10, 10, 10,
    > > 10, 10, 10, 10, 10, -3, 10, 10,
    > > ...
    > > 10, 10, 10, 10, 10, 10, 10, 10,
    > > 10, 4, 10, 10, 10, 10, 10, 10,
    > > ]
    >
    > Output data dst_gm:
    > > [2, -3, ..., 4]

- `ReduceDataBlock<ReduceType::SUM>`-tensor high-dimensional slicing computation example-`mask` bit-by-bit mode:

    ```cpp
    // Set mask so that all 128 elements participate in the computation.
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // 128 elements per repeat, 128 elements in total.
    int repeat = 1;
    // dstLocal: destination operand tensor
    // srcLocal: source operand tensor
    // srcBlkStride = 1, no gap between blocks within a repeat.
    // dstRepStride = 1, srcRepStride = 8, no gap between repeats.
    ReduceDataBlock<AscendC::ReduceType::SUM, half>(dstLocal, srcLocal, mask, repeat, 1, 1, 8);
    ```

    The result is as follows:

    > Input data src_gm:
    > 
    > > [1, 1, 1, 1, 1, 1, 1, 1,
    > > 1, 1, 1, 1, 1, 1, 1, 1,
    > > 2, 2, 2, 2, 2, 2, 2, 2,
    > > 2, 2, 2, 2, 2, 2, 2, 2,
    > > ...
    > > 3, 3, 3, 3, 3, 3, 3, 3,
    > > 3, 3, 3, 3, 3, 3, 3, 3]
    >
    > Output data dst_gm:
    > > [16, 32, ..., 48]
