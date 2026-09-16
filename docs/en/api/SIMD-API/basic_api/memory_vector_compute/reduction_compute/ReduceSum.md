# ReduceSum

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T09:52:57.178Z -->

## Applicable Products

<!-- npu="950" id29 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id29 -->
<!-- npu="A3" id30 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id30 -->
<!-- npu="910b" id31 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id31 -->
<!-- npu="310b" id32 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id32 -->
<!-- npu="310p" id33 -->
- Atlas inference products AI Core: Supported
<!-- end id33 -->
<!-- npu="310p" id34 -->
- Atlas inference products Vector Core: Not supported
<!-- end id34 -->
<!-- npu="910" id35 -->
- Atlas training products: Supported
<!-- end id35 -->

## Description

Header file path: `"basic_api/kernel_operator_vec_reduce_intf.h"`

The `ReduceSum` API sums all input data. The computation process is as follows.

**Figure 1** `ReduceSum` computation diagram<a id="fig1"></a>

![](../../../../figures/ReduceSum_basic_api.png "ReduceSum computation diagram")

## Prototype

- Computation of the first n data of the tensor:

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void ReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t count)
    ```

- High-dimensional splitting computation of the tensor:
    - `mask` bit-by-bit mode:

        ```cpp
        template <typename T>
        __aicore__ inline void ReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const uint64_t mask[], const int32_t repeatTime, const int32_t srcRepStride)
        ```

    - `mask` continuous mode:

        ```cpp
        template <typename T>
        __aicore__ inline void ReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t mask, const int32_t repeatTime, const int32_t srcRepStride)
        ```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. |
| isSetMask | Whether to set the mask mode and mask value inside the API.<br>&bull; true: set inside the API.<br>&nbsp; For the API that computes the first n data of a tensor, the [Counter mode](../SIMD_compute/mask.md#mask-mode) of the mask is used inside the API. In general, keep the default value of isSetMask, which means that the mask mode and mask value are set inside the API based on the count parameter passed by the developer.<br>&bull; false: set outside the API.<br>&nbsp; The developer needs to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this case, the count input parameter does not take effect, and it is recommended to set it to 1.<br>This parameter does not take effect on some product models. For details, see [Constraints](#constraints). |

**Table 2** Parameters

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand.<br>Type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition values are VECIN, VECCALC, and VECOUT (storage location: Unified Buffer). |
| src | Input | Source operand.<br>Type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition values are VECIN, VECCALC, and VECOUT (storage location: Unified Buffer). |
| sharedTmpBuffer | Input | Stores intermediate results during instruction execution and provides the workspace required for internal computation. Pay special attention to the space size. For details, see [Key Features](#key-features).<br>Type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition values are VECIN, VECCALC, and VECOUT (storage location: Unified Buffer). |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md). The maximum amount of data to be processed cannot exceed the UB size limit. |
| mask/mask[] | Input | `mask` controls the source operands that participate in the computation in each iteration. For details, see [Mask Overview](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of iterations. For details about this parameter, see [High-Dimensional Splitting](../SIMD_compute/high_dimension_slicing.md). **Note: Unlike high-dimensional splitting, `repeatTime` supports a larger value range, as long as it does not exceed the maximum value of int32_t.** |
| srcRepStride | Input | Address stride between adjacent iterations of the source operand, that is, the number of DataBlocks skipped by the source operand in each iteration. The value range is [0, $2^{16}-1$]. |

**Note: For the high-dimensional splitting related parameters `mask`, `repeatTime`, and `srcRepStride`, see [High-Dimensional Splitting](../SIMD_compute/high_dimension_slicing.md).**

## Data Type

The supported data types are as follows:

<!-- npu="950" id6 -->
- Ascend 950PR/Ascend 950DT: half, float, int64_t, and uint64_t.
<!-- end id6 -->
<!-- npu="A3" id7 -->
- Atlas A3 training products/Atlas A3 inference products: half and float.
<!-- end id7 -->
<!-- npu="910b" id8 -->
- Atlas A2 training products/Atlas A2 inference products: half and float.
<!-- end id8 -->
<!-- npu="310b" id9 -->
- Atlas inference products: half and float.
<!-- end id9 -->
<!-- npu="310p" id10 -->
- Atlas inference products AI Core supports half and float.
<!-- end id10 -->
<!-- npu="910" id11 -->
- Atlas training products supports half.
<!-- end id11 -->

## Return Value

None

## Constraints

- For the address alignment constraints of the source operand and `sharedTmpBuffer`, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912). The start address must be 32-byte aligned. For the start address alignment constraint of the destination operand, see [ReduceRepeat-Table 3](ReduceRepeat.md#tab3).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- When `sharedTmpBuffer` is required, address overlap between `dst` and `sharedTmpBuffer` is supported (normally `dst` requires less space than `sharedTmpBuffer`). In this case, `sharedTmpBuffer` must meet the required space. For details, see [Key Features](#key-features).

<!-- npu="A3,910b" id21 -->
- For the following models, when `mask=0` or `repeatTime=0`, the reduction operation is not performed, the destination operand is not written, and this API is treated as a `NOP` (no operation).
  <!-- npu="A3" id12 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id12 -->
  <!-- npu="910b" id13 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id13 -->
<!-- end id21 -->

<!-- npu="950" id14 -->
- For Ascend 950PR/Ascend 950DT, the `int64_t`/`uint64_t` data types support only the API for computing the first n data of a tensor.
<!-- end id14 -->
- The value range of `srcRepStride` is [0, $2^{16}-1$]. It must be used in combination with the actual UB size to avoid out-of-bounds access.
<!-- npu="950,910,310p,310b" id1 -->
- For the following models, the template parameter `isSetMask` does not take effect. Keep its default value:
  <!-- npu="950" id2 -->
  - Ascend 950PR/Ascend 950DT
  <!-- end id2 -->
  <!-- npu="310b" id3 -->
  - Atlas 200I/500 A2 inference products
  <!-- end id3 -->
  <!-- npu="310p" id4 -->
  - Atlas inference products AI Core
  <!-- end id4 -->
  <!-- npu="910" id5 -->
  - Atlas training products
  <!-- end id5 -->
    <!-- end id1 -->

## Key Features

**The accumulation methods of `ReduceSum` are divided into two types:**

- Method 1: Tree accumulation is used within the same repeat, and the results of different repeats are accumulated in order.
- Method 2: Tree accumulation is used within the same repeat, and the results of different repeats are also accumulated using a tree.

The `ReduceSum` API completes the summation within each repeat by tree accumulation. For details, see [ReduceDataBlock Key Features](ReduceDataBlock.md#key-features).

**For the two accumulation methods above, the space required for `sharedTmpBuffer` is set as follows:**

- Method 1: Pass in a `sharedTmpBuffer` of any size. The `sharedTmpBuffer` will not be used.
- Method 2: Calculate the required space according to the following formula:

    ```cpp
    int typeSize = 2;                           // 2 bytes for half and 4 bytes for float
    int elementsPerBlock = 32 / typeSize;       // Number of elements stored in 1 datablock
    int elementsPerRepeat = 256 / typeSize;     // Number of elements processed per repeat
    int firstMaxRepeat = repeatTime;            // The first maximum repeat value
    // Note: For the tensor high-dimensional splitting computation API, firstMaxRepeat is repeatTime. For the tensor first-n-data computation API, firstMaxRepeat is count/elementsPerRepeat. When count is less than elementsPerRepeat, firstMaxRepeat is 1.
    int iter1OutputCount = firstMaxRepeat;                                              // Number of elements generated in the first round
    int iter1AlignEnd = DivCeil(iter1OutputCount, elementsPerBlock) * elementsPerBlock; // The number of elements generated in the first round, rounded up
    int finalWorkLocalNeedSize = iter1AlignEnd; // The final elements space size required by sharedTmpBuffer is the rounded-up number of elements generated in the first round
    ```

**The addition methods of `ReduceSum` for different hardware forms are as follows:**

<!-- npu="950" id15 -->
- Ascend 950PR/Ascend 950DT adopts method 2.
<!-- end id15 -->
<!-- npu="A3" id16 -->
- For Atlas A3 training products/Atlas A3 inference products, the tensor first-n-data computation API adopts method 1, and the tensor high-dimensional splitting computation API adopts method 2.
<!-- end id16 -->
<!-- npu="910b" id17 -->
- For Atlas A2 training products/Atlas A2 inference products, the tensor first-n-data computation API adopts method 1, and the tensor high-dimensional splitting computation API adopts method 2.
<!-- end id17 -->
<!-- npu="310b" id18 -->
- Atlas inference products adopts method 2.
<!-- end id18 -->
<!-- npu="310p" id19 -->
- Atlas inference products AI Core adopts method 2.
<!-- end id19 -->
<!-- npu="910" id20 -->
- Atlas training products adopts method 2.
<!-- end id20 -->

## Example

For more examples, see [ReduceSum examples](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation).

- Tensor high-dimensional splitting computation example - `mask` continuous mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 8320 and the data is continuously arranged. Use the tensor high-dimensional splitting computation API, set repeatTime to 65, and set mask so that all elements participate in the computation.
    int32_t mask = 128;
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8);
    ```

