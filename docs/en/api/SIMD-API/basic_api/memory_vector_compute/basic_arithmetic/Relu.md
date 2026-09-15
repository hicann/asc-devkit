# Relu<a name="ZH-CN_TOPIC_0000001429563265"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T11:37:19.026Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->
## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_unary_intf.h"`.

Relu is a unary vector computation API that performs ReLU (Rectified Linear Unit) computation on the input tensor element by element.

The Rectified Linear Unit (ReLU), also known as the rectified linear unit, is a commonly used activation function in artificial neural networks. In simple terms, ReLU sets all negative input values to 0 and outputs non-negative input values as they are.

The Relu API computation formula is as follows:

$dst_i = \max(0, src_i)$

## Prototype<a name="section620mcpsimp"></a>

- Continuous computation on the first n data elements of the tensor

    ```cpp
    template <typename T>
    __aicore__ inline void Relu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
    ```

- High-dimensional splitting computation on the tensor
    - Bitwise mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Relu(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - Continuous mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Relu(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>The type is LocalTensor, and the supported TPosition is VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>The type is LocalTensor, and the supported TPosition is VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md). |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, all data must be read and computed through multiple iterations (repeats). repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Splitting](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

<!-- npu="950" id10 -->
- For Ascend 950PR/Ascend 950DT, the data types supported by T are half, int32_t, float, and int64_t. The int64_t data type is supported only by the API for computing the first n data of a tensor.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- For Atlas A3 training products/Atlas A3 inference products, the data types supported by T are half, int32_t, and float.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- For Atlas A2 training products/Atlas A2 inference products, the data types supported by T are half, int32_t, and float.
<!-- end id12 -->
<!-- npu="310b" id13 -->
- For Atlas 200I/500 A2 inference products, the data types supported by T are half, int32_t, and float.
<!-- end id13 -->
<!-- npu="310p" id14 -->
- For Atlas inference products AI Core, the data types supported by T are half, int32_t, and float.
<!-- end id14 -->
<!-- npu="910" id15 -->
- For Atlas training products, the data type supported by T is half.
<!-- end id15 -->
## Return Value<a name="section38228281712"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="A3,910b,950" id20 -->
- When the parameter count or repeatTime is set to 0, the behavior of this API is as follows:
  <!-- npu="A3,910b" id23 -->
  - For the following models, when the parameter count or repeatTime is set to 0, no computation is performed, no write is made to the destination operand, and this API is treated as a NOP (no operation).
    <!-- npu="A3" id24 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id24 -->
    <!-- npu="910b" id26 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id26 -->
  <!-- end id23 -->
  <!-- npu="950" id27 -->
  - For Ascend 950PR/Ascend 950DT: this API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_computation.md) through VF. When the parameter count or repeatTime is set to 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id27 -->
<!-- end id20 -->
<!-- npu="950" id21 -->
- Description of UB space usage. For Ascend 950PR/Ascend 950DT:
  - Tensor high-dimensional splitting computation occupies 8KB of Unified Buffer.
  - Continuous computation of the first n data elements of a tensor does not involve the occupation of the 8KB Unified Buffer.
<!-- end id21 -->

## Example<a name="section642mcpsimp"></a>

- Tensor high-dimensional splitting computation example - mask continuous mode

    ```cpp
    uint64_t mask = 256 / sizeof(half);
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Relu(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Tensor high-dimensional splitting computation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Relu(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Sample for computing the first n data of a tensor

    ```cpp
    AscendC::Relu(dstLocal, srcLocal, 512);
    ```

The result is as follows:

```bash
Input data srcLocal: [6.938 -8.86 -0.2263 ... 1.971 1.778]
Output data dstLocal: [6.938 0 0 ... 1.971 1.778]
```
