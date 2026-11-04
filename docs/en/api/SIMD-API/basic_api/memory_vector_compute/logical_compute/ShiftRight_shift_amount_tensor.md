# ShiftRight (Right Shift Amount as Tensor)<a name="ZH-CN_TOPIC_0000002193989473"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:13:59.526Z pushedAt=2026-10-28T03:14:39.537Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/logical_compute/ShiftRight_shift_amount_tensor_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Performs a right shift on each element of the source operand.

Performs a logical right shift on the source operand of an unsigned data type, and an arithmetic right shift on the source operand of a signed data type.

A logical right shift removes the least significant bit and fills the most significant bit with 0.

An arithmetic right shift removes the least significant bit and fills the most significant bit with a copy of the sign bit.

Example: For the data type uint16\_t, the binary number 1010101010101010 shifted right by one bit logically yields 0101010101010101.

For the data type int16\_t, the binary number 1010101010101010 shifted right by one bit arithmetically yields 1101010101010101.

For the data type int16\_t, the binary number 1010101010101010 shifted right by three bits arithmetically yields 1111010101010101.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T, typename U>
__aicore__ inline void ShiftRight(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src1, const int32_t& count)
```

## Parameters<a name="section1436019411811"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T | Data type of the source/destination operands. |
| U | Data type of the source operand. |

**Table 2**  Parameters

| **Parameter** | Input/Output | **Description** |
| :--- | :--- | :--- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| src0 | Input | Source operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.<br>The data type must be the same as that of the destination operand. |
| src1 | Input | LocalTensor that stores the right shift amount. The byte size of its data type must match the byte size of the element data type in the source src0 operand tensor. Negative values are not supported. |
| count | Input | Number of elements involved in the computation. |


## Data Type
<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, T supports the following data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, and uint64_t. U supports the following data types: int8_t, int16_t, int32_t, and int64_t.
<!-- end id8 -->

## Return Value<a name="section194321251175110"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

-   For a logical shift (unsigned data type), if the shift amount is greater than the data type bit width, the output is 0.
-   For an arithmetic shift (signed data type), if src0 is less than 0, src1 is greater than 0, and the shift amount is greater than the data type bit width, the output is -1; if src0 is greater than 0 and the shift amount is greater than the data type bit width, the output is 0.

## Example<a name="section642mcpsimp"></a>

```cpp
AscendC::ShiftRight(dstLocal, srcLocal0, srcLocal1, 512);
```

The result is as follows:

```plain
Input data (srcLocal0): [1 2 3 ... 512]
Input data (srcLocal1): [2 2 2 ... 2]
Output data (dstLocal): [0 0 0 1 1 1 1 ... 128]
```
