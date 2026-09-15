# Adds<a name="ZH-CN_TOPIC_0000001929668280"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:42:12.231Z -->

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

Each element in the vector is added to a scalar. The calculation formula is as follows, where VL\_T indicates the number of elements that the vector computation unit can process in one iteration. For the value of VL\_T, see [RegTensor](../register_data_types/RegTensor.md).

![](../../../../figures/zh-cn_formulaimage_0000002488831970.png)

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void Adds(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.55%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.45%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.55%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.45%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Data type of the vector destination operand and source operand.</p>
<p id="p12519205464314"><a name="p12519205464314"></a><a name="p12519205464314"></a><span id="ph7519155484316"><a name="ph7519155484316"></a><a name="ph7519155484316"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/complex32/int64_t/uint64_t/complex64</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="18.55%" headers="mcps1.2.3.1.1 "><p id="p6524185113155"><a name="p6524185113155"></a><a name="p6524185113155"></a>U</p></td>
<td class="cellrowborder" valign="top" width="81.45%" headers="mcps1.2.3.1.2 "><p id="p1775118537244"><a name="p1775118537244"></a><a name="p1775118537244"></a>Data type of the scalar source operand.</p>
<p id="p288815516227"><a name="p288815516227"></a><a name="p288815516227"></a><span id="ph18448175614315"><a name="ph18448175614315"></a><a name="ph18448175614315"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/complex32/int64_t/uint64_t/complex64</p></td>
</tr>
<tr id="row18128134152619"><td class="cellrowborder" valign="top" width="18.55%" headers="mcps1.2.3.1.1 "><p id="p2012917416262"><a name="p2012917416262"></a><a name="p2012917416262"></a>mode</p></td>
<td class="cellrowborder" valign="top" width="81.45%" headers="mcps1.2.3.1.2 "><p id="p77520541653"><a name="p77520541653"></a><a name="p77520541653"></a>Selects the MERGING mode or the ZEROING mode.</p>
<a name="ul1163765616511"></a><a name="ul1163765616511"></a><ul id="ul1163765616511"><li>**ZEROING**: Elements not filtered by the mask are set to zero in **dstReg**.</li><li>**MERGING**: Not supported yet.</li></ul></td>
</tr>
<tr id="row134244441918"><td class="cellrowborder" valign="top" width="18.55%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>S</p></td>
<td class="cellrowborder" valign="top" width="81.45%" headers="mcps1.2.3.1.2 "><p id="p131047476612"><a name="p131047476612"></a><a name="p131047476612"></a>**dstReg** is of the **RegTensor** type, for example, **RegTensor&lt;half&gt;**. It is automatically deduced by the compiler and does not need to be specified by the user.</p></td>
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
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>Destination operand.</p>
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p6844125874315"><a name="p6844125874315"></a><a name="p6844125874315"></a>srcReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>Source operand.</p>
<p id="p157411581277"><a name="p157411581277"></a><a name="p157411581277"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p>
<p id="p142153314474"><a name="p142153314474"></a><a name="p142153314474"></a>The data type must be consistent with the destination operand.</p></td>
</tr>
<tr id="row891912431168"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p195756503168"><a name="p195756503168"></a><a name="p195756503168"></a>scalarValue</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p457515071618"><a name="p457515071618"></a><a name="p457515071618"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p732824954017"><a name="p732824954017"></a><a name="p732824954017"></a>Source operand.</p>
<p id="p73715114429"><a name="p73715114429"></a><a name="p73715114429"></a>Type: scalar.</p>
<p id="p15619124724713"><a name="p15619124724713"></a><a name="p15619124724713"></a>The data type must be consistent with the destination operand.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a>Mask register, used to indicate whether the operation on the source operand elements is valid. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</p></td>
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
__simd_vf__ inline void AddsVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, T scalarValue, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;    ;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Adds(dstReg, srcReg, scalarValue, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

