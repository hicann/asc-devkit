# Compare<a name="ZH-CN_TOPIC_0000001504145204"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:51:25.313Z pushedAt=2026-10-28T03:14:39.437Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/Compare_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_cmpsel_intf.h"`.

Compares two tensors element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0.

Supports multiple comparison modes:

- LT: less than

- GT: greater than

- GE: greater than or equal to

- EQ: equal to

- NE: not equal to

- LE: less than or equal to

## Prototype<a name="section620mcpsimp"></a>

- Entire Tensor Computation

    ```cpp
    dst = src0 < src1;
    dst = src0 > src1;
    dst = src0 <= src1;
    dst = src0 >= src1;
    dst = src0 == src1;
    dst = src0 != src1;
    ```

    <!-- npu="310b" id10 -->
    Atlas 200I/500 A2 inference products do not currently support operator overloading for entire tensor computation.
    <!-- end id10 -->

- Tensor First n Elements Computation

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void Compare(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, uint32_t count)
    ```

- Tensor High-Dimensional Split Computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true>
        __aicore__ inline void Compare(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true>
        __aicore__ inline void Compare(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

### Template Parameters and API Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand. |
| U | Data type of the destination operand. |
| isSetMask | Reserved parameter. Keep the default value. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.<br>dst stores the comparison result. The uint8_t data in dst is expanded bit by bit, and from left to right each bit represents the comparison result of src0 and src1 at the corresponding position. If the comparison result is true, the corresponding bit is 1; otherwise, it is 0. |
| src0, src1 | Input | Source operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| cmpMode | Input | CMPMODE type, indicating the comparison mode, including EQ, NE, GE, LE, GT, and LT.<br>&bull; LT: src0 is less than src1 <br>&bull; GT: src0 is greater than src1 <br>&bull; GE: src0 is greater than or equal to src1 <br>&bull; EQ: src0 is equal to src1 <br>&bull; NE: src0 is not equal to src1 <br>&bull; LE: src0 is less than or equal to src1<br> |
| mask/mask[] | Input | mask controls the elements that participate in calculation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector calculation unit reads 256 bytes of consecutive data for calculation each time. To complete processing of the input data, multiple iterations (repeat) are required to read and calculate all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Splitting](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. Type: [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), including the address stride of the same DataBlock between adjacent iterations of the operands, the address stride of different DataBlocks within the same iteration, and other parameters.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |
| count | Input | Number of elements that participate in calculation. For details about this parameter, see [Continuous Calculation](../SIMD_compute/continuous_compute.md). |

### mask/mask[] Parameter Description

<!-- npu="950" id11 -->
- For Ascend 950PR&950DT products, this parameter takes effect.
<!-- end id11 -->

<!-- npu="A3" id12 -->
- For Atlas A3 products, this parameter is reserved and the setting is invalid.
<!-- end id12 -->

<!-- npu="910b" id13 -->
- For Atlas A2 products, this parameter is reserved and the setting is invalid.
<!-- end id13 -->

<!-- npu="310b" id14 -->
- For Atlas 200I/500 A2 inference products, this parameter takes effect.
<!-- end id14 -->

<!-- npu="310p" id15 -->
- For Atlas inference products AI Core, this parameter is reserved and the setting is invalid.
<!-- end id15 -->

<!-- npu="910" id16 -->
- For Atlas training products, this parameter is reserved and the setting is invalid.
<!-- end id16 -->

## Data Type

<!-- npu="950" id19 -->
- For Ascend 950PR&950DT products
    - Supported data types for T: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double. Among them, int8\_t/uint8\_t/int64\_t/uint64\_t/double data types support only the API for computing the first n data of a tensor and the operator overloading where the entire tensor participates in calculation.
    - Supported data types for U: int8_t, uint8_t.
<!-- end id19 -->

<!-- npu="A3" id20 -->
- For Atlas A3 products
    - Supported data types for T: half (supported by all CMPMODE values), float (supported by all CMPMODE values), int32_t (supported only by CMPMODE::EQ).
    - Supported data types for U: int8_t, uint8_t.
<!-- end id20 -->

<!-- npu="910b" id21 -->
- For Atlas A2 products
    - Supported data types for T: half (supported by all CMPMODE values), float (supported by all CMPMODE values), int32_t (supported only by CMPMODE::EQ).
    - Supported data types for U: int8_t, uint8_t.
<!-- end id21 -->

<!-- npu="310b" id22 -->
- For Atlas 200I/500 A2 inference products
    - Supported data types for T: half, float.
    - Supported data types for U: int8_t, uint8_t.
<!-- end id22 -->

<!-- npu="310p" id23 -->
- For Atlas inference products AI Core
    - Supported data types for T: half, float.
    - Supported data types for U: int8_t, uint8_t.
<!-- end id23 -->

<!-- npu="910" id24 -->
- For Atlas training products
    - Supported data types for T: half, float.
    - Supported data types for U: int8_t, uint8_t.
<!-- end id24 -->

## Return Value<a name="section128671456102513"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).

- dst is sorted into a binary result in little-endian order, corresponding to the comparison result of the data at the corresponding position in src.

<!-- npu="A3,910b,950" id27 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id28 -->
  - For the following models, this API does not perform the computation operation, does not write to the destination operand, and is treated as a NOP (no operation).
    <!-- npu="A3" id29 -->
    - Atlas A3 products
    <!-- end id29 -->
    <!-- npu="910b" id30 -->
    - Atlas A2 products
    <!-- end id30 -->
  <!-- end id28 -->
  <!-- npu="950" id31 -->
  - For Ascend 950PR&950DT products, this API implements compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, it is not guaranteed that this API is treated as a NOP (no operation).
  <!-- end id31 -->
<!-- end id27 -->

- For the operator overloading function that uses the entire tensor for computation, src0 and src1 must meet 256-byte alignment.
<!-- npu="A3,910b" id32 -->
- For the following models, when using the API that uses the first n data of the tensor for computation and setting count, ensure that the space occupied by count elements meets 256-byte alignment. The unaligned elements do not participate in the computation, and only the complete aligned blocks are valid.
  <!-- npu="A3" id33 -->
  - Atlas A3 products
  <!-- end id33 -->
  <!-- npu="910b" id34 -->
  - Atlas A2 products
  <!-- end id34 -->
<!-- end id32 -->

## Example<a name="section642mcpsimp"></a>

In this example, the source operands src0 and src1 each store 256 float data. The function implemented in this example performs element-wise comparison of the data in src0 and src1. If an element in src0 is less than the corresponding element in src1, the corresponding bit in the dst result is set to 1; otherwise, it is set to 0. The dst result is stored using uint8\_t type data.

For the complete calling example, see [Compare class example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/compare) scenario 1.

- The entire tensor participates in the computation

    ```cpp
    dstLocal = src0Local < src1Local;  // Less than (LT).
    dstLocal = src0Local > src1Local;  // Greater than (GT).
    dstLocal = src0Local <= src1Local; // Less than or equal to (LE).
    dstLocal = src0Local >= src1Local; // Greater than or equal to (GE).
    dstLocal = src0Local == src1Local; // Equal to (EQ).
    dstLocal = src0Local != src1Local; // Not equal to (NE).
    ```

- Tensor first n elements computation

    ```cpp
    // srcDataSize: number of elements participating in the computation.
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LT, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GT, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::LE, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::GE, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::EQ, srcDataSize);
    AscendC::Compare(dstLocal, src0Local, src1Local, AscendC::CMPMODE::NE, srcDataSize);
    ```

    The result is as follows:

    ```plain
    LT: Less than.
    Input data src0Local: [ 2  2  2  2  2  2  2  2  3  3  3  3  3  3  3  3  5  5  5  5  5  5  5  5  7  7  7  7  7  7  7  7
                        11 11 11 11 11 11 11 11 13 13 13 13 13 13 13 13 17 17 17 17 17 17 17 17 19 19 19 19 19 19 19 19 ]
    Input data src1Local: [ 2  2  2  2  2  2  2  2  4  4  4  4  4  4  4  4  6  6  6  6  6  6  6  6  8  8  8  8  8  8  8  8
                        10 10 10 10 10 10 10 10 12 12 12 12 12 12 12 12 14 14 14 14 14 14 14 14 16 16 16 16 16 16 16 16 ]
    Output data dstLocal: [ 0(0b00000000) 255(0b11111111) 255(0b11111111) 255(0b11111111) 0(0b00000000) 0(0b00000000) 0(0b00000000) 0(0b00000000) ]

    GT: greater than
    Input data src0Local: [ 2 3 5 7 11 13 17 19 ... ]
    Input data src1Local: [ 2 4 6 8 10 12 14 16 ... ]
    Element-wise comparison result: [ 0 0 0 0  1  1  1  1 ... ]
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

