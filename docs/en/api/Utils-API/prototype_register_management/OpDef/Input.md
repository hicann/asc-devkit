# Input<a name="ZH-CN_TOPIC_0000002078497954"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T10:48:42.711Z pushedAt=2026-09-11T02:58:49.323Z -->

## Description<a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section36583473819"></a>

Registers the operator input. After this API is called, an **OpParamDef** structure is returned, through which the operator input information can be configured subsequently.

## Prototype<a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &Input(const char *name)
```

## Parameters<a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"></a>name</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_p096733515614"><a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_p096733515614"></a><a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_p096733515614"></a>Operator input name.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section25791320141317"></a>

Operator parameter definition and an OpParamDef instance. For details, see [OpParamDef](../OpParamDef/OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001549188224_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section19165124931511"></a>

The order of parameter registration must be consistent with that of the operator kernel entry function.

