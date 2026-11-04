# ReduceSum

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:31:03.364Z pushedAt=2026-10-28T03:14:39.562Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceSum_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_vec_reduce_intf.h"`.

The `ReduceSum` API sums all input data. The computation process is as follows.

**Figure 1** `ReduceSum` computation diagram<a id="fig1"></a>

![](../../../../figures/ReduceSum_basic_api.png "ReduceSum computation diagram")

## Function Prototype

- Computation of the first n data elements of the tensor:

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void ReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t count)
    ```

- Tensor high-dimensional split computation:
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

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. |
| isSetMask | Whether to set the mask mode and mask value inside the API.<br>&bull; true: set inside the API.<br>&nbsp; The API for computing the first n data elements of the tensor uses the [Counter mode](../SIMD_compute/mask.md#mask-mode) of mask internally. In general, keep the default value of isSetMask, which means the mask mode and mask value are set inside the API based on the count parameter passed by you.<br>&bull; false: set outside the API.<br>&nbsp; You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this case, the count input parameter does not take effect, and it is recommended to set it to 1.<br>For some product models, this parameter does not take effect. See [Constraints](#constraints) for details. |

**Table 2**  Parameters

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| src | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| sharedTmpBuffer | Input | Stores intermediate results during instruction execution and provides the operation space required for internal computation. Pay special attention to the space size. See [Key Features](#key-features) for details.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| count | Input | Number of elements involved in the computation. See [Continuous Computation](../SIMD_compute/continuous_compute.md) for details about this parameter. The maximum amount of data to be processed cannot exceed the UB size limit. |
| mask/mask[] | Input | `mask` controls the source operands involved in the computation within each iteration. See [Mask Overview](../SIMD_compute/mask.md) for detailed settings. |
| repeatTime | Input | Number of iterations. See [High-Dimensional Splitting](../SIMD_compute/high_dim_split.md) for details about this parameter. **Note: Unlike in high-dimensional splitting, `repeatTime` supports a larger value range, as long as it does not exceed the maximum value of int32_t.** |
| srcRepStride | Input | Address stride between adjacent iterations of the source operand, that is, the number of DataBlocks skipped by the source operand in each iteration. The value range is [0, $2^{16}-1$]. |

**Note: For the high-dimensional splitting related parameters `mask`, `repeatTime`, and `srcRepStride` above, see [High-Dimensional Splitting](../SIMD_compute/high_dim_split.md).**

## Data Types

The supported data types are as follows:

<!-- npu="950" id10 -->
- Ascend 950PR&950DT products support half, float, int32_t, uint32_t, int64_t, and uint64_t. The int64_t and uint64_t data types are supported only by the API for computing the first n data of a tensor.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 products support half and float.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 products support half and float.
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 Inference Products support half and float.
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core supports half and float.
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas training products support half.
<!-- end id15 -->

## Return Value

None

## Constraints

- For the address alignment constraints of the source operand and `sharedTmpBuffer`, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912). The start address must be 32-byte aligned. For the start address alignment constraint of the destination operand, see [ReduceRepeat-Table 3](./ReduceRepeat.md#tab3).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- When `sharedTmpBuffer` is required, address overlap between `dst` and `sharedTmpBuffer` is supported (typically `dst` requires less space than `sharedTmpBuffer`). In this case, `sharedTmpBuffer` must meet the required space. For details, see [Key Features](#key-features).

<!-- npu="950,A3,910b" id18 -->
- When the count or repeatTime parameter is set to 0, the API behaves as follows:
  <!-- npu="A3,910b" id19 -->
  - For the following models, when the count or repeatTime parameter is set to 0, no computation is performed and no data is written to the destination operand. The API is treated as a NOP (no operation).
    <!-- npu="A3" id20 -->
    - Atlas A3 products
    <!-- end id20 -->
    <!-- npu="910b" id21 -->
    - Atlas A2 products
    <!-- end id21 -->
  <!-- end id19 -->

  <!-- npu="950" id22 -->
  - For Ascend 950PR&950DT products, this API implements compatibility by calling the [Reg vector computation](../../reg_vector_compute/reg_vector_compute.md) API through VF. When the count or repeatTime parameter is set to 0, the API is not guaranteed to be treated as a NOP (no operation).
  <!-- end id22 -->
<!-- end id18 -->

- The value range of `srcRepStride` is [0, $2^{16}-1$]. It must be set based on the actual size of the Unified Buffer (UB) to avoid out-of-bounds access.

<!-- npu="950,910,310p,310b" id23 -->
- For the following models, the `isSetMask` template parameter does not take effect. Keep its default value:
  <!-- npu="950" id24 -->
  - Ascend 950PR&950DT products
  <!-- end id24 -->
  <!-- npu="310b" id25 -->
  - Atlas 200I/500 A2 inference products
  <!-- end id25 -->
  <!-- npu="310p" id26 -->
  - Atlas inference products AI Core
  <!-- end id26 -->
  <!-- npu="910" id27 -->
  - Atlas training products
  <!-- end id27 -->
<!-- end id23 -->

## Key Features

**The accumulation methods of `ReduceSum` are divided into the following types:**

- Method one: Binary tree accumulation is used within the same repeat, and the results of different repeats are accumulated in order.
- Method two: Binary tree accumulation is used within the same repeat, and the results of different repeats are also accumulated using binary tree accumulation.
- Method three: Data from different repeats is merged into one repeat through vector addition, and binary tree accumulation is used within the repeat.

The `ReduceSum` API performs summation within each repeat using binary tree accumulation. For details, see [ReduceDataBlock Key Features](./ReduceDataBlock.md#key-features).

**For the two accumulation methods above, the space required for `sharedTmpBuffer` is set as follows:**

- Method one: Pass a `sharedTmpBuffer` of any size. The `sharedTmpBuffer` is not used.
- Method two: Calculate the required space using the following formula:

    ```cpp
    int typeSize = 2;                           // 2 bytes for the half type, and 4 bytes for the float type.
    int elementsPerBlock = 32 / typeSize;       // Number of elements stored in one data block.
    int elementsPerRepeat = 256 / typeSize;     // Number of elements that can be processed in one repeat.
    int firstMaxRepeat = repeatTime;            // Finally determine the first maximum repeat value.
    // Note: For the tensor high-dimensional splitting computation API, firstMaxRepeat is repeatTime. For the tensor first-n-data computation API, firstMaxRepeat is count/elementsPerRepeat. When count is less than elementsPerRepeat, firstMaxRepeat is 1.
    int iter1OutputCount = firstMaxRepeat;                                              // Number of elements generated in the first round of operations.
    int iter1AlignEnd = DivCeil(iter1OutputCount, elementsPerBlock) * elementsPerBlock; // Round up the number of elements generated in the first round.
    int finalWorkLocalNeedSize = iter1AlignEnd; // The element space size required by the final sharedTmpBuffer is the result of rounding up the elements produced in the first round of operations.
    ```

**The `ReduceSum` accumulation methods for different hardware forms are as follows:**

<!-- npu="950" id30 -->
- Ascend 950PR&950DT products adopt different accumulation methods by scenario:
  - int64_t and uint64_t adopt Method Three.
  - In [Counter mode](../SIMD_compute/mask.md#mask-mode) with a data volume between 256 and 1024 bytes, Method Three is adopted; otherwise, Method Two is adopted.
  - In [Normal mode](../SIMD_compute/mask.md#mask-mode), Method Two is adopted.
<!-- end id30 -->
<!-- npu="A3" id31 -->
- For Atlas A3 products, the tensor first-n-data computation API adopts Method One, and the tensor high-dimensional splitting computation API adopts Method Two.
<!-- end id31 -->
<!-- npu="910b" id32 -->
- For Atlas A2 products, the tensor first-n-data computation API adopts Method One, and the tensor high-dimensional splitting computation API adopts Method Two.
<!-- end id32 -->
<!-- npu="310b" id33 -->
- For Atlas 200I/500 A2 Inference Products, Method Two is adopted.
<!-- end id33 -->
<!-- npu="310p" id34 -->
- For the AI Core of Atlas inference products, Method Two is adopted.
<!-- end id34 -->
<!-- npu="910" id35 -->
- Atlas training products use Method Two.
<!-- end id35 -->

## Example

For more examples, see [ReduceSum example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation).

- Tensor high-dimensional splitting computation example - `mask` continuous mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. srcLocal has 8320 data elements arranged contiguously. Use the tensor high-dimensional splitting computation interface, set repeatTime to 65, and set mask so that all elements participate in the computation.
    int32_t mask = 128;
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8);
    ```

