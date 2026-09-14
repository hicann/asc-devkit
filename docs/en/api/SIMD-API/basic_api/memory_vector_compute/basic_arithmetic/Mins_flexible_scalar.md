# Mins (Flexible Scalar Position)<a name="ZH-CN_TOPIC_0000002135039244"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T11:08:02.942Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`.

Mins is a binary scalar calculation API that computes the smaller value between each element in a vector and a scalar, supporting both scalar-first and scalar-last scenarios. The scalar input supports configuring a LocalTensor single-point element. The calculation formulas are as follows:

$dst_i = \operatorname{Min}(src_i, scalar)$

$dst_i = \operatorname{Min}(scalar, src_i)$

## Prototype<a name="section620mcpsimp"></a>

- Calculation of the first n data elements of the tensor

    ```cpp
    template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
    __aicore__ inline void Mins(const U& dst, const S& src0, const V& src1, const int32_t& count)
    ```

- High-dimensional slicing calculation of the tensor
    - Bitwise mask mode

        ```cpp
        template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
        __aicore__ inline void Mins(const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - Continuous mask mode

        ```cpp
        template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
        __aicore__ inline void Mins(const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| ---- | ---- |
| T | Data type of the operands.<br>For the flexible scalar position API, this is a reserved parameter that is not yet enabled and is retained for future function extension. When it needs to be specified, pass the default value BinaryDefaultType. |
| isSetMask | Whether to set the mask mode and mask value inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. The developer needs to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameters must be set to the placeholder MASK_PLACEHOLDER.<br>For details about how to use it, see [Mask](../SIMD_compute/mask.md). |
| config | Type: BinaryConfig. It takes effect when the scalar is a LocalTensor single-point element and is used to specify the position of the single-point element operand. The default value DEFAULT_BINARY_CONFIG indicates that the right operand is the scalar.<br>struct BinaryConfig {<br>    int8_t scalarTensorIndex = 1; // Specifies the position of the scalar when the scalar is a LocalTensor single-point element. 0 indicates the left operand, and 1 indicates the right operand.<br>};<br>constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1};<br>|
| U | LocalTensor type. The corresponding data type is automatically derived from the input parameter dst. The developer does not need to configure this parameter, but must ensure that dst meets the data type constraints. |
| S | LocalTensor type or scalar type. The corresponding data type is automatically derived from the input parameter src0. The developer does not need to configure this parameter, but must ensure that src0 meets the data type constraints. |
| V | LocalTensor type or scalar type. The corresponding data type is automatically derived from the input parameter src1. The developer does not need to configure this parameter, but must ensure that src1 meets the data type constraints. |

**Table 2** Parameters

| Parameter | Type | Description |
| ---- | ---- | ---- |
| dst | Output | Destination operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraints, see [Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src0/src1 | Input | Source operands in the flexible scalar position API.<br>&bull; When the type is LocalTensor: it can be used as a vector operand or a scalar single-point element, and the supported TPosition values are VECIN/VECCALC/VECOUT. For the address alignment constraints, see [Address Alignment Constraints](../../../general_description_and_constraints.md). <br>&bull; The type is scalar. |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md). |
| mask/mask[] | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector calculation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeats) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Splitting](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

<!-- npu="950" id8 -->
For Ascend 950PR/Ascend 950DT, the supported data types are int8_t, uint8_t, int16_t, half, bfloat16_t, int32_t, float, int64_t, and uint64_t. Among them, the int8_t, uint8_t, int64_t, and uint64_t data types are supported only by the API that computes the first n data of a tensor.
<!-- end id8 -->

## Return Value<a name="section194321251175110"></a>

None

## Constraints<a name="section199031843133716"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
- When the flexible scalar position API is called and the source operand is a LocalTensor single-point element, address overlap between the source operand and the destination operand is not supported.
- At least one of the left operand and the right operand must be a vector. Currently, both operands being scalars is not supported.
- When LocalTensor single-point data is passed as a scalar to this API, the idx parameter must be a compile-time constant. If a variable is passed, it must be declared as constexpr.

<!-- npu="950" id9 -->
- For Ascend 950PR/Ascend 950DT: This API is implemented for compatibility by calling the [Reg Vector Calculation API](../../reg_vector_compute/reg_vector_computation.md) through VF. When the count or repeatTime parameter is set to 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
- Description of Unified Buffer space usage. For Ascend 950PR/Ascend 950DT:
  - Tensor high-dimensional slicing calculation occupies 8 KB of Unified Buffer.
  - Continuous calculation of the first n data of a tensor does not involve the occupation of the 8 KB Unified Buffer.
- For Ascend 950PR/Ascend 950DT, the isSetMask parameter in the API for calculating the first n data of a tensor does not take effect. Keep its default value.
<!-- end id9 -->

## Example<a name="section633mcpsimp"></a>

For more examples, see [LINK](more_sample_9.md).

- Example of tensor high-dimensional slicing calculation - continuous mask mode

    ```cpp
    uint64_t mask = 128;
    // repeatTime = 4, 128 numbers are processed in a single iteration, and 4 iterations are required to calculate 512 numbers.
    // dstBlkStride, srcBlkStride = 1, the address interval of the data involved in the calculation of src0 within each iteration is 1 datablock, indicating that data is read and written continuously within a single iteration.
    // dstRepStride, srcRepStride = 8, the address interval between adjacent iterations is 8 datablocks, indicating that data is read and written continuously between adjacent iterations.
    // Example with the scalar at the end.
    AscendC::Mins(dstLocal, src0Local, src1Local[0], mask, 4, { 1, 1, 8, 8 });
    
    // Example with the scalar at the beginning.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Mins<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of tensor high-dimensional slicing calculation - bitwise mask mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 numbers are processed in a single iteration, and 4 iterations are required to calculate 512 numbers.
    // dstBlkStride, srcBlkStride = 1, the address interval of the data of src0 involved in the calculation within each iteration is 1 datablock, indicating that data is read and written continuously within a single iteration
    // dstRepStride, srcRepStride = 8, the address interval between adjacent iterations is 8 datablocks, indicating that data is read and written continuously between adjacent iterations
    // Example with scalar at the end
    AscendC::Mins(dstLocal, src0Local, src1Local[0], mask, 4, {1, 1, 8, 8});
    
    // Example with scalar at the front
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Mins<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of computing the first n data of a tensor

    ```cpp
    // Example with scalar at the end
    AscendC::Mins(dstLocal, src0Local, src1Local[0], 512);
    
    // Example with scalar at the front
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Mins<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, 512);
    ```

The result is as follows:

```bash
Input data (src0Local): [1 2 3 ... 512]
Input data src1Local = [2 2 2 ... 2]
// Scalar at the front, src0Local[0] is used as the scalar
Output data (dstLocal): [1 1 1 ... 1]
// Scalar at the end, src1Local[0] as the scalar
Output data (dstLocal): [1 2 2 ... 2]
```
