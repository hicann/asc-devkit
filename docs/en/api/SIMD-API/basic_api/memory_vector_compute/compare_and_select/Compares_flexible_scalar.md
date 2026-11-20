# Compares (Flexible Scalar Position)<a name="ZH-CN_TOPIC_0000002136591178"></a>

<!-- md-trans-meta sourceCommit=f266635ab179c912cc9641061c2d27246be9170c translatedAt=2026-11-02T07:11:05.694Z pushedAt=2026-11-18T01:25:07.052Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/Compares_flexible_scalar_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Provides an API with a flexible scalar position, supporting two scenarios: scalar first and scalar last. The scalar input supports configuring a LocalTensor single-point element. The calculation formulas are as follows, where idx indicates the position coefficient of the LocalTensor single-point element.

$dst_i = Compares(src_i, scalar)$

$dst_i = Compares(scalar, src_i)$

$dst_i = Compares(src_i, scalar_{idx})$

$dst_i = Compares(scalar_{idx}, src_i)$

Supports multiple comparison modes:

-   LT: Less than
-   GT: Greater than

-   GE: Greater than or equal to
-   EQ: Equal to
-   NE: Not equal to
-   LE: Less than or equal to

## Prototype<a name="section620mcpsimp"></a>

-   Tensor first n elements computation

    ```cpp
    template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
    __aicore__ inline void Compares(const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, uint32_t count)
    ```

