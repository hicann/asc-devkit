# FormatForBinQuery<a name="ZH-CN_TOPIC_0000002084619088"></a>

<!-- md-trans-meta sourceCommit=c9ba5e84f8f5d1c96c38636648c995a8c2814fbc translatedAt=2026-11-10T11:11:00.072Z pushedAt=2026-11-13T08:46:49.436Z nl=2 -->

## Description<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section36583473819"></a>

Sets the data formats used for runtime operator binary lookup for Input/Output. The number of data formats is the same as that of [Format](Format.md)/[FormatList](FormatList.md), and they correspond one-to-one.

During operator compilation, multiple .o files are generated based on the data formats, and the operator binary is indexed at runtime by these data formats. Some operators support multiple data formats and are insensitive to the data format. In this case, this API can be used to map multiple data formats to the same operator binary, so that multiple data formats can reuse one .o file, thereby reducing the number of generated binary files.

For example, if the input of an operator supports multiple data formats (ge::FORMAT\_NC and ge::FORMAT\_ND), and the binary file of ge::FORMAT\_ND can be reused when ge::FORMAT\_NC is used as the input without affecting the final result, the following configuration can be adopted:

```
this->Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_INT16, ge::DT_INT16})
            .Format({ge::FORMAT_NC, ge::FORMAT_ND})
            .FormatForBinQuery({ge::FORMAT_ND,ge::FORMAT_ND});
```

In this way, only one object file (.o) needs to be generated to support multiple data formats.

## Prototype<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &FormatForBinQuery(std::vector<ge::Format> formats)
```

## Parameters<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p318615392613"></a>formats</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_p43231148103313"><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_p43231148103313"></a><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_p43231148103313"></a>Data format of the operator parameters.</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpParamDef operator definition. For details about OpParamDef, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section19165124931511"></a>

-   The number of parameters of FormatForBinQuery must be consistent with the number of parameters of Format or FormatList of the current operator parameter.
-   It cannot be used together with [Scalar](Scalar.md)/[ScalarList](ScalarList.md).
-   It cannot be used together with [ValueDepend](ValueDepend.md).
-   After FormatForBinQuery is set, the data format of the current Input/Output is replaced with the data format of FormatForBinQuery, and the system checks whether the new combination exists before the replacement. If it exists, the corresponding binary is used. If it does not exist, the parameter becomes invalid and the original data format is used for generation. For details, see [Example 1](#li26512368417).

## Examples<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_section320753512363"></a>

-   <a name="li26512368417"></a>Example 1

    ```
            this->Input("x")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT16})
                .Format({ge::FORMAT_NC, ge::FORMAT_NCHW, ge::FORMAT_NHWC, ge::FORMAT_ND})
                .FormatForBinQuery({ge::FORMAT_NC, ge::FORMAT_NC, ge::FORMAT_ND, ge::FORMAT_NCHW});
            this->Output("y")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT, ge::DT_FLOAT, ge::DT_FLOAT, ge::DT_FLOAT})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_NC, ge::FORMAT_ND});
    ```

    As shown in the following figure, before FormatForBinQuery is set, four binaries are generated. After FormatForBinQuery is set through the preceding code:

    -   After replacement, column 4 uses the binary originally used by column 2, and columns 1 and 2 use the binary originally used by column 1. Column 3 still uses the binary of column 3.
    -   After replacement, columns 1 and 2 are completely identical, achieving binary reuse. The total number of operator binaries is reduced from the original four (bin1, bin2, bin3, and bin4) to the current three (bin1, bin2, and bin3).

    ![](../../../figures/FormatForBinQuery.png)

-   Example 2

    ```
            // Simple use case: two pairs of reuse occur here, with columns 1 and 2 mapped to column 1, and columns 3 and 4 mapped to column 4. Two binaries, 1 and 4, are generated in total. All supported formats are passed to these two binaries for execution.
            this->Input("x")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT16})
                .Format({ge::FORMAT_NC, ge::FORMAT_NCHW, ge::FORMAT_NHWC, ge::FORMAT_ND})
                .FormatForBinQuery({ge::FORMAT_NC, ge::FORMAT_NC, ge::FORMAT_ND, ge::FORMAT_ND});
            this->Output("y")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT, ge::DT_FLOAT, ge::DT_FLOAT, ge::DT_FLOAT})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
    
    ```

-   Example 3

    ```
            // In a complex use case, multiple Input/Output pairs can use FormatBinQuery simultaneously, which also produces two pairs of reuse: columns 1 and 2 -> column 1, and columns 3 and 4 -> column 3. A total of two binaries, 1 and 3, are generated. All supported formats are passed to these two binaries for execution.
            this->Input("x")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT16})
                .Format({ge::FORMAT_NC, ge::FORMAT_NCHW, ge::FORMAT_NHWC, ge::FORMAT_ND})
                .FormatForBinQuery({ge::FORMAT_NC, ge::FORMAT_NC, ge::FORMAT_NHWC, ge::FORMAT_NHWC});
            this->Input("y")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT16})
                .Format({ge::FORMAT_NC, ge::FORMAT_NCHW, ge::FORMAT_NHWC, ge::FORMAT_ND})
                .FormatForBinQuery({ge::FORMAT_NC, ge::FORMAT_NC, ge::FORMAT_NHWC, ge::FORMAT_NHWC});
            this->Output("z")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT16})
                .Format({ge::FORMAT_NC, ge::FORMAT_NCHW, ge::FORMAT_NHWC, ge::FORMAT_ND})
                .FormatForBinQuery({ge::FORMAT_NC, ge::FORMAT_NC, ge::FORMAT_NHWC, ge::FORMAT_NHWC});
    ```

