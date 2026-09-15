# ShiftRight<a name="ZH-CN_TOPIC_0000001975252378"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T22:12:50.928Z -->

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
- Atlas 200I/500 A2 inference product: Not supported
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

Based on **mask**, performs a right shift operation on the input data **srcReg0** by the corresponding elements of **srcReg1**, and writes the result to **dstReg**.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S, typename V>
__simd_callee__ inline void ShiftRight(S& dstReg, S& srcReg0, V& srcReg1, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.35%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.65%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.35%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>**T**</p></td>
<td class="cellrowborder" valign="top" width="81.65%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Source operand data type.</p>
<p id="p15959203674218"><a name="p15959203674218"></a><a name="p15959203674218"></a><span id="ph1795914368427"><a name="ph1795914368427"></a><a name="ph1795914368427"></a>Ascend 950PR/Ascend 950DT</span>, supported data types are: uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/uint64_t/int64_t</p></td>
</tr>
<tr id="row356441781813"><td class="cellrowborder" valign="top" width="18.35%" headers="mcps1.2.3.1.1 "><p id="p1756419170189"><a name="p1756419170189"></a><a name="p1756419170189"></a>**U**</p></td>
<td class="cellrowborder" valign="top" width="81.65%" headers="mcps1.2.3.1.2 "><p id="p7564191710185"><a name="p7564191710185"></a><a name="p7564191710185"></a>Right shift parameter data type.</p>
<p id="p16699194618424"><a name="p16699194618424"></a><a name="p16699194618424"></a><span id="ph969918463426"><a name="ph969918463426"></a><a name="ph969918463426"></a>Ascend 950PR/Ascend 950DT</span>, supported data types are: int8_t/int16_t/int32_t/int64_t</p>
<p id="p876115422318"><a name="p876115422318"></a><a name="p876115422318"></a><span>For the data type constraints of the source operand and destination operand, see</span><a href="#table15348612514">Table 3</a>.</p></td>
</tr>
<tr id="row916216311197"><td class="cellrowborder" valign="top" width="18.35%" headers="mcps1.2.3.1.1 "><p id="p29551327675"><a name="p29551327675"></a><a name="p29551327675"></a>**mode**</p></td>
<td class="cellrowborder" valign="top" width="81.65%" headers="mcps1.2.3.1.2 "><p id="p77520541653"><a name="p77520541653"></a><a name="p77520541653"></a>Selects MERGING mode or ZEROING mode.</p>
<a name="ul1163765616511"></a><a name="ul1163765616511"></a><ul id="ul1163765616511"><li>**ZEROING**: Elements not filtered by the mask are set to zero in dst.</li><li>**MERGING**: Currently not supported.</li></ul></td>
</tr>
<tr id="row2097312253711"><td class="cellrowborder" valign="top" width="18.35%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>S</p></td>
<td class="cellrowborder" valign="top" width="81.65%" headers="mcps1.2.3.1.2 "><p id="p131047476612"><a name="p131047476612"></a><a name="p131047476612"></a>RegTensor type of srcReg0/dstReg, for example, RegTensor&lt;uint32_t&gt;, which is automatically deduced by the compiler and does not need to be specified by the user.</p></td>
</tr>
<tr id="row1517562211610"><td class="cellrowborder" valign="top" width="18.35%" headers="mcps1.2.3.1.1 "><p id="p156882018157"><a name="p156882018157"></a><a name="p156882018157"></a>V</p></td>
<td class="cellrowborder" valign="top" width="81.65%" headers="mcps1.2.3.1.2 "><p id="p206881518751"><a name="p206881518751"></a><a name="p206881518751"></a>RegTensor type of srcReg1, for example, RegTensor&lt;int32_t&gt;, which is automatically deduced by the compiler and does not need to be specified by the user.</p></td>
</tr>
</tbody>
</table>

**Table 2**  Parameters

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p108051250181214"><a name="p108051250181214"></a><a name="p108051250181214"></a>**dstReg**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>Destination operand.</p>
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a>Type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p19574165615129"><a name="p19574165615129"></a><a name="p19574165615129"></a>**srcReg0**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>Source operand.</p>
<p id="p7123111612517"><a name="p7123111612517"></a><a name="p7123111612517"></a>Type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p>
<p id="p1484485824312"><a name="p1484485824312"></a><a name="p1484485824312"></a>The data type of the source operand must be consistent with that of the destination operand.</p></td>
</tr>
<tr id="row11773440341"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p10132448173420"><a name="p10132448173420"></a><a name="p10132448173420"></a>**srcReg1**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p151325484342"><a name="p151325484342"></a><a name="p151325484342"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p142191643103911"><a name="p142191643103911"></a><a name="p142191643103911"></a>Source operand.</p>
<p id="p181326485341"><a name="p181326485341"></a><a name="p181326485341"></a>Type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p>
<p id="p16979104419396"><a name="p16979104419396"></a><a name="p16979104419396"></a>Element values are not supported to be set to negative numbers.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>**mask**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input.</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>Valid indication of the source operand element operation. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
</tr>
</tbody>
</table>

**Table 3** Data type constraints of the source operand and destination operand

<a name="table15348612514"></a>
<table><thead align="left"><tr id="row3341061250"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p10342614250"><a name="p10342614250"></a><a name="p10342614250"></a>srcReg0/dstReg data type</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p2034968252"><a name="p2034968252"></a><a name="p2034968252"></a>srcReg1 data type</p></th>
</tr>
</thead>
<tbody><tr id="row10341367252"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p51901218260"><a name="p51901218260"></a><a name="p51901218260"></a>uint8_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1328659182611"><a name="p1328659182611"></a><a name="p1328659182611"></a>int8_t</p></td>
</tr>
<tr id="row5341466253"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p819052102614"><a name="p819052102614"></a><a name="p819052102614"></a>uint16_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p122869913269"><a name="p122869913269"></a><a name="p122869913269"></a>int16_t</p></td>
</tr>
<tr id="row163413612519"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p61908212262"><a name="p61908212262"></a><a name="p61908212262"></a>uint32_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p6286109202619"><a name="p6286109202619"></a><a name="p6286109202619"></a>int32_t</p></td>
</tr>
<tr id="row03411632513"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p204451180503"><a name="p204451180503"></a><a name="p204451180503"></a>uint64_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1144501845010"><a name="p1144501845010"></a><a name="p1144501845010"></a>int64_t</p></td>
</tr>
<tr id="row203417618256"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1319018218263"><a name="p1319018218263"></a><a name="p1319018218263"></a>int8_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1228615912614"><a name="p1228615912614"></a><a name="p1228615912614"></a>int8_t</p></td>
</tr>
<tr id="row1022415972519"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p51904232620"><a name="p51904232620"></a><a name="p51904232620"></a>int16_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p128629122611"><a name="p128629122611"></a><a name="p128629122611"></a>int16_t</p></td>
</tr>
<tr id="row597443535113"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1919013222612"><a name="p1919013222612"></a><a name="p1919013222612"></a>int32_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p628615962615"><a name="p628615962615"></a><a name="p628615962615"></a>int32_t</p></td>
</tr>
<tr id="row8366839165113"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p125401256184615"><a name="p125401256184615"></a><a name="p125401256184615"></a>int64_t</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p654025674614"><a name="p654025674614"></a><a name="p654025674614"></a>int64_t</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   For a logical shift (unsigned data type), if the shift amount is greater than the data type bit width, the output is 0.
-   For an arithmetic shift (signed data type), if **srcReg0** is less than 0, **srcReg1** is greater than 0, and the shift amount is greater than the data type bit width, the output is -1; if **srcReg0** is greater than 0 and the shift amount is greater than the data type bit width, the output is 0.

## Example<a name="section642mcpsimp"></a>

```
template<typename T, typename U>
__simd_vf__ inline void ShiftRightVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ U* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<U> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);       
        AscendC::Reg::ShiftRight(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

