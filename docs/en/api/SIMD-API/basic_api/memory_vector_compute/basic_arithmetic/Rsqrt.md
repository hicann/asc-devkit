# Rsqrt<a name="ZH-CN_TOPIC_0000001378924050"></a>

<!-- md-trans-meta sourceCommit=f266635ab179c912cc9641061c2d27246be9170c translatedAt=2026-11-02T06:58:30.721Z pushedAt=2026-11-18T01:25:07.042Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

### Prototype without passing config

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: not supported
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
- Atlas training products: supported
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Rsqrt_res.md#id1 -->

### Prototype with config

<!-- npu="950" id10 -->
- Ascend 950PR&950DT products: supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 products: not supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 products: not supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core: not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products Vector Core: not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: not supported
<!-- end id16 -->
<!-- npu="x90" id17 -->
- Kirin X90 processor products: Not supported
<!-- end id17 -->
<!-- npu="9030" id18 -->
- Kirin 9030 processor products: Not supported
<!-- end id18 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Rsqrt_res.md#id2 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_unary_intf.h"`.

Rsqrt is a unary vector computation API that is responsible for performing the square root operation and then taking the reciprocal on the input tensor element-wise. The calculation formula is as follows:

$dst_i = \frac{1}{\sqrt{src_i}}$

## Prototype<a name="section620mcpsimp"></a>

- Prototype with config

    - Continuous computation of the first n elements of the tensor

        ```cpp
        template <typename T, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
        __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - Tensor high-dimensional slicing computation
        - **mask** bit-by-bit mode

            ```cpp
            template <typename T, bool isSetMask = true, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
            __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - **mask** continuous mode

            ```cpp
            template <typename T, bool isSetMask = true, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
            __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

- Prototype without passing config

    - Continuous computation of the first n elements of the tensor

        ```cpp
        template <typename T>
        __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - Tensor high-dimensional slicing computation
        - **mask** bit-by-bit mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - **mask** continuous mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operands. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |
