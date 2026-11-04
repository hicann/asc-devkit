# ShiftLeft<a name="ZH-CN_TOPIC_0000002011932045"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:23:14.146Z pushedAt=2026-10-26T07:41:44.536Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/logical_compute/ShiftLeft_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"`.

The `ShiftLeft` instruction performs a left shift on the source operand **srcReg0** element-wise according to **srcReg1** based on the mask, and writes the result to the destination operand **dstReg**. Depending on the data type of the source operand, the left shift operation falls into the following two cases:

- **When the data type is an unsigned type: a logical left shift is performed.**

  A logical left shift moves the entire binary number to the left by the specified number of bits. For each bit shifted, the most significant bit is discarded and the least significant bit is filled with 0. For example, after the binary number 1010101010101010 (uint16\_t type) is logically shifted left by 1 bit, the result is 0101010101010100.
- **When the data type is a signed type: an arithmetic left shift is performed.**

  An arithmetic left shift moves the entire binary number to the left by the specified number of bits. For each bit shifted, the most significant bit is discarded (the sign bit is discarded) and the least significant bit is filled with 0. For example, after the binary number 1010101010101010 (int16\_t type) is arithmetically shifted left by 1 bit, the result is 0101010101010100; after it is arithmetically shifted left by 3 bits, the result is 0101010101010000.

$$
dstReg_i = srcReg0_i \ll srcReg1_i
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S, typename V>
__simd_callee__ inline void ShiftLeft(S& dstReg, S& srcReg0, V& srcReg1, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the source operand. |
| U | Data type of the left shift parameter. |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration type. Selects the MERGING mode or the ZEROING mode.<br>• In ZEROING mode, elements not filtered by the mask are set to zero in **dstReg**.<br>• The MERGING mode is currently not supported. |
| S | `RegTensor` type of the destination operand and the source operand, for example, `RegTensor<uint32_t>`. It is automatically deduced by the compiler and does not need to be specified by the user. |
| V | `RegTensor` type of the left shift parameter, for example, `RegTensor<int32_t>`. It is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg0 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md).<br>The data type of the source operand must be consistent with that of the destination operand. |
| srcReg1 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md).<br>Setting it to a negative number is not supported; the behavior is undefined for negative numbers. |
| mask | Input | Valid indication of the element operation on the source operand. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Return Value<a name="section640mcpsimp"></a>

None

## Data Type

**Table 3**  Data type support

| dstReg   | srcReg0  | srcReg1 |
| :------- | :------- | :------ |
| int8_t   | int8_t   | int8_t  |
| uint8_t  | uint8_t  | int8_t  |
| int16_t  | int16_t  | int16_t |
| uint16_t | uint16_t | int16_t |
| int32_t  | int32_t  | int32_t |
| uint32_t | uint32_t | int32_t |
| int64_t  | int64_t  | int64_t |
| uint64_t | uint64_t | int64_t |

## Constraints<a name="section633mcpsimp"></a>

- For a logical shift (unsigned data type), if the shift amount (**srcReg1**) is greater than the data type bit width, the output is 0.
- For an arithmetic shift (signed data type), if the shift amount is greater than the data type bit width, the output is 0.
- **srcReg1** does not support being set to a negative number; the behavior for a negative number is undefined.

## Example<a name="section642mcpsimp"></a>

```cpp
// Example: Shift the data in src0Addr left by the value of the corresponding element in src1Addr, and write the result to dstAddr.
// Input: src0Addr points to the data to be shifted (for example, uint16_t type [1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 9, 3, 4, 6, 5, 7, ...]).
//       src1Addr points to the number of bits to shift each element left (for example, int16_t type [1, 2, 3, 4, 1, 2, 3, 4, 2, 3, 1, 2, 3, 1, 2, 1, ...]).
// Output: dstAddr stores the left shift result (for example, [2, 8, 24, 64, 10, 24, 56, 128, 4, 16, 18, 12, 32, 12, 20, 14, ...], that is, 1<<1=2, 2<<2=8, 3<<3=24, 4<<4=64...).
template<typename T, typename U>
__simd_vf__ inline void ShiftLeftVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ U* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;  // Source operand, the data to be shifted.
    AscendC::Reg::RegTensor<U> srcReg1;  // Source operand, the number of bits to shift each element left.
    AscendC::Reg::RegTensor<T> dstReg;   // Destination operand, storing the left shift result.
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);  // Update the mask based on the actual number of elements.
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);  // Load the data to be shifted.
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);  // Load the number of bits to shift left for each element.
        AscendC::Reg::ShiftLeft(dstReg, srcReg0, srcReg1, mask);  // Perform the left shift: dstReg[i] = srcReg0[i] << srcReg1[i].
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);  // Store the left shift result back to dstAddr.
    }
}
```
