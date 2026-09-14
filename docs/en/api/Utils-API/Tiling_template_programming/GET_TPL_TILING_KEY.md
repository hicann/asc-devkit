# GET\_TPL\_TILING\_KEY<a name="ZH-CN_TOPIC_0000002103405630"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T16:29:42.969Z pushedAt=2026-09-11T06:58:32.788Z -->

## Description<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section36583473819"></a>

In Tiling template programming, developers call this API to automatically generate a **TilingKey**. The API converts the passed-in template parameters into binary according to the defined bit widths, combines them in order, and then converts the result into a **uint64** value, which is the **TilingKey**.

To use this API, include the header file that defines the template parameters and the template parameter combination. For details, see [Tiling Template Programming](tiling_template_programming.md).

## Prototype<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
namespace AscendC {
    uint64_t EncodeTilingKey(TilingDeclareParams declareParams,
                             TilingSelectParams selectParamsVec,
                             std::vector<uint64_t> tilingParams);
}

#define GET_TPL_TILING_KEY(...) \
    AscendC::EncodeTilingKey(g_tilingDeclareParams, g_tilingSelectParams, {__VA_ARGS__}) // GET_TPL_TILING_KEY generates the TilingKey by calling the EncodeTilingKey interface. EncodeTilingKey is an internal associated interface and does not require developer attention.
```

## Parameters<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section75395119104"></a>

<a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p10223674448"><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p10223674448"></a><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p645511218169"><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p645511218169"></a><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p1922337124411"><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p1922337124411"></a><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p1693913355118"><a name="p1693913355118"></a><a name="p1693913355118"></a>...</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p15663137127"><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p15663137127"></a><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p15663137127"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p010373710272"><a name="p010373710272"></a><a name="p010373710272"></a>Variable-length parameter, the specific values of the template parameters. When passed in, they must follow the same order as the template parameters in the header file that defines the template parameters and the template parameter combinations.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section25791320141317"></a>

**TilingKey** value.

## Constraints<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section320753512363"></a>

```
#include "tiling_key_add_custom.h"
static ge::graphStatus TilingFunc(gert::TilingContext *context)
{
    TilingDataTemplate tiling;
    uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();
    ge::DataType dtype_x = context->GetInputDesc(0)->GetDataType();
    ge::DataType dtype_y = context->GetInputDesc(1)->GetDataType();
    ge::DataType dtype_z = context->GetOutputDesc(0)->GetDataType();
    uint32_t D_T_X = static_cast<int>(dtype_x), D_T_Y = static_cast<int>(dtype_y), D_T_Z = static_cast<int>(dtype_z), TILE_NUM = 1, IS_SPLIT = 0;
    if (totalLength < MIN_LENGTH_FOR_SPLIT) {
        IS_SPLIT = 0;
        TILE_NUM = 1;
    } else {
        IS_SPLIT = 1;
        TILE_NUM = DEFAULT_TILE_NUM;
    }
    context->SetBlockDim(NUM_BLOCKS);
    tiling.set_totalLength(totalLength);
    tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
    context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
    const uint64_t tilingKey = GET_TPL_TILING_KEY(D_T_X, D_T_Y, D_T_Z, TILE_NUM, IS_SPLIT);  // Configure the tiling key with template parameters.
    context->SetTilingKey(tilingKey);
    size_t *currentWorkspace = context->GetWorkspaceSizes(1);
    currentWorkspace[0] = 0;
    return ge::GRAPH_SUCCESS;
}
```

