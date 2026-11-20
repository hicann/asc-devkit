# LeakyRelu<a name="ZH-CN_TOPIC_0000001475000405"></a>

<!-- md-trans-meta sourceCommit=f266635ab179c912cc9641061c2d27246be9170c translatedAt=2026-11-02T06:32:46.512Z pushedAt=2026-11-18T01:25:07.015Z nl=2 -->

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
- Atlas training products: not supported
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/LeakyRelu_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`.

LeakyRelu is a binary scalar computation API. Its inputs are a vector tensor and a scalar. LeakyRelu is responsible for performing the Leaky ReLU (Leaky Rectified Linear Unit) operation on each element in the vector tensor. The calculation formula is as follows:

$dst_i = \begin{cases} src_i, & src_i \geq 0 \\ scalarValue \times src_i, & src_i < 0 \end{cases}$

Leaky ReLU is a commonly used activation function in artificial neural networks. Its mathematical expression is:

$f(x) = \begin{cases} x, & x \geq 0 \\ \alpha x, & x < 0 \end{cases}$

where $\alpha$ is the negative slope (corresponding to the scalarValue parameter).

The difference from ReLU is that ReLU sets all negative values to zero, whereas Leaky ReLU assigns a slope to all negative values. The following figure shows the difference between ReLU and Leaky ReLU:

![](../../../../figures/image_0000001475211729.png) ![](../../../../figures/image_0000001425411698.png)

## Prototype<a name="section620mcpsimp"></a>

- Tensor first n elements computation

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void LeakyRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
    ```

- Tensor high-dimensional slicing computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void LeakyRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void LeakyRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

When dst and src use the [TensorTrait](../../aux_data_structures/TensorTrait/TensorTrait.md) type, the data type TensorTrait is inconsistent with the data type of scalarValue (corresponding to the LiteType type in TensorTrait). Therefore, a new template type U is introduced to represent the data type of scalarValue, and std::enable\_if is used to check whether the LiteType extracted from T is exactly the same as U. If they are the same, the API passes compilation; otherwise, compilation fails. The API prototype is defined as follows:

- Tensor first n elements computation

    ```cpp
    template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void LeakyRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
    ```

- Tensor high-dimensional slicing computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void LeakyRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

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
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use [SetVectorMask](../mask_operations/SetVectorMask.md) to set the mask value. In this mode:<br>&nbsp;&nbsp;&bull; For the Tensor first n elements computation API, the count input parameter does not take effect. It is recommended to set it to 1.<br>&nbsp;&nbsp;&bull; For the Tensor high-dimensional slicing computation API, the mask input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md). |
| scalarValue | Input | Source operand. Its data type must be consistent with the element type of the destination operand. |
| count | Input | Number of elements involved in the calculation. For details about this parameter, see [Continuous Compute](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements involved in the calculation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector compute unit reads 256 consecutive bytes of data for calculation each time. To process all input data, multiple iterations (repeat) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameter that controls the address step of the operand. Type: [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which contains parameters such as the address step of the same DataBlock between adjacent iterations of the operand and the address step of different DataBlocks within the same iteration of the operand.<br>For details about the address step between adjacent iterations, refer to [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address step of DataBlocks within the same iteration, refer to [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Type<a name="zh-cn_topic_0000002530007180_section4219135304818"></a>

Supported data types for T and U: half, float.
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/LeakyRelu_res.md#id2 -->

## Return Value<a name="section194321251175110"></a>

None

## Constraints<a name="section1636318590"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="A3,910b,950" id16 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id17 -->
  - For the following models, when the count or repeatTime parameter is set to 0, no computation is performed and no data is written to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id19 -->
    - Atlas A3 products
    <!-- end id19 -->
    <!-- npu="910b" id21 -->
    - Atlas A2 products
    <!-- end id21 -->
  <!-- end id17 -->
  <!-- npu="950" id23 -->
  - For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id23 -->
<!-- end id16 -->
<!-- npu="950" id24 -->
- Notes on Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - High-dimensional split computation of the tensor occupies 8 KB of UB.
  - Continuous computation of the first n data elements of the tensor does not involve the 8 KB UB usage.
- For Ascend 950PR&950DT products, the isSetMask parameter in the API for computing the first n tensor data does not take effect. Keep the default value.
<!-- end id24 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/LeakyRelu_res.md#id3 -->

## Examples<a name="section112161084424"></a>

- Tensor high-dimensional split calculation example - mask continuous mode

    ```cpp
    uint64_t mask = 128;
    half scalar = 0.001;
    AscendC::LeakyRelu(dstLocal, srcLocal, scalar, mask, 4, {1, 1, 8, 8});
    ```

- Tensor high-dimensional split calculation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    half scalar = 0.001;
    AscendC::LeakyRelu(dstLocal, srcLocal, scalar, mask, 4, {1, 1, 8, 8});
    ```

- Example of computing the first n tensor data

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
