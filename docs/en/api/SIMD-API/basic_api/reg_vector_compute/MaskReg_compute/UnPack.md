# UnPack<a name="ZH-CN_TOPIC_0000001985577793"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T22:28:40.994Z -->

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

Based on the selected low-order mode or high-order mode, expands the lower half or upper half of the input [MaskReg](../register_data_types/MaskReg.md) into the output MaskReg. The expansion is performed by expanding each bit into 2 bits, with the high-order bits set to zero.

## Prototype<a name="section620mcpsimp"></a>

```
template <HighLowPart part = HighLowPart::LOWEST> 
__simd_callee__ inline void UnPack(MaskReg& dst, MaskReg& src);
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>part</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Enumeration type, low-part mode or high-part mode.</p>
<a name="ul1188693111513"></a><a name="ul1188693111513"></a><ul id="ul1188693111513"><li>**LOWEST**: low-part mode;</li><li>**HIGHEST**: high-part mode.</li></ul></td>
</tr>
</tbody>
</table>

**Table 2** Parameter description

<a name="table299395481215"></a>
<table><thead align="left"><tr id="row1399413543129"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p14994175441217"><a name="p14994175441217"></a><a name="p14994175441217"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p999420547126"><a name="p999420547126"></a><a name="p999420547126"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1399415546125"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p12994155416123"><a name="p12994155416123"></a><a name="p12994155416123"></a>**dst**</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p159941254191217"><a name="p159941254191217"></a><a name="p159941254191217"></a>Destination operand.</p></td>
</tr>
<tr id="row49941154111217"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p6994354131210"><a name="p6994354131210"></a><a name="p6994354131210"></a>**src**</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1399425410128"><a name="p1399425410128"></a><a name="p1399425410128"></a>Source operand.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section932512912207"></a>

```
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