- Tensor high-dimensional splitting computation example - `mask` bit-by-bit mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 8320 and the data is continuously arranged. Use the tensor high-dimensional splitting computation API, set repeatTime to 65, and set mask so that all elements participate in the computation.
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8);
    ```

- Tensor first n data computation example:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 8320 and the data is continuously arranged. Use the tensor first n data computation API.
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, 8320);
    ```

- Example of the tensor high-dimensional splitting computation API:

    ```cpp
    #include "kernel_operator.h"
    int srcDataSize = 8320;
    int dstDataSize = 16;
    int mask = 128;
    int repStride = 8;
    int repeat = srcDataSize / mask; // 65 here

    // Initialize srcLocal, dstLocal, and sharedTmpBuffer.
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();
    // mask is 128, so 128 elements are computed at a time. 65 repeats complete the computation of 8320 numbers.
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, repeat, repStride);
    // Release the Tensor.
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    The example result is as follows:

    > Input data src_gm:
    > > [1. 1. 1. ... 1. 1. 1.]
    >
    > Output data dst_gm:
    > > [8320.    0.    0.    0.    0.    0.    0.    0.    0.    0.    0.    0.
    > >     0.    0.    0.    0.]

- Example of the tensor first-n-data computation API:

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 288;
    // Initialize srcLocal, dstLocal, and sharedTmpBuffer.
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();

    // The level2 API computes the first 288 numbers and calculates their sum.
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, srcDataSize);
    // Release the Tensor.
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    The example result is as follows:

    > Input data src_gm:
    > > [1. 1. 1. ... 1. 1. 1.]
    >
    > Output data dst_gm:
    > > [288.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.]
