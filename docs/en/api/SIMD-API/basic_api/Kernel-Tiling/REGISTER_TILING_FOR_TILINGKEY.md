# REGISTER\_TILING\_FOR\_TILINGKEY<a name="ZH-CN_TOPIC_0000002087702626"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:28:22.271Z pushedAt=2026-10-26T07:41:44.417Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/Kernel-Tiling/REGISTER_TILING_FOR_TILINGKEY_res.md#id1 -->


## Description<a name="zh-cn_topic_0000001526206862_section212607105720"></a>

Registers a custom **TilingData** structure that matches the **TilingKey** on the kernel side. This API requires a logical expression, which is provided as a string in which “TILING\_KEY\_VAR” represents the actual **TilingKey** and expresses the range that the **TilingKey** satisfies.

## Prototype<a name="zh-cn_topic_0000001526206862_section1630753514297"></a>

```
REGISTER_TILING_FOR_TILINGKEY(EXPRESSION, TILING_STRUCT)
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

<a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a>EXPRESSION</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="p137163271873"><a name="p137163271873"></a><a name="p137163271873"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p1190216251682"><a name="p1190216251682"></a><a name="p1190216251682"></a>EXPRESSION is a logical operation, where **TILING_KEY_VAR** refers to **TilingKey**.</p></td>
</tr>
<tr id="zh-cn_topic_0000001526206862_row1239183183016"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p17557191817713"><a name="p17557191817713"></a><a name="p17557191817713"></a>TILING_STRUCT</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001526206862_p7239938308"><a name="zh-cn_topic_0000001526206862_p7239938308"></a><a name="zh-cn_topic_0000001526206862_p7239938308"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001526206862_p72396320307"><a name="zh-cn_topic_0000001526206862_p72396320307"></a><a name="zh-cn_topic_0000001526206862_p72396320307"></a>Custom **TilingData** structure registered by the user that matches **TilingKey**.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

-   When using this API, ensure that the default user-defined TilingData structure has been registered using REGISTER\_TILING\_DEFAULT to inform the framework side that the user defines TilingData using standard C++ syntax.
-   EXPRESSION currently supports bitwise operations: &, |, \~, ^; shift operators: <<, \>\>; arithmetic operations: +, -, \*, /, %; conditional operators: ==, !=, \>, <, \>=, <=; logical AND &&, OR ||, and \(\). The precedence follows C++.
-   If the TilingData structure is inside a namespace, the corresponding namespace scope qualifier must be included during registration.
-   Mapping the same TilingKey to different TilingData structures is not supported and will trigger an interception error.
-   Kernel launch projects are not supported yet.

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
extern "C" __global__ __aicore__ void add_custom(__gm__ uint8_t *x, __gm__ uint8_t *y, __gm__ uint8_t *z, __gm__ uint8_t *tiling)
{
    REGISTER_TILING_DEFAULT(optiling::TilingData);  // Register the user-defined default TilingData structure.
    REGISTER_TILING_FOR_TILINGKEY("TILING_KEY_VAR == 1", optiling::TilingDataA); // Register the TilingData structure for TilingKey 1.
    REGISTER_TILING_FOR_TILINGKEY("(TILING_KEY_VAR >= 10) && (TILING_KEY_VAR <= 15)", optiling::TilingDataB); // Register the TilingData structure for TilingKey in [10, 15].
    REGISTER_TILING_FOR_TILINGKEY("TILING_KEY_VAR & 0xFF", optiling::TilingDataC); // Register the TilingData structure for TilingKey whose lower 8 bits are 1.
    if (TILING_KEY_IS(1)) {
        GET_TILING_DATA_WITH_STRUCT(optiling::TilingDataA, tilingData, tiling);
        ......
    } else if (TILING_KEY_IS(11)) {
        GET_TILING_DATA_WITH_STRUCT(optiling::TilingDataB, tilingData, tiling);
        ......
    } else if (TILING_KEY_IS(14)) {
        GET_TILING_DATA_WITH_STRUCT(optiling::TilingDataB, tilingData, tiling);
        ......
    } else if (TILING_KEY_IS(255)) {
        GET_TILING_DATA_WITH_STRUCT(optiling::TilingDataC, tilingData, tiling);
        ......
    } else {
        GET_TILING_DATA(tilingData, tiling);
        ......
    }
}
```

Use standard C++ syntax to register the tiling structure:

```
class TilingDataA{
public:
    ...
};
class TilingDataB{
public:
    ...
};
class TilingDataC{
public:
    ...
};
```

Example of the corresponding host-side tiling function:

```
ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    // Other code logic.
    ...
    if(condition1){
        context->SetTilingKey(1);
        optiling::TilingDataA *Addtiling = context->GetTilingData<optiling::TilingDataA>();
        ...
    } else if (condition2){
        context->SetTilingKey(11);
        optiling::TilingDataB *Addtiling = context->GetTilingData<optiling::TilingDataB >();
        ...
    } else if (condition3){
        context->SetTilingKey(14);
        optiling::TilingDataB *Addtiling = context->GetTilingData<optiling::TilingDataB >();
        ...
    } else if (condition4){
        context->SetTilingKey(255);
        optiling::TilingDataC *Addtiling = context->GetTilingData<optiling::TilingDataC >();
        ...
    }
    ...
    // Other code logic.
}
```
