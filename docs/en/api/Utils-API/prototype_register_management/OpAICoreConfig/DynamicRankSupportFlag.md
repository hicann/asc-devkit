# DynamicRankSupportFlag<a name="ZH-CN_TOPIC_0000002114097089"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-10T10:41:20.621Z pushedAt=2026-11-13T08:46:49.303Z nl=2 -->

## Description<a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section36583473819"></a>

Whether the operator supports dynamicRank (dynamic dimensions).

## Prototype<a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpAICoreConfig &DynamicRankSupportFlag(bool flag)
```

## Parameters<a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.329999999999998%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.45%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"></a>flag</p>
</td>
<td class="cellrowborder" valign="top" width="15.329999999999998%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.45%" headers="mcps1.1.4.1.3 "><a name="zh-cn_topic_0000001575612432_ul133221017184817"></a><a name="zh-cn_topic_0000001575612432_ul133221017184817"></a><ul id="zh-cn_topic_0000001575612432_ul133221017184817"><li>true: The operator supports dynamic rank, and its shape contains (-2), which is used to determine whether to perform dynamic compilation.</li><li>false: The operator does not support dynamic rank.</li></ul>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpAICoreConfig class. For details, see [OpAICoreConfig](OpAICoreConfig.md).

## Constraints<a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

