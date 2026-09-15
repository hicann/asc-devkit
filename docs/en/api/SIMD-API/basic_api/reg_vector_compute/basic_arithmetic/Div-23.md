# Div<a name="ZH-CN_TOPIC_0000001929668264"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:42:52.531Z -->

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

Performs element-wise division on the input data **srcReg0** and **srcReg1** based on **mask**, and writes the result to **dstReg**. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000001957629005.png)

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Div(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.310000000000002%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.69%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.310000000000002%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.69%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Operand data type.</p>
<p id="p565812586214"><a name="p565812586214"></a><a name="p565812586214"></a><span id="ph13658558725"><a name="ph13658558725"></a><a name="ph13658558725"></a>Ascend 950PR/Ascend 950DT</span>, supported data types are: uint16_t, int16_t, half, uint32_t, int32_t, float, complex32, int64_t, uint64_t, complex64.</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="18.310000000000002%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a>mode</p></td>
<td class="cellrowborder" valign="top" width="81.69%" headers="mcps1.2.3.1.2 "><p id="p1315151232819"><a name="p1315151232819"></a><a name="p1315151232819"></a>Configurable as an enumeration of **MaskMergeMode** or a structure pointer of **DivSpecificMode**.</p>
<a name="ul1131085281310"></a><a name="ul1131085281310"></a><ul id="ul1131085281310"><li>Configure **MaskMergeMode** to select the **MERGING** mode or **ZEROING** mode.<a name="ul1163765616511"></a><a name="ul1163765616511"></a><ul id="ul1163765616511"><li>**ZEROING**: Elements not filtered by **mask** are set to zero in **dstReg**.</li><li>**MERGING**: Not supported yet.</li></ul>
</li><li>Configure **DivSpecificMode**<pre class="screen" id="screen18481935171419"><a name="screen18481935171419"></a><a name="screen18481935171419"></a>enum class DivAlgo {
    INTRINSIC = 0,
    DIFF_COMPENSATION,
    PRECISION_1ULP_FTZ_TRUE,
    PRECISION_0ULP_FTZ_TRUE,
    PRECISION_0ULP_FTZ_FALSE,
    PRECISION_1ULP_FTZ_FALSE
};
struct DivSpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING,
    bool precisionMode = false;
    DivAlgo algo = DivAlgo::INTRINSIC;
};</pre>
<p id="p175231030713"><a name="p175231030713"></a><a name="p175231030713"></a>When **precisionMode** is true, higher-precision **Div** computation is enabled, using the difference compensation algorithm to obtain the result, with a maximum precision error of 0 ulp. Currently, it takes effect only for the float data type.</p>
</li></ul>
<a name="ul11875104117375"></a><a name="ul11875104117375"></a><ul id="ul11875104117375"><li>**algo**: Used to configure the **Subnormal** mode.<a name="ul1639612391388"></a><a name="ul1639612391388"></a><ul id="ul1639612391388"><li>**DivAlgo::INTRINSIC** and **DivAlgo::PRECISION_1ULP_FTZ_TRUE** use a single instruction to obtain the result, with a maximum precision error of 1 ulp.</li><li>**DivAlgo::DIFF_COMPENSATION** and **DivAlgo::PRECISION_0ULP_FTZ_TRUE** use the difference compensation algorithm to obtain the result, with a maximum precision error of 0 ulp. Currently, the algorithm supports the float and complex64 data types.</li><li>**DivAlgo::PRECISION_0ULP_FTZ_FALSE** supports Subnormal data computation, using the difference compensation algorithm to obtain the result, with a maximum precision error of 0 ulp. Currently, the algorithm supports the float data type.</li><li>**DivAlgo::PRECISION_1ULP_FTZ_FALSE** supports Subnormal data computation, using a single instruction to obtain the result, with a maximum precision error of 1 ulp. Currently, the algorithm supports the half and float data types.</li></ul>
</li></ul></td>
</tr>
<tr id="row11614203610563"><td class="cellrowborder" valign="top" width="18.310000000000002%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>U</p></td>
<td class="cellrowborder" valign="top" width="81.69%" headers="mcps1.2.3.1.2 "><p id="p15901115914145"><a name="p15901115914145"></a><a name="p15901115914145"></a><span id="ph19851723182011"><a name="ph19851723182011"></a><a name="ph19851723182011"></a>RegTensor type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically inferred by the compiler and does not need to be specified by the user.</span></p></td>
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
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a><span id="ph134278176129"><a name="ph134278176129"></a><a name="ph134278176129"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p19574165615129"><a name="p19574165615129"></a><a name="p19574165615129"></a>srcReg0</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>Source operand.</p>
<p id="p2733105861210"><a name="p2733105861210"></a><a name="p2733105861210"></a><span id="ph87333589121"><a name="ph87333589121"></a><a name="ph87333589121"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p>
<p id="p1484485824312"><a name="p1484485824312"></a><a name="p1484485824312"></a>The data type must be consistent with that of the destination operand.</p></td>
</tr>
<tr id="row11773440341"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p10132448173420"><a name="p10132448173420"></a><a name="p10132448173420"></a>srcReg1</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p151325484342"><a name="p151325484342"></a><a name="p151325484342"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p181326485341"><a name="p181326485341"></a><a name="p181326485341"></a>Source operand.</p>
<p id="p1542710121318"><a name="p1542710121318"></a><a name="p1542710121318"></a><span id="ph194272041316"><a name="ph194272041316"></a><a name="ph194272041316"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p>
<p id="p12132154853419"><a name="p12132154853419"></a><a name="p12132154853419"></a>The data type must be consistent with that of the destination operand.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p1559991025517"><a name="p1559991025517"></a><a name="p1559991025517"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>Valid indication of the operation on source operand elements. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
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
__simd_vf__ inline void DivVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    // High-precision mode.
    // static constexpr AscendC::Reg::DivSpecificMode mode = {AscendC::Reg::MaskMergeMode::ZEROING, true};
    // Subnormal mode.
    // static constexpr AscendC::Reg::DivSpecificMode mode = {AscendC::Reg::MaskMergeMode::ZEROING, true, DivAlgo::PRECISION_0ULP_FTZ_FALSE};
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Div(dstReg, srcReg0, srcReg1, mask);
        // High-precision mode/Subnormal mode.
        // AscendC::Reg::Div<T, &mode>(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

