# Relu<a name="ZH-CN_TOPIC_0000001929651446"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:55:00.623Z -->

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

Performs the relu operation on the input data **srcReg** based on **mask**, and writes the result to **dstReg**. The relu calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002521151595.png)

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Relu(U& dstReg, U& srcReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.58%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.42%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.58%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.42%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Operand data type.</p>
<p id="p2366753194920"><a name="p2366753194920"></a><a name="p2366753194920"></a><span id="ph236695334914"><a name="ph236695334914"></a><a name="ph236695334914"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: int32_t/int64_t/half/float</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="18.58%" headers="mcps1.2.3.1.1 "><p id="p1756419170189"><a name="p1756419170189"></a><a name="p1756419170189"></a>mode</p></td>
<td class="cellrowborder" valign="top" width="81.42%" headers="mcps1.2.3.1.2 "><p id="p1557705555811"><a name="p1557705555811"></a><a name="p1557705555811"></a>Selects the MERGING mode or the ZEROING mode.</p>
<a name="ul195771555135820"></a><a name="ul195771555135820"></a><ul id="ul195771555135820"><li>ZEROING: Elements not selected by the mask are set to zero in dstReg.</li><li>MERGING: Currently not supported.</li></ul></td>
</tr>
<tr id="row24011032174411"><td class="cellrowborder" valign="top" width="18.58%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>U</p></td>
<td class="cellrowborder" valign="top" width="81.42%" headers="mcps1.2.3.1.2 "><p id="p15901115914145"><a name="p15901115914145"></a><a name="p15901115914145"></a><span id="ph19851723182011"><a name="ph19851723182011"></a><a name="ph19851723182011"></a>RegTensor type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically inferred by the compiler and does not need to be specified by the user.</span></p></td>
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
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p108051250181214"><a name="p108051250181214"></a><a name="p108051250181214"></a>dstReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>Destination operand.</p>
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p19574165615129"><a name="p19574165615129"></a><a name="p19574165615129"></a>srcReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>Source operand.</p>
<p id="p7123111612517"><a name="p7123111612517"></a><a name="p7123111612517"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p>
<p id="p1484485824312"><a name="p1484485824312"></a><a name="p1484485824312"></a>The data type of the source operand must be consistent with that of the destination operand.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>Indicates the validity of the source operand element operation. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
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
__simd_vf__ inline void ReluVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, 
 uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;    
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Relu(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

