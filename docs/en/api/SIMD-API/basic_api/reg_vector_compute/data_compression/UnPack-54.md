# UnPack<a name="ZH-CN_TOPIC_0000002047032737"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T22:18:28.056Z -->

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


## Description<a name="section1922292299"></a>

For unsigned integers, the elements in the lower half or upper half of the source operand **srcReg** are written to **dstReg** with their bit width expanded by padding the high bits with zeros. For signed integers, the elements in the lower half or upper half of the source operand **srcReg** are written to **dstReg** with their bit width expanded while preserving the sign bit.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, typename U = DefaultType, HighLowPart part = HighLowPart::LOWEST, typename S, typename V>
__simd_callee__ inline void UnPack(S& dstReg, V& srcReg)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

<a name="table104985541254"></a>
<table><thead align="left"><tr id="row1049811545512"><th class="cellrowborder" valign="top" width="17.9%" id="mcps1.2.3.1.1"><p id="p3498854954"><a name="p3498854954"></a><a name="p3498854954"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="82.1%" id="mcps1.2.3.1.2"><p id="p11498954655"><a name="p11498954655"></a><a name="p11498954655"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row11313124114714"><td class="cellrowborder" valign="top" width="17.9%" headers="mcps1.2.3.1.1 "><p id="p9314741179"><a name="p9314741179"></a><a name="p9314741179"></a>**T**</p></td>
<td class="cellrowborder" valign="top" width="82.1%" headers="mcps1.2.3.1.2 "><p id="p831413412717"><a name="p831413412717"></a><a name="p831413412717"></a>Destination operand data type.</p>
<p id="p172481539145717"><a name="p172481539145717"></a><a name="p172481539145717"></a><span id="ph5248539195712"><a name="ph5248539195712"></a><a name="ph5248539195712"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: int16_t/uint16_t/int32_t/uint32_t/int64_t/uint64_t</p>
<p id="p20454143018480"><a name="p20454143018480"></a><a name="p20454143018480"></a>For the data type constraints of the source operand and destination operand, see <a href="#table113383813355">Table 3</a>.</p></td>
</tr>
<tr id="row1964716441376"><td class="cellrowborder" valign="top" width="17.9%" headers="mcps1.2.3.1.1 "><p id="p16487444716"><a name="p16487444716"></a><a name="p16487444716"></a>**U**</p></td>
<td class="cellrowborder" valign="top" width="82.1%" headers="mcps1.2.3.1.2 "><p id="p176481441671"><a name="p176481441671"></a><a name="p176481441671"></a>Source operand data type.</p>
<p id="p1085122254811"><a name="p1085122254811"></a><a name="p1085122254811"></a><span id="ph13851192274818"><a name="ph13851192274818"></a><a name="ph13851192274818"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: int8_t/uint8_t/int16_t/uint16_t/int32_t/uint32_t</p></td>
</tr>
<tr id="row949845412516"><td class="cellrowborder" valign="top" width="17.9%" headers="mcps1.2.3.1.1 "><p id="p1249817547512"><a name="p1249817547512"></a><a name="p1249817547512"></a>**part**</p></td>
<td class="cellrowborder" valign="top" width="82.1%" headers="mcps1.2.3.1.2 "><p id="p208823103615"><a name="p208823103615"></a><a name="p208823103615"></a>Enumeration type used to control whether the low half or the high half of **srcReg** is read.</p>
<a name="ul9882111017617"></a><a name="ul9882111017617"></a><ul id="ul9882111017617"><li>**HighLowPart::LOWEST**: Low mode, reads the low half of **srcReg**.</li><li>**HighLowPart::HIGHEST**: High mode, reads the high half of **srcReg**.</li></ul>
<p id="p1699981417236"><a name="p1699981417236"></a><a name="p1699981417236"></a>Note: **RegTraitNumTwo** supports only the LOWEST mode.</p></td>
</tr>
<tr id="row187272531671"><td class="cellrowborder" valign="top" width="17.9%" headers="mcps1.2.3.1.1 "><p id="p1972755311715"><a name="p1972755311715"></a><a name="p1972755311715"></a>S</p></td>
<td class="cellrowborder" valign="top" width="82.1%" headers="mcps1.2.3.1.2 "><p id="p2727175317715"><a name="p2727175317715"></a><a name="p2727175317715"></a>RegTensor type of the destination operation.</p></td>
</tr>
<tr id="row03641457677"><td class="cellrowborder" valign="top" width="17.9%" headers="mcps1.2.3.1.1 "><p id="p1836419571272"><a name="p1836419571272"></a><a name="p1836419571272"></a>V</p></td>
<td class="cellrowborder" valign="top" width="82.1%" headers="mcps1.2.3.1.2 "><p id="p0364125720714"><a name="p0364125720714"></a><a name="p0364125720714"></a>RegTensor type of the source operand.</p></td>
</tr>
</tbody>
</table>

