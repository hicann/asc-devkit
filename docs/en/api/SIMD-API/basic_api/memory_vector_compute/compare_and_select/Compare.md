# Compare<a name="ZH-CN_TOPIC_0000001504145204"></a>

<!-- md-trans-meta sourceCommit=27a8d829e1498b83498ed8bfc14b3c9e69ae662d translatedAt=2026-09-09T07:49:03.326Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id15 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id15 -->
<!-- npu="A3" id16 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id16 -->
<!-- npu="910b" id17 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id17 -->
<!-- npu="310b" id18 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id18 -->
<!-- npu="310p" id19 -->
- Atlas inference products AI Core: Supported
<!-- end id19 -->
<!-- npu="310p" id20 -->
- Atlas inference products Vector Core: Not supported
<!-- end id20 -->
<!-- npu="910" id21 -->
- Atlas training products: Supported
<!-- end id21 -->


## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_cmpsel_intf.h"`

Compares two tensors element-wise. If the comparison result is true, the corresponding bit of the output result is set to 1; otherwise, it is set to 0.

The API supports multiple comparison modes:

- LT: less than

- GT: greater than

- GE: greater than or equal to

- EQ: equal to

- NE: not equal to

- LE: less than or equal to

## Prototype<a name="section620mcpsimp"></a>

- Computation on the entire tensor

    ```cpp
    dst = src0 < src1;
    dst = src0 > src1;
    dst = src0 <= src1;
    dst = src0 >= src1;
    dst = src0 == src1;
    dst = src0 != src1;
    ```

    <!-- npu="310b" id1 -->
    Atlas 200I/500 A2 inference products do not support operator overloading for the entire tensor participating in calculation.
    <!-- end id1 -->

