# Muls<a name="ZH-CN_TOPIC_0000001956827105"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:53:01.819Z -->

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

Multiplies each element in the vector by a scalar. The calculation formula is as follows, where **VL_T** indicates the number of elements that the vector computation unit can process in one iteration. For the value of **VL_T**, see [RegTensor](../register_data_types/RegTensor.md).

![](../../../../figures/zh-cn_formulaimage_0000002488832082.png)

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void Muls(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.61%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.39%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.61%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.39%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Data type of the vector destination operand and source operand.</p>
<p id="p177491635184115"><a name="p177491635184115"></a><a name="p177491635184115"></a><span id="ph17501335124115"><a name="ph17501335124115"></a><a name="ph17501335124115"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: int16_t/uint16_t/half/int32_t/uint32_t/float/complex32/int64_t/uint64_t/complex64</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="18.61%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a>S</p></td>
<td class="cellrowborder" valign="top" width="81.39%" headers="mcps1.2.3.1.2 "><p id="p1775118537244"><a name="p1775118537244"></a><a name="p1775118537244"></a>Data type of the scalar source operand</p>
<p id="p45851742204115"><a name="p45851742204115"></a><a name="p45851742204115"></a><span id="ph9585942184118"><a name="ph9585942184118"></a><a name="ph9585942184118"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: int16_t/uint16_t/half/int32_t/uint32_t/float/complex32/int64_t/uint64_t/complex64</p></td>
</tr>
<tr id="row116701341181212"><td class="cellrowborder" valign="top" width="18.61%" headers="mcps1.2.3.1.1 "><p id="p146715417128"><a name="p146715417128"></a><a name="p146715417128"></a>mode</p></td>
<td class="cellrowborder" valign="top" width="81.39%" headers="mcps1.2.3.1.2 "><p id="p77520541653"><a name="p77520541653"></a><a name="p77520541653"></a>Selects the MERGING mode or ZEROING mode.</p>
<a name="ul1163765616511"></a><a name="ul1163765616511"></a><ul id="ul1163765616511"><li>**ZEROING**: Elements not selected by the mask are set to zero in **dstReg**.</li><li>**MERGING**: Not supported.</li></ul></td>
</tr>
<tr id="row6795538131917"><td class="cellrowborder" valign="top" width="18.61%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>U</p></td>
<td class="cellrowborder" valign="top" width="81.39%" headers="mcps1.2.3.1.2 "><p id="p5280148151414"><a name="p5280148151414"></a><a name="p5280148151414"></a><span id="ph19851723182011"><a name="ph19851723182011"></a><a name="ph19851723182011"></a>RegTensor type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user.</span></p></td>
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
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a><span id="ph2493837204215"><a name="ph2493837204215"></a><a name="ph2493837204215"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p6844125874315"><a name="p6844125874315"></a><a name="p6844125874315"></a>**srcReg**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>Source operand.</p>
<p id="p20179926154412"><a name="p20179926154412"></a><a name="p20179926154412"></a><span id="ph71795261444"><a name="ph71795261444"></a><a name="ph71795261444"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p>
<p id="p17140111519459"><a name="p17140111519459"></a><a name="p17140111519459"></a>The data type must be consistent with the destination operand.</p></td>
</tr>
<tr id="row891912431168"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p195756503168"><a name="p195756503168"></a><a name="p195756503168"></a>**scalarValue**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p457515071618"><a name="p457515071618"></a><a name="p457515071618"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p732824954017"><a name="p732824954017"></a><a name="p732824954017"></a>Source operand.</p>
<p id="p73715114429"><a name="p73715114429"></a><a name="p73715114429"></a>Type: scalar.</p>
<p id="p8784917184512"><a name="p8784917184512"></a><a name="p8784917184512"></a>The data type must be consistent with the destination operand.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>**mask**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p1559991025517"><a name="p1559991025517"></a><a name="p1559991025517"></a>Indicates the validity of elements in the source operand. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</p></td>
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
__simd_vf__ inline void MulsVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, T scalarValue, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;    
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Muls(dstReg, srcReg, scalarValue, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

