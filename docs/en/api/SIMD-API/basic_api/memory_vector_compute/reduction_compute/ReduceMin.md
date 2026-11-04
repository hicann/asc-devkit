# ReduceMin

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:24:09.774Z pushedAt=2026-10-28T03:14:39.555Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceMin_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_vec_reduce_intf.h"`.

The `ReduceMin` API is used to find the minimum value and its index from all input data.

The `ReduceMin` computation process is shown in the following figure: first, the minimum value and the repeat internal index are computed in each repeat iteration, and these intermediate results are temporarily stored in the `sharedTmpBuffer` workspace; then, based on the intermediate results, the final minimum value and minimum value index are obtained by continuing the repeat iterations. Note that the minimum value index obtained in each repeat iteration is the repeat internal index. When returning the final result, the minimum value index of the full data must be derived from the iteration position and the repeat internal index.

**Figure 1**  `ReduceMin` computation schematic<a id="fig1"></a>

![ReduceMin computation schematic](../../../../figures/ReduceMin_scale.png "ReduceMin computation schematic")

## Function Prototype

- Computation on the first n data of the tensor:

    ```cpp
    template <typename T>
    __aicore__ inline void ReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t count, bool calIndex = 0)
    ```

- Tensor high-dimensional split computation:
    - `mask` bit-by-bit mode:

        ```cpp
        template <typename T>
        __aicore__ inline void ReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const uint64_t mask[], const int32_t repeatTime, const int32_t srcRepStride, bool calIndex = 0)
        ```

    - `mask` continuous mode:

        ```cpp
        template <typename T>
        __aicore__ inline void ReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t mask, const int32_t repeatTime, const int32_t srcRepStride, bool calIndex = 0)
        ```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. |

