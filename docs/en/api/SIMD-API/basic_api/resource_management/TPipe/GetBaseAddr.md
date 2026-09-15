# GetBaseAddr<a name="ZH-CN_TOPIC_0000001892282805"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:33:32.155Z -->

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
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Obtains the base address at the position specified by **logicPos** (logical abstract position). This API takes effect only in CPU debugging scenarios. It is typically used to calculate the offset address of a **Tensor** at **logicPos**, that is, the **Tensor** address minus the return value of **GetBaseAddr**.

## Prototype<a name="section620mcpsimp"></a>

```
inline uint8_t* GetBaseAddr(int8_t logicPos)
```

## Parameters<a name="section622mcpsimp"></a>

<a name="table193329316393"></a>
<table><thead align="left"><tr id="row123331131153919"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.1.4.1.1"><p id="p8333133153913"><a name="p8333133153913"></a><a name="p8333133153913"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.1.4.1.2"><p id="p518118718459"><a name="p518118718459"></a><a name="p518118718459"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.1.4.1.3"><p id="p833353113393"><a name="p833353113393"></a><a name="p833353113393"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row11660173845017"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.1.4.1.1 "><p id="p3887155315568"><a name="p3887155315568"></a><a name="p3887155315568"></a>logicPos</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.1.4.1.2 "><p id="p9331531865"><a name="p9331531865"></a><a name="p9331531865"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.1.4.1.3 "><p id="p143308311967"><a name="p143308311967"></a><a name="p143308311967"></a>Logical position type. For details about this type, see <a href="../../aux_data_structures/TPosition.md">TPosition</a>.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

NA

## Return Value<a name="section640mcpsimp"></a>

Returns the base address corresponding to the **logicPos**.

## Example<a name="section6191129670"></a>

```
auto absAddr = GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(pos));
```
