# Output<a name="ZH-CN_TOPIC_0000002114052061"></a>

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T10:45:32.384Z pushedAt=2026-11-13T08:46:49.317Z nl=2 -->

## Description<a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section36583473819"></a>

In some scenarios, the same operator supports different prototype outputs on different AI processor models.

This API allows you to register differentiated operator outputs for different AI processor models. After this API is called, it returns an OpParamDef structure, which you can then use to configure the operator output information.

## Prototype<a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section13230182415108"></a>

```
OpParamDef &Output(const char *name)
```

## Parameters<a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section75395119104"></a>

<a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p318615392613"></a>name</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p453018873120"><a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p453018873120"></a><a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p453018873120"></a>Operator output name.</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section25791320141317"></a>

Operator parameter definition. For details, see [OpParamDef](../OpParamDef/OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001760150049_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section320753512363"></a>

```
class AddCustom : public OpDef {
public:
    AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x").DataType({ ge::DT_FLOAT16 }).ParamType(OPTIONAL);
        this->Output("y").DataType({ ge::DT_FLOAT16 });
        OpAICoreConfig aicConfig1;
        OpAICoreConfig aicConfig2;
        aicConfig1.Output("y")
            .ParamType(OPTIONAL)
            .DataType({ ge::DT_FLOAT })
            .Format({ ge::FORMAT_ND });
        aicConfig2.Output("y")
            .ParamType(REQUIRED)
            .DataType({ ge::DT_INT32 })
            .Format({ ge::FORMAT_ND });
        this->AICore().AddConfig("ascendxxx1", aicConfig1);
        this->AICore().AddConfig("ascendxxx2", aicConfig2);
    }
};
```

