# NeedCheckSupportFlag<a name="ZH-CN_TOPIC_0000002078498002"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T10:03:10.784Z pushedAt=2026-09-11T01:54:28.497Z -->

## Description<a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section36583473819"></a>

Specifies whether to invoke the operator parameter validation function during the operator fusion phase to validate data type and shape.

-   If set to "true", the framework invokes the operator parameter validation function set through [SetCheckSupport](../OpAICoreDef/SetCheckSupport.md) to check whether the operator supports the specified inputs. In this case, you need to implement the callback function for operator parameter validation.
-   If set to "false", no validation is required.

## Prototype<a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpAICoreConfig &NeedCheckSupportFlag(bool flag)
```

## Parameters<a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"></a>flag</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001626211657_p1440515434301"><a name="zh-cn_topic_0000001626211657_p1440515434301"></a><a name="zh-cn_topic_0000001626211657_p1440515434301"></a>Whether to call the operator parameter validation function during operator fusion to validate data type and shape.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpAICoreConfig** class. See [OpAICoreConfig](OpAICoreConfig.md).

## Constraints<a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001626211657_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_section320753512363"></a>

See the example in [SetCheckSupport](../OpAICoreDef/SetCheckSupport.md).

