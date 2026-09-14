# FormatForBinQuery<a name="ZH-CN_TOPIC_0000002084619088"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T11:26:55.629Z pushedAt=2026-09-11T03:26:20.232Z -->

## Description<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section36583473819"></a>

Sets the data format used for runtime operator binary lookup for Input/Output. The number of data formats is the same as that of [Format](Format.md)/[FormatList](FormatList.md), and they correspond one-to-one.

During operator compilation, multiple .o files are generated based on the data formats, and these data formats are used to index the operator binary at runtime. Some operators support multiple data formats and are insensitive to the data format. In this case, you can use this API to map multiple data formats to the same operator binary, so that multiple data formats can reuse one .o file, thereby reducing the number of generated binary files.

For example, if the input of an operator supports multiple data formats (ge::FORMAT\_NC and ge::FORMAT\_ND), and the binary file of ge::FORMAT\_ND can be reused when ge::FORMAT\_NC is used as the input without affecting the final result, the following configuration can be used:

```
this->Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_INT16, ge::DT_INT16})
            .Format({ge::FORMAT_NC, ge::FORMAT_ND})
            .FormatForBinQuery({ge::FORMAT_ND,ge::FORMAT_ND});
```

In this way, support for multiple data formats can be achieved by generating only one object file (.o).

## Prototype<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &FormatForBinQuery(std::vector<ge::Format> formats)
```

## Parameters<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p318615392613"></a>formats</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_p43231148103313"><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_p43231148103313"></a><a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_p43231148103313"></a>Data format of the operator parameter.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpParamDef** operator definition. For details about **OpParamDef**, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001525424352_section19165124931511"></a>

-   The number of parameters of **FormatForBinQuery** must be consistent with the number of parameters of **Format** or **FormatList** of the current operator.
-   It cannot be used together with [Scalar](Scalar.md)/[ScalarList](ScalarList.md).
-   It cannot be used together with [ValueDepend](ValueDepend.md).
-   After **FormatForBinQuery** is set, the data format of **FormatForBinQuery** replaces the data format of the current Input/Output, and the system checks whether the new combination exists before replacement. If it exists, the corresponding binary is used. If it does not exist, the parameter becomes invalid and the original data format is used for generation. For details, see [Example 1](#li26512368417).

## Example<a name="zh-cn_topic_0000001549188228_zh-cn_topic_0000001526275046_zh-cn_topic_0000001575944081_section320753512363"></a>

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

    -   After replacement, column 4 uses the binary of the original column 2, and columns 1 and 2 use the binary of the original column 1. Column 3 still uses the binary of column 3.
    -   After replacement, columns 1 and 2 are completely identical, achieving binary reuse. The total number of operator binaries is reduced from the original four (bin1, bin2, bin3, bin4) to the current three (bin1, bin2, bin3).

    ![](../../../figures/operator_development_end_to_end_process_67.png)

-   Example 2

    ```
            // Simple use case: two pairs are reused, columns 1 and 2 map to column 1, and columns 3 and 4 map to column 4. A total of two binaries, 1 and 4, are generated. All supported formats are passed to these two binaries for execution.
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
            // Complex use case: multiple inputs/outputs can use FormatBinQuery at the same time, which also produces two pairs of reuse. Columns 1 and 2 map to column 1, and columns 3 and 4 map to column 3. A total of two binaries, 1 and 3, are generated. All supported formats are passed to these two binaries for execution.
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

