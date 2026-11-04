# ShiftRights<a name="ZH-CN_TOPIC_0000001956827109"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:24:54.009Z pushedAt=2026-10-26T07:41:44.540Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/logical_compute/ShiftRights_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_vec_binary_scalar_intf.h"`.

The `ShiftRights` instruction performs a right shift on the source operand srcReg by scalarValue according to the mask, and writes the result to the destination operand dstReg. Depending on the data type of the source operand, the right shift operation falls into the following two cases:

- **The data type is an unsigned type: a logical right shift is performed.**

  A logical right shift moves the entire binary number to the right by the specified number of bits. For each bit shifted, the least significant bit is discarded and the most significant bit is filled with 0. For example, after a logical right shift of 1 bit on the binary number 1010101010101010 (uint16\_t type), the result is 0101010101010101.
- **The data type is a signed type: an arithmetic right shift is performed.**

  An arithmetic right shift moves the entire binary number to the right by the specified number of bits. For each bit shifted, the least significant bit is discarded and the most significant bit is filled with a copy of the sign bit. For example, after an arithmetic right shift of 1 bit on the binary number 1010101010101010 (int16\_t type), the result is 1101010101010101; after an arithmetic right shift of 3 bits, the result is 1111010101010101.

$$
dstReg_i = srcReg_i \gg scalarValue
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void ShiftRights(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the vector destination operand and source operand. |
| U | Data type of the scalar source operand. |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration type. Selects the MERGING mode or the ZEROING mode.<br>• In ZEROING mode, elements not filtered by the mask are set to zero in **dstReg**.<br>• The MERGING mode is currently not supported. |
| S | `RegTensor` type of the vector destination operand and source operand, for example, `RegTensor<uint16_t>`. It is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md).<br>The data type must be consistent with that of the destination operand. |
| scalarValue | Input | Source operand.<br>The type is a scalar.<br>Setting it to a negative number is not supported, and the behavior is undefined for negative numbers. |
| mask | Input | Indicates the validity of the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Return Value<a name="section640mcpsimp"></a>

None

## Data Type

**Table 3** Data type support

| dstReg   | srcReg   | scalarValue |
| :------- | :------- | :---------- |
| int8_t   | int8_t   | int16_t     |
| uint8_t  | uint8_t  | int16_t     |
| int16_t  | int16_t  | int16_t     |
| uint16_t | uint16_t | int16_t     |
| int32_t  | int32_t  | int16_t     |
| uint32_t | uint32_t | int16_t     |
| int64_t  | int64_t  | int16_t     |
| uint64_t | uint64_t | int16_t     |

## Constraints<a name="section633mcpsimp"></a>

- For logical shift (unsigned data type), if the shift amount (**scalarValue**) is greater than the data type bit width, the output is 0.
- For arithmetic shift (signed data type), if **srcReg** is less than 0 and the shift amount is greater than the data type bit width, the output is -1; if **srcReg** is greater than 0 and the shift amount is greater than the data type bit width, the output is 0.
- Setting **scalarValue** to a negative number is not supported, and the behavior for negative numbers is undefined.

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T, typename U>
__simd_vf__ inline void ShiftRightsVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, U scalarValue, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::ShiftRights(dstReg, srcReg, scalarValue, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