**Table 2**  Parameters

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| src | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| sharedTmpBuffer | Input | Stores intermediate results during instruction execution and provides the workspace required for internal computation. Pay special attention to the space size. See details in [Key Features](#key-features).<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| count | Input | Number of elements involved in the computation. For details about this parameter, refer to [Continuous Computation](../SIMD_compute/continuous_compute.md). The maximum amount of data to be processed cannot exceed the UB size limit.  |
| mask/mask[] | Input | `mask` controls the source operands involved in the computation within each iteration. For detailed settings, refer to [Mask Overview](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of iterations. For details about this parameter, refer to [High-Dimensional Splitting](../SIMD_compute/high_dim_split.md). **Note: Unlike in high-dimensional splitting, `repeatTime` supports a larger value range, as long as it does not exceed the maximum value of int32_t.** |
| srcRepStride | Input | Address stride between adjacent iterations of the source operand, that is, the number of DataBlocks skipped by the source operand in each iteration. The value range is [0, $2^{16}-1$]. |
| calIndex | Input | Specifies whether to obtain the index of the minimum value. It is of the bool type, with a default value of `false`. Values:<br>&bull; `true`: Obtain both the minimum value and the minimum value index.<br>&bull; `false`: Do not obtain the index; obtain only the minimum value. |

**Note: For the high-dimensional splitting related parameters `mask`, `repeatTime`, and `srcRepStride` above, refer to the introduction in [High-Dimensional Splitting](../SIMD_compute/high_dim_split.md).**

## Data Types

The supported data types are as follows:

<!-- npu="950" id10 -->
- Ascend 950PR&950DT products support int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, and uint64_t. The int64_t and uint64_t data types are supported only by the API for computing the first n data of a tensor.
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

## Return Value

None

## Constraints

- For the address alignment constraints of the source operand and `sharedTmpBuffer`, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912). The start address must be 32-byte aligned. For the start address alignment constraints of the destination operand, see [ReduceRepeat-Table 3](./ReduceRepeat.md#tab3).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- When `sharedTmpBuffer` is required, address overlap between `dst` and `sharedTmpBuffer` is supported (typically `dst` requires less space than `sharedTmpBuffer`). In this case, `sharedTmpBuffer` must meet the required space. For details, see [Key Features](#key-features).

<!-- npu="950,A3,910b" id18 -->
- When the count or repeatTime parameter is 0, this API behaves as follows:
  <!-- npu="A3,910b" id19 -->
  - For the following models, when the count or repeatTime parameter is 0, no computation is performed and no write is made to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id20 -->
    - Atlas A3 products
    <!-- end id20 -->
    <!-- npu="910b" id21 -->
    - Atlas A2 products
    <!-- end id21 -->
  <!-- end id19 -->

  <!-- npu="950" id22 -->
  - For Ascend 950PR&950DT products, this API is implemented for compatibility by calling the [Reg vector compute](../../reg_vector_compute/reg_vector_compute.md) API through VF. When the count or repeatTime parameter is 0, this API is not guaranteed to be treated as a NOP (no operation).
  <!-- end id22 -->
<!-- end id18 -->

- `srcRepStride` ranges from [0, $2^{16}-1$]. It must be set based on the actual size of the Unified Buffer (UB) to avoid out-of-bounds access.
- If multiple minimum values exist, this instruction writes the minimum index to the destination operand.
- The `dst` result is stored in the order of the minimum value followed by the minimum index. If the index is not required, only the minimum value is stored.
- The index is stored in the data type of the operand. To read the index, convert the type to an integer. Refer to [ReduceRepeat key features](./ReduceRepeat.md#key-features).
- When the input type is `half`, only index values up to 65535 (the maximum value representable by `uint16_t`) are supported.
<!-- npu="950" id23 -->
- For Ascend 950PR&950DT products, because the internal algorithm implementation differs, `sharedTmpBuffer` is not required. You can directly pass `src` or a `sharedTmpBuffer` of any size.
<!-- end id23 -->

<!-- npu="A3,910b,910,310p,310b" id24 -->
- For the following models, `sharedTmpBuffer` is required.
  <!-- npu="A3" id25 -->
  - Atlas A3 products
  <!-- end id25 -->
  <!-- npu="910b" id26 -->
  - Atlas A2 products
  <!-- end id26 -->
  <!-- npu="310b" id27 -->
  - Atlas 200I/500 A2 inference products
  <!-- end id27 -->
  <!-- npu="310p" id28 -->
  - Atlas inference products AI Core
  <!-- end id28 -->
  <!-- npu="910" id29 -->
  - Atlas training products
  <!-- end id29 -->
<!-- end id24 -->

## Key Features

- **The index value requires a forced type conversion**. For details, see [`ReduceRepeat` key features](./ReduceRepeat.md#key-features).

- **Minimum required space for `sharedTmpBuffer`**: Similar to `ReduceMax`. For details, see [ReduceMax key features](./ReduceMax.md#key-features).

## Example

For more examples, see [ReduceMin examples](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation).

- Tensor high-dimensional splitting computation example - `mask` continuous mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 8320 and is contiguously arranged. An index value is required. Use the tensor high-dimensional splitting compute API, set repeatTime to 65, and set mask so that all elements participate in the computation.
    int32_t mask = 128;
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- Tensor high-dimensional splitting computation example - `mask` bit-by-bit mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 8320 and is contiguously arranged. An index value is required. Use the tensor high-dimensional splitting compute API, set repeatTime to 65, and set mask so that all elements participate in the computation.
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- Tensor first-n-data computation example:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 8320 and is contiguously arranged. An index value is required. Use the tensor first-n-data compute API.
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, 8320, true);
    ```

- <a name="li0437172884019"></a>Complete call example of the tensor high-dimensional splitting compute API:

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 512;
    int dstDataSize = 512;
    int mask = 128;
    int repStride = 8;
    int repeat = srcDataSize / mask;

    // Initialize srcLocal, dstLocal, and sharedTmpBuffer.
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();

    // With mask set to 128, 128 elements are computed at a time. Four repeats compute all 512 numbers. calIndex is set to true to obtain the index of the minimum value.
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, repeat, repStride, true);
    // Release the tensors.
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    The result is as follows:

    > Input data src_gm:

    ```plain
    [0.769    0.8584   0.1082   0.2715   0.1759   0.7646   0.6406   0.2944   0.4255   0.927    0.8022   0.04507  0.9688   0.919    0.3008   0.7144   0.3206   0.6753   0.8276
     ...
     0.304   ]
    ```

    > Output data dst_gm: [0.01034,  2.104e-05]
    > 2.104e-05 needs to be converted using the reinterpret_cast method to obtain the index value 353.

- Complete call example of the API for computing the first n data of a tensor:

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 288;
    // Initialize srcLocal, dstLocal, and sharedTmpBuffer.
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();

    // The level2 API computes the first 288 numbers. calIndex is set to true to obtain the index of the minimum value.
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, srcDataSize, true);
    // Release the tensors.
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    The example result is as follows:

    > Input data src_gm:

    ```plain
    [0.556    0.5225   0.3623   0.214    0.556    0.0643   0.769    0.594    0.261    0.3652   0.911    0.924    0.386    0.3696   0.2296   0.5957   0.1709   0.79     0.8516
     ...
     0.2242  ]
    ```

    > Output data dst_gm: [0.00393,  4.3e-06]
    > 4.3e-06 needs to be converted to index value 72 using the reinterpret_cast method.
