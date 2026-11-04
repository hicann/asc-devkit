# ReduceRepeat

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:27:34.970Z pushedAt=2026-10-28T03:14:39.558Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceRepeat_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_vec_reduce_intf.h"`.

The `ReduceRepeat` API is used to perform a reduction operation on all data within each repeat. Based on the template parameter reduceType, it performs the sum/maximum/minimum operation, and writes the results to the destination address in order.

When computing the maximum or minimum value, the corresponding index can be returned. The returned index value is the internal index of each repeat. [High-dimensional split](../SIMD_compute/high_dim_split.md) computation is supported.

**Figure 1**  ReduceRepeat computation diagram

![](../../../../figures/ReduceRepeat.png "ReduceRepeat computation diagram")

## Function Prototype

- `mask` bit-by-bit mode:

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReduceRepeat(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, ReduceOrder order = ReduceOrder::ORDER_VALUE_INDEX)
    ```

- `mask` continuous mode:

    ```cpp
    template <ReduceType reduceType, typename T, typename U, bool isSetMask = true>
    __aicore__ inline void ReduceRepeat(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, ReduceOrder order = ReduceOrder::ORDER_VALUE_INDEX)
    ```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| reduceType | Reduction operation type.<br>The type is the `ReduceType` enum, which supports:<br>&bull; `ReduceType::SUM`: sum;<br>&bull; `ReduceType::MAX`: maximum value;<br>&bull; `ReduceType::MIN`: minimum value. |
| T | Data type of the destination operand. |
| U | Data type of the source operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2**  Parameters

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| src | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN, VECCALC, and VECOUT. |
| mask[]/mask | Input | `mask` controls the source operands that participate in the computation within each iteration. For details, see [Mask Overview](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of iterations. The value range is [0, 255]. |
| dstRepStride | Input | Address stride between adjacent iterations of the destination operand, in units of the length after one repeatTime reduction. The value range is [0, $2^{16}-1$].<br>**Note: The unit of dstRepStride is affected by the operand data type, `reduceType`, and `order` parameters.** See [Table 3](#tab3):<br>&bull; When both the index and the extreme value are returned, the unit is twice the byte length of the dst data type;<br>&bull; When only the extreme value is returned, the unit is the byte length of the dst data type;<br>&bull; When only the index is returned, the unit is the byte length of the uint32_t type.<!-- npu="910" id46 --><br>**Note: Atlas training products do not support configuring 0.**<!-- end id46 --> |
| srcBlkStride | Input | Address stride of the DataBlock within a single iteration, in units of 32 bytes. The value range is [0, $2^{16}-1$]. |
| srcRepStride | Input | Address stride between adjacent iterations of the source operand, that is, the number of DataBlocks skipped by the source operand in each iteration. The value range is [0, $2^{16}-1$]. |
| order | Input | When the reduction operation type is MAX or MIN, specifies the relative position of the maximum value and the index value in dst, as well as the return result behavior. The parameter type is the `ReduceOrder` enum, and the default value is `ORDER_VALUE_INDEX`.<br>The values of `ReduceOrder` are as follows:<br>&bull; `ORDER_VALUE_INDEX`: value is in the lower half, and the return result storage order is [value, index].<br>&bull; `ORDER_INDEX_VALUE`: index is in the lower half, and the return result storage order is [index, value].<br>&bull; `ORDER_ONLY_VALUE`: only the extreme value is returned, and the return result storage order is [value].<br>&bull; `ORDER_ONLY_INDEX`: only the index of the extreme value is returned, and the return result storage order is [index].<br>**Note: When the reduction operation type is SUM, this parameter does not take effect.** |

**Note: For the high-dimensional split related parameters `mask`, `repeatTime`, `dstRepStride`, `srcBlkStride`, and `srcRepStride` above, see [High-dimensional Split](../SIMD_compute/high_dim_split.md).**

## Data Types

**Supported data types are as follows:**

<!-- npu="950" id10 -->
- Ascend 950PR&950DT products support int16_t, uint16_t, half, int32_t, uint32_t, and float.
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
- Atlas inference products (AI Core) support half and float.
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas training products support half.
<!-- end id15 -->

<!-- npu="950" id18 -->
**For Ascend 950PR&950DT products:**

- When `reduceType` is `MAX` or `MIN`, the data type of the destination operand must be consistent with that of the source operand.
- When `reduceType` is `SUM`, the following data type combinations are supported:

    | Destination Operand Data Type T | Source Operand Data Type U |
    | --- | --- |
    | `int32_t` | `int16_t`, `int32_t` | 
    | `uint32_t` | `uint16_t`, `uint32_t` |
    | `half` | `half` |
    | `float` | `float` |

<!-- end id18 -->

<!-- npu="A3,910b,910,310p,310b" id19 -->
**For the following models, the data types of the destination operand and the source operand must be consistent.**

<!-- npu="A3" id20 -->
- Atlas A3 products
<!-- end id20 -->
<!-- npu="910b" id21 -->
- Atlas A2 products
<!-- end id21 -->
<!-- npu="310b" id22 -->
- Atlas 200I/500 A2 inference products
<!-- end id22 -->
<!-- npu="310p" id23 -->
- Atlas inference products AI Core
<!-- end id23 -->
<!-- npu="910" id24 -->
- Atlas training products
<!-- end id24 -->
<!-- end id19 -->

## Return Value

None

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- For source operand address alignment constraints, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#general-address-alignment-constraints). The start address must be 32-byte aligned. The start address alignment constraint of the destination operand is affected by the operand data type, `reduceType`, and `order` parameters. See [Table 3](#tab3).

    **Table 3** Destination operand address alignment constraints and dstRepStride unit

    <a id="tab3"></a>

    | Reduction Operation Type | Data Type | Destination Operand Address Alignment Constraint | dstRepStride Unit |
    | --- | --- | --- | --- |
    | `SUM` | `half` | 2-byte alignment | 2 bytes |
    | `SUM` | `float` | 4-byte alignment | 4 bytes |

    | Reduction Operation Type | Data Type | `order` | Destination Operand Address Alignment Constraint | dstRepStride Unit |
    | --- | --- | --- | --- | --- |
    | MAX/MIN | `half` | `ORDER_VALUE_INDEX` | 4-byte alignment | 4 bytes |
    | MAX/MIN | `half` | `ORDER_INDEX_VALUE` | 4-byte alignment | 4 bytes |
    | MAX/MIN | `half` | `ORDER_ONLY_VALUE` | 2-byte alignment | 2 bytes |
    | MAX/MIN | `half` | `ORDER_ONLY_INDEX` | 4-byte alignment | 4 bytes |
    | MAX/MIN | `float` | `ORDER_VALUE_INDEX` | 8-byte alignment | 8 bytes |
    | MAX/MIN | `float` | `ORDER_INDEX_VALUE` | 8-byte alignment | 8 bytes |
    | MAX/MIN | `float` | `ORDER_ONLY_VALUE` | 4-byte alignment | 4 bytes |
    | MAX/MIN | `float` | `ORDER_ONLY_INDEX` | 4-byte alignment | 4 bytes |

- The value range of `dstRepStride`, `srcBlkStride`, and `srcRepStride` is [0, $2^{16}-1$]. Ensure that no out-of-bounds access occurs based on the actual size of the Unified Buffer (UB).

<!-- npu="950,A3,910b" id27 -->
- When the mask or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id28 -->
  - For the following models, when the mask or repeatTime parameter is set to 0, no computation is performed and no write is made to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id29 -->
    - Atlas A3 products
    <!-- end id29 -->
    <!-- npu="910b" id30 -->
    - Atlas A2 products
    <!-- end id30 -->
  <!-- end id28 -->

  <!-- npu="950" id31 -->
  - For Ascend 950PR&950DT products, this API is implemented for compatibility by calling the [Reg vector compute](../../reg_vector_compute/reg_vector_compute.md) API through VF. When the mask or repeatTime parameter is set to 0, it is not guaranteed that this API is treated as a NOP (no operation).
  <!-- end id31 -->
<!-- end id27 -->

<!-- npu="950" id32 -->
- UB space usage. For Ascend 950PR&950DT products:
  - High-dimensional split computation of the tensor occupies 8 KB of UB.
  - Continuous computation of the first n data elements of the tensor does not involve the 8 KB UB usage.
<!-- end id32 -->
- Constraints when the reduction operation type is MAX or MIN:
  - If multiple extrema exist, this instruction writes the minimum index to the destination operand.
  - The index is stored in the data type of the operand. To read the index, convert the type to an integer. See [Key Features](#key-features).
  - The `ReduceOrder` supported by different chips is as follows:
    <!-- npu="950" id33 -->
    - Ascend 950PR&950DT products supports `ORDER_VALUE_INDEX`, `ORDER_INDEX_VALUE`, `ORDER_ONLY_VALUE`, and `ORDER_ONLY_INDEX`.
    <!-- end id33 -->
    <!-- npu="A3" id34 -->
    - Atlas A3 products supports `ORDER_VALUE_INDEX`, `ORDER_INDEX_VALUE`, `ORDER_ONLY_VALUE`, and `ORDER_ONLY_INDEX`.
    <!-- end id34 -->
    <!-- npu="910b" id35 -->
    - Atlas A2 products supports `ORDER_VALUE_INDEX`, `ORDER_INDEX_VALUE`, `ORDER_ONLY_VALUE`, and `ORDER_ONLY_INDEX`.
    <!-- end id35 -->
    <!-- npu="310b" id36 -->
    - Atlas 200I/500 A2 inference product supports `ORDER_VALUE_INDEX` and `ORDER_ONLY_VALUE`.
    <!-- end id36 -->
    <!-- npu="310p" id37 -->
    - Atlas inference products AI Core supports `ORDER_VALUE_INDEX` and `ORDER_INDEX_VALUE`.
    <!-- end id37 -->
    <!-- npu="910" id38 -->
    - Atlas training products supports `ORDER_VALUE_INDEX`.
    <!-- end id38 -->

## Key Features

- **Index values require forced type conversion**:

    The maximum/minimum value index of dst is stored according to the data type of the maximum/minimum value. For example, when dst is of the half type, the index is stored as the half type. Therefore, reading the index requires using the `reinterpret_cast` method to convert it to an integer type. If the maximum/minimum value data type is half, you need to use `reinterpret_cast<uint16_t*>`; if the maximum/minimum value data type is float, you need to use `reinterpret_cast<uint32_t*>`.

    For example, if the input data is of the half type and the calculation result is [0.9985,  6.8e-06], 6.8e-06 needs to be converted using the `reinterpret_cast<uint16_t*>` method to obtain the index value 114. The conversion example is as follows:

    ```cpp
    half maxIndex = dst.GetValue(1);
    uint16_t realIndex = *reinterpret_cast<uint16_t*>(&maxIndex);
    ```

    <!-- npu="950,A3,910b" id41 -->

    In particular,
    <!-- npu="950" id42 -->
    - For Ascend 950PR&950DT products, in the `ORDER_ONLY_INDEX` (return only the extreme value index) case, when the operand data type is `int16_t`, `uint16_t`, or `half`, use `reinterpret_cast<uint32_t*>` to read the index.
    <!-- end id42 -->

    <!-- npu="A3,910b" id43 -->
    - For the following models, in the `ORDER_ONLY_INDEX` (return only the extreme value index) case, use `reinterpret_cast<uint32_t*>` to read the index value.
      <!-- npu="A3" id44 -->
      - Atlas A3 products
      <!-- end id44 -->
      <!-- npu="910b" id45 -->
      - Atlas A2 products
      <!-- end id45 -->
    <!-- end id43 -->

    <!-- end id41 -->

- When the reduction operation type is `SUM`, the API sums each repeat using binary tree accumulation. For details, see [ReduceDataBlock key feature](./ReduceDataBlock.md#key-features).

## Example

For more examples, see [ReduceRepeat examples](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_repeat).

- `ReduceRepeat<ReduceType::MAX>` tensor high-dimensional split computation example - `mask` continuous mode:

    ```cpp
    // dstLocal and srcLocal are both of the half type. The computation data volume of srcLocal is 512, laid out contiguously, and the computation result must also be laid out contiguously. Use the tensor high-dimensional split computation API and set mask so that all 128 elements participate in the computation.
    // Based on the above information, infer that repeatTime is 4, dstRepStride is 1, srcBlkStride is 1, and srcRepStride is 8.
    // To obtain the maximum value and its index, and to store the result in the [value, index] order, use the default order. The API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, 128, 4, 1, 1, 8);

    // To obtain the maximum value and its index, and to store the result in the [index, value] order, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, 128, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_INDEX_VALUE);

    // To obtain only the maximum value, and to store the [value] result, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, 128, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_VALUE);

    // If only the index is required and the [index] result needs to be stored, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, 128, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_INDEX);
    ```

- `ReduceRepeat<AscendC::ReduceType::MAX>` tensor high-dimensional split computation example - `mask` bit-by-bit mode:

    ```cpp
    // dstLocal and srcLocal are both of the half type. The computation data volume of srcLocal is 512, arranged contiguously, and the computation result also needs to be arranged contiguously. Use the tensor high-dimensional split computation API and set mask so that all 128 elements participate in the computation.
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };

    // Based on the above information, infer that repeatTime is 4, dstRepStride is 1, srcBlkStride is 1, and srcRepStride is 8.

    // If the maximum value and index are required and the result needs to be stored in the [value, index] order, use the default order. The API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8);

    // If the maximum value and index are required and the result needs to be stored in the [index, value] order, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_INDEX_VALUE);

    // If only the maximum value is required and the [value] result needs to be stored, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_VALUE);

    // If only the index is required and the [index] result needs to be stored, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_INDEX);
    ```

    The example result is as follows:

    > Input data src_gm:

    ```plain
    [1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1
     1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1
     1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1
     1   1   1   1   11   1   1   1   1   1   1   1   1   1   1   1
     ...
     2   2   2   2   2   2   2   2   2   2   2   2   2   2   2   2
     2   2   2   2   12   2   2   2   2   2   2   2   2   2   2   2
     ...
     3   3   3   3   3   3   3   3   3   3   3   3   3   3   3   3
     3   3   3   13   3   3   3   3   3   3   3   3   3   3   3   3
     3   3   3   3   3   3   3   3   3   3   3   3   3   3   3   3
     3   3   3   3   3   3   3   3   3   3   3   3   3   3   3   3]
    ```

    > Output data dst_gm:
    > - If the `ReduceOrder` type is `ORDER_VALUE_INDEX` or the default: [11 3.09944e-06 12 5.96046e-06 ... 13 1.13249e-06]
    > - If the `ReduceOrder` type is `ORDER_INDEX_VALUE`: [3.09944e-06 11 5.96046e-06 12 ... 1.13249e-06 13]
    > - If the `ReduceOrder` type is `ORDER_ONLY_VALUE`: [11 12 ... 13 0 0 0 ...]
    > - If the `ReduceOrder` type is `ORDER_ONLY_INDEX`: [3.09944e-06 0 5.96046e-06 0 ... 1.13249e-06 0]
    >
    > Here, the index value is the half representation of the binary form of the int value. Taking the above results as an example:
    > - Among the first 128 numbers, the position of 11 in the corresponding repeat is 52, whose hexadecimal value is 0x0034, corresponding to a half value of 3.09944e-06.
    > - Among the second 128 numbers, the position of 12 in the corresponding repeat is 100, whose hexadecimal value is 0x0064, corresponding to a half value of 5.96046e-06.
    > - Among the last 128 numbers, the position of 13 in the corresponding repeat is 19, whose hexadecimal value is 0x0013, corresponding to a half value of 1.13249e-06.

- `ReduceRepeat<AscendC::ReduceType::MIN>` tensor high-dimensional split computation example - `mask` bit-by-bit mode:

    ```cpp
    // dstLocal and srcLocal are both of the half type. The computation data volume of srcLocal is 512, arranged contiguously, and the computation result also needs to be arranged contiguously. Use the tensor high-dimensional split computation API and set mask so that all 128 elements participate in the computation at most.
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    
    // Based on the above information, infer that repeatTime is 4, dstRepStride is 1, srcBlkStride is 1, and srcRepStride is 8.
    // To obtain the minimum value and its index, and store the result in the [value, index] order, use the default order. The API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8);
    
    // To obtain the minimum value and its index, and store the result in the [index, value] order, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_INDEX_VALUE);
    
    // To obtain only the minimum value, and store the [value] result, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_VALUE);
    
    // To obtain only the index, and store the [index] result, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_INDEX);
    ```

    The example results are as follows:

    > Input data src_gm:

    ```plain
    [10  10  10  10  10  10  10  10  10  10  10  10  10  10  10  10
     10  10  10  10  10  10  10  10  10  10  10  10  10  10  10  10
     10  10  10  10  10  10  10  10  10  10  10  10  10  10  10  10
     10  10  10  10  1   10  10  10  10  10  10  10  10  10  10  10
     ...
     20  20  20  20  20  20  20  20  20  20  20  20  20  20  20  20
     20  20  20  20  2   20  20  20  20  20  20  20  20  20  20  20
     ...
     30  30  30  30  30  30  30  30  30  30  30  30  30  30  30  30
     30  30  30  3   30  30  30  30  30  30  30  30  30  30  30  30
     30  30  30  30  30  30  30  30  30  30  30  30  30  30  30  30
     30  30  30  30  30  30  30  30  30  30  30  30  30  30  30  30]
    ```

    > Output data dst_gm:
    > - If the `ReduceOrder` type is `ORDER_VALUE_INDEX` or the default: [1 3.09944e-06 2 5.96046e-06 ... 3 1.13249e-06]
    > - If the `ReduceOrder` type is `ORDER_INDEX_VALUE`: [3.09944e-06 1 5.96046e-06 2 ... 1.13249e-06 3]
    > - If the `ReduceOrder` type is `ORDER_ONLY_VALUE`: [1 2 ... 3 0 0 0 ...]
    > - If the `ReduceOrder` type is `ORDER_ONLY_INDEX`: [3.09944e-06 0 5.96046e-06 0 ... 1.13249e-06 0]
    >
    > Here, the index value is the half representation of the binary form of the int value. Taking the above results as an example:
    > - Among the first 128 numbers, the position of 1 in the corresponding repeat is 52, whose hexadecimal value is 0x0034, corresponding to a half value of 3.09944e-06.
    > - Among the second 128 numbers, the position of 2 in the corresponding repeat is 100, whose hexadecimal value is 0x0064, corresponding to a half value of 5.96046e-06.
    > - Among the last 128 numbers, the position of 3 in the corresponding repeat is 19, whose hexadecimal value is 0x0013, corresponding to a half value of 1.13249e-06.

- `ReduceRepeat<ReduceType::SUM>` tensor high-dimensional split computation example - `mask` bit-by-bit mode:

    ```cpp
    // dstLocal and srcLocal are both of the half type. The computation data volume of srcLocal is 512, laid out contiguously, and the computation result also needs to be laid out contiguously. Use the tensor high-dimensional split computation API, and set mask so that all 128 elements participate in the computation at most.
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    
    // Based on the above information, infer that repeatTime is 4, dstRepStride is 1, srcBlkStride is 1, and srcRepStride is 8.
    AscendC::ReduceRepeat<AscendC::ReduceType::SUM, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8);
    ```
