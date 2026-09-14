# ReduceMax

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T09:29:14.245Z -->

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

The `ReduceMax` API is used to find the maximum value and its index from all input data.

The `ReduceMax` calculation process is shown in the following figure: first, the maximum value and the intra-repeat index are obtained in each repeat iteration, and these intermediate results are temporarily stored in the `sharedTmpBuffer` workspace; then, based on the intermediate results, the final maximum value and its index are obtained by continuing the repeat iterations. Note that the maximum index obtained in each repeat iteration is the intra-repeat index. When returning the final result, the maximum index of the full data must be derived based on the iteration position and the intra-repeat index.

**Figure 1**  `ReduceMax` calculation diagram<a id="fig1"></a>

![ReduceMax calculation diagram](../../../../figures/ReduceMax_scale.png "ReduceMax calculation diagram")

## Prototype

- Calculation on the first n data of the tensor:

    ```cpp
    template <typename T>
    __aicore__ inline void ReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t count, bool calIndex = 0)
    ```

- Computation with high-dimensional slicing of the tensor:
    - `mask` bit-by-bit mode:

        ```cpp
        template <typename T>
        __aicore__ inline void ReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const uint64_t mask[], const int32_t repeatTime, const int32_t srcRepStride, bool calIndex = 0)
        ```

    - `mask` continuous mode:

        ```cpp
        template <typename T>
        __aicore__ inline void ReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t mask, const int32_t repeatTime, const int32_t srcRepStride, bool calIndex = 0)
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
| sharedTmpBuffer | Input | Stores intermediate results during instruction execution and provides the workspace required for internal computation. Pay special attention to the workspace size. For details, see [Key Feature Description](#key-feature-description).<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN, VECCALC, and VECOUT (the storage location is Unified Buffer). |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md). The maximum amount of data to be processed cannot exceed the UB size limit. |
| mask/mask[] | Input | `mask` controls the source operands that participate in the computation in each iteration. For details, see [Mask Overview](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of iterations. For details about this parameter, see [High-Dimensional Slicing](../SIMD_compute/high_dimension_slicing.md). **Note: Unlike in high-dimensional slicing, `repeatTime` supports a larger value range, as long as it does not exceed the maximum value of int32_t.** |
| srcRepStride | Input | Address stride between adjacent iterations of the source operand, that is, the number of DataBlocks skipped by the source operand in each iteration. The value range is [0, $2^{16}-1$]. |
| calIndex | Input | Specifies whether to obtain the index of the maximum value. The type is bool, and the default value is `false`. Values:<br>&bull; `true`: Obtain both the maximum value and its index.<br>&bull; `false`: Do not obtain the index; obtain only the maximum value. |

**Note: For the high-dimensional slicing related parameters `mask`, `repeatTime`, and `srcRepStride`, see [High-Dimensional Slicing](../SIMD_compute/high_dimension_slicing.md).**

## Data Type

The supported data types are as follows:

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, and uint64_t are supported.
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training series products/Atlas A3 inference series products: half and float are supported.
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training series products/Atlas A2 inference series products: half and float are supported.
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: supports half and float.
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference series product AI Core: supports half and float.
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training series product: supports half.
<!-- end id6 -->

## Return Value

None

## Constraints

- For the address alignment constraints of the source operand and `sharedTmpBuffer`, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912). The start address must be 32-byte aligned. For the start address alignment constraint of the destination operand, see [ReduceRepeat-Table 3](ReduceRepeat.md#tab3).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
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

- The value range of `srcRepStride` is [0, $2^{16}-1$]. It must be used in combination with the actual size of the UB to avoid out-of-bounds access.
- If multiple maximum values exist, the instruction writes the minimum index to the destination operand.
- The `dst` result is stored in the order of the maximum value followed by the maximum index. If the index is not required, only the maximum value is stored.
- The index is stored according to the data type of the operand. To read the index, convert the type to an integer. For details, see [ReduceRepeat Key Feature Description](ReduceRepeat.md#key-feature-description).
- When the input type is `half`, only index values not exceeding 65535 (the maximum value representable by `uint16_t`) are supported.
<!-- npu="950" id9 -->
- For Ascend 950PR/Ascend 950DT, the `int64_t`/`uint64_t` data types support only the API for computing the first n data of a tensor.
<!-- end id9 -->
<!-- npu="950" id10 -->
- For Ascend 950PR/Ascend 950DT, because the internal algorithm implementation differs, `sharedTmpBuffer` is not required. You can directly pass `src` or a `sharedTmpBuffer` of any size.
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
  - Atlas training series products
  <!-- end id15 -->
<!-- end id17 -->

## Key Feature Description

- **The index value requires an explicit type conversion.** For details, see [ReduceRepeat Key Feature Description](ReduceRepeat.md#key-feature-description).

- **Space required for `sharedTmpBuffer`:**

    The `sharedTmpBuffer` space must be applied for and passed in by the developer. Depending on whether the index needs to be obtained, the space is calculated differently: if the index needs to be returned, the space required for each round of computation must be accumulated, and the space for each round must satisfy the 32-byte alignment requirement of the UB space; if the index does not need to be returned, only the space required for the first round of computation needs to be provided and must satisfy the 32-byte alignment requirement. Subsequent rounds can directly reuse this space. In this case, the index derivation process is not required, so the intermediate data of previous rounds can be directly overwritten. The algorithm for calculating the required space is as follows:

    - When the maximum index does not need to be returned:

        ```cpp
        int firstMaxRepeat = repeatTime; // For the tensor high-dimensional slicing API, firstMaxRepeat is repeatTime. For the API that computes the first n elements of a tensor, firstMaxRepeat is count/elementsPerRepeat.
        int iter1OutputCount = firstMaxRepeat * 2; // Number of elements produced in the first round. Regardless of whether the developer needs the index to be returned, the underlying instruction returns the index, so space must be reserved for the index here. The number of elements produced is repeat count * 2.
        int iter1AlignEnd = DivCeil(iter1OutputCount, elementsPerBlock) * elementsPerBlock; // Number of elements produced in the first round, aligned upward to a datablock (32 bytes).
        int finalWorkLocalNeedSize = iter1AlignEnd; // After the first round of computation, multiple subsequent rounds may still be required, but the same space can be reused. Therefore, the space required for the first round is the final space required for sharedTmpBuffer.
        ```

    - When the maximum index needs to be returned:

        ```cpp
        int firstMaxRepeat = repeatTime; 
        // For the tensor high-dimensional slicing computation API, firstMaxRepeat is repeatTime; for the tensor first-n-data calculation API, firstMaxRepeat is count/elementsPerRepeat
        int iter1OutputCount = firstMaxRepeat * 2;                                            // Number of elements produced by the first round of operations
        int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock) * elementsPerBlock; // Start offset of the second round of operations, that is, the result of rounding up the number of elements produced by the first round to a datablock (32 bytes)
        // After the first round of calculation is complete, multiple subsequent iterations may still be required. In this case, the same space cannot be reused because the intermediate result indexes of the first round still need to be used. Therefore, space for subsequent rounds must continue to be prepared
        int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat) * 2;              // Number of elements produced by the second round of operations
        int iter2AlignEnd = RoundUp(iter2OutputCount, elementsPerBlock) * elementsPerBlock;   // Result of rounding up the number of elements produced by the second round to a datablock (32 bytes)
        int finalWorkLocalNeedSize = iter2AlignStart + iter2AlignEnd;                         // Space size required by sharedTmpBuffer when the maximum value and index can be obtained after the second round
        if (iter2OutputCount > 2) {                                                           // When the number of elements produced by the second round is greater than 2, a third round of operations is required
            int iter3AlignStart = iter2AlignEnd;                                              // Start offset of the third round of operations relative to the second round output space
            int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat) * 2;          // Number of elements produced by the third round of operations
            int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock; // The number of elements generated in the third round, rounded up to the datablock (32 bytes) alignment
            finalWorkLocalNeedSize = iter2AlignStart + iter3AlignStart + iter3AlignEnd;       // The final space required by sharedTmpBuffer
        }
        ```

    The final space calculated above is in the unit of element count. To convert it to bytes, it is expressed as `finalWorkLocalNeedSize * typeSize` (bytes).

    > [!NOTE]Note
    > To save address space, developers can choose to reuse the source operand space for `sharedTmpBuffer`. In this case, because the minimum space required by `sharedTmpBuffer` is always smaller than the source operand space, there is no need to pay attention to or calculate the minimum space.

## Example<a name="section107745237168"></a>

For a detailed example, see [ReduceMax sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation).

- Tensor high-dimensional slicing computation sample - `mask` continuous mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data volume of srcLocal is 8320 and is continuously arranged. Index values are required. Use the tensor high-dimensional slicing computation API, set repeatTime to 65, and set mask so that all elements participate in the computation.
    int32_t mask = 128;
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- Tensor high-dimensional slicing computation sample - `mask` bit-by-bit mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data volume of srcLocal is 8320 and is continuously arranged. Index values are required. Use the tensor high-dimensional slicing computation API, set repeatTime to 65, and set mask so that all elements participate in the computation.
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- Tensor first-n-data computation sample:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data volume of srcLocal is 8320 and is continuously arranged. Index values are required. Use the tensor first-n-data computation API.
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 8320, true);
    ```

