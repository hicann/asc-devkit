# MaskReg Unaligned Store (StoreUnAlign)<a name="ZH-CN_TOPIC_0000002554449396"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T22:37:32.219Z -->

## Applicable Products

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

## Description

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

**StoreUnAlign** transfers data continuously from [MaskReg](../register_data_types/MaskReg.md) to a non-aligned Unified Buffer (UB). It uses the non-aligned register **UnalignRegForStore** as a temporary buffer to hold data that crosses alignment boundaries, thereby enabling efficient continuous non-aligned data transfer.

## Function Prototype

```cpp
template <typename T>
__simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, MaskReg& mask, UnalignRegForStore& ureg)

template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
__simd_callee__ inline void StoreUnAlignPost(__ubuf__ T*& dstAddr, UnalignRegForStore& ureg, int32_t postUpdateStride)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
|-----|-----|
| T | Data type of the destination operand. For supported data types, see [data type](#data-type). |
| postMode | Controls whether to enable automatic UB address update (post update). The type is **PostLiteral**. For details, see [PostLiteral](../aux_data_types/PostLiteral.md). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstAddr | Input/Output | Destination operand, the UB start address, which does not require 32-byte alignment. For the b16 data type, 16-byte alignment is required; for the b32 data type, 8-byte alignment is required. |
| mask | Input | Source operand. The type is [MaskReg](../register_data_types/MaskReg.md). |
| ureg | Input/Output | Unaligned register of the **UnalignRegForStore** type, used to store data that is not 32B-aligned. The register size is 32 bytes. |
| postUpdateStride | Input | Address update stride, of the **uint32_t** type, in elements. Depending on the value of **postMode**, there are two cases:<br>&bull; **POST_MODE_NORMAL**: Not supported.<br>&bull; **POST_MODE_UPDATE**: The actual UB start address for the transfer is **dstAddr**. After the transfer, the address update **dstAddr += postUpdateStride** is performed. |

## Data Type

The destination operand supports the following data types: b16 and b32.

## Return Value

None

## Constraints

- When the data type is **b16**, the address in the **Unified Buffer** must be 16-byte aligned. After the transfer is complete, the UB address is updated by a 16B offset, and no manual address update is required.
- When the data type is **b32**, the address in the **Unified Buffer** must be 8-byte aligned. After the transfer is complete, the UB address is updated by an 8B offset, and no manual address update is required.

## Key Features

**Principle of Unaligned Data Store**

According to [MaskReg Principle](../register_data_types/MaskReg.md), when the operand type is b16, each element corresponds to 2 bits of MaskReg, and only the least significant bit of the 2 bits is valid; when the operand type is b32, each element corresponds to 4 bits of MaskReg, and only the least significant bit of the 4 bits is valid.

Store the source register MaskReg (32B) to the unaligned UB:

- When the data type at the UB address is b16, the hardware instruction extracts the least significant bit (LSB) from every 2 bits of data, packs the 32B data in MaskReg into 16B, and writes it to the UB. After the transfer is complete, the UB address is updated by a 16B offset.
- When the data type at the UB address is b32, the hardware instruction extracts the least significant bit (LSB) from every 4 bits of data, packs the 32B data in MaskReg into 8B, and writes it to the UB. After the transfer is complete, the UB address is updated by an 8B offset.

## Example

```cpp
template <typename T>
__simd_vf__ inline void StoreUnAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t offset, uint16_t repeatTimes)
{
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::UnalignRegForStore ureg;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign(mask, srcAddr + i * oneRepeatSize);
        // MaskReg unaligned store automatically performs address offset.
        // When the data type is b16, 32 bytes are packed into 16 bytes, with an offset of 16 bytes.
        // When the data type is b32, 32 bytes are packed into 8 bytes, with an offset of 8 bytes.
        AscendC::Reg::StoreUnAlign(dstAddr, mask, ureg);
    }
    AscendC::Reg::StoreUnAlignPost(dstAddr, ureg, 0);
}
```

