# IrInstanceNum<a name="ZH-CN_TOPIC_0000002114097961"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-13T09:57:53.005Z pushedAt=2026-11-16T07:07:41.668Z nl=2 -->

## Description<a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_section36583473819"></a>

Declares the actual number of each input during operator instantiation based on the operator's IR definition.

## Prototype<a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
ContextBuilder &IrInstanceNum(std::vector<uint32_t> instanceNum)
```

## Parameters<a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_section75395119104"></a>

<a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p10223674448"><a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p10223674448"></a><a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p645511218169"><a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p645511218169"></a><a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p1922337124411"><a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p1922337124411"></a><a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p8563195616313"><a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p8563195616313"></a><a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p8563195616313"></a>instanceNum</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p15663137127"><a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p15663137127"></a><a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p15663137127"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p2684123934216"><a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p2684123934216"></a><a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_p2684123934216"></a>Based on the operator IR prototype definition, declares the number of instances in the order of input indexes.</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_section25791320141317"></a>

The current ContextBuilder object.

## Constraints<a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_section19165124931511"></a>

Must be used together with NodeIoNum.

## Examples<a name="zh-cn_topic_0000001867289933_zh-cn_topic_0000001389787297_section320753512363"></a>

```
context_ascendc::ContextBuilder builder;
(void)builder.NodeIoNum(5,3)   // The operator has 5 types of input and 3 types of output.
             .IrInstanceNum({1, 1, 3, 1, 1});  // When the operator is instantiated, the dynamic-type input tensor with index 2 has 3 instances.
```

