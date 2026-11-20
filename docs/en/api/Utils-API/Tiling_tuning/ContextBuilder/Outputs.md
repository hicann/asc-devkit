# Outputs<a name="ZH-CN_TOPIC_0000002114097957"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-13T09:58:49.564Z pushedAt=2026-11-16T07:07:41.675Z nl=2 -->

## Description<a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_section36583473819"></a>

Sets the vector of void\* pointers as the output of KernelContext.

## Prototype<a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
ContextBuilder &Outputs(std::vector<void *> outputs)
```

## Parameters<a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_section75395119104"></a>

<a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p10223674448"><a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p10223674448"></a><a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p645511218169"><a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p645511218169"></a><a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p1922337124411"><a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p1922337124411"></a><a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p8563195616313"><a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p8563195616313"></a><a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p8563195616313"></a>outputs</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p15663137127"><a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p15663137127"></a><a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p15663137127"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p2684123934216"><a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p2684123934216"></a><a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_p2684123934216"></a>void* pointer vector that stores the output</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_section25791320141317"></a>

The current ContextBuilder object.

## Constraints<a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001867289929_zh-cn_topic_0000001389787297_section320753512363"></a>

```
PlatformInfo platformInfo;
auto contextBuilder = context_ascendc::ContextBuilder().Outputs({nullptr, reinterpret_cast<void *>(&platformInfo)});
```

