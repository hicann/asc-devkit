# OpAICoreConfig Registration Interface (REGISTER\_OP\_AICORE\_CONFIG)<a name="ZH-CN_TOPIC_0000002385359568"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-06T09:53:44.442Z pushedAt=2026-09-11T04:46:59.246Z -->

## Description<a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001525424352_section36583473819"></a>

Different hardware forms may define operator prototypes differently. You can add an [OpAICoreConfig](OpAICoreConfig/OpAICoreConfig.md) to [register differentiated operator prototypes](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/advanced_programming/aclnn_operator_development/operator_prototype_definition.md) for different AI processor models. The REGISTER\_OP\_AICORE\_CONFIG macro allows you to add a separate file to register differentiated information of an operator on different hardware forms without modifying the original registration.

To use this registration macro, include the following header file:

```
#include "register/op_config_registry.h"
```

## Prototype<a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
REGISTER_OP_AICORE_CONFIG(opType, socVersion, opFunc)
```

## Parameters<a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p318615392613"></a>opType</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_p096733515614"><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_p096733515614"></a><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_p096733515614"></a>Operator type.</p></td>
</tr>
<tr id="row33032551664"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p830415551762"><a name="p830415551762"></a><a name="p830415551762"></a>socVersion</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="p103041655864"><a name="p103041655864"></a><a name="p103041655864"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p1030417553617"><a name="p1030417553617"></a><a name="p1030417553617"></a>Supported AI processor models.</p></td>
</tr>
<tr id="row418410573612"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p141847571966"><a name="p141847571966"></a><a name="p141847571966"></a>opFunc</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="p31849571962"><a name="p31849571962"></a><a name="p31849571962"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p61841157266"><a name="p61841157266"></a><a name="p61841157266"></a>Callback function pointer that returns OpAICoreConfig. The callback function prototype is defined as follows:</p>
<a name="screen35771245210"></a><a name="screen35771245210"></a><pre class="screen" codetype="Cpp" id="screen35771245210">OpAICoreConfig (*)()</pre></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001525424352_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001525424352_section19165124931511"></a>

If an operator uses [AddConfig](OpAICoreDef/AddConfig.md) to register both the AI processor models supported by the operator and the OpAICoreConfig information, and the AI processor models are the same, the configuration registered through AddConfig has a higher priority and overrides the OpAICoreConfig information registered by the **REGISTER_OP_AICORE_CONFIG** macro.

## Example<a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_section320753512363"></a>

Assume that the existing prototype registration file op\_host/add\_custom.cpp is implemented as follows, configuring the AI processor model ascendxxx1 supported by the operator and the operator input/output prototype information:

```
...
namespace ops {
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
        this->AICore()
            .SetTiling(optiling::TilingFunc);
        // Replace it with the actual AI processor model.
        this->AICore().AddConfig("ascendxxx1");
    }
};
OP_ADD(AddCustom);
} // namespace ops
```

You can add a file op\_host/add\_custom\_xxx.cpp and use REGISTER\_OP\_AICORE\_CONFIG to separately register the AI processor model ascendxxx2 supported by the operator, as shown in the following example:

```
#include "register/op_config_registry.h"
namespace ops {
REGISTER_OP_AICORE_CONFIG(AddCustom, ascendxxx2, []() {
    ops::OpAICoreConfig config("ascendxxx2");
    return config;
});
}
```
