# Maxs<a name="ZH-CN_TOPIC_0000001424924030"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:22:47.706Z pushedAt=2026-10-28T03:14:39.403Z nl=2 -->

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
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Maxs_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`.

Maxs is a binary scalar computation API. Its inputs are a vector tensor and a scalar. Maxs computes the maximum of each element in the vector tensor with the scalar. The formula is as follows:

$dst_i = \max(src_i, scalarValue)$

## Prototype<a name="section620mcpsimp"></a>

- Compute the first n elements of the tensor continuously

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Maxs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
    ```

- Tensor high-dimensional split computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Maxs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Maxs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

When dst and src use the [TensorTrait](../../aux_data_structures/TensorTrait/TensorTrait.md) type, the data type TensorTrait is inconsistent with the data type of scalarValue (corresponding to the LiteType type in TensorTrait). Therefore, a new template type U is introduced to represent the data type of scalarValue, and std::enable\_if checks whether the LiteType extracted from T is exactly the same as U. If they are the same, the API passes compilation; otherwise, compilation fails. The API prototype is defined as follows:

- Compute the first n elements of the tensor

    ```cpp
    template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void Maxs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
    ```

- Tensor high-dimensional split computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void Maxs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void Maxs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| U | Data type of scalarValue. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode:<br>&nbsp;&nbsp;&bull; For the API that computes the first n elements of the tensor, the count input parameter does not take effect. It is recommended to set it to 1.<br>&nbsp;&nbsp;&bull; For the tensor high-dimensional split computation API, set the mask input parameter to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md). |
| scalarValue | Input | Source operand. Its data type must be the same as the element type of the destination operand. |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Compute](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements involved in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To process all input data, multiple iterations (repeats) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Splitting](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameter that controls the address stride of the operands. Type: [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which contains parameters such as the address stride of the same DataBlock between adjacent iterations of an operand and the address stride of different DataBlocks within the same iteration of an operand.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

<!-- npu="950" id13 -->
- For Ascend 950PR&950DT products, the data types supported by T and U are int8_t, uint8_t, int16_t, half, bfloat16_t, int32_t, float, int64_t, and uint64_t. The data types int8_t, uint8_t, int64_t, and uint64_t are supported only by the tensor first-n-data computation API.
<!-- end id13 -->
<!-- npu="A3" id14 -->
- For Atlas A3 products, the data types supported by T and U are int16_t, half, int32_t, and float.
<!-- end id14 -->
<!-- npu="910b" id15 -->
- For Atlas A2 products, the data types supported by T and U are int16_t, half, int32_t, and float.
<!-- end id15 -->
<!-- npu="310b" id16 -->
- For Atlas 200I/500 A2 inference products, the data types supported by T and U are int16_t, half, int32_t, and float.
<!-- end id16 -->
<!-- npu="310p" id17 -->
- For Atlas inference products AI Core, the data types supported by T and U are int16_t, half, int32_t, and float.
<!-- end id17 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="A3,910b,950" id23 -->
- When the value of count or repeatTime is 0, the API behaves as follows:
  <!-- npu="A3,910b" id24 -->
  - For the following models, when the value of count or repeatTime is 0, no computation is performed and no write is made to the destination operand. The API is treated as a NOP (no operation).
    <!-- npu="A3" id26 -->
    - Atlas A3 products
    <!-- end id26 -->
    <!-- npu="910b" id28 -->
    - Atlas A2 products
    <!-- end id28 -->
  <!-- end id24 -->
  <!-- npu="950" id30 -->
  - For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg Vector Compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the value of count or repeatTime is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id30 -->
<!-- end id23 -->
<!-- npu="950" id31 -->
- Description of Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - The tensor high-dimensional splitting computation occupies 8 KB of UB.
  - The continuous computation of the first n tensor data does not involve the 8 KB UB usage.
- For Ascend 950PR&950DT products, the isSetMask parameter in the API for computing the first n tensor data does not take effect. Keep its default value.
<!-- end id31 -->

## Example<a name="section642mcpsimp"></a>

- Example of tensor high-dimensional splitting computation - mask continuous mode

    ```cpp
    uint64_t mask = 128;
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Maxs(dstLocal, srcLocal, scalar, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of tensor high-dimensional splitting computation - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Maxs(dstLocal, srcLocal, scalar, mask, 4, {1, 1, 8, 8});
    ```

- Example of the API for computing the first n tensor data

    ```cpp
    int16_t scalar = 2;
    AscendC::Maxs(dstLocal, srcLocal, scalar, 512);
    ```

The result is as follows:

```bash
Input data src0Local: [1 2 3 ... 512]
Input data scalar = 2
Output data dstLocal: [2 2 3 ... 512]
```
