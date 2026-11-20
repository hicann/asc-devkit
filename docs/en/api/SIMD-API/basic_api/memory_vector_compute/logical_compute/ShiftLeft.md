# ShiftLeft<a name="ZH-CN_TOPIC_0000001474880641"></a>

<!-- md-trans-meta sourceCommit=f266635ab179c912cc9641061c2d27246be9170c translatedAt=2026-11-02T08:41:03.808Z pushedAt=2026-11-18T01:25:07.154Z nl=2 -->

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
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/logical_compute/ShiftLeft_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`.

Performs a left shift operation on each element of the source operand, where the number of bits to shift is determined by the scalar value scalarValue. Depending on the data type of the source operand, the left shift operation falls into the following two cases:

- When the data type is an unsigned type: a logical left shift is performed. A logical left shift moves the entire binary number to the left by the specified number of bits, discarding the most significant bit and filling the least significant bit with 0. For example, after the binary number 1010101010101010 (uint16\_t type) is logically shifted left by 1 bit, the result is 0101010101010100.
- When the data type is a signed type: an arithmetic left shift is performed. An arithmetic left shift moves the entire binary number to the left by the specified number of bits, discarding the most significant bit (the sign bit is discarded) and filling the least significant bit with 0. For example, after the binary number 1010101010101010 (int16\_t type) is arithmetically shifted left by 1 bit, the result is 0101010101010100; after it is arithmetically shifted left by 3 bits, the result is 0101010101010000.

The calculation formula is as follows:

$dst_i = src_i \ll scalarValue$

## Prototype<a name="section620mcpsimp"></a>

- Continuous computation of the first n elements of the tensor

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
    ```

- Tensor high-dimensional slicing computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

When dst and src use the [TensorTrait](../../aux_data_structures/TensorTrait/TensorTrait.md) type, the data type TensorTrait is inconsistent with the data type of scalarValue (corresponding to the LiteType type in TensorTrait). Therefore, a new template type U is introduced to represent the data type of scalarValue, and std::enable\_if is used to check whether the LiteType extracted from T is exactly the same as U. If they are the same, the API passes compilation; otherwise, compilation fails. The API prototype is defined as follows:

- Tensor first n elements computation

    ```cpp
    template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
    ```

- Tensor high-dimensional slicing computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

### Template Parameters and API Parameters

**Table 1** Template parameter description

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| U | Data type of scalarValue. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: sets the mask inside the API.<br>&bull; false: sets the mask outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode:<br>&nbsp;&nbsp;&bull; For the API that computes the first n data elements of a tensor, the count input parameter does not take effect. Set it to 1.<br>&nbsp;&nbsp;&bull; For the API that computes high-dimensional split of a tensor, set the mask input parameter to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning.<br><!-- npu="950,310b" id8 -->For the following models, the isSetMask parameter in the API that computes the first n data elements of a tensor does not take effect. Keep the default value.<br><!-- end id8 --><!-- npu="950" id9 -->&bull; Ascend 950PR&950DT products<br><!-- end id9 --><!-- npu="310b" id10 -->&bull; Atlas 200I/500 A2 inference products<!-- end id10 --> |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md).<br>The data type of the source operand must be the same as that of the destination operand. |
| scalarValue | Input | Number of bits to shift left. The data type must be the same as the data type of the elements in the destination operand. For details about the value description, see [Value Description of scalarValue](#value-description-of-scalarvalue). |
| mask[]/mask | Input | mask controls the elements involved in the calculation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector compute unit reads 256 consecutive bytes of data for calculation each time. To process all input data, multiple iterations (repeat) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operand. Type: [UnaryRepeatParams](../../aux_data_structures//UnaryRepeatParams.md), which contains the address stride between the same DataBlock in adjacent iterations and the address stride between different DataBlocks in the same iteration.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlock in the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

### Value Description of scalarValue

<!-- npu="950" id11 -->
- For Ascend 950PR&950DT products, the value of scalarValue must be greater than or equal to 0. If the number of bits shifted left is greater than the bit width of the src data type, all elements of dst are set to 0.
<!-- end id11 -->
<!-- npu="A3" id12 -->
- For Atlas A3 products, when src is of the uint16_t or int16_t type, the value range of scalarValue is [0, 16]; when src is of the uint32_t or int32_t type, the value range of scalarValue is [0, 32].
<!-- end id12 -->
<!-- npu="910b" id13 -->
- For Atlas A2 products, when src is of the uint16_t or int16_t type, the value range of scalarValue is [0, 16]; when src is of the uint32_t or int32_t type, the value range of scalarValue is [0, 32].
<!-- end id13 -->
<!-- npu="310b" id14 -->
- For Atlas 200I/500 A2 inference product, when src is of the uint16_t or int16_t type, the value range of scalarValue is [0, 16]; when src is of the uint32_t or int32_t type, the value range of scalarValue is [0, 32].
<!-- end id14 -->

## Data Type

<!-- npu="950" id15 -->
- For Ascend 950PR&950DT products, the data types supported by T and U are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t. The data types int8_t, uint8_t, int64_t, and uint64_t are supported only by the API for computing the first n data of a tensor.
<!-- end id15 -->
<!-- npu="A3" id16 -->
- For Atlas A3 products, the data types supported by T and U are: int16_t, uint16_t, int32_t, uint32_t.
<!-- end id16 -->
<!-- npu="910b" id17 -->
- For Atlas A2 products, the data types supported by T and U are: int16_t, uint16_t, int32_t, uint32_t.
<!-- end id17 -->
<!-- npu="310b" id18 -->
- For Atlas 200I/500 A2 inference product, the data types supported by T and U are: int16_t, uint16_t, int32_t, uint32_t.
<!-- end id18 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/logical_compute/ShiftLeft_res.md#id2 -->

## Return Value<a name="section194321251175110"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
<!-- npu="A3,910b,950" id19 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id20 -->
  - For the following models, the API does not perform any calculation and does not write to the destination operand. The API is treated as a NOP (no operation).
    <!-- npu="A3" id21 -->
    - Atlas A3 products
    <!-- end id21 -->
    <!-- npu="910b" id22 -->
    - Atlas A2 products
    <!-- end id22 -->
  <!-- end id20 -->
  <!-- npu="950" id23 -->
  - For Ascend 950PR&950DT products, the API implements compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, it is not guaranteed that the API is treated as a NOP (no operation).
  <!-- end id23 -->
<!-- end id19 -->
<!-- npu="950" id24 -->
- For Ascend 950PR&950DT products, the isSetMask parameter in the API for computing the first n tensor data does not take effect. Keep the default value.
<!-- end id24 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/logical_compute/ShiftLeft_res.md#id3 -->

## Examples<a name="section132384819392"></a>

- Tensor high-dimensional split calculation example - mask continuous mode

    ```cpp
    uint64_t mask = 128;
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::ShiftLeft(dstLocal, srcLocal, scalar, mask, 4, { 1, 1, 8, 8 });
    ```

- Tensor high-dimensional split calculation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::ShiftLeft(dstLocal, srcLocal, scalar, mask, 4, {1, 1, 8, 8});
    ```

- Example of computing the first n tensor data

    ```cpp
    int16_t scalar = 2;
    AscendC::ShiftLeft(dstLocal, srcLocal, scalar, 512);
    ```

The result is as follows:

```plain
Input data srcLocal: [1 2 3 ... 512]
Input data scalar = 2
Output data dstLocal: [4 8 12 ... 2048]
```
