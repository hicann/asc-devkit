# Reciprocal<a name="ZH-CN_TOPIC_0000001429443329"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:39:14.727Z pushedAt=2026-10-28T03:14:39.423Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

### Prototype Without config

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Reciprocal_res.md#id1 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Reciprocal_res.md#id2 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_unary_intf.h"`.

Reciprocal is a unary vector computation API that is responsible for taking the reciprocal of the input tensor element-wise. The calculation formula is as follows:

$dst_i = \frac{1}{src_i}$

## Prototype<a name="section620mcpsimp"></a>

- Prototype with config

    - Continuous computation of the first n tensor data elements

        ```cpp
        template <typename T, const ReciprocalConfig& config = DEFAULT_RECIPROCAL_CONFIG>
        __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - Tensor High-Dimensional Split Computation
        - **mask** bit-by-bit mode

            ```cpp
            template <typename T, bool isSetMask = true, const ReciprocalConfig& config = DEFAULT_RECIPROCAL_CONFIG>
            __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - **mask** continuous mode

            ```cpp
            template <typename T, bool isSetMask = true, const ReciprocalConfig& config = DEFAULT_RECIPROCAL_CONFIG>
            __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

- Prototype without config

    - Continuous computation of the first n tensor data elements

        ```cpp
        template <typename T>
        __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - Tensor High-Dimensional Split Computation
        - **mask** bit-by-bit mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - **mask** continuous mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask within the API.<br>&bull; true: The mask is set within the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |
| <!-- npu="950" id19 -->config | This parameter is supported only on Ascend 950PR&950DT products.<br>Used to configure the precision computation mode. It is of the ReciprocalConfig type, defined as follows:<br>enum&nbsp;class&nbsp;ReciprocalAlgo&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;INTRINSIC&nbsp;=&nbsp;0,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct&nbsp;ReciprocalConfig&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;ReciprocalAlgo&nbsp;algo&nbsp;=&nbsp;ReciprocalAlgo::INTRINSIC;<br>};<br>The algo parameter of the ReciprocalConfig struct selects the Reciprocal algorithm and configures the Subnormal mode. For details, see [Key Features](#reciprocal-key-features).<br>The values of algo are as follows:<br>&bull; ReciprocalAlgo::INTRINSIC: Default algorithm, with a maximum precision error of 1 ulp. For half and float types, Subnormal handling is controlled by the --cce-ftz compilation option (default value: true).<br>&bull; ReciprocalAlgo::PRECISION_1ULP_FTZ_TRUE: Computed using a single instruction, with a maximum precision error of 1 ulp.<br>&bull; ReciprocalAlgo::PRECISION_1ULP_FTZ_FALSE: Implemented through software emulation, supporting Subnormal data computation, with a maximum precision error of 1 ulp.<br><br>The default value DEFAULT_RECIPROCAL_CONFIG of this parameter is as follows:<br>constexpr&nbsp;ReciprocalConfig&nbsp;DEFAULT_RECIPROCAL_CONFIG&nbsp;=&nbsp;{&nbsp;ReciprocalAlgo::INTRINSIC&nbsp;};<br>If config is not explicitly passed when calling this prototype, DEFAULT_RECIPROCAL_CONFIG is used by default, and the behavior is equivalent to the prototype without the config parameter.<!-- end id19 --> |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition values: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition values: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Compute](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements involved in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of contiguous data each time for computation. To process the input data, multiple iterations (repeats) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Partitioning](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameter that controls the address stride of the operand. It is of the [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md) type, containing parameters such as the address stride of the same DataBlock between adjacent iterations of the operand and the address stride of different DataBlocks within the same iteration of the operand.<br>For details about the address stride parameter between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride parameter of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

<!-- npu="950" id20 -->
- For Ascend 950PR&950DT products, T supports the following data types: half, float, int64_t, and uint64_t. The int64_t and uint64_t data types are supported only by the API that computes the first n data of a tensor.
<!-- end id20 -->
<!-- npu="A3" id21 -->
- For Atlas A3 products, T supports the following data types: half and float.
<!-- end id21 -->
<!-- npu="910b" id22 -->
- For Atlas A2 products, T supports the following data types: half and float.
<!-- end id22 -->
<!-- npu="310b" id23 -->
- For Atlas 200I/500 A2 inference product, T supports the following data types: half and float.
<!-- end id23 -->
<!-- npu="310p" id24 -->
- For Atlas inference products (AI Core), T supports the following data types: half and float.
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
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id29 -->
  - For the following models, when the count or repeatTime parameter is set to 0, no computation is performed and no write is made to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id30 -->
    - Atlas A3 products
    <!-- end id30 -->
    <!-- npu="910b" id31 -->
    - Atlas A2 products
    <!-- end id31 -->
  <!-- end id29 -->
  <!-- npu="950" id33 -->
  - For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg Vector Compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id33 -->
<!-- end id28 -->
<!-- npu="950" id34 -->
- Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - Tensor high-dimensional partitioning computation occupies 8 KB of UB.
  - Continuous computation of the first n tensor data does not involve the 8 KB UB usage.
<!-- end id34 -->

- If the value in src is 0, the result may be undefined.
- When using Reciprocal, the comparison error of the half operator result does not meet the requirement of two parts per thousand, and the comparison error of the float operator result does not meet the requirement of two parts per ten thousand. If higher precision is required, use [Div](Div.md) instead.

<!-- npu="950" id35 -->
## Key Features<a id="reciprocal-key-features"></a>

For Ascend 950PR&950DT products, the key features are as follows:

### Maximum Precision Error

The maximum precision error of ReciprocalAlgo::INTRINSIC, ReciprocalAlgo::PRECISION_1ULP_FTZ_TRUE, and ReciprocalAlgo::PRECISION_1ULP_FTZ_FALSE is 1 ulp.

### Configuring Subnormal Mode

FTZ (Flush-To-Zero): A floating-point computation mode in which, when the result is [Subnormal](../../data_structures/builtin_data_types.md#p7381131713310), it is directly flushed to zero (approximated as 0) instead of preserving its precise tiny value.

#### Default Algorithm

ReciprocalAlgo::INTRINSIC is the default algorithm. When --cce-ftz=false, Subnormal values are preserved; when --cce-ftz=true (the default value), FTZ mode is used.

#### Explicitly Specified Algorithms

- ReciprocalAlgo::PRECISION_1ULP_FTZ_TRUE uses single-instruction computation and always adopts the FTZ mode.
- ReciprocalAlgo::PRECISION_1ULP_FTZ_FALSE is implemented through software simulation and supports Subnormal data computation.

#### Usage Recommendations

Because the computation behavior that preserves Subnormal is implemented through software simulation, when --cce-ftz=true (the default value), it is recommended to use the default ReciprocalAlgo::INTRINSIC or explicitly select ReciprocalAlgo::PRECISION_1ULP_FTZ_TRUE in general scenarios to achieve better performance; when precise Subnormal output is required, use ReciprocalAlgo::PRECISION_1ULP_FTZ_FALSE.
<!-- end id35 -->

## Examples<a name="section176061616102911"></a>

- Example of high-dimensional partitioning computation for tensor - mask continuous mode

    ```cpp
    uint64_t mask = 256 / sizeof(half);
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Reciprocal(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of high-dimensional partitioning computation for tensor - mask bitwise mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Reciprocal(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of the API for computing the first n data elements of a tensor

    ```cpp
    AscendC::Reciprocal(dstLocal, srcLocal, 512);
    ```

    <!-- npu="950" id32 -->
    The following examples are supported only by the Ascend 950PR&950DT products

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