- Tensor high-dimensional splitting computation, mask bit-by-bit mode

    ```cpp
    // The masks array controls the elements that participate in the computation in each iteration. The two uint64_t values total 128 bits, and each bit controls one element. A value of 1 means the element participates in the computation, and 0 means it does not.
    // masks[0] controls the first 64 elements, with lower bits controlling elements with smaller indices. Similarly, masks[1] controls the last 64 elements.
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

    ```plain
    LE: less than or equal to
    Input data src0Local: [ 2  2  2  2  2  2  2  2  3  3  3  3  3  3  3  3  5  5  5  5  5  5  5  5  7  7  7  7  7  7  7  7
                        11 11 11 11 11 11 11 11 13 13 13 13 13 13 13 13 17 17 17 17 17 17 17 17 19 19 19 19 19 19 19 19 ]
    Input data src1Local: [ 2  2  2  2  2  2  2  2  4  4  4  4  4  4  4  4  6  6  6  6  6  6  6  6  8  8  8  8  8  8  8  8
                        10 10 10 10 10 10 10 10 12 12 12 12 12 12 12 12 14 14 14 14 14 14 14 14 16 16 16 16 16 16 16 16 ]
    Input data masks: { 858993459, 0 }
    Output data dstLocal: [ 51(0b00110011) 51(0b00110011) 51(0b00110011) 51(0b00110011) 0(0b00000000) 0(0b00000000) 0(0b00000000) 0(0b00000000) ]
    ```

- Tensor high-dimensional splitting computation, mask continuous mode

    ```cpp
    // The mask controls the number of consecutive elements that participate in the computation in each iteration.
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

    ```plain
    LE: less than or equal to
    Input data src0Local: [ 2  2  2  2  2  2  2  2  3  3  3  3  3  3  3  3  5  5  5  5  5  5  5  5  7  7  7  7  7  7  7  7
                        11 11 11 11 11 11 11 11 13 13 13 13 13 13 13 13 17 17 17 17 17 17 17 17 19 19 19 19 19 19 19 19 ]
    Input data src1Local: [ 2  2  2  2  2  2  2  2  4  4  4  4  4  4  4  4  6  6  6  6  6  6  6  6  8  8  8  8  8  8  8  8
                        10 10 10 10 10 10 10 10 12 12 12 12 12 12 12 12 14 14 14 14 14 14 14 14 16 16 16 16 16 16 16 16 ]
    Input data mask: 64
    Output data dstLocal: [ 255(0b11111111) 255(0b11111111) 255(0b11111111) 255(0b11111111) 0(0b00000000) 0(0b00000000) 0(0b00000000) 0(0b00000000) ]
    ```
