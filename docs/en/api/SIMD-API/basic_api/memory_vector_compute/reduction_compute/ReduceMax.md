# ReduceMax

<!-- md-trans-meta sourceCommit=f266635ab179c912cc9641061c2d27246be9170c translatedAt=2026-11-05T01:34:16.429Z pushedAt=2026-11-18T01:25:30.728Z nl=2 -->

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
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceMax_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_vec_reduce_intf.h"`.

The `ReduceMax` API is used to find the maximum value and its index from all input data.

The `ReduceMax` computation process is as follows: first, the maximum value and the internal index within each repeat iteration are computed, and these intermediate results are temporarily stored in the `sharedTmpBuffer` workspace; then, based on the intermediate results, the final maximum value and maximum index are obtained by continuing the repeat iterations. Note that the maximum index obtained in each repeat iteration is the internal index within the repeat. When returning the final result, the maximum index of the full data must be derived from the iteration position and the internal index within the repeat.

**Figure 1**  `ReduceMax` computation diagram<a id="fig1"></a>

![ReduceMax computation diagram](../../../../figures/ReduceMax_scale.png "ReduceMax computation diagram")

## Function Prototype

- Computation of the first n data elements of the tensor:

    ```cpp
    template <typename T>
    __aicore__ inline void ReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t count, bool calIndex = 0)
    ```

- Tensor high-dimensional split computation:
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

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. |

