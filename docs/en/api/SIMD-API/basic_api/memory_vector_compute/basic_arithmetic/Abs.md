# Abs<a name="ZH-CN_TOPIC_0000001429123797"></a>

<!-- md-trans-meta sourceCommit=f266635ab179c912cc9641061c2d27246be9170c translatedAt=2026-11-02T06:17:12.419Z pushedAt=2026-11-18T01:25:07.000Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Abs_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_unary_intf.h"`.

Abs is a unary vector computation API that is responsible for taking the absolute value of the input tensor element-wise. The calculation formula is as follows:

$dst_i = |src_i|$

## Prototype<a name="section620mcpsimp"></a>

- Continuous computation of the first n elements of the tensor

    ```cpp
    template <typename T>
    __aicore__ inline void Abs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
    ```

<!-- npu="950" id10 -->
- Tensor First n Complex Number Computation

    ```cpp
    // This function prototype is only supported on Ascend 950PR&950DT products.
    template <typename T, typename U>
    __aicore__ inline void Abs(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t& count)
    ```
<!-- end id10 -->

- Tensor high-dimensional slicing computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Abs(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Abs(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| <!-- npu="950" id11 -->U | Data type of the source operand in the tensor first n complex number computation API.<!-- end id11 --> |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the calculation. For details about this parameter, see [Continuous Compute](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements involved in the calculation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector compute unit reads 256 consecutive bytes of data for calculation each time. To process all input data, multiple iterations (repeat) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operand. Type: [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which contains parameters such as the address stride of the same DataBlock between adjacent iterations of the operand and the address stride of different DataBlocks within the same iteration of the operand.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

<!-- npu="950" id12 -->
- For Ascend 950PR&950DT products
    - T supports the following data types: int8_t, int16_t, half, int32_t, float, and int64_t. The int8_t and int64_t data types are supported only by the API for computing the first n data elements of a tensor.
    - U supports the following data types: complex32 and complex64. When U is complex32, T must be half. When U is complex64, T must be float.
<!-- end id12 -->
<!-- npu="A3" id13 -->
- For Atlas A3 products, T supports the following data types: half and float.
<!-- end id13 -->
<!-- npu="910b" id14 -->
- For Atlas A2 products, T supports the following data types: half and float.
<!-- end id14 -->
<!-- npu="310b" id15 -->
- For Atlas 200I/500 A2 inference products, T supports the following data types: int16_t, half, and float.
<!-- end id15 -->
<!-- npu="310p" id16 -->
- For Atlas inference products (AI Core), T supports the following data types: half and float.
<!-- end id16 -->
<!-- npu="910" id17 -->
- For Atlas training products, T supports the following data types: half and float.
<!-- end id17 -->
<!-- npu="x90" id18 -->
- For Kirin X90 processor products, T supports the following data types: half and float.
<!-- end id18 -->
<!-- npu="9030" id19 -->
- For Kirin 9030 processor products, T supports the following data types: half and float.
<!-- end id19 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Abs_res.md#id2 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
<!-- npu="A3,910b,950" id20 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id25 -->
  - For the following models, when the count or repeatTime parameter is set to 0, no computation is performed and no data is written to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id26 -->
    - Atlas A3 products
    <!-- end id26 -->
    <!-- npu="910b" id28 -->
    - Atlas A2 products
    <!-- end id28 -->
  <!-- end id25 -->
  <!-- npu="950" id27 -->
  - For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id27 -->
<!-- end id20 -->
<!-- npu="950" id21 -->
- Notes on Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - High-dimensional split computation of the tensor occupies 8 KB of UB.
  - Continuous computation of the first n data elements of the tensor does not involve the 8 KB UB usage.
<!-- end id21 -->
<!-- npu="950" id29 -->
- For the Ascend 950PR&950DT products, only the APIs of the **complex32** and **complex64** types involve Subnormal in internal computation, and the handling is controlled by the compilation option **--cce-ftz** (default value: **true**):
    - When **--cce-ftz=false**, Subnormal values are preserved during computation and participate in subsequent computation according to their actual values.
    - When **--cce-ftz=true**, the FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in computation are treated as 0, which may cause precision differences in the computation result compared with preserving Subnormal values.
<!-- end id29 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Abs_res.md#id3 -->

## Examples<a name="section176061616102911"></a>

- Tensor high-dimensional split calculation example - mask continuous mode

    ```cpp
    uint64_t mask = 256 / sizeof(half);
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Abs(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

    The result is as follows:

    ```bash
    Input data srcLocal: [0.0 -1.0 2.0 -3.0 ...]
    Output data dstLocal: [0.0 1.0 2.0 3.0 ...]
    ```

- Tensor high-dimensional split calculation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Abs(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

    The result is as follows:

    ```bash
    Input data srcLocal: [0.0 -1.0 2.0 -3.0 ...]
    Output data dstLocal: [0.0 1.0 2.0 3.0 ...]
    ```

- example of the API for computing the first n data of a tensor

    ```cpp
    AscendC::Abs(dstLocal, srcLocal, 512);
    ```

    The result is as follows:

    ```bash
    Input data srcLocal: [0.0 -1.0 2.0 -3.0 ...]
    Output data dstLocal: [0.0 1.0 2.0 3.0 ...]
    ```

<!-- npu="950" id24 -->
For Ascend 950PR&950DT products:

- Example of the API for computing the first n complex numbers of a tensor

    ```cpp
    LocalTensor<complex64> srcLocal = inQueue.DeQue<complex64>();
    LocalTensor<float> dstLocal = outQueue.AllocTensor<float>();
    AscendC::Abs(dstLocal, srcLocal, 512);
    ```

    The result is as follows:

    ```bash
    Input data srcLocal: [1+2j  2-3j ...]
    Output data dstLocal: [2.236 3.605 ...]
    ```
<!-- end id24 -->
