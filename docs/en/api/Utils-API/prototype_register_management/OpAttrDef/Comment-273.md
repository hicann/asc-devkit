# Comment<a name="ZH-CN_TOPIC_0000002114097073"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T10:37:11.717Z pushedAt=2026-09-11T02:55:04.325Z -->

## Description<a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section36583473819"></a>

Sets the comment of an operator attribute. It is used to generate the operator prototype comment synchronously when the operator prototype header file is automatically generated.

Based on the **OpDef** operator prototype definition, a custom operator project can implement the following automation capabilities: automatically generate the operator prototype header file used in graph mode scenarios. Developers can use the generated operator prototype to perform operations such as graph construction, graph compilation, and graph execution.

The generated comments help understand the operator prototype and can be used to automatically generate documentation for the operator prototype. Typically, built-in CANN operators use this feature more frequently. Developers can use it as needed.

## Prototype<a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpAttrDef &Comment(const char *comment)
```

## Parameters<a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"></a>comment</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_p096733515614"><a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_p096733515614"></a><a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_p096733515614"></a>Comment content.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section25791320141317"></a>

Operator attribute definition. For details about **OpAttrDef**, see [OpAttrDef](OpAttrDef-272.md).

## Constraints<a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000002055278364_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_section320753512363"></a>

```
this->Attr("attrname").Comment("Attr cmt 1");
```

