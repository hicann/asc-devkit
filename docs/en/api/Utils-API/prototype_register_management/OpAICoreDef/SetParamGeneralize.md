# SetParamGeneralize<a name="ZH-CN_TOPIC_0000002114097081"></a>

<!-- md-trans-meta sourceCommit=2737c34bbaf046b0c4e915117e38247d0f621f63 translatedAt=2026-09-06T10:24:18.000Z pushedAt=2026-09-11T02:43:30.326Z -->

## Description<a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section36583473819"></a>

Registers an operator generalization function that contains the range of each parameter shape. In graph mode, shapes within the same range share the same binary.

## Prototype<a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpAICoreDef& SetParamGeneralize(optiling::PARAM_GENERALIZE_FUNC func)
```

## Parameters<a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"></a>func</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_p12935163055011"><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_p12935163055011"></a><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_p12935163055011"></a>Shape range of the operator parameters. The PARAM_GENERALIZE_FUNC type is defined as follows:</p>
<pre class="screen" id="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_screen746910291708"><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_screen746910291708"></a><a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_screen746910291708"></a>using PARAM_GENERALIZE_FUNC = ge::graphStatus (*)(const ge::Operator &<em id="zh-cn_topic_0000001575610072_i3386102911263"><a name="zh-cn_topic_0000001575610072_i3386102911263"></a><a name="zh-cn_topic_0000001575610072_i3386102911263"></a>op</em>, const ge::AscendString &<em id="zh-cn_topic_0000001575610072_i103865293260"><a name="zh-cn_topic_0000001575610072_i103865293260"></a><a name="zh-cn_topic_0000001575610072_i103865293260"></a>generalize_config</em>,
                                      ge::AscendString &<em id="zh-cn_topic_0000001575610072_i18386182915268"><a name="zh-cn_topic_0000001575610072_i18386182915268"></a><a name="zh-cn_topic_0000001575610072_i18386182915268"></a>generalized_op_params</em>);</pre></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpAICoreDef** operator definition. For details about OpAICoreDef, see [OpAICoreDef](OpAICoreDef.md).

## Constraints<a name="zh-cn_topic_0000001575610072_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