**Table 2**  Function parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="17.64%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="82.36%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="17.64%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>dstReg</p></td>
<td class="cellrowborder" valign="top" width="82.36%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Destination operand.</p>
<p id="p18671158172215"><a name="p18671158172215"></a><a name="p18671158172215"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="17.64%" headers="mcps1.2.3.1.1 "><p id="p59747391278"><a name="p59747391278"></a><a name="p59747391278"></a>srcReg</p></td>
<td class="cellrowborder" valign="top" width="82.36%" headers="mcps1.2.3.1.2 "><p id="p53355414286"><a name="p53355414286"></a><a name="p53355414286"></a>Source operand.</p>
<p id="p25982110231"><a name="p25982110231"></a><a name="p25982110231"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
</tbody>
</table>

**Table 3** Data type mapping between the source operand and the destination operand

<a name="table113383813355"></a>
<table><thead align="left"><tr id="row14332038103513"><th class="cellrowborder" valign="top" width="49.95%" id="mcps1.2.3.1.1"><p id="p1833133810356"><a name="p1833133810356"></a><a name="p1833133810356"></a><strong id="b9504175916367"><a name="b9504175916367"></a><a name="b9504175916367"></a>T data type</strong></p></th>
<th class="cellrowborder" valign="top" width="50.05%" id="mcps1.2.3.1.2"><p id="p122992052379"><a name="p122992052379"></a><a name="p122992052379"></a><strong id="b5299155103716"><a name="b5299155103716"></a><a name="b5299155103716"></a>U data type</strong></p></th>
</tr>
</thead>
<tbody><tr id="row133323813516"><td class="cellrowborder" valign="top" width="49.95%" headers="mcps1.2.3.1.1 "><p id="p18331038103513"><a name="p18331038103513"></a><a name="p18331038103513"></a>int16_t</p></td>
<td class="cellrowborder" valign="top" width="50.05%" headers="mcps1.2.3.1.2 "><p id="p15408550165913"><a name="p15408550165913"></a><a name="p15408550165913"></a>int8_t</p></td>
</tr>
<tr id="row203373817352"><td class="cellrowborder" valign="top" width="49.95%" headers="mcps1.2.3.1.1 "><p id="p2063453093715"><a name="p2063453093715"></a><a name="p2063453093715"></a>uint16_t</p></td>
<td class="cellrowborder" valign="top" width="50.05%" headers="mcps1.2.3.1.2 "><p id="p171344768"><a name="p171344768"></a><a name="p171344768"></a>uint8_t</p></td>
</tr>
<tr id="row2341238123517"><td class="cellrowborder" valign="top" width="49.95%" headers="mcps1.2.3.1.1 "><p id="p18889624919"><a name="p18889624919"></a><a name="p18889624919"></a>int32_t</p></td>
<td class="cellrowborder" valign="top" width="50.05%" headers="mcps1.2.3.1.2 "><p id="p15872614498"><a name="p15872614498"></a><a name="p15872614498"></a>int16_t</p></td>
</tr>
<tr id="row4262125631120"><td class="cellrowborder" valign="top" width="49.95%" headers="mcps1.2.3.1.1 "><p id="p4871766493"><a name="p4871766493"></a><a name="p4871766493"></a>uint32_t</p></td>
<td class="cellrowborder" valign="top" width="50.05%" headers="mcps1.2.3.1.2 "><p id="p1186166154919"><a name="p1186166154919"></a><a name="p1186166154919"></a>uint16_t</p></td>
</tr>
<tr id="row56958525913"><td class="cellrowborder" valign="top" width="49.95%" headers="mcps1.2.3.1.1 "><p id="p1069513521294"><a name="p1069513521294"></a><a name="p1069513521294"></a>uint64_t</p></td>
<td class="cellrowborder" valign="top" width="50.05%" headers="mcps1.2.3.1.2 "><p id="p136959525918"><a name="p136959525918"></a><a name="p136959525918"></a>uint32_t</p></td>
</tr>
<tr id="row9314561598"><td class="cellrowborder" valign="top" width="49.95%" headers="mcps1.2.3.1.1 "><p id="p832567915"><a name="p832567915"></a><a name="p832567915"></a>int64_t</p></td>
<td class="cellrowborder" valign="top" width="50.05%" headers="mcps1.2.3.1.2 "><p id="p4314561193"><a name="p4314561193"></a><a name="p4314561193"></a>int32_t</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section932512912207"></a>

```
template<typename T, typename U, int32_t mode>
__simd_vf__ inline void UnPackVF(__ubuf__ T* dstAddr, __ubuf__ U* srcAddr, uint32_t oneDstRepSize, uint16_t repeatTimes, uint32_t oneSrcRepSize)
{
    AscendC::Reg::RegTensor<U> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneSrcRepSize);
        if constexpr (mode == 0) {
            AscendC::Reg::UnPack<T, U, AscendC::Reg::HighLowPart::LOWEST>(dstReg, srcReg);
        } else if constexpr (mode == 1) {
            AscendC::Reg::UnPack<T, U, AscendC::Reg::HighLowPart::HIGHEST>(dstReg, srcReg);
        }
        AscendC::Reg::StoreAlign(dstAddr + i * oneDstRepSize, dstReg, mask);
    }
}
```

