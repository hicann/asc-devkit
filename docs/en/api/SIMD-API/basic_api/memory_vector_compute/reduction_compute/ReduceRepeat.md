# ReduceRepeat

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T09:47:50.603Z -->

## Applicable Products

<!-- npu="950" id36 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id36 -->
<!-- npu="A3" id37 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id37 -->
<!-- npu="910b" id38 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id38 -->
<!-- npu="310b" id39 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id39 -->
<!-- npu="310p" id40 -->
- Atlas inference products AI Core: Supported
<!-- end id40 -->
<!-- npu="310p" id41 -->
- Atlas inference products Vector Core: Not supported
<!-- end id41 -->
<!-- npu="910" id42 -->
- Atlas training products: Supported
<!-- end id42 -->

## Description

Header file path: `"basic_api/kernel_operator_vec_reduce_intf.h"`

The `ReduceRepeat` API performs a reduction operation on all data within each repeat. Based on the template parameter `reduceType`, it performs sum/maximum/minimum operations, and writes the results to the destination address in order.

When computing the maximum or minimum value, the corresponding index can be returned. The returned index value is the internal index of each repeat. [High-dimensional slicing](../SIMD_compute/high_dimension_slicing.md) computation is supported.

**Figure 1**  ReduceRepeat computation diagram

![](../../../../figures/ReduceRepeat.png "ReduceRepeat computation diagram")

