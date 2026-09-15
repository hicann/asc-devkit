# Truncate<a name="ZH-CN_TOPIC_0000001929827676"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T22:04:17.835Z -->

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

Truncates the floating-point elements of the source operand to integer positions while keeping the data type of the source operand unchanged.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, RoundMode roundMode = RoundMode::CAST_NONE, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void Truncate(S& dstReg, S& srcReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.48%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.52000000000001%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.48%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.52000000000001%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Operand data type.</p>
<p id="p18316267203"><a name="p18316267203"></a><a name="p18316267203"></a><span id="ph1831122612203"><a name="ph1831122612203"></a><a name="ph1831122612203"></a>Ascend 950PR/Ascend 950DT</span>: The data types supported by dstReg/srcReg are half, float, and bfloat16_t.</p></td>
</tr>
<tr id="row878242318442"><td class="cellrowborder" valign="top" width="18.48%" headers="mcps1.2.3.1.1 "><p id="p1578212233441"><a name="p1578212233441"></a><a name="p1578212233441"></a>roundMode</p></td>
<td class="cellrowborder" valign="top" width="81.52000000000001%" headers="mcps1.2.3.1.2 "><p id="p10782152316444"><a name="p10782152316444"></a><a name="p10782152316444"></a>Rounding mode. The values are as follows:</p>
<a name="ul10735652193110"></a><a name="ul10735652193110"></a><ul id="ul10735652193110"><li>**RoundMode::CAST_NONE**: Default value. In scenarios where conversion causes precision loss, this mode is equivalent to **CAST_RINT**; in other scenarios, it has no effect.</li><li>**RoundMode::CAST_RINT**: This mode returns the integer closest to the argument. If two integers are equally close, the even one is returned.</li><li>**RoundMode::CAST_ROUND**: Round mode, which rounds to the nearest integer.</li><li>**RoundMode::CAST_FLOOR**: Floor mode, which rounds down.</li><li>**RoundMode::CAST_CEIL**: Ceil mode, which rounds up.</li><li>**RoundMode::CAST_TRUNC**: Truncation mode, which rounds toward zero and discards the fractional part.</li></ul></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="18.48%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a>mode</p></td>
<td class="cellrowborder" valign="top" width="81.52000000000001%" headers="mcps1.2.3.1.2 "><p id="p77520541653"><a name="p77520541653"></a><a name="p77520541653"></a>Selects the MERGING mode or ZEROING mode.</p>
<a name="ul1163765616511"></a><a name="ul1163765616511"></a><ul id="ul1163765616511"><li>**ZEROING**: Elements not filtered by the mask are set to zero in dst.</li><li>**MERGING**: Currently not supported.</li></ul></td>
</tr>
<tr id="row6323202343820"><td class="cellrowborder" valign="top" width="18.48%" headers="mcps1.2.3.1.1 "><p id="p432372303816"><a name="p432372303816"></a><a name="p432372303816"></a>S</p></td>
<td class="cellrowborder" valign="top" width="81.52000000000001%" headers="mcps1.2.3.1.2 "><p id="p93238232385"><a name="p93238232385"></a><a name="p93238232385"></a>**srcReg/dstReg** type, for example **RegTensor&lt;float&gt;**, which is automatically deduced by the compiler and does not need to be filled in by the user.</p></td>
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
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p19574165615129"><a name="p19574165615129"></a><a name="p19574165615129"></a>srcReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>Source operand.</p>
<p id="p7123111612517"><a name="p7123111612517"></a><a name="p7123111612517"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p>
<p id="p1484485824312"><a name="p1484485824312"></a><a name="p1484485824312"></a>The data type of the source operand must be consistent with that of the destination operand.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p1559991025517"><a name="p1559991025517"></a><a name="p1559991025517"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>Validity indicator for source operand element operations. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   In non-saturation mode, if the input is nan, the output is nan; if the input value exceeds the maximum/minimum value range of the input type, the output is +/-inf.
-   In saturation mode, if the input is nan, the output is 0; if the input value exceeds the maximum/minimum value range of the input type, it is saturated to the maximum/minimum value.
-   The float type supports only the non-saturation mode.

## Example<a name="section642mcpsimp"></a>

```
template<typename T>
__simd_vf__ inline void TruncVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::Truncate<T, AscendC::RoundMode::CAST_FLOOR, AscendC::Reg::MaskMergeMode::ZEROING>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

