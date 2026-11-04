# Div<a name="ZH-CN_TOPIC_0000001429710845"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:11:24.955Z pushedAt=2026-10-28T03:14:39.361Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

### Prototype without config

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Div_res.md#id1 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Div_res.md#id2 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_intf.h"`.

Div is a binary vector computation API that computes the element-wise quotient of the two input tensors. The formula is as follows:

$dst_i = src0_i / src1_i$

## Prototype<a name="section620mcpsimp"></a>

- The entire tensor participates in computation

    ```cpp
    dst = src0 / src1;
    ```

- Prototype with config

    - Continuous computation of the first n data elements of the tensor

        ```cpp
        template <typename T, const DivConfig& config = DEFAULT_DIV_CONFIG>
        __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
        ```

    - Tensor high-dimensional split computation
        - **mask** bit-by-bit mode

            ```cpp
            template <typename T, bool isSetMask = true, const DivConfig& config = DEFAULT_DIV_CONFIG>
            __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

        - **mask** continuous mode

            ```cpp
            template <typename T, bool isSetMask = true, const DivConfig& config = DEFAULT_DIV_CONFIG>
            __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

- Prototype without config

    - Continuous computation of the first n data elements of the tensor

        ```cpp
        template <typename T>
        __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
        ```

    - Tensor high-dimensional splitting computation
        - **mask** bit-by-bit mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

        - **mask** continuous mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: set the mask inside the API.<br>&bull; false: set the mask outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |
