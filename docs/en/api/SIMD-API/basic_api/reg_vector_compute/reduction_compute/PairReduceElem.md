# PairReduceElem<a name="ZH-CN_TOPIC_0000002045329737"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:36:44.412Z -->

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

Adds two adjacent values in the input **srcReg** and stores the result in the lower bits of **dstReg**.

## Prototype<a name="section620mcpsimp"></a>

```
template <PairReduce type = PairReduce::SUM, typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void PairReduceElem(U& dstReg, U srcReg, MaskReg mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="38.269999999999996%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="61.73%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row115388354431"><td class="cellrowborder" valign="top" width="38.269999999999996%" headers="mcps1.2.3.1.1 "><p id="p4539133519432"><a name="p4539133519432"></a><a name="p4539133519432"></a>type</p></td>
<td class="cellrowborder" valign="top" width="61.73%" headers="mcps1.2.3.1.2 "><p id="p653953534317"><a name="p653953534317"></a><a name="p653953534317"></a>Specific **PairReduce** type. Currently, only reduction sum is supported.</p>
<pre class="screen" id="screen7190913105015"><a name="screen7190913105015"></a><a name="screen7190913105015"></a>enum class PairReduce {
    SUM = 0,
};</pre></td>
</tr>
<tr id="row1835857145817"><td class="cellrowborder" valign="top" width="38.269999999999996%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="61.73%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Data type of the destination operand and source operand.</p>
<p id="p91231181314"><a name="p91231181314"></a><a name="p91231181314"></a><span id="ph21231881016"><a name="ph21231881016"></a><a name="ph21231881016"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: half/float</p></td>
</tr>
<tr id="row3742113618507"><td class="cellrowborder" valign="top" width="38.269999999999996%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a>mode</p></td>
<td class="cellrowborder" valign="top" width="61.73%" headers="mcps1.2.3.1.2 "><p id="p77520541653"><a name="p77520541653"></a><a name="p77520541653"></a>Selects the MERGING mode or ZEROING mode.</p>
<a name="ul1163765616511"></a><a name="ul1163765616511"></a><ul id="ul1163765616511"><li>**ZEROING**: Elements not filtered by the mask are set to zero in the destination. Currently, only this mode is supported.</li></ul></td>
</tr>
<tr id="row1439216366474"><td class="cellrowborder" valign="top" width="38.269999999999996%" headers="mcps1.2.3.1.1 "><p id="p439323610479"><a name="p439323610479"></a><a name="p439323610479"></a>**U**</p></td>
<td class="cellrowborder" valign="top" width="61.73%" headers="mcps1.2.3.1.2 "><p id="p2727175317715"><a name="p2727175317715"></a><a name="p2727175317715"></a>RegTensor type of the destination operand and source operand, which is automatically deduced by the compiler and does not need to be specified by the user.</p></td>
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
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>**dstReg**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>Destination operand.</p>
<p id="p1818424418593"><a name="p1818424418593"></a><a name="p1818424418593"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row198393197176"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1664684418171"><a name="p1664684418171"></a><a name="p1664684418171"></a>srcReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p9839171971716"><a name="p9839171971716"></a><a name="p9839171971716"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p779095312179"><a name="p779095312179"></a><a name="p779095312179"></a>Source operand.</p>
<p id="p97903531171"><a name="p97903531171"></a><a name="p97903531171"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p>
<p id="p15580452122512"><a name="p15580452122512"></a><a name="p15580452122512"></a>The data type of the source operand must be the same as that of the destination operand.</p></td>
</tr>
<tr id="row17332152321719"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p71017491847"><a name="p71017491847"></a><a name="p71017491847"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p73331523101710"><a name="p73331523101710"></a><a name="p73331523101710"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>Validity indicator for the operation on source operand elements. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section177921451558"></a>

None

## Example<a name="section642mcpsimp"></a>

```
template<typename T>
__simd_vf__ inline void PairReduceElemVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, 
 uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::PairReduceElem<AscendC::Reg::PairReduce::SUM>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

