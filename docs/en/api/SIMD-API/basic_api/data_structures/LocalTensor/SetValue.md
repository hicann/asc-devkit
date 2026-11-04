# SetValue<a name="ZH-CN_TOPIC_0000002364845697"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:03:32.357Z pushedAt=2026-10-26T07:41:44.361Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/data_structures/LocalTensor/SetValue_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

Sets a value in **LocalTensor**.

**This API is supported only when the TPosition of LocalTensor is VECIN/VECCALC/VECOUT.**

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T1> __aicore__ inline __inout_pipe__(S)
void SetValue(const uint32_t index, const T1 value) const
```

## Parameter Description<a name="section622mcpsimp"></a>

**Table 1**  Parameter Description

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="13.94%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter Name</p></th>
<th class="cellrowborder" valign="top" width="12.98%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.08%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="13.94%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001441184464_p5938528141020"><a name="zh-cn_topic_0000001441184464_p5938528141020"></a><a name="zh-cn_topic_0000001441184464_p5938528141020"></a>index</p></td>
<td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.08%" headers="mcps1.2.4.1.3 "><p id="p14360822193719"><a name="p14360822193719"></a><a name="p14360822193719"></a>LocalTensor index, in elements.</p></td>
</tr>
<tr id="row184841037192110"><td class="cellrowborder" valign="top" width="13.94%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001441184464_p188644116443"><a name="zh-cn_topic_0000001441184464_p188644116443"></a><a name="zh-cn_topic_0000001441184464_p188644116443"></a>value</p></td>
<td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.4.1.2 "><p id="p18484133715217"><a name="p18484133715217"></a><a name="p18484133715217"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.08%" headers="mcps1.2.4.1.3 "><p id="p2411756104110"><a name="p2411756104110"></a><a name="p2411756104110"></a>Value to be set.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

Do not use **SetValue** extensively to assign values to **LocalTensor**, as this degrades performance. If large-scale assignment is required, select the [basic data padding API](../../memory_vector_compute/data_padding/data_padding.md) or the advanced data padding API ([Pad](../../../adv_api/tensor_transform/Pad.md), [Broadcast](../../../adv_api/tensor_transform/Broadcast.md)) based on the actual scenario, and select [Arange](../../../adv_api/index_compute/Arange.md) in scenarios where an increasing sequence needs to be generated.

## Example<a name="section17531157161314"></a>

```cpp
// srcLen = 256, num = 100, M=50
// Example 1.
for (int32_t i = 0; i < srcLen; ++i) {
    inputLocal.SetValue(i, num); // Assign num to the i-th position in inputLocal.
}
// The result of Example 1 is as follows:
// Data (inputLocal): [100 100 100  ... 100]


// srcLen = 256, num = 99, M=50
// Example 2.
for (int32_t i = 0; i < srcLen; ++i) {
    inputLocal(i) = num; // Assign num to the i-th position in inputLocal.
}
// The result of Example 2 is as follows:
// Data (inputLocal): [99 99 99  ... 99]
```
