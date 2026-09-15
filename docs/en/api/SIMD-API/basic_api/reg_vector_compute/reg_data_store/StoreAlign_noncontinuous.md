# Non-contiguous Aligned Store (StoreAlign)<a name="ZH-CN_TOPIC_0000001956110781"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T22:34:39.546Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not Supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

**StoreAlign** moves data from [RegTensor](../register_data_types/RegTensor.md) to the Unified Buffer (UB) in a non-contiguous manner. It provides two types of APIs:

- Ordinary move API: After a move is completed, the UB address is not updated automatically, so the address must be updated manually in each iteration.
- PostUpdate extended move API: After a move is completed, the UB address is updated automatically, so the address does not need to be updated manually in each iteration.

## Prototype<a name="section620mcpsimp"></a>

- Normal transfer API

    ```cpp
    template <typename T = DefaultType, DataCopyMode dataMode, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, uint32_t dataBlockStride, MaskReg& mask)
    ```

- **PostUpdate** extended transfer API

    ```cpp
    template <typename T = DefaultType, DataCopyMode dataMode, PostLiteral postMode, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T*& dstAddr, U& srcReg, uint32_t dataBlockStride, uint32_t repeatStride, MaskReg& mask)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template Parameter Description

| Parameter | Description |
|-----|-----|
| T | Data type of the destination operand. For supported data types, see [Data Type](#data-type). |
| dataMode | Copy mode in non-contiguous alignment scenarios, of the **DataCopyMode** type. For details, see [Table 3 DataCopyMode Parameter Description](#table-3-datacopymode-parameter-description). |
| postMode | Controls whether to enable the UB address auto-update feature (post update), of the **PostLiteral** type. For details, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| U | **RegTensor** type of the source operand, for example, **RegTensor**\<half>. It is automatically deduced by the compiler and does not need to be manually specified by the user. |

**Table 2** Parameter Description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstAddr | Input/Output | Destination operand, UB start address, which must be 32-byte aligned. |
| srcReg | Input | Source operand, of the [RegTensor](../register_data_types/RegTensor.md) type. |
| dataBlockStride | Input | A single instruction copies 8 **DataBlock**s at a time. This parameter indicates the interval between adjacent **DataBlock**s (the interval between the head of the preceding **DataBlock** and the head of the following **DataBlock**), in **DataBlock** (32B). |
| repeatStride | Input | Address update step, in **DataBlock** (32B). repeatStride * sizeof(T) must be 32-byte aligned.<br>&bull; **POST_MODE_NORMAL**: The actual UB start address for the copy is dstAddr + repeatStride * 32B / sizeof(T), and the address is not updated after the copy.<br>&bull; **POST_MODE_UPDATE**: The actual UB start address for the copy is dstAddr, and after the copy, the address update dstAddr += repeatStride * 32B / sizeof(T) is performed. |
| mask | Input | Valid indication of the source operand element operation. For details, see [MaskReg](../register_data_types/MaskReg.md).<br>Note: When all elements in a **DataBlock** are filtered out as invalid by the mask, no error is reported even if the UB is out of bounds. |

<a id="table-3-datacopymode-parameter-description"></a>
**Table 3** DataCopyMode Parameter Description

| Parameter | Description |
|-----|-----|
| DATA_BLOCK_COPY | **DataBlock** copy mode, that is, copying in units of **DataBlock** (32B). |

## Data Type

The data types of the destination operand and the source operand must be consistent. The supported data types are **b8**, **b16**, and **b32**.

## Return Value<a name="section1575141714439"></a>

None

## Constraints<a name="section11585101304320"></a>

- The address in the Unified Buffer must be 32-byte aligned.
- The **regTrait** template parameter of **RegTensor** supports only **RegTraitNumOne**.
- repeatStride * 32B / sizeof(T) must be 32-byte aligned.

## Example<a name="section642mcpsimp"></a>

```cpp
template <typename T>
__simd_vf__ inline void Compute(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign<T, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(srcReg, srcAddr, 1, i * 8, mask);
        AscendC::Reg::StoreAlign<T, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstAddr, srcReg, 1, i * 8, mask);
    }
}
```

