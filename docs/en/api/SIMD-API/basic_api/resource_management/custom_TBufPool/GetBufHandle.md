# GetBufHandle<a name="ZH-CN_TOPIC_0000002051883238"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:18:03.005Z -->

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

Retrieves the memory block managed by the custom **TBufPool** based on the **offset**.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline TBufHandle GetBufHandle(uint8_t offset)
```

**Table 1**  Parameters

<a name="table1960411494236"></a>
<table><thead align="left"><tr id="row17605204922320"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p6605649122315"><a name="p6605649122315"></a><a name="p6605649122315"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.2.4.1.2"><p id="p17605949182313"><a name="p17605949182313"></a><a name="p17605949182313"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.2.4.1.3"><p id="p17605184915230"><a name="p17605184915230"></a><a name="p17605184915230"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row260544916231"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p104214518241"><a name="p104214518241"></a><a name="p104214518241"></a>offset</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p1842145172415"><a name="p1842145172415"></a><a name="p1842145172415"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p171754287124"><a name="p171754287124"></a><a name="p171754287124"></a>Index value of the memory block.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

The **offset** value must not exceed the **BUFID_SIZE** defined in **EXTERN_IMPL_BUFPOOL**. You can add validation as needed.

## Return Value<a name="section640mcpsimp"></a>

Returns the specified memory block, of the **TBufHandle** type.

## Example<a name="section1234017553610"></a>

See [Example](extern_impl_bufpool_macro.md#section1234017553610).

