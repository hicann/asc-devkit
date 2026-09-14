# DynamicShapeSupportFlag<a name="ZH-CN_TOPIC_0000002114052065"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T09:58:55.103Z pushedAt=2026-09-11T01:52:00.502Z -->

## Description<a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section36583473819"></a>

Identifies whether the operator supports the dynamic shape scenario during graph compilation.

## Prototype<a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpAICoreConfig &DynamicShapeSupportFlag(bool flag)
```

## Parameters<a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"></a>flag</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><a name="zh-cn_topic_0000001626331617_ul38782045134920"></a><a name="zh-cn_topic_0000001626331617_ul38782045134920"></a><ul id="zh-cn_topic_0000001626331617_ul38782045134920"><li>true: The operator supports the dynamic shape scenario during graph compilation.</li><li>false: The operator does not support the dynamic shape scenario during graph compilation.</li></ul></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpAICoreConfig** class. See [OpAICoreConfig](OpAICoreConfig.md).

## Constraints<a name="zh-cn_topic_0000001626331617_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

