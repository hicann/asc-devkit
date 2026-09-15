# ShiftLeft<a name="ZH-CN_TOPIC_0000001474880641"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T12:49:13.844Z -->

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_binary_scalar_intf.h"`

Performs a left shift operation on each element of the source operand, where the number of bits to shift is determined by the scalar scalarValue. Depending on the data type of the source operand, the left shift operation falls into the following two cases:

- Data type is an unsigned type: A logical left shift is performed. A logical left shift moves the entire binary number to the left by the specified number of bits, discards the most significant bit, and fills the least significant bit with 0. For example, after the binary number 1010101010101010 (uint16\_t type) is logically shifted left by 1 bit, the result is 0101010101010100.
- Data type is a signed type: An arithmetic left shift is performed. An arithmetic left shift moves the entire binary number to the left by the specified number of bits, discards the second most significant bit, and fills the least significant bit with 0. For example, after the binary number 1010101010101010 (int16\_t type) is arithmetically shifted left by 1 bit, the result is 1101010101010100; after it is arithmetically shifted left by 3 bits, the result is 1101010101010000.

The calculation formula is as follows:

$dst_i = src_i \ll scalarValue$

## Prototype<a name="section620mcpsimp"></a>

- The first n data of the tensor are calculated continuously

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
    ```

- The tensor is calculated by high-dimensional splitting
    - Bitwise mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - Continuous mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

When dst and src use the [TensorTrait](../../aux_data_structures/TensorTrait/TensorTrait.md) type, the data type TensorTrait is inconsistent with the data type of scalarValue (corresponding to the LiteType type in TensorTrait). Therefore, a new template type U is added to represent the data type of scalarValue, and std::enable\_if is used to check whether the LiteType extracted from T is exactly the same as U. If they are the same, the API passes compilation; otherwise, compilation fails. The API prototype is defined as follows:

- Calculation of the first n data of the tensor

    ```cpp
    template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
    ```

- High-dimensional splitting calculation of the tensor
    - Bitwise mask mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - Continuous mask mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

### Template Parameters and API Parameters

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| U | Data type of scalarValue. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. The developer needs to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode:<br>&nbsp;&nbsp;&bull; For the API that computes the first n data of a tensor, the count input parameter does not take effect. It is recommended to set it to 1.<br>&nbsp;&nbsp;&bull; For the API that computes high-dimensional splitting of a tensor, the mask input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning.<br>For the following models, the isSetMask parameter in the API for computing the first n data of a tensor does not take effect, and the default value can be retained.<br>&bull; Ascend 950PR/Ascend 950DT<br>&bull; Atlas inference products |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>The type is LocalTensor, and the supported TPosition is VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>The type is LocalTensor, and the supported TPosition is VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).<br>The data type of the source operand must be the same as that of the destination operand. |
| scalarValue | Input | Number of bits to shift left. The data type must be the same as the element data type of the destination operand. For details about the value, see [scalarValue Value Description](#scalarvalue-value-description) below. |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 consecutive bytes of data for computation each time. To complete the processing of the input data, multiple iterations (repeat) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Splitting](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

### scalarValue Value Description

<!-- npu="950" id9 -->
- For Ascend 950PR/Ascend 950DT, the value of scalarValue must be greater than or equal to 0. If the number of bits shifted left is greater than the bit width of the src data type, all elements of dst are assigned 0.
<!-- end id9 -->
<!-- npu="A3" id10 -->
- For Atlas A3 training products/Atlas A3 inference products, when src is of the uint16_t or int16_t type, the value range of scalarValue is [0, 16]; when src is of the uint32_t or int32_t type, the value range of scalarValue is [0, 32].
<!-- end id10 -->
<!-- npu="910b" id11 -->
- For Atlas A2 training products/Atlas A2 inference products, when src is of the uint16_t or int16_t type, the value range of scalarValue is [0, 16]; when src is of the uint32_t or int32_t type, the value range of scalarValue is [0, 32].
<!-- end id11 -->
<!-- npu="310b" id12 -->
- For Atlas inference products, when src is of the uint16_t or int16_t type, the value range of scalarValue is [0, 16]; when src is of the uint32_t or int32_t type, the value range of scalarValue is [0, 32].
<!-- end id12 -->

## Data Type

<!-- npu="950" id13 -->
- For Ascend 950PR/Ascend 950DT, the data types supported by T and U are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t. The data types int8_t, uint8_t, int64_t, and uint64_t are supported only by the API for computing the first n data of a tensor.
<!-- end id13 -->
<!-- npu="A3" id14 -->
- For Atlas A3 training products/Atlas A3 inference products, the data types supported by T and U are: int16_t, uint16_t, int32_t, uint32_t.
<!-- end id14 -->
<!-- npu="910b" id15 -->
- For Atlas A2 training products/Atlas A2 inference products, the data types supported by T and U are: int16_t, uint16_t, int32_t, uint32_t.
<!-- end id15 -->
<!-- npu="310b" id16 -->
- For Atlas inference products, the data types supported by T and U are: int16_t, uint16_t, int32_t, uint32_t.
<!-- end id16 -->

## Return Value<a name="section194321251175110"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
<!-- npu="A3,910b" id17 -->
- For the following models, when the value of the count or repeatTime parameter is 0, no calculation operation is performed, no write is made to the destination operand, and the API is treated as a NOP (no operation).
  <!-- npu="A3" id18 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id18 -->
  <!-- npu="910b" id19 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id19 -->
<!-- end id17 -->

## Example<a name="section132384819392"></a>

- tensor high-dimensional splitting calculation example - continuous mask mode

    ```cpp
    uint64_t mask = 128;
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::ShiftLeft(dstLocal, srcLocal, scalar, mask, 4, { 1, 1, 8, 8 });
    ```

- tensor high-dimensional splitting calculation example - bitwise mask mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::ShiftLeft(dstLocal, srcLocal, scalar, mask, 4, {1, 1, 8, 8});
    ```

- tensor first n data calculation example

    ```cpp
    int16_t scalar = 2;
    AscendC::ShiftLeft(dstLocal, srcLocal, scalar, 512);
    ```

The result is as follows:

```bash
Input data srcLocal: [1 2 3 ... 512]
Input data scalar = 2
Output data dstLocal: [4 8 12 ... 2048]
```
