# FormatList<a name="ZH-CN_TOPIC_0000002114052033"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-06T11:28:49.346Z pushedAt=2026-09-11T03:26:50.165Z -->

## Description<a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section36583473819"></a>

Defines the data format of operator parameters. If the data format supported by an input/output can be used in combination with the data types and data formats supported by all other inputs/outputs, you can use this API to define the data format.

When [Format](Format.md) is used to configure data formats, these data formats correspond one-to-one to the data types and data formats of other inputs and outputs. In the following example, when the data format of inputs x and y is FORMAT\_NHWC, the data format of the corresponding output z is also FORMAT\_NHWC, and the data type of x, y, and z is required to be ge::DT\_FLOAT.

```
class AddCustom : public OpDef {
public:
    AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_NHWC, ge::FORMAT_ND});
        this->Input("y")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_NHWC, ge::FORMAT_ND});
        this->Output("z")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_NHWC, ge::FORMAT_ND});
        ...
    }
};
```

If the data format supported by an input/output can be used in combination with the data types and data formats supported by all other inputs/outputs, the Format API must be written in the following form, indicating that when input x is FORMAT\_ND, all combinations of data types and data formats of inputs y and z are supported.

```
class XxxCustom : public OpDef {
public:
    XxxCustom(const char* name) : OpDef(name)
    {
        this->Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
        this->Input("y")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
        this->Output("z")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
        ...
    }
};
```

In this case, you can use FormatList to specify the data type without listing it repeatedly, for example:

```
class XxxCustom : public OpDef {
public:
    XxxCustom(const char* name) : OpDef(name)
    {
        this->Input("x")
            .ParamType(REQUIRED)
            .DataTypeList({ge::DT_FLOAT16})
            .FormatList({ge::FORMAT_ND});
        this->Input("y")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
        this->Output("z")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
        ...
    }
};
```

## Prototype<a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &FormatList(std::vector<ge::Format> formats)
```

## Parameters<a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p318615392613"></a>formats</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_p43231148103313"><a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_p43231148103313"></a><a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_p43231148103313"></a>Data format of the operator parameter.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpParamDef** operator definition. For details about **OpParamDef**, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section19165124931511"></a>

-   **Format** and **FormatList** cannot be set at the same time for the same input/output.
-   This API cannot be used together with [UnknownShapeFormat](UnknownShapeFormat_deprecated.md).

## Example<a name="zh-cn_topic_0000001991734609_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_section320753512363"></a>

```
class AddCustom : public OpDef {
public:
    AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .FormatList({ge::FORMAT_ND});
        this->Input("x1")
        ......
    }
};
```

