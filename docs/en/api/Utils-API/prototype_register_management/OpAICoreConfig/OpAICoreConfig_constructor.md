# OpAICoreConfig Constructor<a name="ZH-CN_TOPIC_0000002385193100"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-10T10:43:34.679Z pushedAt=2026-11-13T08:46:49.312Z nl=2 -->

## Description<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section36583473819"></a>

Constructor of OpAICoreConfig.

## Prototype<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
OpAICoreConfig()
OpAICoreConfig(const char *soc)
OpAICoreConfig(const OpAICoreConfig &aicore_config)
```

## Parameters<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section75395119104"></a>

<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_table111938719446"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| soc | Input | AI processor model. |
| aicore_config | Input | Constructed OpAICoreConfig used for copy-constructing OpAICoreConfig. |

## Return Value<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section19165124931511"></a>

The constructor that takes the soc parameter initializes some parameters in the OpAICoreConfig structure. The specific parameters and their initialization values are listed in the following table:

<a name="table56008147710"></a>
<table><thead align="left"><tr id="row3601014470"><th class="cellrowborder" valign="top" width="18.09%" id="mcps1.1.4.1.1"><p id="p19601111411713"><a name="p19601111411713"></a><a name="p19601111411713"></a>Configuration Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="60.589999999999996%" id="mcps1.1.4.1.2"><p id="p14573171417333"><a name="p14573171417333"></a><a name="p14573171417333"></a>Description</p>
</th>
<th class="cellrowborder" valign="top" width="21.32%" id="mcps1.1.4.1.3"><p id="p56011114871"><a name="p56011114871"></a><a name="p56011114871"></a>Initial Value</p>
</th>
</tr>
</thead>
<tbody><tr id="row460191414720"><td class="cellrowborder" valign="top" width="18.09%" headers="mcps1.1.4.1.1 "><p id="p136012142712"><a name="p136012142712"></a><a name="p136012142712"></a><a href="DynamicCompileStaticFlag.md">DynamicCompileStaticFlag</a></p>
</td>
<td class="cellrowborder" valign="top" width="60.589999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001575931912_p183604264191"><a name="zh-cn_topic_0000001575931912_p183604264191"></a><a name="zh-cn_topic_0000001575931912_p183604264191"></a>Whether the operator implementation supports static Shape compilation during graph entry.</p>
</td>
<td class="cellrowborder" valign="top" width="21.32%" headers="mcps1.1.4.1.3 "><p id="p15601114777"><a name="p15601114777"></a><a name="p15601114777"></a>true</p>
</td>
</tr>
<tr id="row2060161415717"><td class="cellrowborder" valign="top" width="18.09%" headers="mcps1.1.4.1.1 "><p id="p26019142078"><a name="p26019142078"></a><a name="p26019142078"></a><a href="DynamicFormatFlag.md">DynamicFormatFlag</a></p>
</td>
<td class="cellrowborder" valign="top" width="60.589999999999996%" headers="mcps1.1.4.1.2 "><p id="p145734149339"><a name="p145734149339"></a><a name="p145734149339"></a>Whether to automatically infer the supported dtype and format of the operator input and output based on the function set by <a href="../OpAICoreDef/SetOpSelectFormat.md">SetOpSelectFormat</a>.</p>
</td>
<td class="cellrowborder" valign="top" width="21.32%" headers="mcps1.1.4.1.3 "><p id="p56015149714"><a name="p56015149714"></a><a name="p56015149714"></a>true</p>
</td>
</tr>
<tr id="row9601014375"><td class="cellrowborder" valign="top" width="18.09%" headers="mcps1.1.4.1.1 "><p id="p116011714075"><a name="p116011714075"></a><a name="p116011714075"></a><a href="DynamicRankSupportFlag.md">DynamicRankSupportFlag</a></p>
</td>
<td class="cellrowborder" valign="top" width="60.589999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_p10448657201310"><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_p10448657201310"></a><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_p10448657201310"></a>Whether the operator supports dynamicRank (dynamic dimensions).</p>
</td>
<td class="cellrowborder" valign="top" width="21.32%" headers="mcps1.1.4.1.3 "><p id="p66017146713"><a name="p66017146713"></a><a name="p66017146713"></a>true</p>
</td>
</tr>
<tr id="row126011142077"><td class="cellrowborder" valign="top" width="18.09%" headers="mcps1.1.4.1.1 "><p id="p2601101420710"><a name="p2601101420710"></a><a name="p2601101420710"></a><a href="DynamicShapeSupportFlag.md">DynamicShapeSupportFlag</a></p>
</td>
<td class="cellrowborder" valign="top" width="60.589999999999996%" headers="mcps1.1.4.1.2 "><p id="p5890739574"><a name="p5890739574"></a><a name="p5890739574"></a>Whether the operator supports dynamic Shape scenarios when the graph is loaded.</p>
</td>
<td class="cellrowborder" valign="top" width="21.32%" headers="mcps1.1.4.1.3 "><p id="p1160131416714"><a name="p1160131416714"></a><a name="p1160131416714"></a>true</p>
</td>
</tr>
<tr id="row160116140712"><td class="cellrowborder" valign="top" width="18.09%" headers="mcps1.1.4.1.1 "><p id="p16601914377"><a name="p16601914377"></a><a name="p16601914377"></a><a href="NeedCheckSupportFlag.md">NeedCheckSupportFlag</a></p>
</td>
<td class="cellrowborder" valign="top" width="60.589999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001626211657_p1114624162912"><a name="zh-cn_topic_0000001626211657_p1114624162912"></a><a name="zh-cn_topic_0000001626211657_p1114624162912"></a>Whether to call the operator parameter validation function to validate data type and Shape during operator fusion.</p>
</td>
<td class="cellrowborder" valign="top" width="21.32%" headers="mcps1.1.4.1.3 "><p id="p1360141410720"><a name="p1360141410720"></a><a name="p1360141410720"></a>false</p>
</td>
</tr>
<tr id="row5601181415716"><td class="cellrowborder" valign="top" width="18.09%" headers="mcps1.1.4.1.1 "><p id="p13601181417712"><a name="p13601181417712"></a><a name="p13601181417712"></a><a href="PrecisionReduceFlag.md">PrecisionReduceFlag</a></p>
</td>
<td class="cellrowborder" valign="top" width="60.589999999999996%" headers="mcps1.1.4.1.2 "><p id="p957351413338"><a name="p957351413338"></a><a name="p957351413338"></a>This field controls the precision mode of operators during ATC model conversion or network debugging.</p>
</td>
<td class="cellrowborder" valign="top" width="21.32%" headers="mcps1.1.4.1.3 "><p id="p46013144712"><a name="p46013144712"></a><a name="p46013144712"></a>true</p>
</td>
</tr>
</tbody>
</table>

The default constructor without input parameters does not initialize the preceding parameters.

The copy constructor copies the configured input, output, and configuration item information from the copied object.

## Examples<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section320753512363"></a>

```
class AddCustom : public OpDef {
public:
    AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x").DataType({ ge::DT_FLOAT16 }).ParamType(OPTIONAL);
        this->Output("y").DataType({ ge::DT_FLOAT16 });
        // Use the constructor with the soc parameter.
        OpAICoreConfig aicConfig1("ascendxxx1");
        OpAICoreConfig aicConfig2("ascendxxx2");
        aicConfig1.Input("x")
            .ParamType(OPTIONAL)
            .DataType({ ge::DT_FLOAT })
            .Format({ ge::FORMAT_ND });
        aicConfig2.Input("x")
            .ParamType(REQUIRED)
            .DataType({ ge::DT_INT32 })
            .Format({ ge::FORMAT_ND });
        // Use the copy constructor.
        OpAICoreConfig aicConfig3(aicConfig1);
        aicConfig3.Input("x")
            .ParamType(REQUIRED)
            .DataType({ ge::DT_FLOAT16 })
            .Format({ ge::FORMAT_ND });
        this->AICore().AddConfig("ascendxxx1", aicConfig1);
        this->AICore().AddConfig("ascendxxx2", aicConfig2);
        this->AICore().AddConfig("ascendxxx3", aicConfig3);
    }
};
```
