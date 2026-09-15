# ClearSpr<a name="ZH-CN_TOPIC_0000002186694092"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T22:23:12.928Z -->

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

Clears the specified special register.

## Prototype<a name="section620mcpsimp"></a>

```
template <SpecialPurposeReg spr>
__simd_callee__ inline void ClearSpr()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.27%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.73%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.27%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>spr</p></td>
<td class="cellrowborder" valign="top" width="81.73%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Special register, of the **SpecialPurposeReg** enum class type.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

This API can be called only within a VF function, with the namespace **AscendC::Reg** and the function specifier **__simd_callee__**. To call it outside a VF function, use the namespace **AscendC** and the function specifier **__aicore__**. For details, see [ClearSpr](../../special_register_access/ClearSpr.md).

## Example<a name="section642mcpsimp"></a>

In the following example, the **Gather Reg** vector computation API stores the total number of bytes of valid elements into the **AR** register. Before the for loop in the macro function starts, **ClearSpr** is used to clear the **AR** register.

```cpp
template<typename T, typename U>
__simd_vf__ inline void VFDemo(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ U* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0, dstReg;
    AscendC::Reg::RegTensor<U> srcReg1;
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::LoadAlign(srcReg1, src1Addr);
    AscendC::Reg::ClearSpr<AscendC::SpecialPurposeReg::AR>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::Gather(dstReg, srcReg0, srcReg1);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
