# Output<a name="ZH-CN_TOPIC_0000002078492688"></a>

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-10T10:59:32.295Z pushedAt=2026-11-13T08:46:49.384Z nl=2 -->

## Description<a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section36583473819"></a>

Registers the operator output. After this API is called, an OpParamDef structure is returned, through which the operator output information can be configured subsequently.

## Prototype<a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section13230182415108"></a>

```
OpParamDef &Output(const char *name)
```

## Parameters<a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section75395119104"></a>

<a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p318615392613"></a>name</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p453018873120"><a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p453018873120"></a><a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p453018873120"></a>Operator output name.</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section25791320141317"></a>

Returns the operator parameter definition, an OpParamDef instance. For details, see [OpParamDef](../OpParamDef/OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001549347676_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section19165124931511"></a>

The order in which parameters are registered must be consistent with the operator kernel (Kernel) entry function.

