# LeakyRelu<a name="ZH-CN_TOPIC_0000001475000405"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T10:43:18.982Z -->

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
- Atlas training products: Not supported
<!-- end id7 -->
## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`

LeakyRelu is a binary scalar computation API. It takes a vector tensor and a scalar as inputs, and applies the Leaky ReLU (Leaky Rectified Linear Unit) operation to each element in the vector tensor. The calculation formula is as follows:

$dst_i = \begin{cases} src_i, & src_i \geq 0 \\ scalarValue \times src_i, & src_i < 0 \end{cases}$

Leaky ReLU, the leaky rectified linear unit function, is an activation function commonly used in artificial neural networks. Its mathematical expression is as follows:

$f(x) = \begin{cases} x, & x \geq 0 \\ \alpha x, & x < 0 \end{cases}$

where $\alpha$ is the negative slope (corresponding to the scalarValue parameter).

The difference from ReLU is that ReLU sets all negative values to zero, whereas Leaky ReLU assigns a slope to all negative values. The following figure illustrates the difference between ReLU and Leaky ReLU:

![](../../../../figures/zh-cn_image_0000001475211729.png) ![](../../../../figures/zh-cn_image_0000001425411698.png)

## Prototype<a name="section620mcpsimp"></a>

- tensor first n data calculation

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void LeakyRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
    ```

- tensor high-dimensional splitting calculation
    - Bitwise mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void LeakyRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - Continuous mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void LeakyRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

When dst and src use the [TensorTrait](../../aux_data_structures/TensorTrait/TensorTrait.md) type, the data type of TensorTrait is inconsistent with the data type of scalarValue (corresponding to the LiteType type in TensorTrait). Therefore, a new template type U is added to represent the data type of scalarValue, and std::enable\_if checks whether the LiteType extracted from T is exactly the same as U. If they are the same, the API passes compilation; otherwise, compilation fails. The API prototype is defined as follows:

- tensor first n data calculation

    ```cpp
    template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void LeakyRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
    ```

- tensor high-dimensional splitting calculation
    - Bitwise mask mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void LeakyRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - Continuous mask mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void LeakyRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section166351181919"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| U | Data type of scalarValue. Valid only when dst and src use the [TensorTrait](../../aux_data_structures/TensorTrait/TensorTrait.md) type. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: indicates that the mask is set inside the API.<br>&bull; false: indicates that the mask is set outside the API, and developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode:<br>&nbsp;&nbsp;&bull; For the API that calculates the first n data of a tensor, the count input parameter does not take effect, and it is recommended to set it to 1.<br>&nbsp;&nbsp;&bull; For the tensor high-dimensional splitting calculation API, the mask value in the API input parameters is set to the placeholder `MASK_PLACEHOLDER`, which is used as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>The type is LocalTensor, and the supported TPosition is VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraint](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>The type is LocalTensor, and the supported TPosition is VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraint](../../../general_description_and_constraints.md). |
| scalarValue | Input | Source operand. The data type must be consistent with the element type in the destination operand. |
| count | Input | Number of elements involved in the calculation. For details about this parameter, see [Continuous Mode](../SIMD_compute/continuous_computation.md). |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector calculation unit reads 256 consecutive bytes of data for calculation each time. To complete the processing of the input data, multiple iterations (repeat) are required to read and calculate all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-dimensional Splitting](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type<a name="zh-cn_topic_0000002530007180_section4219135304818"></a>

The data types supported by T and U are half and float.

## Return Value<a name="section194321251175110"></a>

None

## Constraints<a name="section1636318590"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="A3,910b,950" id16 -->
- When the value of the count or repeatTime parameter is 0, the behavior of this API is as follows:
  <!-- npu="A3,910b" id17 -->
  - For the following models, when the value of the count or repeatTime parameter is 0, no calculation operation is performed, no write is made to the destination operand, and this API is treated as a NOP (no operation).
    <!-- npu="A3" id19 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id19 -->
    <!-- npu="910b" id21 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id21 -->
  <!-- end id17 -->
  <!-- npu="950" id23 -->
  - For Ascend 950PR/Ascend 950DT: This API is implemented for compatibility by calling the [Reg vector calculation API](../../reg_vector_compute/reg_vector_computation.md) through VF. When the value of the count or repeatTime parameter is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id23 -->
<!-- end id16 -->
<!-- npu="950" id24 -->
- Description of Unified Buffer space usage. For Ascend 950PR/Ascend 950DT:
  - Tensor high-dimensional splitting calculation occupies 8KB of Unified Buffer.
  - Continuous calculation of the first n data of a tensor does not involve the occupation of the 8KB Unified Buffer.
- For Ascend 950PR/Ascend 950DT, the isSetMask parameter in the API for calculating the first n data of a tensor does not take effect. Keep its default value.
<!-- end id24 -->

## Example<a name="section112161084424"></a>

- Example of tensor high-dimensional splitting calculation - continuous mask mode

    ```cpp
    uint64_t mask = 128;
    half scalar = 0.001;
    AscendC::LeakyRelu(dstLocal, srcLocal, scalar, mask, 4, {1, 1, 8, 8});
    ```

- Example of tensor high-dimensional splitting calculation - bitwise mask mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    half scalar = 0.001;
    AscendC::LeakyRelu(dstLocal, srcLocal, scalar, mask, 4, {1, 1, 8, 8});
    ```

- Example of calculating the first n data of a tensor

    ```cpp
    half scalar = 0.001;
    AscendC::LeakyRelu(dstLocal, srcLocal, scalar, 512);
    ```

The result is as follows:

```bash
Input data src0Local: [-287.  246. -438.  177. 596. -950.  -293.  322. ... -900.]
Input data scalar = 0.001
Output data dstLocal: [-0.287 246. -0.438 177. 596. -0.950 -0.293 322. ... -0.900]
```
