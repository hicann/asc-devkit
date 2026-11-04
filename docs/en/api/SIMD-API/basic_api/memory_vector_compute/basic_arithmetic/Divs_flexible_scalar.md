# Divs<a name="ZH-CN_TOPIC_0000002132391848"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:12:13.797Z pushedAt=2026-10-28T03:14:39.378Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Divs_flexible_scalar_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`.

Divs is a binary scalar computation API that divides each element in a vector by a scalar, supporting two scenarios where the scalar serves as the dividend or the divisor. The scalar input supports configuring a LocalTensor single-point element. The calculation formulas are as follows:

$dst_i = src_i \div scalar$

$dst_i = scalar \div src_i$

## Prototype<a name="section620mcpsimp"></a>

- Tensor First n Elements Computation

    ```cpp
    template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
    __aicore__ inline void Divs(const U& dst, const S& src0, const V& src1, const int32_t& count)
    ```

- Tensor High-Dimensional Split Computation
    - mask bitwise mode

        ```cpp
        template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
        __aicore__ inline void Divs(const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
        __aicore__ inline void Divs(const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| ---- | ---- |
| T | Data type of the operands.<br>For the flexible scalar position API, this is a reserved parameter that is not yet enabled and is retained for future function extension. When it needs to be specified, pass the default value BinaryDefaultType. |
| isSetMask | Whether to set the mask mode and mask value inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameters must be set to the placeholder MASK_PLACEHOLDER.<br>For details, see [Mask](../SIMD_compute/mask.md). |
| config | Type: BinaryConfig. It takes effect when the scalar is of the LocalTensor single-point element type and specifies the position of the single-point element operand. The default value DEFAULT_BINARY_CONFIG indicates that the right operand is a scalar.<br>struct BinaryConfig {<br>    int8_t scalarTensorIndex = 1; // Specifies the position of the scalar when the scalar is a LocalTensor single-point element. 0 indicates the left operand, and 1 indicates the right operand.<br>};<br>constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1}; |
| U | LocalTensor type. The data type is automatically inferred from the input parameter dst. You do not need to configure this parameter, but ensure that dst meets the data type constraints. |
| S | LocalTensor type or scalar type. The data type is automatically inferred from the input parameter src0. You do not need to configure this parameter, but ensure that src0 meets the data type constraints. |
| V | LocalTensor type or scalar type. The data type is automatically inferred from the input parameter src1. You do not need to configure this parameter, but ensure that src1 meets the data type constraints. |

**Table 2** Parameters

| Parameter | Type | Description |
| ---- | ---- | ---- |
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src0/src1 | Input | Source operands in the flexible scalar position API.<br>&bull; Type: LocalTensor: Can serve as a vector operand or a scalar single-point element. Supported TPosition: VECIN/VECCALC/VECOUT. For address alignment constraints, see [Address Alignment Constraints](../../../general_description_and_constraints.md). <br>&bull; Type: Scalar. |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_compute.md). |
| mask/mask[] | Input | mask controls the elements involved in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 consecutive bytes of data for computation each time. To complete processing of the input data, multiple iterations (repeat) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameter that controls the address stride of the operands. Type: [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md). It includes parameters such as the address stride of the same DataBlock between adjacent iterations of an operand and the address stride of different DataBlocks within the same iteration of an operand.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Type

<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, the supported data types are half, float, complex32, int64_t, uint64_t, and complex64. Among them, complex32, int64_t, uint64_t, and complex64 are supported only by the API that computes the first n data elements of a tensor.
<!-- end id8 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
- When using the Tensor high-dimensional slicing computation API, to save address space, you can define a single Tensor that is used by both the source operand and the destination operand (that is, address overlap). The related constraints are as follows:
    - For a single repeat (repeatTime=1), the source operand and the destination operand must overlap 100% completely. Partial overlap is not supported.
    - For multiple repeats (repeatTime\>1), when a dependency exists between the operand and the destination operand, that is, the destination operand of the Nth iteration is the source operand of the (N+1)th iteration, address overlap is not supported.
    - When the source operand is a single-point element of a LocalTensor, address overlap with the destination operand is not supported.

- At least one of the left operand and the right operand must be a vector. Currently, both operands being scalars is not supported.
- When this API passes a single-point LocalTensor data element as a scalar, the idx parameter must be a constant known at compile time. If a variable is passed, it must be declared as constexpr.

<!-- npu="950" id9 -->
- For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, the soft simulation behavior does not guarantee that this API is treated as a NOP (no operation).
- Notes on Unified Buffer (UB) space usage. For the Ascend 950PR&950DT products:
  - Tensor high-dimensional slicing computation occupies 8 KB of UB.
  - Continuous computation of the first n tensor data does not involve the 8 KB UB usage.
- For the Ascend 950PR&950DT products, the isSetMask parameter in the API for computing the first n tensor data does not take effect. Keep its default value.
- For the Ascend 950PR&950DT products, only half and float computations are affected by Subnormal, and their handling is controlled by the compilation option --cce-ftz (default value: true):
    - When **--cce-ftz=false**, Subnormal values are preserved during computation and participate in subsequent computation according to their actual values.
    - When **--cce-ftz=true**, the FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in computation are treated as 0, which may cause precision differences in the computation result compared with preserving Subnormal values.
<!-- end id9 -->

## Example<a name="section642mcpsimp"></a>

For more examples, see [LINK](more_examples.md).

- Tensor high-dimensional slicing computation example - mask continuous mode

    ```cpp
    // dstLocal: output tensor
    // src0Local: input tensor
    // src1Local: input tensor

    uint64_t mask = 128;
    // repeatTime = 4, 128 numbers are processed per iteration, so 512 numbers require 4 iterations.
    // dstBlkStride, srcBlkStride = 1, the address interval of the data in src0 involved in the computation within each iteration is 1 datablock, indicating that data is read and written continuously within a single iteration.
    // dstRepStride, srcRepStride = 8, the address interval between adjacent iterations is 8 datablocks, indicating that data is read and written continuously between adjacent iterations.
    // Example with the scalar at the end.
    AscendC::Divs(dstLocal, src0Local, src1Local[0], mask, 4, { 1, 1, 8, 8 });

    // Example with the scalar at the beginning.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Divs<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, mask, 4, {1, 1, 8, 8});
    ```

- Tensor high-dimensional slicing computation example - mask bit-by-bit mode

    ```cpp
    // dstLocal: output tensor.
    // src0Local: input tensor.
    // src1Local: input tensor.

    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 numbers are processed per iteration, so 512 numbers require 4 iterations.
    // dstBlkStride, srcBlkStride = 1. The address interval of the data that src0 participates in computation within each iteration is 1 data block, indicating that data is read and written continuously within a single iteration.
    // dstRepStride, srcRepStride = 8. The address interval between adjacent iterations is 8 data blocks, indicating that data is read and written continuously between adjacent iterations.
    // Example with the scalar as the second operand.
    AscendC::Divs(dstLocal, src0Local, src1Local[0], mask, 4, {1, 1, 8, 8});

    // Example with the scalar as the first operand.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Divs<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, mask, 4, {1, 1, 8, 8});
    ```

- Tensor First n Elements Computation Example

    ```cpp
    // Example with the scalar as the second operand.
    AscendC::Divs(dstLocal, src0Local, src1Local[0], 512);

    // Example with the scalar as the first operand.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Divs<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, 512);
    ```

The result is as follows:

```bash
// Scalar as the second operand, with src1Local[0] used as the scalar.
Input data src0Local: [2 4 6 ... 1024]
Input data src1Local: [2]
Output data dstLocal: [1 2 3 ... 512]

// The scalar comes first, and src0Local[0] is used as the scalar.
Input data src0Local: [1024]
Input data src1Local: [2 2 2 ... 2]
Output data dstLocal: [512 512 512 ... 512]
```
