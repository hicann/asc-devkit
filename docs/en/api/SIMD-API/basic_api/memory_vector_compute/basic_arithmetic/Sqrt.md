# Sqrt<a name="ZH-CN_TOPIC_0000001379163546"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:42:46.650Z pushedAt=2026-10-28T03:14:39.426Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Sqrt_res.md#id1 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Sqrt_res.md#id2 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_unary_intf.h"`.

Sqrt is a unary vector computation API that is responsible for performing the square root operation on the input tensor element-wise. The calculation formula is as follows:

$dst_i = \sqrt{src_i}$

## Prototype<a name="section620mcpsimp"></a>

- Prototype with config

    - Continuous computation of the first n data elements of the tensor

        ```cpp
        template <typename T, const SqrtConfig& config = DEFAULT_SQRT_CONFIG>
        __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - Tensor high-dimensional slicing computation
        - **mask** bit-by-bit mode

            ```cpp
            template <typename T, bool isSetMask = true, const SqrtConfig& config = DEFAULT_SQRT_CONFIG>
            __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - **mask** continuous mode

            ```cpp
            template <typename T, bool isSetMask = true, const SqrtConfig& config = DEFAULT_SQRT_CONFIG>
            __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

- Prototype without config

    - Continuous computation of the first n data elements of the tensor

        ```cpp
        template <typename T>
        __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - Tensor high-dimensional slicing computation
        - **mask** bit-by-bit mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - **mask** continuous mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |
