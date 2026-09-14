# AddAttr<a name="ZH-CN_TOPIC_0000002078498890"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T15:41:47.563Z pushedAt=2026-09-11T06:23:09.376Z -->

## Description<a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_section36583473819"></a>

Sets the attributes of an operator and their corresponding values.

## Prototype<a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
ContextBuilder &AddAttr(const std::string& attrName, int64_t attrValue)
ContextBuilder &AddAttr(const std::string& attrName, bool attrValue)
ContextBuilder &AddAttr(const std::string& attrName, const std::string& attrValue)
ContextBuilder &AddAttr(const std::string& attrName, float attrValue)
ContextBuilder &AddAttr(const std::string& attrName, const std::vector<float>& attrValue)
ContextBuilder &AddAttr(const std::string& attrName, const std::vector<bool>& attrValue)
ContextBuilder &AddAttr(const std::string& attrName, const std::vector<int64_t>& attrValue)
ContextBuilder &AddAttr(const std::string& attrName, const std::vector<std::string>& attrValue)
ContextBuilder &AddAttr(const std::string& attrName, const std::vector<std::vector<int64_t>>& attrValue)
```

## Parameters<a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_section75395119104"></a>

<a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p10223674448"><a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p10223674448"></a><a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p645511218169"><a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p645511218169"></a><a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p1922337124411"><a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p1922337124411"></a><a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p8563195616313"><a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p8563195616313"></a><a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p8563195616313"></a>attrName</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p15663137127"><a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p15663137127"></a><a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p15663137127"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p2684123934216"><a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p2684123934216"></a><a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_p2684123934216"></a>Attribute name of the operator.</p></td>
</tr>
<tr id="zh-cn_topic_0000001820490268_row4927145645816"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001820490268_p5927145635816"><a name="zh-cn_topic_0000001820490268_p5927145635816"></a><a name="zh-cn_topic_0000001820490268_p5927145635816"></a>attrValue</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001820490268_p1492795645810"><a name="zh-cn_topic_0000001820490268_p1492795645810"></a><a name="zh-cn_topic_0000001820490268_p1492795645810"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p1490882610516"><a name="p1490882610516"></a><a name="p1490882610516"></a>Operator attribute value, supporting the specification of int64_t, bool, std::string, float, std::vector&lt;float&gt;, std::vector&lt;int64_t&gt;,</p>
<p id="zh-cn_topic_0000001820490268_p1292745695816"><a name="zh-cn_topic_0000001820490268_p1292745695816"></a><a name="zh-cn_topic_0000001820490268_p1292745695816"></a>std::vector&lt;std::string&gt;, std::vector&lt;bool&gt;, and std::vector&lt;std::vector&lt;int64_t&gt;&gt; types.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_section25791320141317"></a>

The current **ContextBuilder** object.

## Constraints<a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_section19165124931511"></a>

**AddAttr** does not support adding attributes with the same name repeatedly.

## Example<a name="zh-cn_topic_0000001820490268_zh-cn_topic_0000001389787297_section320753512363"></a>

```
context_ascendc::ContextBuilder builder;
auto builder
    .NodeIoNum(1,1)
    .IrInstanceNum({1})
    .AddAttr("attr_1", 1)
    .AddAttr("attr_2", true)
    .AddAttr("attr_3", "stringValue")
    .AddAttr("attr_4", 1.f)
    .AddAttr("attr_5", {1})
    .AddAttr("attr_6", {false})
    .AddAttr("attr_7", {"stringValue"})
    .AddAttr("attr_8", {1.f})
    .AddAttr("attr_9", {{1, 2}, {3, 4}})
```

