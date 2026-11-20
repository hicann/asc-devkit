# HcclGroup<a name="ZH-CN_TOPIC_0000002114052073"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-10T11:03:35.682Z pushedAt=2026-11-13T08:46:49.410Z nl=2 -->

## Description<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_section36583473819"></a>

Configures communication domain names, each corresponding to a communication domain. After configuration, call the GetHcclContext API on the kernel function (Kernel) side to obtain the context (message area) address corresponding to the communication domain.

## Prototype<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_section13230182415108"></a>

```
OpMC2Def &HcclGroup(const char *value)
OpMC2Def &HcclGroup(std::vector<const char *> value)
```

## Parameters<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_section75395119104"></a>

<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p10223674448"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p10223674448"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p645511218169"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p645511218169"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p1922337124411"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p1922337124411"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2340183613156"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2340183613156"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2340183613156"></a>value</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p320343694214"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p320343694214"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p320343694214"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2684123934216"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2684123934216"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2684123934216"></a>Name of the communication domain to be configured. Use const char * for a single communication domain and std::vector&lt;const char *&gt; for multiple communication domains.</p>
</td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_section19165124931511"></a>

Before using this API, the operator must be registered as a general-purpose compute fusion operator through the [MC2](../OpDef/MC2.md) API. After registration, an [OpMC2Def](OpMC2Def.md) structure is returned.

The communication domain name must first be configured as an attribute of the REQUIRED String type, and the attribute name is the communication domain name.

## Examples<a name="zh-cn_topic_0000001962490173_section163549032418"></a>

```
class MC2Custom : public OpDef {
public:
    MC2Custom(const char* name) : OpDef(name)
    {
        this->Input("x").ParamType(REQUIRED).DataType({ge::DT_FLOAT}).Format({ge::FORMAT_ND});
        this->Input("y").ParamType(REQUIRED).DataType({ge::DT_FLOAT}).Format({ge::FORMAT_ND});
        this->Output("z").ParamType(REQUIRED).DataType({ge::DT_FLOAT}).Format({ge::FORMAT_ND});
        this->Attr("group").AttrType(REQUIRED).String();
        this->AICore().AddConfig("ascendxxx");
        this->MC2().HcclGroup("group"); // Configure the communication domain name as group.
    }
};
OP_ADD(MC2Custom);
```

