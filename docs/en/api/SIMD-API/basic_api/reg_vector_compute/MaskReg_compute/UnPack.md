# UnPack<a name="ZH-CN_TOPIC_0000001985577793"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:32:03.823Z pushedAt=2026-10-26T07:41:44.554Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/MaskReg_compute/UnPack_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"`.

Depending on the selected low bit mode or high bit mode, unpacks the low half or high half of the source operand **src** into the destination operand **dst**. The unpacking method is as follows: each bit is expanded to 2 bits, with the high bit set to zero.

![UnPack diagram](../../../../figures/reg_unpack.png)

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <HighLowPart part = HighLowPart::LOWEST> 
__simd_callee__ inline void UnPack(MaskReg& dst, MaskReg& src)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| part | Enumeration type, low bit mode or high bit mode.<br>&bull; **LOWEST**: low bit mode;<br>&bull; **HIGHEST**: high bit mode. |

**Table 2** Parameter description

| Parameter | Description |
| --- | --- |
| dst | Destination operand, of type [MaskReg](../register_data_types/MaskReg.md). |
| src | Source operand, of type [MaskReg](../register_data_types/MaskReg.md). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section932512912207"></a>

```cpp
template <typename T>
__simd_vf__ inline void UnpackVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg maskFull = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    AscendC::Reg::MaskReg mask0;
    AscendC::Reg::MaskReg mask1;
    AscendC::Reg::UnPack<AscendC::Reg::HighLowPart::LOWEST>(mask0, maskFull);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask1 = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Adds(srcReg, srcReg, 0, mask0);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask1);
    }
}
```
