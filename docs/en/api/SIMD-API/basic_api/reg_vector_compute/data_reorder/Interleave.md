# Interleave<a name="ZH-CN_TOPIC_0000001956130785"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:15:07.504Z pushedAt=2026-10-26T07:41:44.519Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/data_reorder/Interleave_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_interleave_intf.h"`.

Given the source operand registers **srcReg0** and **srcReg1**, interleave the elements in **srcReg0** and **srcReg1** and store them into the result operands **dstReg0** and **dstReg1**. The interleaving arrangement is shown in the following figure, where each square represents one element:

![](../../../../figures/reg_reginterleave.png)

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U>
__simd_callee__ inline void Interleave(U& dstReg0, U& dstReg1, U& srcReg0, U& srcReg1)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the destination operand and source operand. For supported data types, see [Data Type](#data-type). |
| U | RegTensor type of the source operand and destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg0<br>dstReg1 | Output | Destination operand.<br>Type: [RegTensor](../register_data_types/RegTensor.md). |
| srcReg0<br>srcReg1 | Input | Source operand.<br>Type: [RegTensor](../register_data_types/RegTensor.md).<br>The data type of the source operand must be consistent with that of the destination operand. |

## Data Type

The data types of the destination operands and source operands must be consistent. Supported data types are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.

## Return Value

None

## Constraints<a name="section177921451558"></a>

- The data types of **srcReg0**, **srcReg1**, **dstReg0**, and **dstReg1** must be consistent.
- **srcReg0** and **srcReg1** can be the same **RegTensor**.
- **dstReg0** and **dstReg1** cannot be the same **RegTensor**.
- The source operands and destination operands are allowed to be the same **RegTensor**, for example, Interleave(srcReg0, srcReg1, srcReg0, srcReg1).
- When the data type is **b64**, only **RegTraitNumTwo** is supported.

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void InterleaveVF(__ubuf__ T* dst0Addr, __ubuf__ T* dst1Addr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg0;
    AscendC::Reg::RegTensor<T> dstReg1;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Interleave(dstReg0, dstReg1, srcReg0, srcReg1);
        AscendC::Reg::StoreAlign(dst0Addr + i * oneRepeatSize, dstReg0, mask);
        AscendC::Reg::StoreAlign(dst1Addr + i * oneRepeatSize, dstReg1, mask);
    }
}
```
