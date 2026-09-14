# SetInferShape<a name="ZH-CN_TOPIC_0000002114052021"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T10:54:24.318Z pushedAt=2026-09-11T03:07:03.284Z -->

## Description<a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001525424352_section36583473819"></a>

When using graph mode, call this API to register a shape inference function.

## Prototype<a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpDef &SetInferShape(gert::OpImplRegisterV2::InferShapeKernelFunc func)
```

## Parameters<a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p318615392613"></a>func</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_p096733515614"><a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_p096733515614"></a><a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_p096733515614"></a>Shape inference function. The InferShapeKernelFunc type is defined as follows:</p>
<a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_screen17603182541315"></a><a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_screen17603182541315"></a><pre class="screen" codetype="Cpp" id="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_screen17603182541315">using InferShapeKernelFunc = UINT32 (*)(InferShapeContext *);</pre></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpDef** operator definition. For details about **OpDef**, see [OpDef](OpDef.md).

## Constraints<a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

