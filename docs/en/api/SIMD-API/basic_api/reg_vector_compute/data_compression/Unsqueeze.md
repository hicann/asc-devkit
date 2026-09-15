# Unsqueeze<a name="ZH-CN_TOPIC_0000002009161778"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T22:17:52.416Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Uses **dstReg** as both the source operand and the destination operand to unpack data according to **mask**. Unpacking method: the 0th element in **dstReg** is set to 0, and the i-th element in **dstReg** equals the number of 1s from the 0th to the \(i-1\)-th element in **mask**. The most significant bit of **mask** is ignored and does not participate in the counting. Taking the `uint8_t` type as an example, the algorithm logic is expressed as follows:

```
dstReg[0] = 0;
for(int i = 1; i < AscendC::GetVecLen() / sizeof(uint8_t); i++){
    dstReg[i] = mask[i - 1] ? (dstReg[i - 1] + 1) : dstReg[i - 1];
}
```

where AscendC::GetVecLen\(\) / sizeof\(uint8\_t\) is the number of elements contained in **dstReg**.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, typename U>
__simd_callee__ inline void Unsqueeze(U& dstReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.970000000000002%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.03%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.970000000000002%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="81.03%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Data type of the destination operand.</p>
<p id="p1474319251999"><a name="p1474319251999"></a><a name="p1474319251999"></a><span id="ph191588264914"><a name="ph191588264914"></a><a name="ph191588264914"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t.</p></td>
</tr>
<tr id="row2013785594119"><td class="cellrowborder" valign="top" width="18.970000000000002%" headers="mcps1.2.3.1.1 "><p id="p141384557419"><a name="p141384557419"></a><a name="p141384557419"></a>U</p></td>
<td class="cellrowborder" valign="top" width="81.03%" headers="mcps1.2.3.1.2 "><p id="p1513845554113"><a name="p1513845554113"></a><a name="p1513845554113"></a>RegTensor type of the destination operand, which is automatically deduced by the compiler and does not need to be specified by the user.</p></td>
</tr>
</tbody>
</table>

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| **dstReg** | Input/Output | Source operand and destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| **mask** | Input | Provides the decompression information for **dstReg**.<br>The type is [MaskReg](../register_data_types/MaskReg.md). |

## Return Value

None

## Data Types

Supported data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t.

## Constraints<a name="section177921451558"></a>

None

## Example<a name="section642mcpsimp"></a>

```
template<typename T>
__simd_vf__ inline void UnsqueezeVF(__ubuf__ T* dstAddr, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    mask = AscendC::Reg::CreateMask<T>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::Unsqueeze(dstReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
