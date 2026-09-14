# To<a name="ZH-CN_TOPIC_0000002114097065"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T11:39:56.793Z pushedAt=2026-09-11T03:37:56.445Z -->

## Description<a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section36583473819"></a>

After this parameter is configured, when the single-operator API \(aclnnxxx\) is called, the data type of **aclScalar**/**aclScalarList** is converted to the data type specified by this parameter.

The specified data type can be passed in two ways:

-   Directly pass **datatype**.
-   Pass the name of an input, indicating that the data type remains consistent with the **datatype** of that input.

**This API is supported only in the following scenarios:**

-   Developing a single-operator invocation application by using the single-operator API.
-   Indirectly calling the single-operator API \(aclnnxxx\): the scenario of direct invocation of a single operator in the PyTorch framework.

## Prototype<a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &To(const ge::DataType type)
OpParamDef &To(const char *name)
```

## Parameters<a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p318615392613"></a><strong id="zh-cn_topic_0000001759578869_b124780476475"><a name="zh-cn_topic_0000001759578869_b124780476475"></a><a name="zh-cn_topic_0000001759578869_b124780476475"></a>type</strong></p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_p096733515614"><a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_p096733515614"></a><a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_p096733515614"></a>Specified data type.</p></td>
</tr>
<tr id="zh-cn_topic_0000001759578869_row19976124917474"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001759578869_p12976184904720"><a name="zh-cn_topic_0000001759578869_p12976184904720"></a><a name="zh-cn_topic_0000001759578869_p12976184904720"></a><strong id="zh-cn_topic_0000001759578869_b73001254114718"><a name="zh-cn_topic_0000001759578869_b73001254114718"></a><a name="zh-cn_topic_0000001759578869_b73001254114718"></a>name</strong></p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001759578869_p1497694924718"><a name="zh-cn_topic_0000001759578869_p1497694924718"></a><a name="zh-cn_topic_0000001759578869_p1497694924718"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001759578869_p179771249204713"><a name="zh-cn_topic_0000001759578869_p179771249204713"></a><a name="zh-cn_topic_0000001759578869_p179771249204713"></a>Name of the operator input, indicating that the specified data type is consistent with the data type of this input.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpParamDef** operator definition. For details about **OpParamDef**, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001759578869_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_section320753512363"></a>

```
this->Input("x")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT, ge::DT_FLOAT})
    .Format({ge::FORMAT_ND, ge::FORMAT_ND})
    .ScalarList()
    .To(ge::DT_FLOAT);
this->Input("x1")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT, ge::DT_FLOAT})
    .Format({ge::FORMAT_ND, ge::FORMAT_ND});
this->Input("x2")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT, ge::DT_FLOAT})
    .Format({ge::FORMAT_ND, ge::FORMAT_ND})
    .ScalarList()
    .To("x1");
```

