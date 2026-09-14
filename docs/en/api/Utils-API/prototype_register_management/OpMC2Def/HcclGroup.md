# HcclGroup<a name="ZH-CN_TOPIC_0000002114052073"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T11:07:34.564Z pushedAt=2026-09-11T03:16:11.241Z -->

## Description<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_section36583473819"></a>

Configures communication domain names, where each name corresponds to a communication domain. After configuration, the **GetHcclContext** API can be called on the Kernel side to obtain the context (message area) address of the communication domain.

## Prototype<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_section13230182415108"></a>

```
OpMC2Def &HcclGroup(const char *value)
OpMC2Def &HcclGroup(std::vector<const char *> value)
```

## Parameters<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_section75395119104"></a>

<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p10223674448"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p10223674448"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p645511218169"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p645511218169"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p1922337124411"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p1922337124411"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2340183613156"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2340183613156"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2340183613156"></a>value</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p320343694214"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p320343694214"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2684123934216"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2684123934216"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2684123934216"></a>Configured communication domain name. A single communication domain uses const char *, and multiple communication domains use std::vector&lt;const char *&gt;.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_section19165124931511"></a>

Before using this API, the operator must be registered as a communication-computation fusion operator through the [MC2](../OpDef/MC2.md) API, which returns an [OpMC2Def](OpMC2Def.md) structure upon registration.

The communication domain name must be configured as a REQUIRED String type attribute, and the attribute name is the communication domain name.

## Example<a name="zh-cn_topic_0000001962490173_section163549032418"></a>

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

