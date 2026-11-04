# ShiftLeft (Left Shift Count as Tensor)<a name="ZH-CN_TOPIC_0000002158548560"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:10:56.645Z pushedAt=2026-10-28T03:14:39.535Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/logical_compute/ShiftLeft_shift_amount_tensor_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Performs a left shift operation on each element in the source operand. Depending on the data type of the source operand, the left shift operation falls into the following two cases:

-   Data type is an unsigned type: a logical left shift is performed. A logical left shift moves the entire binary number to the left by the specified number of bits. The most significant bit is discarded and the least significant bit is filled with 0. For example, after the binary number 1010101010101010 (uint16\_t type) is logically shifted left by 1 bit, the result is 0101010101010100.
-   Data type is a signed type: an arithmetic left shift is performed. An arithmetic left shift moves the entire binary number to the left by the specified number of bits. The most significant bit is discarded (the sign bit is discarded), and the least significant bit is filled with 0. For example, after the binary number 1010101010101010 (int16\_t type) is arithmetically shifted left by 1 bit, the result is 0101010101010100; after it is arithmetically shifted left by 3 bits, the result is 0101010101010000.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T, typename U>
__aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src1, const int32_t& count)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T | Data type of the source/destination operand. |
| U | Data type of the source operand. |

**Table 2**  Parameters

| **Parameter** | Input/Output | **Description** |
| :--- | :--- | :--- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| src0 | Input | Source operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.<br>The data type must be consistent with that of the destination operand. |
| src1 | Input | LocalTensor that stores the left shift count. The byte size of the data type must match the byte size of the element data type in the src0 source operand tensor. Negative values are not supported. |
| count | Input | Number of elements involved in the computation. |

## Data Type
<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, T supports the following data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t. U supports the following data types: int8_t, int16_t, int32_t, int64_t.
<!-- end id8 -->

## Return Value<a name="section194321251175110"></a>

None

## Constraints<a name="section15109150164412"></a>

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

-   For a logical shift (unsigned data type), if the shift amount is greater than the data type bit width, the output is 0.
-   For an arithmetic shift (signed data type), if src0 is less than 0, src1 is less than 0, and the shift amount is greater than the data type bit width, the output is -1; if src0 is greater than 0 and the shift amount is greater than the data type bit width, the output is 0.

## Examples<a name="section132384819392"></a>

```cpp
AscendC::ShiftLeft(dstLocal, srcLocal0, srcLocal1, 512);
```

The result is as follows:

```plain
Input data srcLocal0: [1 2 3 ... 512]
Input data srcLocal1: [2 2 2 ... 2]
Output data dstLocal: [4 8 12 ... 2048]
```
