# ShiftRight<a name="ZH-CN_TOPIC_0000001475200925"></a>

<!-- md-trans-meta sourceCommit=9bb45191e5688a8d85bea8f7a62e273b7704ea8f translatedAt=2026-10-26T10:16:42.646Z pushedAt=2026-10-28T03:14:39.543Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/logical_compute/ShiftRight_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`.

Performs a right shift operation on each element in the source operand, where the number of bits to shift is determined by the scalar scalarValue. Depending on the data type of the source operand, the right shift operation falls into the following two cases:

- When the data type is an unsigned type: a logical right shift is performed. A logical right shift moves the entire binary number to the right by the specified number of bits, discarding the least significant bit and filling the most significant bit with 0. For example, after the binary number 1010101010101010 (uint16\_t type) is logically shifted right by 1 bit, the result is 0101010101010101.
- When the data type is a signed type: an arithmetic right shift is performed. An arithmetic right shift moves the entire binary number to the right by the specified number of bits, discarding the least significant bit and filling the most significant bit with a copy of the sign bit. For example, after the binary number 1010101010101010 (int16\_t type) is arithmetically shifted right by 1 bit, the result is 1101010101010101; after it is arithmetically shifted right by 3 bits, the result is 1111010101010101.

The calculation formula is as follows:

$dst_i = src_i \gg scalarValue$

## Prototype<a name="section620mcpsimp"></a>

- Continuous computation of the first n elements of the tensor

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void ShiftRight(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
    ```

