# UnknownShapeFormat (Deprecated)<a name="ZH-CN_TOPIC_0000002078497970"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-06T11:41:33.939Z pushedAt=2026-09-11T04:05:01.495Z -->

## Description<a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section36583473819"></a>

> [!CAUTION]
>-   This API is deprecated and will be removed in a later version. Do not use it. There is no need to set the format separately for dynamic/static shapes. Use the [Format](Format.md) API to set it uniformly.
>-   If you use this API and enable the -Werror -Wall compilation options to treat all warnings as errors, a compilation error will occur. You can eliminate it by adding the -Wno-deprecated compilation option. However, there is a risk of compilation errors after the API is removed in a later version. It is recommended that you do not use this API and instead use the [Format](Format.md) API to set the format uniformly.
>    Add the compilation option to the cust\_optiling and cust\_opproto compilation targets in the op\_host/CMakeLists.txt file under the custom operator project directory, as shown in the following example:
>    ```
>    target_compile_options(cust_optiling PRIVATE
>            -Wno-deprecated
>    )
>    ```

Default value of Format when the shape is unknown.

## Prototype<a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &UnknownShapeFormat(std::vector<ge::Format> formats)
```

## Parameters<a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"></a>formats</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_p43231148103313"><a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_p43231148103313"></a><a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_p43231148103313"></a>Operator parameter data format.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpDef** operator definition. For details about **OpDef**, see [OpDef](../OpDef/OpDef.md).

## Constraints<a name="zh-cn_topic_0000001549347680_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

