# ExpSub<a name="ZH-CN_TOPIC_0000002008646072"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:32:17.074Z -->

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

Subtracts **srcReg1** from **srcReg0**, uses the difference as the exponent of e, and writes the result to **dstReg** based on **mask**.

When **srcReg** is of the **float** type: ![](../../../../figures/zh-cn_formulaimage_0000002563310903.png)

When **srcReg** is of the **half** type: ![](../../../../figures/zh-cn_formulaimage_0000002532471002.png)

## Function Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, typename U = DefaultType, RegLayout layout = RegLayout::ZERO, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S, typename V>
__simd_callee__ inline void ExpSub(S& dstReg, V& srcReg0, V& srcReg1, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.48%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.52000000000001%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.48%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.52000000000001%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Destination operand data type.</p>
<p id="p96431308228"><a name="p96431308228"></a><a name="p96431308228"></a><span id="ph7643907227"><a name="ph7643907227"></a><a name="ph7643907227"></a>Ascend 950PR/Ascend 950DT</span>, supported data type: float.</p>
<p id="p9260849185017"><a name="p9260849185017"></a><a name="p9260849185017"></a><span id="ph9490151519499"><a name="ph9490151519499"></a><a name="ph9490151519499"></a>MC62CM12A AI processor</span>, supported data types: half/float.</p></td>
</tr>
<tr id="row396814422169"><td class="cellrowborder" valign="top" width="18.48%" headers="mcps1.2.3.1.1 "><p id="p89687420164"><a name="p89687420164"></a><a name="p89687420164"></a>U</p></td>
<td class="cellrowborder" valign="top" width="81.52000000000001%" headers="mcps1.2.3.1.2 "><p id="p1896874212162"><a name="p1896874212162"></a><a name="p1896874212162"></a>Source operand data type.</p>
<p id="p8238111018225"><a name="p8238111018225"></a><a name="p8238111018225"></a><span id="ph192381110152211"><a name="ph192381110152211"></a><a name="ph192381110152211"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: half/float.</p></td>
</tr>
<tr id="row105019122115"><td class="cellrowborder" valign="top" width="18.48%" headers="mcps1.2.3.1.1 "><p id="p95016121914"><a name="p95016121914"></a><a name="p95016121914"></a>layout</p></td>
<td class="cellrowborder" valign="top" width="81.52000000000001%" headers="mcps1.2.3.1.2 "><p id="p1433681723313"><a name="p1433681723313"></a><a name="p1433681723313"></a><a href="../aux_data_types/RegLayout.md">RegLayout</a> enumeration type:</p>
<pre class="screen" id="screen18481935171419"><a name="screen18481935171419"></a><a name="screen18481935171419"></a>enum class RegLayout {
    UNKNOWN = -1,
    ZERO,
    ONE,
    TWO,
    THREE
};</pre>
<p id="p150112121614"><a name="p150112121614"></a><a name="p150112121614"></a>This API supports only **RegLayout::ZERO** and **RegLayout::ONE**. They take effect when the **src** type is **half** and do not take effect for **float**. For the **half** type, **RegLayout::ZERO** indicates reading **half** elements from the even bits of the b16 **RegTensor** and converting them to **float**, and **RegLayout::ONE** indicates reading **half** elements from the odd bits of the b16 **RegTensor** and converting them to **float**.</p></td>
</tr>
<tr id="row356441781813"><td class="cellrowborder" valign="top" width="18.48%" headers="mcps1.2.3.1.1 "><p id="p1756419170189"><a name="p1756419170189"></a><a name="p1756419170189"></a>**mode**</p></td>
<td class="cellrowborder" valign="top" width="81.52000000000001%" headers="mcps1.2.3.1.2 "><p id="p77520541653"><a name="p77520541653"></a><a name="p77520541653"></a>Selects the MERGING mode or the ZEROING mode.</p>
<a name="ul1163765616511"></a><a name="ul1163765616511"></a><ul id="ul1163765616511"><li>**ZEROING**: Elements not filtered by the mask are set to zero in **dst**.</li><li>**MERGING**: Currently not supported.</li></ul></td>
</tr>
<tr id="row916216311197"><td class="cellrowborder" valign="top" width="18.48%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>**S**</p></td>
<td class="cellrowborder" valign="top" width="81.52000000000001%" headers="mcps1.2.3.1.2 "><p id="p131047476612"><a name="p131047476612"></a><a name="p131047476612"></a>**dstReg** is of the **RegTensor** type, for example, **RegTensor&lt;half&gt;**. It is automatically deduced by the compiler and does not need to be specified by the user.</p></td>
</tr>
<tr id="row17951175431514"><td class="cellrowborder" valign="top" width="18.48%" headers="mcps1.2.3.1.1 "><p id="p495215411152"><a name="p495215411152"></a><a name="p495215411152"></a>**V**</p></td>
<td class="cellrowborder" valign="top" width="81.52000000000001%" headers="mcps1.2.3.1.2 "><p id="p1217321168"><a name="p1217321168"></a><a name="p1217321168"></a>**srcReg0**/**srcReg1** are of the **RegTensor** type, for example, **RegTensor&lt;half&gt;**. They are automatically deduced by the compiler and do not need to be specified by the user.</p></td>
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
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p19574165615129"><a name="p19574165615129"></a><a name="p19574165615129"></a>**srcReg0**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>Source operand.</p>
<p id="p7123111612517"><a name="p7123111612517"></a><a name="p7123111612517"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row11773440341"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p10132448173420"><a name="p10132448173420"></a><a name="p10132448173420"></a>**srcReg1**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p151325484342"><a name="p151325484342"></a><a name="p151325484342"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p181326485341"><a name="p181326485341"></a><a name="p181326485341"></a>Source operand.</p>
<p id="p74744436423"><a name="p74744436423"></a><a name="p74744436423"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>**mask**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>Valid indication of the operation on source operand elements. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

When **srcReg** is of the **half** type, the **Vector** computing unit processes at most VL/sizeof(float) **half** elements per computation, and only the even bits of the **mask** are valid. Refer to the following figure:

![](../../../../figures/3_3_4_6_3_constraint_desc.png)

## Example<a name="section642mcpsimp"></a>

```
template<typename T, typename U>
static __simd_vf__ inline void ExpSubVF(__ubuf__ T* dstAddr, __ubuf__ U* src0Addr, __ubuf__ U* src1Addr, uint32_t count, uint32_t srcRepeatSize, uint32_t dstRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<U> srcReg0;
    AscendC::Reg::RegTensor<U> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<U>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * srcRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * srcRepeatSize);
        AscendC::Reg::ExpSub<T, U, AscendC::Reg::RegLayout::ZERO, AscendC::Reg::MaskMergeMode::ZEROING>(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * dstRepeatSize, dstReg, mask);
    }
}
```
