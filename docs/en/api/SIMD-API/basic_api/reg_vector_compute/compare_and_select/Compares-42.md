# Compares<a name="ZH-CN_TOPIC_0000001956986933"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T21:25:57.558Z -->

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

Compares the elements of a Tensor with a scalar element by element. If the comparison result is true, the corresponding bit in the output result is set to 1; otherwise, it is set to 0.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, CMPMODE mode = CMPMODE::EQ, typename U, typename S>
__simd_callee__ inline void Compares(MaskReg& dst, U& srcReg, S scalarValue, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="14.95%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="85.05%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="14.95%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="85.05%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Data type of the vector source operand.</p>
<p id="p12347511192513"><a name="p12347511192513"></a><a name="p12347511192513"></a><span id="ph434701113253"><a name="ph434701113253"></a><a name="ph434701113253"></a>Ascend 950PR/Ascend 950DT</span> supports the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="14.95%" headers="mcps1.2.3.1.1 "><p id="p464337182920"><a name="p464337182920"></a><a name="p464337182920"></a>mode</p></td>
<td class="cellrowborder" valign="top" width="85.05%" headers="mcps1.2.3.1.2 "><p id="p025716534299"><a name="p025716534299"></a><a name="p025716534299"></a>Comparison mode. The following values are supported:</p>
<a name="ul14429497516"></a><a name="ul14429497516"></a><ul id="ul14429497516"><li>LT: less than</li><li>GT: greater than</li></ul>
<a name="ul147379458500"></a><a name="ul147379458500"></a><ul id="ul147379458500"><li>GE: greater than or equal to</li><li>EQ: equal to</li><li>NE: not equal to</li><li>LE: less than or equal to</li></ul></td>
</tr>
<tr id="row962916316272"><td class="cellrowborder" valign="top" width="14.95%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>U</p></td>
<td class="cellrowborder" valign="top" width="85.05%" headers="mcps1.2.3.1.2 "><p id="p47566332405"><a name="p47566332405"></a><a name="p47566332405"></a>RegTensor type of the source operand, which is automatically deduced by the compiler and does not need to be specified by the user.</p></td>
</tr>
<tr id="row3625143218279"><td class="cellrowborder" valign="top" width="14.95%" headers="mcps1.2.3.1.1 "><p id="p1062543242714"><a name="p1062543242714"></a><a name="p1062543242714"></a>S</p></td>
<td class="cellrowborder" valign="top" width="85.05%" headers="mcps1.2.3.1.2 "><p id="p16251432182715"><a name="p16251432182715"></a><a name="p16251432182715"></a>Scalar data type.</p>
<p id="p2711145253"><a name="p2711145253"></a><a name="p2711145253"></a><span id="ph27414102512"><a name="ph27414102512"></a><a name="ph27414102512"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/half/float/bfloat16_t/uint64_t/int64_t</p></td>
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
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>dst</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p159941254191217"><a name="p159941254191217"></a><a name="p159941254191217"></a>**MaskReg** type, destination operand.</p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p6844125874315"><a name="p6844125874315"></a><a name="p6844125874315"></a>srcReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>Source operand.</p>
<p id="p5261335123513"><a name="p5261335123513"></a><a name="p5261335123513"></a><span id="ph15261035183514"><a name="ph15261035183514"></a><a name="ph15261035183514"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p></td>
</tr>
<tr id="row944414143710"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1059534613404"><a name="p1059534613404"></a><a name="p1059534613404"></a>scalarValue</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p14444941113712"><a name="p14444941113712"></a><a name="p14444941113712"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p13444154119374"><a name="p13444154119374"></a><a name="p13444154119374"></a>Source operand.</p>
<p id="p11311221910"><a name="p11311221910"></a><a name="p11311221910"></a>The type is scalar.</p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>Valid indication of the source operand element operation. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section12223121385116"></a>

```
template<typename T>
__simd_vf__ inline void ComparesVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, T scalarValue, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::MaskReg cmpMaskReg;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);       
        AscendC::Reg::Compares<T, AscendC::CMPMODE::EQ>(cmpMaskReg, srcReg0, scalarValue, mask);
        AscendC::Reg::Select(dstReg, srcReg0, srcReg1, cmpMaskReg);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

