# ReduceMin

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T09:38:23.824Z -->

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

The `ReduceMin` API is used to find the minimum value and its index from all input data.

The `ReduceMin` computation process is shown in the following figure: first, the minimum value and the repeat internal index are calculated in each repeat iteration, and these intermediate results are temporarily stored in the `sharedTmpBuffer` workspace; then, based on the intermediate results, the final minimum value and its index are obtained by continuing the repeat iterations. Note that the minimum value index obtained in each repeat iteration is the repeat internal index. When returning the final result, the minimum value index of the full data needs to be derived based on the iteration position and the repeat internal index.

**Figure 1**  `ReduceMin` computation diagram<a id="fig1"></a>

![ReduceMin computation diagram](../../../../figures/ReduceMin_scale.png "ReduceMin computation diagram")

## Prototype

- Calculation of the first n data of the tensor:

    ```cpp
    template <typename T>
    __aicore__ inline void ReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t count, bool calIndex = 0)
    ```

- High-dimensional slicing computation of the tensor:
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

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. |

**Table 2** Parameters

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN, VECCALC, and VECOUT (the storage location is Unified Buffer). |
| src | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN, VECCALC, and VECOUT (the storage location is Unified Buffer). |
| sharedTmpBuffer | Input | Stores intermediate results during instruction execution and provides the operation space required for internal computation. Pay special attention to the space size. For details, see [Key Feature Description](#key-feature-description).<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN, VECCALC, and VECOUT (the storage location is Unified Buffer). |
| count | Input | Number of elements that participate in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md). The maximum amount of data to be processed cannot exceed the UB size limit.  |
| mask/mask[] | Input | `mask` controls the source operands that participate in the computation in each iteration. For details, see [Mask Overview](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of iterations. For details about this parameter, see [High-dimensional Slicing](../SIMD_compute/high_dimension_slicing.md). **Note: Unlike high-dimensional slicing, `repeatTime` supports a larger value range, as long as it does not exceed the maximum value of int32_t.** |
| srcRepStride | Input | Address stride between adjacent iterations of the source operand, that is, the number of DataBlocks skipped by the source operand in each iteration. The value range is [0, $2^{16}-1$]. |
| calIndex | Input | Specifies whether to obtain the index of the minimum value. The type is bool, and the default value is `false`. Values:<br>&bull; `true`: Obtain both the minimum value and its index.<br>&bull; `false`: Do not obtain the index; obtain only the minimum value. |

**Note: For the high-dimensional slicing related parameters `mask`, `repeatTime`, and `srcRepStride` above, see [High-dimensional Slicing](../SIMD_compute/high_dimension_slicing.md).**

## Data Type

The supported data types are as follows:

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, and uint64_t are supported.
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: half and float are supported.
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: half and float are supported.
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: supports half and float.
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supports half and float.
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: supports half.
<!-- end id6 -->

## Return Value

None

## Constraints

- For the address alignment constraints of the source operand and `sharedTmpBuffer`, refer to [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912). The start address must be 32-byte aligned. For the start address alignment constraint of the destination operand, refer to [ReduceRepeat-Table 3](ReduceRepeat.md#tab3).
- For operand address overlap constraints, refer to [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- When `sharedTmpBuffer` is required, address overlap between `dst` and `sharedTmpBuffer` is supported (typically, `dst` requires less space than `sharedTmpBuffer`). In this case, `sharedTmpBuffer` must meet the required space. For details, see [Key Feature Description](#key-feature-description).

<!-- npu="A3,910b" id16 -->
- For the following models, when `mask=0` or `repeatTime=0`, the reduction operation is not executed, the destination operand is not written, and the API is treated as a `NOP` (no operation).
  <!-- npu="A3" id7 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id7 -->
  <!-- npu="910b" id8 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id8 -->
<!-- end id16 -->

- The value range of `srcRepStride` is [0, $2^{16}-1$]. It must be used in combination with the actual UB size to avoid out-of-bounds access.
- If multiple minimum values exist, this instruction writes the minimum index to the destination operand.
- The `dst` result is stored in the order of the minimum value followed by the minimum index. If the index is not required, only the minimum value is stored.
- When the input type is `half`, only index values up to 65535 (the maximum value representable by `uint16_t`) are supported.
- Indexes are stored in the data type of the operand. To read an index, convert its type to an integer type. Refer to [ReduceRepeat key feature description](ReduceRepeat.md#key-feature-description).
<!-- npu="950" id9 -->
- For Ascend 950PR/Ascend 950DT, the `int64_t`/`uint64_t` data types are supported only by the API for computing the first n data of a tensor.
<!-- end id9 -->
<!-- npu="950" id10 -->
- For Ascend 950PR/Ascend 950DT, because the internal algorithm implementation of the API differs, `sharedTmpBuffer` is not required. You can directly pass `src` or a `sharedTmpBuffer` of any size.
<!-- end id10 -->

<!-- npu="A3,910b,310b,310p,910" id17 -->
- For the following models, `sharedTmpBuffer` is required.
  <!-- npu="A3" id11 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id11 -->
  <!-- npu="910b" id12 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id12 -->
  <!-- npu="310b" id13 -->
  - Atlas 200I/500 A2 inference products
  <!-- end id13 -->
  <!-- npu="310p" id14 -->
  - Atlas inference products AI Core
  <!-- end id14 -->
  <!-- npu="910" id15 -->
  - Atlas training products
  <!-- end id15 -->
<!-- end id17 -->

## Key Feature Description

- **The index value requires forced type conversion**. For details, see [`ReduceRepeat` Key Feature Description](ReduceRepeat.md#key-feature-description).

- **Minimum required space setting for `sharedTmpBuffer`**: Similar to `ReduceMax`. For details, see [ReduceMax Key Feature Description](ReduceMax.md#key-feature-description).

## Example

For more examples, see [ReduceMin examples](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation).

- Tensor high-dimensional slicing computation example - `mask` continuous mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 8320 and the data is continuously arranged. An index value is required. Use the tensor high-dimensional slicing computation API, set repeatTime to 65, and set mask so that all elements participate in the computation.
    int32_t mask = 128;
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- Tensor high-dimensional slicing computation example - `mask` bit-by-bit mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 8320 and the data is continuously arranged. An index value is required. Use the tensor high-dimensional slicing computation API, set repeatTime to 65, and set mask so that all elements participate in the computation.
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- Tensor first-n-data computation example:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 8320 and the data is continuously arranged. An index value is required. Use the tensor first-n-data computation API.
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, 8320, true);
    ```

- <a name="li0437172884019"></a>Complete call example of the tensor high-dimensional slicing computation API:

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

    // // mask = 128, 128 elements per repeat, 512 elements in total for four repeats. calIndex = true, obtaining the index of the minimum value.
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, repeat, repStride, true);
    // Release the Tensor.
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    The example result is as follows:

    > Input data src_gm:
    > > [0.769    0.8584   0.1082   0.2715   0.1759   0.7646   0.6406   0.2944   0.4255   0.927    0.8022   0.04507  0.9688   0.919    0.3008   0.7144   0.3206   0.6753   0.8276
    > >  ...
    > >  0.304   ]
    >
    > Output data dst_gm:
    > > [0.01034,  2.104e-05] // 2.104e-05 needs to be converted using the reinterpret_cast method to obtain the index value 353.

- Complete call example of the API for calculating the first n data of a tensor:

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 288;
    // Initialize srcLocal, dstLocal, and sharedTmpBuffer.
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();

    // The level2 API calculates the first 288 data elements. calIndex is true to obtain the index of the minimum value.
    AscendC::ReduceMin<half>(dstLocal, srcLocal, sharedTmpBuffer, srcDataSize, true);
    // Release the Tensor.
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    The example result is as follows:

    > Input data src_gm:
    > > [0.556    0.5225   0.3623   0.214    0.556    0.0643   0.769    0.594    0.261    0.3652   0.911    0.924    0.386    0.3696   0.2296   0.5957   0.1709   0.79     0.8516
    > >  ...
    > >  0.2242  ]
    >
    > Output data dst_gm:
    > > [0.00393,  4.3e-06] // 4.3e-06 needs to be converted using the reinterpret_cast method to obtain the index value 72