- Tensor high-dimensional split computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void ShiftRight(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool roundEn = false)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void ShiftRight(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool roundEn = false)
        ```

When dst and src use the [TensorTrait](../../aux_data_structures/TensorTrait/TensorTrait.md) type, the data type TensorTrait is inconsistent with the data type of scalarValue (corresponding to the LiteType type in TensorTrait). Therefore, a new template type U is introduced to represent the data type of scalarValue, and std::enable\_if is used to check whether the LiteType extracted from T is exactly the same as U. If they are the same, the API passes compilation; otherwise, compilation fails. The API prototype is defined as follows:

- Computation of the first n elements of the tensor

    ```cpp
    template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void ShiftRight(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
    ```

- tensor high-dimensional split calculation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void ShiftRight(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool roundEn)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void ShiftRight(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool roundEn)
        ```

## Parameters<a name="section1436019411811"></a>

### Template Parameters and API Parameters

**Table 1** Template parameter description

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| U | Data type of scalarValue. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: sets the mask inside the API.<br>&bull; false: sets the mask outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode:<br>&nbsp;&nbsp;&bull; For the API that computes the first n data elements of a tensor, the count input parameter does not take effect. Set it to 1.<br>&nbsp;&nbsp;&bull; For the API that computes high-dimensional split of a tensor, set the mask input parameter to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning.<br><!-- npu="950,310b" id8 -->For the following models, the isSetMask parameter in the API that computes the first n data elements of a tensor does not take effect. Keep the default value.<br><!-- end id8 --><!-- npu="950" id9 -->&bull; Ascend 950PR&950DT products<br><!-- end id9 --><!-- npu="310b" id10 -->&bull; Atlas 200I/500 A2 inference products<!-- end id10 -->  |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md).<br>The data type of the source operand must be the same as that of the destination operand. |
| scalarValue | Input | Number of bits to shift right. The data type must be the same as the element data type of the destination operand. For details about the value, see [Value Description of scalarValue](#value-description-of-scalarvalue). |
| count | Input | Number of elements involved in the calculation. For details about this parameter, see [Continuous Compute](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements involved in the calculation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector compute unit reads 256 consecutive bytes of data for calculation each time. To process all input data, multiple iterations (repeat) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameter that controls the address stride of the operand. Type: [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md). It contains parameters such as the address stride of the same DataBlock between adjacent iterations of the operand and the address stride of different DataBlocks within the same iteration of the operand.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |
| roundEn | Input | Switch for enabling the rounding function. Supported data type: bool. true enables it, and false disables it. It takes effect only when src is of the int16\_t/int32\_t type.<br>Example: Enable the rounding function. The src data type is int16\_t. Shift src arithmetically right by 5 bits. If the fifth bit of the src\_ele binary number is 1, the dst\_ele value is the result of arithmetically shifting src\_ele right by 5 and then adding 1. <br>src\_ele = 17 = 0b00000000000**1**0001, the fifth bit is 1<br>dst\_ele = arithmetic\_right\_shift(src\_ele, 5) + 1 = 0b00000000000**00000** + 1 = 0b00000000000**00001**<br><!-- npu="310b" id11 -->For Atlas 200I/500 A2 inference products, enabling the rounding function is not supported. Only false can be passed.<!-- end id11 --><br><!-- npu="950" id12 -->For Ascend 950PR&950DT products, enabling the rounding function is not supported. Only false can be passed.<!-- end id12 --> |

### Value Description of scalarValue

<!-- npu="950" id13 -->
- For Ascend 950PR&950DT products, the value of scalarValue must be greater than or equal to 0. For a logical right shift, if the number of shifted bits is greater than the bit width of the src data type, all elements of dst are assigned 0. For an arithmetic right shift, if the number of shifted bits is greater than the bit width of the src data type, the corresponding destination operand element is assigned 0 when the source operand element is positive, and is assigned -1 when the source operand element is negative.
<!-- end id13 -->
<!-- npu="A3" id14 -->
- For Atlas A3 products, when src is of the uint16_t or int16_t type, the value range of scalarValue is [0, 16]; when src is of the uint32_t or int32_t type, the value range of scalarValue is [0, 32].
<!-- end id14 -->
<!-- npu="910b" id15 -->
- For Atlas A2 products, when src is of the uint16_t or int16_t type, the value range of scalarValue is [0, 16]; when src is of the uint32_t or int32_t type, the value range of scalarValue is [0, 32].
<!-- end id15 -->
<!-- npu="310b" id16 -->
- For Atlas 200I/500 A2 inference product, when src is of the uint16_t or int16_t type, the value range of scalarValue is [0, 16]; when src is of the uint32_t or int32_t type, the value range of scalarValue is [0, 32].
<!-- end id16 -->

## Data Type

<!-- npu="950" id17 -->
- For Ascend 950PR&950DT products, the data types supported by T and U are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t. The data types int8_t, uint8_t, int64_t, and uint64_t are supported only by the API for computing the first n data of a tensor.
<!-- end id17 -->
<!-- npu="A3" id18 -->
- For Atlas A3 products, the data types supported by T and U are: int16_t, uint16_t, int32_t, uint32_t.
<!-- end id18 -->
<!-- npu="910b" id19 -->
- For Atlas A2 products, the data types supported by T and U are: int16_t, uint16_t, int32_t, uint32_t.
<!-- end id19 -->
<!-- npu="310b" id20 -->
- For Atlas 200I/500 A2 inference product, the data types supported by T and U are: int16_t, uint16_t, int32_t, uint32_t.
<!-- end id20 -->

## Return Value<a name="section194321251175110"></a>

None

## Constraints<a name="section23625411480"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
<!-- npu="A3,910b,950" id21 -->
- When the count or repeatTime parameter is set to 0, the API behaves as follows:
  <!-- npu="A3,910b" id22 -->
  - For the following models, the API does not perform any calculation and does not write to the destination operand. The API is treated as a NOP (no operation).
    <!-- npu="A3" id23 -->
    - Atlas A3 products
    <!-- end id23 -->
    <!-- npu="910b" id24 -->
    - Atlas A2 products
    <!-- end id24 -->
  <!-- end id22 -->
  <!-- npu="950" id25 -->
  - For Ascend 950PR&950DT products, the API implements compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, it is not guaranteed that the API is treated as a NOP (no operation).
  <!-- end id25 -->
<!-- end id21 -->
<!-- npu="950" id26 -->
- For Ascend 950PR&950DT products, the isSetMask parameter in the API for computing the first n tensor data does not take effect. Keep the default value.
<!-- end id26 -->

## Example<a name="section642mcpsimp"></a>

- Tensor high-dimensional split calculation example - mask continuous mode

    ```cpp
    uint64_t mask = 128;
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::ShiftRight(dstLocal, srcLocal, scalar, mask, 4, { 1, 1, 8, 8 }, false);
    ```

- Tensor high-dimensional split calculation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::ShiftRight(dstLocal, srcLocal, scalar, mask, 4, {1, 1, 8, 8}, false);
    ```

- Example of computing the first n tensor data

    ```cpp
    int16_t scalar = 2;
    AscendC::ShiftRight(dstLocal, srcLocal, scalar, 512);
    ```

The result is as follows:

```plain
Input data srcLocal: [1 2 3 ... 512]
Input data scalar = 2
Output data dstLocal: [0 0 0 1 1 1 1 ... 128]
```
