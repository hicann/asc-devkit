# LaunchWithZeroEleOutputTensors<a name="ZH-CN_TOPIC_0000002520742223"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T10:12:19.633Z pushedAt=2026-09-11T02:41:18.773Z -->

## Description<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section36583473819"></a>

When the operator output is an all-empty tensor, the user can configure the operator to still execute on the NPU.

## Prototype<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpAICoreDef &OpAICoreDef::LaunchWithZeroEleOutputTensors(bool launchFlag)
```

## Parameters<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001575929572_row261104521415"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001575929572_p13611145151412"><a name="zh-cn_topic_0000001575929572_p13611145151412"></a><a name="zh-cn_topic_0000001575929572_p13611145151412"></a>launchFlag</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001575929572_p9611345141418"><a name="zh-cn_topic_0000001575929572_p9611345141418"></a><a name="zh-cn_topic_0000001575929572_p9611345141418"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001575929572_p14611945131415"><a name="zh-cn_topic_0000001575929572_p14611945131415"></a><a name="zh-cn_topic_0000001575929572_p14611945131415"></a>For a user-developed custom operator, when all outputs are empty tensors, set this parameter to true if the operator needs to be executed on the NPU; otherwise, the operator will not be executed.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpAICoreDef operator definition. For details about OpAICoreDef, see [OpAICoreDef](OpAICoreDef.md).

## Constraints<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_section320753512363"></a>

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
            .SetCheckSupport(optiling::CheckSupported)
            .LaunchWithZeroEleOutputTensors(true);

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

