# DeInterleave<a name="ZH-CN_TOPIC_0000001929091478"></a>

<!-- md-trans-meta sourceCommit=c5471ceb6d0822ce77b3dc5a1d15ba9dd72783a5 translatedAt=2026-08-27T22:19:51.111Z -->

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

Given the source operand register tensors **srcReg0** and **srcReg1**, deinterleave the elements in **srcReg0** and **srcReg1** and store them into the destination operands **dstReg0** and **dstReg1**. The deinterleaving arrangement is shown in the following figure, where each cell represents an element:

![](../../../../figures/DeInterleave-39.png)

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, typename U>
__simd_callee__ inline void DeInterleave(U& dstReg0, U& dstReg1, U& srcReg0, U& srcReg1)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.529999999999998%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.47%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.529999999999998%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.47%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Data type of the destination operand and source operand.</p>
<p id="p19741733173613"><a name="p19741733173613"></a><a name="p19741733173613"></a><span id="ph1574633193611"><a name="ph1574633193611"></a><a name="ph1574633193611"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: bool/uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/uint64_t/int64_t/half/float/bfloat16_t</p></td>
</tr>
<tr id="row2424150175914"><td class="cellrowborder" valign="top" width="18.529999999999998%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>U</p></td>
<td class="cellrowborder" valign="top" width="81.47%" headers="mcps1.2.3.1.2 "><p id="p131047476612"><a name="p131047476612"></a><a name="p131047476612"></a>RegTensor type of the source operand and destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user.</p></td>
</tr>
</tbody>
</table>

**Table 2** Function parameter description

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>dstReg0, dstReg1</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>Destination operand.</p>
<p id="p18220202613566"><a name="p18220202613566"></a><a name="p18220202613566"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p6844125874315"><a name="p6844125874315"></a><a name="p6844125874315"></a>srcReg0, srcReg1</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p9761550745"><a name="p9761550745"></a><a name="p9761550745"></a>Source operand.</p>
<p id="p7484358103819"><a name="p7484358103819"></a><a name="p7484358103819"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p>
<p id="p37015312238"><a name="p37015312238"></a><a name="p37015312238"></a>The data type of the source operand must be consistent with that of the destination operand.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section1240634052310"></a>

- The data types of **srcReg0**, **srcReg1**, **dstReg0**, and **dstReg1** must be consistent.
- **srcReg0** and **srcReg1** can be the same **RegTensor**.
- **dstReg0** and **dstReg1** cannot be the same **RegTensor**.
- The source operand and the destination operand are allowed to be the same **RegTensor**, for example, **DeInterleave(srcReg0, srcReg1, srcReg0, srcReg1)**.
- Under the **b64** data type, only **RegTraitNumTwo** is supported.

## Example<a name="section642mcpsimp"></a>

```
template<typename T>
__simd_vf__ inline void DeInterLeaveVF(__ubuf__ T* dst0Addr, __ubuf__ T* dst1Addr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t oneRepeatSize, uint16_t repeatTimes)
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
