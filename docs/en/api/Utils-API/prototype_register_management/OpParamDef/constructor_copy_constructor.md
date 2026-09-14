# Constructor & Copy Constructor<a name="ZH-CN_TOPIC_0000002491981930"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T11:14:20.594Z pushedAt=2026-09-11T07:29:52.379Z -->

## Description<a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section36583473819"></a>

Constructs **OpParamDef** for operator parameters. **OpParamDef** is used to describe the information of operator parameters, such as data type and format. Generally, users do not directly construct **OpParamDef**; instead, they indirectly construct **OpParamDef** for operator parameters through the **Input** and **Output** APIs.

## Prototype<a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
explicit OpParamDef(const char *name);
OpParamDef(const OpParamDef &def);
```

## Parameters<a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p318615392613"></a>name</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_p096733515614"><a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_p096733515614"></a><a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_p096733515614"></a>Operator parameter name. All parameter names of the same operator must be unique.</p></td>
</tr>
<tr id="row1672710644213"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p1472719624212"><a name="p1472719624212"></a><a name="p1472719624212"></a>def</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="p18708133714421"><a name="p18708133714421"></a><a name="p18708133714421"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p4727166184216"><a name="p4727166184216"></a><a name="p4727166184216"></a>The constructed operator parameter OpParamDef, used for copy-constructing OpParamDef.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001600307125_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

