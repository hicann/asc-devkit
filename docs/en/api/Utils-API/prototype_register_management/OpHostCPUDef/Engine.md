# Engine<a name="ZH-CN_TOPIC_0000002489092022"></a>

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T11:01:04.705Z pushedAt=2026-11-13T08:46:49.396Z nl=2 -->

## Description<a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001525424352_section36583473819"></a>

Configures the engine invoked by the operator. The input parameter is fixed to "DNN\_VM\_HOST\_CPU".

## Prototype<a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpHostCPUDef &Engine(const char *value)
```

## Parameters<a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p318615392613"></a>value</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_p12935163055011"><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_p12935163055011"></a><a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_p12935163055011"></a><span>Engine name for configuring the operator call</span>.</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpHostCPUDef operator definition. For details about OpHostCPUDef, see [OpHostCPUDef](OpHostCPUDef.md).

## Constraints<a name="zh-cn_topic_0000001549507408_zh-cn_topic_0000001576923281_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