- `sharedTmpBuffer` space computation example:

    ```cpp
    // Computation Example 1 of sharedTmpBuffer for the ReduceMax API:
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The data count of srcLocal is 8320. The tensor High-Dimensional Splitting calculation API is used, repeatTime is 65, mask is 128, and the index value is required.
    // Example of the tensor High-Dimensional Splitting calculation API:
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 128, 65, 8, true);
    // In this case, the computation process of the minimum space required by sharedTmpBuffer is as follows:
    int RoundUp(int a, int b)
    {
        return (a + b - 1) / b;
    }
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = repeatTime;
    int iter1OutputCount = firstMaxRepeat * 2 = 130;                                          // Number of elements generated in the first round of operations
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 144; // Round up the number of outputs from the first round of operations
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 4;                // Number of elements generated in the second round of operations
    int iter3AlignStart = RoundUp(iter2OutputCount, elementsPerBlock)*elementsPerBlock = 16;  // Round up the number of outputs from the second round of operations
    int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat)*2 = 2;                // Number of elements generated in the third round of operations
    int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock = 16;  // Round up the number of elements generated in the third round
    // The final minimum space required by sharedTmpBuffer is iter2AlignStart + iter3AlignStart + iter3AlignEnd = 144 + 16 + 16 = 176, that is, 352 bytes.
    // Computation Example 2 of sharedTmpBuffer for the ReduceMax API:
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The data count of srcLocal is 32640. The tensor High-Dimensional Splitting calculation API is used, repeatTime is 255, mask is 128, and the index value is required.
    // Example of the tensor High-Dimensional Splitting calculation API:
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 128, 255, 8, true);
    // In this case, the computation process of the minimum space required by sharedTmpBuffer is as follows:
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = repeatTime;
    int iter1OutputCount = firstMaxRepeat * 2 = 510;                                          // Number of elements generated in the first round of operations
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 512; // Round up the number of outputs from the first round of operations
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 8;                // Number of elements generated in the second round of operations
    int iter3AlignStart = RoundUp(iter2OutputCount, elementsPerBlock)*elementsPerBlock = 16;  // Round up the number of outputs from the second round of operations
    int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat)*2 = 2;                // Number of elements generated in the third round of operations
    int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock = 16;  // Round up the number of elements generated in the third round
    // The required space is iter2AlignStart + iter3AlignStart + iter3AlignEnd = 512 + 16 + 16 = 544, that is, 1088 bytes.
    // Computation Example 3 of sharedTmpBuffer for the ReduceMax API:
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The data count of srcLocal is 65408. The API for computing the first n data of a tensor is used, with count = 65408, and the index value is required.
    // Example of the API for computing the first n data of a tensor:
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 65408, true);
    // In this case, the computation process of the minimum space required by sharedTmpBuffer is as follows:
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = count / elementsPerRepeat = 511;
    int iter1OutputCount = firstMaxRepeat * 2 = 1022;                                          // Number of elements generated in the first round of operations
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 1024; // Round up the number of iter1OutputCount outputs
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 16;                // Number of elements generated in the second round of operations
    int iter3AlignStart = RoundUp(iter2OutputCount, elementsPerBlock)*elementsPerBlock = 16;   // Round up the number of iter2OutputCount outputs
    int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat)*2 = 2;                 // Number of elements generated in the third round of operations
    int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock = 16;   // Round up the number of elements generated in the third round
    // The required space is iter2AlignStart + iter3AlignStart + iter3AlignEnd = 1024 + 16 + 16 = 1056, that is, 2112 bytes.
    // Computation Example 4 of sharedTmpBuffer for the ReduceMax API:
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The data count of srcLocal is 512. The tensor High-Dimensional Splitting calculation API is used, repeatTime is 4, mask is 128, and the index value is required.
    // Example of the tensor High-Dimensional Splitting calculation API:
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 128, 4, 8, true);
    // In this case, the computation process of the minimum space required by sharedTmpBuffer is as follows:
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = repeatTime;
    int iter1OutputCount = firstMaxRepeat * 2 = 8;                                           // Number of elements generated in the first round of operations
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 16; // Round up the number of iter1OutputCount outputs
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 2;               // Number of elements generated in the second round of operations
    // In this use case, because the number of elements generated in the second round of operations is 2, the maximum value and its index value can be obtained after the second round. Therefore, the required space is iter2AlignStart + RoundUp(iter2OutputCount, elementsPerBlock) * elementsPerBlock = 16 + 16 = 32, that is, 64 bytes.
    // Computation Example 5 of sharedTmpBuffer for the ReduceMax API:
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The data count of srcLocal is 65408. The API for computing the first n data of a tensor is used, with count = 65408, and no index value is required.
    // Example of the API for computing the first n data of a tensor:
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 65408, false);
    // In this case, the computation process of the minimum space required by sharedTmpBuffer is as follows:
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = count / elementsPerRepeat = 511;
    int iter1OutputCount = firstMaxRepeat * 2 = 1022;                                          // Number of elements generated in the first round of operations
    int iter1AlignEnd = RoundUp(iter1OutputCount, elementsPerBlock) * elementsPerBlock = 1024; // Round up the number of elements generated in the first round
    // Because calIndex is false, the final minimum space required by sharedTmpBuffer is the result of rounding up the elements generated in the first round, which is 1024 here, that is, 2048 bytes.
    // Computation Example 6 of sharedTmpBuffer for the ReduceMax API:
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the float type. The data count of srcLocal is 8320. The tensor High-Dimensional Splitting calculation API is used, repeatTime is 130, mask is 64, and the index value is required.
    // Example of the tensor High-Dimensional Splitting calculation API:
    AscendC::ReduceMax<float>(dstLocal, srcLocal, sharedTmpBuffer, 64, 130, 8, true);
    // In this case, the computation process of the minimum space required by sharedTmpBuffer is as follows:
    int typeSize = 4;
    int elementsPerBlock = 32 / typeSize = 8;
    int elementsPerRepeat = 256 / typeSize = 64;
    int firstMaxRepeat = repeatTime;
    int iter1OutputCount = firstMaxRepeat * 2 = 260;                                          // Number of elements generated in the first round of operations
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 264; // Round up the number of outputs from the first round of operations
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 10;               // Number of elements generated in the second round of operations
    int iter3AlignStart = RoundUp(iter2OutputCount, elementsPerBlock)*elementsPerBlock = 16;  // Round up the number of outputs from the second round of operations
    int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat)*2 = 2;                // Number of elements generated in the third round of operations
    int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock = 8;   // Round up the number of elements generated in the third round
    // The final minimum space required by sharedTmpBuffer is iter2AlignStart + iter3AlignStart + iter3AlignEnd = 264 + 16 + 8 = 288, that is, 1152 bytes.
    ```

