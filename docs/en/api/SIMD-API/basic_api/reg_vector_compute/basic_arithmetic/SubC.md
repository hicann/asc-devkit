# SubC<a name="ZH-CN_TOPIC_0000002080143865"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:57:31.587Z -->

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

Performs element-wise subtraction of the source operands **srcReg0**, **srcReg1**, and the carry data **carrySrc** based on **mask**, and writes the result to the destination operand **dstReg**. If a borrow occurs during the subtraction of **srcReg0**, **srcReg1**, and **carrySrc**, 0 is written to the least significant bit of each 4-bit group at the corresponding position in **MaskReg** **carry**; otherwise, 1 is written. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002566625829.png)

The following is a specific example:

**Table 1**  Example description

<a name="table78091736133120"></a>
<table><thead align="left"><tr id="row480917360316"><th class="cellrowborder" valign="top" width="16.31163116311631%" id="mcps1.2.4.1.1"><p id="p580917368314"><a name="p580917368314"></a><a name="p580917368314"></a>Data type</p></th>
<th class="cellrowborder" valign="top" width="19.561956195619562%" id="mcps1.2.4.1.2"><p id="p1280912365313"><a name="p1280912365313"></a><a name="p1280912365313"></a>Whether to borrow</p></th>
<th class="cellrowborder" valign="top" width="64.12641264126412%" id="mcps1.2.4.1.3"><p id="p680913653119"><a name="p680913653119"></a><a name="p680913653119"></a>Example description</p></th>
</tr>
</thead>
<tbody><tr id="row13795744172313"><td class="cellrowborder" rowspan="2" valign="top" width="16.31163116311631%" headers="mcps1.2.4.1.1 "><p id="p21571440123413"><a name="p21571440123413"></a><a name="p21571440123413"></a>int32_t data type</p>
<p id="p1721120463234"><a name="p1721120463234"></a><a name="p1721120463234"></a></p></td>
<td class="cellrowborder" valign="top" width="19.561956195619562%" headers="mcps1.2.4.1.2 "><p id="p154637020320"><a name="p154637020320"></a><a name="p154637020320"></a>No borrow is generated</p></td>
<td class="cellrowborder" valign="top" width="64.12641264126412%" headers="mcps1.2.4.1.3 "><p id="p3311243193110"><a name="p3311243193110"></a><a name="p3311243193110"></a>a_i = 5, b_i = 2, carrySrc_i = 1</p>
<p id="p531113439313"><a name="p531113439313"></a><a name="p531113439313"></a>dst_i = a_i - b_i - carrySrc_i  = 2</p>
<p id="p4311164312316"><a name="p4311164312316"></a><a name="p4311164312316"></a>Write 1 to the least significant bit of each 4-bit group at the corresponding position in carryp: carry_i = 1</p></td>
</tr>
<tr id="row1179654412318"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p58101636153112"><a name="p58101636153112"></a><a name="p58101636153112"></a>A borrow is generated</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p15172134819317"><a name="p15172134819317"></a><a name="p15172134819317"></a>a_i = 5, b_i = -7, carrySrc_i = 1</p>
<p id="p2172348123110"><a name="p2172348123110"></a><a name="p2172348123110"></a>dst_i = a_i - b_i - carrySrc_i  = 11</p>
<p id="p5172174883118"><a name="p5172174883118"></a><a name="p5172174883118"></a>Write 0 to the least significant bit of each 4-bit group at the corresponding position in carryp: carry_i = 0</p></td>
</tr>
<tr id="row381063633114"><td class="cellrowborder" rowspan="2" valign="top" width="16.31163116311631%" headers="mcps1.2.4.1.1 "><p id="p3815204223410"><a name="p3815204223410"></a><a name="p3815204223410"></a>uint32_t data type</p></td>
<td class="cellrowborder" valign="top" width="19.561956195619562%" headers="mcps1.2.4.1.2 "><p id="p11843103015347"><a name="p11843103015347"></a><a name="p11843103015347"></a>No borrow is generated</p></td>
<td class="cellrowborder" valign="top" width="64.12641264126412%" headers="mcps1.2.4.1.3 "><p id="p570203082917"><a name="p570203082917"></a><a name="p570203082917"></a>a_i = 5, b_i = 2, carrySrc_i = 1</p>
<p id="p168941627173112"><a name="p168941627173112"></a><a name="p168941627173112"></a>dst_i = a_i - b_i - carrySrc_i  = 2</p>
<p id="p29562393290"><a name="p29562393290"></a><a name="p29562393290"></a>Write 1 to the least significant bit of each 4-bit group at the corresponding position in carryp: carry_i = 1</p></td>
</tr>
<tr id="row1481017368317"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p148435308342"><a name="p148435308342"></a><a name="p148435308342"></a>A borrow is generated</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p686855917304"><a name="p686855917304"></a><a name="p686855917304"></a>a_i = 5, b_i = 7, carrySrc_i = 1</p>
<p id="p34696716311"><a name="p34696716311"></a><a name="p34696716311"></a>dst_i = a_i - b_i - carrySrc_i  = -3</p>
<p id="p8868185973017"><a name="p8868185973017"></a><a name="p8868185973017"></a>Write 0 to the least significant bit of each 4-bit group at the corresponding position in carryp: carry_i = 0</p></td>
</tr>
</tbody>
</table>

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, typename U>
__simd_callee__ inline void SubC(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& carrySrc, MaskReg& mask)
```

## Parameters<a name="section389364115374"></a>

**Table 2** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.099999999999998%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.89999999999999%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.099999999999998%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.89999999999999%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Data type of the vector destination operand and source operand.</p>
<p id="p15149173514615"><a name="p15149173514615"></a><a name="p15149173514615"></a><span id="ph7464649163819"><a name="ph7464649163819"></a><a name="ph7464649163819"></a>Ascend 950PR/Ascend 950DT</span>, supported data types are: uint32_t/int32_t</p></td>
</tr>
<tr id="row94714521855"><td class="cellrowborder" valign="top" width="18.099999999999998%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>U</p></td>
<td class="cellrowborder" valign="top" width="81.89999999999999%" headers="mcps1.2.3.1.2 "><p id="p131047476612"><a name="p131047476612"></a><a name="p131047476612"></a><span id="ph19851723182011"><a name="ph19851723182011"></a><a name="ph19851723182011"></a>RegTensor type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user.</span></p></td>
</tr>
</tbody>
</table>

**Table 3** Parameter description

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>dstReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>Destination operand.</p>
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a><span id="ph134278176129"><a name="ph134278176129"></a><a name="ph134278176129"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p6844125874315"><a name="p6844125874315"></a><a name="p6844125874315"></a>srcReg0</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p18831019185112"><a name="p18831019185112"></a><a name="p18831019185112"></a>Source operand.</p>
<p id="p38835197519"><a name="p38835197519"></a><a name="p38835197519"></a><span id="ph20507826177"><a name="ph20507826177"></a><a name="ph20507826177"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p>
<p id="p488351919519"><a name="p488351919519"></a><a name="p488351919519"></a>The data type must be consistent with that of the destination operand.</p></td>
</tr>
<tr id="row891912431168"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p195756503168"><a name="p195756503168"></a><a name="p195756503168"></a>**srcReg1**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p457515071618"><a name="p457515071618"></a><a name="p457515071618"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p732824954017"><a name="p732824954017"></a><a name="p732824954017"></a>Source operand.</p>
<p id="p887317147178"><a name="p887317147178"></a><a name="p887317147178"></a><span id="ph198734141172"><a name="ph198734141172"></a><a name="ph198734141172"></a>Type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p>
<p id="p101261830153914"><a name="p101261830153914"></a><a name="p101261830153914"></a>The data type must be consistent with the destination operand.</p></td>
</tr>
<tr id="row16125155710429"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p14751813191114"><a name="p14751813191114"></a><a name="p14751813191114"></a>**carrySrc**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p137521315114"><a name="p137521315114"></a><a name="p137521315114"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p075513101113"><a name="p075513101113"></a><a name="p075513101113"></a>Source operand. Input carry value.</p>
<p id="p1149514185128"><a name="p1149514185128"></a><a name="p1149514185128"></a>Type is <a href="../register_data_types/MaskReg.md">MaskReg</a>.</p></td>
</tr>
<tr id="row1710132819566"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p15429173015619"><a name="p15429173015619"></a><a name="p15429173015619"></a>**carry**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p342933075615"><a name="p342933075615"></a><a name="p342933075615"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p1142915300561"><a name="p1142915300561"></a><a name="p1142915300561"></a>Destination operand. Output carry value.</p>
<p id="p1742912304566"><a name="p1742912304566"></a><a name="p1742912304566"></a>Type is <a href="../register_data_types/MaskReg.md">MaskReg</a>.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>**mask**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>Valid indication of the element-wise operation on the source operand. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Calling Example<a name="section232816306478"></a>

```
template <typename T>
__simd_vf__ inline void SubC(__ubuf__ T* dst0Addr, __ubuf__ T* dst1Addr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t repeatTimes, uint32_t oneRepeatSize){
    
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg0;
    AscendC::Reg::MaskReg mask; 
    AscendC::Reg::MaskReg maskZero = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
    AscendC::Reg::MaskReg carry = AscendC::Reg::CreateMask<uint8_t>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::SubC(carry, dstReg0, srcReg0, srcReg1, maskZero, mask);
        // 8*4B=32B align
        AscendC::Reg::StoreAlign<uint32_t, AscendC::Reg::MaskDist::DIST_NORM>((__ubuf__ uint32_t*)dst1Addr + i * 8, carry);
        AscendC::Reg::StoreAlign(dst0Addr + i * oneRepeatSize, dstReg0, mask);
    }
}
```

