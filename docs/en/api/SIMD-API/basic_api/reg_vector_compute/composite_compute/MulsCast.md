# MulsCast<a name="ZH-CN_TOPIC_0000002008487800"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:36:42.990Z -->

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

The result of multiplying **src** by **scalar** is converted to the half type in **CAST\_ROUND** mode, and the calculation result is written to **dst** based on **mask**. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002521033481.png)

## Function Prototype<a name="section620mcpsimp"></a>

```
template <typename T0 = DefaultType, typename T1 = DefaultType, typename T2, RegLayout layout = RegLayout::ZERO, typename T3, typename T4>
__simd_callee__ inline void MulsCast(T3& dstReg, T4& srcReg, T2 scalarValue, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.22%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.78%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.22%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T0</p></td>
<td class="cellrowborder" valign="top" width="81.78%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Data type of the destination operand.</p>
<p id="p15959203674218"><a name="p15959203674218"></a><a name="p15959203674218"></a><span id="ph1795914368427"><a name="ph1795914368427"></a><a name="ph1795914368427"></a>Ascend 950PR/Ascend 950DT</span>: Supported data type: half.</p></td>
</tr>
<tr id="row4251861118"><td class="cellrowborder" valign="top" width="18.22%" headers="mcps1.2.3.1.1 "><p id="p22521061415"><a name="p22521061415"></a><a name="p22521061415"></a>T1</p></td>
<td class="cellrowborder" valign="top" width="81.78%" headers="mcps1.2.3.1.2 "><p id="p11953112614"><a name="p11953112614"></a><a name="p11953112614"></a>Data type of the source operand.</p>
<p id="p7973921201115"><a name="p7973921201115"></a><a name="p7973921201115"></a><span id="ph597312121114"><a name="ph597312121114"></a><a name="ph597312121114"></a>Ascend 950PR/Ascend 950DT</span>: Supported data type: float.</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="18.22%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a>T2</p></td>
<td class="cellrowborder" valign="top" width="81.78%" headers="mcps1.2.3.1.2 "><p id="p1775118537244"><a name="p1775118537244"></a><a name="p1775118537244"></a>Data type of the scalar source operand.</p>
<p id="p69731028111110"><a name="p69731028111110"></a><a name="p69731028111110"></a><span id="ph17973162841112"><a name="ph17973162841112"></a><a name="ph17973162841112"></a>Ascend 950PR/Ascend 950DT</span>, supported data type: float</p></td>
</tr>
<tr id="row416056114119"><td class="cellrowborder" valign="top" width="18.22%" headers="mcps1.2.3.1.1 "><p id="p95016121914"><a name="p95016121914"></a><a name="p95016121914"></a>layout</p></td>
<td class="cellrowborder" valign="top" width="81.78%" headers="mcps1.2.3.1.2 "><p id="p1433681723313"><a name="p1433681723313"></a><a name="p1433681723313"></a><a href="../aux_data_types/RegLayout.md">RegLayout</a> enumeration type.</p>
<p id="p24515419439"><a name="p24515419439"></a><a name="p24515419439"></a>This API supports only **RegLayout::ZERO** and **RegLayout::ONE**.</p>
<p id="p150112121614"><a name="p150112121614"></a><a name="p150112121614"></a>Used when **src** is of the half type; it does not take effect for the float type. For the half type, **RegLayout::ZERO** indicates reading half elements from the even positions of the b16 RegTensor and converting them to float, and **RegLayout::ONE** indicates reading half elements from the odd positions of the b16 RegTensor and converting them to float.</p></td>
</tr>
<tr id="row17775181422"><td class="cellrowborder" valign="top" width="18.22%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>T3</p></td>
<td class="cellrowborder" valign="top" width="81.78%" headers="mcps1.2.3.1.2 "><p id="p131047476612"><a name="p131047476612"></a><a name="p131047476612"></a>The RegTensor type of dstReg, for example, RegTensor&lt;float&gt;, is automatically deduced by the compiler, and the user does not need to fill it in.</p></td>
</tr>
<tr id="row5528594214"><td class="cellrowborder" valign="top" width="18.22%" headers="mcps1.2.3.1.1 "><p id="p145090101527"><a name="p145090101527"></a><a name="p145090101527"></a>T4</p></td>
<td class="cellrowborder" valign="top" width="81.78%" headers="mcps1.2.3.1.2 "><p id="p12509510225"><a name="p12509510225"></a><a name="p12509510225"></a>RegTensor type of srcReg, for example, RegTensor&lt;float&gt;, which is automatically deduced by the compiler and does not need to be specified by the user.</p></td>
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
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>dstReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p86271310162918"><a name="p86271310162918"></a><a name="p86271310162918"></a>Destination operand.</p>
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p6844125874315"><a name="p6844125874315"></a><a name="p6844125874315"></a>srcReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>Source operand.</p>
<p id="p17716122843014"><a name="p17716122843014"></a><a name="p17716122843014"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row891912431168"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p195756503168"><a name="p195756503168"></a><a name="p195756503168"></a>scalarValue</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p457515071618"><a name="p457515071618"></a><a name="p457515071618"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p732824954017"><a name="p732824954017"></a><a name="p732824954017"></a>Source operand.</p>
<p id="p73715114429"><a name="p73715114429"></a><a name="p73715114429"></a>Type: scalar.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>**mask**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a>Validity indicator for element-wise operations on the source operand. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

```
template<typename T>
__simd_vf__ inline void MulsCastVF(__ubuf__ half* dstAddr, __ubuf__ float* srcAddr, float scalarValue, uint32_t count, uint32_t srcRepeatSize, uint32_t dstRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<float> srcReg0;
    AscendC::Reg::RegTensor<half> dstReg0;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<float>(count);
        AscendC::Reg::LoadAlign(srcReg0, srcAddr + i * srcRepeatSize);
        AscendC::Reg::MulsCast<half, float, float, AscendC::Reg::RegLayout::ZERO>(dstReg0, srcReg0, scalarValue, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * dstRepeatSize, dstReg0, mask);
    }
}
```
