# Interleave<a name="ZH-CN_TOPIC_0000001952378500"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T22:24:41.233Z -->

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

Interleaves the elements in source operands **src0** and **src1** into target operands **dst0** and **dst1**. The interleaving arrangement is shown in the following figure, where each square represents one element:

![](../../../../figures/Interleave.png)

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T>
__simd_callee__ inline void Interleave(MaskReg& dst0, MaskReg& dst1, MaskReg& src0, MaskReg& src1)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table329841281718"></a>
<table><thead align="left"><tr id="row629871213174"><th class="cellrowborder" valign="top" width="49.95%" id="mcps1.2.3.1.1"><p id="p62981112131710"><a name="p62981112131710"></a><a name="p62981112131710"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="50.05%" id="mcps1.2.3.1.2"><p id="p12981512141717"><a name="p12981512141717"></a><a name="p12981512141717"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row62981812101717"><td class="cellrowborder" valign="top" width="49.95%" headers="mcps1.2.3.1.1 "><p id="p1929871211179"><a name="p1929871211179"></a><a name="p1929871211179"></a>T</p></td>
<td class="cellrowborder" valign="top" width="50.05%" headers="mcps1.2.3.1.2 "><p id="p162981212111710"><a name="p162981212111710"></a><a name="p162981212111710"></a>Data type supported by **MaskReg**, which determines the bit width of interleaving. For example, for the uint32_t type, interleaving is performed in groups of 4 bits.</p>
<p id="p3966152216478"><a name="p3966152216478"></a><a name="p3966152216478"></a><span id="ph1966152212477"><a name="ph1966152212477"></a><a name="ph1966152212477"></a>Ascend 950PR/Ascend 950DT</span> supports the following data types: b8/b16/b32.</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>**dst0**</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Target operand.</p></td>
</tr>
<tr id="row16248184111201"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1064220453202"><a name="p1064220453202"></a><a name="p1064220453202"></a>**dst1**</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p396713598201"><a name="p396713598201"></a><a name="p396713598201"></a>Target operand.</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p59747391278"><a name="p59747391278"></a><a name="p59747391278"></a>**src0**</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p53355414286"><a name="p53355414286"></a><a name="p53355414286"></a>Source operand.</p></td>
</tr>
<tr id="row2521428183011"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p58511219123111"><a name="p58511219123111"></a><a name="p58511219123111"></a>**src1**</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p175262810308"><a name="p175262810308"></a><a name="p175262810308"></a>Source operand.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section932512912207"></a>

```
template <typename T>
__simd_vf__ inline void InterleaveDeInterleaveVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg maskFull = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    AscendC::Reg::MaskReg maskM3 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::M3>();
    AscendC::Reg::MaskReg newMask0;
    AscendC::Reg::MaskReg newMask1;
    AscendC::Reg::Interleave<T>(newMask0, newMask1, maskFull, maskM3);
    AscendC::Reg::DeInterleave<T>(newMask0, newMask1, newMask0, newMask1);
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Adds(srcReg, srcReg, 0, newMask0);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
    }
}
```

