# ClearSpr<a name="ZH-CN_TOPIC_0000002186694092"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:13:05.088Z pushedAt=2026-10-26T07:41:44.611Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/system_variable_access/ClearSpr_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_gather_mask_intf.h"`.

This API clears the specified special-purpose register (SpecialPurposeReg). The target special-purpose register is specified by the template parameter **spr**. After the call, the value of the register is set to 0. For the currently supported special-purpose registers, see [Table SpecialPurposeReg template parameter description](#table2).

The AR special-purpose register is typically used together with [Squeeze](../compare_and_select/Squeeze.md) and scenario 3 of [continuous unaligned store-out](../reg_data_store/StoreUnAlign_continuous.md) (no explicit offset needs to be passed). When the template parameter `store` of Squeeze is set to `GatherMaskMode::STORE_REG`, the total number of bytes of valid elements is stored in the AR register and used as the address offset for continuous unaligned store-out. Before the first call to the continuous unaligned store-out operation, call this API to clear the AR register.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <SpecialPurposeReg spr>
__simd_callee__ inline void ClearSpr()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter | Description |
| ------ | ------ |
| spr | Special-purpose register, of the **SpecialPurposeReg** enum class type. For specific values, see [Table SpecialPurposeReg template parameter description](#table2). |

**Table 2**  SpecialPurposeReg template parameter description<a id="table2"></a>

| Value | Meaning |
| ------ | ------ |
| AR | A special address register, usually used together with the [Squeeze](../compare_and_select/Squeeze.md)Reg vector calculation API. The [Squeeze](../compare_and_select/Squeeze.md)Reg vector calculation API stores the total number of bytes of valid elements into the AR register. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

This API can only be called inside a VF function, with the namespace **AscendC::Reg** and the function specifier **__simd_callee__**. If it needs to be called outside a VF, the namespace is **AscendC** and the function specifier is **__aicore__**. For details, see [ClearSpr](../../special_register_access/ClearSpr.md).

## Example<a name="section642mcpsimp"></a>

In the following example, the `GatherMaskMode::STORE_REG` mode of the [Squeeze](../../reg_vector_compute/compare_and_select/Squeeze.md) Reg vector calculation API stores the total number of bytes of valid elements into the AR register. Before the for loop in the macro function starts, the AR register is cleared by using ClearSpr.

```cpp
template <typename T>
__simd_vf__ inline void SqueezeVF(__ubuf__ T* xAddr, __ubuf__ T* yAddr, uint32_t repeatTimes, uint32_t oneRepeatSize)
{
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::M4>();
    AscendC::Reg::RegTensor<T> xReg;
    AscendC::Reg::RegTensor<T> yReg;
    AscendC::Reg::UnalignRegForStore ureg;
    // In STORE_REG mode, Squeeze accumulates the total number of bytes of valid elements into the AR register. Clear it first to initialize the move-out offset.
    AscendC::Reg::ClearSpr<AscendC::SpecialPurposeReg::AR>();
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(xReg, xAddr, oneRepeatSize);
        AscendC::Reg::Squeeze<T, AscendC::Reg::GatherMaskMode::STORE_REG>(yReg, xReg, mask);
        AscendC::Reg::StoreUnAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(yAddr, yReg, ureg);
    }
    AscendC::Reg::StoreUnAlignPost(yAddr, ureg);
}
```
