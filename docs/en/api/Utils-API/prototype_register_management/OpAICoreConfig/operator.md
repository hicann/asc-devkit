# operator =<a name="ZH-CN_TOPIC_0000002114052069"></a>

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T10:44:50.080Z pushedAt=2026-11-13T08:46:49.316Z nl=2 -->

## Description<a name="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section36583473819"></a>

Overloaded assignment operator that implements a deep copy of an OpAICoreConfig object.

## Prototype<a name="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpAICoreConfig &operator=(const OpAICoreConfig &aicore_config)
```

## Parameters<a name="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001921572893_p156551819113"><a name="zh-cn_topic_0000001921572893_p156551819113"></a><a name="zh-cn_topic_0000001921572893_p156551819113"></a>aicore_config</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001921572893_p1464218131118"><a name="zh-cn_topic_0000001921572893_p1464218131118"></a><a name="zh-cn_topic_0000001921572893_p1464218131118"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001921572893_p362131813114"><a name="zh-cn_topic_0000001921572893_p362131813114"></a><a name="zh-cn_topic_0000001921572893_p362131813114"></a>OpAICoreConfig object to be copied.</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section25791320141317"></a>

Returns a reference to the copied result object.

## Constraints<a name="zh-cn_topic_0000001921572893_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

