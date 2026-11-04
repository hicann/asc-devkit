# Ln<a name="ZH-CN_TOPIC_0000001429283377"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:18:02.734Z pushedAt=2026-10-28T03:14:39.395Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

### Prototype Without Passing config

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Ln_res.md#id1 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Ln_res.md#id2 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_unary_intf.h"`.

Ln is a unary vector computation API that is responsible for taking the natural logarithm of the input tensor element-wise. The calculation formula is as follows:

$dst_i = \ln(src_i)$

## Prototype<a name="section620mcpsimp"></a>

- Prototype with config

    - Continuous computation of the first n data elements of the tensor

        ```cpp
        template <typename T, const LnConfig& config = DEFAULT_LN_CONFIG>
        __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - Tensor High-Dimensional Split Computation
        - **mask** bit-by-bit mode

            ```cpp
            template <typename T, bool isSetMask = true, const LnConfig& config = DEFAULT_LN_CONFIG>
            __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - **mask** continuous mode

            ```cpp
            template <typename T, bool isSetMask = true, const LnConfig& config = DEFAULT_LN_CONFIG>
            __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

- Prototype without config

    - Continuous computation of the first n data elements of the tensor

        ```cpp
        template <typename T>
        __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - Tensor High-Dimensional Split Computation
        - **mask** bit-by-bit mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - **mask** continuous mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use [SetVectorMask](../mask_operations/SetVectorMask.md) to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |
| <!-- npu="950" id19 -->config | This parameter is supported only on Ascend 950PR&950DT products.<br>Used to configure the precision computation mode. It is of the LnConfig type, defined as follows:<br>enum&nbsp;class&nbsp;LnAlgo&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;INTRINSIC&nbsp;=&nbsp;0,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct&nbsp;LnConfig&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;LnAlgo&nbsp;algo&nbsp;=&nbsp;LnAlgo::INTRINSIC;<br>};<br>The algo parameter of the LnConfig struct selects the Ln algorithm and configures the Subnormal mode. For details, see [Key Features](#ln-key-features).<br>The values of algo are as follows:<br>&bull; LnAlgo::INTRINSIC: Default algorithm, with a maximum precision error of 1 ulp. For half and float types, Subnormal handling is controlled by the --cce-ftz compilation option (default value: true).<br>&bull; LnAlgo::PRECISION_1ULP_FTZ_TRUE: Uses a single instruction for computation, with a maximum precision error of 1 ulp.<br>&bull; LnAlgo::PRECISION_1ULP_FTZ_FALSE: Implemented through software emulation, supports Subnormal data computation, with a maximum precision error of 1 ulp.<br><br>The default value DEFAULT_LN_CONFIG of this parameter is as follows:<br>constexpr&nbsp;LnConfig&nbsp;DEFAULT_LN_CONFIG&nbsp;=&nbsp;{&nbsp;LnAlgo::INTRINSIC&nbsp;};<br>If the config parameter is not explicitly passed when calling this prototype, DEFAULT_LN_CONFIG is used by default, in which case the behavior is equivalent to the prototype without the config parameter.<!-- end id19 --> |

**Table 2** Parameters

<a name="table1055216132132"></a>

| Parameter | Input/Output | Description |
|--------|-----------|------|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | The mask controls the elements involved in the computation within each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of contiguous data for computation each time. To process all input data, multiple iterations (repeats) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Partitioning](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operand. Type: [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes the address stride of the same DataBlock between adjacent iterations of the operand, the address stride of different DataBlocks within the same iteration, and other parameters.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

Supported data types for T: half, float.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="A3,910b,950" id20 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id26 -->
  - For the following models, when the count or repeatTime parameter is set to 0, no computation is performed and no write is made to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id27 -->
    - Atlas A3 products
    <!-- end id27 -->
    <!-- npu="910b" id29 -->
    - Atlas A2 products
    <!-- end id29 -->
  <!-- end id26 -->
  <!-- npu="950" id30 -->
  - For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id30 -->
<!-- end id20 -->
<!-- npu="950" id21 -->
- Notes on Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - High-dimensional partitioning computation of the tensor occupies 8 KB of UB.
  - Continuous computation of the first n data elements of the tensor does not involve the 8 KB UB usage.
<!-- end id21 -->

<!-- npu="950" id24 -->
## Key Features<a id="ln-key-features"></a>

For the Ascend 950PR&950DT products, the following key features apply:

### Maximum Precision Error

The maximum precision error of **LnAlgo::INTRINSIC**, **LnAlgo::PRECISION_1ULP_FTZ_TRUE**, and **LnAlgo::PRECISION_1ULP_FTZ_FALSE** is 1 ulp.

### Configuring Subnormal Mode

FTZ (Flush-To-Zero): A floating-point operation mode that, when the result is [Subnormal](../../data_structures/builtin_data_types.md#p7381131713310), flushes it to zero (approximates it to 0) instead of preserving its exact tiny value.

#### Default Algorithm

**LnAlgo::INTRINSIC** is the default algorithm. When **--cce-ftz=false**, Subnormal values are preserved; when **--cce-ftz=true** (the default value), FTZ mode is used.

#### Explicitly Specified Algorithms

- LnAlgo::PRECISION_1ULP_FTZ_TRUE uses a single instruction for computation and always adopts the FTZ mode.
- LnAlgo::PRECISION_1ULP_FTZ_FALSE is implemented through software simulation and supports Subnormal data computation.

#### Usage Recommendations

Because the computation behavior that preserves Subnormal is implemented through software simulation, when **--cce-ftz=true** (the default value), it is recommended to use the default **LnAlgo::INTRINSIC** or explicitly select **LnAlgo::PRECISION_1ULP_FTZ_TRUE** in general scenarios to achieve better performance. When Subnormal needs to be output accurately, use **LnAlgo::PRECISION_1ULP_FTZ_FALSE**.
<!-- end id24 -->

## Examples<a name="section176061616102911"></a>

- tensor high-dimensional partitioning computation example - mask continuous mode

    ```cpp
    uint64_t mask = 256 / sizeof(half);
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Ln(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- tensor high-dimensional partitioning computation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Ln(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- example of the API for computing the first n data elements of a tensor

    ```cpp
    AscendC::Ln(dstLocal, srcLocal, 512);
    ```

    <!-- npu="950" id25 -->
    The following example is supported only by the Ascend 950PR&950DT products.

    ```cpp
    static constexpr LnConfig config = { LnAlgo::PRECISION_1ULP_FTZ_FALSE };
    AscendC::Ln<T, config>(dstLocal, srcLocal, 512);
    ```
    <!-- end id25 -->

The result is as follows:

```bash
Input data srcLocal: [1 2 3 4 ...]
Output data dstLocal: [0 0.6931 1.0986 1.3863 ...]
```