- Computation of the first n data elements of the tensor

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void Compare(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, uint32_t count)
    ```

- High-dimensional splitting computation of the tensor
    - Bitwise mask mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true>
        __aicore__ inline void Compare(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

    - Continuous mask mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true>
        __aicore__ inline void Compare(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

### Template Parameters and API Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand. |
| U | Data type of the destination operand. |
| isSetMask | Reserved parameter. Keep the default value. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.<br>dst stores the comparison result. The uint8_t data in dst is expanded bit by bit, and each bit from left to right represents the comparison result of src0 and src1 at the corresponding position. If the comparison result is true, the corresponding bit is 1; otherwise, it is 0. |
| src0, src1 | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned. |
| cmpMode | Input | CMPMODE type, indicating the comparison mode, including EQ, NE, GE, LE, GT, and LT.<br>&bull; LT: src0 is less than src1 <br>&bull; GT: src0 is greater than src1 <br>&bull; GE: src0 is greater than or equal to src1 <br>&bull; EQ: src0 is equal to src1 <br>&bull; NE: src0 is not equal to src1 <br>&bull; LE: src0 is less than or equal to src1<br> |
| mask/mask[] | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector calculation unit reads 256 bytes of consecutive data for calculation each time. To complete the processing of the input data, multiple iterations (repeat) are required to read and calculate all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |
 | count | Input | Number of elements that participate in the calculation. For details about this parameter, see [Continuous Calculation](../SIMD_compute/continuous_computation.md). |

### mask/mask[] Parameters

<!-- npu="950" id2 -->
- For Ascend 950PR/Ascend 950DT, the setting takes effect.
<!-- end id2 -->

<!-- npu="A3" id3 -->
- For Atlas A3 training/Atlas A3 inference products, this is a reserved parameter and the setting is invalid.
<!-- end id3 -->

<!-- npu="910b" id4 -->
- For Atlas A2 training/Atlas A2 inference products, this is a reserved parameter and the setting is invalid.
<!-- end id4 -->

<!-- npu="310b" id5 -->
- For Atlas 200I/500 A2 inference products, the setting takes effect.
<!-- end id5 -->

<!-- npu="310p" id6 -->
- For Atlas inference products AI Core, this is a reserved parameter and the setting is invalid.
<!-- end id6 -->

<!-- npu="910" id7 -->
- For Atlas training products, this is a reserved parameter and the setting is invalid.
<!-- end id7 -->

## Data Type

<!-- npu="950" id8 -->
- For Ascend 950PR/Ascend 950DT
    - The data types supported by T are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double. Among them, the int8\_t/uint8\_t/int64\_t/uint64\_t/double data types are supported only by the API for calculating the first n data of a tensor and the operator overloading in which the entire tensor participates in the calculation.
    - The data types supported by U are: int8_t and uint8_t.
<!-- end id8 -->

<!-- npu="A3" id9 -->
- For Atlas A3 training products/Atlas A3 inference products
    - The data types supported by T are: half (supported by all CMPMODE), float (supported by all CMPMODE), and int32_t (only CMPMODE::EQ is supported).
    - The data types supported by U are: int8_t and uint8_t.
<!-- end id9 -->

<!-- npu="910b" id10 -->
- For Atlas A2 training products/Atlas A2 inference products
    - The data types supported by T are: half (supported by all CMPMODE), float (supported by all CMPMODE), and int32_t (only CMPMODE::EQ is supported).
    - The data types supported by U are: int8_t and uint8_t.
<!-- end id10 -->

<!-- npu="310b" id11 -->
- For Atlas 200I/500 A2 inference products
    - The data types supported by T are: half and float.
    - The data types supported by U are: int8_t and uint8_t.
<!-- end id11 -->

<!-- npu="310p" id12 -->
- For Atlas inference products AI Core
    - The supported data types of T are half and float.
    - The supported data types of U are int8_t and uint8_t.
<!-- end id12 -->

<!-- npu="910" id13 -->
- For Atlas training products
    - The supported data types of T are half and float.
    - The supported data types of U are int8_t and uint8_t.
<!-- end id13 -->

## Return Value<a name="section128671456102513"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).

- **dst** is sorted into a binary result in little-endian order, corresponding to the comparison result of the data at the corresponding position in **src**.

- For the operator overloading feature that uses the entire tensor for computation, src0 and src1 must be 256-byte aligned.

<!-- npu="A3,910b" id32 -->
- For the following models, when using the API that uses the first n elements of the tensor for computation, ensure that the space occupied by count elements is 256-byte aligned when setting count. The unaligned elements do not participate in the computation, and only the complete aligned blocks are valid.
  <!-- npu="A3" id33 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id33 -->
  <!-- npu="910b" id34 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id34 -->
<!-- end id32 -->

## Example<a name="section642mcpsimp"></a>

In this example, the source operands src0 and src1 each store 256 float data elements. The example compares the data in src0 and src1 element-wise. If an element in src0 is less than the corresponding element in src1, the corresponding bit in the dst result is set to 1; otherwise, it is set to 0. The dst result is stored using the uint8\_t data type.

For the complete example, see scenario 1 of the [Compare class example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/compare).

- Computation on the entire tensor


    ```cpp
    dstLocal = src0Local < src1Local;  // less than (LT)
    dstLocal = src0Local > src1Local;  // greater than (GT)
    dstLocal = src0Local <= src1Local; // less than or equal to (LE)
    dstLocal = src0Local >= src1Local; // greater than or equal to (GE)
    dstLocal = src0Local == src1Local; // equal to (EQ)
    dstLocal = src0Local != src1Local; // not equal to (NE)
    ```

- Calculation on the first n data of the tensor

    ```cpp
    // srcDataSize: number of elements participating in the calculation
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LT, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GT, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LE, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GE, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::EQ, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::NE, srcDataSize);
    ```

    The result is as follows:

    ```
    LT: less than
    Input data src0Local: [ 2  2  2  2  2  2  2  2  3  3  3  3  3  3  3  3  5  5  5  5  5  5  5  5  7  7  7  7  7  7  7  7
                        11 11 11 11 11 11 11 11 13 13 13 13 13 13 13 13 17 17 17 17 17 17 17 17 19 19 19 19 19 19 19 19 ]
    Input data src1Local: [ 2  2  2  2  2  2  2  2  4  4  4  4  4  4  4  4  6  6  6  6  6  6  6  6  8  8  8  8  8  8  8  8
                        10 10 10 10 10 10 10 10 12 12 12 12 12 12 12 12 14 14 14 14 14 14 14 14 16 16 16 16 16 16 16 16 ]
    Output data dstLocal: [ 0(0b00000000) 255(0b11111111) 255(0b11111111) 255(0b11111111) 0(0b00000000) 0(0b00000000) 0(0b00000000) 0(0b00000000) ]

    GT: greater than
    Input data src0Local: [ 2 3 5 7 11 13 17 19 ... ]
    Input data src1Local: [ 2 4 6 8 10 12 14 16 ... ]
    Element-wise comparison result:   [ 0 0 0 0  1  1  1  1 ... ]
    Output data dstLocal: [ 240(0b11110000) ... ]

    GE: greater than or equal to
    Input data src0Local: [ 2 3 5 7 11 13 17 19 ... ]
    Input data src1Local: [ 2 4 6 8 10 12 14 16 ... ]
    Output data dstLocal: [ 241(0b11110001) ... ]

    LE: less than or equal to
    Input data src0Local: [ 2 3 5 7 11 13 17 19 ... ]
    Input data src1Local: [ 2 4 6 8 10 12 14 16 ... ]
    Output data dstLocal: [ 15(0b00001111) ... ]

    EQ: equal to
    Input data src0Local: [ 2 3 5 7 11 13 17 19 ... ]
    Input data src1Local: [ 2 4 6 8 10 12 14 16 ... ]
    Output data dstLocal: [ 1(0b00000001) ... ]

    NE: not equal to
    Input data src0Local: [ 2 3 5 7 11 13 17 19 ... ]
    Input data src1Local: [ 2 4 6 8 10 12 14 16 ... ]
    Output data dstLocal: [ 254(0b11111110) ... ]
    ```

- Computation on high-dimensional tensor slices in bitwise mask mode

    ```cpp
    // The masks array controls which elements participate in each iteration. The two uint64_t values contain 128 bits in total, with each bit controlling one element: 1 for participation in the computation and 0 otherwise.
    // masks[0] controls the first 64 elements, with lower bits controlling elements with smaller indices; masks[1] similarly controls the last 64 elements.
    // For example, for float data, each iteration processes 256B / sizeof(float) = 64 elements, so only masks[0] is needed for control.
    uint64_t masks[2] = {858993459, 0}; // 858993459(0x33333333)
    // repeat: 1, dstBlkStride: 1, src0BlkStride: 1, src1BlkStride: 1, dstRepStride: 1, src0RepStride: 8, src1RepStride: 8
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LT, masks, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GT, masks, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LE, masks, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GE, masks, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::EQ, masks, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::NE, masks, 1, { 1, 1, 1, 1, 8, 8 });
    ```

    The result is as follows:

    ```
    LE: less than or equal to
    Input data src0Local: [ 2  2  2  2  2  2  2  2  3  3  3  3  3  3  3  3  5  5  5  5  5  5  5  5  7  7  7  7  7  7  7  7
                        11 11 11 11 11 11 11 11 13 13 13 13 13 13 13 13 17 17 17 17 17 17 17 17 19 19 19 19 19 19 19 19 ]
    Input data src1Local: [ 2  2  2  2  2  2  2  2  4  4  4  4  4  4  4  4  6  6  6  6  6  6  6  6  8  8  8  8  8  8  8  8
                        10 10 10 10 10 10 10 10 12 12 12 12 12 12 12 12 14 14 14 14 14 14 14 14 16 16 16 16 16 16 16 16 ]
    Input data masks: { 858993459, 0 }
    Output data dstLocal: [ 51(0b00110011) 51(0b00110011) 51(0b00110011) 51(0b00110011) 0(0b00000000) 0(0b00000000) 0(0b00000000) 0(0b00000000) ]
    ```

- Computation on high-dimensional tensor slices in continuous mask mode

    ```cpp
    // mask controls the number of consecutive elements participating in the calculation in each iteration.
    // For example, for float data, each iteration processes 256B / sizeof(float) = 64 elements.
    uint64_t mask = 64;
    // repeat: 1, dstBlkStride: 1, src0BlkStride: 1, src1BlkStride: 1, dstRepStride: 1, src0RepStride: 8, src1RepStride: 8
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LT, mask, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GT, mask, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LE, mask, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GE, mask, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::EQ, mask, 1, { 1, 1, 1, 1, 8, 8 });
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::NE, mask, 1, { 1, 1, 1, 1, 8, 8 });
    ```

    The result is as follows:

    ```
    LE: less than or equal to
    Input data src0Local: [ 2  2  2  2  2  2  2  2  3  3  3  3  3  3  3  3  5  5  5  5  5  5  5  5  7  7  7  7  7  7  7  7
                        11 11 11 11 11 11 11 11 13 13 13 13 13 13 13 13 17 17 17 17 17 17 17 17 19 19 19 19 19 19 19 19 ]
    Input data src1Local: [ 2  2  2  2  2  2  2  2  4  4  4  4  4  4  4  4  6  6  6  6  6  6  6  6  8  8  8  8  8  8  8  8
                        10 10 10 10 10 10 10 10 12 12 12 12 12 12 12 12 14 14 14 14 14 14 14 14 16 16 16 16 16 16 16 16 ]
    Input data mask: 64
    Output data dstLocal: [ 255(0b11111111) 255(0b11111111) 255(0b11111111) 255(0b11111111) 0(0b00000000) 0(0b00000000) 0(0b00000000) 0(0b00000000) ]
    ```
