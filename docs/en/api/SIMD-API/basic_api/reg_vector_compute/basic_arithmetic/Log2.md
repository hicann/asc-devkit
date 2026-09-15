# Log2<a name="ZH-CN_TOPIC_0000002008231220"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:47:17.428Z -->

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

Performs a base-2 logarithm operation on the input data **srcReg** according to **mask**, and writes the result to **dstReg**. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002008389576.png)

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Log2(U& dstReg, U& srcReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.529999999999998%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.47%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.529999999999998%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.47%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Operand data type.</p>
<p id="p657315412482"><a name="p657315412482"></a><a name="p657315412482"></a><span id="ph7573654184814"><a name="ph7573654184814"></a><a name="ph7573654184814"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: half, float.</p></td>
</tr>
<tr id="row9587154117217"><td class="cellrowborder" valign="top" width="18.529999999999998%" headers="mcps1.2.3.1.1 "><p id="p1756419170189"><a name="p1756419170189"></a><a name="p1756419170189"></a>mode</p></td>
<td class="cellrowborder" valign="top" width="81.47%" headers="mcps1.2.3.1.2 "><div class="p" id="p4367181482310"><a name="p4367181482310"></a><a name="p4367181482310"></a>Can be configured as an enumeration of MaskMergeMode or a struct instance of Log2SpecificMode.<a name="ul1131085281310"></a><a name="ul1131085281310"></a><ul id="ul1131085281310"><li>MaskMergeMode, selects the MERGING mode or the ZEROING mode.<a name="ul13721340113818"></a><a name="ul13721340113818"></a><ul id="ul13721340113818"><li>ZEROING, elements not filtered by the mask are set to zero in dstReg.</li><li>MERGING, not supported.</li></ul>
</li><li>Log2SpecificMode, defined as follows:<pre class="screen" id="screen18481935171419"><a name="screen18481935171419"></a><a name="screen18481935171419"></a>enum class Log2Algo {
    INTRINSIC = 0,
    PRECISION_1ULP_FTZ_TRUE,
    PRECISION_1ULP_FTZ_FALSE,
};
struct Log2SpecificMode{
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING,
    Log2Algo algo = Log2Algo::INTRINSIC;
};</pre>
<a name="ul196783155815"></a><a name="ul196783155815"></a><ul id="ul196783155815"><li>mrgMode: selects the MERGING mode or the ZEROING mode.</li><li>algo: used to configure the Subnormal mode.<a name="ul111801915217"></a><a name="ul111801915217"></a><ul id="ul111801915217"><li>Log2Algo::INTRINSIC and Log2Algo::PRECISION_1ULP_FTZ_TRUE use a single instruction to compute the result, with all Subnormal values approximated to 0.</li><li>Log2Algo::PRECISION_1ULP_FTZ_FALSE supports computation on Subnormal data.</li></ul>
</li></ul>
</li></ul>
</div></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="18.529999999999998%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>U</p></td>
<td class="cellrowborder" valign="top" width="81.47%" headers="mcps1.2.3.1.2 "><p id="p20196203834110"><a name="p20196203834110"></a><a name="p20196203834110"></a><span id="ph20196138204118"><a name="ph20196138204118"></a><a name="ph20196138204118"></a>RegTensor type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically inferred by the compiler and does not need to be specified by the user.</span></p></td>
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
<p id="p209611316124917"><a name="p209611316124917"></a><a name="p209611316124917"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p>
<p id="p7123111612517"><a name="p7123111612517"></a><a name="p7123111612517"></a>The data type of the source operand must be consistent with that of the destination operand.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p15901115914145"><a name="p15901115914145"></a><a name="p15901115914145"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>Valid indication of the operation on source operand elements. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

When **src** is -0, the output is -inf.

## Example<a name="section642mcpsimp"></a>

```
template<typename T>
__simd_vf__ inline void Log2VF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, 
uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    // Subnormal mode.
    // static constexpr AscendC::Reg::Log2SpecificMode mode = {MaskMergeMode::ZEROING, Log2Algo::PRECISION_1ULP_FTZ_FALSE};
    for (uint16_t i = 0; i < repeatTimes; i++) {     
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Log2(dstReg, srcReg, mask);
        // Subnormal mode.
        // AscendC::Reg::Log2<T, &mode>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