- Tensor high-dimensional splitting computation example - `mask` bit-by-bit mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. srcLocal has 8320 data elements arranged contiguously. Use the tensor high-dimensional splitting computation interface, set repeatTime to 65, and set mask so that all elements participate in the computation.
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8);
    ```

- Tensor first-n data computation example:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. srcLocal has 8320 data elements arranged contiguously. Use the tensor first-n data computation interface.
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, 8320);
    ```

- Tensor high-dimensional splitting computation interface example:

    ```cpp
    #include "kernel_operator.h"
    int srcDataSize = 8320;
    int dstDataSize = 16;
    int mask = 128;
    int repStride = 8;
    int repeat = srcDataSize / mask; // 65 here.

    // Initialize srcLocal, dstLocal, and sharedTmpBuffer.
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();
    // With mask set to 128, 128 elements are computed at a time, and 65 repeats compute all 8320 numbers.
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, repeat, repStride);
    // Release the tensors.
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    The example result is as follows:

    > Input data src_gm: [1. 1. 1. ... 1. 1. 1.]
    >
    > Output data dst_gm: [8320.    0.    0.    0.    0.    0.    0.    0.    0.    0.   0.    0.    0.    0.    0.    0.]

- Example of the tensor first-n-data computation interface:

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 288;
    // Initialize srcLocal, dstLocal, and sharedTmpBuffer.
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();

    // The level2 API computes the first 288 numbers and their sum.
    AscendC::ReduceSum<half>(dstLocal, srcLocal, sharedTmpBuffer, srcDataSize);
    // Release the tensors.
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    The example result is as follows:

    > Input data src_gm: [1. 1. 1. ... 1. 1. 1.]
    >
    > Output data dst_gm: [288.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.   0.]