- <a name="li7962161711114"></a>Complete example of the tensor high-dimensional slicing computation API:

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 512;
    int mask = 128;
    int repStride = 8;
    int repeat = srcDataSize / mask;

    // Initialize srcLocal, dstLocal, and sharedTmpBuffer
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();
    // mask = 128, 128 elements per repeat, 512 elements in total for four repeats. calIndex = true, obtaining the index of the maximum value.
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, repeat, repStride, true);
    // Release the Tensor.
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    The example result is as follows:

    > Input data src_gm:
    > > [0.4795   0.951    0.866    0.008545 0.8037   0.551    0.754    0.73     0.6035   0.251    0.4841   0.05914  0.9414   0.379    0.664    0.6914   0.9307   0.3853   0.4048
    > >  ...
    > >  0.4106   0.604   ]
    >
    > Output data dst_gm:
    > > [0.9985,  6.8e-06] // After conversion using the reinterpret_cast method, 6.8e-06 is the index value 114.

- Complete call example of the API for computing the first n data of a tensor:

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 288;
    // Initialize srcLocal, dstLocal, and sharedTmpBuffer.
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();

    // The level2 API computes the first 288 numbers. calIndex is true, so the index of the maximum value is obtained.
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, srcDataSize, true);
    // Release the Tensor.
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    The example result is as follows:

    > Input data src_gm:
    > > [0.4778   0.5903   0.2433   0.698    0.1943   0.407    0.891    0.1766   0.5977   0.9473   0.6523   0.10913  0.0143   0.86     0.2366   0.625    0.3696   0.708    0.946
    > >  ...
    > >  0.262   ]
    >
    > Output data dst_gm:
    > > [0.999,  1.38e-05] // After conversion using the reinterpret_cast method, 1.38e-05 is the index value 232.
