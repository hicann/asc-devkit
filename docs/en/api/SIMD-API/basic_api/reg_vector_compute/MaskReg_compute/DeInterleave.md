# DeInterleave<a name="ZH-CN_TOPIC_0000001985577789"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-05T03:08:14.829Z pushedAt=2026-11-18T01:25:30.898Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/MaskReg_compute/DeInterleave_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"`.

Deinterleaves the elements in source operands src0 and src1 and stores them into destination operands dst0 and dst1. The deinterleaving arrangement is shown in the following figure, where each square represents a group:

![DeInterleave diagram](../../../../figures/reg_deinterleave.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__simd_callee__ inline void DeInterleave(MaskReg& dst0, MaskReg& dst1, MaskReg& src0, MaskReg& src1)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type supported by MaskReg, which determines the bit width of deinterleaving. For example, for the uint32_t type, deinterleaving is performed in groups of 4 bits. |

**Table 2** Parameters

| Parameter | Description |
| --- | --- |
| dst0 | Destination operand. |
| dst1 | Destination operand. |
| src0 | Source operand. |
| src1 | Source operand. |

## Data Types

The supported data types are b8, b16, and b32.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Examples<a name="section932512912207"></a>

```cpp
template <typename T>
__simd_vf__ inline void InterleaveDeInterleaveVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg maskFull = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    AscendC::Reg::MaskReg maskM3 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::M3>();
    AscendC::Reg::MaskReg newMask0;
    AscendC::Reg::MaskReg newMask1;
    AscendC::Reg::Interleave<T>(newMask0, newMask1, maskFull, maskM3);
    AscendC::Reg::DeInterleave<T>(newMask0, newMask1, newMask0, newMask1);
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Adds(srcReg, srcReg, 0, newMask0);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
    }
}
```
