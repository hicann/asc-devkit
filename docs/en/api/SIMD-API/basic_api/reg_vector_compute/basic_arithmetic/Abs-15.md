# Abs<a name="ZH-CN_TOPIC_0000001956810269"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:39:10.559Z -->

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

Takes the absolute value of each valid element in **srcReg** and writes the result to the corresponding position in **dstReg**.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Abs(U& dstReg, U& srcReg, MaskReg& mask)

template <typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S, typename V>
__simd_callee__ inline void Abs(S& dstReg, V& srcReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.25%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.75%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.25%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.75%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Operand data type.</p>
<p id="p3966152216478"><a name="p3966152216478"></a><a name="p3966152216478"></a><span id="ph1966152212477"><a name="ph1966152212477"></a><a name="ph1966152212477"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: int8_t/int16_t/int32_t/int64_t/half/float.</p></td>
</tr>
<tr id="row8105194616578"><td class="cellrowborder" valign="top" width="18.25%" headers="mcps1.2.3.1.1 "><p id="p71509475576"><a name="p71509475576"></a><a name="p71509475576"></a>U</p></td>
<td class="cellrowborder" valign="top" width="81.75%" headers="mcps1.2.3.1.2 "><p id="p9150347175710"><a name="p9150347175710"></a><a name="p9150347175710"></a>Source operand data type.</p>
<p id="p515064716579"><a name="p515064716579"></a><a name="p515064716579"></a><span id="ph14150547175719"><a name="ph14150547175719"></a><a name="ph14150547175719"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: complex32/complex64; when U is of the complex32 type, T must be of the half type; when U is of the complex64 type, T must be of the float type.</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="18.25%" headers="mcps1.2.3.1.1 "><p id="p1756419170189"><a name="p1756419170189"></a><a name="p1756419170189"></a>mode</p></td>
<td class="cellrowborder" valign="top" width="81.75%" headers="mcps1.2.3.1.2 "><p id="p16933245348"><a name="p16933245348"></a><a name="p16933245348"></a>Selects the MERGING mode or ZEROING mode.</p>
<a name="ul49337457411"></a><a name="ul49337457411"></a><ul id="ul49337457411"><li>In ZEROING mode, elements not filtered by the mask are set to zero in dstReg.</li><li>MERGING mode is currently not supported.</li></ul></td>
</tr>
<tr id="row154799208400"><td class="cellrowborder" valign="top" width="18.25%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>S</p></td>
<td class="cellrowborder" valign="top" width="81.75%" headers="mcps1.2.3.1.2 "><p id="p131047476612"><a name="p131047476612"></a><a name="p131047476612"></a><span id="ph6705211132010"><a name="ph6705211132010"></a><a name="ph6705211132010"></a>RegTensor type of the destination operand, for example, RegTensor&lt;half&gt;. It is automatically deduced by the compiler and does not need to be specified by the user.</span></p></td>
</tr>
<tr id="row954012741914"><td class="cellrowborder" valign="top" width="18.25%" headers="mcps1.2.3.1.1 "><p id="p9541167151915"><a name="p9541167151915"></a><a name="p9541167151915"></a>V</p></td>
<td class="cellrowborder" valign="top" width="81.75%" headers="mcps1.2.3.1.2 "><p id="p1254114771913"><a name="p1254114771913"></a><a name="p1254114771913"></a><span id="ph82484115207"><a name="ph82484115207"></a><a name="ph82484115207"></a>RegTensor type of the source operand, for example, RegTensor&lt;half&gt;. It is automatically deduced by the compiler and does not need to be specified by the user.</span></p></td>
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
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a><span id="ph20846151694212"><a name="ph20846151694212"></a><a name="ph20846151694212"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p19574165615129"><a name="p19574165615129"></a><a name="p19574165615129"></a>srcReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>Source operand.</p>
<p id="p7123111612517"><a name="p7123111612517"></a><a name="p7123111612517"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a><span id="ph1254614714212"><a name="ph1254614714212"></a><a name="ph1254614714212"></a>Valid indication of the operation on source operand elements. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

If the computation result of integer data exceeds the representable range of the data type, non-saturating truncation is applied. For example, for the int8 type, if **srcReg** is -128, its absolute value 128 is truncated to -128.

## Example<a name="section642mcpsimp"></a>

```
template<typename T>
__simd_vf__ inline void AbsVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, 
 uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask; 
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Abs(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}

template<typename T, typename U>
__simd_vf__ inline void AbsVF(__ubuf__ U* dstAddr, __ubuf__ T* srcAddr, uint32_t count, 
 uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<U> dstReg;
    AscendC::Reg::MaskReg mask; 
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Abs(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

