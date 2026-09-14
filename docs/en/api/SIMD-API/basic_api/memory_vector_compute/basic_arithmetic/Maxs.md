# Maxs<a name="ZH-CN_TOPIC_0000001424924030"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T10:58:17.677Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
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
- Atlas training products: Not supported
<!-- end id7 -->
## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`.

Maxs is a binary scalar computation API. Its inputs are a vector tensor and a scalar. Maxs computes the maximum value between each element in the vector tensor and the scalar. The calculation formula is as follows:

$dst_i = \max(src_i, scalarValue)$

## Prototype<a name="section620mcpsimp"></a>

- Continuous computation of the first n data elements of the tensor

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Maxs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
    ```

- High-dimensional slicing computation of the tensor
    - Bitwise mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Maxs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - Continuous mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Maxs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

When dst and src use the [TensorTrait](../../aux_data_structures/TensorTrait/TensorTrait.md) type, the data type TensorTrait is inconsistent with the data type of scalarValue (corresponding to the LiteType type in TensorTrait). Therefore, a new template type U is introduced to represent the data type of scalarValue, and std::enable\_if is used to check whether the LiteType extracted from T is exactly the same as U. If they are consistent, the API passes compilation; otherwise, compilation fails. The API prototype is defined as follows:

- Computation of the first n data elements of the tensor

    ```cpp
    template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void Maxs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
    ```

- High-dimensional slicing computation of the tensor
    - Bitwise mask mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void Maxs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - Continuous mask mode

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
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, the developer needs to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode:<br>&nbsp;&nbsp;&bull; For the API that computes the first n elements of a tensor, the count input parameter does not take effect. It is recommended to set it to 1.<br>&nbsp;&nbsp;&bull; For the API that computes high-dimensional slicing of a tensor, the mask input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| scalarValue | Input | Source operand. Its data type must be consistent with the element type of the destination operand. |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md). |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeats) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

<!-- npu="950" id13 -->
- For Ascend 950PR/Ascend 950DT, the data types supported by T and U are int8_t, uint8_t, int16_t, half, bfloat16_t, int32_t, float, int64_t, and uint64_t. The data types int8_t, uint8_t, int64_t, and uint64_t are supported only by the API that computes the first n elements of a tensor.
<!-- end id13 -->
<!-- npu="A3" id14 -->
- For Atlas A3 training products/Atlas A3 inference products, the data types supported by T and U are int16_t, half, int32_t, and float.
<!-- end id14 -->
<!-- npu="910b" id15 -->
- For Atlas A2 training products/Atlas A2 inference products, the data types supported by T and U are int16_t, half, int32_t, and float.
<!-- end id15 -->
<!-- npu="310b" id16 -->
- For Atlas 200I/500 A2 Inference Product, the data types supported by T and U are int16_t, half, int32_t, and float.
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
- When the value of the count or repeatTime parameter is 0, the behavior of this API is as follows:
  <!-- npu="A3,910b" id24 -->
  - For the following models, when the value of the count or repeatTime parameter is 0, no computation is performed, no write is made to the destination operand, and this API is treated as a NOP (no operation).
    <!-- npu="A3" id26 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id26 -->
    <!-- npu="910b" id28 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id28 -->
    <!-- end id24 -->
    <!-- npu="950" id30 -->
  - For Ascend 950PR/Ascend 950DT: This API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_computation.md) through VF. When the value of the count or repeatTime parameter is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id30 -->
  <!-- end id23 -->
  <!-- npu="950" id31 -->
- Description of UB space usage. For Ascend 950PR/Ascend 950DT:
  - Tensor high-dimensional slicing computation occupies 8KB of Unified Buffer.
  - Continuous computation of the first n data of a tensor does not involve occupation of the 8KB Unified Buffer.
- For Ascend 950PR/Ascend 950DT, the isSetMask parameter in the API for computing the first n data of a tensor does not take effect. Keep its default value.
<!-- end id31 -->

## Example<a name="section642mcpsimp"></a>

- Tensor high-dimensional slicing computation example - continuous mask mode

    ```cpp
    uint64_t mask = 128;
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Maxs(dstLocal, srcLocal, scalar, mask, 4, { 1, 1, 8, 8 });
    ```

- tensor high-dimensional slicing computation sample - bitwise mask mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Maxs(dstLocal, srcLocal, scalar, mask, 4, {1, 1, 8, 8});
    ```

- Sample of the computation API for the first n data elements of a tensor

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
