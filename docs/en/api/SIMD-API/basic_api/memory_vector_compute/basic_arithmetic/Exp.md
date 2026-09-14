# Exp<a name="ZH-CN_TOPIC_0000001530181537"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T10:38:29.709Z -->

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

Exp is a unary vector calculation API that computes the natural exponential of each element of the input tensor. The calculation formula is as follows:

$dst_i = e^{src_i}$

## Prototype<a name="section620mcpsimp"></a>

- Prototype with config

    - Continuous calculation of the first n data elements of the tensor

        ```cpp
        template <typename T, const ExpConfig& config = DEFAULT_EXP_CONFIG>
        __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - High-dimensional splitting calculation of the tensor
        - Bitwise mask mode

            ```cpp
            template <typename T, bool isSetMask = true, const ExpConfig& config = DEFAULT_EXP_CONFIG>
            __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - Continuous mask mode

            ```cpp
            template <typename T, bool isSetMask = true, const ExpConfig& config = DEFAULT_EXP_CONFIG>
            __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

- Prototype without config

    - Continuous calculation of the first n data elements of the tensor

        ```cpp
        template <typename T>
        __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - High-dimensional splitting calculation of the tensor
        - Bitwise mask mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - Continuous mask mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

## Parameters<a name="section176711403104"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |
| <!-- npu="950" id19 -->config | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br>It is used to configure the Subnormal calculation mode. The type is ExpConfig, which is defined as follows:<br>enum&nbsp;class&nbsp;ExpAlgo&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;INTRINSIC&nbsp;=&nbsp;0,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct&nbsp;ExpConfig&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;ExpAlgo&nbsp;algo&nbsp;=&nbsp;ExpAlgo::INTRINSIC;<br>};<br>The Subnormal calculation mode is configured through the algo parameter of the ExpConfig structure. The values of algo are as follows:<br>&bull; ExpAlgo::INTRINSIC and ExpAlgo::PRECISION_1ULP_FTZ_TRUE: The result is obtained through a single instruction, and all Subnormal values are approximated to 0.<br>&bull; ExpAlgo::PRECISION_1ULP_FTZ_FALSE: Subnormal data calculation is supported.<br>The default value DEFAULT_EXP_CONFIG of this parameter is as follows:<br>constexpr&nbsp;ExpConfig&nbsp;DEFAULT_EXP_CONFIG&nbsp;=&nbsp;{&nbsp;ExpAlgo::INTRINSIC&nbsp;};<br>If the config parameter is not explicitly passed when this prototype is called, DEFAULT_EXP_CONFIG is used by default. In this case, the behavior is equivalent to that of the prototype without the config parameter.<!-- end id19 --> |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the calculation. For details about this parameter, see [Continuous Calculation](../SIMD_compute/continuous_computation.md). |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector calculation unit reads 256 bytes of consecutive data for calculation each time. To complete the processing of the input data, multiple iterations (repeats) are required to read and calculate all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Splitting](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

The data types supported by T are half and float.

## Return Value<a name="section14483414194"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="A3,910b,950" id20 -->
- When the value of the count or repeatTime parameter is 0, the behavior of this API is as follows:
  <!-- npu="A3,910b" id26 -->
  - For the following models, when the value of the count or repeatTime parameter is 0, no calculation is performed, no write is made to the destination operand, and this API is treated as a NOP (no operation).
    <!-- npu="A3" id27 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id27 -->
    <!-- npu="910b" id29 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id29 -->
  <!-- end id26 -->
  <!-- npu="950" id30 -->
  - For Ascend 950PR/Ascend 950DT: This API invokes the [Reg Vector Calculation API](../../reg_vector_compute/reg_vector_computation.md) through VF for compatibility. When the value of the count or repeatTime parameter is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id30 -->
<!-- end id20 -->
<!-- npu="950" id21 -->
- Description of UB space usage. For Ascend 950PR/Ascend 950DT:
  - Tensor high-dimensional splitting calculation occupies 8 KB of Unified Buffer.
  - Continuous calculation of the first n data of a tensor does not involve the occupation of the 8 KB Unified Buffer.
<!-- end id21 -->

<!-- npu="950" id24 -->
## Key Features<a name="section18972943153217"></a>

For Ascend 950PR/Ascend 950DT, the key features are as follows:

**Maximum precision error**:

- ExpAlgo::INTRINSIC, ExpAlgo::PRECISION\_1ULP\_FTZ\_TRUE, with a maximum precision error of 1 ulp.
- ExpAlgo::PRECISION\_1ULP\_FTZ\_FALSE, implemented by software simulation, with a maximum precision error of 1 ulp.

**Configuring the Subnormal mode**:

FTZ (Flush To Zero): a floating-point operation mode in which, when the result is [Subnormal](../../data_structures/built_in_data_type.md#p7381131713310), it is directly cleared to zero (approximated as 0) instead of retaining its precise tiny value.

Only when algo is set to ExpAlgo::PRECISION\_1ULP\_FTZ\_false does the Exp API retain and correctly output Subnormal results; in other modes, Subnormal values are all flushed to zero (FTZ).

Because the calculation behavior of Subnormal is implemented through a software simulation algorithm, it is recommended to use ExpAlgo::INTRINSIC and ExpAlgo::PRECISION\_1ULP\_FTZ\_TRUE, which offer better performance, in general scenarios; use ExpAlgo::PRECISION\_1ULP\_FTZ\_FALSE in scenarios that require precise Subnormal output (such as algorithms with specific data precision requirements or avoiding division-by-zero errors).
<!-- end id24 -->

## Example<a name="section176061616102911"></a>

- Example of high-dimensional splitting calculation for tensor - mask continuous mode

    ```cpp
    uint64_t mask = 256 / sizeof(half);
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Exp(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of high-dimensional splitting calculation for tensor - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Exp(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of the API for calculating the first n data of a tensor

    ```cpp
    AscendC::Exp(dstLocal, srcLocal, 512);
    ```

    <!-- npu="950" id25 -->
    The following examples are supported only on Ascend 950PR/Ascend 950DT

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
