# SetTiling<a name="ZH-CN_TOPIC_0000002078497986"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T10:25:27.202Z pushedAt=2026-09-11T02:43:45.612Z -->

## Description<a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001525424352_section36583473819"></a>

Registers a Tiling function. The prototype of the Tiling function is fixed: it accepts a **TilingContext** as input, on which pointers to the input and output **Shape** and other content can be obtained. The registered Tiling function is invoked by the framework, which passes a **TilingContext** parameter when calling it.

## Prototype<a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpAICoreDef &SetTiling(gert::OpImplRegisterV2::TilingKernelFunc func)
```

## Parameters<a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p318615392613"></a>func</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_p12935163055011"><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_p12935163055011"></a><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_p12935163055011"></a>Tiling function. The TilingKernelFunc type is defined as follows:</p>
<a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_screen2936530125010"></a><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_screen2936530125010"></a><pre class="screen" codetype="Cpp" id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_screen2936530125010">using TilingKernelFunc = UINT32 (*)(TilingContext *);</pre></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpAICoreDef** operator definition. For details about **OpAICoreDef**, see [OpAICoreDef](OpAICoreDef.md).

## Constraints<a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

