# Reciprocal<a name="ZH-CN_TOPIC_0000001429443329"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T11:33:06.000Z -->

## Applicable Products<a name="section1550532418810"></a>

### Prototype without config

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
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
### Prototype with config

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products Vector Core: Not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: Not supported
<!-- end id16 -->
## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_unary_intf.h"`.

Reciprocal is a unary vector computation API that computes the reciprocal of each element of the input tensor. The computation formula is as follows:

$dst_i = \frac{1}{src_i}$

## Prototype<a name="section620mcpsimp"></a>

- Prototype with config

    - Continuous computation of the first n data of the tensor

        ```cpp
        template <typename T, const ReciprocalConfig& config = DEFAULT_RECIPROCAL_CONFIG>
        __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - High-dimensional slicing computation of the tensor
        - Bitwise mask mode

            ```cpp
            template <typename T, bool isSetMask = true, const ReciprocalConfig& config = DEFAULT_RECIPROCAL_CONFIG>
            __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - Continuous mask mode

            ```cpp
            template <typename T, bool isSetMask = true, const ReciprocalConfig& config = DEFAULT_RECIPROCAL_CONFIG>
            __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

- Prototype without config

    - Continuous computation of the first n data of the tensor

        ```cpp
        template <typename T>
        __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - High-dimensional slicing computation of the tensor
        - Bitwise mask mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - Continuous mask mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |
| <!-- npu="950" id19 -->config | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br>It is used to configure the Subnormal computation mode. The type is ReciprocalConfig, which is defined as follows:<br>enum&nbsp;class&nbsp;ReciprocalAlgo&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;INTRINSIC&nbsp;=&nbsp;0,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct&nbsp;ReciprocalConfig&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;ReciprocalAlgo&nbsp;algo&nbsp;=&nbsp;ReciprocalAlgo::INTRINSIC;<br>};<br>The Subnormal computation mode is configured through the algo parameter of the ReciprocalConfig structure. The values of algo are as follows:<br>&bull; ReciprocalAlgo::INTRINSIC and ReciprocalAlgo::PRECISION_1ULP_FTZ_TRUE: the result is computed using a single instruction, and all Subnormal values are approximated to 0.<br>&bull; ReciprocalAlgo::PRECISION_1ULP_FTZ_FALSE: Subnormal data computation is supported.<br>The default value DEFAULT_RECIPROCAL_CONFIG of this parameter is as follows:<br>constexpr&nbsp;ReciprocalConfig&nbsp;DEFAULT_RECIPROCAL_CONFIG&nbsp;=&nbsp;{&nbsp;ReciprocalAlgo::INTRINSIC&nbsp;};<br>If the config parameter is not explicitly passed when this prototype is called, DEFAULT_RECIPROCAL_CONFIG is used by default, and the behavior is equivalent to that of the prototype without the config parameter.<!-- end id19 --> |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md). |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeats) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Partitioning](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

<!-- npu="950" id20 -->
- For Ascend 950PR/Ascend 950DT, T supports the following data types: half, float, int64_t, and uint64_t. The int64_t and uint64_t data types are supported only by the API that computes the first n data of the tensor.
<!-- end id20 -->
<!-- npu="A3" id21 -->
- For Atlas A3 training products/Atlas A3 inference products, T supports the following data types: half and float.
<!-- end id21 -->
<!-- npu="910b" id22 -->
- For Atlas A2 training products/Atlas A2 inference products, T supports the following data types: half and float.
<!-- end id22 -->
<!-- npu="310b" id23 -->
- For Atlas 200I/500 A2 inference products, T supports the following data types: half and float.
<!-- end id23 -->
<!-- npu="310p" id24 -->
- For Atlas inference products AI Core, T supports the following data types: half and float.
<!-- end id24 -->
<!-- npu="910" id25 -->
- For Atlas training products, T supports the following data types: half and float.
<!-- end id25 -->
## Return Value<a name="section15227195416231"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="A3,910b,950" id28 -->
- When the value of count or repeatTime is 0, the behavior of this API is as follows:
  <!-- npu="A3,910b" id29 -->
  - For the following models, when the value of count or repeatTime is 0, no computation is performed, the destination operand is not written, and this API is treated as a NOP (no operation).
    <!-- npu="A3" id30 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id30 -->
    <!-- npu="910b" id31 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id31 -->
  <!-- end id29 -->
  <!-- npu="950" id33 -->
  - For Ascend 950PR/Ascend 950DT: This API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_computation.md) through VF. When the value of count or repeatTime is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id33 -->
<!-- end id28 -->
<!-- npu="950" id34 -->
- Description of UB space usage. For Ascend 950PR/Ascend 950DT:
  - High-dimensional slicing computation of tensor occupies 8 KB of Unified Buffer.
  - Continuous computation of the first n data of tensor does not occupy the 8 KB Unified Buffer.
<!-- end id34 -->

- If the value in src is 0, an unknown result may be produced.
- When Reciprocal is used, the comparison error of the half operator result does not meet the requirement of two parts per thousand, and the comparison error of the float operator result does not meet the requirement of two parts per ten thousand. If high precision is required, use [Div](Div.md) instead.

## Example<a name="section176061616102911"></a>

- Example of tensor high-dimensional slicing computation - continuous mask mode

    ```cpp
    uint64_t mask = 256 / sizeof(half);
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Reciprocal(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of tensor high-dimensional slicing computation - bitwise mask mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Reciprocal(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of the API for computing the first n data of a tensor

    ```cpp
    AscendC::Reciprocal(dstLocal, srcLocal, 512);
    ```

    <!-- npu="950" id32 -->
    The following examples are supported only on Ascend 950PR/Ascend 950DT

    ```cpp
    static constexpr ReciprocalConfig config = { ReciprocalAlgo::PRECISION_1ULP_FTZ_FALSE };
    AscendC::Reciprocal<T, config>(dstLocal, srcLocal, 512);
    ```
    <!-- end id32 -->

The result is as follows:

```bash
Input data srcLocal: [-7.152 -7.24 1.771 ... -1.339 4.473]
Output data dstLocal: [-0.1396 -0.1382 0.5645 ... -0.748 0.2231]
```
