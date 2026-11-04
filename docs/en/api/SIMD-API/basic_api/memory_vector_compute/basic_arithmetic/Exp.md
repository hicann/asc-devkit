# Exp<a name="ZH-CN_TOPIC_0000001530181537"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:12:13.985Z pushedAt=2026-10-28T03:14:39.386Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Exp_res.md#id1 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Exp_res.md#id2 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_unary_intf.h"`.

Exp is a unary vector computation API that is responsible for taking the natural exponential of the input tensor element-wise. The formula is as follows:

$dst_i = e^{src_i}$

## Prototype<a name="section620mcpsimp"></a>

- Prototype with config

    - Continuous computation of the first n data elements of the tensor

        ```cpp
        template <typename T, const ExpConfig& config = DEFAULT_EXP_CONFIG>
        __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - Tensor high-dimensional split computation
        - **mask** bit-by-bit mode

            ```cpp
            template <typename T, bool isSetMask = true, const ExpConfig& config = DEFAULT_EXP_CONFIG>
            __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - **mask** continuous mode

            ```cpp
            template <typename T, bool isSetMask = true, const ExpConfig& config = DEFAULT_EXP_CONFIG>
            __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

- Prototype without config

    - Continuous computation of the first n data elements of the tensor

        ```cpp
        template <typename T>
        __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - Tensor high-dimensional split computation
        - **mask** bit-by-bit mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - **mask** continuous mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

## Parameters<a name="section176711403104"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use [SetVectorMask](../mask_operations/SetVectorMask.md) to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |
| <!-- npu="950" id19 -->config | This parameter is supported only on Ascend 950PR&950DT products.<br>Configures the precision computation mode. The type is ExpConfig, defined as follows:<br>enum&nbsp;class&nbsp;ExpAlgo&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;INTRINSIC&nbsp;=&nbsp;0,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct&nbsp;ExpConfig&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;ExpAlgo&nbsp;algo&nbsp;=&nbsp;ExpAlgo::INTRINSIC;<br>};<br>Selects the Exp algorithm and configures the Subnormal mode through the algo parameter of the ExpConfig struct. For details, see [Key Features](#exp-key-features).<br>The values of algo are as follows:<br>&bull; ExpAlgo::INTRINSIC: Default algorithm, with a maximum precision error of 1 ulp. For half and float types, Subnormal handling is controlled by the --cce-ftz compilation option (default value: true).<br>&bull; ExpAlgo::PRECISION_1ULP_FTZ_TRUE: Uses a single instruction for computation, with a maximum precision error of 1 ulp.<br>&bull; ExpAlgo::PRECISION_1ULP_FTZ_FALSE: Implemented through software emulation, supports Subnormal data computation, with a maximum precision error of 1 ulp.<br><br>The default value DEFAULT_EXP_CONFIG of this parameter is as follows:<br>constexpr&nbsp;ExpConfig&nbsp;DEFAULT_EXP_CONFIG&nbsp;=&nbsp;{&nbsp;ExpAlgo::INTRINSIC&nbsp;};<br>If the config parameter is not explicitly passed when calling this prototype, DEFAULT_EXP_CONFIG is used by default, and the behavior is equivalent to that of the prototype without the config parameter.<!-- end id19 --> |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Compute](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements involved in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To process all input data, multiple iterations (repeats) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. Type: [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which contains the address stride of the same DataBlock between adjacent iterations of the operands, the address stride of different DataBlocks within the same iteration, and other parameters.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

Supported data types: half, float.

## Return Value<a name="section14483414194"></a>

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
  - For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, the simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id30 -->
<!-- end id20 -->
<!-- npu="950" id21 -->
- Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - High-dimensional slicing computation of the tensor occupies 8 KB of UB.
  - Continuous computation of the first n data elements of the tensor does not involve the 8 KB UB usage.
<!-- end id21 -->

<!-- npu="950" id24 -->
## Key Features<a id="exp-key-features"></a>

The following key features apply to the Ascend 950PR&950DT products:

### Maximum Precision Error

The maximum precision error of **ExpAlgo::INTRINSIC**, **ExpAlgo::PRECISION_1ULP_FTZ_TRUE**, and **ExpAlgo::PRECISION_1ULP_FTZ_FALSE** is 1 ulp.

### Configuring Subnormal Mode

FTZ (Flush-To-Zero): A floating-point operation mode in which a [Subnormal](../../data_structures/builtin_data_types.md#p7381131713310) result is directly flushed to zero (approximated as 0) instead of retaining its exact tiny value.

#### Default Algorithm

**ExpAlgo::INTRINSIC** is the default algorithm. When **--cce-ftz=false**, Subnormal values are preserved; when **--cce-ftz=true** (the default value), FTZ mode is used.

#### Explicitly Specified Algorithms

- **ExpAlgo::PRECISION_1ULP_FTZ_TRUE** uses a single instruction for computation and always adopts the FTZ mode.
- **ExpAlgo::PRECISION_1ULP_FTZ_FALSE** is implemented through software simulation and supports Subnormal data computation.

#### Usage Recommendations

Because the computation behavior that preserves Subnormal is implemented through software simulation, when **--cce-ftz=true** (the default value) is used, it is recommended to use the default **ExpAlgo::INTRINSIC** or explicitly select **ExpAlgo::PRECISION_1ULP_FTZ_TRUE** in general scenarios to achieve better performance. When precise Subnormal output is required, use **ExpAlgo::PRECISION_1ULP_FTZ_FALSE**.
<!-- end id24 -->

## Examples<a name="section176061616102911"></a>

- Example of high-dimensional slicing computation on tensor - mask continuous mode

    ```cpp
    uint64_t mask = 256 / sizeof(half);
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Exp(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of high-dimensional slicing computation on tensor - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Exp(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of the API for computing the first n data elements of a tensor

    ```cpp
    AscendC::Exp(dstLocal, srcLocal, 512);
    ```

    <!-- npu="950" id25 -->
    The following examples are supported only by the Ascend 950PR&950DT products.

    ```cpp
    static constexpr ExpConfig config = { ExpAlgo::PRECISION_1ULP_FTZ_FALSE };
    AscendC::Exp<T, config>(dstLocal, srcLocal, 512);
    ```
    <!-- end id25 -->

The result is as follows:

```bash
Input data srcLocal: [0.0 1.0 2.0 3.0 ...]
Output data dstLocal: [1.0 2.719 7.391 20.08 ...]
```