-   Tensor high-dimensional slicing computation
    -   **mask** bit-by-bit mode

        ```cpp
        template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
        __aicore__ inline void Compares(const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    -   **mask** continuous mode

        ```cpp
        template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
        __aicore__ inline void Compares(const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T0 | For the fixed scalar position API, indicates the source operand data type.<br>In particular, for the flexible scalar position API, this is a reserved parameter that is not enabled yet and is kept for future function extension. When it needs to be specified, pass the default value BinaryDefaultType. |
| T1 | For the fixed scalar position API, indicates the destination operand data type.<br>In particular, for the flexible scalar position API, this is a reserved parameter that is not enabled yet and is kept for future function extension. When it needs to be specified, pass the default value BinaryDefaultType. |
| isSetMask | Whether to set the mask inside the API.<br>- true: The mask is set inside the API.<br>- false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameters is set to the placeholder MASK_PLACEHOLDER, which is used only as a placeholder and has no actual meaning. |
| config | Type: BinaryConfig. It takes effect when the scalar is a single-point element of LocalTensor, and is used to specify the position of the single-point element operand. The default value DEFAULT_BINARY_CONFIG indicates that the right operand is a scalar.<br>struct BinaryConfig {<br>&nbsp;&nbsp;&nbsp;&nbsp;int8_t scalarTensorIndex = 1; // Specifies the position of the scalar when the scalar is a single-point element of LocalTensor. 0 indicates the left operand, and 1 indicates the right operand.<br>};<br>constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1}; |
| T2 | LocalTensor type. The corresponding data type is automatically inferred based on the input parameter dst. You do not need to configure this parameter. You only need to ensure that dst satisfies the data type constraint. |
| T3 | LocalTensor type or scalar type. The corresponding data type is automatically inferred based on the input parameter src0. You do not need to configure this parameter. You only need to ensure that src0 satisfies the data type constraint. |
| T4 | LocalTensor type or scalar type. The corresponding data type is automatically inferred based on the input parameter src1. You do not need to configure this parameter. You only need to ensure that src1 satisfies the data type constraint. |

**Table 2** API parameter description

| Parameter | Input/Output | Meaning |
| :--- | :--- | :--- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.<br>dst stores the comparison result. The uint8_t data in dst is expanded bit by bit, and from left to right each bit represents the comparison result of src0 and src1 at the corresponding position. If the comparison result is true, the corresponding bit is 1; otherwise, it is 0. |
| src0/src1 | Input | Source operands of the flexible scalar position API.<br>Supported types: LocalTensor or scalar. The data type must be consistent with that of the destination operand.<br>When the type is LocalTensor, it can be used as a vector operand or a scalar single-point element. Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.|
| cmpMode | Input | CMPMODE type, indicating the comparison mode, including EQ, NE, GE, LE, GT, and LT.<br>- LT: src0 is less than src1<br>- GT: src0 is greater than src1<br>- GE: src0 is greater than or equal to src1<br>- EQ: src0 is equal to src1<br>- NE: src0 is not equal to src1<br>- LE: src0 is less than or equal to src1 |
| mask/mask[] | Input | mask controls the elements involved in the calculation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeat) are required to finish reading and computing all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing API](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operand. Type: [UnaryRepeatParams](../../aux_data_structures//UnaryRepeatParams.md), which contains the address stride between the same DataBlock in adjacent iterations and the address stride between different DataBlocks in the same iteration.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlock in the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |
| count | Input | Number of elements that participate in the computation. **When setting count, ensure that the space occupied by count elements is 256-byte aligned.** |

## Data Types

<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, the destination operand dst supports the following data type: uint8_t. The source operand src supports the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, float, int32_t, uint32_t, int64_t, uint64_t, and double. Among them, int8\_t, uint8\_t, uint64\_t, int64\_t, and double support only the tensor first-n data computation API, and double supports only CMPMODE::EQ.
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/Compares_flexible_scalar_res.md#id2 -->

## Return Value<a name="section128671456102513"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   In the scenario where the flexible scalar position API is called and the source operand is a LocalTensor single-point element, address overlap between the source operand and the destination operand is not supported.

-   dst is sorted into a binary result in little-endian order, corresponding to the comparison result of the data at the corresponding position in src.
-   For APIs that use the first n data of a tensor for computation, when setting count, ensure that the space occupied by count elements is 256-byte aligned.
-   At least one of the left operand and the right operand must be a vector. Both operands being scalars at the same time is not supported.
-   When this API passes a single-point LocalTensor data element as a scalar, the idx parameter must be a compile-time constant. If a variable is passed, it must be declared as constexpr.
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/Compares_flexible_scalar_res.md#id3 -->

## Example<a name="section642mcpsimp"></a>

For the flexible scalar position API, you can pass an immediate value or a single-point LocalTensor directly as the scalar, and both call forms with the scalar at the front and at the end are supported.

For a complete call example, see scenario 4 of [Compare class examples](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/compare).

-   example of the API for computing the first n data of a tensor

    ```cpp
    // Scalar at the end, with src1Local[0] as the scalar.
    AscendC::Compares(dstLocal, src0Local, src1Local[0], AscendC::CMPMODE::LT, srcDataSize);
    
    // Scalar at the beginning, with src0Local[0] as the scalar.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Compares<BinaryDefaultType, BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, AscendC::CMPMODE::LT, srcDataSize);
    ```

-   Tensor high-dimensional splitting computation - mask continuous mode

    ```cpp
    uint64_t mask = 256 / sizeof(float); // 256 is the number of bytes processed per iteration.
    int repeat = 4;
    AscendC::UnaryRepeatParams repeatParams = { 1, 1, 8, 8 };
    // repeat = 4, 64 elements one repeat, 256 elements total
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    // Scalar at the end, with src1Local[0] as the scalar.
    AscendC::Compares(dstLocal, src0Local, src1Local[0], AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    
    // Scalar at the beginning, with src0Local[0] as the scalar.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Compares<BinaryDefaultType, BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    ```

-   Tensor high-dimensional splitting computation - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, 0};
    int repeat = 4;
    AscendC::UnaryRepeatParams repeatParams = { 1, 1, 8, 8 };
    // repeat = 4, 64 elements one repeat, 256 elements total
    // srcBlkStride, = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    // Scalar at the end, with src1Local[0] as the scalar.
    AscendC::Compares(dstLocal, src0Local, src1Local[0], AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    
    // Scalar at the beginning, with src0Local[0] as the scalar.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Compares<BinaryDefaultType, BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    ```
