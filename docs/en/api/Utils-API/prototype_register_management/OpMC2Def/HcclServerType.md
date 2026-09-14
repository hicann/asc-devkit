# HcclServerType<a name="ZH-CN_TOPIC_0000002464943441"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T11:08:47.634Z pushedAt=2026-09-11T03:16:42.577Z -->

## Description<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_section36583473819"></a>

Configures the server type of HCCL.

## Prototype<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_section13230182415108"></a>

```
void HcclServerType(enum HcclServerType type, const char *soc=nullptr)
```

## Parameters<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_section75395119104"></a>

<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p10223674448"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p10223674448"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p645511218169"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p645511218169"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p1922337124411"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p1922337124411"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2340183613156"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2340183613156"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p2340183613156"></a>type</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p320343694214"><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p320343694214"></a><a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p895664212548"><a name="p895664212548"></a><a name="p895664212548"></a>HCCL server type, of the HcclServerType enumeration class, defined as follows:</p>
<a name="screen112171935175919"></a><a name="screen112171935175919"></a><pre class="screen" codetype="Cpp" id="screen112171935175919">namespace ops{
enum HcclServerType : uint32_t {
    AICPU = 0,  // AI CPU server.
    AICORE, // AI Core server.
    CCU,    // CCU server, supported only when the <span id="ph18742853192816"><a name="ph18742853192816"></a><a name="ph18742853192816"></a>AI processor</span> contains a CCU unit.
    MAX     // Reserved parameter, not supported for use.
};
}</pre></td>
</tr>
<tr id="row122371621145118"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p8563195616313"><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p8563195616313"></a><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p8563195616313"></a>soc</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p15663137127"><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p15663137127"></a><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p15663137127"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p991524490"><a name="p991524490"></a><a name="p991524490"></a><span id="ph19321164217255"><a name="ph19321164217255"></a><a name="ph19321164217255"></a>AI processor</span> model. Configures the server type for this model.</p>
<p id="p366542912619"><a name="p366542912619"></a><a name="p366542912619"></a>Optional parameter. nullptr or "" configures the server type for all models supported by the operator.</p>
<p id="p94761732201220"><a name="p94761732201220"></a><a name="p94761732201220"></a>The value of soc must be within the range of <span id="ph3848185012164"><a name="ph3848185012164"></a><a name="ph3848185012164"></a>AI processor</span> models supported by the operator, that is, models already registered by calling the <a href="../OpAICoreDef/AddConfig.md">AddConfig</a> API.</p>
<p id="zh-cn_topic_0000001575929572_p14611945131415"><a name="zh-cn_topic_0000001575929572_p14611945131415"></a><a name="zh-cn_topic_0000001575929572_p14611945131415"></a>For the filling rules, see the <span>ASCEND_COMPUTE_UNIT field in the CMakePresets.json compilation configuration file in the operator project directory. The value of this field is automatically generated when the project is created using msOpGen</span>.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000001962490173_zh-cn_topic_0000001339105488_section19165124931511"></a>

-   Before using this API, the operator must be registered as a compute-communication fusion operator through the [MC2](../OpDef/MC2.md) API. After registration, an [OpMC2Def](OpMC2Def.md) structure is returned.
-   When the server type is configured for both a specific AI processor type and all AI processor types, the configuration for the specific AI processor type takes precedence.

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
        this->AICore().AddConfig("ascendxxx1");
        this->AICore().AddConfig("ascendxxx2");
        this->MC2().HcclGroup("group"); // Configure the communication domain name as group.
        this->MC2().HcclServerType(HcclServerType::AICPU, "ascendxxx1"); // Configure the communication mode of the ascendxxx1 model as AI CPU.
        this->MC2().HcclServerType(HcclServerType::AICORE); // Configure the communication mode of other models, that is, ascendxxx2, as AI Core.
    }
};
OP_ADD(MC2Custom);
```