## Prototype

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

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| reduceType | Reduction operation type.<br>The type is the `ReduceType` enum, which supports:<br>&bull; `ReduceType::SUM`: sum;<br>&bull; `ReduceType::MAX`: maximum;<br>&bull; `ReduceType::MIN`: minimum. |
| T | Destination operand data type. |
| U | Data type of the source operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN, VECCALC, and VECOUT (the storage location is Unified Buffer). |
| src | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN, VECCALC, and VECOUT (the storage location is Unified Buffer). |
| mask[]/mask | Input | `mask` controls the source operands that participate in the computation in each iteration. For details, see [Mask Overview](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of iterations. The value range is [0, 255]. |
| dstRepStride | Input | Address stride between adjacent iterations of the destination operand, in the unit of the length after one repeatTime reduction. The value range is [0, $2^{16}-1$].<br>**Note: The unit of dstRepStride is affected by the operand data type, `reduceType`, and `order` parameters.** See [Table 3](#tab3):<br>&bull; When both the index and the extreme value are returned, the unit is twice the byte length of the dst data type;<br>&bull; When only the extreme value is returned, the unit is the byte length of the dst data type;<br>&bull; When only the index is returned, the unit is the byte length of the uint32_t type.<!-- npu="910" id35 --><br>**Note: Atlas training products do not support configuring 0.** <!-- end id35 -->|
| srcBlkStride | Input | Address stride of the DataBlock within a single iteration, in the unit of 32 bytes. The value range is [0, $2^{16}-1$]. |
| srcRepStride | Input | Address stride between adjacent iterations of the source operand, that is, the number of DataBlocks skipped by the source operand in each iteration. The value range is [0, $2^{16}-1$]. |
| order | Input | When the reduction operation type is MAX or MIN, specifies the relative position of the maximum value and the index value in dst and the return result behavior. The parameter type is the `ReduceOrder` enum, and the default value is `ORDER_VALUE_INDEX`.<br>The values of `ReduceOrder` are as follows:<br>&bull; `ORDER_VALUE_INDEX`: indicates that value is in the lower half, and the return result storage order is [value, index].<br>&bull; `ORDER_INDEX_VALUE`: indicates that index is in the lower half, and the return result storage order is [index, value].<br>&bull; `ORDER_ONLY_VALUE`: indicates that only the extreme value is returned, and the return result storage order is [value].<br>&bull; `ORDER_ONLY_INDEX`: indicates that only the index of the extreme value is returned, and the return result storage order is [index].<br>**Note: When the reduction operation type is SUM, this parameter does not take effect.** |

**Note: For the high-dimensional slicing related parameters `mask`, `repeatTime`, `dstRepStride`, `srcBlkStride`, and `srcRepStride`, see [High-dimensional Slicing](../SIMD_compute/high_dimension_slicing.md).**

## Data Type

**The supported data types are as follows:**

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: int16_t, uint16_t, half, int32_t, uint32_t, and float are supported.
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: half and float are supported.
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: half and float are supported.
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: half and float are supported.
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: half and float are supported.
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products support half.
<!-- end id6 -->

<!-- npu="950" id7 -->
**For Ascend 950PR/Ascend 950DT:**

- When `reduceType` is `MAX` or `MIN`, the data types of the destination operand and the source operand must be the same.
- When `reduceType` is `SUM`, the following data type combinations are supported:

    | Destination Operand Data Type T | Source Operand Data Type U |
    | --- | --- |
    | `int32_t` | `int16_t`, `int32_t` | 
    | `uint32_t` | `uint16_t`, `uint32_t` |
    | `half` | `half` |
    | `float` | `float` |
<!-- end id7 -->

<!-- npu="A3,910b,310b,310p,910" id24 -->
**For the following models, the data types of the destination operand and the source operand must be the same.**

<!-- npu="A3" id8 -->
- Atlas A3 training products/Atlas A3 inference products
<!-- end id8 -->
<!-- npu="910b" id9 -->
- Atlas A2 training products/Atlas A2 inference products
<!-- end id9 -->
<!-- npu="310b" id10 -->
- Atlas 200I/500 A2 inference products
<!-- end id10 -->
<!-- npu="310p" id11 -->
- Atlas inference products AI Core
<!-- end id11 -->
<!-- npu="910" id12 -->
- Atlas training products
<!-- end id12 -->
<!-- end id24 -->

## Return Value

None

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- For the address alignment constraint of the source operand, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#general-address-alignment-constraints). The start address must be 32-byte aligned. The start address alignment constraint of the destination operand is affected by the operand data type, `reduceType`, and `order` parameters. For details, see [Table 3](#tab3).

    **Table 3**  Destination operand address alignment constraint and dstRepStride unit

    <a id="tab3"></a>

    | Reduction Operation Type | Data Type | Destination Operand Address Alignment Constraint | dstRepStride Unit |
    | --- | --- | --- | --- |
    | `SUM` | `half` | 2-byte alignment | 2-byte |
    | `SUM` | `float` | 4-byte alignment | 4-byte |

    | Reduction Operation Type | Data Type | `order` | Destination Operand Address Alignment Constraint | dstRepStride Unit |
    | --- | --- | --- | --- | --- |
    | MAX/MIN | `half` | `ORDER_VALUE_INDEX` | 4-byte alignment | 4-byte |
    | MAX/MIN | `half` | `ORDER_INDEX_VALUE` | 4-byte alignment | 4-byte |
    | MAX/MIN | `half` | `ORDER_ONLY_VALUE` | 2-byte alignment | 2-byte |
    | MAX/MIN | `half` | `ORDER_ONLY_INDEX` | 4-byte alignment | 4-byte |
    | MAX/MIN | `float` | `ORDER_VALUE_INDEX` | 8-byte alignment | 8-byte |
    | MAX/MIN | `float` | `ORDER_INDEX_VALUE` | 8-byte alignment | 8-byte |
    | MAX/MIN | `float` | `ORDER_ONLY_VALUE` | 4-byte alignment | 4-byte |
    | MAX/MIN | `float` | `ORDER_ONLY_INDEX` | 4-byte alignment | 4-byte |

- The value range of `dstRepStride`, `srcBlkStride`, and `srcRepStride` is [0, $2^{16}-1$]. Consider the actual size of the UB to avoid out-of-bounds access.

<!-- npu="A3,910b" id25 -->
- For the following models, when `mask=0` or `repeatTime=0`, the reduction operation is not executed, the destination operand is not written, and the API is treated as a `NOP` (no operation).
  <!-- npu="A3" id13 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id13 -->
  <!-- npu="910b" id14 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id14 -->
<!-- end id25 -->

- Constraints when the reduction operation type is MAX or MIN:
  - If multiple extrema exist, the instruction writes the smallest index to the destination operand.
  - The index is stored according to the data type of the operand. To read the index, convert the type to an integer. For details, see [Key Feature Description](#key-feature-description).
  - The `ReduceOrder` values supported by different chips are as follows:
    <!-- npu="950" id15 -->
    - Ascend 950PR/Ascend 950DT supports `ORDER_VALUE_INDEX`, `ORDER_INDEX_VALUE`, `ORDER_ONLY_VALUE`, and `ORDER_ONLY_INDEX`.
    <!-- end id15 -->
    <!-- npu="A3" id16 -->
    - Atlas A3 training products/Atlas A3 inference products support `ORDER_VALUE_INDEX`, `ORDER_INDEX_VALUE`, `ORDER_ONLY_VALUE`, and `ORDER_ONLY_INDEX`.
    <!-- end id16 -->
    <!-- npu="910b" id17 -->
    - Atlas A2 training products/Atlas A2 inference products support `ORDER_VALUE_INDEX`, `ORDER_INDEX_VALUE`, `ORDER_ONLY_VALUE`, and `ORDER_ONLY_INDEX`.
    <!-- end id17 -->
    <!-- npu="310b" id18 -->
    - Atlas 200I/500 A2 inference products support `ORDER_VALUE_INDEX` and `ORDER_ONLY_VALUE`.
    <!-- end id18 -->
    <!-- npu="310p" id19 -->
    - Atlas inference products AI Core support `ORDER_VALUE_INDEX` and `ORDER_INDEX_VALUE`.
    <!-- end id19 -->
    <!-- npu="910" id20 -->
    - Atlas training products support `ORDER_VALUE_INDEX`.
    <!-- end id20 -->

## Key Feature Description

- **Index value requires forced type conversion**:

    The maximum/minimum value index of dst is stored according to the data type of the maximum/minimum value. For example, when dst is of the half type, the index is stored as the half type. Therefore, to read the index, you need to use the `reinterpret_cast` method to convert it to an integer type. If the maximum/minimum value data type is half, use `reinterpret_cast<uint16_t*>`; if the maximum/minimum value data type is float, use `reinterpret_cast<uint32_t*>`.

    For example, the input data is of the half type, and the computation result is [0.9985, 6.8e-06]. To obtain the index value 114 from 6.8e-06, use the `reinterpret_cast<uint16_t*>` method. The conversion example is as follows:

    ```cpp
    half maxIndex = dst.GetValue(1);
    uint16_t realIndex = *reinterpret_cast<uint16_t*>(&maxIndex);
    ```

    <!-- npu="950,A3,910b" id27 -->
    In particular,
    <!-- npu="950" id21 -->
    - For Ascend 950PR/Ascend 950DT, in the `ORDER_ONLY_INDEX` (return only the maximum/minimum value index) case, when the operand data type is `int16_t`, `uint16_t`, or `half`, use `reinterpret_cast<uint32_t*>` to read the index.
    <!-- end id21 -->

    <!-- npu="A3,910b" id26 -->
    - For the following models, in the `ORDER_ONLY_INDEX` (return only the maximum/minimum value index) case, use `reinterpret_cast<uint32_t*>` to read the index value.
      <!-- npu="A3" id22 -->
      - Atlas A3 training products/Atlas A3 inference products
      <!-- end id22 -->
      <!-- npu="910b" id23 -->
      - Atlas A2 training products/Atlas A2 inference products
      <!-- end id23 -->
    <!-- end id26 -->
    <!-- end id27 -->

- When the reduction operation type is `SUM`, the API completes the summation within each repeat by means of binary tree accumulation. For details, see [ReduceDataBlock Key Feature Description](ReduceDataBlock.md#key-features).

## Example

For more examples, see [ReduceRepeat examples](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_repeat).

- `ReduceRepeat<ReduceType::MAX>` tensor high-dimensional slicing computation example - `mask` continuous mode:

    ```cpp
    // dstLocal and srcLocal are both of the half type. The computation data volume of srcLocal is 512, arranged contiguously, and the computation result also needs to be arranged contiguously. Use the tensor high-dimensional slicing computation API and set mask so that all 128 elements, the maximum number, participate in the computation
    // Based on the preceding information, infer that repeatTime is 4, dstRepStride is 1, srcBlkStride is 1, and srcRepStride is 8
    // To obtain the maximum value and its index, and to store the result in the [value, index] order, use the default order. The API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, 128, 4, 1, 1, 8);

    // To obtain the maximum value and its index, and to store the result in the [index, value] order, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, 128, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_INDEX_VALUE);

    // To obtain only the maximum value, and to store the [value] result, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, 128, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_VALUE);

    // To obtain only the index, and to store the [index] result, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, 128, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_INDEX);
    ```

- `ReduceRepeat<AscendC::ReduceType::MAX>` tensor high-dimensional slicing computation example - `mask` bit-by-bit mode:

    ```cpp
    // dstLocal and srcLocal are both of the half type. The computation data volume of srcLocal is 512, arranged contiguously, and the computation result also needs to be arranged contiguously. Use the tensor high-dimensional slicing computation API and set mask so that all 128 elements, the maximum number, participate in the computation
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };

    // Based on the preceding information, infer that repeatTime is 4, dstRepStride is 1, srcBlkStride is 1, and srcRepStride is 8

    // To obtain the maximum value and its index, and to store the result in the [value, index] order, use the default order. The API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8);

    // To obtain the maximum value and its index, and store the result in the [index, value] order, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_INDEX_VALUE);

    // To obtain only the maximum value, and store the result in the [value] order, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_VALUE);

    // To obtain only the index, and store the result in the [index] order, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MAX, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_INDEX);
    ```

    The example result is as follows:

    > Input data src_gm:
    > > [1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1
    > >  1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1
    > >  1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1
    > >  1   1   1   1   11   1   1   1   1   1   1   1   1   1   1   1
    > >  ...
    > >  3   3   3   3   3   3   3   3   3   3   3   3   3   3   3   3
    > >  3   3   3   13   3   3   3   3   3   3   3   3   3   3   3   3
    > >  3   3   3   3   3   3   3   3   3   3   3   3   3   3   3   3
    > >  3   3   3   3   3   3   3   3   3   3   3   3   3   3   3   3]
    >
    > Output data dst_gm:
    > > - If the `ReduceOrder` type is `ORDER_VALUE_INDEX` or the default: [11 3.09944e-06 12 5.96046e-06 ... 13 1.13249e-06]
    > > - If the `ReduceOrder` type is `ORDER_INDEX_VALUE`: [3.09944e-06 11 5.96046e-06 12 ... 1.13249e-06 13]
    > > - If the `ReduceOrder` type is `ORDER_ONLY_VALUE`: [11 12 ... 13 0 0 0 ...]
    > > - If the `ReduceOrder` type is `ORDER_ONLY_INDEX`: [3.09944e-06 0 5.96046e-06 0 ... 1.13249e-06 0]
    >
    > Here, the index value is the representation of the binary of an int value in half. Taking the above result as an example:
    > - Among the first 128 numbers, the position of 11 in the corresponding repeat is 52, whose hexadecimal value is 0x3400, corresponding to the half value 3.09944e-06.
    > - Among the second 128 numbers, the position of 12 in the corresponding repeat is 100, whose hexadecimal value is 0x6400, corresponding to the half value 5.96046e-06.
    > - Among the last 128 numbers, the position of 13 in the corresponding repeat is 19, whose hexadecimal value is 0x1300, corresponding to the half value 1.13249e-06.

- `ReduceRepeat<AscendC::ReduceType::MIN>` tensor high-dimensional slicing computation example - `mask` bit-by-bit mode:

    ```cpp
    // dstLocal and srcLocal are both of the half type. The computation data volume of srcLocal is 512, arranged contiguously, and the computation result also needs to be arranged contiguously. Use the tensor high-dimensional slicing computation API and set mask so that all 128 elements participate in the computation.
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    
    // Based on the above information, infer that repeatTime is 4, dstRepStride is 1, srcBlkStride is 1, and srcRepStride is 8.
    // To obtain the minimum value and its index, and store the result in the [value, index] order, use the default order. The API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8);
    
    // To obtain the minimum value and its index, and store the result in the [index, value] order, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_INDEX_VALUE);
    
    // If only the minimum value is required and the [value] result needs to be stored, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_VALUE);
    
    // If only the index is required and the [index] result needs to be stored, the API example is as follows:
    AscendC::ReduceRepeat<AscendC::ReduceType::MIN, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_INDEX);
    ```

    The example result is as follows:

    > Input data src_gm:
    > > [10   10   10   10   10   10   10   10   10   10   10   10   10   10   10   10
    > >  ...
    > >  30   30   30   30   30   30   30   30   30   30   30   30   30   30   30   30
    > >  30   30   30   3   30   30   30   30   30   30   30   30   30   30   30   30
    > >  30   30   30   30   30   30   30   30   30   30   30   30   30   30   30   30
    > >  30   30   30   30   30   30   30   30   30   30   30   30   30   30   30   30
    > >  30   30   30   30   30   30   30   30   30   30   30   30   30   30   30   30
    > >  30   30   30   30   30   30   30   30   30   30   30   30   30   30   30   30
    > >  30   30   30   30   30   30   30   30   30   30   30   30   30   30   30   30]
    >
    > Output data dst_gm:
    > > - If the `ReduceOrder` type is `ORDER_VALUE_INDEX` or the default: [1 3.09944e-06 2 5.96046e-06 ... 3 1.13249e-06]
    > > - If the `ReduceOrder` type is `ORDER_INDEX_VALUE`: [3.09944e-06 1 5.96046e-06 2 ... 1.13249e-06 3]
    > > - If the `ReduceOrder` type is `ORDER_ONLY_VALUE`: [1 2 ... 3 0 0 0 ...]
    > > - If the `ReduceOrder` type is `ORDER_ONLY_INDEX`: [3.09944e-06 0 5.96046e-06 0 ... 1.13249e-06 0]
    >
    > Here, the value of index is the representation of the binary of the int value in half. Taking the preceding result as an example:
    > - Among the first 128 numbers, the position of 1 in the corresponding repeat is 52, whose hexadecimal value is 0x3400, corresponding to the half value 3.09944e-06.
    > - Among the second 128 numbers, the position of 2 in the corresponding repeat is 100, whose hexadecimal value is 0x6400, corresponding to the half value 5.96046e-06.
    > - Among the last 128 numbers, the position of 3 in the corresponding repeat is 19, whose hexadecimal value is 0x1300, corresponding to the half value 1.13249e-06.

- `ReduceRepeat<ReduceType::SUM>` tensor high-dimensional slicing computation example - `mask` bit-by-bit mode:

    ```cpp
    // dstLocal and srcLocal are both of the half type. The computation data volume of srcLocal is 512, arranged contiguously, and the computation result also needs to be arranged contiguously. Use the tensor high-dimensional slicing computation API and set mask so that all 128 elements, the maximum number, participate in the computation.
    uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    
    // Based on the preceding information, infer that repeatTime is 4, dstRepStride is 1, srcBlkStride is 1, and srcRepStride is 8.
    AscendC::ReduceRepeat<AscendC::ReduceType::SUM, half>(dstLocal, srcLocal, mask, 4, 1, 1, 8);
    ```
