# DataTypeForBinQuery<a name="ZH-CN_TOPIC_0000002084778976"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T11:19:32.194Z pushedAt=2026-09-11T07:30:07.071Z -->

## Description<a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001525424352_section36583473819"></a>

Sets the data types used for runtime operator binary lookup of Input/Output. The number of data types is consistent with that in [DataType](DataType.md)/[DataTypeList](DataTypeList.md), and they correspond one-to-one.

During operator compilation, multiple .o files are generated based on data types, and these data types are used to index the operator binary at runtime. Some operators support multiple data types and are insensitive to data types. In this case, you can use this API to map multiple data types to the same operator binary, so that multiple data types can reuse one .o file, thereby reducing the number of generated binary files.

For example, if the input of an operator supports multiple data types (ge::DT\_INT16 and ge::DT\_INT32), and when using ge::DT\_INT16 input, the binary file of ge::DT\_INT32 can be reused without affecting the final result, the following configuration can be adopted:

```
this->Input("x")
    .ParamType(REQUIRED)
    .DataType({ge::DT_INT16, ge::DT_INT32})
    .DataTypeForBinQuery({ge::DT_INT32, ge::DT_INT32})
    .Format({ge::FORMAT_ND, ge::FORMAT_ND});
```

In this way, only one object file (.o) needs to be generated to support multiple data types.

## Prototype<a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &DataTypeForBinQuery(std::vector<ge::DataType> types)
```

## Parameters<a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p318615392613"></a>types</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_p096733515614"><a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_p096733515614"></a><a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_p096733515614"></a>Operator parameter data type.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpParamDef** operator definition. For details about **OpParamDef**, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001525424352_section19165124931511"></a>

-   The number of parameters in **DataTypeForBinQuery** must be consistent with the number of parameters in the **DataType** or **DataTypeList** of the current operator parameters.
-   It cannot be used together with the [To](To.md) (specified data type) API.
-   After using **DataTypeForBinQuery**, ensure that the new operator parameter attribute set generated (replacing the original **DataType** sequence with **DataTypeForBinQuery**) exists in the originally supported parameter attribute set.

    A parameter attribute set is defined as the set of attributes of all parameters supported by an operator, equivalent to a collection of parameters.

    For example, in [Example 1](#li26512368417), the operator supports four original collections with no duplication.

    1) **x** : DT\_FLOAT16, FORMAT\_ND    **y** : DT\_INT16, FORMAT\_ND

    2) **x** : DT\_FLOAT, FORMAT\_ND    **y** : DT\_INT16, FORMAT\_ND

    3) **x** : DT\_INT16, FORMAT\_ND    **y** : DT\_INT16, FORMAT\_ND

    4) **x** : DT\_INT32, FORMAT\_ND    **y** : DT\_INT16, FORMAT\_NC

    After replacing the original **DataType** sequence with **DataTypeForBinQuery**, the new collection is

    1) x : DT\_INT16, FORMAT\_ND    y : DT\_INT16, FORMAT\_ND

    2) x : DT\_FLOAT16, FORMAT\_ND    y : DT\_INT16, FORMAT\_ND

    3) x : DT\_FLOAT16, FORMAT\_ND    y : DT\_INT16, FORMAT\_ND

    4) x : DT\_INT16, FORMAT\_ND    y : DT\_INT16, FORMAT\_NC

    At this point, new collection 1 is identical to original collection 3, and new collections 2 and 3 are identical to original collection 1. The setting takes effect. New collection 4 does not belong to the original collection, so the setting does not take effect. In this case, compilation proceeds according to the original collection 4.

## Example<a name="zh-cn_topic_0000001600172389_zh-cn_topic_0000001526115138_zh-cn_topic_0000001575944081_section320753512363"></a>

-   <a name="li26512368417"></a>Example 1

    ```
            this->Input("x")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT16, ge::DT_INT32})
                .DataTypeForBinQuery({ge::DT_INT16, ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_INT16})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
            this->Output("y")
                .ParamType(REQUIRED)
                .DataType({ge::DT_INT16, ge::DT_INT16, ge::DT_INT16, ge::DT_INT16})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_NC});
    ```

    As shown in the following figure, before **DataTypeForBinQuery** is set, four binaries are generated. After **DataTypeForBinQuery** is set through the preceding code:

    -   After replacement, column 1 uses the binary of the original column 3, and columns 2 and 3 use the binary of the original column 1. Column 4 still uses the binary of column 4.
    -   After replacement, columns 2 and 3 are completely identical, achieving the effect of binary reuse. The total number of operator binaries is reduced from the original four (bin1, bin2, bin3, bin4) to the current three (bin1, bin3, bin4).

    ![](../../../figures/operator_development_end_to_end_process.png)

-   Example 2

    ```
    // For this simple use case, there are two pairs of reuse: columns 1 and 2 map to column 1, and columns 3 and 4 map to column 4. As a result, two binaries (1 and 4) are generated in total. All supported data types are passed into these two binaries for execution.
            this->Input("x")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT16, ge::DT_INT32})
                .DataTypeForBinQuery({ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_INT32, ge::DT_INT32})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
            this->Output("y")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT, ge::DT_FLOAT, ge::DT_FLOAT, ge::DT_FLOAT})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
    ```

-   Example 3

    ```
    // Complex use case. Multiple Inputs/Outputs can use DataTypeForBinQuery at the same time. In this case, two pairs of reuse are also produced: columns 1 and 2 -> column 2, and columns 3 and 4 -> column 1. A total of two binaries, 1 and 2, are generated. All supported data types are passed to these two binaries for execution.
            this->Input("x")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT16, ge::DT_INT32})
                .DataTypeForBinQuery({ge::DT_FLOAT, ge::DT_FLOAT, ge::DT_FLOAT16, ge::DT_FLOAT16})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
            this->Input("y")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32, ge::DT_INT16})
                .DataTypeForBinQuery({ge::DT_FLOAT, ge::DT_FLOAT, ge::DT_FLOAT16, ge::DT_FLOAT16})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
            this->Output("z")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32, ge::DT_INT16})
                .DataTypeForBinQuery({ge::DT_FLOAT, ge::DT_FLOAT, ge::DT_FLOAT16, ge::DT_FLOAT16})
                .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
    ```

