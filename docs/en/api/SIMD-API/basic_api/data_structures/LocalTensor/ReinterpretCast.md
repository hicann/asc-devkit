# ReinterpretCast<a name="ZH-CN_TOPIC_0000002330767554"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T13:10:13.012Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Reinterprets the current **Tensor** as a new type specified by the user. The converted **Tensor** has exactly the same address and content as the original **Tensor**, and the size (in bytes) of the **Tensor** remains unchanged.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename CAST_T> 
__aicore__ inline LocalTensor<CAST_T> ReinterpretCast() const
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001429830437_row118356578583"><th class="cellrowborder" valign="top" width="16.28%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000001429830437_p48354572582"><a name="zh-cn_topic_0000001429830437_p48354572582"></a><a name="zh-cn_topic_0000001429830437_p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="83.72%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000001429830437_p583535795817"><a name="zh-cn_topic_0000001429830437_p583535795817"></a><a name="zh-cn_topic_0000001429830437_p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001429830437_row1835857145817"><td class="cellrowborder" valign="top" width="16.28%" headers="mcps1.2.3.1.1 "><p id="p2177206172414"><a name="p2177206172414"></a><a name="p2177206172414"></a>CAST_T</p></td>
<td class="cellrowborder" valign="top" width="83.72%" headers="mcps1.2.3.1.2 "><p id="p106801314162417"><a name="p106801314162417"></a><a name="p106801314162417"></a>New type specified by the user.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Returns the **Tensor** after reinterpretation.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section17531157161314"></a>

```cpp
// Example.
// input_local is of the int32_t type and contains 16 elements (64 bytes).
for (int32_t i = 0; i < 16; ++i) {
    inputLocal.SetValue(i, i); // Assign the value i to the i-th position in inputLocal.
}


// Call ReinterpretCast to reinterpret input_local as the int16_t type.
AscendC::LocalTensor<int16_t> interpreTensor = inputLocal.ReinterpretCast<int16_t>();
// The example result is as follows. The data of the two tensors is completely identical, and they share the same physical memory address, with the data merely reinterpreted according to different types.
// inputLocal:0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
// interpreTensor:0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0 10 0 11 0 12 0 13 0 14 0 15 0
```

