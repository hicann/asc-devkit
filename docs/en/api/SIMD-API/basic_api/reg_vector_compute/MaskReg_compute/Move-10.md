# Move<a name="ZH-CN_TOPIC_0000001985577785"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T22:25:52.533Z -->

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

Copies the elements in **src** to the corresponding positions in **dst**. If an input **mask** is provided, only the valid elements selected by the mask are copied, and invalid elements are filled with 0.

## Prototype<a name="section620mcpsimp"></a>

-   With input **mask**

    ```
    __simd_callee__ inline void Move(MaskReg& dst, MaskReg& src, MaskReg& mask)
    ```

-   Without input **mask**

    ```
    __simd_callee__ inline void Move(MaskReg& dst, MaskReg& src)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>dst</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Destination operand.</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p59747391278"><a name="p59747391278"></a><a name="p59747391278"></a>src</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p53355414286"><a name="p53355414286"></a><a name="p53355414286"></a>Source operand.</p></td>
</tr>
<tr id="row3926164202714"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1892618426271"><a name="p1892618426271"></a><a name="p1892618426271"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p3926174212712"><a name="p3926174212712"></a><a name="p3926174212712"></a>Indicates which bits are valid during the copy process.</p></td>
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
__simd_vf__ inline void MoveVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg src = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
    AscendC::Reg::MaskReg dst;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Move(dst, src, mask);
        AscendC::Reg::Adds(srcReg, srcReg, 0, dst);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
    }
}
```

