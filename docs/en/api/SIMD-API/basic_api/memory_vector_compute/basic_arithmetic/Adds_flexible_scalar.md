# Adds (Flexible Scalar Position)<a name="ZH-CN_TOPIC_0000002170278873"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:04:45.288Z pushedAt=2026-10-28T03:14:39.353Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Adds_flexible_scalar_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`.

Adds is a binary scalar computation API that performs addition between each element in a vector and a scalar. It supports two scenarios: scalar first and scalar last. The scalar input supports configuring a LocalTensor single-point element. The calculation formulas are as follows:

$dst_i = src_i + scalar$

$dst_i = scalar + src_i$

## Prototype<a name="section620mcpsimp"></a>

- Tensor first n elements computation

    ```cpp
    template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
    __aicore__ inline void Adds(const U& dst, const S& src0, const V& src1, const int32_t& count)
    ```

- Tensor high-dimensional split computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
        __aicore__ inline void Adds(const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
        __aicore__ inline void Adds(const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| ---- | ---- |
| T | Operand data type.<br>For the flexible scalar position API, this is a reserved parameter that is not yet enabled and is kept for future function extension. When it needs to be specified, pass the default value BinaryDefaultType. |
| isSetMask | Whether to set the mask mode and mask value inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use [SetVectorMask](../mask_operations/SetVectorMask.md) to set the mask value. In this mode, the mask value in the API input parameters must be set to the placeholder MASK_PLACEHOLDER.<br>For details, see [Mask](../SIMD_compute/mask.md). |
| config | Type BinaryConfig. It takes effect when the scalar is a LocalTensor single-point element and specifies the position of the single-point element operand. The default value DEFAULT_BINARY_CONFIG indicates that the right operand is a scalar.<br>struct BinaryConfig {<br>    int8_t scalarTensorIndex = 1; // Specifies the position of the scalar when the scalar is a LocalTensor single-point element. 0 indicates the left operand, and 1 indicates the right operand.<br>};<br>constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1}; |
| U | LocalTensor type. The corresponding data type is automatically inferred from the input parameter dst. You do not need to configure this parameter, but ensure that dst meets the data type constraint. |
| S | LocalTensor type or scalar type. The corresponding data type is automatically inferred from the input parameter src0. You do not need to configure this parameter, but ensure that src0 meets the data type constraint. |
| V | LocalTensor type or scalar type. The corresponding data type is automatically inferred from the input parameter src1. You do not need to configure this parameter, but ensure that src1 meets the data type constraint. |

**Table 2** Parameters

| Parameter | Type | Description |
| ---- | ---- | ---- |
| dst | Output | Destination operand.<br>Type LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src0/src1 | Input | Source operands in the flexible scalar position API.<br>&bull; Type LocalTensor: Can be used as a vector operand or a scalar single-point element. Supported TPosition: VECIN/VECCALC/VECOUT. For the address alignment constraint, see [Address Alignment Constraints](../../../general_description_and_constraints.md). <br>&bull; Type scalar. |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_compute.md). |
| mask/mask[] | Input | mask controls the elements involved in the computation within each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To process all input data, multiple iterations (repeat) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the operand address stride. Type [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), including the address stride of the same DataBlock between adjacent iterations of an operand, the address stride of different DataBlocks within the same iteration of an operand, and other parameters.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the DataBlock address stride within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, the supported data types are uint8_t, int8_t, half, bfloat16_t, int16_t, float, int32_t, uint64_t, int64_t, complex32, and complex64. Among them, int8_t, uint8_t, uint64_t, int64_t, complex32, and complex64 are supported only by the API for computing the first n data of a tensor.
<!-- end id8 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
- When the flexible scalar position API is called and the source operand is a single-point element of a LocalTensor, address overlap between the source operand and the destination operand is not supported.
- At least one of the left operand and the right operand must be a vector. Currently, both operands being scalars is not supported.
- When this API passes single-point LocalTensor data as a scalar, the idx parameter must be a compile-time constant. If a variable is passed, it must be declared as constexpr.

<!-- npu="950" id9 -->
- For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is 0, the soft simulation behavior does not guarantee that this API is treated as a NOP (no operation).
- Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - Tensor high-dimensional slicing computation occupies 8 KB of UB.
  - Continuous computation of the first n data of a tensor does not involve the 8 KB UB usage.
- For Ascend 950PR&950DT products, the isSetMask parameter in the API for computing the first n tensor data does not take effect. Keep its default value.
<!-- end id9 -->

## Example<a name="section642mcpsimp"></a>

For more examples, see [LINK](more_examples.md).

- Example of tensor high-dimensional slicing computation - mask continuous mode

    ```cpp
    uint64_t mask = 128;
    // repeatTime = 4. Process 128 numbers per iteration. Computing 512 numbers requires 4 iterations.
    // dstBlkStride, srcBlkStride = 1. Within each iteration, the address interval of the data involved in src0 computation is 1 datablock, indicating that data is read and written continuously within a single iteration.
    // dstRepStride, srcRepStride = 8. The address interval between adjacent iterations is 8 datablocks, indicating that data is read and written continuously between adjacent iterations.
    // Example of calling with the scalar at the end.
    AscendC::Adds(dstLocal, src0Local, src1Local[0], mask, 4, { 1, 1, 8, 8 });
    
    // Example of calling with the scalar at the beginning.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Adds<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of tensor high-dimensional slicing computation - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4. Process 128 numbers per iteration. Computing 512 numbers requires 4 iterations.
    // dstBlkStride, srcBlkStride = 1, the address interval of the data that src0 participates in computing within each iteration is 1 datablock, indicating that data is read and written continuously within a single iteration.
    // dstRepStride, srcRepStride = 8, the address interval between adjacent iterations is 8 datablocks, indicating that data is read and written continuously between adjacent iterations.
    // Example of calling with the scalar at the end.
    AscendC::Adds(dstLocal, src0Local, src1Local[0], mask, 4, {1, 1, 8, 8});
    
    // Example of calling with the scalar at the front.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Adds<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of computing the first n data elements of a tensor.

    ```cpp
    // Example of calling with the scalar at the end.
    AscendC::Adds(dstLocal, src0Local, src1Local[0], 512);
    
    // Example of calling with the scalar at the front.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Adds<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, 512);
    ```

The result is as follows:

```bash
// Use src0Local[0] or src1Local[0] as the scalar.
Input data src0Local: [1 2 3 ... 512]
Input data src1Local: [1 2 3 ... 512]
Output data dstLocal: [2 3 4 ... 513]
```