| <!-- npu="950" id19 -->config | This parameter is supported only on Ascend 950PR&950DT products.<br>Configures the precision computation mode. The type is DivConfig, defined as follows:<br>enum&nbsp;class&nbsp;DivAlgo&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;INTRINSIC&nbsp;=&nbsp;0,<br>&nbsp;&nbsp;&nbsp;&nbsp;DIFF_COMPENSATION,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_0ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_0ULP_FTZ_FALSE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_FALSE<br>};<br>struct&nbsp;DivConfig&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;DivAlgo&nbsp;algo&nbsp;=&nbsp;DivAlgo::INTRINSIC;<br>};<br>Selects the Div algorithm and configures the Subnormal mode through the algo parameter of the DivConfig struct. For details, see [Key Feature Description](#div-key-features).<br>The values of algo are as follows:<br>&bull; DivAlgo::INTRINSIC: default algorithm, with a maximum precision error of 1 ulp. For the half and float types, Subnormal handling is controlled by the --cce-ftz compilation option (default value: true).<br>&bull; DivAlgo::PRECISION_1ULP_FTZ_TRUE: uses single-instruction computation, with a maximum precision error of 1 ulp.<br>&bull; DivAlgo::DIFF_COMPENSATION: uses the difference compensation algorithm, with a maximum precision error of 0 ulp. For the float data type, Subnormal handling is controlled by the --cce-ftz compilation option (default value: true).<br>&bull; DivAlgo::PRECISION_0ULP_FTZ_TRUE: uses the difference compensation algorithm, with a maximum precision error of 0 ulp, in FTZ mode. Currently, this algorithm supports the float data type.<br>&bull; DivAlgo::PRECISION_0ULP_FTZ_FALSE: uses the difference compensation algorithm, supports Subnormal data computation, with a maximum precision error of 0 ulp. Currently, this algorithm supports the float data type.<br>&bull; DivAlgo::PRECISION_1ULP_FTZ_FALSE: supports Subnormal data computation, with a maximum precision error of 1 ulp.<br><br>The default value DEFAULT_DIV_CONFIG of this parameter is as follows:<br>constexpr&nbsp;DivConfig&nbsp;DEFAULT_DIV_CONFIG&nbsp;=&nbsp;{&nbsp;DivAlgo::INTRINSIC&nbsp;};<br>If the config parameter is not explicitly passed when calling this prototype, DEFAULT_DIV_CONFIG is used by default, and the behavior is equivalent to that of the prototype without the config parameter.<!-- end id19 --> |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src0, src1 | Input | Source operands.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md).<br>The data types of the two source operands must be consistent with that of the destination operand. |
| count | Input | Number of elements that participate in computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements that participate in computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To process all input data, multiple iterations (repeats) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Splitting](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of operands. Type: [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of an operand and the address stride of different DataBlocks within the same iteration of an operand.<br>For details about the address stride parameter between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride parameter of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

<!-- npu="950" id20 -->
- For Ascend 950PR&950DT products, T supports the following data types: int16_t, uint16_t, half, int32_t, uint32_t, float, complex32, int64_t, uint64_t, and complex64. The complex32, int64_t, uint64_t, and complex64 data types are supported only by the API that computes the first n data elements of a tensor and by the operator overloading that uses the entire tensor to participate in computation.
<!-- end id20 -->
<!-- npu="A3" id21 -->
- For Atlas A3 products, T supports the following data types: half and float.
<!-- end id21 -->
<!-- npu="910b" id22 -->
- For Atlas A2 products, T supports the following data types: half and float.
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

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
- When the operator overloading that uses the entire tensor to participate in computation is used, the operand is the total length of the destination LocalTensor.

<!-- npu="A3,910b,950" id28 -->
- When the count or repeatTime parameter is set to 0, the API behaves as follows:
  <!-- npu="A3,910b" id34 -->
  - For the following models, when the value of count or repeatTime is 0, no computation is performed and no write is made to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id29 -->
    - Atlas A3 products
    <!-- end id29 -->
    <!-- npu="910b" id30 -->
    - Atlas A2 products
    <!-- end id30 -->
  <!-- end id34 -->
  <!-- npu="950" id33 -->
  - For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the value of count or repeatTime is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id33 -->
<!-- end id28 -->
<!-- npu="950" id32 -->
- Notes on Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - High-dimensional splitting computation of tensors occupies 8 KB of UB.
  - Continuous computation of the first n data elements of a tensor does not involve the 8 KB UB occupation.
<!-- end id32 -->

<!-- npu="950" id35 -->
## Key Features<a id="div-key-features"></a>

For Ascend 950PR&950DT products, the key features are as follows:

### Maximum Precision Error

- The maximum precision error of DivAlgo::INTRINSIC, DivAlgo::PRECISION_1ULP_FTZ_TRUE, and DivAlgo::PRECISION_1ULP_FTZ_FALSE is 1 ulp.
- The maximum precision error of DivAlgo::DIFF_COMPENSATION, DivAlgo::PRECISION_0ULP_FTZ_TRUE, and DivAlgo::PRECISION_0ULP_FTZ_FALSE is 0 ulp.

### Configuring Subnormal Mode

FTZ (Flush-To-Zero): A floating-point operation mode in which, when the result is [Subnormal](../../data_structures/builtin_data_types.md#p7381131713310), it is directly cleared to zero (approximated as 0) instead of preserving its exact tiny value.

#### Default Algorithm

DivAlgo::INTRINSIC is the default algorithm. When --cce-ftz=false, Subnormal is preserved; when --cce-ftz=true (the default value), FTZ mode is used.

#### Explicitly Specified Algorithms

- DivAlgo::DIFF_COMPENSATION supports the float type, and Subnormal handling is controlled by --cce-ftz.
- Algorithms whose names contain **FTZ_FALSE** always preserve Subnormal.
- Algorithms whose names contain **FTZ_TRUE** always use FTZ mode.

#### Usage Recommendations

When --cce-ftz=true (the default value), in general scenarios it is recommended to use DivAlgo::INTRINSIC, which offers better performance, or to explicitly select DivAlgo::PRECISION_1ULP_FTZ_TRUE or DivAlgo::PRECISION_0ULP_FTZ_TRUE; when exact Subnormal output is required, use DivAlgo::PRECISION_0ULP_FTZ_FALSE or DivAlgo::PRECISION_1ULP_FTZ_FALSE.
<!-- end id35 -->

## Example<a name="section642mcpsimp"></a>

- Tensor high-dimensional splitting computation example - mask continuous mode

    ```cpp
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Div(dstLocal, src0Local, src1Local, 128, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- Tensor high-dimensional splitting computation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Div(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- Sample of the API for computing the first n data elements of a tensor

    ```cpp
    AscendC::Div(dstLocal, src0Local, src1Local, 512);
    ```

    <!-- npu="950" id31 -->
    The following example is supported only by the Ascend 950PR&950DT products.

    ```cpp
    // Div 0ulp.
    static constexpr DivConfig config = { DivAlgo::DIFF_COMPENSATION };
    Div<T, config>(dstLocalX, srcLocalX, srcLocalY, 512);
    // Div 0ulp precision and Subnormal mode.
    static constexpr DivConfig config2 = { DivAlgo::PRECISION_0ULP_FTZ_FALSE };
    Div<T, config2>(dstLocalX, srcLocalX, srcLocalY, 512);
    ```
    <!-- end id31 -->

- Sample of the entire tensor participating in computation

    ```cpp
    dstLocal = src0Local / src1Local;
    ```

The result is as follows:

```bash
Input data src0Local: [1.0 2.0 3.0 ... 512.0]
Input data src1Local: [2.0 2.0 2.0 ... 2.0]
Output data dstLocal: [0.5 1.0 1.5 ... 256.0]
```
