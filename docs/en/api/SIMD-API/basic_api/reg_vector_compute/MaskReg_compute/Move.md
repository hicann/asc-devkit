# Move<a name="ZH-CN_TOPIC_0000001985577785"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:29:50.342Z pushedAt=2026-10-26T07:41:44.548Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/MaskReg_compute/Move_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"`.

Copies the elements in **src** to the corresponding positions in **dst**. If an input **mask** is provided, only the valid elements selected by the mask are copied, and invalid elements are filled with 0.

To copy a RegTensor to a MaskReg, see [MaskGenWithRegTensor](../reg_data_load/MaskGenWithRegTensor_MaskReg.md).

## Prototype<a name="section620mcpsimp"></a>

- With input mask

    ```cpp
    __simd_callee__ inline void Move(MaskReg& dst, MaskReg& src, MaskReg& mask)
    ```

- Without input mask

    ```cpp
    __simd_callee__ inline void Move(MaskReg& dst, MaskReg& src)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Description |
| --- | --- |
| dst | Destination operand, type is [MaskReg](../register_data_types/MaskReg.md). |
| src | Source operand, type is [MaskReg](../register_data_types/MaskReg.md). |
| mask | Indicates which bits are valid during the copy, type is [MaskReg](../register_data_types/MaskReg.md). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section932512912207"></a>

```cpp
template <typename T>
__simd_vf__ inline void MoveVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg src = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
    AscendC::Reg::MaskReg dst;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Move(dst, src, mask);
        AscendC::Reg::Adds(srcReg, srcReg, 0, dst);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
    }
}
```
