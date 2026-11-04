# MoveMask<a name="ZH-CN_TOPIC_0000001985457933"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:30:08.428Z pushedAt=2026-10-26T07:41:44.550Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/MaskReg_compute/MoveMask_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"`.

Reads the Mask value from the mask register {MASK1, MASK0} set by [SetVectorMask](../../memory_vector_compute/mask_operations/SetVectorMask.md), converts it according to the data format corresponding to the template parameter **T**, and writes it to the return value **MaskReg**. The specific implementation is as follows:

For the b16 type, the complete 128-bit {MASK1, MASK0} is read, and each bit is copied as 2 bits. For the b32 type, the 64-bit MASK0 is read, and each bit is copied as 4 bits.

![Comparison of the A2/A3 and 950 Mask mechanisms](../../../../figures/reg_move_mask.png)

For a comparison of the Mask and MaskReg mechanisms, see [Comparison of the A2/A3 and 950 Mask mechanisms](../register_data_types/MaskReg.md).

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T> 
__simd_callee__ inline MaskReg MoveMask()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type, which determines the format for writing the mask value:<br>&bull; For the b16 type, the complete 128-bit {MASK1, MASK0} data is read, and each bit is copied as 2 bits and written to the **MaskReg** return value.<br>&bull; For the b32 type, the 64-bit MASK0 data is read, and each bit is copied as 4 bits and written to the **MaskReg** return value. |

## Data Types

Supported data types: **b16**, **b32**.

## Return Value<a name="section640mcpsimp"></a>

The **MaskReg** read from the special-purpose register.

## Constraints<a name="section633mcpsimp"></a>

- This API is a compatibility API. It is recommended to preferentially use the [CreateMask API](../register_data_types/MaskReg.md) and [UpdateMask API](../register_data_types/MaskReg.md) for MaskReg computation.
- Before use, call [SetMaskCount](../../memory_vector_compute/mask_operations/SetMaskCount.md)/[SetMaskNorm](../../memory_vector_compute/mask_operations/SetMaskNorm.md) to manually set the mask mode, and call [SetVectorMask](../../memory_vector_compute/mask_operations/SetVectorMask.md) to set the mask register SPR{MASK1, MASK0}.

## Example<a name="section932512912207"></a>

```cpp
template <typename T>
__simd_vf__ inline void MoveMaskVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg addMask = AscendC::Reg::MoveMask<T>();
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Adds(srcReg, srcReg, 0, addMask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
    }
}
```
