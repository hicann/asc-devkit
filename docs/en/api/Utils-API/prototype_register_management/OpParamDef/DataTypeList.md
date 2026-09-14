# DataTypeList<a name="ZH-CN_TOPIC_0000002078492700"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-06T11:22:12.643Z pushedAt=2026-09-11T03:22:59.941Z -->

## Description<a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001525424352_section36583473819"></a>

Defines the data type of an operator parameter. If the data type supported by a certain input/output can be combined with the data types and data formats supported by all other inputs/outputs, you can use this API to define the data type.

When [DataType](DataType.md) is used to configure data types, the data types and formats of operator parameters must be configured through explicit combinations, and each combination contains the complete correspondence between input/output data types and data formats. The following example indicates that when the data types of inputs x and y are DT\_FLOAT16, the corresponding data type of output z is also DT\_FLOAT16, and the required supported data format is FORMAT\_ND.

```
class AddCustom : public OpDef {
public:
    AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
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

If the data type supported by a certain input/output can be combined with the data types and data formats supported by all other inputs/outputs, using the DataType API requires the following format, which indicates that when input x is DT\_FLOAT16, all data type and data format combinations of input y and input z are supported.

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

In this case, you can use DataTypeList to specify the data type without listing it repeatedly, for example:

```
class XxxCustom : public OpDef {
public:
    XxxCustom(const char* name) : OpDef(name)
    {
        this->Input("x")
            .ParamType(REQUIRED)
            .DataTypeList({ge::DT_FLOAT16})
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

## Prototype<a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &DataTypeList(std::vector<ge::DataType> types)
```

## Parameters<a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p318615392613"></a><strong id="zh-cn_topic_0000001991854801_b1871392631720"><a name="zh-cn_topic_0000001991854801_b1871392631720"></a><a name="zh-cn_topic_0000001991854801_b1871392631720"></a>types</strong></p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_p096733515614"><a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_p096733515614"></a><a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_p096733515614"></a>Operator parameter data type.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpParamDef** operator definition. For details about **OpParamDef**, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001525424352_section19165124931511"></a>

-   The same input/output cannot have both **DataType** and **DataTypeList** set at the same time.
-   This API cannot be used together with [UnknownShapeFormat](UnknownShapeFormat_deprecated.md).

## Example<a name="zh-cn_topic_0000001991854801_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_section320753512363"></a>

```
class AddCustom : public OpDef {
public:
    AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x")
            .ParamType(REQUIRED)
            .DataTypeList({ge::DT_FLOAT})
            .Format({ge::FORMAT_ND, ge::FORMAT_NCHW});
        this->Input("x1")
             ......
    }
};
```

