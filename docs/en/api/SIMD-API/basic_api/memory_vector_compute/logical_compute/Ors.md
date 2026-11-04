# Ors<a name="ZH-CN_TOPIC_0000002132550008"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:10:54.869Z pushedAt=2026-10-28T03:14:39.534Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/logical_compute/Ors_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Performs a bitwise OR operation between each element in a vector and a scalar, supporting both scalar-first and scalar-last scenarios. The scalar input supports configuring a single-point element of LocalTensor. The calculation formulas are as follows, where idx indicates the position coefficient of the single-point element of LocalTensor.

$dst_i = src_i | scalar$

$dst_i = scalar | src_i$

$dst_i = src_i | scalar_{idx}$

$dst_i = scalar_{idx} | src_i$

## Prototype<a name="section620mcpsimp"></a>

-   Tensor First n Elements Computation

    ```cpp
    template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
    __aicore__ inline void Ors(const U& dst, const S& src0, const V& src1, const int32_t& count)
    ```

-   Tensor High-Dimensional Split Computation
    -   mask bit-by-bit mode

        ```cpp
        template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
        __aicore__ inline void Ors(const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    -   **mask** continuous mode

        ```cpp
        template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
        __aicore__ inline void Ors(const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T | Reserved parameter, not enabled yet. It is reserved for future function extension. When it needs to be specified, pass the default value BinaryDefaultType. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use [SetVectorMask](../mask_operations/SetVectorMask.md) to set the mask value. In this mode:<br>&nbsp;&nbsp;&bull; For the Tensor first n elements computation API, the count input parameter does not take effect. It is recommended to set it to 1.<br>&nbsp;&nbsp;&bull; For the Tensor high-dimensional slicing computation API, the mask input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |
| config | Type: BinaryConfig. It takes effect when the scalar is a single-point element of LocalTensor, and is used to specify the position of the single-point element operand. The default value DEFAULT_BINARY_CONFIG indicates that the right operand is a scalar.<br>struct BinaryConfig {<br>&nbsp;&nbsp;&nbsp;&nbsp;int8_t scalarTensorIndex = 1; // Specifies the position of the scalar when the scalar is a single-point element of LocalTensor. 0 indicates the left operand, and 1 indicates the right operand.<br>};<br>constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1}; |
| U | LocalTensor type. The corresponding data type is automatically inferred based on the input parameter dst. You do not need to configure this parameter. You only need to ensure that dst satisfies the data type constraint. |
| S | LocalTensor type or scalar type. The corresponding data type is automatically inferred based on the input parameter src0. You do not need to configure this parameter. You only need to ensure that src0 satisfies the data type constraint. |
| V | LocalTensor type or scalar type. The corresponding data type is automatically inferred based on the input parameter src1. You do not need to configure this parameter. You only need to ensure that src1 satisfies the data type constraint. |

**Table 2**  Parameters

| **Parameter Name** | **Type** | **Description** |
| :--- | :--- | :--- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.|
| src0/src1 | Input | Source operand.<br>Supported type: LocalTensor or scalar. The data type must be consistent with that of the destination operand.<br> When the type is LocalTensor, it can be used as a vector operand or a scalar single-point element. Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| count | Input | Number of elements involved in the computation. |
| mask/mask[] | Input | mask controls the elements involved in the computation within each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeat) are required to finish reading and computing all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing API](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Element operation control structure information. For details, see [UnaryRepeatParams](../../aux_data_structures//UnaryRepeatParams.md). |

## Data Types

<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, the data types supported by the destination operand dst and the source operand src are: int16_t, uint16_t, int64_t, and uint64_t. The int64_t and uint64_t data types are supported only by the API for computing the first n data elements of a tensor.
<!-- end id8 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   When using the Tensor high-dimensional slicing computation API, to save address space, you can define a single Tensor that is used by both the source operand and the destination operand (that is, with overlapping addresses). The related constraints are as follows:
    -   For a single repeat (repeatTime=1), the source operand and the destination operand must overlap 100% completely. Partial overlap is not supported.
    -   For multiple repeats (repeatTime\>1), when a dependency exists between the operand and the destination operand, that is, when the destination operand of the Nth iteration is the source operand of the (N+1)th iteration, address overlap is not supported.
    -   When the source operand is a single-point element of a LocalTensor, address overlap between the source operand and the destination operand is not supported.

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id9 -->
-   For Ascend 950PR&950DT products, the isSetMask parameter in the API for computing the first n data elements of a tensor does not take effect. Keep its default value.
<!-- end id9 -->
-   At least one of the left operand and the right operand must be a vector. Both operands being scalars at the same time is not supported.
-   When this API passes a single-point LocalTensor data element as a scalar, the idx parameter must be a compile-time constant. If a variable is passed, it must be declared as constexpr.

## Example<a name="section642mcpsimp"></a>

-   Tensor high-dimensional slicing computation example - mask continuous mode

    ```cpp
    // dstLocal: Output tensor.
    // src0Local: Input tensor.
    // src1Local: Input tensor.

    uint64_t mask = 128;
    // repeatTime = 4. Each iteration processes 128 numbers, so 4 iterations are required to process 512 numbers.
    // dstBlkStride, srcBlkStride = 1. Within each iteration, the address interval of the data involved in the calculation for src0 is 1 datablock, indicating that data is read and written continuously within a single iteration.
    // dstRepStride, srcRepStride = 8. The address interval between adjacent iterations is 8 datablocks, indicating that data is read and written continuously between adjacent iterations.
    // Example with the scalar at the end.
    AscendC::Ors(dstLocal, src0Local, src1Local[0], mask, 4, { 1, 1, 8, 8 });

    // Example with the scalar at the beginning.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Ors<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, mask, 4, {1, 1, 8, 8});
    ```

-   Tensor high-dimensional slicing calculation example - mask bit-by-bit mode

    ```cpp
    // dstLocal: Output tensor.
    // src0Local: Input tensor.
    // src1Local: Input tensor.

    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4. Each iteration processes 128 elements, so computing 512 elements requires 4 iterations.
    // dstBlkStride, srcBlkStride = 1. The address interval of the data in src0 involved in the computation within each iteration is 1 datablock, indicating that data is read and written continuously within a single iteration.
    // dstRepStride, srcRepStride = 8. The address interval between adjacent iterations is 8 datablocks, indicating that data is read and written continuously between adjacent iterations.
    // Example with the scalar as the second operand.
    AscendC::Ors(dstLocal, src0Local, src1Local[0], mask, 4, {1, 1, 8, 8});

    // Example with the scalar as the first operand.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Ors<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, mask, 4, {1, 1, 8, 8});
    ```

-   Tensor First n Elements Computation Example

    ```cpp
    // Example with the scalar as the second operand.
    AscendC::Ors(dstLocal, src0Local, src1Local[0], 512);

    // Example with the scalar as the first operand.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Ors<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, 512);
    ```

The result is as follows:

```plain
// Scalar at the end, src1Local[0] as the scalar
Input data src0Local: [1 2 3 ... 512]
Input data src1Local: [1]
Output data dstLocal: [1 2 3 ... 512]

// Scalar at the beginning, src0Local[0] as the scalar
Input data src0Local: [1]
Input data src1Local: [1 2 3 ... 512]
Output data dstLocal: [1 2 3 ... 512]
```