**Table 2**  Parameters

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| src | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| sharedTmpBuffer | Input | Stores intermediate results during instruction execution and provides the operation space required for internal computation. Pay special attention to the space size. See [Key Features](#key-features) for details.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| count | Input | Number of elements involved in the computation. See [Continuous Computation](../SIMD_compute/continuous_compute.md) for details about this parameter. The maximum amount of data to be processed cannot exceed the Unified Buffer (UB) size limit. |
| mask/mask[] | Input | `mask` controls the source operands involved in the computation within each iteration. See [Mask Overview](../SIMD_compute/mask.md) for detailed settings. |
| repeatTime | Input | Number of iterations. See [High-Dimensional Splitting](../SIMD_compute/high_dim_split.md) for details about this parameter. **Note: Unlike in high-dimensional splitting, `repeatTime` supports a larger value range, as long as it does not exceed the maximum value of int32_t.** |
| srcRepStride | Input | Address stride between adjacent iterations of the source operand, that is, the number of DataBlocks skipped by the source operand in each iteration. The value range is [0, $2^{16}-1$]. |
| calIndex | Input | Whether to obtain the index of the maximum value. The type is bool, and the default value is `false`. Values:<br>&bull; `true`: Obtain both the maximum value and the maximum index.<br>&bull; `false`: Do not obtain the index; obtain only the maximum value. |

**Note: For the high-dimensional splitting related parameters `mask`, `repeatTime`, and `srcRepStride` above, see [High-Dimensional Splitting](../SIMD_compute/high_dim_split.md).**

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
- Atlas 200I/500 A2 Inference Products support half and float.
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas Inference products AI Core supports half and float.
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas Training products support half.
<!-- end id15 -->
<!-- npu="x90" id16 -->
- Kirin X90 processor products support half and float.
<!-- end id16 -->
<!-- npu="9030" id17 -->
- Kirin 9030 processor products support half and float.
<!-- end id17 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceMax_res.md#id2 -->

## Return Value

None

## Constraints

- For the address alignment constraints of the source operand and `sharedTmpBuffer`, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912). The start address must be 32-byte aligned. For the start address alignment constraint of the destination operand, see [ReduceRepeat-Table 3](./ReduceRepeat.md#tab3).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- When `sharedTmpBuffer` is required, address overlap between `dst` and `sharedTmpBuffer` is supported (typically `dst` requires less space than `sharedTmpBuffer`). In this case, `sharedTmpBuffer` must meet the required space. For details, see [Key Features](#key-features).

<!-- npu="950,A3,910b" id18 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id19 -->
  - For the following models, when the count or repeatTime parameter is set to 0, no computation is performed and no data is written to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id20 -->
    - Atlas A3 products
    <!-- end id20 -->
    <!-- npu="910b" id21 -->
    - Atlas A2 products
    <!-- end id21 -->
  <!-- end id19 -->

  <!-- npu="950" id22 -->
  - For Ascend 950PR&950DT products, this API is implemented for compatibility by calling the [Reg vector computation](../../reg_vector_compute/reg_vector_compute.md) API through VF. When the count or repeatTime parameter is set to 0, it is not guaranteed that this API is treated as a NOP (no operation).
  <!-- end id22 -->
<!-- end id18 -->

- The value range of `srcRepStride` is [0, $2^{16}-1$]. It must be combined with the actual UB size to avoid out-of-bounds access.
- If multiple maximum values exist, this instruction writes the minimum index to the destination operand.
- The `dst` result is stored in the order of the maximum value followed by the maximum index. If the index is not required, only the maximum value is stored.
- The index is stored in the data type of the operand. To read the index, convert the type to an integer. Refer to [ReduceRepeat key features](./ReduceRepeat.md#key-features).
- When the input type is `half`, only index values up to 65535 (the maximum value representable by `uint16_t`) are supported.

<!-- npu="950" id23 -->
- For Ascend 950PR&950DT products, because the internal algorithm implementation differs, `sharedTmpBuffer` is not required. You can directly pass `src` or a `sharedTmpBuffer` of any size.
<!-- end id23 -->

<!-- npu="A3,910b,910,310p,310b,x90,9030" id24 -->
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
  <!-- npu="x90" id30 -->
  - Kirin X90 processor products
  <!-- end id30 -->
  <!-- npu="9030" id31 -->
  - Kirin 9030 processor products
  <!-- end id31 -->
<!-- end id24 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceMax_res.md#id3 -->



## Key Features

- **The index value requires forced type conversion.** For details, see [ReduceRepeat Key Features](./ReduceRepeat.md#key-features).

- **Space required for `sharedTmpBuffer`:**

    The `sharedTmpBuffer` space must be applied for and passed in by you. Depending on whether the index needs to be obtained, the `sharedTmpBuffer` space is calculated differently: when the index needs to be returned, the space required for each round of computation must be accumulated, and the space for each round must meet the 32-byte alignment requirement of the UB space; when the index does not need to be returned, only the space required for the first round of computation needs to be provided and meet the 32-byte alignment requirement, and subsequent rounds can directly reuse this space. In this case, the index derivation process is not needed, so the intermediate data of previous rounds can be directly overwritten. The algorithm for calculating the required space is as follows:

    - When the maximum index does not need to be returned:

        ```cpp
        int firstMaxRepeat = repeatTime; // For the tensor high-dimensional split computation API, firstMaxRepeat is repeatTime; for the tensor first n data computation API, firstMaxRepeat is count/elementsPerRepeat.
        int iter1OutputCount = firstMaxRepeat * 2; // Number of elements produced by the first round of operations. Regardless of whether you need the index returned, the underlying instruction always returns the index, so space must be reserved for the index here. The number of elements produced is repeat count * 2.
        int iter1AlignEnd = DivCeil(iter1OutputCount, elementsPerBlock) * elementsPerBlock; // The number of elements produced by the first round is aligned upward by datablock (32 bytes).
        int finalWorkLocalNeedSize = iter1AlignEnd; // After the first round of computation completes, multiple subsequent iterations may still be required, but the same space can be reused. Therefore, the space required for the first round of computation is the final space size required by sharedTmpBuffer.
        ```

    - When the maximum index needs to be returned:

        ```cpp
        int firstMaxRepeat = repeatTime; 
        // For the tensor high-dimensional split computation API, firstMaxRepeat is repeatTime; for the tensor first n data computation API, firstMaxRepeat is count/elementsPerRepeat.
        int iter1OutputCount = firstMaxRepeat * 2;                                            // Number of elements produced by the first round of operations.
        int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock) * elementsPerBlock; // Start offset of the second round of operations, that is, the result of aligning the number of elements produced by the first round upward to datablock (32 bytes).
        // After the first round of computation completes, multiple subsequent iterations may still be required. In this case, the same space cannot be reused because the intermediate result index of the first round is still needed, so space for subsequent rounds must be prepared.
        int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat) * 2;              // Number of elements produced by the second round of operations.
        int iter2AlignEnd = RoundUp(iter2OutputCount, elementsPerBlock) * elementsPerBlock;   // Result of aligning the number of elements produced by the second round upward to datablock (32 bytes).
        int finalWorkLocalNeedSize = iter2AlignStart + iter2AlignEnd;                         // Final space size required by sharedTmpBuffer when the maximum value and index can be obtained after the second round.
        if (iter2OutputCount > 2) {                                                           // When the number of elements produced by the second round of operations is greater than 2, a third round of operations is required.
            int iter3AlignStart = iter2AlignEnd;                                              // Start offset of the third round of operations relative to the second round output space.
            int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat) * 2;          // Number of elements produced by the third round of operations.
            int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock; // Number of elements generated in the third round, rounded up to the datablock (32 bytes) alignment.
            finalWorkLocalNeedSize = iter2AlignStart + iter3AlignStart + iter3AlignEnd;       // Final space required for sharedTmpBuffer.
        }
        ```

    The final space size calculated above is in units of element count. To convert it to bytes, use `finalWorkLocalNeedSize * typeSize` (bytes).

    > [!NOTE]
    > To save address space, you can reuse the source operand space for `sharedTmpBuffer`. In this case, because the minimum space required by `sharedTmpBuffer` is always smaller than the source operand space, you do not need to consider or calculate the minimum space.

## Examples<a name="section107745237168"></a>

For detailed examples, refer to [ReduceMax examples](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation).

- Tensor high-dimensional splitting computation example - `mask` continuous mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 8320 and the data is contiguous. Index values are required. Use the tensor high-dimensional split computation API, set repeatTime to 65, and set mask so that all elements participate in the computation.
    int32_t mask = 128;
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- Tensor high-dimensional splitting computation example - `mask` bit-by-bit mode:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 8320 and the data is contiguous. Index values are required. Use the tensor high-dimensional split computation API, set repeatTime to 65, and set mask so that all elements participate in the computation.
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, 65, 8, true);
    ```

- Tensor first-n data computation example:

    ```cpp
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 8320 and the data is contiguous. Index values are required. Use the tensor first n data computation API.
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 8320, true);
    ```

- `sharedTmpBuffer` space computation example:

    ```cpp
    // Example 1 of sharedTmpBuffer computation for the ReduceMax API:
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 8320. Use the tensor high-dimensional split computation API, with repeatTime set to 65, mask set to 128, and index values required.
    // Example of calling the tensor high-dimensional split computation API:
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 128, 65, 8, true);
    // The minimum space required for sharedTmpBuffer is computed as follows:
    int RoundUp(int a, int b)
    {
        return (a + b - 1) / b;
    }
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = repeatTime;
    int iter1OutputCount = firstMaxRepeat * 2 = 130;                                          // Number of elements generated in the first round.
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 144; // Round up the output count of the first iteration.
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 4;                // Number of elements produced by the second iteration.
    int iter3AlignStart = RoundUp(iter2OutputCount, elementsPerBlock)*elementsPerBlock = 16;  // Round up the output count of the second iteration.
    int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat)*2 = 2;                // Number of elements produced by the third iteration.
    int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock = 16;  // Round up the number of elements produced by the third iteration.
    // The minimum space required for sharedTmpBuffer is iter2AlignStart + iter3AlignStart + iter3AlignEnd = 144 + 16 + 16 = 176, that is, 352 bytes.
    // Example 2 of sharedTmpBuffer computation for the ReduceMax API:
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data volume of srcLocal is 32640. The tensor high-dimensional split computation API is used, with repeatTime set to 255, mask set to 128, and index values required.
    // Example of calling the tensor high-dimensional split computation API:
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 128, 255, 8, true);
    // The minimum space required for sharedTmpBuffer is computed as follows:
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = repeatTime;
    int iter1OutputCount = firstMaxRepeat * 2 = 510;                                          // Number of elements produced in the first round of operations.
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 512; // Round up the output count of the first round of operations.
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 8;                // Number of elements produced in the second round of operations.
    int iter3AlignStart = RoundUp(iter2OutputCount, elementsPerBlock)*elementsPerBlock = 16;  // Round up the output count of the second round of operations.
    int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat)*2 = 2;                // Number of elements produced in the third round of operations.
    int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock = 16;  // Round up the number of elements produced in the third round.
    // The required space is iter2AlignStart + iter3AlignStart + iter3AlignEnd = 512 + 16 + 16 = 544, that is, 1088 bytes.
    // Example 3 of sharedTmpBuffer space calculation for the ReduceMax API:
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data volume of srcLocal is 65408. Use the tensor first n data computation API with count=65408, and index values are required.
    // Example of calling the tensor first n data computation API:
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 65408, true);
    // The minimum space required for sharedTmpBuffer is calculated as follows:
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = count / elementsPerRepeat = 511;
    int iter1OutputCount = firstMaxRepeat * 2 = 1022;                                          // Number of elements produced in the first round.
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 1024; // Round up the output count of iter1OutputCount.
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 16;                // Number of elements produced in the second round.
    int iter3AlignStart = RoundUp(iter2OutputCount, elementsPerBlock)*elementsPerBlock = 16;   // Round up the output count of iter2OutputCount.
    int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat)*2 = 2;                 // Number of elements produced in the third round.
    int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock = 16;   // Round up the number of elements produced in the third round.
    // The required space is iter2AlignStart + iter3AlignStart + iter3AlignEnd = 1024 + 16 + 16 = 1056, that is, 2112 bytes.
    // Example 4 of sharedTmpBuffer computation for the ReduceMax API:
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data volume of srcLocal is 512. The tensor high-dimensional split computation API is used, with repeatTime set to 4, mask set to 128, and index values required.
    // Example of calling the tensor high-dimensional split computation API:
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 128, 4, 8, true);
    // The minimum space required for sharedTmpBuffer is calculated as follows:
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = repeatTime;
    int iter1OutputCount = firstMaxRepeat * 2 = 8;                                           // Number of elements produced by the first round of operations.
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 16; // Round up the iter1OutputCount output count.
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 2;               // Number of elements produced by the second round of operations.
    // In this example, the second round of operations produces 2 elements, meaning the maximum value and its index are obtained at the end of the second round. Therefore, the required space is iter2AlignStart + RoundUp(iter2OutputCount, elementsPerBlock) * elementsPerBlock = 16 + 16 = 32, that is, 64 bytes.
    // Example 5 of sharedTmpBuffer calculation for the ReduceMax API:
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the half type. The computation data count of srcLocal is 65408. The tensor first n data computation API is used, with count = 65408 and no index value required.
    // Example of calling the tensor first n data computation API:
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, 65408, false);
    // The minimum space required for sharedTmpBuffer is calculated as follows:
    int typeSize = 2;
    int elementsPerBlock = 32 / typeSize = 16;
    int elementsPerRepeat = 256 / typeSize = 128;
    int firstMaxRepeat = count / elementsPerRepeat = 511;
    int iter1OutputCount = firstMaxRepeat * 2 = 1022;                                          // Number of elements produced in the first round.
    int iter1AlignEnd = RoundUp(iter1OutputCount, elementsPerBlock) * elementsPerBlock = 1024; // Round up the number of elements produced in the first round.
    // Because calIndex is false, the minimum space required for sharedTmpBuffer is the result of rounding up the elements produced in the first round, which is 1024 here, that is, 2048 bytes.
    // Example 6 of sharedTmpBuffer computation for the ReduceMax API:
    // dstLocal, srcLocal, and sharedTmpBuffer are all of the float type. The computation data volume of srcLocal is 8320. The tensor high-dimensional split computation API is used, repeatTime is 130, mask is 64, and the index value is required.
    // Example of calling the tensor high-dimensional split computation API:
    AscendC::ReduceMax<float>(dstLocal, srcLocal, sharedTmpBuffer, 64, 130, 8, true);
    // The minimum space required for sharedTmpBuffer is computed as follows:
    int typeSize = 4;
    int elementsPerBlock = 32 / typeSize = 8;
    int elementsPerRepeat = 256 / typeSize = 64;
    int firstMaxRepeat = repeatTime;
    int iter1OutputCount = firstMaxRepeat * 2 = 260;                                          // Number of elements produced in the first round.
    int iter2AlignStart = RoundUp(iter1OutputCount, elementsPerBlock)*elementsPerBlock = 264; // Round up the number of elements output in the first round.
    int iter2OutputCount = RoundUp(iter1OutputCount, elementsPerRepeat)*2 = 10;               // Number of elements produced in the second round.
    int iter3AlignStart = RoundUp(iter2OutputCount, elementsPerBlock)*elementsPerBlock = 16;  // Round up the output count of the second-round operation.
    int iter3OutputCount = RoundUp(iter2OutputCount, elementsPerRepeat)*2 = 2;                // Number of elements produced by the third-round operation.
    int iter3AlignEnd = RoundUp(iter3OutputCount, elementsPerBlock) * elementsPerBlock = 8;   // Round up the number of elements produced by the third round.
    // The minimum space required for sharedTmpBuffer is iter2AlignStart + iter3AlignStart + iter3AlignEnd = 264 + 16 + 8 = 288, that is, 1152 bytes.
    ```

- <a name="li7962161711114"></a>Complete example of the tensor high-dimensional split computation API:

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 512;
    int mask = 128;
    int repStride = 8;
    int repeat = srcDataSize / mask;

    // Initialize srcLocal, dstLocal, and sharedTmpBuffer.
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();
    // With mask set to 128, compute 128 elements at a time. Four repeats compute 512 numbers. calIndex is true to obtain the maximum index.
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, mask, repeat, repStride, true);
    // Release the tensors.
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    The example result is as follows:

    > Input data src_gm:

    ```plain
    [0.4795   0.951    0.866    0.008545 0.8037   0.551    0.754    0.73     0.6035   0.251    0.4841   0.05914  0.9414   0.379    0.664    0.6914   0.9307   0.3853   0.4048
     ...
     0.4106   0.604   ]
    ```

    > Output data dst_gm: [0.9985,  6.8e-06]
    > After conversion using the reinterpret_cast method, 6.8e-06 is the index value 114.

- Complete call example of the API for computing the first n data of a tensor:

    ```cpp
    #include "kernel_operator.h"

    int srcDataSize = 288;
    // Initialize srcLocal, dstLocal, and sharedTmpBuffer.
    AscendC::LocalTensor<half> srcLocal = inQueueSrc.DeQue<half>();
    AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
    AscendC::LocalTensor<half> sharedTmpBuffer = workQueue.AllocTensor<half>();

    // The level2 API computes the first 288 numbers. calIndex is true to obtain the maximum index.
    AscendC::ReduceMax<half>(dstLocal, srcLocal, sharedTmpBuffer, srcDataSize, true);
    // Release the tensors.
    outQueueDst.EnQue<half>(dstLocal);
    inQueueSrc.FreeTensor(srcLocal);
    workQueue.FreeTensor(sharedTmpBuffer);
    ```

    The example result is as follows:

    > Input data src_gm:

    ```plain
    [0.4778   0.5903   0.2433   0.698    0.1943   0.407    0.891    0.1766   0.5977   0.9473   0.6523   0.10913  0.0143   0.86     0.2366   0.625    0.3696   0.708    0.946
     ...
     0.262   ]
    ```

    > Output data dst_gm: [0.999,  1.38e-05]
    > After conversion using the reinterpret_cast method, 1.38e-05 is index value 232
