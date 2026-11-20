# Comment<a name="ZH-CN_TOPIC_0000002078497982"></a>

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T11:06:41.627Z pushedAt=2026-11-13T08:46:49.425Z nl=2 -->

## Description<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section36583473819"></a>

Sets comments for input/output parameters. These comments are used to generate operator prototype comments when the operator prototype header file is automatically generated.

Based on the OpDef operator prototype definition, a custom operator project can provide the following automation capabilities: automatically generate the operator prototype header file used in graph mode scenarios, so that you can use the generated operator prototype to perform operations such as graph construction, graph compilation, and graph execution.

The generated comments help you understand the operator prototype, and can be used to automatically generate documentation for the operator prototype. In most cases, built-in CANN operators use these comments extensively. You can use them as needed.

## Prototype<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &Comment(const char *comment)
```

## Parameters<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"></a>comment</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_p096733515614"><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_p096733515614"></a><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_p096733515614"></a>Comment content.</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section25791320141317"></a>

Operator parameter definition. For OpParamDef, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_section320753512363"></a>

```
class AddCustom : public OpDef {
public:
    explicit AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT, ge::DT_INT32})
            .FormatList({ge::FORMAT_ND})
            .Comment("Input cmt 1"); // Comment content.
        this->Input("y")
            .ParamType(REQUIRED)
            .Comment("Input cmt 2") // Comment content.
            .DataType({ge::DT_FLOAT, ge::DT_INT32})
            .FormatList({ge::FORMAT_ND});

        this->Output("z")
            .Comment("Output cmt 1") // comment content.
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT, ge::DT_INT32})
            .FormatList({ge::FORMAT_ND});

        this->SetInferShape(ge::InferShape).SetInferDataType(ge::InferDataType);

        this->AICore()
            .SetTiling(optiling::TilingFunc);
        this->AICore().AddConfig("ascendxxx");

    }
};
```

