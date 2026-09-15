# Select<a name="ZH-CN_TOPIC_0000001985457929"></a>

<!-- md-trans-meta sourceCommit=27a8d829e1498b83498ed8bfc14b3c9e69ae662d translatedAt=2026-08-27T22:27:46.620Z -->

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

Given two source operands **src0** and **src1**, elements are selected based on the bit values of **mask** to obtain the destination operand **dst**. The selection rule is as follows: when a bit of **mask** is 1, the element at the corresponding position is selected from **src0**; when the bit is 0, the element at the corresponding position is selected from **src1**.

## Prototype<a name="section620mcpsimp"></a>

```
__simd_callee__ inline void Select(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask);
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>dst</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Destination operand.</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p59747391278"><a name="p59747391278"></a><a name="p59747391278"></a>src0</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p53355414286"><a name="p53355414286"></a><a name="p53355414286"></a>Source operand.</p></td>
</tr>
<tr id="row2521428183011"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p58511219123111"><a name="p58511219123111"></a><a name="p58511219123111"></a>src1</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p175262810308"><a name="p175262810308"></a><a name="p175262810308"></a>Source operand.</p></td>
</tr>
<tr id="row849213245316"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p04921832195310"><a name="p04921832195310"></a><a name="p04921832195310"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p3926174212712"><a name="p3926174212712"></a><a name="p3926174212712"></a>Selects **src0** or **src1**.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section932512912207"></a>

The following example processes 32 `float` data elements and combines two masks of different ranges through `Select`. The first 16 bits of `maskFirstHalf` are 1 and the last 16 bits are 0, so the generated `selMask` uses the corresponding bits of `maskFirstQuarter` for the first 16 bits and the corresponding bits of `maskFullLength` for the last 16 bits. Finally, `StoreAlign` moves out only the source data corresponding to the valid bits in `selMask`.

```cpp
__simd_vf__ inline void SelectVF(__ubuf__ float* dstAddr, __ubuf__ float* srcAddr)
{
    AscendC::Reg::RegTensor<float> srcReg;
    AscendC::Reg::MaskReg maskFirstQuarter =
        AscendC::Reg::CreateMask<float, AscendC::Reg::MaskPattern::VL8>();
    AscendC::Reg::MaskReg maskFirstHalf =
        AscendC::Reg::CreateMask<float, AscendC::Reg::MaskPattern::VL16>();
    AscendC::Reg::MaskReg maskFullLength =
        AscendC::Reg::CreateMask<float, AscendC::Reg::MaskPattern::VL32>();
    AscendC::Reg::MaskReg selMask;
    // Take the first 16 bits from maskFirstQuarter and the last 16 bits from maskFullLength.
    AscendC::Reg::Select(selMask, maskFirstQuarter, maskFullLength, maskFirstHalf);
    // Move in only the first 32 elements indicated by maskFullLength.
    AscendC::Reg::LoadAlign<float, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY>(
        srcReg, srcAddr, 1, maskFullLength);
    AscendC::Reg::StoreAlign(dstAddr, srcReg, selMask);
}
```

The input data is as follows: the first 16 elements of `srcAddr` are `1.0`, the last 16 elements are `2.0`, and all 32 elements of `dstAddr` are `0.0`:

```text
srcAddr = [1.0, 1.0, ..., 1.0, 2.0, 2.0, ..., 2.0]
dstAddr = [0.0, 0.0, ..., 0.0]
```

After `Select` is executed, the first 8 bits of `selMask` are 1, bits 9 through 16 are 0, the last 16 bits are 1, and all remaining bits are 0. `selMask` is of the `MaskReg` type, so its values represent mask bits:

```text
selMask = [1, 1, ..., 1, 0, 0, ..., 0, 1, 1, ..., 1]
```

After `StoreAlign` is executed, the first 8 elements of `dstAddr` are `1.0`, elements 9 through 16 retain their initial value `0.0`, and the last 16 elements are `2.0`:

```text
dstAddr = [1.0, 1.0, ..., 1.0, 0.0, 0.0, ..., 0.0, 2.0, 2.0, ..., 2.0]
```
