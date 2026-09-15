# FreeTensor<a name="ZH-CN_TOPIC_0000002125876522"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:58:54.379Z -->

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

Releases the specified **Tensor** in TSCM.

## Function Prototype<a name="section620mcpsimp"></a>

```
template <typename T>
__aicore__ inline void FreeTensor(LocalTensor<T>& tensor)
```

## Parameters<a name="section622mcpsimp"></a>

<a name="table193329316393"></a>
<table><thead align="left"><tr id="row123331131153919"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.1.4.1.1"><p id="p8333133153913"><a name="p8333133153913"></a><a name="p8333133153913"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.1.4.1.2"><p id="p518118718459"><a name="p518118718459"></a><a name="p518118718459"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.1.4.1.3"><p id="p833353113393"><a name="p833353113393"></a><a name="p833353113393"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row11660173845017"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.1.4.1.1 "><p id="p466053810507"><a name="p466053810507"></a><a name="p466053810507"></a>tensor</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.1.4.1.2 "><p id="p885774605014"><a name="p885774605014"></a><a name="p885774605014"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.1.4.1.3 "><p id="p0660153818501"><a name="p0660153818501"></a><a name="p0660153818501"></a>Tensor to be released.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

See [Example](AllocTensor-60.md#section642mcpsimp).

