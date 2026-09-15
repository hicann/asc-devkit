# Gather<a name="ZH-CN_TOPIC_0000001956119717"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T22:38:46.203Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
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

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

This instruction gathers elements from the source operand into the destination operand **dstReg** according to the index value **index**. The gathering process is shown in Figure 1:

**Figure 1** Gather function description

![Figure 1 Gather function description](../../../../figures/reg_gather_ub2reg.png)  


## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T0 = DefaultType, typename T1, typename T2 = DefaultType, typename T3, typename T4>
__simd_callee__ inline void Gather(T3& dstReg, __ubuf__ T1* baseAddr, T4& index, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|-----|-----|
| T0 | Data type of the destination operand. For supported data types, see [Data Type](#data-type). |
| T1 | Data type of the source operand. For supported data types, see [Data Type](#data-type). |
| T2 | Data type of the index value. For supported data types, see [Data Type](#data-type). |
| T3 | RegTensor type of the destination operand, for example, RegTensor\<half>. It is automatically derived by the compiler and does not need to be specified by the user. |
| T4 | RegTensor type of the index value, for example, RegTensor\<uint16_t>. It is automatically derived by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstReg | Output | Destination operand, of the [RegTensor](../register_data_types/RegTensor.md) type. |
| baseAddr | Input | Source operand, the base address in the UB, which must be 32-byte aligned. |
| index | Input | Index value, the position of each element in **dstReg** relative to **baseAddr** in the UB, in elements. The type is [RegTensor](../register_data_types/RegTensor.md). Values in **index** can be repeated.<br>Example: **baseAddr**: [elem0, elem1, elem2, elem3, elem4, elem5, elem6, elem7, ...]. <br>The index position of each element relative to **baseAddr** is: [0, 1, 2, 3, 4, 5, 6, 7, ...]. |
| mask | Input | Valid indication for the operation on source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

**Table 3** Data type mapping for Gather operands

| destination operand | source operand | index value |
|-----|-----|-----|
| int16_t | int8_t | uint16_t |
| int16_t | int16_t | uint16_t |
| uint16_t | uint8_t | uint16_t |
| uint16_t | uint16_t | uint16_t |
| half | half | uint16_t |
| bfloat16_t | bfloat16_t | uint16_t |
| int32_t | int32_t | uint32_t |
| uint32_t | uint32_t | uint32_t |
| float | float | uint32_t |
| int64_t | int64_t | uint32_t |
| int64_t | int64_t | uint64_t |
| uint64_t | uint64_t | uint32_t |
| uint64_t | uint64_t | uint64_t |

## Return Value

None

## Constraints<a name="section177921451558"></a>

- The address in the Unified Buffer must be 32-byte aligned.
- The data corresponding to the **index** index value must be within the valid address range of the UB.
- When T0 is of the b16 type and T1 is of the b8 data type, the lower 8 bits of the destination operand are the same as those of the source operand, and the upper 8 bits are automatically padded with 0. For example, when the source operand T1 data type is int8_t:

    40=0b00101000 -> 0b0000000000101000, which equals 40 after being extended to 16 bits;

    -40=0b11011000 -> 0b0000000011011000, which equals 216 after being extended to 16 bits.

- When the source operand data type T1 is B64, T0, T1, T2, T3, and T4 support only the following combinations:

    | T0 | T1 | T2 | T3 (automatic derivation) | T4 (automatic derivation) | Remarks |
    |-----|-----|-----|-----|-----|-----|
    | int64_t | int64_t | uint32_t | RegTensor<int64_t, RegTraitNumOne> | RegTensor<uint32_t> | The first 32 index values are valid. |
    | int64_t | int64_t | uint32_t | RegTensor<int64_t, RegTraitNumTwo> | RegTensor<uint32_t> | - |
    | int64_t | int64_t | uint64_t | RegTensor<int64_t, RegTraitNumOne> | RegTensor<uint64_t, RegTraitNumOne> | - |
    | int64_t | int64_t | uint64_t | RegTensor<int64_t, RegTraitNumOne> | RegTensor<uint64_t, RegTraitNumTwo> | The first 32 index values are valid. |
    | int64_t | int64_t | uint64_t | RegTensor<int64_t, RegTraitNumTwo> | RegTensor<uint64_t, RegTraitNumTwo> | - |
    | uint64_t | uint64_t | uint32_t | RegTensor<uint64_t, RegTraitNumOne> | RegTensor<uint32_t> | The first 32 index values are valid. |
    | uint64_t | uint64_t | uint32_t | RegTensor<uint64_t, RegTraitNumTwo> | RegTensor<uint32_t> | - |
    | uint64_t | uint64_t | uint64_t | RegTensor<uint64_t, RegTraitNumOne> | RegTensor<uint64_t, RegTraitNumOne> | - |
    | uint64_t | uint64_t | uint64_t | RegTensor<uint64_t, RegTraitNumOne> | RegTensor<uint64_t, RegTraitNumTwo> | The first 32 index values are valid. |
    | uint64_t | uint64_t | uint64_t | RegTensor<uint64_t, RegTraitNumTwo> | RegTensor<uint64_t, RegTraitNumTwo> | - |

## Example<a name="section642mcpsimp"></a>

```cpp
template <typename T, typename U>
__simd_vf__ inline void GatherVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ U* indexAddr, uint32_t count, uint16_t oneRepeatSize)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::RegTensor<U> indexReg;
    AscendC::Reg::MaskReg mask;
    uint16_t repeatTimes = AscendC::CeilDivision(count, oneRepeatSize);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(indexReg, indexAddr + i * oneRepeatSize);
        AscendC::Reg::Gather(dstReg, srcAddr, indexReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

