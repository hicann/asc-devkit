# Arange<a name="ZH-CN_TOPIC_0000001960926493"></a>

<!-- md-trans-meta sourceCommit=c5471ceb6d0822ce77b3dc5a1d15ba9dd72783a5 translatedAt=2026-08-27T22:21:06.532Z -->

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

In increment mode, this function uses the value of the passed **scalarValue** as the start value to generate an incrementing index sequence. In decrement mode, this function uses the value of the passed **scalarValue** as the end value to generate a decrementing index sequence, and stores the index in **dstReg**.

**Figure 1**  Arange increment mode

![Arange diagram](../../../../figures/reg_arange_in.png)

**Figure 2**  Arange decrement mode

![Arange diagram](../../../../figures/reg_arange_de.png)

## Prototype<a name="section520771712327"></a>

```
template <typename T = DefaultType, IndexOrder order = IndexOrder::INCREASE_ORDER, typename U, typename S>
__simd_callee__ inline void Arange(S& dstReg, U scalarValue);
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.21%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.78999999999999%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.21%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.78999999999999%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Data type of the target operand.</p>
<p id="p162947163119"><a name="p162947163119"></a><a name="p162947163119"></a><span id="ph42907183115"><a name="ph42907183115"></a><a name="ph42907183115"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: int8_t/int16_t/int32_t/half/float/int64_t</p></td>
</tr>
<tr id="row137401528960"><td class="cellrowborder" valign="top" width="18.21%" headers="mcps1.2.3.1.1 "><p id="p07403281766"><a name="p07403281766"></a><a name="p07403281766"></a>U</p></td>
<td class="cellrowborder" valign="top" width="81.78999999999999%" headers="mcps1.2.3.1.2 "><p id="p18151938469"><a name="p18151938469"></a><a name="p18151938469"></a>Data type of the starting index value.</p>
<p id="p102066298311"><a name="p102066298311"></a><a name="p102066298311"></a><span id="ph15207152912319"><a name="ph15207152912319"></a><a name="ph15207152912319"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: int8_t/int16_t/int32_t/half/float/int64_t</p>
<p id="p490534311431"><a name="p490534311431"></a><a name="p490534311431"></a>t</p></td>
</tr>
<tr id="row1198511361014"><td class="cellrowborder" valign="top" width="18.21%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>S</p></td>
<td class="cellrowborder" valign="top" width="81.78999999999999%" headers="mcps1.2.3.1.2 "><p id="p131047476612"><a name="p131047476612"></a><a name="p131047476612"></a>RegTensor type of the target operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be explicitly specified by the user.</p></td>
</tr>
</tbody>
</table>

**Table 2**  Function parameter description

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>dstReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>Target operand.</p>
<p id="p37015312238"><a name="p37015312238"></a><a name="p37015312238"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p6844125874315"><a name="p6844125874315"></a><a name="p6844125874315"></a>scalarValue</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p9761550745"><a name="p9761550745"></a><a name="p9761550745"></a>Source operand.</p>
<p id="p1976165012411"><a name="p1976165012411"></a><a name="p1976165012411"></a>Scalar type.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section177921451558"></a>

None

## Example<a name="section642mcpsimp"></a>

```
template<typename T>
__simd_vf__ inline void ArangeVF(__ubuf__ T* dstAddr, T scalarValue, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    mask = AscendC::Reg::CreateMask<T>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::Arange(dstReg, scalarValue);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
