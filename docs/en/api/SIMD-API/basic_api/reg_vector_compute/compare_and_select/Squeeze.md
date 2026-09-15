# Squeeze<a name="ZH-CN_TOPIC_0000002009320090"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:26:23.576Z -->

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
- Atlas inference product AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference product Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training product: Not supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Copies the valid elements selected by **mask** from the input **srcReg** to **dstReg** in sequence. The valid elements are arranged contiguously from low to high in **dstReg**, and the remaining positions in **dstReg** are set to 0.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, GatherMaskMode store = GatherMaskMode::NO_STORE_REG, typename U>
__simd_callee__ inline void Squeeze(U& dstReg, U& srcReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.77%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.23%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.77%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>**T**</p></td>
<td class="cellrowborder" valign="top" width="81.23%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>**T** indicates the data type of the destination operand and the source operand.</p>
<p id="p159731458125912"><a name="p159731458125912"></a><a name="p159731458125912"></a><span id="ph1497313580592"><a name="ph1497313580592"></a><a name="ph1497313580592"></a>Ascend 950PR/Ascend 950DT</span> supports the following data types: uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/half/float</p></td>
</tr>
<tr id="row3742113618507"><td class="cellrowborder" valign="top" width="18.77%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a>**store**</p></td>
<td class="cellrowborder" valign="top" width="81.23%" headers="mcps1.2.3.1.2 "><p id="p77520541653"><a name="p77520541653"></a><a name="p77520541653"></a>**GatherMaskMode** selects whether to store the total number of bytes of valid elements into the AR register. For details about the AR register, see <a href="../../special_register_access/GetSpr.md#table37531617424">Table 1</a>.</p>
<a name="ul1163765616511"></a><a name="ul1163765616511"></a><ul id="ul1163765616511"><li>**NO_STORE_REG**: The total number of bytes of valid elements is not stored into the AR register.</li><li>**STORE_REG**: The total number of bytes of valid elements is stored into the AR register.</li></ul></td>
</tr>
<tr id="row104455914519"><td class="cellrowborder" valign="top" width="18.77%" headers="mcps1.2.3.1.1 "><p id="p144175913451"><a name="p144175913451"></a><a name="p144175913451"></a>**U**</p></td>
<td class="cellrowborder" valign="top" width="81.23%" headers="mcps1.2.3.1.2 "><p id="p34418597453"><a name="p34418597453"></a><a name="p34418597453"></a>**srcReg**/**dstReg** are of the **RegTensor** type, for example, **RegTensor&lt;uint32_t&gt;**, which is automatically deduced by the compiler and does not need to be specified by the user.</p></td>
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
<tr id="row198393197176"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1664684418171"><a name="p1664684418171"></a><a name="p1664684418171"></a>**srcReg**</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p9839171971716"><a name="p9839171971716"></a><a name="p9839171971716"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p779095312179"><a name="p779095312179"></a><a name="p779095312179"></a>Source operand.</p>
<p id="p97903531171"><a name="p97903531171"></a><a name="p97903531171"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a></p></td>
</tr>
<tr id="row17332152321719"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1538745616196"><a name="p1538745616196"></a><a name="p1538745616196"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p73331523101710"><a name="p73331523101710"></a><a name="p73331523101710"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p1559991025517"><a name="p1559991025517"></a><a name="p1559991025517"></a>**mask** controls the elements involved in computation within each iteration.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section177921451558"></a>

When **store** is set to **STORE_REG**, due to hardware constraints, the **StoreUnAlign** instruction and the **Squeeze** instruction must be used alternately, for example:

```
Squeeze(dstVreg, srcVreg, mask);
StoreUnAlign(dstAddr, dstVreg, ureg);
Squeeze(dstVreg, srcVreg, mask);
StoreUnAlign(dstAddr, dstVreg, ureg);
```

## Example<a name="section642mcpsimp"></a>

```
template<typename T>
__simd_vf__ inline void SqueezeVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::UnalignRegForStore ureg;
    AscendC::Reg::MaskReg sqzMask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::H>();
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(srcReg0, srcAddr, oneRepeatSize);
        AscendC::Reg::Squeeze<T, AscendC::Reg::GatherMaskMode::STORE_REG>(srcReg1, srcReg0, sqzMask);
        AscendC::Reg::StoreUnAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstAddr, srcReg1, ureg);
     }
     AscendC::Reg::StoreUnAlignPost(dstAddr, ureg);
}
```