| <!-- npu="950" id19 -->config | This parameter is supported only on Ascend 950PR&950DT products.<br>It configures the precision computation mode. The type is RsqrtConfig, defined as follows:<br>enum&nbsp;class&nbsp;RsqrtAlgo&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;INTRINSIC&nbsp;=&nbsp;0,<br>&nbsp;&nbsp;&nbsp;&nbsp;FAST_INVERSE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_0ULP_FTZ_FALSE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct&nbsp;RsqrtConfig&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;RsqrtAlgo&nbsp;algo&nbsp;=&nbsp;RsqrtAlgo::INTRINSIC;<br>};<br>The algo parameter of the RsqrtConfig struct selects the Rsqrt algorithm and configures the Subnormal mode. For details, see [Key Features](#rsqrt-key-features).<br>The values of algo are as follows:<br>&bull; RsqrtAlgo::INTRINSIC: Default algorithm, with a maximum precision error of 1 ulp. For half and float types, Subnormal handling is controlled by the compilation option --cce-ftz (default value is true).<br>&bull; RsqrtAlgo::FAST_INVERSE and RsqrtAlgo::PRECISION_0ULP_FTZ_FALSE: Use the fast inverse algorithm to obtain the result. Currently, this algorithm supports only the float data type and supports Subnormal data computation in this mode.<br>&bull; RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE: Uses single-instruction computation, with a maximum precision error of 1 ulp.<br>&bull; RsqrtAlgo::PRECISION_1ULP_FTZ_FALSE: Maximum precision error is 1 ulp, and supports Subnormal data computation of the half type.<br><br>The default value DEFAULT_RSQRT_CONFIG of this parameter is as follows:<br>constexpr&nbsp;RsqrtConfig&nbsp;DEFAULT_RSQRT_CONFIG&nbsp;=&nbsp;{&nbsp;RsqrtAlgo::INTRINSIC&nbsp;};<br>If config is not explicitly passed when calling this prototype, DEFAULT_RSQRT_CONFIG is used by default, and the behavior is equivalent to the prototype without the config parameter.<!-- end id19 --> |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the calculation. For details about this parameter, see [Continuous Compute](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements involved in the calculation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector compute unit reads 256 consecutive bytes of data for calculation each time. To process all input data, multiple iterations (repeat) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. It is of the [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md) type, including the address stride of the same DataBlock between adjacent iterations of the operand, the address stride of different DataBlocks within the same iteration, and other parameters.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

Supported data types for T: half, float.
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Rsqrt_res.md#id3 -->

## Return Value<a name="section42373919454"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="A3,910b,950" id20 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id26 -->
  - For the following models, when the count or repeatTime parameter is set to 0, no computation is performed and no data is written to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id27 -->
    - Atlas A3 products
    <!-- end id27 -->
    <!-- npu="910b" id29 -->
    - Atlas A2 products
    <!-- end id29 -->
  <!-- end id26 -->
  <!-- npu="950" id30 -->
  - For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id30 -->
<!-- end id20 -->
<!-- npu="950" id21 -->
- Notes on Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - High-dimensional split computation of the tensor occupies 8 KB of UB.
  - Continuous computation of the first n data elements of the tensor does not involve the 8 KB UB usage.
<!-- end id21 -->

- If the value in src is non-positive, the result may be undefined.
- When using Rsqrt, the comparison error of the half operator result does not meet the requirement of two parts per thousand, and the comparison error of the float operator result does not meet the requirement of two parts per ten thousand. If high precision is required, use [Div](Div.md) and [Sqrt](Sqrt.md) instead.
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Rsqrt_res.md#id4 -->

<!-- npu="950" id35 -->

## Key Features<a id="rsqrt-key-features"></a>

For Ascend 950PR&950DT products, the key features are as follows:

### Maximum Precision Error

- The maximum precision error of RsqrtAlgo::INTRINSIC and RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE is 1 ulp.
- The maximum precision error of RsqrtAlgo::FAST_INVERSE and RsqrtAlgo::PRECISION_0ULP_FTZ_FALSE is 0 ulp.
- The maximum precision error of RsqrtAlgo::PRECISION_1ULP_FTZ_FALSE is 1 ulp.

### Configuring Subnormal Mode

FTZ (Flush-To-Zero): A floating-point computation mode in which, when the result is [Subnormal](../../data_structures/builtin_data_types.md#p7381131713310), it is directly flushed to zero (approximated as 0) instead of preserving its precise tiny value.

#### Default Algorithm

RsqrtAlgo::INTRINSIC is the default algorithm. When --cce-ftz=false, Subnormal values are preserved; when --cce-ftz=true (the default value), FTZ mode is used.

#### Explicitly Specified Algorithms

- RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE uses single-instruction computation and always adopts the FTZ mode.
- RsqrtAlgo::FAST_INVERSE and RsqrtAlgo::PRECISION_0ULP_FTZ_FALSE use the fast inverse algorithm, support only the float type, and support Subnormal data computation.
- RsqrtAlgo::PRECISION_1ULP_FTZ_FALSE supports Subnormal data computation for the half type.

#### Usage Recommendations

When --cce-ftz=true (the default value), it is recommended to use the default RsqrtAlgo::INTRINSIC or explicitly select RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE in general scenarios to achieve better performance. When precise Subnormal output is required, use RsqrtAlgo::FAST_INVERSE, RsqrtAlgo::PRECISION_0ULP_FTZ_FALSE, or RsqrtAlgo::PRECISION_1ULP_FTZ_FALSE.
<!-- end id35 -->

## Example<a name="section642mcpsimp"></a>

- Tensor high-dimensional split calculation example - mask continuous mode

    ```cpp
    uint64_t mask = 256 / sizeof(half);
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Rsqrt(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Tensor high-dimensional split calculation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Rsqrt(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- example of the API for computing the first n data of a tensor

    ```cpp
    AscendC::Rsqrt(dstLocal, srcLocal, 512);
    ```

    <!-- npu="950" id23 -->
    The following examples are supported only by the Ascend 950PR&950DT products

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
