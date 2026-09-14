# SetOpSelectFormat<a name="ZH-CN_TOPIC_0000002114052053"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T10:15:28.247Z pushedAt=2026-09-11T02:42:26.361Z -->

## Description<a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section36583473819"></a>

If you need to derive the data types and formats supported by the operator inputs and outputs on your own, you can implement a derivation callback function and register it through this API. In addition, set [DynamicFormatFlag](../OpAICoreConfig/DynamicFormatFlag.md) to **true**. Then, during operator fusion, the derivation function is automatically called to set the data types and formats, and there is no need to configure the supported data types and formats of the inputs and outputs when registering the operator prototype.

Note that if the data types and formats have already been registered for the operator prototype, the data types and formats registered for the operator prototype take precedence, and the derivation function is not executed even if it is registered.

## Prototype<a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpAICoreDef &SetOpSelectFormat(optiling::OP_CHECK_FUNC func)
```

## Parameters<a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"></a>func</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_p12935163055011"><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_p12935163055011"></a><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_p12935163055011"></a>Function that derives the supported data types and formats of the operator input and output. The OP_CHECK_FUNC type is defined as follows:</p>
<a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_screen746910291708"></a><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_screen746910291708"></a><pre class="screen" codetype="Cpp" id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_screen746910291708">using OP_CHECK_FUNC = ge::graphStatus (*)(const ge::Operator &op, ge::AscendString &result);</pre>
<p id="zh-cn_topic_0000001626409757_p1830771344310"><a name="zh-cn_topic_0000001626409757_p1830771344310"></a><a name="zh-cn_topic_0000001626409757_p1830771344310"></a>The input parameter of this function is the operator description, including the operator input, output, attributes, and other information. The output parameter is a string containing the list of data types and formats supported by the current operator input and output. A sample string format is as follows:</p>
<pre class="screen" id="zh-cn_topic_0000001626409757_screen11586142152320"><a name="zh-cn_topic_0000001626409757_screen11586142152320"></a><a name="zh-cn_topic_0000001626409757_screen11586142152320"></a>{
    "input0": {"name": "x","dtype": "float16,float32,int32","format": "ND,ND,ND"},
    "input1": {"name": "y","dtype": "float16,float32,int32","format": "ND,ND,ND"},
    "output0": {"name": "z","dtype": "float16,float32,int32","format": "ND,ND,ND"}
}</pre></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpAICoreDef** operator definition. For details about **OpAICoreDef**, see [OpAICoreDef](OpAICoreDef.md).

## Constraints<a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_section320753512363"></a>

The following shows an example of implementing and registering the derivation function for a custom Add operator.

```
static ge::graphStatus OpSelectFormat(const ge::Operator &op, ge::AscendString &result)
{
    std::string resultJsonStr;
    // If the first dimension of the shape of the first input parameter in this execution is less than or equal to 8, more formats are supported; otherwise, only int32 is supported.
    if (op.GetInputDesc(0).GetShape().GetDim(0) <= 8) {
        resultJsonStr = R"({
        "input0": {"name": "x","dtype": "float16,float32,int32","format": "ND,ND,ND","unknownshape_format": "ND,ND,ND"},
        "input1": {"name": "y","dtype": "float16,float32,int32","format": "ND,ND,ND","unknownshape_format": "ND,ND,ND"},
        "output0": {"name": "z","dtype": "float16,float32,int32","format": "ND,ND,ND","unknownshape_format": "ND,ND,ND"}
        })";
    } else {
        resultJsonStr = R"({
        "input0": {"name": "x","dtype": "int32","format": "ND","unknownshape_format": "ND"},
        "input1": {"name": "y","dtype": "int32","format": "ND","unknownshape_format": "ND"},
        "output0": {"name": "z","dtype": "int32","format": "ND","unknownshape_format": "ND"}
        })";
    }
    result = ge::AscendString(resultJsonStr.c_str());
    return ge::GRAPH_SUCCESS;
}
```

The following shows an example of registering the derivation function:

```
class AddCustom : public OpDef {
public:
    AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x")
            .ParamType(REQUIRED);
        this->Input("y")
            .ParamType(REQUIRED);
        this->Output("z")
            .ParamType(REQUIRED);
        this->SetInferShape(ge::InferShape);
        this->AICore()
            .SetTiling(optiling::TilingFunc)
            .SetTilingParse(optiling::TilingPrepare)
            .SetOpSelectFormat(optiling::OpSelectFormat);

        OpAICoreConfig aicConfig;
        aicConfig.DynamicCompileStaticFlag(true)
            .DynamicFormatFlag(true)
            .DynamicRankSupportFlag(true)
            .DynamicShapeSupportFlag(true)
            .NeedCheckSupportFlag(false)
            .PrecisionReduceFlag(true);
        // Note: Replace soc_version with the actual AI processor model.
        this->AICore().AddConfig("soc_version", aicConfig);
    }
};
```

