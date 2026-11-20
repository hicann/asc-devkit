# DeInterleave<a name="ZH-CN_TOPIC_0000001929091478"></a>

<!-- md-trans-meta sourceCommit=af3f6adafe94d121d34283378bfbe7e348767ea3 translatedAt=2026-11-05T02:54:59.907Z pushedAt=2026-11-18T01:25:30.872Z nl=2 -->

## Applicable Products

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
<!-- npu="9050" id8 -->
- Kirin 9050 processor products: Supported
<!-- end id8 -->
<!-- npu="kirin1" id9 -->
- KirinDev0001 processor products: Supported
<!-- end id9 -->
<!-- npu="kirin2" id10 -->
- KirinDev0002 processor products: Supported
<!-- end id10 -->
<!-- npu="kirin3" id11 -->
- KirinDev0003 processor products: Supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/data_reorder/DeInterleave_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_interleave_intf.h"`.

Given the source operand registers srcReg0 and srcReg1, deinterleave the elements in srcReg0 and srcReg1 and store them into the destination operands dstReg0 and dstReg1. The deinterleaving arrangement is shown in the following figure, where each square represents one element:

![](../../../../figures/reg_regdeinterleave.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U>
__simd_callee__ inline void DeInterleave(U& dstReg0, U& dstReg1, U& srcReg0, U& srcReg1)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the destination operand and source operand. For supported data types, see [Data Type](#data-type). |
| U | RegTensor type of the source operands and destination operands, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg0<br>dstReg1 | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg0<br>srcReg1 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md).<br>The data type of the source operand must be consistent with the destination operand. |

## Data Type

<!-- npu="950" id12 -->
For Ascend 950PR&950DT products, the data types of the destination operand and the source operand must be consistent. Supported data types: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.
<!-- end id12 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/data_reorder/DeInterleave_res.md#id2 -->

## Return Value

None

## Constraints<a name="section1240634052310"></a>

- The data types of srcReg0, srcReg1, dstReg0, and dstReg1 must be the same.
- srcReg0 and srcReg1 can be the same RegTensor.
- dstReg0 and dstReg1 cannot be the same RegTensor.
- The source operands and destination operands are allowed to be the same RegTensor, for example, DeInterleave(srcReg0, srcReg1, srcReg0, srcReg1).
- When the data type is b64, only RegTraitNumTwo is supported.

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void DeInterLeaveVF(__ubuf__ T* dst0Addr, __ubuf__ T* dst1Addr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg0;
    AscendC::Reg::RegTensor<T> dstReg1;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::DeInterleave(dstReg0, dstReg1, srcReg0, srcReg1);
        AscendC::Reg::StoreAlign(dst0Addr + i * oneRepeatSize, dstReg0, mask);
        AscendC::Reg::StoreAlign(dst1Addr + i * oneRepeatSize, dstReg1, mask);
    }
}
```
