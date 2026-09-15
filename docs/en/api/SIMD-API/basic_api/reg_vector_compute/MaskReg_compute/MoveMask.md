# MoveMask<a name="ZH-CN_TOPIC_0000001985457933"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T22:27:08.342Z -->

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

Reads the mask value from the special registers SPR\{MASK1, MASK0\} and writes it to the return value **MaskReg** according to the data type format.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T> 
__simd_callee__ inline MaskReg MoveMask()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Data type that determines the mask value write format:</p>
<a name="ul17688174154916"></a><a name="ul17688174154916"></a><ul id="ul17688174154916"><li>For the **b16** type, the complete 128-bit {**MASK1**, **MASK0**} data is read, and each bit is replicated into 2 bits and written to the **MaskReg** return value.</li><li>For the **b32** type, the 64-bit **MASK0** data is read, and each bit is replicated into 4 bits and written to the **MaskReg** return value.</li></ul></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Returns the **MaskReg** read from the special register.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section932512912207"></a>

```
template <typename T>
__simd_vf__ inline void MoveMaskVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg addMask = AscendC::Reg::MoveMask<T>();
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Adds(srcReg, srcReg, 0, addMask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
    }
}
```