| <!-- npu="950" id22 -->config | This parameter is supported only on Ascend 950PR&950DT products.<br>Used to configure the precision computation mode. It is of the SqrtConfig type, defined as follows:<br>enum&nbsp;class&nbsp;SqrtAlgo&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;INTRINSIC&nbsp;=&nbsp;0,<br>&nbsp;&nbsp;&nbsp;&nbsp;FAST_INVERSE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_0ULP_FTZ_FALSE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct&nbsp;SqrtConfig&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;SqrtAlgo&nbsp;algo&nbsp;=&nbsp;SqrtAlgo::INTRINSIC;<br>};<br>The algo parameter of the SqrtConfig struct selects the Sqrt algorithm and configures the Subnormal mode. For details, see [Key Features](#sqrt-key-features).<br>The values of algo are as follows:<br>&bull; SqrtAlgo::INTRINSIC: Default algorithm, with a maximum precision error of 1 ulp. For the half and float types, Subnormal handling is controlled by the --cce-ftz compilation option (default value: true).<br>&bull; SqrtAlgo::FAST_INVERSE and SqrtAlgo::PRECISION_0ULP_FTZ_FALSE: Use the fast inversion algorithm to obtain the result. Currently, this algorithm supports only the float data type and supports Subnormal data computation in this mode.<br>&bull; SqrtAlgo::PRECISION_1ULP_FTZ_TRUE: Uses single-instruction computation, with a maximum precision error of 1 ulp.<br>&bull; SqrtAlgo::PRECISION_1ULP_FTZ_FALSE: Maximum precision error is 1 ulp, and supports Subnormal data computation for the half type.<br><br>The default value of this parameter, DEFAULT_SQRT_CONFIG, is as follows:<br>constexpr&nbsp;SqrtConfig&nbsp;DEFAULT_SQRT_CONFIG&nbsp;=&nbsp;{&nbsp;SqrtAlgo::INTRINSIC&nbsp;};<br>If the config parameter is not explicitly passed when calling this prototype, DEFAULT_SQRT_CONFIG is used by default, and the behavior is equivalent to the prototype without the config parameter.<!-- end id22 --> |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements involved in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of contiguous data for computation each time. To complete processing of the input data, multiple iterations (repeats) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the operand address stride. Type: [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), including the address stride of the same DataBlock between adjacent iterations of the operand, the address stride of different DataBlocks within the same iteration of the operand, and other parameters.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Type

Supported data types: half, float.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="A3,910b,950" id20 -->
- When the value of count or repeatTime is 0, this API behaves as follows:
  <!-- npu="A3,910b" id26 -->
  - For the following models, when the value of count or repeatTime is 0, no computation is performed, no data is written to the destination operand, and this API is treated as a NOP (no operation).
    <!-- npu="A3" id27 -->
    - Atlas A3 products
    <!-- end id27 -->
    <!-- npu="910b" id29 -->
    - Atlas A2 products
    <!-- end id29 -->
  <!-- end id26 -->
  <!-- npu="950" id30 -->
  - For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg Vector Compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the value of count or repeatTime is 0, the simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id30 -->
<!-- end id20 -->
<!-- npu="950" id21 -->
- Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - Tensor high-dimensional slicing computation occupies 8 KB of UB.
  - Continuous computation of the first n tensor data does not involve the 8 KB UB usage.
- For Ascend 950PR&950DT products, SqrtAlgo::FAST\_INVERSE, SqrtAlgo::PRECISION\_0ULP\_FTZ\_FALSE, the result is obtained using the fast inversion algorithm. It applies to computation where the input value is within the range of \[0, 85070596800837026223494223584045301760\]. Within this range, the algorithm guarantees that the maximum precision error is 0 ulp. When the input value is greater than 85070596800837026223494223584045301760, the output is 0. Currently, this algorithm supports only the float data type and supports Subnormal data computation in this mode.
<!-- end id21 -->

- If the value in src is non-positive, the result may be undefined.

<!-- npu="950" id35 -->
## Key Features<a id="sqrt-key-features"></a>

For Ascend 950PR&950DT products, the key features are as follows:

### Maximum Precision Error

- The maximum precision error of SqrtAlgo::INTRINSIC and SqrtAlgo::PRECISION_1ULP_FTZ_TRUE is 1 ulp.
- The maximum precision error of SqrtAlgo::FAST_INVERSE and SqrtAlgo::PRECISION_0ULP_FTZ_FALSE is 0 ulp.
- The maximum precision error of SqrtAlgo::PRECISION_1ULP_FTZ_FALSE is 1 ulp.

### Configuring Subnormal Mode

FTZ (Flush-To-Zero): A floating-point computation mode in which, when the result is [Subnormal](../../data_structures/builtin_data_types.md#p7381131713310), it is directly flushed to zero (approximated as 0) instead of preserving its precise tiny value.

#### Default Algorithm

SqrtAlgo::INTRINSIC is the default algorithm. When --cce-ftz=false, Subnormal values are preserved; when --cce-ftz=true (the default value), FTZ mode is used.

#### Explicitly Specified Algorithms

- **SqrtAlgo::PRECISION_1ULP_FTZ_TRUE** uses single-instruction computation and always adopts the FTZ mode.
- **SqrtAlgo::FAST_INVERSE** and **SqrtAlgo::PRECISION_0ULP_FTZ_FALSE** use the fast inverse algorithm, support only the float type, and support Subnormal data computation.
- **SqrtAlgo::PRECISION_1ULP_FTZ_FALSE** supports Subnormal data computation for the half type.

#### Usage Recommendations

Because the computation behavior that preserves Subnormal is implemented through software simulation, when --cce-ftz=true (the default value), it is recommended to use the default SqrtAlgo::INTRINSIC or explicitly select SqrtAlgo::PRECISION_1ULP_FTZ_TRUE in general scenarios to achieve better performance. When precise Subnormal output is required, use SqrtAlgo::FAST_INVERSE, SqrtAlgo::PRECISION_0ULP_FTZ_FALSE, or SqrtAlgo::PRECISION_1ULP_FTZ_FALSE.
<!-- end id35 -->

## Examples<a name="section176061616102911"></a>

- tensor high-dimensional slicing computation example - mask continuous mode

    ```cpp
    uint64_t mask = 256 / sizeof(half);
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Sqrt(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- tensor high-dimensional slicing computation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Sqrt(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of the API for computing the first n data elements of a tensor

    ```cpp
    AscendC::Sqrt(dstLocal, srcLocal, 512);
    ```

    <!-- npu="950" id25 -->
    The following examples are supported only by the Ascend 950PR&950DT products

    ```cpp
    static constexpr SqrtConfig config = { SqrtAlgo::PRECISION_1ULP_FTZ_FALSE };
    AscendC::Sqrt<T, config>(dstLocal, srcLocal, 512);
    ```
    <!-- end id25 -->

The result is as follows:

```bash
Input data srcLocal: [1.0 2.0 3.0 4.0 ...]
Output data dstLocal: [1.0 1.414 1.732 2.0 ...]
```
