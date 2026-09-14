# Input<a name="ZH-CN_TOPIC_0000002114097085"></a>

<!-- md-trans-meta sourceCommit=3ee506c06c64594337c3b6b8c8af85192bd95f75 translatedAt=2026-09-06T10:01:46.531Z pushedAt=2026-09-11T01:53:18.413Z -->

## Description<a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section36583473819"></a>

In some scenarios, the same operator supports different prototype inputs on different AI processor models.

Through this API, you can [register differentiated operator prototypes](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/programming_guide/advanced_programming/aclnn_operator_development/operator_prototype_definition.md) for different AI processor models. After this API is called, an **OpParamDef** structure is returned, through which you can subsequently configure the operator input information.

## Prototype<a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &Input(const char *name)
```

## Parameters<a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"></a>name</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_p096733515614"><a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_p096733515614"></a><a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_p096733515614"></a>Operator input name.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section25791320141317"></a>

Operator parameter definition. For details about OpParamDef, see [OpParamDef](../OpParamDef/OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001712470512_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_section320753512363"></a>

```
class AddCustom : public OpDef {
public:
    AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x").DataType({ ge::DT_FLOAT16 }).ParamType(OPTIONAL);
        this->Output("y").DataType({ ge::DT_FLOAT16 });
        OpAICoreConfig aicConfig1;
        OpAICoreConfig aicConfig2;
        aicConfig1.Input("x")
            .ParamType(OPTIONAL)
            .DataType({ ge::DT_FLOAT })
            .Format({ ge::FORMAT_ND });
        aicConfig2.Input("x")
            .ParamType(REQUIRED)
            .DataType({ ge::DT_INT32 })
            .Format({ ge::FORMAT_ND });
        this->AICore().AddConfig("ascendxxx1", aicConfig1);
        this->AICore().AddConfig("ascendxxx2", aicConfig2);
    }
};
```
