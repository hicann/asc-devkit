# TILING\_KEY\_IS<a name="ZH-CN_TOPIC_0000001586995454"></a>

<!-- md-trans-meta sourceCommit=22a41aa8a74d7a2c3f454ea4bd5f0f8fb27aee29 translatedAt=2026-11-19T03:11:20.838Z pushedAt=2026-11-19T05:58:49.127Z nl=2 -->

## Supported Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/tiling_key_management/TILING_KEY_IS_res.md#id1 -->


## Function Description<a name="zh-cn_topic_0000001610027821_section212607105720"></a>

The header file path is `"utils/base/sys_macros.h"`.

In a kernel function, determine whether the tiling\_key at the current execution equals a certain key set on the host side at runtime, thereby identifying tiling\_key==key a kernel function branch.

## Prototype<a name="zh-cn_topic_0000001610027821_section1630753514297"></a>

```
TILING_KEY_IS(key)
```

## Parameters<a name="zh-cn_topic_0000001610027821_section129451113125413"></a>

<a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p2340183613156"><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p2340183613156"></a><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p2340183613156"></a>key</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001610027821_p18857658124919"><a name="zh-cn_topic_0000001610027821_p18857658124919"></a><a name="zh-cn_topic_0000001610027821_p18857658124919"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001610027821_p53562135013"><a name="zh-cn_topic_0000001610027821_p53562135013"></a><a name="zh-cn_topic_0000001610027821_p53562135013"></a>key indicates a certain kernel function branch and must be a non-negative integer.</p>
</td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000001610027821_section65498832"></a>

-   TILING\_KEY\_IS applies to if and else if branches, and does not support the else branch. That is, to represent N branches using the TILING\_KEY\_IS function, N TILING\_KEY\_IS\(key\) must be used to represent them separately.
-   The kernel function direct invocation project is not supported yet.

## Example<a name="zh-cn_topic_0000001610027821_section97001499599"></a>

```
extern "C" __global__ __aicore__ void add_custom(__gm__ uint8_t *x, __gm__ uint8_t *y, __gm__ uint8_t *z, __gm__ uint8_t *workspace, __gm__ uint8_t *tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    if (workspace == nullptr) {
        return;
    }
    KernelAdd op;
    op.Init(x, y, z, tilingData.numBlocks, tilingData.totalLength, tilingData.tileNum);
    // When TilingKey is 1, execute Process1; when it is 2, execute Process2; when it is 3, execute Process3.
    if (TILING_KEY_IS(1)) {
        op.Process1();
    } else if (TILING_KEY_IS(2)) {
        op.Process2();
    } else if (TILING_KEY_IS(3)) {
        op.Process3();
    }
    // Other code logic.
    ...
    // In this example, when TilingKey is 3, ProcessOther is executed.
    if (TILING_KEY_IS(3)) {
        op.ProcessOther();
    }
}
```

The corresponding host-side tiling function example (pseudocode):

```
ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    // Other code logic.
    ...
    if (context->GetInputShape(0) > 10) {
        context->SetTilingKey(1);
    } else if (some condition) {
        context->SetTilingKey(2);
    } else if (some condition) {
        context->SetTilingKey(3);
    }
}
```

