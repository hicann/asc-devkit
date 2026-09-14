# Comment<a name="ZH-CN_TOPIC_0000002078497982"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T11:16:38.209Z pushedAt=2026-09-11T03:19:46.134Z -->

## Description<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section36583473819"></a>

Sets the comment for the input/output parameters. It is used to synchronously generate the operator prototype comment when automatically generating the operator prototype header file.

Based on the **OpDef** operator prototype definition, a custom operator project can implement the following automation capabilities: automatically generate the operator prototype header file used in graph mode scenarios, so that developers can use the generated operator prototype to perform graph construction, graph compilation, graph execution, and other operations.

The generated comments help understand the operator prototype and can be used to automatically generate documentation for the operator prototype. Typically, built-in CANN operators use this feature more frequently. Developers can use it as needed.

## Prototype<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &Comment(const char *comment)
```

## Parameters<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"></a>comment</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_p096733515614"><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_p096733515614"></a><a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_p096733515614"></a>Comment content.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section25791320141317"></a>

Operator parameter definition. For details about **OpParamDef**, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000002055436684_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_section320753512363"></a>

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
            .Comment("Output cmt 1") // Comment content.
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

