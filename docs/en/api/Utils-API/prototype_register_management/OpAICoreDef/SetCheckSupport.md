# SetCheckSupport<a name="ZH-CN_TOPIC_0000002114097077"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T10:13:57.699Z pushedAt=2026-09-11T02:42:07.130Z -->

## Description<a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section36583473819"></a>

If you need to perform operator parameter validation during the operator fusion phase, you can implement an operator parameter validation callback function and register it through this interface. In addition, set the [NeedCheckSupportFlag](../OpAICoreConfig/NeedCheckSupportFlag.md) parameter to **true**, so that the registered operator parameter validation function is invoked during the operator compilation and fusion phases to validate the relevant information.

If the operator parameter validation function passes, it indicates that the AI Core supports these operator parameters, and the corresponding operator on the AI Core is selected for execution. Otherwise, the AI CPU operator library is queried and then executed.

## Prototype<a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpAICoreDef &SetCheckSupport(optiling::OP_CHECK_FUNC func)
```

## Parameters<a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"></a>func</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_p12935163055011"><a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_p12935163055011"></a><a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_p12935163055011"></a>Parameter validation function. The OP_CHECK_FUNC type is defined as follows:</p>
<a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_screen746910291708"></a><a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_screen746910291708"></a><pre class="screen" codetype="Cpp" id="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_screen746910291708">using OP_CHECK_FUNC = ge::graphStatus (*)(const ge::Operator &op, ge::AscendString &result);</pre>
<p id="zh-cn_topic_0000001600467085_p154394427555"><a name="zh-cn_topic_0000001600467085_p154394427555"></a><a name="zh-cn_topic_0000001600467085_p154394427555"></a>The input parameter of this function is the description of the operator, including the operator's inputs, outputs, attributes, and other information. The output parameter is a string that contains the validation return code and reason, in the following format:</p>
<pre class="screen" id="zh-cn_topic_0000001600467085_screen54391842185513"><a name="zh-cn_topic_0000001600467085_screen54391842185513"></a><a name="zh-cn_topic_0000001600467085_screen54391842185513"></a>{"ret_code": "1","reason": "your reason"}</pre>
<p id="zh-cn_topic_0000001600467085_p74391442175513"><a name="zh-cn_topic_0000001600467085_p74391442175513"></a><a name="zh-cn_topic_0000001600467085_p74391442175513"></a>If the validation succeeds, the function returns ge::GRAPH_SUCCESS; if the validation fails, it returns ge::GRAPH_FAILED.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section25791320141317"></a>

For **OpAICoreDef**, see [OpAICoreDef](OpAICoreDef.md).

## Constraints<a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001600467085_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_section320753512363"></a>

The following shows an example of implementing and registering a parameter validation function for the custom Add operator.

-   The parameter validation function is implemented as follows: it validates the shape of the first input parameter, and only supports the case where the first dimension of the input x shape is 8; otherwise, it is not supported.

    ```
    static ge::graphStatus CheckSupported(const ge::Operator &op, ge::AscendString &result)
    {
        std::string resultJsonStr;
        // Only the case where the first dimension of the first input parameter shape is 8 is supported; other shapes are not supported.
        if (op.GetInputDesc(0).GetShape().GetDim(0) == 8) {
            resultJsonStr = R"({"ret_code": "1","reason": "x.dim[0] is 8"})";
            result = ge::AscendString(resultJsonStr.c_str());
            return ge::GRAPH_SUCCESS;
        }
        resultJsonStr = R"({"ret_code": "0","reason": "xxx"})";
        result = ge::AscendString(resultJsonStr.c_str());
        return ge::GRAPH_FAILED;
    }
    ```

-   An example of registering the parameter validation function is as follows:

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
                .SetOpSelectFormat(optiling::OpSelectFormat)
                .SetCheckSupport(optiling::CheckSupported);
    
            OpAICoreConfig aicConfig;
            aicConfig.DynamicCompileStaticFlag(true)
                .DynamicFormatFlag(true)
                .DynamicRankSupportFlag(true)
                .DynamicShapeSupportFlag(true)
                .NeedCheckSupportFlag(true)
                .PrecisionReduceFlag(true);
            // Note: Replace soc_version with the actual AI processor model.
            this->AICore().AddConfig("soc_version", aicConfig);
        }
    };
    ```

