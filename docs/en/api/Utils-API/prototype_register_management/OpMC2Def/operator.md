# operator=<a name="ZH-CN_TOPIC_0000002078498010"></a>

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T11:04:41.928Z pushedAt=2026-11-13T08:46:49.416Z nl=2 -->

## Description<a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_section36583473819"></a>

Overloads the assignment operator to implement deep copy of OpMC2Def objects.

## Prototype<a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_section13230182415108"></a>

```
OpMC2Def &operator=(const OpMC2Def &def)
```

## Parameters<a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_section75395119104"></a>

<a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p10223674448"><a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p10223674448"></a><a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p645511218169"><a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p645511218169"></a><a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p1922337124411"><a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p1922337124411"></a><a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p2340183613156"><a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p2340183613156"></a><a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p2340183613156"></a>def</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p320343694214"><a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p320343694214"></a><a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p320343694214"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p2684123934216"><a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p2684123934216"></a><a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_p2684123934216"></a>The OpMC2Def object to be copied.</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_section25791320141317"></a>

Returns a reference to the copied result object.

## Constraints<a name="zh-cn_topic_0000001962610381_zh-cn_topic_0000001339105488_section19165124931511"></a>

None

