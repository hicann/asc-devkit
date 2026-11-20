# Follow<a name="ZH-CN_TOPIC_0000002114052045"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-10T11:09:10.641Z pushedAt=2026-11-13T08:46:49.435Z nl=2 -->

## Description<a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section36583473819"></a>

Specifies that the datatype/format/shape information of the current input/output is consistent with that of a previously defined input.

## Prototype<a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section13230182415108"></a>

-   Keep all datatype/format/shape information consistent.

    ```
    OpParamDef &Follow(const char *paramName)
    ```

-   Keep the specified datatype/format/shape information consistent.

    ```
    OpParamDef &Follow(const char *paramName, FollowType ftype)
    ```

## Parameters<a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"></a>paramName</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002052867297_p15488205233414"><a name="zh-cn_topic_0000002052867297_p15488205233414"></a><a name="zh-cn_topic_0000002052867297_p15488205233414"></a>Name of a previously defined input.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002052867297_row03533135010"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002052867297_p11333395017"><a name="zh-cn_topic_0000002052867297_p11333395017"></a><a name="zh-cn_topic_0000002052867297_p11333395017"></a>ftype</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002052867297_p231133115014"><a name="zh-cn_topic_0000002052867297_p231133115014"></a><a name="zh-cn_topic_0000002052867297_p231133115014"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002052867297_p1731033135017"><a name="zh-cn_topic_0000002052867297_p1731033135017"></a><a name="zh-cn_topic_0000002052867297_p1731033135017"></a>ftype is of the enumeration type FollowType, which indicates the Follow mode. The values are as follows:</p>
<a name="zh-cn_topic_0000002052867297_ul1782019075220"></a><a name="zh-cn_topic_0000002052867297_ul1782019075220"></a><ul id="zh-cn_topic_0000002052867297_ul1782019075220"><li>ALL: datatype/format/shape all keep consistent with paramName.</li><li>DTYPE: datatype keeps consistent with paramName.</li><li>FORMAT: format keeps consistent with paramName.</li><li>SHAPE: shape keeps consistent with paramName.</li></ul>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpParamDef operator definition. For details about OpParamDef, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section19165124931511"></a>

-   The data source of all Follow operations can only be Input.
-   For shape inference, only output parameters can Follow input parameters. Input parameters cannot Follow another input parameter.
-   Chained Follow is supported, for example, C Follows B and B Follows A. However, the Follow mode cannot be changed midway (ftype must remain consistent).
-   The Follow API is usually simpler than the InferShape function. If the logic can be expressed with Follow, use the Follow API so that you do not need to register an InferShape function.
-   The InferShape inference function and the Follow API for following shapes cannot be used together. That is, the case where some outputs use InferShape inference and some outputs use Follow inference is not supported. If you use both the InferShape function and the Follow API, the InferShape function takes precedence, and you must ensure that all output shapes can be inferred in the InferShape function.
-   For datatype/format, the parameter types that support Follow are [DataTypeList](DataTypeList.md)/[FormatList](FormatList.md). After Follow is called, the datatype/format of the current input/output is consistent with the datatype/format combined by paramName.

## Examples<a name="zh-cn_topic_0000002052867297_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_section320753512363"></a>

-   In the scenario where output "y1" follows input "x1", the datatype, format, and shape of "y1" are consistent with those of "x1".

    ```
    this->Input("x1")
        .ParamType(REQUIRED)
        .DataType({ge::DT_FLOAT, ge::DT_FLOAT})
        .Format({ge::FORMAT_ND, ge::FORMAT_ND});
    this->Input("x2")
        .ParamType(REQUIRED)
        .DataType({ge::DT_FLOAT, ge::DT_FLOAT})
        .Format({ge::FORMAT_ND, ge::FORMAT_ND});
    this->Output("y1")
        .ParamType(REQUIRED)
        .Follow("x1");
    ```

-   In the chained follow scenario, y1 -\> x2 -\> x1. In this case, the datatype, format, and shape of "y1" are consistent with those of "x1", and the datatype and format of "x2" are consistent with those of "x1".

    ```
    this->Input("x1")
        .ParamType(REQUIRED)
        .DataType({ge::DT_FLOAT, ge::DT_FLOAT})
        .Format({ge::FORMAT_ND, ge::FORMAT_ND});
    this->Input("x2")
        .ParamType(REQUIRED)
        .Follow("x1");
    this->Output("y1")
        .ParamType(REQUIRED)
        .Follow("x2");
    ```
