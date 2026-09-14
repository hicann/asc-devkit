# Sqrt<a name="ZH-CN_TOPIC_0000001379163546"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T11:44:45.180Z -->

## Applicable Products<a name="section1550532418810"></a>

### Prototype without passing config

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

Sqrt is a unary vector computation API that computes the square root of the input tensor element by element. The calculation formula is as follows:

$dst_i = \sqrt{src_i}$

## Prototype<a name="section620mcpsimp"></a>

- Prototype with config

    - Continuous computation of the first n data elements of the tensor

        ```cpp
        template <typename T, const SqrtConfig& config = DEFAULT_SQRT_CONFIG>
        __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - High-dimensional slicing computation of the tensor
        - Bitwise mask mode

            ```cpp
            template <typename T, bool isSetMask = true, const SqrtConfig& config = DEFAULT_SQRT_CONFIG>
            __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - Continuous mask mode

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

    - High-dimensional slicing computation of the tensor
        - Bitwise mask mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - Continuous mask mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |
| <!-- npu="950" id19 -->config | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br>It is used to configure the precision computation mode. The type is SqrtConfig, which is defined as follows:<br>enum&nbsp;class&nbsp;SqrtAlgo&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;INTRINSIC&nbsp;=&nbsp;0,<br>&nbsp;&nbsp;&nbsp;&nbsp;FAST_INVERSE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_0ULP_FTZ_FALSE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct&nbsp;SqrtConfig&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;SqrtAlgo&nbsp;algo&nbsp;=&nbsp;SqrtAlgo::INTRINSIC;<br>};<br>The precision computation mode is configured through the algo parameter of the SqrtConfig structure. The values of algo are as follows:<br>&bull; SqrtAlgo::INTRINSIC and SqrtAlgo::PRECISION_1ULP_FTZ_TRUE: the result is computed using a single instruction, and the maximum precision error is 1 ulp.<br>&bull; SqrtAlgo::FAST_INVERSE and SqrtAlgo::PRECISION_0ULP_FTZ_FALSE: the result is computed using the fast inverse algorithm. This applies to computations where the input value is in the range [0, 85070596800837026223494223584045301760]. Within this range, the algorithm guarantees that the maximum precision error of the output is 0 ulp. When the input value is greater than 85070596800837026223494223584045301760, the output is 0. Currently, this algorithm supports only the float data type, and supports Subnormal data computation in this mode.<br>&bull; SqrtAlgo::PRECISION_1ULP_FTZ_FALSE: supports only Subnormal data computation of the half type, in which case the maximum precision error is 1 ulp.<br>The default value DEFAULT_SQRT_CONFIG of this parameter is as follows:<br>constexpr&nbsp;SqrtConfig&nbsp;DEFAULT_SQRT_CONFIG&nbsp;=&nbsp;{&nbsp;SqrtAlgo::INTRINSIC&nbsp;};<br>If the config parameter is not explicitly passed when calling this prototype, DEFAULT_SQRT_CONFIG is used by default, in which case the behavior is equivalent to that of the prototype without the config parameter.<!-- end id19 --> |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md). |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeats) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-dimensional Slicing](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

The supported data types of T are half and float.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="A3,910b,950" id20 -->
- When the value of the parameter count or repeatTime is 0, the behavior of this API is as follows:
  <!-- npu="A3,910b" id26 -->
  - For the following models, when the value of the parameter count or repeatTime is 0, no computation is performed, no write is made to the destination operand, and this API is treated as a NOP (no operation).
    <!-- npu="A3" id27 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id27 -->
    <!-- npu="910b" id29 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id29 -->
  <!-- end id26 -->
  <!-- npu="950" id30 -->
  - For Ascend 950PR/Ascend 950DT: This API is implemented for compatibility by calling the [Reg Vector Computation API](../../reg_vector_compute/reg_vector_computation.md) through VF. When the value of the parameter count or repeatTime is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id30 -->
<!-- end id20 -->
<!-- npu="950" id21 -->
- Description of Unified Buffer space usage. For Ascend 950PR/Ascend 950DT:
  - Tensor high-dimensional slicing computation occupies 8 KB of the Unified Buffer.
  - Continuous computation of the first n data of a tensor does not involve occupation of the 8 KB Unified Buffer.
- For Ascend 950PR/Ascend 950DT, SqrtAlgo::FAST\_INVERSE, SqrtAlgo::PRECISION\_0ULP\_FTZ\_FALSE, the result is obtained using the fast inverse algorithm. It applies to computation where the input value is within the range of \[0, 85070596800837026223494223584045301760\]. Within this range, the algorithm guarantees that the maximum precision error of the output is 0 ulp. When the input value is greater than 85070596800837026223494223584045301760, the output is 0. Currently, this algorithm supports only the float data type, and supports Subnormal data computation in this mode.
<!-- end id21 -->

- If the value in src is non-positive, an unknown result may be produced.

<!-- npu="950" id24 -->
## Key Features<a name="section18972943153217"></a>

For Ascend 950PR/Ascend 950DT, the key features are as follows:

**Maximum precision error**:

- SqrtAlgo::INTRINSIC, SqrtAlgo::PRECISION\_1ULP\_FTZ\_TRUE, the maximum precision error is 1 ulp.
- SqrtAlgo::PRECISION\_1ULP\_FTZ\_FALSE, implemented by software simulation, the maximum precision error is 1 ulp.
- SqrtAlgo::FAST\_INVERSE, SqrtAlgo::PRECISION\_0ULP\_FTZ\_FALSE, implemented by software simulation, the maximum precision error is 0 ulp.

**Configuring Subnormal mode**:

FTZ (Flush To Zero): a floating-point operation mode. When the result is [Subnormal](../../data_structures/built_in_data_type.md#p7381131713310), it is directly cleared to zero (approximated as 0) instead of retaining its precise tiny value.

Only when algo is set to SqrtAlgo::FAST\_INVERSE, SqrtAlgo::PRECISION\_0ULP\_FTZ\_FALSE, or SqrtAlgo::PRECISION\_1ULP\_FTZ\_false does the Sqrt API retain and correctly output Subnormal results; in other modes, Subnormal values are all flushed to zero (FTZ).

Because the computation behavior of Subnormal is implemented through a software simulation algorithm, in general scenarios it is recommended to use SqrtAlgo::INTRINSIC and SqrtAlgo::PRECISION\_1ULP\_FTZ\_TRUE, which offer better performance; for scenarios that require precise Subnormal output (such as algorithms with specific data precision requirements or avoiding division-by-zero errors), use SqrtAlgo::FAST\_INVERSE, SqrtAlgo::PRECISION\_0ULP\_FTZ\_FALSE, or SqrtAlgo::PRECISION\_1ULP\_FTZ\_FALSE.
<!-- end id24 -->

## Example<a name="section176061616102911"></a>

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

- tensor first n data computation API example

    ```cpp
    AscendC::Sqrt(dstLocal, srcLocal, 512);
    ```

    <!-- npu="950" id23 -->
    The following examples are supported only on Ascend 950PR/Ascend 950DT

    ```cpp
    static constexpr SqrtConfig config = { SqrtAlgo::PRECISION_1ULP_FTZ_FALSE };
    AscendC::Sqrt<T, config>(dstLocal, srcLocal, 512);
    ```
    <!-- end id23 -->

The result is as follows:

```bash
Input data srcLocal: [1.0 2.0 3.0 4.0 ...]
Output data dstLocal: [1.0 1.414 1.732 2.0 ...]
```
