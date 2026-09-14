# Rsqrt<a name="ZH-CN_TOPIC_0000001378924050"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T11:40:45.152Z -->

## Applicable Products<a name="section1550532418810"></a>

### Prototype Without config

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

Rsqrt is a unary vector calculation API that performs square root followed by reciprocal on each element of the input tensor. The calculation formula is as follows:

$dst_i = \frac{1}{\sqrt{src_i}}$

## Prototype<a name="section620mcpsimp"></a>

- Prototype with config

    - Continuous calculation of the first n data of the tensor

        ```cpp
        template <typename T, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
        __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - High-dimensional split calculation of the tensor
        - Bitwise mask mode

            ```cpp
            template <typename T, bool isSetMask = true, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
            __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - Continuous mask mode

            ```cpp
            template <typename T, bool isSetMask = true, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
            __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

- Prototype without config

    - Continuous calculation of the first n data of the tensor

        ```cpp
        template <typename T>
        __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - High-dimensional split calculation of the tensor
        - Bitwise mask mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - Continuous mask mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |
| <!-- npu="950" id19 -->config | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br>It is used to configure the precision calculation mode. The type is RsqrtConfig, which is defined as follows:<br>enum&nbsp;class&nbsp;RsqrtAlgo&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;INTRINSIC&nbsp;=&nbsp;0,<br>&nbsp;&nbsp;&nbsp;&nbsp;FAST_INVERSE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_0ULP_FTZ_FALSE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct&nbsp;RsqrtConfig&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;RsqrtAlgo&nbsp;algo&nbsp;=&nbsp;RsqrtAlgo::INTRINSIC;<br>};<br>The precision calculation mode is configured through the algo parameter of the RsqrtConfig structure. The values of algo are as follows:<br>&bull; RsqrtAlgo::INTRINSIC and RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE: The result is calculated using a single instruction, with a maximum precision error of 1 ulp.<br>&bull; RsqrtAlgo::FAST_INVERSE and RsqrtAlgo::PRECISION_0ULP_FTZ_FALSE: The result is calculated using the fast inverse algorithm. This applies to calculations where the input value is within the range [0, 85070596800837026223494223584045301760]. Within this range, the algorithm guarantees a maximum precision error of 0 ulp for the output. When the input value is greater than 85070596800837026223494223584045301760, the output is inf. Currently, this algorithm supports the float data type and supports Subnormal data calculation in this mode.<br>&bull; RsqrtAlgo::PRECISION_1ULP_FTZ_FALSE: Only Subnormal data calculation of the half type is supported, with a maximum precision error of 1 ulp.<br>The default value DEFAULT_RSQRT_CONFIG of this parameter is as follows:<br>constexpr&nbsp;RsqrtConfig&nbsp;DEFAULT_RSQRT_CONFIG&nbsp;=&nbsp;{&nbsp;RsqrtAlgo::INTRINSIC&nbsp;};<br>If the config parameter is not explicitly passed when this prototype is called, DEFAULT_RSQRT_CONFIG is used by default. In this case, the behavior is equivalent to that of the prototype without the config parameter.<!-- end id19 --> |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the calculation. For details about this parameter, see [Continuous Calculation](../SIMD_compute/continuous_computation.md). |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector calculation unit reads 256 bytes of consecutive data for calculation each time. To complete the processing of the input data, multiple iterations (repeat) are required to read and calculate all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

The supported data types of T are half and float.

## Return Value<a name="section42373919454"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="A3,910b,950" id20 -->
- When the value of count or repeatTime is 0, the behavior of this API is as follows:
  <!-- npu="A3,910b" id26 -->
  - For the following models, when the value of count or repeatTime is 0, no calculation is performed, no data is written to the destination operand, and this API is treated as a NOP (no operation).
    <!-- npu="A3" id27 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id27 -->
    <!-- npu="910b" id29 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id29 -->
  <!-- end id26 -->
  <!-- npu="950" id30 -->
  - For Ascend 950PR/Ascend 950DT: This API is implemented for compatibility by calling the [Reg Vector Calculation API](../../reg_vector_compute/reg_vector_computation.md) through VF. When the value of count or repeatTime is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id30 -->
<!-- end id20 -->
<!-- npu="950" id21 -->
- Description of UB space usage. For Ascend 950PR/Ascend 950DT:
  - Tensor high-dimensional split calculation occupies 8 KB of Unified Buffer.
  - Continuous calculation of the first n data elements of a tensor does not involve the occupation of the 8 KB Unified Buffer.
<!-- end id21 -->

- If the value in src is non-positive, unknown results may be produced.
- When Rsqrt is used, the comparison error of the half operator result does not meet the requirement of two parts per thousand, and the comparison error of the float operator result does not meet the requirement of two parts per ten thousand. If high precision is required, use [Div](Div.md) and [Sqrt](Sqrt.md) instead.

## Example<a name="section642mcpsimp"></a>

- tensor high-dimensional split calculation example - mask continuous mode

    ```cpp
    uint64_t mask = 256 / sizeof(half);
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Rsqrt(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- tensor high-dimensional split calculation example - mask bitwise mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Rsqrt(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- example of the API for calculating the first n data elements of a tensor

    ```cpp
    AscendC::Rsqrt(dstLocal, srcLocal, 512);
    ```

    <!-- npu="950" id23 -->
    The following examples are supported only on Ascend 950PR/Ascend 950DT.

    ```cpp
    static constexpr RsqrtConfig config = { RsqrtAlgo::FAST_INVERSE };
    AscendC::Rsqrt<T, config>(dstLocal, srcLocal, 512);
    ```
    <!-- end id23 -->

The result is as follows:

```bash
Input data srcLocal: [0.8335 2.2 2.672 ... 2.312 5.36]
Output data dstLocal: [1.094 0.676 0.6113 ... 0.6562 0.4316]
```
